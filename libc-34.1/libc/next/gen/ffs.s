/*
 * (c) NeXT, INC.  July 7, 1987
 */

#ifdef LIBC_SCCS
	.asciz	"@(#)ffs.s	1.0 (NeXT) 5/7/87"
#endif LIBC_SCCS

/* bit = ffs(value) */

#include "cframe.h"

PROCENTRY(ffs)
	moveq	#0,d0
	movl	a_p0,d1
	beq	3f
	tstw	d1
	bne	1f
	swap	d1
	addl	#16,d0
1:	tstb	d1
	bne	2f
	lsrl	#8,d1
	addl	#8,d0
2:	andl	#0xff,d1
	lea	fbit,a0
	addl	d1,a0
	addb	a0@,d0
3:	rts

|		0xn0,0xn1,0xn2,0xn3,0xn4,0xn5,0xn6,0xn7,0xn8,0xn9,0xnA,0xnB,0xnC,0xnD,0xnE,0xnF
fbit:
	.byte	0x00,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0x0m
	.byte	0x05,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0x1m
	.byte	0x06,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0x2m
	.byte	0x05,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0x3m
	.byte	0x07,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0x4m
	.byte	0x05,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0x5m
	.byte	0x06,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0x6m
	.byte	0x05,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0x7m
	.byte	0x08,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0x8m
	.byte	0x05,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0x9m
	.byte	0x06,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0xAm
	.byte	0x05,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0xBm
	.byte	0x07,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0xCm
	.byte	0x05,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0xDm
	.byte	0x06,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0xEm
	.byte	0x05,0x01,0x02,0x01,0x03,0x01,0x02,0x01,0x04,0x01,0x02,0x01,0x03,0x01,0x02,0x01	| 0xFm