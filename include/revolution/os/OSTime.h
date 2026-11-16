#ifndef REVOLUTION_OS_OSTIME_H_
#define REVOLUTION_OS_OSTIME_H_

// Only what the game's code uses so far (added for MenuF
// game/menu/CWiFiGame.cpp). The SDK's own time functions live in the DOL.

#ifdef __cplusplus
extern "C" {
#endif

typedef long long OSTime;

// The time base register: ticks since the console started.
OSTime OSGetTime(void);

// (added for the DOL's game/bcs/CMainApp.cpp) The bus clock the boot code
// stores in low memory, and the time base clock derived from it, as the
// SDK's macros define them.
#define OS_BUS_CLOCK (*(unsigned long *)0x800000F8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSMicrosecondsToTicks(usec) (((usec) * (OS_TIMER_CLOCK / 125000)) / 8)

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_OS_OSTIME_H_ */
