/* 
 * Mach Operating System
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * keyman.h
 *
 * $Source: /os/osdev/LIBS/libs-7/etc/nmserver/server/RCS/keyman.h,v $
 *
 * $Header: keyman.h,v 1.1 88/09/30 15:43:57 osdev Exp $
 *
 */

/*
 * External definitions for Key Management module.
 */

/*
 * HISTORY:
 *  5-Jun-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Replaced km_get_ikey by km_get_dkey.
 *
 *  9-Jan-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

#ifndef	_KEYMAN_
#define	_KEYMAN_

#include <sys/boolean.h>

#include "nm_defs.h"

#define KM_SERVICE_NAME	"NETWORK_SERVER_KEY_MANAGER"

/*
 * Error Codes.
 */
#define KM_SUCCESS	0
#define KM_FAILURE	-1

extern boolean_t km_init();
/*
*/

extern boolean_t km_get_key();
/*
netaddr_t	host_id;
key_ptr_t	key_ptr;
*/

extern boolean_t km_get_dkey();
/*
netaddr_t	host_id;
key_ptr_t	ikey_ptr;
*/

extern void km_do_key_exchange();
/*
int		client_id;
int		(*client_retry)();
netaddr_t	host_id;
*/

#endif	_KEYMAN_
