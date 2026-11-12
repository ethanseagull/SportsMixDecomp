#ifndef REVOLUTION_TPL_H_
#define REVOLUTION_TPL_H_

#include "revolution/gx/GXTexture.h"

#ifdef __cplusplus
extern "C" {
#endif

// The RVL SDK's texture palette (.tpl) functions; only what the game uses.
// Declarations written for this project from the calls in the DOL.
typedef struct TPLPalette TPLPalette;

// (fn_80337000, unnamed in the DOL; named after the SDK function its
// neighbours TPLGet and TPLGetGXTexObjFromPalette belong with) Turns the
// offsets of a loaded palette into pointers.
void TPLBind(TPLPalette *pPal);
void TPLGetGXTexObjFromPalette(TPLPalette *pPal, GXTexObj *pObj, unsigned long nId);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_TPL_H_ */
