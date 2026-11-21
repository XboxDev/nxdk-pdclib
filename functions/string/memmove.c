/* memmove( void *, const void *, size_t )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <string.h>
#include <stdint.h>

#ifndef REGTEST

#if defined( __i386__ )

/* Forward copies go to memcpy(). A destination that starts inside the source
   is copied backward: the last 0-3 bytes, then dwords with the direction flag
   set (cleared again before returning, as the calling convention requires).
*/
void * memmove( void * s1, const void * s2, size_t n )
{
    unsigned char * dest = (unsigned char *) s1;
    const unsigned char * src = (const unsigned char *) s2;
    size_t words;
    size_t tail;

    if ( dest <= src || dest >= src + n )
    {
        return memcpy( s1, s2, n );
    }

    dest += n;
    src += n;
    tail = n & 3;
    while ( tail-- )
    {
        *--dest = *--src;
    }

    words = n >> 2;
    if ( words )
    {
        dest -= 4;
        src -= 4;
        __asm__ volatile( "std\n\trep movsl\n\tcld" : "+D"( dest ), "+S"( src ), "+c"( words ) : : "memory" );
    }
    return s1;
}

#else

void * memmove( void * s1, const void * s2, size_t n )
{
    char * dest = (char *) s1;
    const char * src = (const char *) s2;
    if ( dest <= src )
    {
        while ( n-- )
        {
            *dest++ = *src++;
        }
    }
    else
    {
        src += n;
        dest += n;
        while ( n-- )
        {
            *--dest = *--src;
        }
    }
    return s1;
}

#endif

#endif

#ifdef TEST

#include "_PDCLIB_test.h"

int main( void )
{
    char s[] = "xxxxabcde";
    TESTCASE( memmove( s, s + 4, 5 ) == s );
    TESTCASE( s[0] == 'a' );
    TESTCASE( s[4] == 'e' );
    TESTCASE( s[5] == 'b' );
    TESTCASE( memmove( s + 4, s, 5 ) == s + 4 );
    TESTCASE( s[4] == 'a' );
    /* overlapping moves of every length from 0 to 299, both directions */
    {
        static unsigned char buf[ 400 ];
        static unsigned char want[ 400 ];
        size_t n;
        size_t i;
        int ok = 1;
        for ( n = 0; n < 300; ++n )
        {
            size_t from = 40 + n % 9;
            size_t to = ( n & 1 ) ? from + 1 + n % 37 : from - 1 - n % 37;
            for ( i = 0; i < sizeof( buf ); ++i )
            {
                buf[ i ] = (unsigned char)( i * 13 + n );
                want[ i ] = buf[ i ];
            }
            for ( i = 0; i < n; ++i )
            {
                want[ to + i ] = buf[ from + i ];
            }
            memmove( buf + to, buf + from, n );
            for ( i = 0; i < sizeof( buf ); ++i )
            {
                if ( buf[ i ] != want[ i ] )
                {
                    ok = 0;
                }
            }
        }
        TESTCASE( ok );
    }
    return TEST_RESULTS;
}

#endif
