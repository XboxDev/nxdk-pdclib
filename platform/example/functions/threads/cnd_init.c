/* cnd_init( cnd_t * cond )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#ifndef REGTEST

#include <threads.h>

/* Implicitly casting the parameters. */
extern int pthread_cond_init( cnd_t *, _PDCLIB_cnd_attr_t * );

int cnd_init( cnd_t * cond )
{
    if ( pthread_cond_init( cond, NULL ) == 0 )
    {
        return thrd_success;
    }
    else
    {
        return thrd_error;
    }
}

#endif

#ifdef TEST

#include "_PDCLIB_test.h"

int main( void )
{
#ifndef REGTEST
    TESTCASE( NO_TESTDRIVER );
#endif
    return TEST_RESULTS;
}

#endif
