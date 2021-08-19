#ifndef lint
static char sccsid[] = 	"@(#)klm_prot.c	1.1 88/08/12 4.0NFSSRC Copyr 1988 Sun Micro";
#endif

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 * @(#) from SUN 1.6
 */

/* 
 * Compiled from klm_prot.x using rpcgen.
 * DO NOT EDIT THIS FILE!
 * This is NOT source code!
 */

#include <rpc/rpc.h>
#include <rpcsvc/klm_prot.h>


bool_t
xdr_klm_stats(xdrs, objp)
	XDR *xdrs;
	klm_stats *objp;
{
	if (!xdr_enum(xdrs, (enum_t *)objp)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_klm_lock(xdrs, objp)
	XDR *xdrs;
	klm_lock *objp;
{
	if (!xdr_string(xdrs, &objp->server_name, LM_MAXSTRLEN)) {
		return (FALSE);
	}
	if (!xdr_netobj(xdrs, &objp->fh)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->pid)) {
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->l_offset)) {
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->l_len)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_klm_holder(xdrs, objp)
	XDR *xdrs;
	klm_holder *objp;
{
	if (!xdr_bool(xdrs, &objp->exclusive)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->svid)) {
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->l_offset)) {
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->l_len)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_klm_stat(xdrs, objp)
	XDR *xdrs;
	klm_stat *objp;
{
	if (!xdr_klm_stats(xdrs, &objp->stat)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_klm_testrply(xdrs, objp)
	XDR *xdrs;
	klm_testrply *objp;
{
	if (!xdr_klm_stats(xdrs, &objp->stat)) {
		return (FALSE);
	}
	switch (objp->stat) {
	case klm_denied:
		if (!xdr_klm_holder(xdrs, &objp->klm_testrply_u.holder)) {
			return (FALSE);
		}
		break;
	}
	return (TRUE);
}




bool_t
xdr_klm_lockargs(xdrs, objp)
	XDR *xdrs;
	klm_lockargs *objp;
{
	if (!xdr_bool(xdrs, &objp->block)) {
		return (FALSE);
	}
	if (!xdr_bool(xdrs, &objp->exclusive)) {
		return (FALSE);
	}
	if (!xdr_klm_lock(xdrs, &objp->alock)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_klm_testargs(xdrs, objp)
	XDR *xdrs;
	klm_testargs *objp;
{
	if (!xdr_bool(xdrs, &objp->exclusive)) {
		return (FALSE);
	}
	if (!xdr_klm_lock(xdrs, &objp->alock)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_klm_unlockargs(xdrs, objp)
	XDR *xdrs;
	klm_unlockargs *objp;
{
	if (!xdr_klm_lock(xdrs, &objp->alock)) {
		return (FALSE);
	}
	return (TRUE);
}


