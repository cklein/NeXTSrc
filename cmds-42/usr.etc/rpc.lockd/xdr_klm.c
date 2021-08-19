#ifndef lint
static char sccsid[] = "%w% 88/08/05 NFSSRC4.0 Copyr 1984 Sun Micro";
#endif

/* 
 * Compiled from klm_prot.x using rpcgen.
 * DO NOT EDIT THIS FILE!
 * This is NOT source code!
 */

#include "prot_lock.h"

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
	struct alock *objp;
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
	nlm_holder *objp;
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
	klm_testrply *objp;
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
	reclock *objp;
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
	reclock *objp;
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
	reclock *objp;
{
	if (!xdr_klm_lock(xdrs, &objp->alock)) {
		return (FALSE);
	}
	return (TRUE);
}

