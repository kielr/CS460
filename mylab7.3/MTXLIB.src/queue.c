#define SSIZE 1024
#define FREE     0

typedef struct proc{
    struct proc *next;
    int    *ksp;
    int    uss, usp;
    int    inkmode;
    int    pid;                // add pid for identify the proc
    int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
    int    ppid;               // parent pid
    struct proc *parent;
    int    priority;
    int    event;
    int    exitCode;
    char   name[32];
    int    kstack[SSIZE];      // per proc stack area
}PROC;

/****************** utility.c file ***********************/
PROC *get_proc(PROC **list)  // allocate a FREE proc from freeList
{
     PROC *p = *list;
     if (p)
       *list = p->next;
     return p;
}

int put_proc(PROC **list, PROC *p)   // return a PROC to freeList
{
    p->status = FREE;
    p->next = *list;
    *list = p;
}

    // enter a PROC into queue by priority
int enqueue(PROC **queue, PROC *p)
{
     PROC *q = *queue;
     if (q == 0 || p->priority > q->priority){
        *queue = p;
         p->next = q;
     }
     else{
         while (q->next && p->priority <= q->next->priority)
                q = q->next;
         p->next = q->next;
         q->next = p;
     }
}

    // remove and return first PROC in queue 
PROC *dequeue(PROC **queue)
{
     PROC *p = *queue;
     if (p)
        *queue = (*queue)->next;
     return p;
}

// Other print queue and print list functions 
int printList(name, p) char *name; PROC *p;
{
   printf("%s = ", name);
   while(p){
       printf("%d --> ", p->pid);
       p = p->next;
   }
   printf("NULL\n");
}

