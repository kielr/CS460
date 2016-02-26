int ksleep(int event)
{
    // caller goes to SLEEP on event
    running->event = event;   // record event in PROC.event
    running->status = SLEEP;  // change status to SLEEP
    tswitch();                //give up CPU
}

int kwakeup(int event)
{
    // wakeup ALL procs sleeping on event
    int i;
    PROC *p;
    for (i=1; i<NPROC; i++)  // not applicable to P0
    {
        p = &proc[i];
        if (p->status == SLEEP && p->event == event)
        {
            p->event = 0; // cancel PROCs event
            p->status = READY; // make it ready to run again
            enqueue(&readyQueue, p);
        }
    }
}

int kexit(int exitValue)
{
    // caller becomes a ZOMBIE with an exitValue in its PROC
    // (of course, give away children, if any, and wakeup parent and/or P1)

}
int kwait(int *status)
{
    // caller waits for a ZOMBIE child
    PROC *p;
    int i, hasChild = 0;
    while(1)  // search PROCs for a child
    {
        for (i=1; i<NPROC; i++)  // exclude P0
        {
            p = &proc[i];
            if (p->status != FREE && p->ppid == running->pid)
            {
                hasChild = 1; // has child flag
                if (p->status == ZOMBIE)  // lay the dead child to rest
                {
                    *status = p->exitCode; // collect its exitCode
                    p->status = FREE; // free its PROC
                    put_proc(&freeList, p); // to freeList
                    nproc--; // once less processes
                    return(p->pid); // return its pid
                }
            }
        }
        if (!hasChild) return -1; // no child, return ERROR
        ksleep(running); // still has kids alive: sleep on PROC address
    }
}
