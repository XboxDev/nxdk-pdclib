#include <math.h>
#include <assert.h>

double hypot(double x, double y)
{
    return sqrt(x * x + y * y);
}

float hypotf(float x, float y)
{
    return sqrtf(x * x + y * y);
}

long double hypotl(long double x, long double y)
{
    return sqrtl(x * x + y * y);
}
