#include <math.h>
#include <fenv.h>

//#pragma STDC FENV_ACCESS ON // Causes "'#pragma STDC FENV_ACCESS ON' is illegal when precise is disabled"

double nearbyint(double x)
{
    int inexact_set;

    inexact_set = fetestexcept(FE_INEXACT);
    x = rint(x);
    if (!inexact_set) feclearexcept(FE_INEXACT);

    return x;
}

float nearbyintf(float x)
{
    int inexact_set;

    inexact_set = fetestexcept(FE_INEXACT);
    x = rintf(x);
    if (!inexact_set) feclearexcept(FE_INEXACT);

    return x;
}

long double nearbyintl(long double x)
{
    int inexact_set;

    inexact_set = fetestexcept(FE_INEXACT);
    x = rintl(x);
    if (!inexact_set) feclearexcept(FE_INEXACT);

    return x;
}
