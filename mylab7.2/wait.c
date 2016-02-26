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
int ksleep(int event)
{
  int sr = int_off();
  running->status = SLEEP;
  running->event = event;
  // enter sleepList FIFO
  enqueue(&sleepList, running);
  tswitch();
  int_on(sr);
}
  
int ready(PROC *p)
{
  p->event = 0;
  p->status = READY;
  enqueue(&readyQueue, p);
  printf("wakeup proc %d\n", p->pid);
}

/* wake up ALL procs sleeping on event */

int kwakeup(int event)
{
  PROC *p, *q = 0;
  int sr = int_off();

  while(p = dequeue(&sleepList)){
    if (p->event == event){
       p->status = READY;
       enqueue(&readyQueue, p);
       continue;
    }
    enqueue(&q, p);
  }
  sleepList = q;
  int_on(sr);
}

int kexit(int exitValue)
{
  int i; PROC *p;
  /* send children (dead or alive) to P1's orphanage */
  for (i = 1; i< NPROC; i++){
      p = &proc[i];
      if (p->status != FREE && p->ppid == running->pid){
          p->ppid = 1;
          p->parent = &proc[1];
      }
  }
  // restore name string
  strcpy(running->name, pname[running->pid]);
  /* record exitValue and become a ZOMBIE */
  running->exitCode = exitValue;
  running->status = ZOMBIE;
  
  /* wakeup parent and P1 */
  kwakeup(running->parent);
  kwakeup(&proc[1]);
  tswitch();
}

int kwait(int *status)
{
  PROC *p;
  int  i, found = 0;
  while(1){
    //found = 0;
     for (i=0; i<NPROC; i++){ 
          p = &proc[i];
          if (p->ppid == running->pid && p->status != FREE){ 
             found = 1;
             /* lay the dead child to rest */
             if (p->status == ZOMBIE){
                 *status = p->exitCode;
                 p->status = FREE;       /* free its PROC */
                 put_proc(&freeList, p);
                 nproc--;
                 return(p->pid);         /* return its pid */
             }
          }
     }
     if (!found)                         /* no child */
          return(-1);
     ksleep(running);                     /* has kids still alive */
  }
}
