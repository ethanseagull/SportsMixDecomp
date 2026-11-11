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

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_OS_OSTIME_H_ */
