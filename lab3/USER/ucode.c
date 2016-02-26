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

char *cmd[]={"getpid", "ps", "chname", "kfork", "switch", "wait", "exit", 0};

int show_menu()
{
   printf("***************** Menu *******************\n");
   printf("*  ps  chname  kfork  switch  wait  exit *\n");
   printf("******************************************\n");
}

int find_cmd(char *name)
{
   int i=0;
   char *p=cmd[0];

   while (p){
         if (strcmp(p, name)==0)
            return i;
         i++;
         p = cmd[i];
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

int kfork()
{
    int pid;
    printf("proc %d enter kernel to kfork a child\n", getpid());
    pid = syscall(3, 0, 0);
    if (pid>0)
      printf("proc %d kforked a child %d\n", getpid(), pid);
    else
      printf("kforked failed\n");
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
}

int exit()
{
   int exitValue;
   printf("\nenter an exitValue (0-9) : ");
   exitValue=(getc()&0x7F) - '0';
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _exit(exitValue);
}

int _exit(int exitValue)
{
  syscall(6,exitValue,0);
}

int getc()
{
  return syscall(7,0,0) & 0x7F;
}

int putc(char c)
{
  return syscall(8,c,0,0);
}


int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}
