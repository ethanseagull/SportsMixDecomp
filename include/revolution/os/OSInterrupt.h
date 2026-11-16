#ifndef REVOLUTION_OS_OSINTERRUPT_H_
#define REVOLUTION_OS_OSINTERRUPT_H_

// Only what the game's code uses so far (added for DOL game/bcs/CMovie.cpp).

#ifdef __cplusplus
extern "C" {
#endif

typedef int BOOL;

// Turns interrupts off; returns whether they were on.
BOOL OSDisableInterrupts(void);
// Puts back the state OSDisableInterrupts returned.
BOOL OSRestoreInterrupts(BOOL level);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_OS_OSINTERRUPT_H_ */
