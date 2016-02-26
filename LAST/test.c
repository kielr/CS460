#include "ucode.c"  //<========== AS POSTED on class website

// init.c

int pid, child, s0, s1, status;

main(int argc, char *argv[])
{
    int in = 0;
    int out = 0;

    // open() /dev/tty0 as 0 (READ) and 1 (WRTIE) in order to display messages
    open("/dev/tty0", 0);
    open("/dev/tty0", 1);


    // Now we can use printf, which calls putc(), which writes to stdout
    s0 = fork();
    // If we are the parent of s0
    if(s0)
    {
        s1 = fork();
        // If we are the parent of s1
        if(s1)
        {
            child = fork();
            // If we are the parend of child
            if(child)
            {
                parent();// ?
            }
            else// child (aka console)
            {
                login();
            }
        }
        else// s1
        {
            loginS1();
        }

    }
    else // s0
    {
        loginS0();
    }
}

int login()
{
    exec("login /dev/tty0");
}

int loginS0()
{
    exec("login /dev/ttyS0");
}

int loginS1()
{
    exec("login /dev/ttyS1");
}

int parent()
{
    while(1)
    {
        printf("JINIT : waiting .....\n");

        pid = wait(&status);

        if (pid == child)
        {
            //fork another login child
            child = fork();
            // If we are the parend of child
            if(child)
            {
                continue;
            }
            else// child (aka console)
            {
                login();
            }
        }
        else if (pid == s0)
        {
            //fork another login s0
            s0 = fork();
            // If we are the parend of s0
            if(s0)
            {
                continue;
            }
            else// s0
            {
                loginS0();
            }
        }
        else if (pid == s1)
        {
            //fork another login s1
            s1 = fork();
            // If we are the parend of s1
            if(s1)
            {
                continue;
            }
            else// s1
            {
                loginS1();
            }
        }
        else
        {
            printf("JINIT : buried an orphan child %d\n", pid);
        }
    }
}
