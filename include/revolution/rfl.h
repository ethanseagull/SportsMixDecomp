#ifndef REVOLUTION_RFL_H_
#define REVOLUTION_RFL_H_

// The Mii library (RFL). Only what the game's code passes around so far,
// written from the mangled names and the code that uses them; member names
// are guesses.

#include "revolution/gx/GXStruct.h"

#ifdef __cplusplus
extern "C" {
#endif

// Where a Mii's data comes from (passed and stored as a word).
typedef enum RFLDataSource {
    RFLDataSource_Official = 0,
    RFLDataSource_Controller1,
    RFLDataSource_Controller2,
    RFLDataSource_Controller3,
    RFLDataSource_Controller4,
    RFLDataSource_Middle,
    // The six default Miis (MenuF CCourtChr::CreateChrMenuObj takes the
    // index modulo 6 for this source).
    RFLDataSource_Default,
    // (added for DOL game/bcs/CMiiMan.cpp; name guessed) A middle database
    // given by the caller: RFLGetAdditionalInfo (fn_80336E20) reads the Mii
    // from its RFLMiddleDB argument for source 7, and bcs::CMiiMan passes 7
    // with the database it filled. (So the value 5 named Middle above is
    // probably something else; it is not used yet.)
    RFLDataSource_MiddleDB
} RFLDataSource;

// (added for DOL game/bcs/CMiiMan.cpp) What most RFL functions return.
// Only the values the game tests are named; names guessed, in the order
// the values suggest.
typedef enum RFLErrcode {
    RFLErrcode_Success = 0,
    RFLErrcode_NotAvailable = 1,
    // RFLGetAsyncStatus after loading: bcs::CMiiMan notes it and goes on.
    RFLErrcode_Loadfail = 3,
    // RFLInitRes: bcs::CMiiMan gives up waiting and goes on.
    RFLErrcode_Fatal = 5,
    // RFLGetAsyncStatus while still working.
    RFLErrcode_Busy = 6,
    // RFLGetStoreData's result for an empty slot: bcs::CMiiMan skips such
    // slots while looking for the next Mii.
    RFLErrcode_DBNodata = 10
} RFLErrcode;

// (added for DOL game/bcs/CMiiMan.cpp) The expression of a drawn face. The
// game hands it on from its callers as a word; no value named yet.
typedef long RFLExpression;

// (added for DOL game/bcs/CMiiMan.cpp; names guessed) Axes for
// RFLSetCoordinate: bcs::CMiiMan passes these two as the up and front axes.
typedef enum RFLCoordinate {
    RFLCoordinate_ZP = 0x00010000,
    RFLCoordinate_YP = 0x01000000
} RFLCoordinate;

// (added for DOL game/bcs/CMiiMan.cpp) The kind of an RFLMiddleDB; bcs::
// CMiiMan uses kind 4 (name guessed) for a one-Mii database it fills itself.
typedef enum RFLMiddleDBType {
    RFLMiddleDBType_UserSet = 4
} RFLMiddleDBType;

// (added for DOL game/bcs/CMiiMan.cpp) A small database of Miis kept by the
// caller, 0x18 bytes (bcs::CMiiMan embeds one at 0x38, its next member is at
// 0x50). Contents not used by the game's code.
typedef struct RFLMiddleDB {
    unsigned char data[0x18];
} RFLMiddleDB;

// (added for DOL game/bcs/CMiiMan.cpp) Extra data about a Mii (name, sex,
// colours...). (MenuF game/menu/CMiiSel.cpp) The Mii's name is at the
// start (copied out with wcscpy); the size, 0x3C, is from the stack frames
// of CMiiSel::SetMiiPage, which keeps two of them. The rest is not used by
// the game's own code.
typedef struct RFLAdditionalInfo {
    wchar_t name[11];             // 0x00
    unsigned char m_Unk16[0x1E];  // 0x16
    // 0x34. (added for MenuF game/menu/unit_6EE84.cpp; name guessed) The
    // top bit picks the Mii's voice (CSelectMenu::CharSel_CallMiiVoice), so
    // it is taken to be the sex; the rest of the word is not used.
    unsigned long sex : 1;
    unsigned long m_Unk34 : 31;
    unsigned char m_Unk38[0x4];   // 0x38
} RFLAdditionalInfo;

// Size 8. Identifies one Mii; copied as two words.
typedef struct RFLCreateID {
    unsigned char data[8];
} RFLCreateID;

// A Mii as stored, 0x4C bytes. Its contents are not used by the game's own
// code, so they are not spelled out here.
typedef struct RFLStoreData {
    unsigned char data[0x4C];
} RFLStoreData;

// (added for MenuF game/menu/CFriendList.cpp) How a Mii's face is drawn
// into a texture, 0x10 bytes, as the caller fills it: the size in texels,
// the kind of background (1 there) and its colour, and a flag (0 there).
typedef struct RFLIconSetting {
    unsigned short width;   // 0x00
    unsigned short height;  // 0x02
    long bgType;            // 0x04
    GXColor bgColor;        // 0x08
    long drawXluOnly;       // 0x0C
} RFLIconSetting;

// (added for DOL game/bcs/CMiiMan.cpp) RFL functions. Names are the SDK's
// where the behaviour makes them evident; marked where guessed.
RFLErrcode RFLGetStoreData(RFLStoreData *pData, RFLDataSource source, unsigned short index);
// fn_80335B40: looks createId up in the official database (100 slots); non-zero
// and *pIndex set when found.
int RFLSearchOfficialData(const RFLCreateID *pId, unsigned short *pIndex);
// fn_80336620 (name guessed): the same over the six default Miis.
int RFLSearchDefaultData(const RFLCreateID *pId, unsigned short *pIndex);
// fn_803362D0
void RFLInitMiddleDB(RFLMiddleDB *pDB, RFLMiddleDBType type, void *pBuffer, unsigned short size);
// fn_80336540
RFLErrcode RFLAddMiddleDBStoreData(RFLMiddleDB *pDB, const RFLStoreData *pData);
// fn_80336F30
RFLErrcode RFLGetAdditionalInfo(RFLAdditionalInfo *pInfo, RFLDataSource source, const RFLMiddleDB *pDB,
                                unsigned short index);
// fn_8032C920: the size of the work memory RFLInitRes needs.
unsigned long RFLGetWorkSize(int bDeluxeTex);
// fn_8032C940: starts the library on pWork with the resource file at pRes.
RFLErrcode RFLInitRes(void *pWork, void *pRes, unsigned long nResSize, int bDeluxeTex);
// fn_8032D190: the state of the library's loading started by RFLInitRes.
RFLErrcode RFLGetAsyncStatus(void);
// fn_8032D320 (name guessed): the name of the resource file inside an
// archive ("/RFL_Res.dat").
const char *RFLGetArcFilePath(void);
// fn_8032FC10
void RFLSetCoordinate(RFLCoordinate up, RFLCoordinate front);
// fn_803347E0: draws a face icon into pBuffer.
RFLErrcode RFLMakeIcon(void *pBuffer, RFLDataSource source, const RFLMiddleDB *pDB, unsigned short index,
                       RFLExpression expression, const RFLIconSetting *pSetting);
// fn_803362C0: the buffer size of a middle database of nSize Miis.
unsigned long RFLGetMiddleDBBufferSize(unsigned short nSize);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_RFL_H_ */
