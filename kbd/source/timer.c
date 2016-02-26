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
#define LATCH_COUNT     0x00	   /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	   /* ccaammmb, a = access, m = mode, b = BCD */

#define TIMER_FREQ   1193182L	   /* clock frequency for timer in PC and AT */
#define TIMER_COUNT  TIMER_FREQ/60 /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0

u16 tick, sec, min, hr;           // wall clock hh:mm:hh

int enable_irq(u16 irq_nr)
{
  lock();
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));

}

int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */

  printf("timer init\n");
  tick = sec = min = hr = 0; 
  out_byte(TIMER_MODE, SQUARE_WAVE);	/* set timer to run continuously */
  out_byte(TIMER0, TIMER_COUNT);	/* load timer low byte */
  out_byte(TIMER0, TIMER_COUNT >> 8);	/* load timer high byte */
  enable_irq(TIMER_IRQ); 
}

/*===========================================================================*
 *		    timer interrupt handler                 		     *
 *===========================================================================*/
char *clock = "00:00:ss";
//             01234567

int wall_clock()
{
   int i, w; 
   // every second: display wall clock, write to VGA memory directly
   clock[7] = '0'+ (sec % 10); clock[6]='0'+ sec/10;
   clock[4] = '0'+ (min % 10); clock[3]='0'+ min/10;
   clock[1] = '0'+ (hr % 10);  clock[0]='0'+ hr/10;

   for (i=0; i<8; i++){
        w = 0x0C00 + clock[i];    // attribute = RED color
        put_word(w, 0xB800, org + 2*(23*80 + 70+i) );
   }
}

int thandler()
{
  tick++; tick %= 60;

  if (tick == 0){         // every second: update sec, mm, hh as needed  
       sec++; sec %= 60;
       if (sec == 0){
           min++; min %= 60;
           if (min  == 0){
              hr++;
           }
       }
       wall_clock();
  }

  out_byte(0x20, 0x20);        // re-enable the 8259 controller  
}
