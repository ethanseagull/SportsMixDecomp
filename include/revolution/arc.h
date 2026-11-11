#ifndef REVOLUTION_ARC_H_
#define REVOLUTION_ARC_H_

// The archive (U8 .arc) reader of the SDK. Only what the game's code uses
// so far (added for DOL game/bcs/CMiiMan.cpp); member names are guesses,
// sizes from the stack frame of bcs::CMiiMan::Main.

#ifdef __cplusplus
extern "C" {
#endif

// An archive opened in memory, 0x1C bytes.
typedef struct ARCHandle {
    void *pArchiveStart;    // 0x00
    void *pFST;             // 0x04
    void *pFileStart;       // 0x08
    unsigned long nEntries; // 0x0C
    char *pStrings;         // 0x10
    unsigned long nFSTSize; // 0x14
    unsigned long nCurDir;  // 0x18
} ARCHandle;

// One file of an archive, 0xC bytes.
typedef struct ARCFileInfo {
    ARCHandle *pHandle;        // 0x00
    unsigned long nStart;      // 0x04
    unsigned long nLength;     // 0x08
} ARCFileInfo;

// fn_802C3280
int ARCInitHandle(void *pArchive, ARCHandle *pHandle);
int ARCOpen(ARCHandle *pHandle, const char *pName, ARCFileInfo *pInfo);
void *ARCGetStartAddrInMem(ARCFileInfo *pInfo);
// fn_802C3A90
unsigned long ARCGetLength(ARCFileInfo *pInfo);
// fn_802C3AA0
int ARCClose(ARCFileInfo *pInfo);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_ARC_H_ */
