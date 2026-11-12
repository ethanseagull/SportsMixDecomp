#ifndef REVOLUTION_GD_H_
#define REVOLUTION_GD_H_

#include "revolution/gx/GXEnum.h"

// The GD library builds GX display lists in memory. Only what is used so
// far (added for the DOL's game/bcs/G2D/CLineStripDL.cpp). Names follow the
// public description of the GD interface; the inline writers are written
// for this project from what the game's code shows: every write goes
// through the current list's cursor (`__GDCurrentDL`, .sbss 0x805C1658),
// after a check that the bytes fit (calling GDOverflowed, 0x80315F00, when
// they do not). Multi-byte values are written big-endian, byte by byte.

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GDLObj {
    unsigned char *start;  // 0x00
    unsigned long length;  // 0x04
    unsigned char *ptr;    // 0x08 write cursor
    unsigned char *top;    // 0x0C end of the buffer
} GDLObj;

extern GDLObj *__GDCurrentDL;

void GDOverflowed(void);

#ifdef __cplusplus
}
#endif

inline void GDOverflowCheck(unsigned long size)
{
    if (__GDCurrentDL->ptr + size > __GDCurrentDL->top) {
        GDOverflowed();
    }
}

inline void __GDWrite(unsigned char data)
{
    *__GDCurrentDL->ptr++ = data;
}

inline void GDWrite_u8(unsigned char data)
{
    GDOverflowCheck(1);
    __GDWrite(data);
}

inline void GDWrite_u16(unsigned short data)
{
    GDOverflowCheck(2);
    __GDWrite((unsigned char)(data >> 8));
    __GDWrite((unsigned char)data);
}

inline void GDWrite_u32(unsigned long data)
{
    GDOverflowCheck(4);
    __GDWrite((unsigned char)(data >> 24));
    __GDWrite((unsigned char)(data >> 16));
    __GDWrite((unsigned char)(data >> 8));
    __GDWrite((unsigned char)data);
}

inline void GDWrite_f32(float data)
{
    union {
        float f;
        unsigned long u;
    } fid;
    fid.f = data;
    GDWrite_u32(fid.u);
}

inline void GDBegin(GXPrimitive type, GXVtxFmt vtxfmt, unsigned short nverts)
{
    GDWrite_u8((unsigned char)(type | vtxfmt));
    GDWrite_u16(nverts);
}

inline void GDPosition2f32(float x, float y)
{
    GDWrite_f32(x);
    GDWrite_f32(y);
}

inline void GDColor1u32(unsigned long clr)
{
    GDWrite_u32(clr);
}

#endif /* !REVOLUTION_GD_H_ */
