#if defined(_WIN32) && !defined(__MINGW32__)
    #include "sys_elf.w32.h"
#else
    #include "sys_elf.default.h"
#endif
