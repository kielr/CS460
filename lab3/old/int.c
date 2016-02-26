/****** MTX5.0 syscall handler: ustack layout in syscall ************
usp 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
--|--------------------------------------------------------------------
|uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|rPC| a | b | c | d |
---------------------------------------------------------------------*/
#define AX 8
#define PA 9
int kcinth()
{
    u16 segment, offset;
    int a, b, c, d, r;
    segment = running->uss;
    offset = running->usp;
    /* get syscall parameters from ustack */
    a = get_word(segment, offset + 2*PA);
    b = get_word(segment, offset + 2*(PA+1));
    c = get_word(segment, offset + 2*(PA+2));
    d = get_word(segment, offset + 2*(PA+3));
    /* route syscall call to kernel functions by call# a */

    switch(a)
    {
        case 0 :
            r = getpid();
            break;
        case 1 :
            r = do_ps();
            break;
        case 2 :
            r = chname(b);
            break;
        case 3 :
            r = kfork();
            break;
        case 4 :
            r = tswitch();
            break;
        case 5 :
            r = kwait(b);
            break;
        case 6 :
            kexit(b);
            break;
        case 7 :
            getc(b);
        case 8 :
            putc(b);
        default:
            printf("invalid syscall %d\n", a);
    }
    //put_word(r, segment, offset + 2*AX); // return value in uax

    return r;
}
