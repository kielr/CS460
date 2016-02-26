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

int do_tswitch()
{
  printf("proc %d tswitch()\n", running->pid);
  tswitch();
  printf("proc %d resumes\n", running->pid);
}

int do_kfork()
{
  PROC *p;
  printf("proc%d kfork a child\n");
  p = kfork("/u1");
  if (p == 0)
    printf("kfork failed\n");
  else
    printf("child pid = %d\n", p->pid);
}

int do_exit(int exitValue)
{
  //int exitValue;
  if (running->pid == 1 && nproc > 2){
      printf("other procs still exist, P1 can't die yet !%c\n",007);
      return -1;
  }
  /***************************************
  printf("enter an exitValue (0-9) : ");
  exitValue = (getc()&0x7F) - '0'; 
  printf("%d\n", exitValue);
  **************************************/
  kexit(exitValue);
}

int do_wait(int *ustatus)
{
  int child, status;
  child = kwait(&status);
  if (child<0){
    printf("proc %d wait error : no child\n", running->pid);
    return -1;
  }
  printf("proc %d found a ZOMBIE child %d exitValue=%d\n", 
	   running->pid, child, status);
  // write status to Umode *ustatus
  put_word(status, running->uss, ustatus);
  return child;
}

int body()
{
  char c;
  printf("proc %d resumes to body()\n", running->pid);

  while(1){
    printf("-----------------------------------------\n");
    printList("freelist  ", freeList);
    printList("readyQueue", readyQueue);
    printList("sleepList ", sleepList);
    printf("-----------------------------------------\n");

    printf("proc %d running: parent = %d  enter a char [s|f|w|q|u] : ", 
	   running->pid, running->parent->pid);
    c = getc(); printf("%c\n", c);
    switch(c){
       case 's' : do_tswitch();   break;
       case 'f' : do_kfork();     break;
       case 'w' : do_wait();      break;
       case 'q' : do_exit();      break;
       case 'u' : goUmode();      break;
    }
  }
}

int color;
extern int loader();

extern PROC proc[];
int kmode()
{
  body();
}

char *hh[ ] = {"FREE   ", "READY  ", "RUNNING", "STOPPED", "SLEEP  ", 
               "ZOMBIE ",  0}; 
int do_ps()
{
   int i,j; 
   char *p, *q, buf[16];
   buf[15] = 0;

   printf("============================================\n");
   printf("  name         status      pid       ppid  \n");
   printf("--------------------------------------------\n");

   for (i=0; i<NPROC; i++){
       strcpy(buf,"               ");
       p = proc[i].name;
       j = 0;
       while (*p){
             buf[j] = *p; j++; p++;
       }      
       prints(buf);    prints(" ");
       
       if (proc[i].status != FREE){
           if (running==&proc[i])
              prints("running");
           else
              prints(hh[proc[i].status]);
           prints("     ");
           printd(proc[i].pid);  prints("        ");
           printd(proc[i].ppid);
       }
       else{
              prints("FREE");
       }
       printf("\n");
   }
   printf("---------------------------------------------\n");

   return(0);
}

#define NAMELEN 32
int chname(char *y)
{
  char buf[64];
  char *cp = buf;
  int count = 0; 

  while (count < NAMELEN){
     *cp = get_byte(running->uss, y);
     if (*cp == 0) break;
     cp++; y++; count++;
  }
  buf[31] = 0;

  printf("changing name of proc %d to %s\n", running->pid, buf);
  strcpy(running->name, buf); 
  printf("done\n");
}
