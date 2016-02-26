/******************** timer.c file *************************************/
#define LATCH_COUNT     0x00	   /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	   /* ccaammmb, a = access, m = mode, b = BCD */

#define TIMER_FREQ   1193182L	   /* clock frequency for timer in PC and AT */
#define TIMER_COUNT  TIMER_FREQ/60 /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0

u16 tick, sec, min, hr;
u16 org;

char myClock[8];

typedef struct tn{
       struct tn *next;
       int    time;
       PROC   *who;
} TNODE;

TNODE tnode[NPROC], *tq, *ft;

int enable_irq(u16 irq_nr)
{
  lock();
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));

}

int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */

  printf("timer init\n");
  tick = 0;
  out_byte(TIMER_MODE, SQUARE_WAVE);	// set timer to run continuously
  out_byte(TIMER0, TIMER_COUNT);	// timer count low byte
  out_byte(TIMER0, TIMER_COUNT >> 8);	// timer count high byte
  enable_irq(TIMER_IRQ);
}

/*==========================================================*
 *		    timer interrupt handler       		     *
 *==========================================================*/
int thandler()
{
  u16 base     = 0xB800;    // VRAM base address
  u16 vid_mask = 0x3FFF;    // mask=Video RAM size - 1
  u16 offset = 0;
  u16 pos = 0;
  int w = 0;
  int j = 0;
  int i; PROC *p; TNODE *tp;
  tick++;
  tick %= 60;
  if (tick == 0)
  {
      // at each second
      sec++;

      if(sec % 60 == 0)
      {
          sec = 0;
          min++;
      }
      if(min % 60 == 0 && min != 0)
      {
          min = 0;
          hr++;
      }
      myClock[0] = '0' + (sec%10);
      myClock[1] = '0' + (sec/10);
      myClock[2] = ':';
      myClock[3] = '0' + (min%10);
      myClock[4] = '0' + (min/10);
      myClock[5] = ':';
      myClock[6] = '0' + (hr%10);
      myClock[7] = '0' + (hr/10);

      for(j = 0; j < 8; j++)
      {
          pos = 2*(24*80 + (79-j));
          offset = (org + pos) & vid_mask;
          w = 0x0D; //Set color to purple
          w = (w << 8) + myClock[j];
          put_word(w, base, offset);
      }

      if (tq){ // do these only if tq not empty
          tp = tq;
          while (tp){
              tp->time--;
              printTQ();
              if (tp->time <= 0){ // wakeup sleepers
              printf("timer interrupt handler : wakeup %d\n", tp->who->pid);
                  kwakeup(&tp->who->time);
                  tq = tp->next;
              put_tnode(tp);
                  tp = tq;
               }
              else{
                    break;
              }
      }
      }


  }
  out_byte(0x20, 0x20);                // tell 8259 PIC EOI
}

TNODE *get_tnode()
{
    TNODE *tp;
    tp = ft;
    ft = ft->next;
    return tp;
}

int put_tnode(TNODE *tp)
{
    tp->next = ft;
    ft = tp;
}

int printTQ()
{
   TNODE *tp;
   tp = tq;
   printf("timerQueue = ");
   while(tp){
      printf(" [P%d: %d] ==> ", tp->who->pid, tp->time);
      tp = tp->next;
   }
   printf("\n");
}

itimer(int time)
{
    TNODE *t, *p, *q;
    int ps;

    // CR between clock and this process
    ps = int_off();
    t = get_tnode();
    t->time = time;
    t->who = running;
    /******** enter into tq ***********/
    if (tq==0){ //No nodes
        tq = t; //Add a node
        t->next = 0; //Set next to 0
    }
    else{ //We have a node
          q = p = tq; //Set up extra pointers
          while (p){ //While p is not equal to null
              if (time - p->time < 0)  //If our time to sleep is more than our p time
                  break;
              time -= p->time;
              q = p;
              p = p->next;
          }
          if (p){
              p->time -= time;
          }
          t->time = time;
          if (p==tq){
              t->next = tq;
              tq = t;
          }
          else{
                t->next = p;
                q->next = t;
          }
    }
    int_on(ps);
    printTQ();
    printf("PROC%d going to sleep\n", running->pid);
    ksleep(&running->time);  // sleep on its own time address
}
