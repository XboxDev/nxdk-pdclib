/* localtime( const time_t * )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <time.h>
#include <timezoneapi.h>

#ifndef REGTEST

struct tm * localtime( const time_t * timer )
{
    TIME_ZONE_INFORMATION tzinfo;
    DWORD tz = GetTimeZoneInformation( &tzinfo );

    if ( tz == TIME_ZONE_ID_INVALID )
    {
        return NULL;
    }
    if ( tz == TIME_ZONE_ID_STANDARD )
    {
        tzinfo.Bias += tzinfo.StandardBias;
    }
    else if ( tz == TIME_ZONE_ID_DAYLIGHT )
    {
        tzinfo.Bias += tzinfo.DaylightBias;
    }

    time_t ltimer = *timer - tzinfo.Bias * 60;

    return gmtime( &ltimer );
}

#endif

#ifdef TEST

#include "_PDCLIB_test.h"

int main( void )
{
    TESTCASE( NO_TESTDRIVER );
    return TEST_RESULTS;
}

#endif
