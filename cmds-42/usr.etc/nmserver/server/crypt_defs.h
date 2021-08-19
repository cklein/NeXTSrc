/* 
 * Mach Operating System
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * crypt_defs.h
 *
 * $Source: /os/osdev/LIBS/libs-7/etc/nmserver/server/RCS/crypt_defs.h,v $
 *
 * $Header: crypt_defs.h,v 1.1 88/09/30 15:43:11 osdev Exp $
 *
 */

/*
 * Internal definitions for crypt module.
 */

/*
 * HISTORY:
 * 21-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added DES encryption/decryption functions.
 *
 *  8-Jun-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Moved encryption algorithm definitions to crypt.h.
 *
 * 22-Jan-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

#ifndef	_CRYPT_DEFS_
#define	_CRYPT_DEFS_


/*
 * External definitions of functions.
 */

extern void encrypt_des();
/*
key_t		key;
pointer_t	data_ptr;
int		data_size;
*/

extern void decrypt_des();
/*
key_t		key;
pointer_t	data_ptr;
int		data_size;
*/


extern void encrypt_multperm();
/*
key_t		key;
pointer_t	data_ptr;
int		data_size;
*/

extern void decrypt_multperm();
/*
key_t		key;
pointer_t	data_ptr;
int		data_size;
*/


extern void encrypt_newdes();
/*
key_t		key;
pointer_t	data_ptr;
int		data_size;
*/

extern void decrypt_newdes();
/*
key_t		key;
pointer_t	data_ptr;
int		data_size;
*/


extern void encrypt_xor();
/*
key_t		key;
pointer_t	data_ptr;
int		data_size;
*/

extern void decrypt_xor();
/*
key_t		key;
pointer_t	data_ptr;
int		data_size;
*/

#endif	_CRYPT_DEFS_
