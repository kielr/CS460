/************************  6-3 : KCW **********************************
                           kbd.c file
***********************************************************************/
#define KEYBD	         0x60	/* I/O port for keyboard data */
#define PORT_B           0x61   /* port_B of 8255 */
#define KBIT	         0x80	/* bit used to ack characters to keyboard */

#define NSCAN             64	/* Number of scan codes */
#define KBLEN             64

#define BELL             0x07
#define F1		 0x3B
#define F2		 0x3C
#define F3       0x3D

#define CAPSLOCK         0x3A
#define LSHIFT           0x2A
#define RSHIFT           0x36
#define CONTROL          0x1D
#define ALT              0x38
#define DEL              0x53

#include "pv.c"
#include "keymap.c"

#define KBLEN  64

typedef struct kbd{
    char buf[KBLEN];
    int  head, tail;
    SEMAPHORE data;
}KBD;

KBD kbd;
int shifted;
int caps;

int kbd_init()
{
  printf("kbinit()\n");
  kbd.head = kbd.tail = 0;
  kbd.data.value = 0;
  kbd.data.queue = 0;
  printf("kbinit done\n");
}

/************************************************************************
 kbhandler() is the kbd interrupt handler. The kbd generates 2 interrupts
 for each key typed; one when the key is pressed and another one when the
 key is released. Each key generates a scan code. The scan code of a key
 release is 0x80 + the scan code of key pressed. When the kbd interrupts,
 the scan code is in the data port (0x60) of the KBD interface. First,
 read the scan code from the data port. Then ack the key input by strobing
 its PORT_B at 0x61.
 Some special keys generate ESC key sequences,e.g. arrow keys
                   Then process the scan code:
1. Normal key releases are ignored except for the spcecial keys of
   0xE0, CAPLOCK, CONTROL, ALT, SHIFTs, which are used to set or clear
   the status variables  esc, control, alt, shift
2. For normal keys: translate into ASCII, depending on shifted or not.
3. ASCII keys are entered into a circular input buffer.
4. Sync between upper routines and lower routines is by P/V on semaphores
   kbData, kbline
5. The input buffer contains RAW keys (may have \b). kgets()/ugets() cooks
   the keys to weed out specail keys such as \b. So far only \b is handled.
   Arrow keys are used only by sh for history. Each arrow key is immediately
   made into a line to allow sh to get a line of inputs.
**************************************************************************/
int get_scode()
{
  int scode, value;
  /* Fetch the character from the keyboard hardware and acknowledge it. */
  scode = in_byte(KEYBD);	/* get the scan code of the key struck */
  value = in_byte(PORT_B);	/* strobe the keyboard to ack the char */
  out_byte(PORT_B, value | KBIT);	/* first, strobe the bit high */
  out_byte(PORT_B, value);	/* then strobe it low */
  return scode;
}


int kbhandler()
{
    int scode, value, c;
    KBD *p = &kbd;

    /* Fetch scan code from the keyboard hardware and acknowledge it. */
  scode = in_byte(KEYBD);	 /* get the scan code of the key */
  value = in_byte(PORT_B);	 /* strobe the keyboard to ack the char */
  out_byte(PORT_B, value | KBIT);/* first, strobe the bit high */
  out_byte(PORT_B, value);	 /* then strobe it low */

    //printf("scode=%x\n", scode);

    if (scode & 0x80){ // key release:
        if (scode == 0xAA || scode == 0xB6)
          shifted = 0;    // released the shift key
       goto out;
    }

    // translate scan code to ASCII, using shift[ ] table if shifted;
    if(scode == LSHIFT || scode == RSHIFT)
    {
        shifted = 1;
        goto out;
    }
    if(scode == 0x3A)
    {
        if(caps == 1)
        {
            caps = 0;
        }
        else
        {
            caps = 1;
        }
        printf("caps=%d", caps);
    }
    if (scode == F1){
      printf("Function Key F1\n");
      do_ps();
      goto out;
    }
    if (scode == F2){
      printf("Function Key F2\n");
      printList();
      goto out;
    }
    if (scode == F3){
      printf("Function Key F3\n");
      goto out;
    }

    //c = (shifted ? shift[scode] : unshift[scode]);
    // translate scan code to ASCII, using shift[ ] table if shifted;
    if(caps == 1)
    {
        if(shifted == 0)
        {
            c = shift[scode];
        }
        else
        {
            c = unshift[scode];
        }
    }
    else
    {
        if(shifted == 1)
        {
            c = shift[scode];
        }
        else
        {
            c = unshift[scode];
        }
    }

    p->buf[p->head++] = c;
    p->head %= KBLEN;

    V(&kbd.data);


  out:
    out_byte(0x20, 0x20);
}

