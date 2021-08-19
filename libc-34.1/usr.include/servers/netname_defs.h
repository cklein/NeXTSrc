/* 
 * Mach Operating System
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * netname_defs.h
 *
 * $Source: /afs/cs.cmu.edu/project/mach/src/mach-working/etc/netmsgserver/server/RCS/netname_defs.h,v $
 *
 * $Header: netname_defs.h,v 1.6 88/07/28 14:22:03 mrt Exp $
 *
 */

/*
 * Definitions for the mig interface to the network name service.
 */

/*
 * HISTORY:
 * 28-Jul-88  Mary R. Thompson (mrt) at Carnegie Mellon
 *	Copied definitions of NAME_NOT_YOURS and NAME_NOT_CHECKED_IN
 *	from the old netname_defs.h so that old code would not break
 *
 *  8-Mar-88  Daniel Julin (dpj) at Carnegie-Mellon University
 *	Added NETNAME_INVALID_PORT.
 *
 * 28-Feb-88  Daniel Julin (dpj) at Carnegie-Mellon University
 *	Added NETNAME_PENDING.
 *
 * 23-Dec-86  Robert Sansom (rds) at Carnegie Mellon University
 *	Copied from the previous version of the network server.
 *
 */

#ifndef	_NETNAME_DEFS_
#define	_NETNAME_DEFS_

#define NETNAME_SUCCESS		(0)
#define	NETNAME_PENDING		(-1)
#define NETNAME_NOT_YOURS	(1000)
#define NAME_NOT_YOURS		(1000)
#define NETNAME_NOT_CHECKED_IN	(1001)
#define NAME_NOT_CHECKED_IN	(1001)
#define NETNAME_NO_SUCH_HOST	(1002)
#define NETNAME_HOST_NOT_FOUND	(1003)
#define	NETNAME_INVALID_PORT	(1004)

typedef char netname_name_t[80];

#endif NETNAME_DEFS_
