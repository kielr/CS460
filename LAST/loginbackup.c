#include "ucode.c"
// seems stdin,stdout,stderr are keywords in bcc
// int stdin, stdout, stderr; ==> auto_start symbol
// login : call by init as "login tty0" | "login ttyS0" | ...
// Upon entry, name[0]=filename, name[1]=tty string

int main(int argc, char *argv[ ])
{
    char buf[1024], uName[64], uPass[64];
    char pLine[64], tty[64], *pName[8];
    char *initialCurrentLine, *initialCurrentCharacter, *currentLine, *currentCharacter;
    int gid, uid, i, n;
    int in, out, err;
    int fd;

    close(0); close(1); close(2);
    strcpy(tty, argv[1]);

    in  = open(tty, 0);
    out = open(tty, 1);
    err = open(tty, 2);

    fixtty(tty);  // register tty string in PROC.tty[]

    printf("Kiel Login : open %s as stdin, stdout, stderr\n", tty);

    while(1){
        //Ask for information from user
        print2f("kiel-login:");     //1. show login: to stdout
        gets(uName);              //2. read user name from stdin
        print2f("password:");   //3. show passwd to stdout
        gets(uPass);               //4. read password from stdin

        ///Open /etc/passwd file to get uName's uid, gid, home, program
        fd = open("/etc/passwd", 0);

        //Check that we opened the file...
        if (fd < 0)
        {
            printf("ERROR: no password file!\n");
            exit(1);
        }

        //Read 2048 bytes from the fd to the buf
        n = read(fd, buf, 1024);
        //Make sure last one is null
        buf[n] = 0;

        printf("CONTENTS OF /etc/passwd: %s\n", buf);


        //We must now parse the password file
        initialCurrentLine = initialCurrentCharacter = buf; //Set to beginning of buf
        while(initialCurrentLine < &buf[n]){ //Until the end of the buff
            while (*initialCurrentCharacter != '\n')//End if we find a new line character..
            {
                if (*initialCurrentCharacter == ' ')
                    *initialCurrentCharacter = '-';//Replace ' ' with ';'
                if (*initialCurrentCharacter == ':')
                    *initialCurrentCharacter = ' '; //Replace ':' with ' '

                initialCurrentCharacter++; //Advance current character...
            }

            *initialCurrentCharacter = 0; //Reset 'initial current character'
            strcpy(pLine, initialCurrentLine); //Set our pLine to our newly converted line
            currentLine = currentCharacter = pLine; //Set current line and current character to pline
            i = 0; //Set up an index

            while (*currentCharacter) //While currentCharacter is not null...
            {
                if (*currentCharacter == ' ') //If we reach a space, set
                {
                    *currentCharacter = 0; //Set the current character to null
                    pName[i] = currentLine; //Fill the array of strings with currentLine
                    i++; //Increment
                    currentCharacter++; //Go to the next character...
                    currentLine = currentCharacter; //We are now at the next line
                    continue;
                }
                currentCharacter++; //We didn't find a space, go to the next one.
            }

            /*
            6. if (user account valid){
            setuid to user uid.
            chdir to user HOME directory.
            exec to the program in users's account
            }
            */
            if (strcmp(uName, pName[0])==0 && strcmp(uPass, pName[1])==0){

                printf("KIELLOGIN: Welcome! %s\n",uName);
                printf("KIELLOGIN: cd to home=%s  ",pName[5]);
                gid = atoi(pName[2]); uid = atoi(pName[3]);

                chdir(pName[5]);  // cd to pName[5]
                printf("change uid to %d",uid);

                chuid(uid, 0);        // change gid, uid
                printf("exec to /bin/sh .....\n");

                close(fd);
                exec("sh one two three");
            }
            initialCurrentCharacter++; initialCurrentLine = initialCurrentCharacter;
        }
        printf("login failed, try again\n");
        close(fd);
    }
}
