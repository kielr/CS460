!********************************************************************
!Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
!This program is free software: you can redistribute it and/or modify
!it under the terms of the GNU General Public License as published by
!the Free Software Foundation, either version 3 of the License, or
!(at your option) any later version.

!This program is distributed in the hope that it will be useful,
!but WITHOUT ANY WARRANTY; without even the implied warranty of
!MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!GNU General Public License for more details.

!You should have received a copy of the GNU General Public License
!along with this program.  If not, see <http://www.gnu.org/licenses/>.
!********************************************************************/
        MTXSEG  = 0x1000

       .globl _main,_running,_scheduler
       .globl _diskr,_setes,_inces,_printf
       .globl _proc, _procSize
       .globl _tswitch
       .globl _getds,_setds
       .globl _lock,_unlock,_int_on,_int_off
       .globl _in_byte,_out_byte,_in_word,_out_word

	! added functions for KUMODE
	.globl _int80h,_goUmode,_kcinth
!	.globl _tinth,_thandler
        .globl _kbinth,_kbhandler
        .globl _halt

        jmpi   start,MTXSEG

start:	mov  ax,cs
	mov  ds,ax
	mov  ss,ax
        mov  es,ax
	mov  sp,#_proc
	add  sp,_procSize

	mov     ax,#0x0003    ! set video mode to mono for vid.c driver
        int     #0x10

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

	  mov  bx,_running
	  inc  INK[bx]
          cmp  INK[bx],#1       ! if inkmode == 1 ==> interrupt was in Umode
          jg  ?1                ! imode>1 : was in Kmode: bypass saving uss,usp

          ! was in Umode: save interrupted (SS,SP) into proc
          mov USS[bx],ss        ! save SS  in proc.USS
          mov USP[bx],sp        ! save SP  in proc.USP

          ! change DS,ES,SS to Kernel segment
          mov  ax,ds            ! stupid !!
          mov  es,ax            ! CS=DS=SS=ES in Kmode
          mov  ss,ax

          mov  sp, _running     ! sp -> running's kstack[] high end
          add  sp, _procSize

?1:       call  _?1             ! call handler in C

          br    _ireturn        ! return to interrupted point

         MEND


_int80h: INTH kcinth
!_tinth:  INTH thandler
_kbinth: INTH kbhandler

!*===========================================================================*
!*		_ireturn  and  goUmode()       				     *
!*===========================================================================*
! ustack contains    flag,ucs,upc, ax,bx,cx,dx,bp,si,di,es,ds
! uSS and uSP are in proc
_ireturn:
_goUmode:
        cli
	mov  bx,_running
        dec  INK[bx]
        jg   xkmode

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

!*===========================================================================*
!*		      getds()/setds()                      		     *
!*===========================================================================*
_getds:  mov ax,ds
         ret
_setds:
	push bp
	mov  bp,sp
	mov  ds,4[bp]		! load ds with segment value
	pop  bp
	ret

_halt:  hlt
        ret


!*===========================================================================*
!*		      int_on()/int_off()                      		     *
!*===========================================================================*

_int_off:                       ! cli, return old flag register
        pushf
        cli
        pop ax
        ret


_int_on:                        ! int_on(int SR)
        push bp
        mov  bp,sp
        mov  ax,4[bp]           ! get SR passed in
        push ax
        popf
        pop  bp
        ret
_lock:
        cli
        ret
_unlock:
        sti
        ret
!*===========================================================================*
!*		in_byte/in_word					     *
!*===========================================================================*
! Read a byte from i/o port

_in_byte:
        push    bp
        mov     bp,sp
        mov     dx,4[bp]
	inb     al,dx		! input 1 byte
	subb	ah,ah		! unsign extend
        pop     bp
        ret

! in_word(port)
_in_word:
        push    bp
        mov     bp,sp
        mov     dx,4[bp]
	in      ax,dx		! input 1 word
        pop     bp
        ret
!*==================================================================*
!*	out_byte/out_word: out_byte(port, int)                     *
!*==================================================================*
_out_byte:
        push    bp
        mov     bp,sp
        mov     dx,4[bp]
        mov     ax,6[bp]
	outb	dx,al   	! output 1 byte
        pop     bp
        ret

_out_word:
        push    bp
        mov     bp,sp
        mov     dx,4[bp]
        mov     ax,6[bp]
	out	dx,ax   	! output 2 bytes
        pop     bp
        ret
