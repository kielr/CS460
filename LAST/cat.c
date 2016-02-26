#include "ucode.c"

///Cat will take an arg, which should be a file and attempt to print it to the terminal
int main (int argc, char *argv[])
{
    char buf[1024];
    int readSize = 1024;
    char tty[64];
    char c;
    STAT fileInfo;
    int fd, i, j;
    int remainingBytes;
    char inputLine[1024];
    char n = '\n';
    char r = '\r';

    if(argc == 1) //If we only have one argument we get stuff from stdin
    {
        fd = 0;
        gettty(tty);
        //open(tty, 1);
        open(tty, 0);
        while(read(fd, &c, 1) > 0)
        {
            //First write the character
            write(1, &c, 1);

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
    }
    else if(argc >= 2) //We can have more than two input files if we want
    {
        for(i = 1; i < argc; i++)//For each file...
        {
            fd = open(argv[i], 0); //Open the current file for 0 (reading)

            if(fd == -1)
            {
                printf("cat: failed to open the file for reading.\n");
                exit(-1);
            }

            //printf("FD = %d\n", fd); //Print something about the fd;

            stat(argv[i], &fileInfo); //We need some information about the file...

            //printf("st_size = %d\n", fileInfo.st_size); //Print some more stuff about file...

            remainingBytes = fileInfo.st_size; //Get the size of the file in bytes.

            while(remainingBytes > 0) //Whie we still have bytes to read...
            {
                //If we have less bytes to read than our buffer size...
                if(remainingBytes < 1024)
                {
                    //Set our read size to our remaining byte size so we don't overlap.
                    readSize = remainingBytes;
                }

                //Finally, let's read into the buf from the fd 1024 at a time
                read(fd, buf, readSize);

                //We need to loop through our buffer to print it out character by character
                for( j = 0; j < readSize; j++)
                {
                    c = buf[j];

                    //First write the character
                    write(1, &c, 1);

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
                        //Write a carriage return
                        write(2, &r, 1);
                    }
                }

                //Now all we need to do is decrement our remaining bytes left by our readSize
                remainingBytes -= readSize;

            }

            close(fd);
        }

        exit(0); //We ran successfully.

    }
    else //If we get here then the user probably made a mistake
    {
        printf("usage: cat <filename1> <filename2> ... etc\n");
        exit(-1);
    }
}
