#ifndef CACHE_H
#define CACHE_H

#include "memory.h"

class Cache : public MemoryAccess {
public:
    Cache(Memory *m);
};

class CacheAssociative : public Cache {
public:
    CacheAssociative(Memory *m);
};

// TODO other chaches

#endif // CACHE_H
