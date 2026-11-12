/* fileno( FILE * )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <stdio.h>

#ifndef REGTEST

#include <errno.h>
#include <fcntl.h>
#ifndef __STDC_NO_THREADS__
#include <threads.h>
#endif

/**
 * @brief Return the descriptor owned by a stream without transferring ownership.
 * @param stream Open stream.
 * @return Descriptor, or -1 with errno set for an invalid stream or descriptor.
 */
int fileno( struct _PDCLIB_file_t * stream )
{
    if ( stream == NULL )
    {
        errno = EINVAL;
        return -1;
    }
    _PDCLIB_LOCK( stream->mtx );
    int fd = stream->handle;
    if ( fcntl( fd, F_GETFL ) < 0 )
    {
        fd = -1;
    }
    _PDCLIB_UNLOCK( stream->mtx );
    return fd;
}

#endif
