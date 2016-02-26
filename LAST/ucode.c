#include "type.h"

int pid;
char line[64], pathname[32], i2[32], i3[32];
char *name[16], components[64];
int nk;
#define EOF -1

int getpid()
{
   return syscall(0,0,0);
}

int getppid()
{
   return syscall(1,0,0);
}

int getpri()
{
  return syscall(2,0,0);
}

int chpri(int value)
{
  return syscall(3,value,0);
}

int getuid()
{
  return syscall(4,0,0);
}

int chuid(int uid, int gid)
{
  return syscall(5,uid, gid);
}

int tswitch()
{
  return syscall(6,0,0);
}


int fork()
{
   return syscall(10, 0, 0);
}

int exec(char *cmd_line)
{
   return syscall(11, cmd_line, 0);
}

int wait(int *status)
{
  return syscall(12, status, 0);
}

int vfork()
{
  return syscall(13,0,0);
}

int thread(int fn, int stack, int flag, int ptr)
{
  return syscall(14, fn, stack, flag, ptr);
}

// 15-19: mutex for threads


int mutex_creat()
{
  return syscall(15, 0,0);
}

int mutex_lock(int *m)
{
  return syscall(16, m, 0);
}

int mutex_unlock(int *m)
{
  return syscall(17, m, 0);
}

int mutex_destroy(int *m)
{
  return syscall(18, m, 0);
}


int mkdir(char *name)
{
   return syscall(20, name, 0);
}

int rmdir(char *name)
{
   return syscall(21, name, 0);
}

int creat(char *filename)
{
    return syscall(22, filename, 30);
}

int link(char *oldfile, char *newfile)
{
  return syscall(23, oldfile, newfile,0);
}

int unlink(char *file)
{
  return syscall(24, file, 0);
}


int symlink(char *oldfile, char *newfile)
{
  return syscall(25, oldfile, newfile);
}

int readlink(char *file, char *linkname)
{
  return syscall(26, file, linkname, 0);
}

int chdir(char *name)
{
   return syscall(27, name, 0);
}

int getcwd(char *cwdname)
{
   return syscall(28, cwdname, 0);
}

int stat(char *filename, struct stat *sPtr)
{
   return syscall(29, filename, sPtr);
}

int fstat(int fd, struct stat *sptr)
{
  return syscall(30,fd,sptr,0);
}



int open(char *file, int flag)
{
   return syscall(31, file, flag);
}

int close(int fd)
{
   return syscall(32, fd);
}

u32 lseek(int fd, u32 offset, int ww)
{
   return syscall(33, fd, (u32)offset, ww);
}

int read(int fd, char *buf, int nbytes)
{
    return syscall(34, fd, buf, nbytes);
}

int write(int fd, char *buf, int nbytes)
{
    return syscall(35, fd, buf, nbytes);
}

int pipe(int *pd)
{
    return syscall(36, pd, 0);
}



int chmod(char *file, u16 mode)
{
   return syscall(37, file, mode);
}

int chown(char *file, int uid)
{
   return syscall(38, file, uid);
}

int touch(char *filename)
{
   return syscall(39, filename, 0);
}

int fixtty(char *tty)
{
   return syscall(40, tty, 0);
}

int settty(char *tty)
{
   return syscall(40, tty, 0);
}

int gettty(char *tty)
{
   return syscall(41, tty, 0);
}


int dup(int fd)
{
   return syscall(42, fd, 0);
}

int dup2(int fd, int gd)
{
   return syscall(43, fd, gd);
}

int ps(char *y)
{
   return syscall(44,y,0);
}

int mount(char *dev, char **mpt)
{
  return syscall(45, dev, mpt);
}

int umount(char *dev)
{
  return syscall(46, dev);
}

/********** CDROM syscalls ******************/
int getSector(u32 sector, char *ubuf, u16 nsector)
{
  return syscall(47, (u32)sector, ubuf, nsector);
}

int do_cmd(u16 cmd, u16 value)
{
  return syscall(48, cmd, value);
}


int kill(int sig, int pid)
{
  return syscall(50, sig, pid);
}

int signal(u16 sig, u16 catcher)
{
  return syscall(51, sig, catcher);
}

int pause(u16 t)
{
  return syscall(52, t);
}

int itimer(u16 t)
{
  return syscall(53, t);
}

int send(char *msg, int pid)
{
  syscall(54, msg, pid);
}

int recv(char *msg)
{
  syscall(55,msg, 0);
}


int do_texit()
{
  int pid = getpid();
  printf("thread %d texit()\n", pid);
  texit(pid);
}

int tjoin(int n)
{
  return syscall(56, n, 0);
}

int texit(int v)
{
  syscall(57,v,0);
}

int khits(int *requests, int *hits)
{
  syscall(58,requests,hits);
}


int setcolor(int color)
{
  return syscall(59, color,0);
}

int sync()
{
  return syscall(60, 0, 0);
}

int thinit()
{
  return syscall(61, 0, 0);
}

int exit(int value)
{
   syscall(9, value, 0);
}

int pwd()
{
  char cwd[64];
  getcwd(cwd);
  printf("%s\n", cwd);
}
