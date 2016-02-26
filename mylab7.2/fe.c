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
int copyImage(u16 s1, u16 s2, u16 size)
{
   int i; u16 w;
   for (i=0; i<size; i++){
       w = get_word(s1, 2*i);
       put_word(w, s2, 2*i);
   }
}

int goUmode();

int fork()
{
  PROC *p;  int pid;  u16 segment;

  p = kfork(0);   // kfork() but do NOT load any Umode image for child 
  if (p == 0){     // kfork failed 
    return -1;
  }
  pid = p->pid;
  segment = (pid+1)*0x1000;
  printf("pid=%d proc.pid=%d segment=%x\n", pid, p->pid, segment);

  copyImage(running->uss, segment, 32*1024);

  /********************************************************
    copyImage copies parent's U image to child, including
    ustack contents.  However, the child's segment must be
    changed to its own, and its return pid in saved ax must
    be changed to 0
  *********************************************************/

   /*** first, child's uss and usp must be set: *****/
   p->uss = segment;
   p->usp = running->usp;   /* same as parent's usp */

   /*** next, change uDS, uES, uCS in child's ustack ****/
   put_word(segment, segment, p->usp);        /* uDS    */
   put_word(segment, segment, p->usp+2);      /* uES    */
   put_word(0, segment, p->usp+2*8);          /* ax = 0 */
   put_word(segment, segment, p->usp+2*10);   /* uCS    */
   nproc++;

   printList("readyQueue ", readyQueue);
   printf("Proc %d forked a child %d at segment=%x\n",
           running->pid, p->pid, segment);
   return p->pid;
}

extern int loader();

int exec(char *y)
{
  int i, len;
   u16 segment, offset, HIGH;
   char line[64], *cp, c;

   char filename[32], f2[16],*cq;

   segment = running->uss; // if vfork()ed, we are using parent's segment
  /* get command line line from U space */
   cp = line; 
   while( (*cp=get_byte(segment, y)) != 0 ){
          y++; cp++;
   }

   // now we are using our own segment
   segment = (running->pid+1)*0x1000;
   //   printf("exec : line=%s\n", line);
   /* extract filename to exec */
   cp = line;        cq = f2;

   while(*cp == ' ')  /* SAFETY: skip over leading blanks */
     cp++;

   while (*cp != ' ' && *cp != 0){
         *cq = *cp;
          cq++; cp++;
   }
   *cq = 0;

   if (f2[0]==0){
       return -1;     /* should NOT happen but just in case */
   }

   //printf("exec : cmd=%s   len=%d\n", f2, strlen(f2));
   strcpy(filename, "/bin/");   /* all executables are in /bin */
   strcat(filename, f2);
   //   strcpy(running->name, f2);   /* program name */
  
   printf("Proc %d exec to %s in segment %x\n",
           running->pid, filename, segment);

   //segment = (running->pid+1)*0x1000;
 
   if (!load(filename, segment))
     return -1;

   len = strlen(line) + 1;
   if (len % 2)   // odd ==> must pad a byte
     len ++;

   offset = -len;
  // put_uword()/put_ubyte() are relative to running->uss
   for (i=0; i<len; i++){
       put_byte(line[i], segment, offset+i);
   }
 
   HIGH = offset - 2;  /* followed by INT stack frame */

   /* *s pointing at the command line string */
   put_word(offset, segment, HIGH);
  
   /* zero out U mode registers in ustack di to ax */
   for (i=1; i<=12; i++){
     put_word(0, segment, HIGH-2*i);
   }

   // must set PROC.uss to our own segment
   running->uss = segment;
   running->usp = HIGH-2*12;
/* re-initialize usp to new ustack top */
   /*   0    1   2   3  4  5  6  7  8  9  10 11 12
   /*      flag uCS uPC ax bx cx dx bp si di es ds */
   /* HIGH                                      sp */

   put_word(running->uss, segment, HIGH-2*12);   // uDS=uSS
   put_word(running->uss, segment, HIGH-2*11);   // uES=uSS
   put_word(0,            segment, HIGH-2*3);    // uPC=0
   put_word(segment,      segment, HIGH-2*2);    // uCS=segment
   put_word(0x0200,       segment, HIGH-2*1);        // flag
}

