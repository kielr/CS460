!====================== ts.s file ================================
        MTXSEG  = 0x1000
       .globl _main,_running,_scheduler
       .globl _proc, _procSize
       .globl _tswitch

        ! added functions for KUMODE
	.globl _int80h,_goUmode,_kcinth

        ! added functions for timer interrupt
	.globl _lock,_unlock,_int_off,_int_on
	.globl _in_byte,_out_byte
        .globl _tinth,_thandler

        jmpi   start,MTXSEG    ! just to make sure CS=0x1000
start:	mov  ax,cs
	mov  ds,ax
	mov  ss,ax
        mov  es,ax
	mov  sp,#_proc
	add  sp,_procSize      ! set stack pointer to high end of PROC[0]

       mov     ax,#0x0003     ! these will start in MONO display mode
       int     #0x10          ! needed by vid driver later

	call _main

_tswitch:
SAVE:
        cli
	push ax
	push bx
	push cx
	push dx
	push bp
	push si
	push di
	pushf
	mov  bx,_running
	mov  2[bx],sp

FIND:	call _scheduler

RESUME:
	mov  bx,_running
	mov  sp,2[bx]
	popf
	pop  di
	pop  si
	pop  bp
	pop  dx
	pop  cx
	pop  bx
	pop  ax

	sti
	ret

	!These offsets are defined in struct proc
USS =   4
USP =   6
INK =   8

! as86 macro: parameters are ?1 ?2, etc
! as86 -m -l listing src (generates listing with macro expansion)

         MACRO INTH
          push ax
          push bx
          push cx
          push dx
          push bp
          push si
          push di
          push es
          push ds

          push cs
          pop  ds

	  mov bx,_running   	! ready to access proc
          inc INK[bx]
          cmp INK[bx],#1
          jg   ?1

          ! was in Umode: save interrupted (SS,SP) into proc
          mov USS[bx],ss        ! save SS  in proc.USS
          mov USP[bx],sp        ! save SP  in proc.USP

          ! change DS,ES,SS to Kernel segment
          mov  di,ds            ! stupid !!
          mov  es,di            ! CS=DS=SS=ES in Kmode
          mov  ss,di

          mov  sp,_running      ! sp -> running's kstack[] high end
          add  sp,_procSize

?1:       call  _?1             ! call handler in C

          br    _ireturn        ! return to interrupted point

         MEND


_int80h: INTH kcinth
_tinth:  INTH thandler

!*===========================================================================*
!*		_ireturn  and  goUmode()       				     *
!*===========================================================================*
! ustack contains    flag,ucs,upc, ax,bx,cx,dx,bp,si,di,es,ds
! uSS and uSP are in proc
_ireturn:
_goUmode:
        cli
	mov bx,_running 	! bx -> proc
        dec INK[bx]
        cmp INK[bx],#0
        jg  xkmode

! restore uSS, uSP from running PROC
        mov ax,USS[bx]
        mov ss,ax               ! restore SS
        mov sp,USP[bx]          ! restore SP
xkmode:
	pop ds
	pop es
	pop di
        pop si
        pop bp
        pop dx
        pop cx
        pop bx
        pop ax
        iret

_int_off:             ! cli, return old flag register
        pushf
        cli
        pop ax
        ret

_int_on:              ! int_on(int SR)
        push bp
        mov  bp,sp
        mov  ax,4[bp] ! get SR passed in
        push ax
        popf
        pop  bp
        ret

!*===========================================================================*
!*				lock					     *
!*===========================================================================*
_lock:
	cli			! disable interrupts
	ret			! return to caller

!*===========================================================================*
!*				unlock					     *
!*===========================================================================*
_unlock:
	sti			! enable interrupts
	ret			! return to caller

_in_byte:
        push    bp
        mov     bp,sp
        mov     dx,4[bp]
	in      ax,dx			! input 1 byte
	subb	ah,ah		! unsign extend
        pop     bp
        ret

_out_byte:
        push    bp
        mov     bp,sp
        mov     dx,4[bp]
        mov     ax,6[bp]
	outb	dx,al		! output 1 byte
        pop     bp
        ret
