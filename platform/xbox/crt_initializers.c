#include <assert.h>

typedef int (__cdecl *_PIFV)(void);
typedef void (__cdecl *_PVFV)(void);

#pragma comment(linker, "/merge:.CRT=.rdata")

__attribute__((section(".CRT$XIA"))) _PIFV __xi_a[] = {0};
__attribute__((section(".CRT$XIZ"))) _PIFV __xi_z[] = {0};
__attribute__((section(".CRT$XCA"))) _PVFV __xc_a[] = {0};
__attribute__((section(".CRT$XCZ"))) _PVFV __xc_z[] = {0};
__attribute__((section(".CRT$XXA"))) _PVFV __xx_a[] = {0};
__attribute__((section(".CRT$XXZ"))) _PVFV __xx_z[] = {0};

void _PDCLIB_xbox_run_pre_initializers (void)
{
    for (_PVFV *pf = __xx_a; pf < __xx_z; pf++)
    {
        if (*pf)
        {
            (**pf)();
        }
    }
}

void _PDCLIB_xbox_run_crt_initializers (void)
{
    int ret = 0;

    // Run C initializers
    for (_PIFV *pf = __xi_a; pf < __xi_z && ret == 0; pf++)
    {
        if (*pf)
        {
            ret = (**pf)();
        }
    }
    assert(ret == 0);

    // Run C++ initializers
    for (_PVFV *pf = __xc_a; pf < __xc_z; pf++)
    {
        if (*pf)
        {
            (**pf)();
        }
    }
}
