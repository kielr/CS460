#define PA 9
#define PB 10
#define PC 11
#define PD 12
#define AX  8

/****************** syscall handler in C ***************************/
int kcinth()
{
    u16    segment, offset;
    int    a,b,c,d, r;
    segment = running->uss;
    offset = running->usp;

    /** get syscall parameters from ustack **/
    a = get_word(segment, offset + 2*PA);
    b = get_word(segment, offset + 2*PB);
    c = get_word(segment, offset + 2*PC);
    d = get_word(segment, offset + 2*PD);

    switch(a)
    {
    case 0 :
        r = running->pid;
        break;
    case 1 :
        r = do_ps();
        break;
    case 2 :
        r = chname(b);
        break;
    case 3 :
        r = kkfork();
        break;
    case 4 :
        r = tswitch();
        break;
    case 5 :
        r = do_wait(b);
        break;
    case 6 :
        r = do_exit(b);
        break;

    case 7:
        r =  getc();
        break;
    case 8:
        color=running->pid+11;
        r =  putc(b);
        break;
    case 99:
        do_exit(b);
        break;
    default:
        printf("invalid syscall # : %d\n", a);
    }
    //put_word(r, segment, offset + 2*AX);
    return r;
}
