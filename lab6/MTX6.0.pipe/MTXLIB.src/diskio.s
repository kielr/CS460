        .globl _diskr,_setes,_inces,_printf
_diskr:                             
        push  bp
	mov   bp,sp
	
        movb  dl, #0x00        ! drive 0=fd0
        movb  dh, 6[bp]        ! head
        movb  cl, 8[bp]        ! sector
        incb  cl               ! inc sector by 1 to suit BIOS
        movb  ch, 4[bp]        ! cyl
        mov   ax, #0x0202      ! READ 2 sectors 
        mov   bx, 10[bp]       ! put buf value in BX ==> addr=[ES,BX]
        int  0x13              ! call BIOS to read the block 
        jb   error             ! to error if CarryBit is on [read failed]

	mov   sp,bp
	pop   bp
	ret

_setes:  push  bp
	 mov   bp,sp
	
         mov   ax,4[bp]        
         mov   es,ax

	 mov   sp,bp
	 pop   bp
	 ret

_inces:                        ! inces[] inc ES by 0x40, or 1K
         mov   ax,es
         add   ax,#0x40
         mov   es,ax
         ret

        !------------------------------
        !       error & reboot
        !------------------------------
error:
        push #msg
        call _printf
        int  0x19                       ! reboot
msg:    .asciz  "Error!"
