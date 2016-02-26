int makeUimage(char *filename, PROC *p)
{
  u16 i, segment;

  // make Umode image by loading /u1 into segment
  segment = (p->pid + 1)*0x1000;
  load(filename, segment);

  /***** Fill in U mode information in proc *****/
  /**************************************************
    We know segment=0x2000 + index*0x1000 ====>
    ustack is at the high end of this segment, say TOP.
    We must make ustak contain:
          1   2   3  4  5  6  7  8  9 10 11 12
       flag uCS uPC ax bx cx dx bp si di es ds
     0x0200 seg  0  0  0  0  0  0  0  0 seg seg

    So, first a loop to set all to 0, then
    put_word(seg, segment, -2*i); i=2,11,12;
   **************************************************/

   for (i=1; i<=12; i++){
       put_word(0, segment, -2*i);
   }
   put_word(0x0200,  segment, -2*1);   /* flag */
   put_word(segment, segment, -2*2);   /* uCS */
   put_word(segment, segment, -2*7);  /* uES */
   put_word(segment, segment, -2*8);  /* uDS */

   /* initial USP relative to USS */
   p->usp = -2*8;
   p->uss = segment;
   return 0;
}

/***********************************************************
  kfork() creates a child proc and returns the child pid.
  When scheduled to run, the child process resumes to body();
************************************************************/
PROC *kfork(char *filename)
{
  PROC *p;
  int  i, child;
  u16  segment;

  /*** get a PROC for child process: ***/
  if ( (p = get_proc(&freeList)) == 0){
       printf("no more proc\n");
       return 0;
  }
  /* initialize the new proc and its stack */
  p->status = READY;
  p->ppid = running->pid;
  p->parent = running;
  p->priority  = 1;                 // all of the same priority 1

  // clear all SAVed registers on stack
  for (i=1; i<10; i++)
      p->kstack[SSIZE-i] = 0;

  // fill in resume address
  p->kstack[SSIZE-1] = (int)body;
  // save stack TOP address in PROC
  p->ksp = &(p->kstack[SSIZE - 9]);

  enqueue(&readyQueue, p);
  nproc++;
  if (filename){
    /******** create Umode image *******************/
    segment = 0x1000*(p->pid+1);
    makeUimage("/bin/u1", p);
  }
  printf("Proc %d kforked a child %d at segment=%x\n",
          running->pid, p->pid, segment);
  return p;
}

int do_tswitch()
{
  printf("proc %d tswitch()\n", running->pid);
  tswitch();
  printf("proc %d resumes\n", running->pid);
}

int do_kfork()
{
  PROC *p;
  printf("proc %d kfork a child ", running->pid);
  p = kfork("/bin/u1");  // every proc has /bin/u1 as Umode image
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
int chname(char * y)
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

int kkfork()
{
  PROC *p = kfork("/bin/u1");
  if (p==0)
    return -1;
  return p->pid;
}
