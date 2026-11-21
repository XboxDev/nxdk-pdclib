/* fwrite( const void *, size_t, size_t, FILE * )

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

size_t fwrite( const void * _PDCLIB_restrict ptr, size_t size, size_t nmemb, struct _PDCLIB_file_t * _PDCLIB_restrict stream )
{
    const char * src = (const char *)ptr;
    size_t total;
    size_t done = 0;
    /* Line-buffered streams: the buffer position just after the last newline, 0 if none */
    _PDCLIB_size_t offset = 0;

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

    if ( _PDCLIB_prepwrite( stream ) == EOF )
    {
        _PDCLIB_UNLOCK( stream->mtx );
        return 0;
    }

    while ( done < total )
    {
        size_t left = total - done;
        size_t count;

        if ( stream->bufidx == 0 && left >= stream->bufsize )
        {
            /* At least a buffer's worth into an empty buffer: straight from the caller's memory, in one write
               instead of one per buffer-full, and without the copy. Line-buffered streams too: writing
               everything writes every complete line.
            */
            done += _PDCLIB_writedirect( stream, src + done, left );
            if ( done < total )
            {
                /* Write error: the elements written completely */
                _PDCLIB_UNLOCK( stream->mtx );
                return done / size;
            }
            break;
        }

        count = stream->bufsize - stream->bufidx;
        if ( count > left )
        {
            count = left;
        }
        memcpy( stream->buffer + stream->bufidx, src + done, count );

        if ( stream->status & _IOLBF )
        {
            /* Remember the last newline, in case we have to do a partial line-buffered flush */
            size_t i = count;
            while ( i > 0 )
            {
                if ( src[ done + --i ] == '\n' )
                {
                    offset = stream->bufidx + i + 1;
                    break;
                }
            }
        }

        stream->bufidx += count;
        done += count;

        if ( stream->bufidx == stream->bufsize )
        {
            if ( _PDCLIB_flushbuffer( stream ) == EOF )
            {
                /* Returning number of objects completely buffered */
                _PDCLIB_UNLOCK( stream->mtx );
                return ( done - count ) / size;
            }
            offset = 0;
        }
    }

    /* Fully-buffered streams are OK. Non-buffered streams must be flushed,
       line-buffered streams only if there's a newline in the buffer.
    */
    switch ( stream->status & ( _IONBF | _IOLBF ) )
    {
        case _IONBF:
            if ( _PDCLIB_flushbuffer( stream ) == EOF )
            {
                /* We are in a pinch here. We have an error, which requires a
                   return value < nmemb. On the other hand, all objects have
                   been written to buffer, which means all the caller had to
                   do was removing the error cause, and re-flush the stream...
                   Catch 22. We'll return a value one short, to indicate the
                   error, and can't really do anything about the inconsistency.
                */
                _PDCLIB_UNLOCK( stream->mtx );
                return nmemb - 1;
            }
            break;
        case _IOLBF:
            if ( offset > 0 )
            {
                size_t bufidx = stream->bufidx;
                stream->bufidx = offset;
                if ( _PDCLIB_flushbuffer( stream ) == EOF )
                {
                    /* See comment above. */
                    stream->bufidx = bufidx;
                    _PDCLIB_UNLOCK( stream->mtx );
                    return nmemb - 1;
                }
                stream->bufidx = bufidx - offset;
                memmove( stream->buffer, stream->buffer + offset, stream->bufidx );
            }
    }

    _PDCLIB_UNLOCK( stream->mtx );
    return nmemb;
}

#endif

#ifdef TEST

#include "_PDCLIB_test.h"

int main( void )
{
    /* Testing covered by fread(), except a write of more than the buffer
       holds, which goes straight from the caller's memory to the file.
    */
    FILE * fh;
    static char big[ 3 * BUFSIZ + 5 ];
    static char back[ 3 * BUFSIZ + 5 ];
    size_t i;
    for ( i = 0; i < sizeof( big ); ++i )
    {
        big[ i ] = (char)( i * 7 );
    }
    TESTCASE( ( fh = tmpfile() ) != NULL );
    TESTCASE( fwrite( "x", 1, 1, fh ) == 1 );
    TESTCASE( fwrite( big, 1, sizeof( big ), fh ) == sizeof( big ) );
    TESTCASE( ftell( fh ) == (long)sizeof( big ) + 1 );
    rewind( fh );
    TESTCASE( fgetc( fh ) == 'x' );
    TESTCASE( fread( back, 1, sizeof( back ), fh ) == sizeof( back ) );
    TESTCASE( memcmp( back, big, sizeof( big ) ) == 0 );
    TESTCASE( fclose( fh ) == 0 );
    return TEST_RESULTS;
}

#endif