/********************** upper half rotuine ***********************/
int getc()
{
  int c;
  P(&kbd.data);

  lock();
    c = kbd.buf[kbd.tail++] & 0x7F;
    kbd.tail %= KBLEN;
  unlock();

  return c;
}

/************************* KBD scan code info *****************************

US 104-key keyboard, set 1 scancodes

"Make" code is generated when key is pressed.
"Break" code is generated when key is released.
Hex value of make code for each key is shown.

Most keys:
	one-byte make code	= nn
	one-byte repeat code	= nn
	one-byte break code	= 80h + nn

"Gray" keys (not on original 84-key keyboard):
	two-byte make code	= E0nn
	two-byte repeat code	= E0nn
	two-byte break code	= E0 followed by 80h + nn

"Gray" keys noted by [1] are NumLock-sensitive.
When the keyboard's internal NumLock is active:
	four-byte make code	= E02AE0nn
	two-byte repeat code	= E0nn
	four-byte break code	= E0 followed by 80h + nn followed by E0AA

 ____    ___________________    ___________________    ___________________
|    |  |    |    |    |    |  |    |    |    |    |  |    |    |    |    |
|Esc |  |F1  |F2  |F3  |F4  |  |F5  |F6  |F7  |F8  |  |F9  |F10 |F11 |F12 |
|    |  |    |    |    |    |  |    |    |    |    |  |    |    |    |    |
|  01|  |  3B|  3C|  3D|  3E|  |  3F|  40|  41|  42|  |  43|  44|  57|  58|
|____|  |____|____|____|____|  |____|____|____|____|  |____|____|____|____|

 __________________________________________________________________________
|    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
|~   |!   |@   |#   |$   |%   |^   |&   |*   |(   |)   |_   |+   ||   |bksp|
|`   |1   |2   |3   |4   |5   |6   |7   |8   |9   |0   |-   |=   |\   |    |
|  29|  02|  03|  04|  05|  06|  07|  08|  09|  0A|  0B|  0C|  0D|  2B|  0E|
|____|____|____|____|____|____|____|____|____|____|____|____|____|____|____|
|    |    |    |    |    |    |    |    |    |    |    |    |    |         |
|Tab |Q   |W   |E   |R   |T   |Y   |U   |I   |O   |P   |{   |}   |         |
|    |    |    |    |    |    |    |    |    |    |    |[   |]   |         |
|  0F|  10|  11|  12|  13|  14|  15|  16|  17|  18|  19|  1A|  1B|         |
|____|____|____|____|____|____|____|____|____|____|____|____|____|         |
|    |    |    |    |    |    |    |    |    |    |    |    |              |
|Caps|A   |S   |D   |F   |G   |H   |J   |K   |L   |:   |"   |     Enter    |
|    |    |    |    |    |    |    |    |    |    |;   |'   |              |
|  3A|  1E|  1F|  20|  21|  22|  23|  24|  25|  26|  27|  28|            1C|
|____|____|____|____|____|____|____|____|____|____|____|____|______________|
|         |    |    |    |    |    |    |    |    |    |    |              |
| L Shift |Z   |X   |C   |V   |B   |N   |M   |<   |>   |?   |   R Shift    |
|         |    |    |    |    |    |    |    |,   |.   |/   |              |
|       2A|  2C|  2D|  2E|  2F|  30|  31|  32|  33|  34|  35|            36|
|_________|____|____|____|____|____|____|____|____|____|____|______________|
|       |       |       |                  |       |       |       |       |
|L Ctrl | L win | L Alt |       space      | R Alt | R win | menu  |R Ctrl |
|       |[1]    |       |                  |       |[1]    |[1]    |       |
|     1D|   E05B|     38|                39|   E038|   E05C|   E05D|   E01D|
|_______|_______|_______|__________________|_______|_______|_______|_______|


[2] For PrintScreen/SysRq key: make code = E02AE037,
repeat code = E037, break code = E0B7E0AA

[3] The Pause/Break key does not repeat, and it does not
generate a break code. Its make code is E11D45E19DC5

 ____ ____ ____
|    |    |    |
|Prt |Scrl|Paus|
|Scrn|Lock|Brk |
| [2]|  46| [3]|
|____|____|____|

 ____ ____ ____   ____ ____ ____ ____
|    |    |    | |    |    |    |    |
|Ins |Home|PgUp| |Num |/   |*   |-   |
|[1] |[1] |[1] | |Lock|    |    |    |
|E052|E047|E049| |  45|E035|  37|  4A|
|____|____|____| |____|____|____|____|
|    |    |    | |    |    |    |    |
|Del |End |PgDn| |7   |8   |9   |    |
|[1] |[1] |[1] | |Home|(U) |PgUp|    |
|E053|E04F|E051| |  47|  48|  49|    |
|____|____|____| |____|____|____|    |
		 |    |    |    |+   |
		 |4   |5   |6   |    |
		 |(L) |    |(R) |    |
		 |  4B|  4C|  4D|  4E|
      ____       |____|____|____|____|
     |    |      |    |    |    |    |
     |(U) |      |1   |2   |3   |    |
     |[1] |      |End |(D) |PgDn|    |
     |E048|      |  4F|  50|  51|Ent |
 ____|____|____  |____|____|____|    |
|    |    |    | |         |    |    |
|(L) |(D) |(R) | |0        |.   |    |
|[1] |[1] |[1] | |Ins      |Del |    |
|E04B|E050|E04D| |       52|  53|E01C|
|____|____|____| |_________|____|____|


code key        code key        code key        code key
---- ---        ---- ---        ---- ---        ---- ---
01   Esc        0F   Tab        1D   L Ctrl     2B   \|
02   1!         10   Q          1E   A          2C   Z
03   2"         11   W          1F   S          2D   X
04   3#         12   E          20   D          2E   C
05   4$         13   R          21   F          2F   V
06   5%         14   T          22   G          30   B
07   6^         15   Y          23   H          31   N
08   7&         16   U          24   J          32   M
09   8*         17   I          25   K          33   ,<
0A   9(         18   O          26   L          34   .>
0B   0)         19   P          27   ;:         35   /?
0C   -_         1A   [{         28   '"         36   R Shift
0D   =+         1B   ]}         29   `~         37   *
0E   BackSpace  1C   Enter      2A   L Shift    38   L Alt

code key        code key        code key        code key
---- ---        ---- ---        ---- ---        ---- ---
39   Space      41   F7         49   PageUp 9   51   PageDown 3
3A   CapsLock   42   F8         4A   -          52   Insert 0
3B   F1         43   F9         4B   (left) 4   53   Del .
3C   F2         44   F10        4C   5
3D   F3         45   NumLock    4D   (right) 6  57   F11
3E   F4         46   ScrollLock 4E   +          58   F12
3F   F5         47   Home  7    4F   End 1
40   F6         48   (up) 8     50   (down) 2


code            key
----            ---
E01C            Enter (on numeric keypad)
E01D            R Ctrl
E02A            make code prefix for keyboard internal numlock
E02AE037        PrintScreen make code
E035            /
E037            PrintScreen repeat code
E038            R Alt
E047            Home
E048            (up)
E049            PageUp
E04B            (left)
E04D            (right)
E04F            End
E050            (down)
E051            PageDown
E052            Insert
E053            Del
E05B            L Win
E05C            R Win
E05D            Menu
E0AA            break code suffix for keyboard internal numlock
E0B7E0AA        PrintScreen break code
E11D45E19DC5    Pause
*****************************************************************************/
