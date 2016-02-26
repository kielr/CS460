// ucode.c file

char *cmd[]={"getpid", "ps", "chname", "kmode", "switch", "wait", "exit", 
             "fork", "exec", 0};

int show_menu()
{
   printf("********************* Menu ***************************\n");
   printf("* ps  chname  kmode  switch  wait  exit  fork  exec  *\n");
   /*         1     2      3      4      5     6     7     8      */
   printf("******************************************************\n");
}

int find_cmd(char *name)
{
   int i;   char *p;
   i = 0;   p = cmd[0];
   while (p){
         if (strcmp(p, name)==0)
            return i;
         i++;  p = cmd[i];
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
    printf("input new name : ");
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

int vfork()
{
  return syscall(9,0,0,0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}
