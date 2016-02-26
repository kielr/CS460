/********************************************************************
Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
/******************** timer.c file *************************************/
#define LATCH_COUNT     0x00	   /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	   /* ccaammmb, a = access, m = mode, b = BCD */

#define TIMER_FREQ   1193182L	   /* clock frequency for timer in PC and AT */
#define TIMER_COUNT  TIMER_FREQ/60 /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0

u16 tick, sec, min, hr;
u16 org;                  // current display origin r.e.VRAM base
char myClock[8];
int time = 10;

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

/*===================================================================*
 *		    timer interrupt handler       		     *
 *===================================================================*/
 int thandler()
 {
   u16 base     = 0xB800;    // VRAM base address
   u16 vid_mask = 0x3FFF;    // mask=Video RAM size - 1
   u16 offset = 0;
   u16 pos = 0;
   int w = 0;
   int i = 0;
   tick++;
   tick %= 60;
   if (tick == 0){                      // at each second
       /*printf("1 second timer interrupt in ");
       running->inkmode > 1 ? putc('K') : putc('U');
       printf("mode\n");*/
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

       for(i = 0; i < 8; i++)
       {
           pos = 2*(24*80 + (79-i));
           offset = (org + pos) & vid_mask;
           w = 0x0D; //Set color to purple
           w = (w << 8) + myClock[i];
           put_word(w, base, offset);
       }


   }

   out_byte(0x20, 0x20);                // tell 8259 PIC EOI
 }
