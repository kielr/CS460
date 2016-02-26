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
// ucode.c file
char *cmd[]={"getpid", "ps", "chname", "kmode", "switch", "wait", "exit", 
             "fork", "exec", "pipe", "pfd", "read", "write", "close", 0};

int show_menu()
{
   printf("******************** Menu ***************************\n");
   printf("*  ps  chname  kmode  switch  wait  exit  fork  exec *\n");
          //   1     2      3       4      5     6    7     8 
   printf("*  pipe  pfd   read   write   close                 *\n");
	  //   9     10    11      12     13    
   printf("*****************************************************\n");
}

int find_cmd(char *name)
{
   int i = 0;   
   char *p = cmd[0];

   while (p){
     if (!strcmp(p, name))
        return i;
     p = cmd[++i];
   } 
   return(-1);
}


int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
   syscall(1, 0, 0);
}

int chname()
{
    char s[64];
    printf("\ninput new name : ");
    gets(s);
    syscall(2, s, 0);
}

int kmode()
{
    printf("kmode : enter Kmode via INT 80\n");
    printf("proc %d going K mode ....\n", getpid());
        syscall(3, 0, 0);
    printf("proc %d back from Kernel\n", getpid());
}    

int kswitch()
{
    printf("proc %d enter Kernel to switch proc\n", getpid());
        syscall(4,0,0);
    printf("proc %d back from Kernel\n", getpid());
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n");
    return child; 
} 

int exit()
{
   char exitValue;
   printf("enter an exitValue (0-9) : ");
   exitValue=(getc()&0x7F) - '0';
   printf("enter kernel to die with exitValue=%d\n",exitValue);
   _kexit(exitValue);
}

int _kexit(int exitValue)
{
  syscall(6,exitValue,0);
}

int fork()
{
  int child;
  child = syscall(7,0,0,0);
  if (child)
    printf("parent % return form fork, child=%d\n", getpid(), child);
  else
    printf("child %d return from fork, child=%d\n", getpid(), child);
}

int exec()
{
  int r;
  char filename[32];
  printf("enter exec filename : ");
  gets(filename);
  r = syscall(8,filename,0,0);
  printf("exec failed\n");
}

int pd[2];

int pipe()
{
   printf("pipe syscall\n");
   syscall(30, pd, 0);
   printf("proc %d created a pipe with fd = %d %d\n", 
           getpid(), pd[0], pd[1]);
}

int pfd()
{
  syscall(34,0,0,0);
}
  
int read_pipe()
{
  char fds[32], buf[1024]; 
  int fd, n, nbytes;
  pfd();

  printf("read : enter fd nbytes : ");
  gets(fds);
  sscanf(fds, "%d %d",&fd, &nbytes);
  printf("fd=%d  nbytes=%d\n", fd, nbytes); 

  n = syscall(31, fd, buf, nbytes);

  if (n>=0){
     printf("proc %d back to Umode, read %d bytes from pipe : ",
             getpid(), n);
     buf[n]=0;
     printf("%s\n", buf);
  }
  else
    printf("read pipe failed\n");
}

int write_pipe()
{
  char fds[16], buf[1024]; 
  int fd, n, nbytes;
  pfd();
  printf("write : enter fd text : ");
  gets(fds);
  sscanf(fds, "%d %s", &fd, buf);
  nbytes = strlen(buf);
            
  printf("fd=%d nbytes=%d : %s\n", fd, nbytes, buf);

  n = syscall(32,fd,buf,nbytes);

  if (n>=0){
     printf("\nproc %d back to Umode, wrote %d bytes to pipe\n", getpid(),n);
  }
  else
    printf("write pipe failed\n");
}

int close_pipe()
{
  char s[16]; 
  int fd;
  printf("enter fd to close : ");
  gets(s);
  fd = atoi(s);
  syscall(33, fd);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}
