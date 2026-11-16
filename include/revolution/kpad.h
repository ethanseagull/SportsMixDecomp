#ifndef REVOLUTION_KPAD_H_
#define REVOLUTION_KPAD_H_

#ifdef __cplusplus
extern "C" {
#endif

// The RVL SDK's Wii remote reading library; only what the game uses.
// Declarations written for this project from the calls in the DOL and the
// field offsets they read.

typedef struct Vec2 {
    float x; // 0x00
    float y; // 0x04
} Vec2;

typedef struct KPADRect {
    float left;   // 0x00
    float top;    // 0x04
    float right;  // 0x08
    float bottom; // 0x0C
} KPADRect;

// One reading of a remote (KPADRead fills up to ten per frame). 0xF0 bytes.
typedef struct KPADStatus {
    unsigned long hold;       // 0x00
    unsigned long trig;       // 0x04
    unsigned long release;    // 0x08
    unsigned char unk0C[0x20 - 0x0C];
    Vec2 pos;                 // 0x20 pointer position, -1..1
    unsigned char unk28[0x5C - 0x28];
    unsigned char dev_type;   // 0x5C
    signed char wpad_err;     // 0x5D
    signed char dpd_valid_fg; // 0x5E
    unsigned char data_format; // 0x5F
    unsigned char unk60[0xF0 - 0x60];
} KPADStatus;

void KPADEnableDPD(long chan);
void KPADDisableDPD(long chan);
// dst = the pointer position src mapped into the rectangle, for a viewport
// of width ratio viRatio.
void KPADGetProjectionPos(Vec2 *dst, const Vec2 *src, const KPADRect *rect, float viRatio);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_KPAD_H_ */
