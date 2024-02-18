#ifndef POSIX_POLYFILL_H
#define POSIX_POLYFILL_H

#ifndef _WIN32
// POSIX already provides these functions, just include unistd
#include <unistd.h>
#else
// wrap the "Low-Level I/O API" provided by Microsoft CRT, which exposes
//  a POSIX-like wrapper over Win32
#include <io.h>

#define open _open
#define close _close
#define read _read
#define write _write
#define ftruncate _chsize_s

#endif // _WIN32

#endif // POSIX_POLYFILL_H
