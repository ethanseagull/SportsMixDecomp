#ifndef NW4R_LYT_LAYOUT_H_
#define NW4R_LYT_LAYOUT_H_

#include <new>

#include "nw4r/db/Warning.hpp"
#include "revolution/mem.h"

// The name of this file is part of the binary: Layout::AllocMemory passes
// __FILE__ ("layout.h") to its warning. Written for this project from the
// code MenuF game/menu/CBackground.cpp emits for it (the two strings are
// @STRING@AllocMemory__Q34nw4r3lyt6LayoutFUl objects); the names follow the
// public ogws decompilation's description of nw4r::lyt::Layout (no code
// copied).

namespace nw4r {
    namespace lyt {

        // Lives in the DOL. Only the static allocation helpers so far; the
        // object itself (0x20 bytes, root pane at 0x10) is not described.
        class Layout {
            public:
                static MEMAllocator *mspAllocator;

                static void *AllocMemory(unsigned long size)
                {
                    void *pMem = MEMAllocFromAllocator(mspAllocator, size);
// The original has this check on line 73 (the value passed in r4).
#line 73
                    NW4R_WARNING(pMem, "can't alloc memory.");
                    return pMem;
                }

                // A new T built in memory from mspAllocator; null when the
                // allocation fails.
                template <typename T>
                static T *NewObj()
                {
                    void *pMem = AllocMemory(sizeof(T));
                    if (pMem == 0) {
                        return 0;
                    }
                    return new (pMem) T();
                }
        };
    }
}

#endif /* !NW4R_LYT_LAYOUT_H_ */
