#include "type.h"

/*********** EXT2 FS types for loader.c ******************/
typedef struct ext2_inode {
	u16	i_mode;		/* File mode */
	u16	i_uid;		/* Owner Uid */
	u32	i_size;		/* Size in bytes */
	u32	i_atime;	/* Access time */
	u32	i_ctime;	/* Creation time */
	u32	i_mtime;	/* Modification time */
	u32	i_dtime;	/* Deletion Time */
	u16	i_gid;		/* Group Id */
	u16	i_links_count;	/* Links count */
	u32	i_blocks;	/* Blocks count */
	u32	i_flags;	/* File flags */
        u32     dummy;
	u32	i_block[15];    /* Pointers to blocks */
        u32     pad[7];         /* inode size MUST be 128 bytes */
} INODE;
typedef struct ext2_group_desc
{
	u32	bg_block_bitmap;		/* Blocks bitmap block */
	u32	bg_inode_bitmap;		/* Inodes bitmap block */
	u32	bg_inode_table;		/* Inodes table block */
	u16	bg_free_blocks_count;	/* Free blocks count */
	u16	bg_free_inodes_count;	/* Free inodes count */
	u16	bg_used_dirs_count;	/* Directories count */
	u16	bg_pad;
	u32	bg_reserved[3];
} GD;

typedef struct ext2_dir_entry_2 {
	u32	inode;			/* Inode number */
	u16	rec_len;		/* Directory entry length */
	u8	name_len;		/* Name length */
	u8	file_type;
	char	name[255];      	/* File name */
} DIR;


#define BLK 1024
char b1[BLK], b2[BLK];

u16 tsize, dsize, bsize, tot;

typedef struct h{
  u32 sep, pad, tsize, dsize, bsize, zero, tot, symsize;
} HEADER;

u16 header(hp) HEADER *hp;
{ 
   tsize=hp->tsize; dsize=hp->dsize; bsize=hp->bsize; tot=hp->tot;
   /*******
   printf("[tsize  dsize  bsize] = [%d  %d  %d]\n",
                          tsize, dsize, bsize);
   ************/
}

u16 getblk(blk, buf) u16 blk; char *buf;
{
    diskr( blk/18, ((2*blk)%36)/18, (((2*blk)%36)%18), buf);
}

u16 search(ip, name) INODE *ip; char *name;
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

u16 nn;
char *name[32];  // at most 32 component names

int breakup(path) char *path;
{
  int i;
  char *cp;
  cp = path;
  nn = 0;
  
  while (*cp != 0){
       while (*cp == '/') *cp++ = 0;        
       if (*cp != 0)
           name[nn++] = cp;         
       while (*cp != '/' && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break; 
       cp++;
  }
  /*************
  printf("n = %d : ", nn);
  for (i=0; i<nn; i++)
       printf("  %s  ", name[i]);
  printf("\n");
  **************/

}

char path[32];

int load(filename, segment) char *filename; u16 segment;
{ 
  u16    i;
  u16    me, blk, iblk;
  char   *cp;
  u32    *up;
  INODE  *ip;
  GD     *gp;

  strcpy(path,filename);

  //printf("loader : tokenlize pathname\n");
  breakup(path);     // break up filename into nn string pointers
                     // in name[i]
  getblk(2, b1);
  gp = (GD *)b1;
  iblk = (u16)gp->bg_inode_table;
  //printf("iblk=%d\n", iblk);
  getblk(iblk, b1);  /* read first inode block block */
  ip = (INODE *)b1 +1;

  /* serach for system name */
  for (i=0; i<nn; i++){
      me = search(ip, name[i]);
      if (me == 0){
          printf("\ncan't find %s\n", name[i]); 
          return 0;    // for failure
      }
      me--;
      getblk(iblk+(me/8), b1);      /* read block inode of me */
      ip = (INODE *)b1 + (me % 8);
   }

  // get header information:
  getblk((u16)ip->i_block[0], b2);

  //printf("header information:\n");
  header(b2);

 /* read indirect block into b2, if any */
  if (ip->i_block[12])
      getblk((u16)ip->i_block[12], b2);

  printf("loading %s to segment %x ....", filename, segment);
  setes(segment);

  //printf("loading direct blocks : \n");
  for (i=0; i<12; i++){
      if (ip->i_block[i]==0) 
          break;
      //printf("loading i_block[%d] = %d\n", i, (u16)ip->i_block[i]);
      getblk((u16)ip->i_block[i], 0);
      //putc('.');
      inces();
  }

  // load indirect blocks if any
   if ( (u16)ip->i_block[12]){
     up = (u32 *)b2;
     while(*up){
       getblk((u16)*up, 0); //putc('.');
        inces();
        up++;
     }
   }
   
   //printf("move image upward 32 bytes\n");
   move(segment, tsize, dsize);

   //printf("clear bss to 0\n");
   clear_bss(segment, tsize, dsize, bsize);

   setes(0x1000);  // restore ES to MTX segment
   printf(" done\n");
   return(1);
}  

int move(segment, tsize, dsize) u16 segment, tsize, dsize;
{
  u16 i,w;
  
  for (i=0; i<=tsize+dsize; i+=2){
      w = get_word(segment+2, i);
      put_word(w, segment, i);
  }
}

int clear_bss(segment, tsize, dsize, bsize) 
u16 segment, tsize, dsize, bsize;
{
   u16 i,j, seg, tdsize, rem;

   tdsize = tsize + dsize;
   seg = segment + (tdsize)/16;

   rem = tdsize % 16;

   for (i=0; i<rem; i++)
     put_byte(0, seg, i);

   for (j=0; j<bsize; j++)
    put_byte(0, seg, j + i);
}

