#include <assert.h>
#include <stdlib.h>

#include <hal/debug.h>

#include <xboxkrnl/xboxkrnl.h>

void _xbox_assert(char const * const expression, char const * const file_name, char const * const function_name, unsigned long line)
{
#ifdef DEBUG_CONSOLE
    char buffer[512];
    snprintf(buffer, 512, "In function '%s': ", function_name);
    RtlAssert((PVOID)expression, (PVOID)file_name, line, buffer);
#else
    debugPrint("\nAssertion failed: '%s' in function '%s', file '%s', line %lu\n", expression, function_name, file_name, line);
    __asm__ ("cli\n"
             "1:\n"
             "hlt\n"
             "jmp 1b\n");
#endif
}
