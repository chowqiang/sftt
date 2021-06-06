/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "req_rsp.h"

bool_t
xdr_validate_req (XDR *xdrs, validate_req *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->name_len))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->passwd_len))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->name, USER_NAME_MAX_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->passwd_md5, PASSWD_MD5_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_validate_resp (XDR *xdrs, validate_resp *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->uid))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->name, USER_NAME_MAX_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->session_id, SESSION_ID_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_pwd_req (XDR *xdrs, pwd_req *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->session_id, SESSION_ID_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_pwd_resp (XDR *xdrs, pwd_resp *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->pwd, DIR_PATH_MAX_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_ll_req (XDR *xdrs, ll_req *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->session_id, SESSION_ID_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->path, DIR_PATH_MAX_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_file_entry (XDR *xdrs, file_entry *objp)
{
	register int32_t *buf;

	int i;

	if (xdrs->x_op == XDR_ENCODE) {
		 if (!xdr_vector (xdrs, (char *)objp->name, FILE_NAME_MAX_LEN,
			sizeof (char), (xdrproc_t) xdr_char))
			 return FALSE;
		buf = XDR_INLINE (xdrs, 5 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->type))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->size))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->c_time))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->a_time))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->m_time))
				 return FALSE;
		} else {
			IXDR_PUT_LONG(buf, objp->type);
			IXDR_PUT_LONG(buf, objp->size);
			IXDR_PUT_LONG(buf, objp->c_time);
			IXDR_PUT_LONG(buf, objp->a_time);
			IXDR_PUT_LONG(buf, objp->m_time);
		}
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		 if (!xdr_vector (xdrs, (char *)objp->name, FILE_NAME_MAX_LEN,
			sizeof (char), (xdrproc_t) xdr_char))
			 return FALSE;
		buf = XDR_INLINE (xdrs, 5 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->type))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->size))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->c_time))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->a_time))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->m_time))
				 return FALSE;
		} else {
			objp->type = IXDR_GET_LONG(buf);
			objp->size = IXDR_GET_LONG(buf);
			objp->c_time = IXDR_GET_LONG(buf);
			objp->a_time = IXDR_GET_LONG(buf);
			objp->m_time = IXDR_GET_LONG(buf);
		}
	 return TRUE;
	}

	 if (!xdr_vector (xdrs, (char *)objp->name, FILE_NAME_MAX_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->type))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->size))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->c_time))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->a_time))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->m_time))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_ll_resp (XDR *xdrs, ll_resp *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->nr))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->entries, FILE_ENTRY_MAX_CNT,
		sizeof (file_entry), (xdrproc_t) xdr_file_entry))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->idx))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_cd_req (XDR *xdrs, cd_req *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->session_id, SESSION_ID_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_cd_resp (XDR *xdrs, cd_resp *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->pwd, DIR_PATH_MAX_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_get_req (XDR *xdrs, get_req *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->session_id, SESSION_ID_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->path, DIR_PATH_MAX_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_trans_entry (XDR *xdrs, trans_entry *objp)
{
	register int32_t *buf;

	int i;

	if (xdrs->x_op == XDR_ENCODE) {
		buf = XDR_INLINE (xdrs, 4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->type))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->total_size))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->idx))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->len))
				 return FALSE;

		} else {
		IXDR_PUT_LONG(buf, objp->type);
		IXDR_PUT_LONG(buf, objp->total_size);
		IXDR_PUT_LONG(buf, objp->idx);
		IXDR_PUT_LONG(buf, objp->len);
		}
		 if (!xdr_vector (xdrs, (char *)objp->content, CONTENT_BLOCK_SIZE,
			sizeof (u_char), (xdrproc_t) xdr_u_char))
			 return FALSE;
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = XDR_INLINE (xdrs, 4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->type))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->total_size))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->idx))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->len))
				 return FALSE;

		} else {
		objp->type = IXDR_GET_LONG(buf);
		objp->total_size = IXDR_GET_LONG(buf);
		objp->idx = IXDR_GET_LONG(buf);
		objp->len = IXDR_GET_LONG(buf);
		}
		 if (!xdr_vector (xdrs, (char *)objp->content, CONTENT_BLOCK_SIZE,
			sizeof (u_char), (xdrproc_t) xdr_u_char))
			 return FALSE;
	 return TRUE;
	}

	 if (!xdr_int (xdrs, &objp->type))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->total_size))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->idx))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->len))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->content, CONTENT_BLOCK_SIZE,
		sizeof (u_char), (xdrproc_t) xdr_u_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_get_resp (XDR *xdrs, get_resp *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->nr))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->idx))
		 return FALSE;
	 if (!xdr_trans_entry (xdrs, &objp->entry))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_put_req (XDR *xdrs, put_req *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->session_id, SESSION_ID_LEN,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->nr))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->idx))
		 return FALSE;
	 if (!xdr_trans_entry (xdrs, &objp->entry))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_put_resp (XDR *xdrs, put_resp *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	return TRUE;
}
