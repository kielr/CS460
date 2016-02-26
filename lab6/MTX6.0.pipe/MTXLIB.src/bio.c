typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define  GREEN  10
#define  CYAN   11
#define  RED    12
#define  MAG    13
#define  YELLOW 14

u16 ALIGN = 0;
u16 color = CYAN;

int align(u32 x)
{
  int digits, count, i;
  digits = 0;
  while (x){
    digits++;
    x = x /10;
  }                 
  count = 10-digits;
  while(count>0){
    putc(' ');
    count--;
  }
}

char *tab="0123456789ABCDEF";
int  BASE = 10;

int rpl(u32 x)
{
  char c;
  if (x==0)
    return;
  c = tab[x % BASE];
  rpl(x / BASE);
  putc(c);
}

int printl(u32 x)
{
  if (ALIGN)    // global 0 => no alignment
      align(x);
  if (x==0){
    putc('0'); putc(' ');
    return;
  }
  rpl(x);
  putc(' ');
}

int rpu(u16 x)
{
  char c;

  if (x){
     c = tab[x % BASE]; 
     rpu(x / BASE);
     putc(c);
  } 
}

int printu(u16 x)
{

  (x) ? rpu(x) : putc('0');
  putc(' ');
}

int printd(int x)
{
  if (x<0){
    putc('-');
    x = -x;
  }
  printu(x);
}

int printx(u16 x)
{
  BASE = 16;
  printu(x);
  BASE = 10;
}

int printX(u32 x)
{
  BASE = 16;
  printl(x);
  BASE = 10;
}

int prints(char *s)
{
   while(*s){
       putc(*s);
       s++;
   }
}

int printf(char *fmt)
{
  char   *cp;
  u16    *ip;
  u32    *up;
 
  cp = fmt;
  ip = (int *)&fmt + 1;

  while (*cp){
    if (*cp != '%'){
      putc(*cp);
      if (*cp=='\n')
	putc('\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
      case 'c' : putc  (*ip); break;
      case 's' : prints(*ip); break;
      case 'u' : printu(*ip); break;
      case 'd' : printd(*ip); break;
      case 'x' : printx(*ip); break;
      case 'l' : printl(*(u32 *)ip++); break;  // print long value in decimal
      case 'X' : printX(*(u32 *)ip++); break;  // print long value in hex
    }
    cp++; ip++;
  }
}

#define LEN 64
int gets(char *s)
{ 
  char c, *b; int n = 0;
  b = s;
  while ( (c=getc()) != '\r' && n < LEN){
      if (c=='\b'){
         if (s==b)
            continue;
         s--;
         putc('\b'); putc(' '); putc('\b');
         continue;
      }   
      *s = c;   
      putc(*s); s++; n++;
  }
  *s = 0;
   prints("\n\r");
}


#define OSSEG 0x1000
int get_word(u16 segment, u16 offset)
{
  u16 word;
  setds(segment);
  word = *(u16 *)offset;
  setds(OSSEG);
  return word;
}

u8 get_byte(u16 segment, u16 offset)
{
  u8 byte;
  setds(segment);
  byte = *(u8 *)offset;
  setds(OSSEG);
  return byte;
}

int put_byte(u8 byte, u16 segment, u16 offset)
{
  setds(segment);
  *(u8 *)offset = byte;
  setds(OSSEG);
  return byte;
}

int put_word(u16 word, u16 segment, u16 offset)
{
  setds(segment);
  *(u16 *)offset = word;
  setds(OSSEG);
}

