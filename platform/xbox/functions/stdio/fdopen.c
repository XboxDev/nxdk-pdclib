/* fdopen( int, const char * )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef REGTEST

#include "pdclib/_PDCLIB_glue.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#ifndef __STDC_NO_THREADS__
#include <threads.h>
#endif

/**
 * @brief Associate a stream with an existing descriptor without reopening it.
 * @param fd Open descriptor; ownership transfers only on success.
 * @param mode Compatible stdio mode; w does not truncate an existing descriptor.
 * @return Stream closed by fclose, or NULL with errno set and fd still owned by caller.
 */
struct _PDCLIB_file_t * fdopen( int fd, const char * mode )
{
    unsigned int status = mode == NULL ? 0 : _PDCLIB_filemode( mode );
    if ( status == 0 )
    {
        errno = EINVAL;
        return NULL;
    }
    int flags = fcntl( fd, F_GETFL );
    if ( flags < 0 )
    {
        return NULL;
    }
    int access = status & _PDCLIB_FRW ? O_RDWR : status & _PDCLIB_FREAD ? O_RDONLY : O_WRONLY;
    if ( ( flags & access ) != access )
    {
        errno = EINVAL;
        return NULL;
    }
    struct _PDCLIB_file_t * stream = calloc( 1, sizeof( struct _PDCLIB_file_t ) + _PDCLIB_UNGETCBUFSIZE + BUFSIZ );
    if ( stream == NULL )
    {
        errno = ENOMEM;
        return NULL;
    }
#ifndef __STDC_NO_THREADS__
    if ( mtx_init( &stream->mtx, mtx_plain | mtx_recursive ) != thrd_success )
    {
        free( stream );
        errno = ENOMEM;
        return NULL;
    }
#endif
    if ( ( status & _PDCLIB_FAPPEND ) && fcntl( fd, F_SETFL, flags | O_APPEND ) < 0 )
    {
#ifndef __STDC_NO_THREADS__
        mtx_destroy( &stream->mtx );
#endif
        free( stream );
        return NULL;
    }
    int saved_errno = errno;
    off_t offset = lseek( fd, 0, SEEK_CUR );
    errno = saved_errno;
    stream->handle = fd;
    stream->ungetbuf = (unsigned char *)stream + sizeof( struct _PDCLIB_file_t );
    stream->buffer = (char *)stream->ungetbuf + _PDCLIB_UNGETCBUFSIZE;
    stream->bufsize = BUFSIZ;
    stream->status = status | _IOFBF | ( flags & O_APPEND ? _PDCLIB_FAPPEND : 0 );
    stream->pos.offset = offset < 0 ? 0 : offset;
    if ( stream->status & _PDCLIB_FRW )
    {
        stream->status &= ~( _PDCLIB_FREAD | _PDCLIB_FWRITE );
    }
    _PDCLIB_setstream( stream );
    return stream;
}

#endif
