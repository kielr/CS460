#include "ucode.c"

int main(int argc, char *argv[])
{
    char c;
    int in_fd = 0;
    int out_fd = 0;
    char n = '\n';
    char r = '\r';
    STAT fileInfo;

    if(argc < 1)
    {
        printf("usage: l2u <filename>\n");
        printf("         : l2u <filename> <filename>\n");
        exit(0);
    }

    //If we have just l2u... we want to read from stdin and output to stdout
    if(argc == 1)
    {
        in_fd = dup(0); // our in_fd is now 0;
        out_fd = dup(1);
    }
    //We have l2u and a filename
    else if (argc == 2)
    {
        in_fd = open(argv[1], 0);
        out_fd = dup(1);
    }
    //We have l2u and two filenames, open in_fd for reading, out_fd for writing
    else if(argc == 3)
    {
        in_fd = open(argv[1], 0);

        //Now we need to see if file 2 exists, if it doesn't create it, if it does, unlink it
        stat(argv[2], &fileInfo);

        if(fileInfo.st_size > 0)
        {
            printf("found file already, unlinking file\n");
            //We'll get in here if the file already exists
            unlink(argv[2]);
        }
        //Now we can create a file
        creat(argv[2]);
        out_fd = open(argv[2], 1);
    }

    //Now we need to check that both of our fds are valid
    if(in_fd < 0)
    {
        printf("l2u: couldn't open file for reading!\n");
        exit(-1);
    }
    if(out_fd < 0)
    {
        printf("l2u: couldn't open file for writing\n");
        exit(-1);
    }

    //At this point we have valid fds and we can start reading, converting, and then writing

    //We'll read character by character
    while(read(in_fd, &c, 1))
    {
        //If our character is lower case
        if (c >= 97 && c <= 122)
        {
            //Set it to upper case
            c -= 32;
        }
        //Now print the character to the out_fd
        write(out_fd, &c, 1);
        //If the character is a new line
        if(c == '\n')
        {
            //Write a carriage return
            write(2, &r, 1);
        }
        //If the character is a carriage return
        else if(c == '\r')
        {
            //write a new line
            write(1, &n, 1);
            //Write a carriage return to ERROR so we don't increase file size
            write(2, &r, 1);
        }

    }
    return;
}
