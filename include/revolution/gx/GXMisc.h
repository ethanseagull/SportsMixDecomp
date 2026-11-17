#ifndef REVOLUTION_GX_GXMISC_H_
#define REVOLUTION_GX_GXMISC_H_

#ifdef __cplusplus
extern "C" {
#endif

// GX set-up and synchronisation. Added for the DOL's game/bcs/CMainApp.cpp;
// signatures of the public GX API. The FIFO object is opaque here.
typedef struct _GXFifoObj GXFifoObj;

GXFifoObj *GXInit(void *base, unsigned long size);
void GXDrawDone(void);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_GX_GXMISC_H_ */
