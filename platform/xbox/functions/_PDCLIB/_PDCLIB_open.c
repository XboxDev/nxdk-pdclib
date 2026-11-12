/* _PDCLIB_open( const char * const, int )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <stdio.h>

#ifndef REGTEST

#include "pdclib/_PDCLIB_glue.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

_PDCLIB_fd_t _PDCLIB_open( const char * const filename, unsigned int mode )
{
    int flags;
    if ( mode & _PDCLIB_FRW )
    {
        flags = O_RDWR;
    }
    else
    {
        flags = mode & _PDCLIB_FREAD ? O_RDONLY : O_WRONLY;
    }
    if ( mode & _PDCLIB_FAPPEND )
    {
        flags |= O_CREAT | O_APPEND;
    }
    else if ( mode & _PDCLIB_FWRITE )
    {
        flags |= O_CREAT | O_TRUNC;
    }
    int fd = open( filename, flags, 0666 );
    if ( fd >= 0 && ( mode & _PDCLIB_FAPPEND ) && lseek( fd, 0, SEEK_END ) < 0 )
    {
        int saved_errno = errno;
        close( fd );
        errno = saved_errno;
        return _PDCLIB_NOHANDLE;
    }
    return fd;
}

#endif
