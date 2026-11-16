#ifndef REVOLUTION_OS_OSCACHE_H_
#define REVOLUTION_OS_OSCACHE_H_

// Only what the game's code uses so far (added for DOL game/bcs/CMovie.cpp).

#ifdef __cplusplus
extern "C" {
#endif

// Turns the locked cache (scratch memory at 0xE0000000) on.
void LCEnable(void);

// (added for DOL game/bcs/EF/CResource.cpp) Data cache maintenance of a
// range of memory.
void DCInvalidateRange(void *pAddr, unsigned long nBytes);
void DCFlushRange(void *pAddr, unsigned long nBytes);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_OS_OSCACHE_H_ */
