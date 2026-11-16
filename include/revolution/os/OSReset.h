#ifndef REVOLUTION_OS_OSRESET_H_
#define REVOLUTION_OS_OSRESET_H_

#ifdef __cplusplus
extern "C" {
#endif

// Leaving the game: power off, reset, the Wii Menu and the data management
// screen. Added for the DOL's game/bcs/CMainApp.cpp; signatures of the
// public SDK. Also the halt and panic the start-up code uses.
void OSShutdownSystem(void);
void OSRestart(unsigned long resetCode);
void OSReturnToMenu(void);
void OSReturnToDataManager(void);

void OSPanic(const char *file, int line, const char *msg, ...);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_OS_OSRESET_H_ */
