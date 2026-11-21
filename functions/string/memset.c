/* memset( void *, int, size_t )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <string.h>
#include <stdint.h>

#ifndef REGTEST

#if defined( __i386__ )

/* Dwords with rep stosd (the byte repeated four times), then the 0-3 bytes
   left; long fills first align the destination to 8 bytes.
*/
void * memset( void * s, int c, size_t n )
{
    unsigned char * p = (unsigned char *) s;
    unsigned char b = (unsigned char) c;
    uint32_t v;
    size_t words;
    size_t tail;

    if ( n >= 256 )
    {
        size_t head = ( 0 - (uintptr_t)p ) & 7;
        n -= head;
        while ( head-- )
        {
            *p++ = b;
        }
    }

    v = b * 0x01010101u;
    words = n >> 2;
    tail = n & 3;
    __asm__ volatile( "rep stosl" : "+D"( p ), "+c"( words ) : "a"( v ) : "memory" );
    while ( tail-- )
    {
        *p++ = b;
    }
    return s;
}

#else

void * memset( void * s, int c, size_t n )
{
    unsigned char * p = (unsigned char *) s;
    while ( n-- )
    {
        *p++ = (unsigned char) c;
    }
    return s;
}

#endif

#endif

#ifdef TEST

#include "_PDCLIB_test.h"

int main( void )
{
    char s[] = "xxxxxxxxx";
    TESTCASE( memset( s, 'o', 10 ) == s );
    TESTCASE( s[9] == 'o' );
    TESTCASE( memset( s, '_', (0) ) == s );
    TESTCASE( s[0] == 'o' );
    TESTCASE( memset( s, '_', 1 ) == s );
    TESTCASE( s[0] == '_' );
    TESTCASE( s[1] == 'o' );
    /* every length from 0 to 299 at mixed alignments, and nothing outside it */
    {
        static unsigned char buf[ 320 ];
        size_t n;
        size_t i;
        int ok = 1;
        for ( n = 0; n < 300; ++n )
        {
            size_t at = n % 8;
            memset( buf, 0xCC, sizeof( buf ) );
            memset( buf + at, (int)( 0x100 + n ), n );
            for ( i = 0; i < sizeof( buf ); ++i )
            {
                unsigned char want = ( i >= at && i < at + n ) ? (unsigned char) n : 0xCC;
                if ( buf[ i ] != want )
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
