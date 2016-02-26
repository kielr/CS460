//*************************************************************************
//                      Logic of init.c
// NOTE: this init.c creates only ONE login process on console=/dev/tty0
// YOUR init.c must also create login processes on serial ports /dev/ttyS0
// and /dev/ttyS1..
//************************************************************************

int pid, child, status, s0, s1;
int stdin, stdout;

#include "ucode.c"  //<========== AS POSTED on class website

main(int argc, char *argv[])
{
    // open /dev/tty0 as 0 (READ) and 1 (WRTIE) in order to display messages
    open("/dev/tty0", 0);
    open("/dev/tty0", 1);


    // Now we can use printf, which calls putc(), which writes to stdout
    printf("kiel init : fork a login task on console\n");
    child = fork();

    if (child)
    {
        printf("kiel init : fork a login task on s0\n");
        s0 = fork();
        if(s0)
        {
            printf("kiel init : fork a login task on s1\n");
            s1 = fork();
            if(s1) //parent
            {
                parent();
            }
            else //s1
            {
                logins1();
                //exec("login /dev/ttyS1");
            }
        }
        else //s0
        {
            logins0();
            //exec("login /dev/ttyS0");
        }
    }
    else //child
    {
        login();
        //exec("login /dev/tty0");
    }
}

int login()
{
    exec("login /dev/tty0");
}
int logins0()
{
    exec("login /dev/ttyS0");
}
int logins1()
{
    exec("login /dev/ttyS1");
}

int parent()
{
    while(1)
    {
        printf("kiel init : waiting .....\n");

        pid = wait(&status);

        if (pid == child)
        {
            child = fork();
            if(child)
            {
                continue; //Parent continue waiting
            }
            else
            {
                login(); //Child go do login process!
            }
        }
        if (pid == s0)
        {
            s0 = fork();
            if(s0)
            {
                continue; //Parent continue waiting
            }
            else
            {
                logins0(); //Child go do login process!
            }
        }
        if (pid == s1)
        {
            s1 = fork();
            if(s1)
            {
                continue; //Parent continue waiting
            }
            else
            {
                logins1(); //Child go do login process!
            }
        }
        else
        {
            printf("INIT : buried an orphan child %d\n", pid);
        }
    }
}
