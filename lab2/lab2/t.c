/*********** MTX4.2 kernel t.c file **********************/
#define NPROC 9
#define SSIZE 1024

/******* PROC status ********/
#define FREE 0
#define READY 1
#define STOP 2
#define DEAD 3

typedef struct proc
{
    struct proc *next;
    int *ksp;
    int pid; // add pid for identify the proc
    int ppid; // parent pid;
    int status; // status = FREE|READY|STOPPED|DEAD, etc
    int priority; // scheduling priority
    int kstack[SSIZE]; // proc stack area
} PROC;

PROC *get_proc(PROC **list);
int enqueue(PROC **queue, PROC *p);
PROC *dequeue(PROC **queue);
printList(char *name,PROC *list);


PROC proc[NPROC], *running, *freeList, *readyQueue;
int procSize = sizeof(PROC);

PROC *get_proc(PROC **list)
{
    return dequeue(list);
} //: return a FREE PROC pointer from list

//int put_proc(PROC **list, PROC *p); //: enter p into list
int enqueue(PROC **queue, PROC *p)
{
    PROC *temp = 0, *prev = 0;

    if(*queue == 0)
    {
        *queue = p;
        p->next = 0;
    }
    else
    {
        if(p->priority > (*queue)->priority)
        {
            p->next = (*queue);
            *queue = p;
        }
        else
        {
            temp = prev = *queue;
            while((temp) && temp->priority >= p->priority)
            {
                prev = temp;
                temp = temp->next;
            }

            prev->next = p;
            p->next = temp;
        }
    }

    return 1;
} //: enter p into queue by priority

PROC *dequeue(PROC **queue)
{
    PROC *p = 0;

    if(*queue == 0)
    {
        p = 0;
    }
    else
    {
        p = *queue;
        *queue = (*queue)->next;
    }
    return p;
} //: return first element removed from queue

printList(char *name, PROC *list)
{
    PROC *p = list, *root = list;

    while (p != 0)
    {
        printf("< %d > => ", p->pid);
        p = p->next;
    }
    printf("NULL\r\n\n");

    return 1;
}    //: print name=list content

int body(int pid)
{
    char c;
    printf("proc %d starts from body()\n", running->pid);
    while(1)
    {
        printList("freelist ", freeList);// optional: show the freeList
        printList("readyQueue", readyQueue); // show the readyQueue
        printf("proc %d running: parent=%d\n",running->pid,running->ppid);
        printf("enter a char [s|f] : ");
        c = getc();
        printf("%c\n", c);
        switch(c)
        {
        case 'f' :
            do_kfork();
            break;
        case 's' :
            do_tswitch();
            break;
        }
    }
}

//Called in do_fork() and main()
//kfork() creates a child process by pulling a proc off of the
//free process list
//moves a process from the free process to the readyQueue
//set its parent to running
//zeros out all registers of the new process

PROC *kfork() // create a child process, begin from body()
{
    int i;
    PROC *p = get_proc(&freeList);
    if (!p)
    {
        printf("no more PROC, kfork() failed\n");
        return 0;
    }
    p->status = READY;
    p->priority = 1; // priority = 1 for all proc except P0
    p->ppid = running->pid; // parent = running
    /* initialize new proc's kstack[ ] */
    for (i=1; i<10; i++) // saved CPU registers
        p->kstack[SSIZE-i]= 0 ; // all 0's
    //ADD these lines because we have two new registers to deal with
    p->kstack[SSIZE-10] = 0x1000;
    p->kstack[SSIZE-11] = 0x1000;

    p->kstack[SSIZE-1] = (int)body; // resume point=address of body()

    //Edit this to handle two new registers
    p->ksp = &p->kstack[SSIZE-11]; // stores a local reference to the current proc's stack pointer for later use
    enqueue(&readyQueue, p); // enter p into readyQueue by priority
    return p; // return child PROC pointer
}

int init()
{
    PROC *p;
    int i;
    printf("init ....\n");
    for (i=0; i<NPROC; i++)  // initialize all procs
    {
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;
        p->next = &proc[i+1];
    }
    proc[NPROC-1].next = 0;
    freeList = &proc[0]; // all procs are in freeList
    readyQueue = 0;
    /**** create P0 as running ******/
    p = get_proc(&freeList); // allocate a PROC from freeList
    p->ppid = 0; // P0s parent is itself
    p->status = READY;
    running = p; // P0 is now running
}

int scheduler()
{
    if (running->status == READY) // if running is still READY
        enqueue(&readyQueue, running); // enter it into readyQueue
    running = dequeue(&readyQueue); // new running
}

main()
{
    printf("\nMTX starts in main()\n");
    init(); // initialize and create P0 as running
    kfork(); // P0 creates child P1
    while(1)  // P0 switches if readyQueue not empty
    {
        if (readyQueue)
            tswitch();
    }
}

/*************** kernel command functions ****************/
int do_kfork( )
{
    PROC *p = kfork();
    if (p == 0)
    {
        printf("kfork failed\n");
        return -1;
    }
    printf("PROC %d kfork a child %d\n", running->pid, p->pid);
    return p->pid;
}

int do_tswitch()
{
    tswitch();
}
