	.globl _syscall,_main0,_exit,auto_start
        .globl _getcs
auto_start:		
         call _main0
! if ever return, exit(0)
	push  #0
        call  _exit

_syscall:
        int    80
        ret
_getcs:
	mov   ax,cs
	ret
	
