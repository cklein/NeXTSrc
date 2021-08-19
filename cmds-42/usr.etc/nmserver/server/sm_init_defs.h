/* 
 * Mach Operating System
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * sm_init_defs.h
 *
 * $Source: /os/osdev/LIBS/libs-7/etc/nmserver/server/RCS/sm_init_defs.h,v $
 *
 * $Header: sm_init_defs.h,v 1.1 88/09/30 15:45:27 osdev Exp $
 *
 */

/*
 * Definitions for secure Mach initialisation.
 */

/*
 * HISTORY:
 *  7-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

#ifndef	_SM_INIT_DEFS_
#define	_SM_INIT_DEFS_

#include <sys/mach_param.h>

#define NAME_SERVER_INDEX	0
#define AUTH_SERVER_INDEX	1
#define SM_INIT_INDEX_MAX	1

#undef NAME_SERVER_SLOT
#undef ENVIRONMENT_SLOT
#undef SERVICE_SLOT

#define OLD_NAME_SERVER_SLOT	0
#define NETNAME_SLOT		0
#define SM_INIT_SLOT		1
#define AUTH_PRIVATE_SLOT	1
#define OLD_ENV_SERVER_SLOT	1
#define AUTH_SERVER_SLOT	2
#define OLD_SERVICE_SLOT	2
#define KM_SERVICE_SLOT		2
#define NAME_SERVER_SLOT	3
#define NAME_PRIVATE_SLOT	3
#define SM_INIT_SLOTS_USED	4

#if (SM_INIT_SLOTS_USED > TASK_PORT_REGISTER_MAX)
Things are not going to work!
#endif


#define SM_INIT_FAILURE		11881

#endif	_SM_INIT_DEFS_
