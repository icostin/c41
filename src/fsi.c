/* [c41] File System Interface - functions
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 *  - 2013/05/12 Costin Ionescu: added c41_file_save()
 */

#include <c41.h>

/* c41_fsi_status_name ******************************************************/
C41_API char const * c41_fsi_status_name (uint_t sc)
{
    switch (sc)
    {
        C41_CASE_RET_STR(C41_FSI_OK              );
        C41_CASE_RET_STR(C41_FSI_CLOSE_FAILED    );
        C41_CASE_RET_STR(C41_FSI_MISSING_ACCESS  );
        C41_CASE_RET_STR(C41_FSI_MISSING_ACTION  );
        C41_CASE_RET_STR(C41_FSI_BAD_PATH        );
        C41_CASE_RET_STR(C41_FSI_OPEN_FAILED     );
        C41_CASE_RET_STR(C41_FSI_NO_RES          );
        C41_CASE_RET_STR(C41_FSI_NO_CODE         );
        //C41_CASE_RET_STR();
    }
    return "C41_FSI_UNKNOWN_ERROR";
}

/*  *************************************************************************/
C41_API uint_t C41_CALL c41_file_open 
(
    c41_fsi_t *                   fsi_p,
    c41_io_t * *                  io_pp, 
    uint8_t const *               path_a, 
    size_t                        path_n, // if < 0 then strlen(path_a)
    uint32_t                      mode
)
{
    uint_t st;

    if ((mode & (C41_FSI_READ | C41_FSI_WRITE)) == 0) 
        return C41_FSI_MISSING_ACCESS;
    if ((mode & (C41_FSI_EXF_MASK | C41_FSI_NEWF_MASK)) ==
        (C41_FSI_EXF_REJECT | C41_FSI_NEWF_REJECT))
        return C41_FSI_MISSING_ACTION;
    st = fsi_p->file_open(io_pp, path_a, path_n, mode, fsi_p->context);

    return st;
}

/* c41_file_destroy *********************************************************/
C41_API uint_t C41_CALL c41_file_destroy
(
    c41_fsi_t *                   fsi_p,
    c41_io_t *                    io_p
)
{
    if (!(io_p->flags & C41_IO_CLOSED))
    {
        if (c41_io_close(io_p)) return C41_FSI_CLOSE_FAILED;
    }
    return fsi_p->file_destroy(io_p, fsi_p->context);
}

/* c41_file_save ************************************************************/
C41_API uint_t C41_CALL c41_file_save
(
    uint8_t const *             path_utf8_a,
    size_t                      path_utf8_n,
    uint32_t                    mode,
    void *                      data,
    size_t                      len,
    c41_fsi_t *                 fsi_p,
    c41_fspi_t *                fspi_p,
    c41_ma_t *                  ma_p
)
{
    ssize_t fsp_n;
    uint8_t * fsp_a;
    c41_io_t * io_p;
    uint_t rc, cl_rc;
    size_t wlen;

    if (!path_utf8_n) path_utf8_n = C41_STR_LEN(path_utf8_a);
    fsp_n = fspi_p->fsp_from_utf8(NULL, 0, path_utf8_a, path_utf8_n);
    if (fsp_n < 0) return (uint_t) -fsp_n;
    if (c41_ma_alloc(ma_p, (void * *) &fsp_a, fsp_n)) return C41_FSI_NO_RES;
    if (fsp_n != fspi_p->fsp_from_utf8(fsp_a, fsp_n, path_utf8_a, path_utf8_n))
    {
        rc = C41_FSI_BAD_PATH;
        goto l_fx;
    }
        
    rc = c41_file_open(fsi_p, &io_p, fsp_a, fsp_n, mode);
    if (rc) goto l_fx;
    rc = c41_io_write(io_p, data, len, &wlen);
    if (rc) goto l_cfx;
    rc = 0;
l_cfx:
    cl_rc = c41_io_close(io_p);
    if (!rc && cl_rc) rc = cl_rc;
l_fx:
    c41_ma_free(ma_p, fsp_a, fsp_n);
    return rc;
}

