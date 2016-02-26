	.globl _setds
_setds:
	push bp			
	mov  bp,sp		

	mov  ds,4[bp]		! load ds with segment value

	pop  bp
	ret
	
