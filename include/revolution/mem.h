#ifndef REVOLUTION_MEM_H_
#define REVOLUTION_MEM_H_

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far. The library is in the DOL; the allocator's
// contents are not needed yet.
typedef struct MEMAllocator MEMAllocator;

void *MEMAllocFromAllocator(MEMAllocator *pAllocator, unsigned long size);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_MEM_H_ */
