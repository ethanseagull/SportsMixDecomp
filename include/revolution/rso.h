#ifndef REVOLUTION_RSO_H_
#define REVOLUTION_RSO_H_

// The relocatable module (RSO) loader of the SDK. Only what the game's code
// uses so far (added for DOL game/bcs/CRsoModule.cpp). Field offsets are
// those bcs::CRsoModule reads; the fields it does not touch are padding here.

#ifdef __cplusplus
extern "C" {
#endif

// The header a module starts with.
typedef struct RSOObjectHeader {
    unsigned char unk00[0x1C];
    unsigned long bssSize;       // 0x1C
    unsigned char unk20[0x04];
    void (*prolog)(void);        // 0x24
    void (*epilog)(void);        // 0x28
} RSOObjectHeader;

// Prepares the static module (the .sel file of exported symbols).
int RSOListInit(void *pStaticRso);
// (fn_80315B20) Links a module into the list, its .bss at pBss.
int RSOLinkList(RSOObjectHeader *pRso, void *pBss);
int RSOUnLinkList(RSOObjectHeader *pRso);
// Address of an exported symbol of a module, by name.
unsigned long RSOFindExportSymbolAddr(RSOObjectHeader *pRso, const char *pName);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_RSO_H_ */
