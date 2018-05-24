#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

#if  __GNUC__ >= 7
#define FALLTROUGH  __attribute__((fallthrough));
#else
#define FALLTROUGH
#endif

std::uint32_t sign_extend(std::uint16_t);

#endif // UTILS_H
