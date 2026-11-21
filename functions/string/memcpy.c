/* memcpy( void *, const void *, size_t )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <string.h>
#include <stdint.h>

#ifndef REGTEST

#if defined( __i386__ )

/* Dwords with rep movsd, then the 0-3 bytes left. When source and destination
   share their misalignment and the copy is long enough to pay for it, the
   destination is first aligned to 8 bytes, which the processor's fast string
   copy wants.
*/
void * memcpy( void * _PDCLIB_restrict s1, const void * _PDCLIB_restrict s2, size_t n )
{
    unsigned char * dest = (unsigned char *) s1;
    const unsigned char * src = (const unsigned char *) s2;
    size_t words;
    size_t tail;

    if ( n >= 256 && ( ( (uintptr_t)dest ^ (uintptr_t)src ) & 7 ) == 0 )
    {
        size_t head = ( 0 - (uintptr_t)dest ) & 7;
        n -= head;
        while ( head-- )
        {
            *dest++ = *src++;
        }
    }

    words = n >> 2;
    tail = n & 3;
    __asm__ volatile( "rep movsl" : "+D"( dest ), "+S"( src ), "+c"( words ) : : "memory" );
    __asm__ volatile( "rep movsb" : "+D"( dest ), "+S"( src ), "+c"( tail ) : : "memory" );
    return s1;
}

#else

void * memcpy( void * _PDCLIB_restrict s1, const void * _PDCLIB_restrict s2, size_t n )
{
    char * dest = (char *) s1;
    const char * src = (const char *) s2;
    while ( n-- )
    {
        *dest++ = *src++;
    }
    return s1;
}

#endif

#endif

#ifdef TEST

#include "_PDCLIB_test.h"

int main( void )
{
    char s[] = "xxxxxxxxxxx";
    TESTCASE( memcpy( s, abcde, 6 ) == s );
    TESTCASE( s[4] == 'e' );
    TESTCASE( s[5] == '\0' );
    TESTCASE( memcpy( s + 5, abcde, 5 ) == s + 5 );
    TESTCASE( s[9] == 'e' );
    TESTCASE( s[10] == 'x' );
    /* every length from 0 to 299 at mixed alignments, and nothing outside it */
    {
        static unsigned char src[ 320 ];
        static unsigned char dst[ 320 ];
        size_t n;
        size_t i;
        int ok = 1;
        for ( i = 0; i < sizeof( src ); ++i )
        {
            src[ i ] = (unsigned char)( i * 7 + 1 );
        }
        for ( n = 0; n < 300; ++n )
        {
            size_t so = n % 8;
            size_t dof = ( n / 8 ) % 8;
            memset( dst, 0xCC, sizeof( dst ) );
            memcpy( dst + dof, src + so, n );
            for ( i = 0; i < sizeof( dst ); ++i )
            {
                unsigned char want = ( i >= dof && i < dof + n ) ? src[ so + i - dof ] : 0xCC;
                if ( dst[ i ] != want )
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
