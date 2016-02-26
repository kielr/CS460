#include "ucode.c"


/*
 * READ PAGE 365 OF BOOK TO KNOW SH ALGORITHM
*/

int status;

char *commands[] = {
    "cd",
    "logout",
    "pwd",
    0
};

//This function just loops through my commands string array to try and find the command
int getCmd(char *cmd)
{
    int i;
    char *cp = commands[0];

    i = 0;
    while(cp)
    {
        if(strcmp(cp, cmd) == 0)
        {
            return i;
        }
        i++;
        cp = commands[i];
    }

    return -1;
}

void logout ()
{
    printf("rash: Logging you out.\n");
    chdir("/");
    exit(0);
}



//Here's my big explanation of the do_pipe algorithm...
//Let's say you give do_pipe this:
//
//                cat f       |       cat         |        l2u     |       grep LINE > out
//     1   &^************ HEAD *************^ ^****** TAIL *******^
//          |                                       /
//     2    |  ^****** HEAD *****^^** TAIL **^_____/
//          |                          /
//     3    |  ^ HEAD ^^** TAIL****^__/
//          |              /
//     4    | ^ HEAD ^ ___/
//          v
//
//
//          1. do_pipe accepts your input
//                  -It then scans your input and determines your head and tail, and if you have a pipe
//                  -return 1 if we have a pipe, return 0 if we don't
//                  -we have a pipe, create a pipe, pipe(local_pd);
//                  -fork a process, pid = fork();
//                  -we want parent reader, so close(pd[1]); dup2(pd[0], 0); close(pd[0]);
//                  -execute tail
//                  -we want child to be writer, so recursively call down do_pipe(HEAD, local_pd);
//
//          2. repeat 1
//                  -This time we have a pd
//                  -we want to close stdout and replace with our pipe writer, close(pd[0]); dup2(pd[1], 1); close(pd[1]);
//                  -proceed with 1
//
//         3. repeat 1
//
//         4. no pipe execute command, output gets written to our pipe
//                  -child reads it
//                  -child writes to out to its pipe and so on
//
//
//
int do_pipe(char *cmdLine, int *pd)
{
    //The book says to do this:
    //
    //if has a pipe passed in as a writer
    //close(pd[0]);
    //dup2(pd[1], 1);
    //close(pd[1]);

    int pid;
    int hasPipe;
    int local_pd[2];
    char *tail;
    char *s;

    //printf("we are in do_pipe()\n");
    //s = gets();

    //If we have a pipe, then we are in recursive calls, and we created a pipe earlier...
    //This is the same pipe from before, now we need to make sure that the right side is
    //duplicated to stdout
    if(pd)
    {
        close(pd[0]); //close pipe descriptor 0 reading side in
        dup2(pd[1], 1); //syscall 43:  dup2(fd1, fd2)              dup fd1 to fd2
        close(pd[1]); //Close original pd[1]
    }

    //We need to find if we have a pipe using our SCAN function
    hasPipe =  scan(cmdLine, &tail);

    //If we have a pipe
    if(hasPipe)
    {
        //create a pipe local_pd; (from book)
        pipe(local_pd);

        pid = fork();

        if(pid) //Our parent reader
        {
            //We are a reader on local_pd
            close(local_pd[1]); //Close
            dup2(local_pd[0], 0); //syscall 43:  dup2(fd1, fd2)              dup fd1 to fd2
            close(local_pd[0]);
            do_command(tail);

        }
        else
        {
            //We can recursively call with local_pd as our writer's right pd
            do_pipe(cmdLine, local_pd);
        }
    }
    //Otherwise we're just a normal binary execution call, we'll handle that in our do_command() function
    else
    {
        do_command(cmdLine);
    }

}

//do_command explanation
//
//              cat f   >   f2
//           ^cmd^R^file^
//
//      1. do_command first takes the cmdLine and tokenizes it into a token string array
//      2. do_command then loops through your token string array and attempts to find >, >>, <
//      3. if it finds them, close the appropriate file descriptors and open file
//      4. if it finds them, make sure to keep track of which token it was with lastRedirection integer
//      5. then build the command at the end using the token array and the lastRedirection integer


