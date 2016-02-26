#define PA 13
#define PB 14
#define PC 15
#define PD 16
#define AX  8

extern int color;

int kcinth()
{
   u16    segment, offset;
   int    a,b,c,d, r;

   segment = running->uss;
   offset = running->usp;

   a = get_word(segment, offset + 2*PA);
   b = get_word(segment, offset + 2*PB);
   c = get_word(segment, offset + 2*PC);
   d = get_word(segment, offset + 2*PD);

   switch(a){
       case 0 : r = running->pid;     break;
       case 1 : r = do_ps();          break;
       case 2 : r = chname(b);        break;
       case 3 : r = kmode();          break;
       case 4 : r = tswitch();        break;
       case 5 : r = do_wait(b);       break;
       case 6 : r = do_exit(b);       break;

       case 7 : r = fork();           break;
       case 8 : r = exec(b);          break;

       case 10: r = itimer(b);

       case 90: r =  getc();          break;
       case 91: color=running->pid+11;
                r =  putc(b);         break;
       case 99: do_exit(b);           break;
       default: printf("invalid syscall # : %d\n", a);
   }
   put_word(r, segment, offset + 2*AX);
}
