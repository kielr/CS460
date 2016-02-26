/********************************************************************
Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
show_pipe(PIPE *p)
{
   int i, j;
   printf("------------ PIPE CONTENETS ------------\n");     
   printf("nreader=%d  nwriter=%d  ", p->nreader, p->nwriter);
   printf("data=%d room=%d\n",p->data, p->room);
   prints("contents=");
   if (p->data){
      j = p->tail;
      for (i=0; i<p->data; i++){
          putc(p->buf[j++]);
          j %= PSIZE;
      }
   }
   printf("\n----------------------------------------\n");
}

char *MODE[ ]={"READ      ","WRITE     ","RDWR      ","APPEND    ",
	       "READ_PIPE ","WRITE_PIPE"};
int pfd()
{
 int i, count; 
 count = 0;

 for (i=0; i<NFD; i++){
     if (running->fd[i]) count++;
 }
 if (count==0){
    printf("no opened files\n");
    return(-1);
 }

 printf("=========== valid fd  ==========\n");
 for (i=0; i<NFD; i++){
   if (running->fd[i])
      printf("%d    %s  refCount = %d\n", 
	       i, MODE[running->fd[i]->mode], running->fd[i]->refCount);
 }
 printf("================================\n");
}

int read_pipe(int fd, char *buf, int n)
{
   int r = 0;
   char c; 
   OFT *op;  PIPE *p;

   if (n<=0) return 0;

   if (fd < 0 || fd > NFD || running->fd[fd] == 0){
      printf("bad fd %d\n", fd);
      return -1;
   }
   op = running->fd[fd];

   if (op->mode != READ_PIPE){
      printf("fd = %d is NOT for read\n", fd);
      return -1;
   }
   p  = op->pipe_ptr;
   printf("pipe before reading\n");
   show_pipe(p);
 
   while(n){
      while(p->data){
         c = p->buf[p->tail++];
         put_byte(c, running->uss, buf);
         p->tail %= PSIZE;
         p->data--; p->room++;
         n--; r++; buf++;
         if (n == 0) break;
      }
      if (n==0 || r){
 	 kwakeup(&p->room);
         printf("pipe after reading\n");
         show_pipe(p);
         return r;
      }
      // pipe has no data
      if (p->nwriter){                 // if pipe still has writer
	 printf("pipe before reader goes to sleep\n");
         show_pipe(p);

         kwakeup(&p->room);             // wakeup ALL writers, if any.
         ksleep(&p->data);              // sleep for data
         continue;
       }
       // pipe has no writer and no data
       return 0;
   }
}

int write_pipe(int fd, char *buf, int n)
{
  char c;   int r=0;
  PIPE *p;  OFT *op;

  if (fd < 0 || fd > NFD || running->fd[fd] == 0){
     printf("bad fd %d\n", fd);
     return -1;
  }
  op = running->fd[fd];
  if (op->mode != WRITE_PIPE){
     printf("fd = %d is NOT for write\n", fd);
     return -1;
  }
  p = op->pipe_ptr;
  printf("pipe before writing\n");
  show_pipe(p);

  while(n){
    if (!p->nreader){                // no more readers
       printf("proc %d : BROKEN_PIPE error\n", running->pid);
       kexit(0x0D);              // simulate SIGPIPE=13
     }
     while(p->room && n){
        p->buf[p->head++] = get_byte(running->uss, buf); 
        p->head %= PSIZE;
        p->data++; p->room--; 
        n--; r++; buf++;
     }
     kwakeup(&p->data);            // wakeup ALL readers, if any.
     if (n==0){
        printf("pipe after writing\n");
        show_pipe(p);

        return r;          // finished writing n bytes 
     }
     // still has data to write but pipe has no room
     printf("pipe before writer goes to sleep\n");
     show_pipe(p);

     ksleep(&p->room);             // sleep for room   
  }
  return r;
}

int kpipe(int *y, int *z)
{
  PIPE *p; OFT *p0, *p1; int i;
  
  for (i=0; i<NPIPE; i++){
      if (pipe[i].busy == 0)
          break;
  }
  pipe[i].busy = 1;
  p = &pipe[i];
  p->head = p->tail = p->data = 0;
  p->room = PSIZE;

  /* allocate 2 ofte's */

  for (i=0; i<NOFT; i++){
      if (oft[i].refCount == 0) break;
  }
  p0 = &oft[i];  p0->refCount = 1;

  for (i=0; i<NOFT; i++){
      if (oft[i].refCount == 0) break;
  }
  p1 = &oft[i];  p1->refCount = 1;

  p0->mode = READ_PIPE;
  p1->mode = WRITE_PIPE;
  p0->refCount = p1->refCount = 1;
  p0->pipe_ptr = p1->pipe_ptr = p;

  p->nreader = 1;  p->nwriter = 1;

  /* allocate a pair of even-odd fd's */
  for (i=0; i<NFD; i += 2){ 
      if (running->fd[i] == 0 && running->fd[i+1] == 0){
          running->fd[i]   = p0; 
          running->fd[i+1] = p1;
          break;
      }
  }
  /* fill user pipe[] array with i, i+1 */
  put_word(i, running->uss, y); put_word(i+1, running->uss, y+1);
  printf("do_pipe : file descriptors = [%d %d]\n", i, i+1); 
  return 0;
}

int close_pipe(int fd)
{
  OFT *op; PIPE *pp;
  printf("proc %d close_pipe: fd=%d\n", running->pid, fd);

  op = running->fd[fd];
  running->fd[fd] = 0;                 // clear fd[fd] entry 

  if (op->mode == READ_PIPE){
      pp = op->pipe_ptr;
      pp->nreader--;                   // dec n reader by 1

      if (--op->refCount == 0){        // last reader
	if (pp->nwriter <= 0){         // no more writers
	     pp->busy = 0;             // free the pipe   
             return;
        }
      }
      kwakeup(&pp->room); 
      return;
  }

  if (op->mode == WRITE_PIPE){
      pp = op->pipe_ptr;
      pp->nwriter--;                   // dec nwriter by 1

      if (--op->refCount == 0){        // last writer 
	if (pp->nreader <= 0){         // no more readers 
	    pp->busy = 0;              // free pipe also 
            return;
        }
      }
      kwakeup(&pp->data);
      return;
  }
}
