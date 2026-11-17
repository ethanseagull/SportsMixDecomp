#ifndef REVOLUTION_OS_OSALARM_H_
#define REVOLUTION_OS_OSALARM_H_

#include "revolution/os/OSTime.h"

#ifdef __cplusplus
extern "C" {
#endif

// The SDK's alarms (timer callbacks). Added for the DOL's
// game/bcs/CMainApp.cpp; only what it uses. The alarm is opaque here: only
// its size (0x30 bytes) matters to the game.
typedef struct OSContext OSContext;

typedef struct OSAlarm {
    unsigned char m_Unk00[0x30];
} OSAlarm;

typedef void (*OSAlarmHandler)(OSAlarm *alarm, OSContext *context);

void OSCreateAlarm(OSAlarm *alarm);
void OSSetPeriodicAlarm(OSAlarm *alarm, OSTime start, OSTime period, OSAlarmHandler handler);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_OS_OSALARM_H_ */
