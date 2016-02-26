#include "ucode.c"

int in_fd, out_fd;
STAT in_stat, out_stat;

//This function opens input and stats it for later.
void OpenInput(char *name)
{
    in_fd = open(name, 0);

    stat(name, &in_stat);

    if(in_fd == -1)
    {
        printf("cp: couldn't open file for reading!");
        exit(-1);
    }
}

//This function opens output and tests against our earlier stat to make sure
//we are not copying into ourselves
void OpenOutput(char *name)
{
    stat(name, &out_stat);

    if(out_stat.st_ino == in_stat.st_ino)
    {
        printf("cp: cannot cp a file into itself\n");
        exit(-1);
    }
    creat(name);
    out_fd = open(name, 1);
    if(out_fd == -1)
    {
        printf("cp: could not open file for writing!\n");
        exit(-1);
    }
}

//this is my cp

int main(int argc, char *argv[])
{
    char c;

    //Make sure we didn't get a weird cp call
    if(argc > 3 && argc < 3)
    {
        printf("usage: cp <filename> <filename2>");
    }
    //If we got a valid cp call, open files for copying
    if(argc == 3)
    {
        OpenInput(argv[1]);
        OpenOutput(argv[2]);
    }

    //Make sure the fd's are valid
    if(out_fd == -1)
    {
        printf("cp: could not open file for writing!");
        exit(-1);
    }

    //Read byte by byte into the new file from the old file
    while(read(in_fd, &c, 1))
    {
        write(out_fd, &c, 1);
    }

    exit(0);
}
