        BOOTSEG =  0x07C0       ! Boot block is loaded again to here.
        SSP      =   8192        ! Stack pointer at SS+8KB

        .globl _main,_prints                                 ! IMPORT symbols
        .globl _getc,_putc,_readfd,_setes,_inces,_error      ! EXPORT symbols

        !-------------------------------------------------------
        ! Only one SECTOR loaded at (0000,7C00). Get entire BLOCK in
        !-------------------------------------------------------
        mov  ax,#BOOTSEG    ! set ES to 0x9800
        mov  es,ax
        xor  bx,bx          ! clear BX = 0

        !---------------------------------------------------
        !  diskio[0,0] to read boot BLOCK to [0x9800,0]
        !---------------------------------------------------
        xor  dx,dx          ! drive 0, head 0
        xor  cx,cx
        incb cl             ! cyl 0, sector 1
        mov  ax, #0x0202    ! READ 1 block
        int  0x13

        jmpi    start,BOOTSEG           ! CS=BOOTSEG, IP=start

start:
        mov     ax,cs                   ! Set segment registers for CPU
        mov     ds,ax                   ! we know ES,CS=0x9800. Let DS=CS
        mov     ss,ax                   ! SS = CS ===> all point at 0x9800
        mov     es,ax
        mov     sp,#SSP                 ! SP = 8KB above SS=0x9800

        mov     ax,#0x0012              ! 640x480 color
	int     0x10

        call _main                      ! call main() in C

        test ax, ax
	je  _error

        jmpi 0,0x1000


!======================== I/O functions =================================
       !---------------------------------------
       ! diskr(cyl, head, sector, buf)
       !        4     6     8      10
       !---------------------------------------
_readfd:
        push  bp
	mov   bp,sp            ! bp = stack frame pointer

        movb  dl, #0x00        ! drive 0=FD0
        movb  dh, 6[bp]        ! head
        movb  cl, 8[bp]        ! sector
        incb  cl               ! BIOS count sector from 1
        movb  ch, 4[bp]        ! cyl
        mov   bx, 10[bp]       ! BX=buf ==> memory addr=(ES,BX)
        mov   ax, #0x0202      ! READ 2 sectors to (EX, BX)

        int  0x13              ! call BIOS to read the block
        jb   _error            ! to error if CarryBit is on [read failed]

        pop  bp
	ret

        !---------------------------------------------
        !  char getc()   function: returns a char
        !---------------------------------------------
_getc:
        xorb   ah,ah           ! clear ah
        int    0x16            ! call BIOS to get a char in AX
        ret

        !----------------------------------------------
        ! void putc(char c)  function: print a char
        !----------------------------------------------
_putc:
        push   bp
	mov    bp,sp

        movb   al,4[bp]        ! get the char into aL
        movb   ah,#14          ! aH = 14
        movb   bl,#0x0D        ! bL = cyan color
        int    0x10            ! call BIOS to display the char

        pop    bp
	ret

_setes:  push  bp
	 mov   bp,sp

         mov   ax,4[bp]
         mov   es,ax

	 pop   bp
	 ret

_inces:                         ! inces() inc ES by 0x40, or 1K
         mov   ax,es
         add   ax,#0x40
         mov   es,ax
         ret

        !------------------------------
        !       error & reboot
        !------------------------------
_error:
        mov  bx, #bad
        push bx
        call _prints

!dead:	jmp dead

        int  0x19                       ! reboot

bad:    .asciz  "Error!"
