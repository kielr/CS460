#include "ucode.c"
// seems stdin,stdout,stderr are keywords in bcc
// int stdin, stdout, stderr; ==> auto_start symbol
// login : call by init as "login tty0" | "login ttyS0" | ...
// Upon entry, name[0]=filename, name[1]=tty string

int main(int argc, char *argv[ ])
{
    char buf[1024], uName[64], uPass[64], Home[64], User[64], Password[64];
    char *tty = argv[1];
    int gid, uid, i, n, valid;
    int in, out, err;
    int fd;
    char *s;
    char *tok;

    valid = 0;

    close(0);
    close(1);
    close(2);

    settty(tty);

    in  = open(tty, 0);
    out = open(tty, 1);
    err = open(tty, 2);

    signal(2,1); // ignore Control-C interrupts so that
    // Control-C KILLs other procs on this tty but not the main sh

    while(1){
        //Ask for information from user
        printf("kiel-login:");     //1. show login: to stdout
        gets(uName);              //2. read user name from stdin
        printf("password:");   //3. show passwd to stdout
        gets(uPass);               //4. read password from stdin

        ///Open /etc/passwd file to get uName's uid, gid, home, program
        fd = open("/etc/passwd", 0);

        //Check that we opened the file...
        if (fd < 0)
        {
            printf("login ERROR: no password file!\n");
            exit(1);
        }

        //Read 2048 bytes from the fd to the buf
        n = read(fd, buf, 1024);
        //Make sure last one is null
        buf[n] = 0;

        //printf("CONTENTS OF /etc/passwd: %s\n", buf);


        //We must now parse the password file
        /*
        6. if (user account valid){
        setuid to user uid.
        chdir to user HOME directory.
        exec to the program in users's account
    }
    */
    tok = strtok(buf, ":\n"); //Get first token
    while(tok != 0 && valid == 0) //While token is not NULL,
    {
        //If our input username is the same as one of our file usernames, proceed
        if (strcmp(tok, uName) == 0)
        {
            //At this point, we have a VALID user name
            strcpy(User, uName);
            //Now we need to see if the password the user entered is valid
            //Copy the password from the file
            strcpy(Password, strtok(NULL, ":\n"));
            // If our password is valid then we can successfully go into shell under this user
            //but first we need to set information for the user
            if (strcmp(Password, uPass) == 0)
            {
                //Our flag for if the password and username are correct
                valid = 1;
                // set uid to user uid.
                uid = atoi(strtok(NULL, ":\n"));
                // set gid to user gid
                gid = atoi(strtok(NULL, ":\n"));
                // get full name of user
                strcpy(uName, strtok(NULL, ":\n"));
                // get user home directory
                strcpy(Home, strtok(NULL, ":\n"));
                // chdir to user HOME dir
                chdir(Home);
                // exec to the program in the user's account
                exec("mysh one two three");
            }
            else
            {
                break;
            }
        }
        tok = strtok(NULL, ":\n");
    }
}
printf("login failed, try again\n");
close(fd);
exit(0);
}
