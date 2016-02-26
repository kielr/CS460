#include "ucode.c"

void clearline(char *inputLine)
{
    int i;

    for (i = 0; i < 160; i++)
    {
        inputLine[i]=0;
    }
}

int main(int argc, char *argv[])
{
    int fd, size, i;
    int lineNumber = 0;
    int dataRead = 1;
    char inputLine[160];
    char *c = inputLine;
    char tty[64];

    clearline(&inputLine);


    //Check for no arguments
    if (argc < 3 && argc != 2) // we got a weird grep call
    {
        printf("usage: grep <pattern> <file>\n");
        exit(0);
    }

    //If argc = 2
    if(argc == 2)
    {
        fd = dup(0); // just read from standard in
    }
    else //Otherwise we have a valid input file
    {
        fd = open(argv[2], O_RDONLY); //Open file for READ ONLY
    }
    if(fd < 0) //If we failed to open the file...
    {
        printf("grep: failed to find the file or the file is bad.\n");
        exit(-1);
    }

    size = strlen(argv[1]); //We need the size of the pattern for later

    while(dataRead > 0)
    {
        lineNumber++; //Increment our line number

        //Now we must get a line from the file
        for(i = 0; i < 159; i++) //Loop through our lines array
        {
            dataRead = read(fd, &inputLine[i], 1); //Get the amount to read
            if(dataRead < 1 || inputLine[i] == '\n') //Until the end of file OR new line
            {
                inputLine[i+1] = 0; //Make sure last is cleared
                break; //End the loop here
            }
        }

        c = inputLine; //set our current inputline

        while(*c != '\n') //While we aren't a new line...
        {
            //Compare our input line to our pattern using strncmp
            if (strncmp(c, argv[1], size) == 0 && inputLine[0] != 0)
            {
                printf("%d:",lineNumber, inputLine);
                write(1, inputLine, (i+1));
                printf("\r");
                break;
            }
            c++;
        }

        clearline(&inputLine);
    }

    close(fd);
    exit(0);

}
