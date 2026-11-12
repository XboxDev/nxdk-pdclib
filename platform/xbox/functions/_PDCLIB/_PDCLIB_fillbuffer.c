/* _PDCLIB_fillbuffer( struct _PDCLIB_file_t * )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

/* This is an implementation of _PDCLIB_fillbuffer() for use with nxdk
*/

#include <stdio.h>

#ifndef REGTEST

#include "pdclib/_PDCLIB_glue.h"

#include <errno.h>
#include <unistd.h>

int _PDCLIB_fillbuffer( struct _PDCLIB_file_t * stream )
{
    /* No need to handle buffers > INT_MAX, as PDCLib doesn't allow them */
    ssize_t amount_read = read( stream->handle, stream->buffer, stream->bufsize );
    if ( amount_read >= 0 )
    {
        if (amount_read == 0)
        {
            /* End-of-File */
            stream->status |= _PDCLIB_EOFFLAG;
            return EOF;
        }

        /* Reading successful. */
        if ( ! ( stream->status & _PDCLIB_FBIN ) )
        {
            /* TODO: Text stream conversion here */
        }
        stream->pos.offset += amount_read;
        stream->bufend = amount_read;
        stream->bufidx = 0;
        return 0;
    }
    else
    {
        stream->status |= _PDCLIB_ERRORFLAG;
        return EOF;
    }
}

#endif
