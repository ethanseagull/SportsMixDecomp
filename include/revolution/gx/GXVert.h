#ifndef REVOLUTION_GX_GXVERT_H_
#define REVOLUTION_GX_GXVERT_H_

// Immediate vertex data: GX takes it through the write-gather pipe, a
// hardware register at 0xCC008000 that every write goes to in order. The
// game's code writes it directly (lis 0xCC01, stores at -0x8000), as the
// inline functions of the GX interface do; these are written here for this
// project from that use.

typedef union {
    unsigned char u8;
    unsigned short u16;
    unsigned long u32;
    float f32;
} PPCWGPipe;

#define GXWGFifo (*(volatile PPCWGPipe *)0xCC008000)

static inline void GXPosition2f32(float x, float y)
{
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
}

static inline void GXTexCoord2f32(float s, float t)
{
    GXWGFifo.f32 = s;
    GXWGFifo.f32 = t;
}

static inline void GXColor1u32(unsigned long rgba)
{
    GXWGFifo.u32 = rgba;
}

#endif /* !REVOLUTION_GX_GXVERT_H_ */
