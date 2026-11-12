/* _PDCLIB_flushbuffer( struct _PDCLIB_file_t * )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

/* This is an implementation of _PDCLIB_flushbuffer() for use with nxdk
*/

#include <stdio.h>
#include <string.h>

#ifndef REGTEST

#include "pdclib/_PDCLIB_glue.h"

#include <errno.h>
#include <unistd.h>

int _PDCLIB_flushbuffer( struct _PDCLIB_file_t * stream )
{
    /* No need to handle buffers > INT_MAX, as PDCLib doesn't allow them */
    _PDCLIB_size_t written = 0;
    if ( ! ( stream->status & _PDCLIB_FBIN ) )
    {
        /* TODO: Text stream conversion here */
    }
    /* Continue after short writes; stop on error or lack of progress. */
    while ( written < stream->bufidx )
    {
        ssize_t amount_written = write( stream->handle, stream->buffer + written, stream->bufidx - written );
        if ( amount_written <= 0 )
        {
            /* A zero-byte write cannot make progress. */
            if ( amount_written == 0 )
            {
                *_PDCLIB_errno_func() = _PDCLIB_EIO;
            }
            stream->status |= _PDCLIB_ERRORFLAG;
            /* Move unwritten remains to begin of buffer. */
            stream->bufidx -= written;
            memmove( stream->buffer, stream->buffer + written, stream->bufidx );
            return EOF;
        }
        written += (_PDCLIB_size_t)amount_written;
        stream->pos.offset += amount_written;
        /* O_APPEND may also have been enabled through fileno() and fcntl(). */
        int saved_errno = *_PDCLIB_errno_func();
        off_t offset = lseek( stream->handle, 0, SEEK_CUR );
        if ( offset >= 0 )
        {
            stream->pos.offset = offset;
        }
        *_PDCLIB_errno_func() = saved_errno;
        if ( written == stream->bufidx )
        {
            /* Buffer written completely. */
            stream->bufidx = 0;
            return 0;
        }
    }
    return 0;
}

#endif
