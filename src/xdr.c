/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "xdr.h"

bool_t
xdr_validate_req(XDR *xdrs, validate_req *objp)
{

	if (!xdr_int(xdrs, &objp->name_len))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->passwd_len))
		return (FALSE);
	if (!xdr_vector(xdrs, (char *)objp->name, USER_NAME_MAX_LEN, sizeof(char), (xdrproc_t)xdr_char))
		return (FALSE);
	if (!xdr_vector(xdrs, (char *)objp->passwd_md5, PASSWD_MAX_LEN, sizeof(char), (xdrproc_t)xdr_char))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_validate_resp(XDR *xdrs, validate_resp *objp)
{

	if (!xdr_int(xdrs, &objp->status))
		return (FALSE);
	if (!xdr_u_long(xdrs, &objp->uid))
		return (FALSE);
	if (!xdr_vector(xdrs, (char *)objp->name, USER_NAME_MAX_LEN, sizeof(char), (xdrproc_t)xdr_char))
		return (FALSE);
	if (!xdr_vector(xdrs, (char *)objp->session_id, SESSION_ID_LEN, sizeof(char), (xdrproc_t)xdr_char))
		return (FALSE);
	return (TRUE);
}
