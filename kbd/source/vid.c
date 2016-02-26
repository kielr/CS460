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
/************************************************************************
                 HOW does PC's color display work:
 0. Before vid_init(), MUST set to mono mode, otherwise, no display at all.

 1. Screen = 25 rows X 80 cols = 2000 WORDs = [attribue byte|char byte]
                                 4000 bytes 
 2. Video RAM at 0xB800, size=16 to 64 KB >> screen=4000 bytes;
    To display a screen: write 4000 bytes (2000 WORDs) to the video RAM
    at the ORG location; tell the display register where the ORG is.
    The display hardware will display the 4000 chars from ORG to the screen.

 3. Cursor shape and position can be changed by writing to the CRT register.

 4. Software must keep track of the Cursor position (row,col). For each char
    displayed, must advance the Curcor by one word, which may change(row,col): 
               When col % 80  which may inc row,
               When row > 24, must scroll up one row.

5.  To Scroll up one row:
       inc org by one row (tell Hardware to display from the new org position) 
       write a row of blanks to ORG + one screen  in video RAM 
       if the last row exceeds VRAM size, must physically copy CURRENT screen
       to VRAM beginning and reset org to 0
*****************************************************************/
#define VDC_INDEX      0x3D4
#define VDC_DATA       0x3D5
#define CUR_SIZE          10	/* cursor size register */
#define VID_ORG           12	/* start address register */
#define CURSOR            14	/* cursor position register */

#define LINE_WIDTH        80	/* # characters on a line */
#define SCR_LINES         25	/* # lines on the screen */
#define SCR_BYTES	4000	/* bytes of ONE screen=25*80 */

#define CURSOR_SHAPE      15    /* block cursor for MDA/HGC/CGA/EGA/VGA... */

// attribute byte: 0x0HRGB, H=highLight; RGB determine color
#define HGREEN          0x0A
#define HCYAN           0x0B
#define HRED            0x0C
#define HPURPLE         0x0D
#define HYELLOW         0x0E

u16 base     = 0xB800;    // VRAM base address
u16 vid_mask = 0x3FFF;    // mask=Video RAM size - 1

u16 offset;               // offset from VRAM base
extern int color;                // atttirbute byte
u16 org;                  // current display origin r.e.VRAM base
u16 row, column;          // logical row, col position


// vid_init() initializes the display org=0 (row,col)=(0,0)

int vid_init()
{ 
  int i, w;
  org = row = column = 0;
  color = HCYAN; //HGREEN;

  set_VDC(CUR_SIZE, CURSOR_SHAPE);   // set cursor size	
  set_VDC(VID_ORG, 0);               // display origin to 0
  set_VDC(CURSOR, 0);	             // set cursor position to 0

  // clear screen
  w = 0x0700;    // White, blank char // attribute byte=0000 0111=0000 0RGB
  for (i=0; i<25*80; i++){                               
    put_word(w, base, 0+2*i);         // write 24*80 blanks to VGA memory
  }
}

/*************************************************************************
 scroll(): scroll UP one line
**************************************************************************/
int scroll()
{
  int i;
  u16 w, bytes;  

  // Normal scrolling using the 6845 registers.

  // try offset = org + ONE screen + ONE more line
  offset = org + SCR_BYTES + 2*LINE_WIDTH;

  if (offset <= vid_mask){   // offset still within vram area
    org += 2*LINE_WIDTH;     // just advance org by ONE line
  }
  else{  // offset exceeds vram area ==> reset to vram beginning by    
         // copy current rows 1-24 to BASE, then reset org to 0

    for (i=0; i<24*80; i++){
      w = get_word(base, org+160+2*i);
      put_word(w, base, 0+2*i);
    }  
    org = 0;
  }

  // org has been set up correctly
  offset = org + 2*24*80;   // offset = beginning of row 24

  // copy a line of BLANKs to row 24

  w = 0x0C00;  // HRGB=1100 ==> HighLight RED, Null char

  for (i=0; i<80; i++)                  
    put_word(w, base, offset + 2*i);

  set_VDC(VID_ORG, org >> 1);	  /// 6845 thinks in words 

}

int putc(char c)
{
  // display c to cursor position, advance cursor

  int pos, w, offset;

  if (c=='\n'){
    row++;
    if (row>=25){
      row = 24;
      scroll();
    }

    pos = 2*(row*80 + column);
    offset = (org + pos) & vid_mask;
    set_VDC(CURSOR, offset >> 1);
    return; 
  }

  if (c=='\r'){
    column=0;

    pos = 2*(row*80 + column);
    offset = (org + pos) & vid_mask;
    set_VDC(CURSOR, offset >> 1);
    return;
  }

  if (c=='\b'){
    if (column > 0){
      column--;

      pos = 2*(row*80 + column);
      offset = (org + pos) & vid_mask;
      put_word(0x0700, base, offset);

      set_VDC(CURSOR, offset >> 1); 
    }
    return;
  }

  pos = 2*(row*80 + column);  
  offset = (org + pos) & vid_mask;
  w = color;
  //w = color + (running->pid);
  w = (w << 8) + c;

  put_word(w, base, offset);

  column++;

  if (column >= 80){
    column = 0;
    row++;

    if (row>=25){
      row = 24;
      scroll();
    }
  }

  pos = 2*(row*80 + column);
  offset = (org + pos) & vid_mask;
  set_VDC(CURSOR, offset >> 1);
}     

/*===========================================================================*
 *				set_6845				     *
 *===========================================================================*/
int set_VDC(u16 reg, u16 val)
{
/* Set a register pair inside the 6845.  
 * Registers 10-11 control the format of the cursor (how high it is, etc).
 * Registers 12-13 tell the 6845 where in video ram to start (in WORDS)
 * Registers 14-15 tell the 6845 where to put the cursor (in WORDS)
 *
 * Note that registers 12-15 work in words, i.e. 0x0000 is the top left
 * character, but 0x0001 (not 0x0002) is the next character.  This addressing
 * is different from the way the 8088 addresses the video ram, where 0x0002
 * is the address of the next character.
 */
  lock();			/* try to stop h/w loading in-between value */
  out_byte(VDC_INDEX, reg);	/* set the index register */
  out_byte(VDC_DATA,  (val>>8) & 0xFF);	/* output high byte */
  out_byte(VDC_INDEX, reg + 1);	/* again */
  out_byte(VDC_DATA,  val&0xFF);	/* output low byte */
  unlock();
}

// change color syscall from Umode, just for fun
int chcolor(u16 y)
{
  y &= 0x7F;
  switch(y){
    case 'r' : color=HRED;    break;
    case 'y' : color=HYELLOW; break;
    case 'c' : color=HCYAN;   break;
    case 'g' : color=HGREEN;  break;
  }
}

int ktest()
{
  int y,i,w,pos;
  while(1){
    printf("enter color [r|b|g|y|c|p] : ");
    y = getc() & 0x7F;
    switch(y){
      case 'r' : color=HRED;    break;
      case 'y' : color=HYELLOW; break;
      case 'c' : color=HCYAN;   break;
      case 'g' : color=HGREEN;  break;
      case 'p' : color=HPURPLE; break;
    }
    w = (color << 8) + y;
    for (i=0; i<25*80; i++){                               
       put_word(w, base, 0+2*i);        // write 24*80 blanks to VGA memory
    } 
    org = 0;
    set_VDC(VID_ORG, org >> 1);	  /// 6845 thinks in words 
    pos = 2*(25*80 + 80) + 2;
    offset = (org + pos) & vid_mask;

    row = 24; column=0;
    set_VDC(CURSOR, offset >> 1);
    if (y=='q') break;
  }
}
