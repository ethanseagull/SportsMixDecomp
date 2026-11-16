#ifndef REVOLUTION_MEM_H_
#define REVOLUTION_MEM_H_

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far. The library is in the DOL.
typedef struct MEMAllocator MEMAllocator;

// (added for the DOL's game/bcs/G2D/CLytManager.cpp, which copies a heap's
// allocator and replaces its functions) The layout of the public MEM API.
typedef void *(*MEMFuncAllocatorAlloc)(MEMAllocator *pAllocator, unsigned long size);
typedef void (*MEMFuncAllocatorFree)(MEMAllocator *pAllocator, void *pMem);

typedef struct MEMAllocatorFunc {
    MEMFuncAllocatorAlloc pfAlloc; // 0x00
    MEMFuncAllocatorFree pfFree;   // 0x04
} MEMAllocatorFunc;

struct MEMAllocator {
    const MEMAllocatorFunc *pFunc; // 0x00
    void *pHeap;                   // 0x04
    unsigned long heapParam1;      // 0x08
    unsigned long heapParam2;      // 0x0C
};

void *MEMAllocFromAllocator(MEMAllocator *pAllocator, unsigned long size);
// Added for HockeyF game/hockey/CRadarIcon2DHockeyGoal.cpp.
void MEMFreeToAllocator(MEMAllocator *pAllocator, void *pMem);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_MEM_H_ */
