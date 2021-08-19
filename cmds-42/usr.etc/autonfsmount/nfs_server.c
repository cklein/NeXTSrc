#ifndef lint
static char sccsid[] = 	"@(#)nfs_server.c	1.1 88/03/07 4.0NFSSRC Copyr 1988 Sun Micro";
#endif

/*
 * Copyright (c) 1987 by Sun Microsystems, Inc.
 */

#include <stdio.h>
#include <rpc/rpc.h>
#include "nfs_prot.h"

#if	NeXT
typedef struct nfsprocstat {
	unsigned ncalls;
	unsigned totaltime;
	unsigned maxtime;
	unsigned mintime;
} nfsprocstat_t;

static nfsprocstat_t nfsprocstats[18];

static bool_t
xdr_nfsprocstat_t(
		  XDR *xdrs,
		  nfsprocstat_t *stat
		  )
{
	if (!xdr_u_int(xdrs, &stat->ncalls)) {
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &stat->totaltime)) {
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &stat->maxtime)) {
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &stat->mintime)) {
		return (FALSE);
	}
	return (TRUE);
}

static bool_t
xdr_nfsprocstats(
		 XDR *xdrs,
		 nfsprocstat_t *stats
		 )
{
	if (!xdr_vector(xdrs, (char *)stats, 18, sizeof(nfsprocstat_t),
			xdr_nfsprocstat_t)) {
		return (FALSE);
	}
	return (TRUE);
}

static void
time_record(
	    int procno,
	    int elapsed
	    )
{
	if (nfsprocstats[procno].ncalls == 0) {
		nfsprocstats[procno].mintime = 0x7fffffff;
	}
	nfsprocstats[procno].totaltime += elapsed;
	if (elapsed > nfsprocstats[procno].maxtime) {
		nfsprocstats[procno].maxtime = elapsed;
	}
	if (elapsed < nfsprocstats[procno].mintime) {
		nfsprocstats[procno].mintime = elapsed;
	}
	nfsprocstats[procno].ncalls++;
}


static unsigned
time_msec(
	  void
	  )
{
	static struct timeval base;
	static int initialized;
	struct timeval now;

	if (!initialized) {
		gettimeofday(&base, NULL);
		initialized = 1;
	}
	gettimeofday(&now, NULL);
	if (now.tv_usec < base.tv_usec) {
		return (((now.tv_sec - 1) - base.tv_sec) * 1000 +
			((now.tv_usec + 1000000) - base.tv_usec) / 1000); 
	} else {
		return ((now.tv_sec - base.tv_sec) * 1000 +
			(now.tv_usec - base.tv_usec) / 1000); 
	}
}

#endif	NeXT

int trace;

