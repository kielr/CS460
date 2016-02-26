#include "ucode.c"

#define COLS 70

int in_fd, out_fd;

void quit(int code) {
    close(in_fd);
    close(out_fd);
    exit(code);
}

void convertToUppercase(char *buf) {
    char *ptr = buf;
    while(*ptr != '\0') {
        if (*ptr >= 97 && *ptr <= 122) {
            *ptr -= 32;
        }
        ptr++;
    }
    return;
}

/*
 *
 */
int main(int argc, char** argv) {
    char c;
    STAT out_stat;

    switch (argc) {
        case 1:
//            printf("Using STDIN to STDOUT\n");
            in_fd = dup(0);
            out_fd = dup(1);
            break;
        case 2:
//            printf("Using %s to STDOUT\n", argv[1]);
            in_fd = open(argv[1], 0);
            out_fd = dup(1);
            break;
        case 3:
//            printf("Using %s to %s\n", argv[1], argv[2]);
            in_fd = open(argv[1], 0);
            if (in_fd == -1) {
                fprintf(2, "Could not open input for reading\n");
                exit(0);
            }
            if (stat(argv[2], NULL) < 0) {
                unlink(argv[2]);
            }
            creat(argv[2]);
            out_fd = open(argv[2], 1);
            break;
        default:
            printf("Usage: l2u [infile [outfile]]\n");
            exit(0);
            break;
    }

    if (in_fd == -1) {
        fprintf(2, "Could not open input for reading\n");
        exit(0);
    }
    if (out_fd == -1) {
        fprintf(2, "Could not open output for writing\n");
        exit(0);
    }
    fstat(out_fd, &out_stat);

    while(read(in_fd, &c, 1)) {
        if (c >= 97 && c <= 122) {
            c -= 32;
        }
        //If we're a new line, just write out a return
        if ((c == '\n'))
        {
            write(out_fd, '\r', 1);
        }
        //then just write the character
        write(out_fd, c, 1);
    }
    close(in_fd);
    close(out_fd);
    exit(0);
}
