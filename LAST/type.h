#define NULL       0
#define NPROC     17
#define NTHREAD   16
#define SSIZE    512
#define NSIG      16

#define FREE       0   /* PROC status */
#define READY      1
#define SLEEP      2
#define ZOMBIE     3
#define BLOCK      4
#define PAUSE      5

#define BLOCK_SIZE 1024

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef unsigned short ushort;
typedef unsigned long  ulong;

typedef struct ext2_super_block {
	u32	s_inodes_count;		/* Inodes count */
	u32	s_blocks_count;		/* Blocks count */
	u32	s_r_blocks_count;	/* Reserved blocks count */
	u32	s_free_blocks_count;	/* Free blocks count */
	u32	s_free_inodes_count;	/* Free inodes count */
	u32	s_first_data_block;	/* First Data Block */
	u32	s_log_block_size;	/* Block size */
	u32	s_log_frag_size;	/* Fragment size */
	u32	s_blocks_per_group;	/* # Blocks per group */
	u32	s_frags_per_group;	/* # Fragments per group */
	u32	s_inodes_per_group;	/* # Inodes per group */
	u32	s_mtime;		/* Mount time */
	u32	s_wtime;		/* Write time */
	u16	s_mnt_count;		/* Mount count */
	u16	s_max_mnt_count;	/* Maximal mount count */
	u16	s_magic;		/* Magic signature */
	u16	s_state;		/* File system state */
	u16	s_errors;		/* Behaviour when detecting errors */
	u16	s_minor_rev_level; 	/* minor revision level */
	u32	s_lastcheck;		/* time of last check */
	u32	s_checkinterval;	/* max. time between checks */
	u32	s_creator_os;		/* OS */
	u32	s_rev_level;		/* Revision level */
	u16	s_def_resuid;		/* Default uid for reserved blocks */
	u16	s_def_resgid;		/* Default gid for reserved blocks */
	/*
	* These fields are for EXT2_DYNAMIC_REV superblocks only.
	*
	* Note: the difference between the compatible feature set and
	* the incompatible feature set is that if there is a bit set
	* in the incompatible feature set that the kernel doesn't
	* know about, it should refuse to mount the filesystem.
	*
	* e2fsck's requirements are more strict; if it doesn't know
	* about a feature in either the compatible or incompatible
	* feature set, it must abort and not try to meddle with
	* things it doesn't understand...
	*/
	u32	s_first_ino; 		/* First non-reserved inode */
	u16     s_inode_size; 		/* size of inode structure */
	u16	s_block_group_nr; 	/* block group # of this superblock */
	u32	s_feature_compat; 	/* compatible feature set */
	u32	s_feature_incompat; 	/* incompatible feature set */
	u32	s_feature_ro_compat; 	/* readonly-compatible feature set */
	u8	s_uuid[16];		/* 128-bit uuid for volume */
	char	s_volume_name[16]; 	/* volume name */
	char	s_last_mounted[64]; 	/* directory where last mounted */
	u32	s_reserved[206];	/* Padding to the end of the block */
} SUPER;

typedef struct ext2_group_desc
{
	u32	bg_block_bitmap;	/* Blocks bitmap block */
	u32	bg_inode_bitmap;	/* Inodes bitmap block */
	u32	bg_inode_table;		/* Inodes table block */
	u16	bg_free_blocks_count;	/* Free blocks count */
	u16	bg_free_inodes_count;	/* Free inodes count */
	u16	bg_used_dirs_count;	/* Directories count */
	u16	bg_pad;
	u32	bg_reserved[3];
} GD;

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
		u32     pad[5];         /* inode size MUST be 128 bytes */
		u32	i_date;         /* MTX date */
	u32	i_time;         /* MTX time */
} INODE;

typedef struct ext2_dir_entry_2 {
	u32	inode;			/* Inode number */
	u16	rec_len;		/* Directory entry length */
	u8	name_len;		/* Name length */
	u8	file_type;
	char	name[255];      	/* File name */
} DIR;

typedef struct semaphore{
int value;
struct proc *queue;
}SEMAPHORE;


#define NMINODES          20
#define NMOUNT             4
#define NFD               10
#define NOFT              20
#define NPIPE             10
#define PSIZE            256

// UNIX <fcntl.h> constants: <asm/fcntl.h> in Linux
#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02
#define O_CREAT		   0100	/* not fcntl */
#define O_TRUNC		  01000	/* not fcntl */
#define O_APPEND	  02000

#define READ_PIPE  4
#define WRITE_PIPE 5

typedef struct Oft{
int   mode;
int   refCount;
struct Minode *inodeptr;
struct pipe *pipe_ptr;
long  offset;
char  name[32];
} OFT;

/******************** KCW on MINODE *******************
refCount = # of procs using this MINODE
lock is a semaphore for exclusive access to this MINODE
			WHY lock?
When a proc issues disk I/O on a MINODE, it may gets
blocked while waiting for disk completion interrupt =>
another proc may find the same MINODE and proceed to
use it (before fully loaded) OR (even worse!) modify it.
Hence the need of a lock.
Unix uses a "locked" flag to sleep/wakeup.
MTX uses a semaphore in each MINODE
*****************************************************/