int do_command(char *cmdLine)
{
    //scan cmdLine for I/O redirection symbols
    //do I/O redirections
    //head = cmdLine BEFORE redirections
    //keep track of the last IO redirection for when we build the command
    //exec(head);
    int i,j;
    int lastRedirection;
    int in;
    int cmdCount = 0;
    char buildCmd[128];
    char *tokenBuf[64];
    char tokenizeLine[128];
    char *userTok;
    char copyBuf[64];
    char *s;

    strcpy(copyBuf, cmdLine);

    //printf("we are in do_command()\n");
    //s = gets();

    //Do tokenizing
    userTok = strtok(copyBuf, " ");
    //Get all the tokens
    while(userTok)
    {
        tokenBuf[cmdCount] = userTok;
        userTok = strtok(NULL, " ");
        //DEBUG
        printf("sh do_command(): tokens[%d]=%s\n", cmdCount, tokenBuf[cmdCount]);
        cmdCount++;
    }

    lastRedirection = cmdCount;

    //Main loop to look for redirection
    for(i = 0; i < cmdCount; i++)
    {
        //First, let's look for <
        if(strcmp(tokenBuf[i], "<")  == 0)
        {
            if(lastRedirection > i)
            {
                lastRedirection = i;
            }
            if(tokenBuf[i+1] == 0) //Error catch
            {
                exit(-1);
            }

            //close stdin
            close(0);
            //open file for reading
            in = open(tokenBuf[i+1], O_RDONLY);
            if(in < 0)
            {
                //Debug printing
                printf("rash: no such input file, failed to open file for reading on line 138\n");
                exit(-1);
            }
            //If we get here we can break out of the loop
            break;
        }
        //Now >
        else if(strcmp(tokenBuf[i], ">")  == 0)
        {
            if(lastRedirection > i)
            {
                lastRedirection = i;
            }
            if(tokenBuf[i+1] == 0) //Error catch
            {
                exit(-1);
            }

            //close stdout
            close(1);
            //open file for writing OR create a file
            in = open(tokenBuf[i+1], O_WRONLY | O_CREAT);
            if(in < 0)
            {
                //Debug printing
                printf("rash: couldn't open file, or couldn't create file on line 167\n");
                exit(-1);
            }
            //If we get here we can break out of the loop
            break;
        }
        //Then >>
        else if(strcmp(tokenBuf[i], ">>")  == 0)
        {
            if(lastRedirection > i)
            {
                lastRedirection = i;
            }
            if(tokenBuf[i+1] == 0) //Error catch
            {
                exit(-1);
            }

            //close stdout
            close(1);
            //open file for writing OR create a file
            in = open(tokenBuf[i+1], O_WRONLY | O_CREAT | O_APPEND);
            if(in < 0)
            {
                //Debug printing
                printf("rash: couldn't open file, or couldn't create file on line 167\n");
                exit(-1);
            }
            //If we get here we can break out of the loop
            break;
        }
    }

    //Now we need to build our command to send to exec
    strcpy(buildCmd, tokenBuf[0]);

    for(j = 1; j < lastRedirection; j++)
    {
        strcat(buildCmd, " ");
        strcat(buildCmd, tokenBuf[j]);
    }

    exec(buildCmd);

    return 1; //Yay! we got here successfully!
}

int scan (char *head, char **tail)
{
    //The book says to do this:
    //
    // For piping we need to divide cmdLine into head and tail by right most | symbol
    // if cmdLine = cmd1 | cmd2 | ... | cmdn=1 | cmdn
    //                   <======head======>|^tail^|                               return 1;
    // if cmdLine = cmd1, then head=cmd1 and tail=null                              return 0;

    //Algorithm for scan starts, we need a charPointer
    char *charPointer;

    //We need charPointer to point at the head.
    charPointer = head;

    //We need to find the end of the head, loop through using our charPointer
    while(*charPointer)
    {
        charPointer++;
    }

    //Next, we need to find the first |
    //Easy way to do this is to just loop backwards until we encounter a pipe symbol
    //Stop when we get to the head as well.
    while(charPointer != head && *charPointer != '|')
    {
        charPointer--;
    }

    //If we get to the head without encountering a '|' symbol
    //then we can return 0.
    if(charPointer == head)
    {
        return 0;
    }

    //Otherwise, we need to point to the command right of the '|
    //First, set '|' to NULL
    //Then move to the right one
    //Make sure we have no empty space
    *charPointer = NULL;
    charPointer++;
    while(*charPointer == ' ' )
    {
        charPointer++;
    }

    //Now we can finally set tail to the last command
    *tail = charPointer;

    return 1;

}

int main (int argc, char *argv[])
{
    while(1)
    {
        int i = 0;
        int pid;
        int userCmd;
        int tokensCount = 0;
        char userInputLine[128];
        char tokenizeLine[128];
        char *tokens[64];
        char *userTok;
        char *home;
        //We've entered into shell from parent, let's print name of shell and wait
        printf("rash : ");
        gets(userInputLine);

        //We've got user input, now lets tokenize it
        //First let's make a copy
        strcpy(tokenizeLine, userInputLine);
        strcpy (home, "/");

        //Then do the actual tokenizing
        userTok = strtok(tokenizeLine, " ");
        //Get all the tokens
        while(userTok)
        {
            tokens[tokensCount] = userTok;
            userTok = strtok(NULL, " ");
            //DEBUG
            printf("sh main: tokens[%d]=%s\n", tokensCount, tokens[tokensCount]);
            tokensCount++;
        }

        printf("you entered:%s\n", userInputLine);

        tokens[tokensCount] = 0; //End tokens with NULL

        //Now that we have our tokens, we should find the command that was entered
        userCmd = getCmd(tokens[0]);

        //Now that we have our command we should switch on it because our command list is so large
        if(!tokens[0])
        {
            continue;
        }

        //These are for BUILT IN commands.
        switch(userCmd)
        {
            case 0: //cd
                if(tokens[1])
                {
                    chdir(tokens[1]);
                    continue;
                }
                else
                {
                    chdir("/");
                    continue;
                }
                break;
            case 1: //logout
                logout();
                break;
            case 2: //pwd
                pwd();
                continue;
                break;

            default:
                break;

        }

        //If we get here then we are doing binary executables

        //We must fork a child
        pid = fork();

        if(pid < 0)
        {
            printf("rash: fork failed!\n");
            exit(-1);
        }
        if(pid)
        {
            wait(&status); //Wait for child to be done
        }
        else //We are in child.. let's do the executable
        {
            do_pipe(userInputLine, 0);
        }
    }
}
