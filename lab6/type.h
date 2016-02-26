typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define NULL     0
#define NPROC    9
#define SSIZE 1024

/******* PROC status ********/
#define FREE     0
#define READY    1
#define RUNNING  2
#define STOPPED  3
#define SLEEP    4
#define ZOMBIE   5

#define READ_PIPE  4
#define WRITE_PIPE 5

#define NOFT      20
#define NFD       10

typedef struct Oft{
  int   mode;
  int   refCount;
  struct pipe *pipe_ptr;
} OFT;

#define PSIZE 10
#define NPIPE 10

typedef struct pipe{
  char  buf[PSIZE];
  int   head, tail, data, room;
  int   nreader, nwriter;
  int   busy;
}PIPE;

typedef struct proc{
    struct proc *next;
    int    *ksp;
    int    uss, usp;
    int    pid;                // add pid for identify the proc
    int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
    int    ppid;               // parent pid
    struct proc *parent;
    int    priority;
    int    event;
    int    exitCode;
    char   name[32];

    OFT    *fd[NFD];    
    int    kstack[SSIZE];      // per proc stack area
}PROC;
