/* 	$Header: utils.h,v 1.1 89/06/08 14:20:42 mmeyer Locked $	*/
/*
 * HISTORY
 * 07-Apr-89  Richard Draves (rpd) at Carnegie-Mellon University
 *	Extensive revamping.  Added polymorphic arguments.
 *	Allow multiple variable-sized inline arguments in messages.
 *
 * 28-May-87  Richard Draves (rpd) at Carnegie-Mellon University
 *	Created.
 */

#ifndef	_UTILS_H
#define	_UTILS_H

/* stuff used by more than one of header.c, user.c, server.c */

extern void WriteImport(/* FILE *file, string_t filename */);
extern void WriteRCSDecl(/* FILE *file, identifier_t name, string_t rcs */);
extern void WriteBogusDefines(/* FILE *file */);

extern void WriteList(/* FILE *file, argument_t *args,
			 void (*func)(FILE *file, argument_t *arg),
			 u_int mask, char *between, char *after */);

extern void WriteReverseList(/* FILE *file, argument_t *args,
				void (*func)(FILE *file, argument_t *arg),
				u_int mask, char *between, char *after */);

/* good as arguments to WriteList */
extern void WriteNameDecl(/* FILE *file, argument_t *arg */);
extern void WriteVarDecl(/* FILE *file, argument_t *arg */);
extern void WriteTypeDecl(/* FILE *file, argument_t *arg */);
extern void WriteCheckDecl(/* FILE *file, argument_t *arg */);

extern char *ReturnTypeStr(/* routine_t *rt */);

extern char *FetchUserType(/* ipc_type_t *it */);
extern char *FetchServerType(/* ipc_type_t *it */);
extern void WriteFieldDeclPrim(/* FILE *file, argument_t *arg,
				  char *(*tfunc)(ipc_type_t *it) */);

extern void WriteStructDecl(/* FILE *file, argument_t *args,
			       void (*func)(FILE *file, argument_t *arg),
			       u_int mask, char *name */);

extern void WriteStaticDecl(/* FILE *file, ipc_type_t *it,
			       boolean_t dealloc, boolean_t longform,
			       identifier_t name */);

extern void WriteCopyType(/* FILE *file, ipc_type_t *it,
			     char *left, char *right, ... */);

extern void WritePackMsgType(/* FILE *file, ipc_type_t *it,
				boolean_t dealloc, boolean_t longform,
				char *left, char *right, ... */);

#endif	_UTILS_H
