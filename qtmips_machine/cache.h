#ifndef CACHE_H
#define CACHE_H

#include <memory.h>
#include <machineconfig.h>

namespace machine {

class Cache : public MemoryAccess {
public:
    Cache(Memory *m, MachineConfigCache *c);
};

}

#endif // CACHE_H
