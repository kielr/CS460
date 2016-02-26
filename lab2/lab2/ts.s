!----------------- ts.s file -----------------------------------------------
        .globl _tswitch
        .globl _main,_running,_scheduler,_proc,_procSize   ! IMPORT these

start:
        mov     ax,cs                   ! establish segments
        mov     ds,ax                   ! Let DS,SS,ES = CS=0x1000.
        mov     ss,ax
        mov     es,ax

        mov     sp,#_proc               ! sp -> proc[0]
        add     sp,_procSize            ! sp -> proc[0]'s HIGH END

        call _main                      ! call main() in C

dead:	jmp dead                        ! loop if main() ever returns



!saves all the data that was in there before we wipe it
!store all registers for the current process into kstack before kfork() moves onto a new process
_tswitch:
SAVE:

    push ax !general purpose registers that are set to the 0x1000 segment
    push bx
	push cx
	push dx
	push bp !base pointer
	push si !source index for string and array copying
	push di !destination index
    pushf   !pushes flag register

    !Added lines
	push ds !push data segment
	push ss !push stack segment
    !

	mov   bx, _running
	mov   2[bx], sp

FIND:	call _scheduler

RESUME:

    mov   bx, _running
	mov   sp, 2[bx] !accesses p->ksp (2[bx] does this because the second member variable of the running proc is that proc's ksp)
                    !and setting sp registers to the top of the current stack frame

    !Pop all values so that the c code will have populated instance of class to use
	pop   ss
	pop   ds
	popf
	pop  di !destination index
	pop  si !source index for string copying
	pop  bp !base pointer
	pop  dx !general purpose registers
	pop  cx
	pop  bx
	pop  ax
	!Return
	ret