typedef struct Minode{
INODE    INODE;
int      dev, ino;
int      refCount;
int      dirty;
int      mounted;
struct Mount *mountptr;
struct semaphore lock;  // locking semaphore
} MINODE;


typedef struct Mount{
		int    ninodes;
		int    nblocks;
		int    dev, busy;
		struct Minode *mounted_inode;
		char   name[32];
		char   mount_name[32];
		// mounted dev's map & inodes_block numbers
		// although all EXT2 FS, these values may be different PER device
		int    BMAP,IMAP,IBLOCK;
} MOUNT;

typedef struct pipe{
		char  buf[PSIZE];
		int   head, tail, data, room;
		OFT   *nreader, *nwriter;
		int   busy;
}PIPE;


typedef struct stat {
ushort st_dev;		/* major/minor device number */
ushort st_ino;		/* i-node number */
ushort st_mode;		/* file mode, protection bits, etc. */
ushort st_nlink;		/* # links; TEMPORARY HACK: should be nlink_t*/
ushort st_uid;			/* uid of the file's owner */
ushort st_gid;		/* gid; TEMPORARY HACK: should be gid_t */
ushort st_rdev;
long   st_size;		/* file size */
long   st_atime;		/* time of last access */
long   st_mtime;		// time of last modification
long   st_ctime;		// time of creation
long   st_dtime;
long   st_date;
long   st_time;
} STAT;

#define INBUFLEN    64
#define OUTBUFLEN   64
#define EBUFLEN     10
#define LSIZE       64

typedef struct stty {
/* input buffer */
char inbuf[INBUFLEN];
int inhead, intail;
struct semaphore inchars, inlines, mutex;

/* output buffer */
char outbuf[OUTBUFLEN];
int outhead, outtail;
struct semaphore outspace;
char kline[LSIZE];
int tx_on;

/* echo buffer */
char ebuf[EBUFLEN];
int ehead, etail, e_count;

/* Control section */
char echo;   /* echo inputs */
char ison;   /* on or off */
char erase, kill, intr, quit, x_on, x_off, eof;

/* I/O port base address */
int port;
char *tty;
} STTY;

#define PROCESS 0
#define THREAD  1

typedef struct pres{
int     uid;
int     gid;

u16  segment, tsize, dsize, size;

MINODE *cwd;              // CWD
char    name[32];

char    tty[16];          // opened /dev/ttyXX
int     tcount;           // process tcount
u16     signal;           // 15 signals=bits 1 to 14
u16     sig[NSIG];        // 15 signal handlers
OFT     *fd[NFD];         // open file descriptors

struct semaphore mlock;   // messageQ lock
struct semaphore message; // # of messages in mqueue
struct mbuf      *mqueue; // message queue
} PRES;

typedef struct proc{
		struct  proc *next;
		int    *ksp;
		int     inkmode;
		int     uss,usp;


		int     pid;
		int     ppid;
		int     uid;
		int     status;
		int     pri;           /* scheduling priority */

		int     event;
		int     exitValue;
		int     pause;
		int     vforked;

		int     time;
		int     cpu;           // CPU time ticks used in ONE second
		int     type;          // PROCESS|THREAD

struct proc  *parent;
struct proc  *proc;          // process ptr
struct pres  *res;           // resource struct ptr
struct semaphore *sem;       // ptr to semaphore currently BLOCKed on

int kstack[SSIZE];
}PROC;

/* Default dir and regulsr file modes */
#define DIR_MODE          0040777
#define FILE_MODE         0100644
#define SUPER_MAGIC       0xEF53
#define SUPER_USER        0

// dev number of FD and HD; use their major device #
#define FD  2

struct devtab{
int dev;
ulong  start_sector;
ulong  size;
struct buf *dev_list;
struct buf *io_queue;
};

struct hd {
ushort   opcode;
ushort   drive;
ulong    start_sector;
ulong    size;               // size in number of sectors
};

struct floppy {		/* drive struct, one per drive */
ushort opcode;	/* FDC_READ, FDC_WRITE or FDC_FORMAT */
ushort cylinder;	/* cylinder number addressed */
ushort sector;	/* sector : counts from 1 NOT from 0 */
ushort head;		/* head number addressed */
ushort count;		/* byte count (BLOCK_SIZE) */
ushort address;	/* virtual data address in memory */

char results[7];	/* each cmd may yield upto 7 result bytes */
char calibration;	/* CALIBRATED or UNCALIBRATED */
ushort  curcyl;	/* current cylinder */
};

#define NBUF  4

struct buf{
struct buf *next_free;
struct buf *next_dev;
int opcode;             // READ | WRITE
int dev,blk;

/********* these status variables could be changed to bits **********/
int dirty;
int busy;
int async;              //ASYNC write flag
int valid;              // data valid
int wanted;

struct semaphore lock;

struct semaphore iodone;

char buf[1024];         // data area
};

typedef struct ps{ // 8+16=24 bytes for ps syscall
u16 pid,ppid,uid,status;
char pgm[16];
}PS;

struct devsw {
int (*dev_read)();
int (*dev_write)();
};

#define NDEV 10

#define NMBUF  NPROC

typedef struct mbuf{
struct mbuf *next;
int sender;
char contents[128];
} MBUF;
