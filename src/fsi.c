/* [c41] File System Interface - functions
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 *  - 2013/05/12 Costin Ionescu: added c41_file_save_u8p()
 *  - 2013/09/12 Costin Ionescu: added c41_file_load_u8p()
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

/* c41_file_open_fsp ********************************************************/
C41_API uint_t C41_CALL c41_file_open_fsp
(
    uint8_t const *               path_a, 
    size_t                        path_n, // if < 0 then strlen(path_a)
    uint32_t                      mode,
    c41_fsi_t *                   fsi_p,
    c41_io_t * *                  io_pp
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

/* c41_file_open_u8p ********************************************************/
C41_API uint_t C41_CALL c41_file_open_u8p
(
    uint8_t const *             utf8_path_a, 
    ssize_t                     utf8_path_n, // if < 0 then strlen(path_a)
    uint32_t                    mode,
    c41_fspi_t *                fspi_p,
    c41_fsi_t *                 fsi_p,
    c41_ma_t *                  ma_p,
    c41_io_t * *                io_pp
)
{
    uint8_t fsp_buf[0x4];
    uint8_t * fsp_ptr;
    ssize_t fsp_len;
    uint_t mae, fsie;

    if (utf8_path_n < 0) utf8_path_n = C41_STR_LEN(utf8_path_a);
    fsp_len = fspi_p->fsp_from_utf8(fsp_buf, sizeof(fsp_buf),
                                    utf8_path_a, utf8_path_n);
    if (fsp_len < 0) return C41_FSI_BAD_PATH;
    if (fsp_len > (ssize_t) sizeof(fsp_buf))
    {
        /* must allocate buffer */
        mae = c41_ma_alloc(ma_p, (void * *) &fsp_ptr, fsp_len);
        if (mae) return C41_FSI_NO_RES;
        if (fsp_len != fspi_p->fsp_from_utf8(fsp_ptr, fsp_len,
                                             utf8_path_a, utf8_path_n))
        {
            mae = c41_ma_free(ma_p, fsp_ptr, fsp_len);
            if (mae) return C41_FSI_FREE_ERROR;
        }
    }
    else fsp_ptr = fsp_buf;

    fsie = c41_file_open_fsp(fsp_ptr, fsp_len, mode, fsi_p, io_pp);

    if (fsp_len > (ssize_t) sizeof(fsp_buf))
    {
        mae = c41_ma_free(ma_p, fsp_ptr, fsp_len);
        if (mae) return C41_FSI_FREE_ERROR;
    }

    return fsie;
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

/* c41_file_save_u8p ********************************************************/
C41_API uint_t C41_CALL c41_file_save_u8p
(
    uint8_t const *             path_utf8_a,
    size_t                      path_utf8_n,
    uint32_t                    mode,
    c41_fsi_t *                 fsi_p,
    c41_fspi_t *                fspi_p,
    c41_ma_t *                  ma_p,
    void *                      data,
    size_t                      len
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
        
    rc = c41_file_open_fsp(fsp_a, fsp_n, mode, fsi_p, &io_p);
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

/* c41_file_load_u8p ********************************************************/
C41_API uint_t C41_CALL c41_file_load_u8p
(
    uint8_t const *             utf8_path_a, 
    ssize_t                     utf8_path_n, // if < 0 then strlen(path_a)
    c41_fspi_t *                fspi_p,
    c41_fsi_t *                 fsi_p,
    c41_ma_t *                  ma_p,
    uint8_t * *                 data_p,
    size_t *                    size_p
)
{
    c41_io_t * io_p;
    uint_t mae, fsie, ioe, e;
    size_t size;

    fsie = c41_file_open_u8p(utf8_path_a, utf8_path_n, 
                C41_FSI_EXF_OPEN | C41_FSI_NEWF_REJECT | C41_FSI_READ,
                fspi_p, fsi_p, ma_p, &io_p);
    if (fsie) return fsie;

    do
    {
        e = 0;
        size = 0;
        *data_p = NULL;
        ioe = c41_io_get_size(io_p);
        if (ioe) { e = C41_FSI_IO_SEEK; break; }

        size = io_p->size;
        if ((int64_t) size != io_p->size) { e = C41_FSI_NO_RES; break; }

        mae = c41_ma_alloc(ma_p, (void * *) data_p, size);
        if (mae) { e = C41_FSI_NO_RES; break; }
        
        ioe = c41_io_read_igs(io_p, *data_p, size, size_p);
        if (ioe) { e = C41_FSI_IO_READ; break; }
        
        if (size != *size_p) { e = C41_FSI_IO_READ; break; }
    }
    while (0);

    fsie = c41_file_destroy(fsi_p, io_p);
    if (fsie && !e) e = fsie;

    if (e && *data_p)
    {
        mae = c41_ma_free(ma_p, *data_p, size);
        if (mae) e = C41_FSI_FREE_ERROR;
    }

    return e;
}

