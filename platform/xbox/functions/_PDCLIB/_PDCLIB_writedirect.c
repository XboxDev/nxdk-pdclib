/* _PDCLIB_writedirect( FILE *, const char *, size_t )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

/* This is an implementation of _PDCLIB_writedirect() for use with nxdk
 */

#include <stdio.h>

#ifndef REGTEST

#include "pdclib/_PDCLIB_glue.h"

#include <windows.h>

int _PDCLIB_w32errno( DWORD werror );

/* The number of write attempts before giving up, as in _PDCLIB_flushbuffer() */
#define _PDCLIB_IO_RETRIES 1

_PDCLIB_size_t _PDCLIB_writedirect( struct _PDCLIB_file_t * stream, const char * data, _PDCLIB_size_t size )
{
    _PDCLIB_size_t written = 0;
    unsigned int retries;
    /* Keep trying to write data until everything is written, an error
       occurs, or the configured number of retries is exceeded.
    */
    for ( retries = _PDCLIB_IO_RETRIES; retries > 0 && written < size; --retries )
    {
        DWORD amount_written;
        if ( ! WriteFile( stream->handle, data + written, (DWORD)( size - written ), &amount_written, NULL ) )
        {
            *_PDCLIB_errno_func() = _PDCLIB_w32errno( GetLastError() );
            stream->status |= _PDCLIB_ERRORFLAG;
            return written;
        }
        written += (_PDCLIB_size_t)amount_written;
        stream->pos.offset += amount_written;
    }
    if ( written < size )
    {
        /* Number of retries exceeded. */
        *_PDCLIB_errno_func() = _PDCLIB_EAGAIN;
        stream->status |= _PDCLIB_ERRORFLAG;
    }
    return written;
}

#endif

#ifdef TEST

#include "_PDCLIB_test.h"

int main( void )
{
    /* Testing covered by fwrite.c */
    return TEST_RESULTS;
}

#endif
