#ifndef REVOLUTION_VI_H_
#define REVOLUTION_VI_H_

#include "revolution/gx/GXStruct.h"

#ifdef __cplusplus
extern "C" {
#endif

// The video interface. Added for the DOL's game/bcs/CMainApp.cpp; only what
// it uses, with the values and signatures of the public SDK. Some of the
// functions have no name in the DOL yet and are declared where they are
// called.

// What VIGetTvFormat returns.
enum {
    VI_NTSC,
    VI_PAL,
    VI_MPAL,
    VI_DEBUG,
    VI_DEBUG_PAL,
    VI_EURGB60
};

// What VIGetScanMode returns.
enum {
    VI_INTERLACE,
    VI_NON_INTERLACE,
    VI_PROGRESSIVE
};

void VIInit(void);
void VIConfigure(const GXRenderModeObj *rm);
void VISetNextFrameBuffer(void *fb);
void VISetBlack(int black);
void VIWaitForRetrace(void);
unsigned long VIGetTvFormat(void);
unsigned long VIGetScanMode(void);
unsigned long VIGetDTVStatus(void);
unsigned long VIGetNextField(void);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_VI_H_ */
