         .global _putc,_color

_putc:           
        push   bp
	mov    bp,sp
	
        movb   al,4[bp]        ! get the char into aL
        movb   ah,#14          ! aH = 14
 
        mov    bx,_color       ! bL = color B Cyan C Red  
        int    0x10            ! call BIOS to display the char

        mov    sp,bp
	pop    bp
	ret