void
nfs_program_2(rqstp, transp)
	struct svc_req *rqstp;
	SVCXPRT *transp;
{
	union {
		nfs_fh nfsproc_getattr_2_arg;
		sattrargs nfsproc_setattr_2_arg;
		diropargs nfsproc_lookup_2_arg;
		nfs_fh nfsproc_readlink_2_arg;
#ifndef notdef
	/* in brad's original code */
		readres nfsproc_read_2_arg;
#else notdef
	/* generated by rpcgen */
		readargs nfsproc_read_2_arg;
#endif notdef
		writeargs nfsproc_write_2_arg;
		createargs nfsproc_create_2_arg;
		diropargs nfsproc_remove_2_arg;
		renameargs nfsproc_rename_2_arg;
		linkargs nfsproc_link_2_arg;
		symlinkargs nfsproc_symlink_2_arg;
		createargs nfsproc_mkdir_2_arg;
		diropargs nfsproc_rmdir_2_arg;
		readdirargs nfsproc_readdir_2_arg;
		nfs_fh nfsproc_statfs_2_arg;
	} argument;
	char *result;
	bool_t (*xdr_argument)(), (*xdr_result)();
	char *(*local)();
	extern attrstat *nfsproc_getattr_2();
	extern attrstat *nfsproc_setattr_2();
	extern void *nfsproc_root_2();
	extern diropres *nfsproc_lookup_2();
	extern readlinkres *nfsproc_readlink_2();
	extern readres *nfsproc_read_2();
	extern void *nfsproc_writecache_2();
	extern attrstat *nfsproc_write_2();
	extern diropres *nfsproc_create_2();
	extern nfsstat *nfsproc_remove_2();
	extern nfsstat *nfsproc_rename_2();
	extern nfsstat *nfsproc_link_2();
	extern nfsstat *nfsproc_symlink_2();
	extern diropres *nfsproc_mkdir_2();
	extern nfsstat *nfsproc_rmdir_2();
	extern readdirres *nfsproc_readdir_2();
	extern statfsres *nfsproc_statfs_2();
#if	NeXT
	unsigned time;
#endif
	switch (rqstp->rq_proc) {
#if	NeXT
	case -2:
		svc_sendreply(transp, xdr_nfsprocstats, &nfsprocstats);
		return;
#endif
	case NULLPROC:
		svc_sendreply(transp, xdr_void, (caddr_t)NULL);
		return;

	case NFSPROC_GETATTR:
		xdr_argument = xdr_nfs_fh;
		xdr_result = xdr_attrstat;
		local = (char *(*)()) nfsproc_getattr_2;
		break;

	case NFSPROC_SETATTR:
		xdr_argument = xdr_sattrargs;
		xdr_result = xdr_attrstat;
		local = (char *(*)()) nfsproc_setattr_2;
		break;

	case NFSPROC_ROOT:
		xdr_argument = xdr_void;
		xdr_result = xdr_void;
		local = (char *(*)()) nfsproc_root_2;
		break;

	case NFSPROC_LOOKUP:
		xdr_argument = xdr_diropargs;
		xdr_result = xdr_diropres;
		local = (char *(*)()) nfsproc_lookup_2;
		break;

	case NFSPROC_READLINK:
		xdr_argument = xdr_nfs_fh;
		xdr_result = xdr_readlinkres;
		local = (char *(*)()) nfsproc_readlink_2;
		break;

	case NFSPROC_READ:
		xdr_argument = xdr_readargs;
		xdr_result = xdr_readres;
		local = (char *(*)()) nfsproc_read_2;
		break;

	case NFSPROC_WRITECACHE:
		xdr_argument = xdr_void;
		xdr_result = xdr_void;
		local = (char *(*)()) nfsproc_writecache_2;
		break;

	case NFSPROC_WRITE:
		xdr_argument = xdr_writeargs;
		xdr_result = xdr_attrstat;
		local = (char *(*)()) nfsproc_write_2;
		break;

	case NFSPROC_CREATE:
		xdr_argument = xdr_createargs;
		xdr_result = xdr_diropres;
		local = (char *(*)()) nfsproc_create_2;
		break;

	case NFSPROC_REMOVE:
		xdr_argument = xdr_diropargs;
		xdr_result = xdr_nfsstat;
		local = (char *(*)()) nfsproc_remove_2;
		break;

	case NFSPROC_RENAME:
		xdr_argument = xdr_renameargs;
		xdr_result = xdr_nfsstat;
		local = (char *(*)()) nfsproc_rename_2;
		break;

	case NFSPROC_LINK:
		xdr_argument = xdr_linkargs;
		xdr_result = xdr_nfsstat;
		local = (char *(*)()) nfsproc_link_2;
		break;

	case NFSPROC_SYMLINK:
		xdr_argument = xdr_symlinkargs;
		xdr_result = xdr_nfsstat;
		local = (char *(*)()) nfsproc_symlink_2;
		break;

	case NFSPROC_MKDIR:
		xdr_argument = xdr_createargs;
		xdr_result = xdr_diropres;
		local = (char *(*)()) nfsproc_mkdir_2;
		break;

	case NFSPROC_RMDIR:
		xdr_argument = xdr_diropargs;
		xdr_result = xdr_nfsstat;
		local = (char *(*)()) nfsproc_rmdir_2;
		break;

	case NFSPROC_READDIR:
		xdr_argument = xdr_readdirargs;
		xdr_result = xdr_readdirres;
		local = (char *(*)()) nfsproc_readdir_2;
		break;

	case NFSPROC_STATFS:
		xdr_argument = xdr_nfs_fh;
		xdr_result = xdr_statfsres;
		local = (char *(*)()) nfsproc_statfs_2;
		break;

	default:
		svcerr_noproc(transp);
		return;
	}
	if (rqstp->rq_cred.oa_flavor != AUTH_UNIX) {
		svcerr_weakauth(transp);
		return;
	}
	bzero(&argument, sizeof(argument));
	if (! svc_getargs(transp, xdr_argument, &argument)) {
		svcerr_decode(transp);
		return;
	}
	if (trace)
		trace_call(rqstp->rq_proc, &argument); 
#if	NeXT
	time = time_msec();
#endif
	result = (*local)(&argument, rqstp->rq_clntcred);
#if	NeXT
	time_record(rqstp->rq_proc, time_msec() - time);
#endif
	if (trace)
		trace_return(rqstp->rq_proc, result); 
	if (! svc_sendreply(transp, xdr_result, (caddr_t)result)) {
		svcerr_systemerr(transp);
	}
	if (! svc_freeargs(transp, xdr_argument, &argument)) {
		exit(1);
	}
}


