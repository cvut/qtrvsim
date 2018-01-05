#ifndef UTILS_H
#define UTILS_H

#if  __GNUC__ >= 7
#define FALLTROUGH  __attribute__((fallthrough));
#else
#define FALLTROUGH
#endif

#endif // UTILS_H
