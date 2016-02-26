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
#include "../type.h"

#define BLK 1024
char b1[BLK], b2[BLK];

u16 getblk(u16 blk, char *buf)
{
    diskr( blk/18, ((2*blk)%36)/18, (((2*blk)%36)%18), buf);
}

int showdir(char *buf)
{
  int i; char c, *cp;
   DIR  *dp; 
   cp = buf + 24;
   dp = (DIR *)cp;
   printf("================== commands in /bin ==================\n");
   while ((char *)dp < &buf[BLK]){
        c = dp->name[dp->name_len];  // save last byte
        dp->name[dp->name_len] = 0;
        printf("%s ", dp->name); 
        dp->name[dp->name_len] = c; // restore that last byte
        dp = (char *)dp + dp->rec_len;
   }
   printf("\n======================================================\n");
   
}

u16 search(INODE *ip, char *name)
{
   int i; char c;
   DIR  *dp; 
   for (i=0; i<12; i++){
       if ( (u16)ip->i_block[i] ){
          getblk((u16)ip->i_block[i], b2);
          dp = (DIR *)b2;

          while ((char *)dp < &b2[BLK]){
              c = dp->name[dp->name_len];  // save last byte
              dp->name[dp->name_len] = 0;   
	      //printf("%s ", dp->name); 
              if ( strcmp(dp->name, name) == 0 ){
		// printf("\n"); 
                 return((u16)dp->inode);
              }
              dp->name[dp->name_len] = c; // restore that last byte
              dp = (char *)dp + dp->rec_len;
	}
     }
   }
   return 0;
}



int show()
{ 
  u16    i;
  u16    me, blk, iblk;
  char   *cp;
  u32    *up;
  INODE  *ip;
  GD     *gp;

  getblk(2, b1);
  gp = (GD *)b1;
  iblk = (u16)gp->bg_inode_table;
  //printf("iblk=%d\n", iblk);

  getblk(iblk, b1);  /* read first inode block block */
  ip = (INODE *)b1 +1;

  /* serach for /bin */
  me = search(ip, "bin");
  if (me == 0){
      printf("\ncan't find bin\n"); 
          return(0);
  }
  me--;
  getblk(iblk+(me/8), b1);      /* read block inode of me */
  ip = (INODE *)b1 + (me % 8);

  // get header information:
  for (i=0; i<12; i++){
    if (ip->i_block[i]==0)
      break;

    getblk((u16)ip->i_block[i], b2);
    showdir(b2);
  }
   return(1);
}  
/* Use a single sh.c to replace u1.c u2.c
   keep ucode.c as a syscall interface
   Re-write u1.c u2.c as one sh.c, which
      show menu() ==> /bin dir contents
      then ask for a command line, and then for(), exec()
**********************************************/
char buf[1024];
color = 0x00C;
#include "ucode.c"

main(int argc, char *argv[])
{
  int pid, status;
  char *cp;
  
  color = getpid()+0x000A;
  printf("sh %d running\n", getpid()); 
  while(1){
     show();
     printf("enter command (cmd in /bin | chname | exit): ");
     gets(buf);
     if (buf[0]==0) 
       continue;

     printf("input=%s\n", buf);

     cp = buf;
     while(*cp && *cp != ' ')
         cp++;
     *cp = 0;
     if (strcmp(buf,"chname")==0){
       cp++;
       while(*cp && *cp==' ')
	 cp++;
       //printf("sh chname to %s\n", cp); 
       chname(cp); 
       continue;
     }
     if (strcmp(buf, "exit")==0)
       exit(0);
     
     printf("Parent sh %d fork a child\n", getpid());
     pid = fork(); /* sh forks child */

     if (pid){ /* parent sh */
        printf("parent sh %d waits\n", getpid());
        pid = wait(&status);
     }
     else{
        printf("child  sh %d running\n", getpid());
        *cp = ' ';
        exec(buf);
     }
  }
}

