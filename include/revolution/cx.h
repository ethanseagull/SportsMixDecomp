#ifndef REVOLUTION_CX_H_
#define REVOLUTION_CX_H_

// The SDK's decompression library. Only what the game's code uses so far
// (added for DOL game/bcs/EF/CResource.cpp); declarations of the public
// API, the bodies are in main.dol.

#ifdef __cplusplus
extern "C" {
#endif

// The size of the data the compressed block pSrc unpacks to.
unsigned long CXGetUncompressedSize(const void *pSrc);
// Unpacks LH-compressed pSrc into pDst; pWork is a 0x880-byte work area.
void CXUncompressLH(const void *pSrc, void *pDst, void *pWork);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_CX_H_ */
