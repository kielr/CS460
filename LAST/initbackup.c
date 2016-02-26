#include "ucode.c"

int pid, child, status;
int stdin, stdout;
int s0, s1;

main(int argc, char *argv[])
{
    int In, Out;
    In = open("/dev/tty0", O_RDONLY);
    Out = open("/dev/tty0", O_WRONLY);
     printf("Kiel's init : forking a login task on serial 0...\n");
     s0 = fork();

     if (s0)
     {
         printf("Kiel's init: forking a login task on serial 1...\n");
         s1 = fork();
         if(s1)
         {
             printf("Kiel's init: forking a login task on console...\n");
             child = fork();
             if(child)
             {
                 parent();
             }
             else
             {
                 login();
             }
         }
         else
         {
             logins1();
         }
     }
     else
     {
         logins0();
     }
}

int login()
{
  exec("login2 /dev/tty0");
}

int logins0()
{
  exec("login2 /dev/ttyS0");
}

int logins1()
{
  exec("login2 /dev/ttyS1");
}

int parent()
{
  while(1){
    printf("KIELINIT : waiting .....\n");

    pid = wait(&status);

    if (pid == child)
    {
        child = fork();
        if (child)
        {
            continue;
        }
        else
        {
            login();
        }
    }
    if (pid == s0)
    {
        s0 = fork();
        if (s0)
        {
            continue;
        }
        else
        {
            logins0();
        }
    }
    if (pid == s1)
    {
        s1 = fork();
        if (s1)
        {
            continue;
        }
        else
        {
            logins1();
        }
    }
    printf("KCINIT : I just buried an orphan child task %d\n", pid);

  }
}
