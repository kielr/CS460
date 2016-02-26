#include "ucode.c"

//This function prints a line until the end of the line which should be 80, because the max dimensions
//of MTX is (24, 80)
void PrintLine(int fd)
{
    int i, nRead;
    char c, n = '\n', r = '\r';

    //Iterate 0-79 columns
    for (i = 0; i < 80; i++)
    {
        //Find how much we need to read, and get the first character
        nRead = read(fd, &c, 1);
        write(1, &c, 1); //Write the character to stdout

        //If the character we just wrote is a \n character
        if (c == '\n')
        {
            write(2, &r, 1); //Write a carriage return to stderr, it puts a new line in the terminal and avoids
                                    //staircase printing
        }
        //If the character we just wrote is a \r character
        else if (c == '\r')
        {
            write(1, &n, 1); //Write a \n character to stdout,
            write(2, &r, 1); //and a carriage return stderr so we don't increase file size and don't get
                                    //staircase printing
        }

        //If we get nRead not equal to 1 (meaning we didn't read anything)
        if (nRead != 1)
        {
            exit(0); //Exit
        }

        //If the character was a \n or a \r then just break the loop because that's the end of the line
        if (c == '\n' || c == '\r')
        {
            break;
        }
    }
}


//This function just calls PrintLine() 24 times because there are 24 rows in a page in MTX
void PrintPage(int fd)
{
    int i;

    putc('\n');
    for (i = 0; i < 24; i++)
    {
        PrintLine(fd);
    }
}

//  more.c must print one page, then accept user input for controls
// if the user presses space, print an entire page
// if the user presses enter, print a line
// if the user presses q, quit more.c
int main(int argc, char *argv[])
{
    char c, userInput, tty[64];
    int fd, read, i, j;

    //If we only have "more" as our argument, then we are probably in a pipe, let's handle it
    //
    //                                  cat  f       |       more
    //                                      |_____________|
    //                                                              |
    //                                                             v
    //                                                            stdin = cat f; //we need to read from the terminal AND from cat f
    //                                                            fd = dup(0); //first you need to duplicate your stdin
    //                                                            close(0) // CLOSE 0 so it becomes free on fd table
    //                                                            gettty(tty) //we don't know our tty name, get it
    //                                                            open(tty, 0) //Open it for read, since we closed 0, it will take the place of stdin
    //
    //
    if (argc == 1)
    {
        //Let's say we got passed cat f | more, cat f will be our stdin (0)
        fd = dup(0); //DUPLICATE "cat f" into our file decriptor
        close(0);  //close "cat f" so we can replace it with our terminal
        gettty(tty); //We need to know what our tty is, get it from our PROC.tty[]
        open(tty, 0); //Open it for read or "in", since we still need to accept user input
    }
    //Otherwise, we have a a file input, and we are probably not piping
    else
    {
        fd = open(argv[1], 0); //Open the file for reading
    }

    //Just in case we screw up our fd
    if (fd < 0)
    {
        printf("more: cannot find the file %s\n", argv[1]);
        return -1;
    }

    //When we get here we should have a valid fd open, and be ready to print stuff to the screen
    while (1)
    {
        PrintPage(fd);
        while (1)
        {
            userInput = getc();
            switch(userInput)
            {
            case '\r': //If we get an enter we need to print just a line and move the screen up
                PrintLine(fd);
                break;
            case ' ': //If we get a space character, print the entire screen.
                PrintPage(fd);
                break;
            case 'q': //If we get a 'q' the user wants to end more
                printf("\n");
                printf("more: quitting...\n\n");
                return 0;
                break;
            default:
                break;
            }
        }
    }
}
