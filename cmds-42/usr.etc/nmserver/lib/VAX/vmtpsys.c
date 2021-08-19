/* 
 * Mach Operating System
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

/*
 * Library for VMTP system calls.
 */

/*
 * HISTORY:
 * 28-May-87  Daniel Julin (dpj) at Carnegie-Mellon University
 *	Created.
 *
 */

#include "SYS.h"
#include "syscall.h"

SYSCALL(invoke);
	ret

SYSCALL(recvreq);
	ret

SYSCALL(sendreply);
	ret

SYSCALL(forward);
	ret

SYSCALL(probeentity);
	ret

SYSCALL(getreply);
	ret
