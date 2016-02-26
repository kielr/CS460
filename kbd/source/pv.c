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

typedef struct sem{
    int value;
    PROC *queue;
}SEMAPHORE;

extern int int_off(), int_on();

int P(SEMAPHORE *s)
{
    int sr;

    sr = int_off();

    s->value--;
    if (s->value < 0){
       //printf("proc %d blocked in P() at semaphore = %x\n",unning->pid, s);
       running->sem = s;   // blocked on this semaphore
       running->status = BLOCK;
       enqueue(&(s->queue), running);
       tswitch();
    }
    int_on(sr);
}

int V(SEMAPHORE *s)
{
    int sr; PROC *p;
    sr = int_off();

    s->value++;
    if (s->value<=0){
       p = dequeue(&(s->queue));
       p->status = READY;
       p->sem = 0;               // no longer blocked at this semaphore
       enqueue(&readyQueue, p);
       // printf("proc %d unblocked in V()\n", p->pid);
    }
    int_on(sr);
}
