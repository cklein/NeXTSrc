#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#ifndef lint
_sccsid:.asciz	"@(#)dbxxx.s	5.2 (Berkeley) 7/12/85"
#endif not lint

	.data
#  __lg_flag is used by f77_abort to decide whether or not to dump memory
	.globl	__lg_flag
__lg_flag:
	.long	1
	.text
