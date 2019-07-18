#ifdef __EMSCRIPTEN__

#include <signal.h>

int sigaltstack(const stack_t *ss, stack_t *old_ss)
{
   return -1;
}

#endif
