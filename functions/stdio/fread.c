/* fread( void *, size_t, size_t, FILE * )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef REGTEST

#include "pdclib/_PDCLIB_glue.h"

#ifndef __STDC_NO_THREADS__
#include <threads.h>
#endif

/* A read of at least this many bytes (and at least the buffer's size) that finds the buffer empty goes straight into
   the caller's memory. Smaller ones go through the buffer: many small reads from the file cost more than the copy.
*/
#define _PDCLIB_DIRECT_READ 16384

size_t fread( void * _PDCLIB_restrict ptr, size_t size, size_t nmemb, struct _PDCLIB_file_t * _PDCLIB_restrict stream )
{
    char * dest = (char *)ptr;
    size_t total;
    size_t done = 0;

    if ( size == 0 || nmemb == 0 )
    {
        return 0;
    }

    if ( nmemb > SIZE_MAX / size )
    {
        nmemb = SIZE_MAX / size;
    }

    total = size * nmemb;

    _PDCLIB_LOCK( stream->mtx );

    if ( stream->bufidx == stream->bufend && stream->ungetidx == 0 && total >= _PDCLIB_DIRECT_READ && total >= stream->bufsize )
    {
        /* The checks of _PDCLIB_prepread(), without its filling of the buffer that this read bypasses */
        if ( ( stream->status & ( _PDCLIB_FWRITE | _PDCLIB_FAPPEND | _PDCLIB_ERRORFLAG | _PDCLIB_WIDESTREAM | _PDCLIB_EOFFLAG ) ) ||
             ! ( stream->status & ( _PDCLIB_FREAD | _PDCLIB_FRW ) ) )
        {
            *_PDCLIB_errno_func() = _PDCLIB_EBADF;
            stream->status |= _PDCLIB_ERRORFLAG;
            _PDCLIB_UNLOCK( stream->mtx );
            return 0;
        }
        stream->status |= _PDCLIB_FREAD | _PDCLIB_BYTESTREAM;
    }
    else if ( _PDCLIB_prepread( stream ) == EOF )
    {
        _PDCLIB_UNLOCK( stream->mtx );
        return 0;
    }

    /* Characters pushed back with ungetc() come first, the last one pushed first */
    while ( done < total && stream->ungetidx > 0 )
    {
        dest[ done++ ] = stream->ungetbuf[ --( stream->ungetidx ) ];
    }

    while ( done < total )
    {
        size_t available = stream->bufend - stream->bufidx;
        size_t count;

        if ( available == 0 )
        {
            size_t wanted = total - done;

            if ( wanted >= _PDCLIB_DIRECT_READ && wanted >= stream->bufsize )
            {
                /* A large read: read it straight into the caller's memory, saving the copy and
                   the buffer-sized reads. _PDCLIB_fillbuffer() keeps the file position and the EOF and error flags
                   as for any other read; the buffer is empty again afterwards.
                */
                char * buffer = stream->buffer;
                size_t bufsize = stream->bufsize;
                int rc;

                stream->buffer = dest + done;
                stream->bufsize = wanted;
                rc = _PDCLIB_fillbuffer( stream );
                if ( rc != EOF )
                {
                    done += stream->bufend;
                }
                stream->buffer = buffer;
                stream->bufsize = bufsize;
                stream->bufidx = 0;
                stream->bufend = 0;

                if ( rc == EOF )
                {
                    break;
                }
                continue;
            }

            if ( _PDCLIB_fillbuffer( stream ) == EOF )
            {
                break;
            }
            available = stream->bufend - stream->bufidx;
        }

        count = total - done < available ? total - done : available;
        memcpy( dest + done, stream->buffer + stream->bufidx, count );
        stream->bufidx += count;
        done += count;
    }

    _PDCLIB_UNLOCK( stream->mtx );

    return done / size;
}

#endif

#ifdef TEST

#include "_PDCLIB_test.h"

int main( void )
{
    FILE * fh;
    const char * message = "Testing fwrite()...\n";
    char buffer[21];
    buffer[20] = 'x';
    TESTCASE( ( fh = tmpfile() ) != NULL );
    /* fwrite() / readback */
    TESTCASE( fwrite( message, 1, 20, fh ) == 20 );
    rewind( fh );
    TESTCASE( fread( buffer, 1, 20, fh ) == 20 );
    TESTCASE( memcmp( buffer, message, 20 ) == 0 );
    TESTCASE( buffer[20] == 'x' );
    /* same, different nmemb / size settings */
    rewind( fh );
    TESTCASE( memset( buffer, '\0', 20 ) == buffer );
    TESTCASE( fwrite( message, 5, 4, fh ) == 4 );
    rewind( fh );
    TESTCASE( fread( buffer, 5, 4, fh ) == 4 );
    TESTCASE( memcmp( buffer, message, 20 ) == 0 );
    TESTCASE( buffer[20] == 'x' );
    /* same... */
    rewind( fh );
    TESTCASE( memset( buffer, '\0', 20 ) == buffer );
    TESTCASE( fwrite( message, 20, 1, fh ) == 1 );
    rewind( fh );
    TESTCASE( fread( buffer, 20, 1, fh ) == 1 );
    TESTCASE( memcmp( buffer, message, 20 ) == 0 );
    TESTCASE( buffer[20] == 'x' );
    /* a character pushed back with ungetc() comes first */
    rewind( fh );
    TESTCASE( fgetc( fh ) == 'T' );
    TESTCASE( ungetc( 'T', fh ) == 'T' );
    TESTCASE( memset( buffer, '\0', 20 ) == buffer );
    TESTCASE( fread( buffer, 1, 20, fh ) == 20 );
    TESTCASE( memcmp( buffer, message, 20 ) == 0 );
    /* more than the buffer holds: read straight into the caller's memory */
    {
        static char big[ 3 * BUFSIZ + 5 ];
        static char back[ 3 * BUFSIZ + 5 ];
        size_t i;
        for ( i = 0; i < sizeof( big ); ++i )
        {
            big[ i ] = (char)( i * 7 );
        }
        rewind( fh );
        TESTCASE( fwrite( big, 1, sizeof( big ), fh ) == sizeof( big ) );
        rewind( fh );
        TESTCASE( fread( back, 1, sizeof( back ), fh ) == sizeof( back ) );
        TESTCASE( memcmp( back, big, sizeof( big ) ) == 0 );
        TESTCASE( ftell( fh ) == (long)sizeof( big ) );
    }
    /* Done. */
    TESTCASE( fclose( fh ) == 0 );
    return TEST_RESULTS;
}

#endif
