#include "game/msw/SelectMenu/CCharSel.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "bcs/New.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/SelectMenu/CChrSign.hpp"
#include "game/msw/SelectMenu/CCourtChr.hpp"
#include "game/msw/SelectMenu/CPlusMinus.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "string.h"

// The names in this file are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the window's opening and closing animations. A
    // table, not a scalar, as in CMiiSel: the original converts the value
    // to float at run time and keeps the word first in .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // The four layouts of the screen, one per number of open characters.
    // Each has its layout and button files, the animations its four pad
    // colours give a button, a table of pane and animation ids (indexed by
    // the ID_* values below), the panes shown for one to four players, the
    // first and the last row of character buttons, and all its character
    // buttons by row.

    const char *const s_LayoutNameA = "me_cha_A.brlyt";
    const char *const s_ButtonFileNameA = "me_cha_A.btn";
    const unsigned char s_PadAnimA[4] = { 8, 9, 10, 11 };
    const long s_NoPadAnimA = 7;
    const long s_IdA[] = {
        0x27, 0x1D, 0x1E, 0x1B, 0x1C, 0x48, 0x49, 0x4A, 0x4B,
        0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x05, 0x06, 0x00
    };
    const long s_PlayerPaneA[4][4] = {
        { 0x26, -1, -1, -1 }, { 0x24, 0x25, -1, -1 }, { 0x20, 0x21, 0x25, -1 }, { 0x20, 0x21, 0x22, 0x23 }
    };
    const long s_TopRowA[6] = { 0x8010, 0x8011, 0x8012, 0x8013, -1, -1 };
    const long s_BottomRowA[6] = { 0x8014, 0x8015, 0x8016, 0x8017, -1, -1 };
    const long s_ButtonA[2][4] = {
        { 0x8010, 0x8011, 0x8012, 0x8013 }, { 0x8014, 0x8015, 0x8016, 0x8017 }
    };

    const char *const s_LayoutNameB = "me_cha_B.brlyt";
    const char *const s_ButtonFileNameB = "me_cha_B.btn";
    const unsigned char s_PadAnimB[4] = { 8, 9, 10, 11 };
    const long s_NoPadAnimB = 7;
    const long s_IdB[] = {
        0x27, 0x1D, 0x1E, 0x1B, 0x1C, 0x58, 0x59, 0x5A, 0x5B,
        0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x05, 0x06, 0x00, 0x00
    };
    const long s_PlayerPaneB[4][4] = {
        { 0x26, -1, -1, -1 }, { 0x24, 0x25, -1, -1 }, { 0x20, 0x21, 0x25, -1 }, { 0x20, 0x21, 0x22, 0x23 }
    };
    const long s_TopRowB[6] = { 0x8010, 0x8011, 0x8012, 0x8013, -1, -1 };
    const long s_BottomRowB[6] = { 0x8018, 0x8019, 0x801A, 0x801B, -1, -1 };
    const long s_ButtonB[3][4] = {
        { 0x8010, 0x8011, 0x8012, 0x8013 }, { 0x8014, 0x8015, 0x8016, 0x8017 }, { 0x8018, 0x8019, 0x801A, 0x801B }
    };

    const char *const s_LayoutNameC = "me_cha_C.brlyt";
    const char *const s_ButtonFileNameC = "me_cha_C.btn";
    const unsigned char s_PadAnimC[4] = { 8, 9, 10, 11 };
    const long s_NoPadAnimC = 7;
    const long s_IdC[] = {
        0x63, 0x1D, 0x1E, 0x1B, 0x1C, 0x64, 0x65, 0x66, 0x67,
        0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x05, 0x06, 0x00, 0x00
    };
    const long s_PlayerPaneC[4][4] = {
        { 0x26, -1, -1, -1 }, { 0x24, 0x25, -1, -1 }, { 0x20, 0x21, 0x25, -1 }, { 0x20, 0x21, 0x22, 0x23 }
    };
    const long s_TopRowC[6] = { 0x8010, 0x8011, 0x8012, 0x8013, 0x8014, -1 };
    const long s_BottomRowC[6] = { 0x801A, 0x801B, 0x801C, 0x801D, 0x801E, -1 };
    const long s_ButtonC[3][5] = {
        { 0x8010, 0x8011, 0x8012, 0x8013, 0x8014 },
        { 0x8015, 0x8016, 0x8017, 0x8018, 0x8019 },
        { 0x801A, 0x801B, 0x801C, 0x801D, 0x801E }
    };

    const char *const s_LayoutNameD = "me_cha_D.brlyt";
    const char *const s_ButtonFileNameD = "me_cha_D.btn";
    const unsigned char s_PadAnimD[4] = { 8, 9, 10, 11 };
    const long s_NoPadAnimD = 7;
    const long s_IdD[] = {
        0x27, 0x1D, 0x1E, 0x1B, 0x1C, 0x70, 0x71, 0x72, 0x73,
        0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x05, 0x06, 0x00
    };
    const long s_PlayerPaneD[4][4] = {
        { 0x26, -1, -1, -1 }, { 0x24, 0x25, -1, -1 }, { 0x20, 0x21, 0x25, -1 }, { 0x20, 0x21, 0x22, 0x23 }
    };
    const long s_TopRowD[6] = { 0x8010, 0x8011, 0x8012, 0x8013, 0x8014, 0x8015 };
    const long s_BottomRowD[6] = { 0x801C, 0x801D, 0x801E, 0x801F, 0x8020, 0x8021 };
    const long s_ButtonD[3][6] = {
        { 0x8010, 0x8011, 0x8012, 0x8013, 0x8014, 0x8015 },
        { 0x8016, 0x8017, 0x8018, 0x8019, 0x801A, 0x801B },
        { 0x801C, 0x801D, 0x801E, 0x801F, 0x8020, 0x8021 }
    };

    // Indices into s_Id*.
    enum {
        ID_WIN_PANE = 0,       // the window
        ID_MII_PANE = 1,       // the two panes of the Mii button
        ID_MII_PANE_2 = 2,
        ID_MII_PARENT = 6,     // where those two are hung in the Mii mode
        ID_ANIM_IN_BACK = 9,   // the window coming in after the back button
        ID_ANIM_OUT_BACK = 10, // going out with the back button
        ID_ANIM_IN = 11,
        ID_ANIM_MII_IN = 12,   // the Mii button coming in
        ID_ANIM_OUT = 13,
        ID_ANIM_CANCEL = 15,   // a character button's animations
        ID_ANIM_PUSH = 16,
        ID_ANIM_WAIT = 17
    };

    // The default button of each entry.
    const long s_DefaultButton[4] = { 0x8010, 0x8011, 0x8012, 0x8013 };

    // The first of the voices of each character (three or four follow).
    struct CharaVoice {
        unsigned long m_nChara;
        long m_nVoice;
    };
    const CharaVoice s_CharaVoice[20] = {
        { 0x00, 0x070 }, { 0x01, 0x089 }, { 0x02, 0x0A2 }, { 0x03, 0x0BB }, { 0x04, 0x0D4 },
        { 0x05, 0x0ED }, { 0x06, 0x106 }, { 0x07, 0x11F }, { 0x08, 0x138 }, { 0x09, 0x151 },
        { 0x0A, 0x16A }, { 0x0B, 0x183 }, { 0x0C, 0x19C }, { 0x0D, 0x1B5 }, { 0x0E, 0x1CE },
        { 0x0F, 0x1E7 }, { 0x10, 0x200 }, { 0x11, 0x219 }, { 0x13, 0x232 }, { 0x14, 0x24B }
    };

    // The court place of each of the six character slots, by the number of
    // characters taking part (the pad signs, and the colour markers).
    const unsigned char s_PadSignPos[7][6] = {
        { 0, 1, 2, 3, 4, 5 }, { 0, 1, 2, 3, 4, 5 }, { 0, 1, 2, 3, 4, 5 }, { 0, 1, 2, 3, 4, 5 },
        { 1, 2, 0, 4, 5, 3 }, { 0, 1, 2, 3, 4, 5 }, { 0, 1, 2, 3, 4, 5 }
    };
    const unsigned char s_MarkerPos[7][6] = {
        { 0, 1, 2, 3, 4, 5 }, { 0, 1, 2, 3, 4, 5 }, { 0, 1, 2, 3, 4, 5 }, { 0, 1, 2, 3, 4, 5 },
        { 1, 2, 0, 4, 5, 3 }, { 0, 1, 2, 3, 4, 5 }, { 0, 1, 2, 3, 4, 5 }
    };

    // The group each character belongs to; the groups open decide the
    // layout (OpenCharCount).
    const long s_CharaGroup[19] = { 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 3, 3, 2, 2, 3, 2, -1 };

    // Size 0x10. Where character nChara is shown: its face pane (or the
    // panes of its faces, one per colour, ending with -1), its place on the
    // layout (-1 when it is not there) and its button.
    struct CharaPane {
        long m_nPane;
        long m_nPos;
        long m_nBtnKindId;
        long *m_pColorPane;
    };

    // Per layout, by place: the parent of the face pane, the parent of the
    // "star" and "new" marks, and the button pane.
    long s_FaceParentA[18] = { 0x2B, 0x2F, 0x33, 0x37, 0x3B, 0x3F, 0x43, 0x46, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    long s_MarkParentA[18] = { 0x2C, 0x30, 0x34, 0x38, 0x3C, 0x40, 0x44, 0x47, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    long s_ColorPane05A[] = { 0x05, 0x06, 0x07, -1 };
    long s_ColorPane0EA[] = { 0x0E, 0x0F, 0x10, 0x11, -1 };
    long s_ColorPane14A[] = { 0x14, 0x15, 0x17, 0x16, -1 };
    long s_ColorPane04A[] = { 0x04, 0x4C, -1 };
    long s_ColorPane0CA[] = { 0x0C, 0x4D, -1 };
    long s_ColorPane18A[] = { 0x18, 0x4E, -1 };
    CharaPane s_CharaPaneA[19] = {
        { 0x00, 0, 0x8010, 0 },  { 0x08, 4, 0x8014, 0 },  { 0x01, 1, 0x8011, 0 },  { 0x09, 5, 0x8015, 0 },
        { 0x0E, -1, -1, s_ColorPane0EA }, { 0x02, 2, 0x8012, 0 }, { 0x12, -1, -1, 0 }, { 0x0A, 6, 0x8016, 0 },
        { 0x03, 3, 0x8013, 0 }, { 0x14, -1, -1, s_ColorPane14A }, { 0x13, -1, -1, 0 }, { 0x0B, 7, 0x8017, 0 },
        { 0x0D, -1, -1, 0 }, { 0x19, -1, -1, 0 }, { 0x04, -1, -1, s_ColorPane04A }, { 0x0C, -1, -1, s_ColorPane0CA },
        { 0x05, -1, -1, s_ColorPane05A }, { 0x18, -1, -1, s_ColorPane18A }, { 0x1A, -1, -1, 0 }
    };
    long s_ButtonPaneA[18] = { 0x29, 0x2D, 0x31, 0x35, 0x39, 0x3D, 0x41, 0x45, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

    long s_FaceParentB[18] = { 0x2B, 0x2F, 0x33, 0x37, 0x3B, 0x3F, 0x43, 0x47, 0x4B, 0x4F, 0x53, 0x56, -1, -1, -1, -1, -1, -1 };
    long s_MarkParentB[18] = { 0x2C, 0x30, 0x34, 0x38, 0x3C, 0x40, 0x44, 0x48, 0x4C, 0x50, 0x54, 0x57, -1, -1, -1, -1, -1, -1 };
    long s_ColorPane05B[] = { 0x05, 0x06, 0x07, -1 };
    long s_ColorPane0EB[] = { 0x0E, 0x0F, 0x10, 0x11, -1 };
    long s_ColorPane14B[] = { 0x14, 0x15, 0x17, 0x16, -1 };
    long s_ColorPane04B[] = { 0x04, 0x5C, -1 };
    long s_ColorPane0CB[] = { 0x0C, 0x5D, -1 };
    long s_ColorPane18B[] = { 0x18, 0x5E, -1 };
    CharaPane s_CharaPaneB[19] = {
        { 0x00, 0, 0x8010, 0 },  { 0x08, 4, 0x8014, 0 },  { 0x01, 1, 0x8011, 0 },  { 0x09, 5, 0x8015, 0 },
        { 0x0E, 8, 0x8018, s_ColorPane0EB }, { 0x02, 2, 0x8012, 0 }, { 0x12, 9, 0x8019, 0 }, { 0x0A, 6, 0x8016, 0 },
        { 0x03, 3, 0x8013, 0 }, { 0x14, 11, 0x801B, s_ColorPane14B }, { 0x13, 10, 0x801A, 0 }, { 0x0B, 7, 0x8017, 0 },
        { 0x0D, -1, -1, 0 }, { 0x19, -1, -1, 0 }, { 0x04, -1, -1, s_ColorPane04B }, { 0x0C, -1, -1, s_ColorPane0CB },
        { 0x05, -1, -1, s_ColorPane05B }, { 0x18, -1, -1, s_ColorPane18B }, { 0x1A, -1, -1, 0 }
    };
    long s_ButtonPaneB[18] = { 0x29, 0x2D, 0x31, 0x35, 0x39, 0x3D, 0x41, 0x45, 0x49, 0x4D, 0x51, 0x55, -1, -1, -1, -1, -1, -1 };

    long s_FaceParentC[18] = { 0x29, 0x2D, 0x31, 0x35, 0x39, 0x3D, 0x41, 0x45, 0x49, 0x4D, 0x51, 0x55, 0x59, 0x5D, 0x61, -1, -1, -1 };
    long s_MarkParentC[18] = { 0x2A, 0x2E, 0x32, 0x36, 0x3A, 0x3E, 0x42, 0x46, 0x4A, 0x4E, 0x52, 0x56, 0x5A, 0x5E, 0x62, -1, -1, -1 };
    long s_ColorPane05C[] = { 0x05, 0x06, 0x07, -1 };
    long s_ColorPane0EC[] = { 0x0E, 0x0F, 0x10, 0x11, -1 };
    long s_ColorPane14C[] = { 0x14, 0x15, 0x17, 0x16, -1 };
    long s_ColorPane04C[] = { 0x04, 0x68, -1 };
    long s_ColorPane0CC[] = { 0x0C, 0x69, -1 };
    long s_ColorPane18C[] = { 0x18, 0x6A, -1 };
    CharaPane s_CharaPaneC[19] = {
        { 0x00, 0, 0x8010, 0 },  { 0x08, 5, 0x8015, 0 },  { 0x01, 1, 0x8011, 0 },  { 0x09, 6, 0x8016, 0 },
        { 0x0E, 10, 0x801A, s_ColorPane0EC }, { 0x02, 2, 0x8012, 0 }, { 0x12, 11, 0x801B, 0 }, { 0x0A, 7, 0x8017, 0 },
        { 0x03, 3, 0x8013, 0 }, { 0x14, 13, 0x801D, s_ColorPane14C }, { 0x13, 12, 0x801C, 0 }, { 0x0B, 8, 0x8018, 0 },
        { 0x0D, 9, 0x8019, 0 }, { 0x19, 14, 0x801E, 0 }, { 0x04, 4, 0x8014, s_ColorPane04C },
        { 0x0C, 9, 0x8019, s_ColorPane0CC }, { 0x05, 4, 0x8014, s_ColorPane05C }, { 0x18, 14, 0x801E, s_ColorPane18C },
        { 0x1A, -1, -1, 0 }
    };
    long s_ButtonPaneC[18] = { 0x27, 0x2B, 0x2F, 0x33, 0x37, 0x3B, 0x3F, 0x43, 0x47, 0x4B, 0x4F, 0x53, 0x57, 0x5B, 0x5F, -1, -1, -1 };

    long s_FaceParentD[18] = { 0x2B, 0x2F, 0x33, 0x37, 0x3B, 0x3F, 0x43, 0x47, 0x4B, 0x4F, 0x53, 0x57, 0x5B, 0x5F, 0x63, 0x67, 0x6B, 0x6E };
    long s_MarkParentD[18] = { 0x2C, 0x30, 0x34, 0x38, 0x3C, 0x40, 0x44, 0x48, 0x4C, 0x50, 0x54, 0x58, 0x5C, 0x60, 0x64, 0x68, 0x6C, 0x6F };
    long s_ColorPane05D[] = { 0x05, 0x06, 0x07, -1 };
    long s_ColorPane0ED[] = { 0x0E, 0x0F, 0x10, 0x11, -1 };
    long s_ColorPane14D[] = { 0x14, 0x15, 0x17, 0x16, -1 };
    long s_ColorPane04D[] = { 0x04, 0x74, -1 };
    long s_ColorPane0CD[] = { 0x0C, 0x75, -1 };
    long s_ColorPane18D[] = { 0x18, 0x76, -1 };
    CharaPane s_CharaPaneD[19] = {
        { 0x00, 0, 0x8010, 0 },  { 0x08, 6, 0x8016, 0 },  { 0x01, 1, 0x8011, 0 },  { 0x09, 7, 0x8017, 0 },
        { 0x0E, 12, 0x801C, s_ColorPane0ED }, { 0x02, 2, 0x8012, 0 }, { 0x12, 13, 0x801D, 0 }, { 0x0A, 8, 0x8018, 0 },
        { 0x03, 3, 0x8013, 0 }, { 0x14, 15, 0x801F, s_ColorPane14D }, { 0x13, 14, 0x801E, 0 }, { 0x0B, 9, 0x8019, 0 },
        { 0x0D, 11, 0x801B, 0 }, { 0x19, 17, 0x8021, 0 }, { 0x04, 4, 0x8014, s_ColorPane04D },
        { 0x0C, 10, 0x801A, s_ColorPane0CD }, { 0x05, 5, 0x8015, s_ColorPane05D }, { 0x18, 16, 0x8020, s_ColorPane18D },
        { 0x1A, -1, -1, 0 }
    };
    long s_ButtonPaneD[18] = { 0x29, 0x2D, 0x31, 0x35, 0x39, 0x3D, 0x41, 0x45, 0x49, 0x4D, 0x51, 0x55, 0x59, 0x5D, 0x61, 0x65, 0x69, 0x6D };

    // The number of character buttons of each layout.
    long s_ButtonNum[4] = { 8, 12, 15, 18 };

    // Colour masks: the star colours, the plain ones (also what the mix
    // sport keeps), and the plain ones again.
    unsigned char s_StarColorMask = 0xAA;
    unsigned char s_ColorMask = 0x55;
    unsigned char s_ColorMask2 = 0x55;

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_UNK_01 = 1,
        OBJ_PLUS_MINUS = 2,
        OBJ_COURT_CHR = 0x3B,
        OBJ_CHR_SIGN = 0x3D
    };

    // Events.
    enum {
        EVENT_HIT = 0x13,
        EVENT_PUSH = 0x21,       // plus the pad number
        EVENT_OUT = 0x25,
        EVENT_IS_OUT = 0x26,
        EVENT_GET_LAYOUT = 0x4F
    };

    enum {
        BTN_KIND_CHARA = 0x8010, // the first character button
        BTN_KIND_MII = 0x8050,
        BTN_KIND_UP = 0x8060,    // what StickMovementJudgmentCursor gives above and below the Mii button
        BTN_KIND_DOWN = 0x8070
    };

    // CMenuPad input bits.
    enum {
        PAD_UP = 1,
        PAD_DOWN = 2,
        PAD_LEFT = 4,
        PAD_RIGHT = 8
    };

    // The character number of the Mii.
    enum {
        CHARA_MII = 0x13
    };

    // Inline in the game (name guessed): whether the part is playing its
    // push animation (states 5 and 6).
    inline bool IsPushAnim(const msw::CMenuPartBase *pPart)
    {
        bool bPush = true;
        long nState = pPart->m_Unk08;
        if (nState != 5 && nState != 6) {
            bPush = false;
        }
        return bPush;
    }

}

// Inline in the game (name guessed): the column of button nBtnKindId on
// layout nLayout; 0 when it is not there.
static inline long GetButtonColumn(long nLayout, long nBtnKindId)
{
    long i;
    long j;

    switch (nLayout) {
        case 0:
            for (i = 0; i < 2; i++) {
                for (j = 0; j < 4; j++) {
                    if (nBtnKindId == s_ButtonA[i][j]) {
                        return j;
                    }
                }
            }
            break;
        case 1:
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 4; j++) {
                    if (nBtnKindId == s_ButtonB[i][j]) {
                        return j;
                    }
                }
            }
            break;
        case 2:
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 5; j++) {
                    if (nBtnKindId == s_ButtonC[i][j]) {
                        return j;
                    }
                }
            }
            break;
        case 3:
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 6; j++) {
                    if (nBtnKindId == s_ButtonD[i][j]) {
                        return j;
                    }
                }
            }
            break;
    }
    return 0;
}

// Inline in the game (name guessed): the first voice of character nChara.
static inline long GetCharaVoice(long nChara)
{
    for (long i = 0; i < 20; i++) {
        if (nChara == s_CharaVoice[i].m_nChara) {
            return s_CharaVoice[i].m_nVoice;
        }
    }
    return s_CharaVoice[0].m_nVoice;
}

// Inline in the game (name guessed): id nIndex of the layout shown.
static inline long GetLayoutId(long nLayout, long nIndex)
{
    switch (nLayout) {
        case 0:
            return s_IdA[nIndex];
        case 1:
            return s_IdB[nIndex];
        case 2:
            return s_IdC[nIndex];
        case 3:
            return s_IdD[nIndex];
    }
    return -1;
}

// Inline in the game (name guessed): pane nIndex of those shown for nNum
// + 1 players on the layout shown.
static inline long GetPlayerPane(long nLayout, long nNum, long nIndex)
{
    switch (nLayout) {
        case 0:
            return s_PlayerPaneA[nNum][nIndex];
        case 1:
            return s_PlayerPaneB[nNum][nIndex];
        case 2:
            return s_PlayerPaneC[nNum][nIndex];
        case 3:
            return s_PlayerPaneD[nNum][nIndex];
    }
    return -1;
}

inline int msw::SelectMenu::CCharSel::IsSelectButton(unsigned short nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    if (pPart == 0) {
        return 0;
    }
    if (!(pPart->m_nFlags40 & 2)) {
        return 0;
    }
    if (IsPushAnim(pPart)) {
        return 0;
    }
    if (pPart->m_Unk42_0F != 0) {
        return 0;
    }
    if (pPart->m_nFlags40 & 0x10) {
        return 0;
    }
    return !(m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId)->m_nFlags40 & 1);
}

// The number of character slots taking part (the computer's not counted
// in the modes where it only fills up).
inline long msw::SelectMenu::CCharSel::GetCharaNum()
{
    CSelectMenu *pSelectMenu = GetSelectMenu();
    long nNum = 0;
    int bNoCom = 0;

    if ((long)pSelectMenu->m_Unk9B == 1 || (long)pSelectMenu->m_Unk9B == 4) {
        bNoCom = 1;
    }
    for (long i = 0; i < 6; i++) {
        CSelectMenu::CharaSelectParam *pParam = pSelectMenu->GetCharaSelectParam(i);
        if (pParam != 0 && pParam->m_nPlayerKind != 0xFF && (pParam->m_nPlayerKind != 4 || bNoCom == 0)) {
            nNum++;
        }
    }
    return nNum;
}

// Inline in the game (name guessed): hangs pane nPane under pane nParent,
// at its origin.
inline void msw::SelectMenu::CCharSel::MovePane(long nPane, long nParent)
{
    nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(nParent);
    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(nPane);
    bcs::G2D::SetPaneParent(pPane, pParent, 1, 0);
    pPane->SetTranslate(zero);
}

// Inline in the game (name guessed): a copy, called pName, of the layout's
// pane pSrcName, hung under pane nParent at its origin.
inline void msw::SelectMenu::CCharSel::AddPaneCopy(long nParent, const char *pSrcName, const char *pName)
{
    nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(nParent);
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    bcs::G2D::CPaneObj *pObj = bcs::G2D::DuplicatePaneObj(pPaneCtrl, pSrcName, pName, m_LayoutName, 0);
    pPaneCtrl->AppendPaneObj(pObj, (const char *)0);
    nw4r::lyt::Pane *pPane = pObj->GetPane();
    bcs::G2D::SetPaneParent(pPane, pParent, 1, 0);
    pPane->SetTranslate(zero);
}

msw::SelectMenu::CCharSel::CCharSel(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk64C[0] = 0;
    m_Unk64C[1] = 0;
    m_Unk64C[2] = 0;
    m_Unk64C[3] = 0;
}

msw::SelectMenu::CCharSel::~CCharSel()
{
    Remove();
}

void msw::SelectMenu::CCharSel::Main()
{
    long nWinPane = GetLayoutId(m_nLayout, ID_WIN_PANE);
    long nMiiPane = GetLayoutId(m_nLayout, ID_MII_PANE);
    long nMiiPane2 = GetLayoutId(m_nLayout, ID_MII_PANE_2);

    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap (see notes/decisions.md).
            static const int recursive = 0;
            static const int reset = 1;
            if (IsBackFrameIn(0)) {
                long nAnim = GetLayoutId(m_nLayout, ID_ANIM_IN_BACK);
                long nMiiAnim = GetLayoutId(m_nLayout, ID_ANIM_MII_IN);
                m_pPaneAccessor->PlayAnimation(nWinPane, nAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(nMiiPane, nMiiAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(nMiiPane2, nMiiAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                long nAnim = GetLayoutId(m_nLayout, ID_ANIM_IN);
                long nMiiAnim = GetLayoutId(m_nLayout, ID_ANIM_MII_IN);
                m_pPaneAccessor->PlayAnimation(nWinPane, nAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(nMiiPane, nMiiAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(nMiiPane2, nMiiAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_fWait = 20.0f;
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            m_fWait -= bcs::CMainApp::GetAppFrameRateF();
            if (m_fWait < 0.0f) {
                int bEnd = 0;
                if (IsBackFrameIn(0)) {
                    if (m_pPaneAccessor->IsAnimEnd(nWinPane, GetLayoutId(m_nLayout, ID_ANIM_IN_BACK))) {
                        bEnd = 1;
                    }
                } else {
                    if (m_pPaneAccessor->IsAnimEnd(nWinPane, GetLayoutId(m_nLayout, ID_ANIM_IN))) {
                        bEnd = 1;
                    }
                }
                if (bEnd) {
                    m_bSeEnable = 1;
                    CheckButtonAnim();
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE:
            // The colour markers fade in with the window.
            if (CCameraMenuBG::s_Instance.IsEndAnim()) {
                long nAnim;
                if (IsBackFrameIn(0)) {
                    nAnim = GetLayoutId(m_nLayout, ID_ANIM_IN_BACK);
                } else {
                    nAnim = GetLayoutId(m_nLayout, ID_ANIM_IN);
                }
                bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nWinPane);
                if (pObj != 0) {
                    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
                    if (pCourt != 0) {
                        float fMax = pObj->GetAnimMaxFrame(nAnim);
                        if (fMax < 1.0f) {
                            fMax = 1.0f;
                        }
                        float fFrame = pObj->GetAnimCurrentFrame(nAnim);
                        if (fFrame < 0.0f) {
                            fFrame = 0.0f;
                        }
                        pCourt->SetMarkerAlpha(fFrame / fMax);
                    }
                }
            }
            CheckButtonAnim();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd() &&
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_IS_OUT, 0) != 0) {
                // static-order swap (see notes/decisions.md).
                static const int recursive = 0;
                static const int reset = 1;
                if (IsBackFrameOut(0)) {
                    long nAnim = GetLayoutId(m_nLayout, ID_ANIM_OUT_BACK);
                    m_pPaneAccessor->PlayAnimation(nWinPane, nAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                    m_pPaneAccessor->PlayAnimation(nMiiPane, nAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                    m_pPaneAccessor->PlayAnimation(nMiiPane2, nAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    long nAnim = GetLayoutId(m_nLayout, ID_ANIM_OUT);
                    m_pPaneAccessor->PlayAnimation(nWinPane, nAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                    m_pPaneAccessor->PlayAnimation(nMiiPane, nAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                    m_pPaneAccessor->PlayAnimation(nMiiPane2, nAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                CChrSign *pSign = (CChrSign *)GetSelectMenu()->FindObj(OBJ_CHR_SIGN);
                if (pSign != 0) {
                    for (long i = 0; i < 6; i++) {
                        pSign->ClearSign(i);
                    }
                }
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_OUT, 0);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The colour markers fade out with the window.
            if (IsBackFrameIn(0)) {
                long nAnim = GetLayoutId(m_nLayout, ID_ANIM_OUT_BACK);
                if (m_pPaneAccessor->IsAnimEnd(nWinPane, nAnim)) {
                    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
                    if (pCourt != 0) {
                        for (long i = 0; i < 6; i++) {
                            pCourt->SetMarkerDisp(i, 0);
                        }
                    }
                    m_Unk30 = STATE_NONE;
                }
                bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nWinPane);
                if (pObj != 0) {
                    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
                    if (pCourt != 0) {
                        float fMax = pObj->GetAnimMaxFrame(nAnim);
                        if (fMax < 1.0f) {
                            fMax = 1.0f;
                        }
                        float fFrame = pObj->GetAnimCurrentFrame(nAnim);
                        if (fFrame < 0.0f) {
                            fFrame = 0.0f;
                        }
                        pCourt->SetMarkerAlpha((1.0f - fFrame) / fMax);
                    }
                }
            } else {
                static const int isReverse = 1;
                bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nWinPane);
                if (pObj != 0) {
                    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
                    if (pCourt != 0) {
                        float fMax = pObj->GetAnimMaxFrame(isReverse);
                        if (fMax < 1.0f) {
                            fMax = 1.0f;
                        }
                        float fFrame = pObj->GetAnimCurrentFrame(isReverse);
                        if (fFrame < 0.0f) {
                            fFrame = 0.0f;
                        }
                        pCourt->SetMarkerAlpha((1.0f - fFrame) / fMax);
                    }
                }
                long nAnim = GetLayoutId(m_nLayout, ID_ANIM_OUT);
                if (m_pPaneAccessor->IsAnimEnd(nWinPane, nAnim)) {
                    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
                    if (pCourt != 0) {
                        for (long i = 0; i < 6; i++) {
                            pCourt->SetMarkerDisp(i, 0);
                        }
                    }
                    m_Unk30 = STATE_NONE;
                }
                pObj = m_pPaneAccessor->GetPaneObj(nWinPane);
                if (pObj != 0) {
                    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
                    if (pCourt != 0) {
                        float fMax = pObj->GetAnimMaxFrame(nAnim);
                        if (fMax < 1.0f) {
                            fMax = 1.0f;
                        }
                        float fFrame = pObj->GetAnimCurrentFrame(nAnim);
                        if (fFrame < 0.0f) {
                            fFrame = 0.0f;
                        }
                        pCourt->SetMarkerAlpha((1.0f - fFrame) / fMax);
                    }
                }
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CCharSel::Disp()
{
}

int msw::SelectMenu::CCharSel::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    char buttonFileName[0x20];
    OpenCharCount();
    switch (m_nLayout) {
        case 0:
            strcpy(m_LayoutName, s_LayoutNameA);
            strcpy(buttonFileName, s_ButtonFileNameA);
            break;
        case 1:
            strcpy(m_LayoutName, s_LayoutNameB);
            strcpy(buttonFileName, s_ButtonFileNameB);
            break;
        case 2:
            strcpy(m_LayoutName, s_LayoutNameC);
            strcpy(buttonFileName, s_ButtonFileNameC);
            break;
        case 3:
            strcpy(m_LayoutName, s_LayoutNameD);
            strcpy(buttonFileName, s_ButtonFileNameD);
            break;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(pSelectMenu->GetLoadArchiveFileId(-1));
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            ArchiveBuildLoader(buttonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, m_LayoutName);
            SetUpAnim(pPaneCtrl);
            SetPaneAccessor(pPaneCtrl, GetNameTable(), GetNameNum());
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            for (long i = 0; i < GetButtonNum(); i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);
                unsigned short nBtnKindId = SetUpButton(pPart, i, 0);
                // The character buttons take the colour of the pad on them.
                if (nBtnKindId != 0 && nBtnKindId != 0x50) {
                    switch (m_nLayout) {
                        case 0:
                            pPart->m_nAnim[7] = s_PadAnimA[0];
                            pPart->m_nAnim[8] = s_PadAnimA[1];
                            pPart->m_nAnim[9] = s_PadAnimA[2];
                            pPart->m_nAnim[10] = s_PadAnimA[3];
                            pPart->m_nAnim[11] = s_NoPadAnimA;
                            break;
                        case 1:
                            pPart->m_nAnim[7] = s_PadAnimB[0];
                            pPart->m_nAnim[8] = s_PadAnimB[1];
                            pPart->m_nAnim[9] = s_PadAnimB[2];
                            pPart->m_nAnim[10] = s_PadAnimB[3];
                            pPart->m_nAnim[11] = s_NoPadAnimB;
                            break;
                        case 2:
                            pPart->m_nAnim[7] = s_PadAnimC[0];
                            pPart->m_nAnim[8] = s_PadAnimC[1];
                            pPart->m_nAnim[9] = s_PadAnimC[2];
                            pPart->m_nAnim[10] = s_PadAnimC[3];
                            pPart->m_nAnim[11] = s_NoPadAnimC;
                            break;
                        case 3:
                            pPart->m_nAnim[7] = s_PadAnimD[0];
                            pPart->m_nAnim[8] = s_PadAnimD[1];
                            pPart->m_nAnim[9] = s_PadAnimD[2];
                            pPart->m_nAnim[10] = s_PadAnimD[3];
                            pPart->m_nAnim[11] = s_NoPadAnimD;
                            break;
                    }
                }
                pPart->SetCallBackAnimIn(this);
                pPart->SetCallBackAnimOut(this);
                pPart->SetCallBackAnimMultiIn(this);
                pPart->SetCallBackAnimMultiOut(this);
                pPart->SetCallBackAnimPush(this);
                pPart->SetCallBackAnimCancel(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            if ((long)pSelectMenu->m_Unk9B == 4 && pSelectMenu->m_bStarMii != 0) {
                m_bMiiObj = 0;
            } else {
                // The Mii button goes under the window.
                long nMiiPane = GetLayoutId(m_nLayout, ID_MII_PANE);
                long nMiiPane2 = GetLayoutId(m_nLayout, ID_MII_PANE_2);
                long nParent = GetLayoutId(m_nLayout, ID_MII_PARENT);
                CMenuPaneAccessor *pPaneAccessor = m_pPaneAccessor;
                nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
                nw4r::lyt::Pane *pParent = pPaneAccessor->GetPane(nParent);
                nw4r::lyt::Pane *pMii = m_pPaneAccessor->GetPane(nMiiPane);
                nw4r::lyt::Pane *pMii2 = m_pPaneAccessor->GetPane(nMiiPane2);
                bcs::G2D::SetPaneParent(pMii, pParent, 1, 0);
                bcs::G2D::SetPaneParent(pMii2, pParent, 1, 0);
                pMii->SetTranslate(zero);
                pMii2->SetTranslate(zero);
                m_bMiiObj = 1;
            }
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;

            SetAllCourtCharaObj();
            SetColorMarkerObj();
            m_pPaneAccessor->PlayAnimation(GetLayoutId(m_nLayout, ID_WIN_PANE), GetLayoutId(m_nLayout, ID_ANIM_WAIT),
                                           1.0f, 0.0f, 1, 1);
            m_pPaneAccessor->SetPaneDisp(GetPlayerPane(m_nLayout, 1, 0), 0);
            m_pPaneAccessor->SetPaneDisp(GetPlayerPane(m_nLayout, 1, 1), 0);
            m_pPaneAccessor->SetPaneDisp(GetPlayerPane(m_nLayout, 3, 0), 0);
            m_pPaneAccessor->SetPaneDisp(GetPlayerPane(m_nLayout, 3, 1), 0);
            m_pPaneAccessor->SetPaneDisp(GetPlayerPane(m_nLayout, 3, 2), 0);
            m_pPaneAccessor->SetPaneDisp(GetPlayerPane(m_nLayout, 3, 3), 0);
            SetUpChar();

            // The plus and minus window explains the star and new marks.
            pSelectMenu->Build(OBJ_PLUS_MINUS);
            CPlusMinus *pPlusMinus = (CPlusMinus *)pSelectMenu->FindObj(OBJ_PLUS_MINUS);
            const wchar_t *pText = 0;
            const wchar_t *pMinus = GetMessageLabelString("List_Plus_Minus_Button", "Minus_01");
            if (m_bStarChara == 1 && m_bNewChara == 1) {
                pText = GetMessageLabelString("List_Plus_Minus_Button", "Plus_03");
            } else if (m_bStarChara == 0 && m_bNewChara == 1) {
                pText = GetMessageLabelString("List_Plus_Minus_Button", "Plus_02");
            } else if (m_bStarChara == 1 && m_bNewChara == 0) {
                pText = GetMessageLabelString("List_Plus_Minus_Button", "Plus_01");
            }
            if (pText != 0) {
                pPlusMinus->StartUp(3, pText, pMinus);
                m_Unk7A4 = 3;
            } else {
                pPlusMinus->StartUp(0, pText, pMinus);
                m_Unk7A4 = 0;
            }
            m_nMiiColorNum = -1;
            m_nCursorBtnKindId[0] = 0;
            m_nCursorBtnKindId[1] = 0;
            m_nCursorBtnKindId[2] = 0;
            m_nCursorBtnKindId[3] = 0;
        }
    }
    return 0;
}

// Inlined into Main() although defined after it; the out-of-line copy is
// not called and the link drops it. Defined here, after Set(), because its
// local static follows Set()'s constants in .rodata.
void msw::SelectMenu::CCharSel::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    SetMiiButtonColor();
    if (m_bMiiObj != 0) {
        GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, backEvent, m_nPushButtonId);
    }
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, backEvent, m_nPushButtonId);
}

void msw::SelectMenu::CCharSel::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    GetSelectMenu()->Scratch(OBJ_PLUS_MINUS);
    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
    if (pCourt != 0) {
        for (long i = 0; i < 6; i++) {
            pCourt->SetMarkerDisp(i, 0);
        }
    }
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CCharSel::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    if (m_bMiiObj != 0 && nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, EVENT_HIT, pPos);
    }
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_HIT, pPos);
    }
    return nResult;
}

int msw::SelectMenu::CCharSel::IsMarkerOnPointer(long, long)
{
    return 1;
}

long msw::SelectMenu::CCharSel::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharSel::PushCursorAnim(long, long)
{
    return 0;
}

// Inline in the game (name guessed): puts the face of the button pad
// nPad's cursor was on back to the first colour, unless that button is
// busy.
inline void msw::SelectMenu::CCharSel::ResetCursorFace(long nPad)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(m_nCursorBtnKindId[nPad]);
    CMenuPartBase *pPart2 = m_pMenuPartList->FindObjectByCurrentBtnKindId(m_nCursorBtnKindId[nPad]);
    if ((pPart->m_nFlags40 & 2) && !IsPushAnim(pPart) && !(pPart->m_nFlags40 & 0x10) && !(pPart2->m_nFlags40 & 1)) {
        SetButtonFace(m_nCursorBtnKindId[nPad], 0);
    }
}

long msw::SelectMenu::CCharSel::SetCursorMarkerPosition(long nPad, long nBtnKindId)
{
    if (!CMenuPad::s_Instance.IsInputPointerMode(nPad)) {
        if (m_nCursorBtnKindId[nPad] != 0) {
            ResetCursorFace(nPad);
        }
    } else {
        if (m_nCursorBtnKindId[nPad] != 0 && nBtnKindId != m_nCursorBtnKindId[nPad]) {
            ResetCursorFace(nPad);
        }
    }

    if (nBtnKindId == BTN_KIND_MII && !IsSelectButton(nBtnKindId) && m_nCursorBtnKindId[nPad] != BTN_KIND_MII) {
        SE_Play(0xBC0, 1);
    }

    if ((unsigned long)(nBtnKindId - BTN_KIND_CHARA) <= 0x11 || nBtnKindId == BTN_KIND_MII) {
        m_nCursorBtnKindId[nPad] = nBtnKindId;
    } else {
        m_nCursorBtnKindId[nPad] = 0;
    }
    return 0;
}

long msw::SelectMenu::CCharSel::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CCharSel::StickMoveCheck(long nPad, long nBtnKindId)
{
    long nStart = nBtnKindId;
    if (!IsButtonKindOfStickMode(nStart)) {
        nStart = s_DefaultButton[GetSelectMenu()->GetPadEntryId(nPad)];
    }

    unsigned long nInput = CMenuPad::s_Instance.GetInput(nPad);
    long nNext = (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, nInput);
    if (nNext == nStart && (nInput & 0xF)) {
        nNext = GetNextButtonKindId(nNext, nInput);
        if (nNext == 0) {
            nNext = nStart;
        }
    } else if (nInput & (PAD_UP | PAD_LEFT)) {
        unsigned short nNext3 = GetNextButtonKindId3Point(nStart, nInput);
        if (nNext3 != 0) {
            nNext = nNext3;
        }
    }

    if (nNext == BTN_KIND_MII) {
        // Leaving the Mii button up or down goes back to the column the
        // cursor came from.
        const long *pTopRow;
        const long *pBottomRow;
        switch (m_nLayout) {
            case 0:
                pTopRow = s_TopRowA;
                break;
            case 1:
                pTopRow = s_TopRowB;
                break;
            case 2:
                pTopRow = s_TopRowC;
                break;
            case 3:
                pTopRow = s_TopRowD;
                break;
            default:
                pTopRow = 0;
                break;
        }

        switch (m_nLayout) {
            case 0:
                pBottomRow = s_BottomRowA;
                break;
            case 1:
                pBottomRow = s_BottomRowB;
                break;
            case 2:
                pBottomRow = s_BottomRowC;
                break;
            case 3:
                pBottomRow = s_BottomRowD;
                break;
            default:
                pBottomRow = 0;
                break;
        }
        if (nStart != BTN_KIND_MII) {
            m_nColumn[nPad] = GetButtonColumn(m_nLayout, nStart);
        } else {
            nNext = (unsigned short)m_pMenuPartList->StickMovementJudgmentCursor(nPad, nNext, nInput);
            if (nNext == 0x8000) {
                nNext = BTN_KIND_MII;
            }
        }

        if (m_Unk64C[nPad] != 0) {
            if (nInput & PAD_UP) {
                nNext = pTopRow[m_nColumn[nPad]];
                if (!IsSelectButton(nNext)) {
                    nNext = GetNextButtonKindId(nNext, PAD_UP);
                }
            } else if (nInput & PAD_LEFT) {
                nNext = pBottomRow[m_nColumn[nPad]];
                if (!IsSelectButton(nNext)) {
                    nNext = GetNextButtonKindId(nNext, PAD_LEFT);
                }
            }
        } else {
            if (nNext == BTN_KIND_UP) {
                nNext = pTopRow[m_nColumn[nPad]];
                if (!IsSelectButton(nNext)) {
                    nNext = GetNextButtonKindId(nNext, PAD_UP);
                }
            }
            if (nNext == BTN_KIND_DOWN) {
                nNext = pBottomRow[m_nColumn[nPad]];
                if (!IsSelectButton(nNext)) {
                    nNext = GetNextButtonKindId(nNext, PAD_LEFT);
                }
            }
        }
    }
    return nNext;
}

long msw::SelectMenu::CCharSel::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = s_DefaultButton[GetSelectMenu()->GetPadEntryId(nPad)];
    }
    return nBtnKindId;
}

int msw::SelectMenu::CCharSel::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

void msw::SelectMenu::CCharSel::SetMiiButtonColor()
{
    if (m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_MII) == 0) {
        return;
    }

    long nPane = GetLayoutId(m_nLayout, ID_MII_PANE_2);
    const unsigned char *pPadAnim;
    long nAnim;
    switch (m_nLayout) {
        case 0:
            pPadAnim = s_PadAnimA;
            nAnim = s_NoPadAnimA;
            break;
        case 1:
            pPadAnim = s_PadAnimB;
            nAnim = s_NoPadAnimB;
            break;
        case 2:
            pPadAnim = s_PadAnimC;
            nAnim = s_NoPadAnimC;
            break;
        case 3:
            nAnim = s_NoPadAnimD;
            pPadAnim = s_PadAnimD;
            break;
    }

    // The pads whose cursor is on the Mii button.
    long nNum = 0;
    if (m_Unk64C[0] == 0 && m_nPushButtonId[0] == BTN_KIND_MII) {
        nNum = 1;
    }
    if (m_Unk64C[1] == 0 && m_nPushButtonId[1] == BTN_KIND_MII) {
        nNum++;
    }
    if (m_Unk64C[2] == 0 && m_nPushButtonId[2] == BTN_KIND_MII) {
        nNum++;
    }
    if (m_Unk64C[3] == 0 && m_nPushButtonId[3] == BTN_KIND_MII) {
        nNum++;
    }

    if (m_nMiiColorNum != nNum) {
        if (nNum != 0) {
            m_pPaneAccessor->PlayAnimation(nPane, nAnim, 1.0f, 0.0f, 1, 1);
            long nColor = 0;
            for (long i = 0; i < 4; i++) {
                if (m_Unk64C[i] == 0 && m_nPushButtonId[i] == BTN_KIND_MII) {
                    long nPlayerPane = GetPlayerPane(m_nLayout, nNum - 1, nColor);
                    m_pPaneAccessor->PlayAnimation(nPlayerPane, pPadAnim[i], 1.0f, 0.0f, 0, 0);
                    nColor++;
                }
            }
        } else {
            m_pPaneAccessor->PlayAnimation(nPane, nAnim, 1.0f, 0.0f, 1, 1);
        }
        m_nMiiColorNum = nNum;
    }
}

long msw::SelectMenu::CCharSel::PushButton(long nPad, long nBtnKindId)
{
    long nChara;
    long nColor;

    if (FindChrTypeByBtnKind(nBtnKindId, nChara, nColor) == 0) {
        if (nChara & 0x80) {
            return -1;
        }
        long nId = m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad);
        if (nId >= 0) {
            CMenuPartBase *pPart = m_pMenuPartList->FindObject(nId);
            if (!(pPart->m_nFlags40 & 0x10)) {
                pPart->SetButtonAnim(GetLayoutId(m_nLayout, ID_ANIM_PUSH), 1, -1);
                SE_Play(nPad + 0xBD2, 1);
                if (nChara != CHARA_MII) {
                    CallCharVoice(nChara);
                }
            }
            return 0;
        }
    }

    long nResult = 0;
    long nEvent = nPad + EVENT_PUSH;
    if (m_bMiiObj != 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, nEvent, (void *)nBtnKindId);
    }
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, nEvent, (void *)nBtnKindId);
    return nResult;
}

long msw::SelectMenu::CCharSel::PushCancelButton(long nPad, long nBtnKindId)
{
    long nId = m_pMenuPartList->GetIndexByCurrentBtnKindId(nBtnKindId);
    if (nId >= 0) {
        CMenuPartBase *pPart = m_pMenuPartList->FindObject(nId);
        if (!(pPart->m_nFlags40 & 0x10)) {
            SE_Play(0xBBF, 1);
            m_pMenuPartList->CancelPushButtonAnim(nBtnKindId, nPad);
            pPart->SetButtonAnim(GetLayoutId(m_nLayout, ID_ANIM_CANCEL), 1, -1);
        }
    }
    return 0;
}

long msw::SelectMenu::CCharSel::PushButtonIn(long nPad, unsigned char nChara, unsigned char nColor)
{
    CharaPane *pCharaPane = 0;
    switch (m_nLayout) {
        case 0:
            pCharaPane = s_CharaPaneA;
            break;
        case 1:
            pCharaPane = s_CharaPaneB;
            break;
        case 2:
            pCharaPane = s_CharaPaneC;
            break;
        case 3:
            pCharaPane = s_CharaPaneD;
            break;
    }

    long nBtnKindId = pCharaPane[nChara].m_nBtnKindId;
    long nId = m_pMenuPartList->GetIndexByCurrentBtnKindId(nBtnKindId);
    if (nId >= 0) {
        CMenuPartBase *pPart = m_pMenuPartList->FindObject(nId);
        if (!(pPart->m_nFlags40 & 0x10)) {
            SetButtonFace(nBtnKindId, nColor);
            m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad);
            pPart->SetButtonAnim(GetLayoutId(m_nLayout, ID_ANIM_PUSH), 1, -1);
        }
    }
    return 0;
}

void msw::SelectMenu::CCharSel::SetUpChar()
{
    CSelectMenu *pSelectMenu = GetSelectMenu();
    SAVE_DATA_PLAYER *pPlayerData = CSave::GetPlayerData(-1);
    SAVE_DATA_PLAYER::CharaScore *pScore;

    m_bStarChara = 0;
    m_bNewChara = 0;
    switch (pSelectMenu->m_nGameKind) {
        case 0:
            pScore = pPlayerData->m_CharaScore[0];
            break;
        case 1:
            pScore = pPlayerData->m_CharaScore[1];
            break;
        case 2:
            pScore = pPlayerData->m_CharaScore[2];
            break;
        case 3:
            pScore = pPlayerData->m_CharaScore[3];
            break;
        default:
            pScore = m_MixScore;
            break;
    }

    long *pFaceParent = 0;
    CharaPane *pCharaPane = 0;
    long *pButtonPane = 0;
    long *pMarkParent = 0;
    switch (m_nLayout) {
        case 0:
            pFaceParent = s_FaceParentA;
            pCharaPane = s_CharaPaneA;
            pButtonPane = s_ButtonPaneA;
            pMarkParent = s_MarkParentA;
            break;
        case 1:
            pFaceParent = s_FaceParentB;
            pCharaPane = s_CharaPaneB;
            pButtonPane = s_ButtonPaneB;
            pMarkParent = s_MarkParentB;
            break;
        case 2:
            pFaceParent = s_FaceParentC;
            pCharaPane = s_CharaPaneC;
            pButtonPane = s_ButtonPaneC;
            pMarkParent = s_MarkParentC;
            break;
        case 3:
            pFaceParent = s_FaceParentD;
            pCharaPane = s_CharaPaneD;
            pButtonPane = s_ButtonPaneD;
            pMarkParent = s_MarkParentD;
            break;
    }

    for (long i = 0; i < CHARA_BUTTON_MAX; i++) {
        m_aChara[i].m_nBtnKindId = 0;
    }

    // The open characters: their faces go onto their buttons.
    SAVE_DATA_PLAYER::CharaScore *pCharaScore = pScore;
    CharaPane *pPane = pCharaPane;
    for (unsigned char nChara = 0; nChara < CHARA_BUTTON_MAX; nChara++, pPane++, pCharaScore++) {
        if (pCharaScore->m_nPlayNum == 0) {
            continue;
        }

        long nPos = pPane->m_nPos;
        if (pPane->m_pColorPane != 0) {
            // One face per colour; the first is shown.
            for (long i = 0;; i++) {
                long nColorPane = pPane->m_pColorPane[i];
                if (nColorPane == -1) {
                    break;
                }
                MovePane(nColorPane, pFaceParent[nPos]);
                m_pPaneAccessor->SetPaneDisp(pPane->m_pColorPane[i], 0);
            }
            m_pPaneAccessor->SetPaneDisp(pPane->m_pColorPane[0], 1);
        } else {
            MovePane(pPane->m_nPane, pFaceParent[nPos]);
        }

        Chara &rChara = m_aChara[nPos];
        rChara.m_nBtnKindId = pPane->m_nBtnKindId;
        rChara.m_nChara = nChara;
        unsigned char nMask = s_ColorMask & pCharaScore->m_nPlayNum;
        rChara.m_nColorMask = nMask;
        unsigned char nStar = nMask & s_StarColorMask;
        if (nStar != 0) {
            m_bStarChara = 1;
        }
        if (nMask & 0x54) {
            m_bNewChara = 1;
        }

        // The star mark, or the new mark.
        if (nStar != 0) {
            AddPaneCopy(pMarkParent[nPos], "star", "StarXX");
        } else if (pCharaScore->m_nNew != 0) {
            AddPaneCopy(pMarkParent[nPos], "new", "NewXX");
        }

        m_pPaneAccessor->PlayAnimation(pButtonPane[nPos], GetLayoutId(m_nLayout, ID_ANIM_WAIT), 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(pButtonPane[nPos], GetLayoutId(m_nLayout, ID_ANIM_CANCEL), 1.0f, 0.0f, 1, 0);
    }

    // The places left get a "?" button that cannot be pushed.
    unsigned char nUnknown = 0x80;
    for (long nPos = 0; nPos < s_ButtonNum[m_nLayout]; nPos++) {
        if (m_aChara[nPos].m_nBtnKindId != 0) {
            continue;
        }
        AddPaneCopy(pFaceParent[nPos], "p_hatena", "Hatena");

        unsigned short nBtnKindId = nPos + BTN_KIND_CHARA;
        m_aChara[nPos].m_nBtnKindId = nBtnKindId;
        m_aChara[nPos].m_nChara = nUnknown;
        m_aChara[nPos].m_nColorMask = 0;
        m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId)->m_nFlags40 |= 1;
        m_pPaneAccessor->PlayAnimation(pButtonPane[nPos], GetLayoutId(m_nLayout, ID_ANIM_WAIT), 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(pButtonPane[nPos], GetLayoutId(m_nLayout, ID_ANIM_PUSH), 1.0f, 0.0f, 1, 0);
        nUnknown++;
    }

    // Without the Mii the Mii button is switched off and hidden.
    if (pScore[CHARA_MII].m_nPlayNum == 0) {
        m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_MII)->m_nFlags40 |= 1;
        long nMiiPane = GetLayoutId(m_nLayout, ID_MII_PANE);
        long nMiiPane2 = GetLayoutId(m_nLayout, ID_MII_PANE_2);
        m_pPaneAccessor->SetPaneDisp(nMiiPane, 0);
        m_pPaneAccessor->SetPaneDisp(nMiiPane2, 0);
    }
}

unsigned short msw::SelectMenu::CCharSel::GetNextButtonKindId(long nBtnKindId, unsigned long nInput)
{
    if (nBtnKindId == BTN_KIND_MII && (nInput & (PAD_DOWN | PAD_RIGHT))) {
        return 0;
    }

    // Follow the buttons in that direction until one can take the cursor.
    long nStart = nBtnKindId;
    unsigned short nNext;
    do {
        CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
        if (pPart == 0) {
            break;
        }
        if (nInput & PAD_UP) {
            nNext = pPart->m_nNextBtnKindId[1];
            if (m_pMenuPartList->FindObjectByCurrentBtnKindId(nNext) == 0) {
                break;
            }
        } else if (nInput & PAD_LEFT) {
            nNext = pPart->m_nNextBtnKindId[2];
            if (m_pMenuPartList->FindObjectByCurrentBtnKindId(nNext) == 0) {
                break;
            }
        } else if (nInput & PAD_RIGHT) {
            nNext = pPart->m_nNextBtnKindId[3];
            if (m_pMenuPartList->FindObjectByCurrentBtnKindId(nNext) == 0) {
                break;
            }
        } else if (nInput & PAD_DOWN) {
            nNext = pPart->m_nNextBtnKindId[4];
            if (m_pMenuPartList->FindObjectByCurrentBtnKindId(nNext) == 0) {
                break;
            }
        }
        if (nNext != 0 && nNext != BTN_KIND_MII && IsSelectButton(nNext)) {
            return nNext;
        }
        nBtnKindId = nNext;
    } while (nStart != nBtnKindId);

    return GetNextButtonKindId3Point(nStart, nInput);
}

unsigned short msw::SelectMenu::CCharSel::GetNextButtonKindId3Point(long nBtnKindId, unsigned long nInput)
{
    if (nBtnKindId == BTN_KIND_MII && (nInput & (PAD_DOWN | PAD_RIGHT))) {
        return 0;
    }

    // The next button in that direction and the two beside it.
    unsigned short nResult = 0;
    while (nResult == 0) {
        unsigned short nNext[3];
        CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
        if (pPart == 0) {
            return 0;
        }
        if (nInput & PAD_UP) {
            nNext[0] = pPart->m_nNextBtnKindId[1];
            pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nNext[0]);
            if (pPart == 0) {
                return 0;
            }
            nNext[1] = pPart->m_nNextBtnKindId[3];
            nNext[2] = pPart->m_nNextBtnKindId[4];
        } else if (nInput & PAD_LEFT) {
            nNext[0] = pPart->m_nNextBtnKindId[2];
            pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nNext[0]);
            if (pPart == 0) {
                return 0;
            }
            nNext[1] = pPart->m_nNextBtnKindId[3];
            nNext[2] = pPart->m_nNextBtnKindId[4];
        } else if (nInput & PAD_RIGHT) {
            nNext[0] = pPart->m_nNextBtnKindId[3];
            pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nNext[0]);
            if (pPart == 0) {
                return 0;
            }
            nNext[1] = pPart->m_nNextBtnKindId[1];
            nNext[2] = pPart->m_nNextBtnKindId[2];
        } else if (nInput & PAD_DOWN) {
            nNext[0] = pPart->m_nNextBtnKindId[4];
            pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nNext[0]);
            if (pPart == 0) {
                return 0;
            }
            nNext[1] = pPart->m_nNextBtnKindId[1];
            nNext[2] = pPart->m_nNextBtnKindId[2];
        }
        for (long i = 0; i < 3; i++) {
            unsigned short nId = nNext[i];
            if (nId != 0 && nId != BTN_KIND_MII && IsSelectButton(nId)) {
                nResult = nNext[i];
                break;
            }
        }
        nBtnKindId = nNext[0];
    }
    return nResult;
}

long msw::SelectMenu::CCharSel::FindChrTypeByBtnKind(unsigned short nBtnKindId, long &rChara, long &rColor)
{
    if (nBtnKindId == BTN_KIND_MII) {
        rChara = CHARA_MII;
        rColor = 0xFF;
        return 0;
    }

    // A button being pushed counts as no character.
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    if (pPart != 0 && IsPushAnim(pPart)) {
        rChara = 0x80;
        rColor = 0;
        return 0;
    }

    for (long i = 0; i < CHARA_BUTTON_MAX; i++) {
        if (m_aChara[i].m_nBtnKindId == 0) {
            break;
        }
        Chara *pChara = &m_aChara[i];
        if (nBtnKindId == pChara->m_nBtnKindId) {
            rChara = pChara->m_nChara;
            rColor = pChara->m_nColor;
            return 0;
        }
    }
    return -1;
}

long msw::SelectMenu::CCharSel::FindBtnKindByChrType(long nChara)
{
    if (nChara == CHARA_MII) {
        return BTN_KIND_MII;
    }
    for (long i = 0; i < CHARA_BUTTON_MAX; i++) {
        Chara *pChara = &m_aChara[i];
        if (nChara == pChara->m_nChara) {
            return pChara->m_nBtnKindId;
        }
    }
    return -1;
}

long msw::SelectMenu::CCharSel::RandomMoveCheck()
{
    long nNum;
    switch (m_nLayout) {
        case 0:
            nNum = sizeof(s_ButtonA) / sizeof(s_ButtonA[0][0]);
            break;
        case 1:
            nNum = sizeof(s_ButtonB) / sizeof(s_ButtonB[0][0]);
            break;
        case 2:
            nNum = sizeof(s_ButtonC) / sizeof(s_ButtonC[0][0]);
            break;
        case 3:
            nNum = sizeof(s_ButtonD) / sizeof(s_ButtonD[0][0]);
            break;
    }

    long nBtnKindId;
    do {
        nBtnKindId = bcs::BCS_RandRange(0L, nNum) + BTN_KIND_CHARA;
    } while (!IsSelectButton(nBtnKindId));
    return nBtnKindId;
}

int msw::SelectMenu::CCharSel::FindChrTypeChangeColor(unsigned short nBtnKindId)
{
    for (long i = 0; i < CHARA_BUTTON_MAX; i++) {
        if (m_aChara[i].m_nBtnKindId == 0) {
            break;
        }
        Chara *pChara = &m_aChara[i];
        if (nBtnKindId == pChara->m_nBtnKindId) {
            // The number of colours open.
            unsigned char nMask = pChara->m_nColorMask;
            int nNum = 1;
            for (long j = 0; j < 8; j++) {
                nMask >>= 1;
                if (nMask & 1) {
                    nNum++;
                }
            }
            return nNum;
        }
    }
    return -1;
}

unsigned char msw::SelectMenu::CCharSel::GetNextChrColor(unsigned short nBtnKindId, unsigned char nColor)
{
    if (nColor != 0) {
        unsigned char nNext = nColor << 1;
        if (nNext == 0) {
            nNext = 1;
        }
        for (long i = 0; i < CHARA_BUTTON_MAX; i++) {
            if (m_aChara[i].m_nBtnKindId == 0) {
                break;
            }
            Chara *pChara = &m_aChara[i];
            if (nBtnKindId == pChara->m_nBtnKindId) {
                // The next colour open after nColor, round to the first.
                unsigned char nMask = pChara->m_nColorMask;
                if (nMask & nNext) {
                    return nNext;
                }
                for (;;) {
                    nNext <<= 1;
                    if (nNext == 0) {
                        return 1;
                    }
                    if (nMask & nNext) {
                        return nNext;
                    }
                }
            }
        }
    }
    return 0xFF;
}

long msw::SelectMenu::CCharSel::GetChrColor(unsigned short nBtnKindId, unsigned char nColor)
{
    if (nColor != 0) {
        unsigned char nBit = nColor;
        unsigned long nIndex = 0;
        for (;;) {
            if (nBit & 1) {
                break;
            }
            nBit >>= 1;
            nIndex++;
        }

        for (long i = 0; i < CHARA_BUTTON_MAX; i++) {
            if (m_aChara[i].m_nBtnKindId == 0) {
                break;
            }
            Chara *pChara = &m_aChara[i];
            if (nBtnKindId == pChara->m_nBtnKindId) {
                // Without that colour, the next open one below it of the
                // same kind (star or plain).
                unsigned char nMask = pChara->m_nColorMask;
                if (!(nMask & nColor)) {
                    if ((nColor & s_StarColorMask) && (nMask & s_StarColorMask)) {
                        for (;;) {
                            if (nMask & nColor) {
                                break;
                            }
                            nColor >>= 2;
                            nIndex -= 2;
                            if (nColor == 0) {
                                nIndex = 7;
                                nColor = 0x80;
                            }
                        }
                    } else if ((nColor & s_ColorMask2) && (nMask & s_ColorMask2)) {
                        for (;;) {
                            if (nMask & nColor) {
                                break;
                            }
                            nColor >>= 2;
                            nIndex -= 2;
                            if (nColor == 0) {
                                nIndex = 6;
                                nColor = 0x40;
                            }
                        }
                    } else {
                        for (;;) {
                            if (nMask & nColor) {
                                break;
                            }
                            nColor >>= 1;
                            nIndex--;
                        }
                    }
                }
                SetButtonFace(nBtnKindId, nIndex >> 1);
                return nIndex;
            }
        }
    }
    return -1;
}

void msw::SelectMenu::CCharSel::SetButtonFace(unsigned short nBtnKindId, unsigned char nColor)
{
    CharaPane *pCharaPane = 0;
    switch (m_nLayout) {
        case 0:
            pCharaPane = s_CharaPaneA;
            break;
        case 1:
            pCharaPane = s_CharaPaneB;
            break;
        case 2:
            pCharaPane = s_CharaPaneC;
            break;
        case 3:
            pCharaPane = s_CharaPaneD;
            break;
    }

    for (long i = 0; i < CHARA_BUTTON_MAX; i++) {
        if (m_aChara[i].m_nBtnKindId == 0) {
            break;
        }
        Chara *pChara = &m_aChara[i];
        if (nBtnKindId == pChara->m_nBtnKindId) {
            CharaPane *pPane = &pCharaPane[pChara->m_nChara];
            if (pPane->m_pColorPane != 0) {
                for (long j = 0;; j++) {
                    long nColorPane = pPane->m_pColorPane[j];
                    if (nColorPane == -1) {
                        break;
                    }
                    m_pPaneAccessor->SetPaneDisp(nColorPane, 0);
                }
                m_pPaneAccessor->SetPaneDisp(pPane->m_pColorPane[nColor], 1);
            }
            break;
        }
    }
}

void msw::SelectMenu::CCharSel::OpenCharCount()
{
    CSelectMenu *pSelectMenu = GetSelectMenu();
    SAVE_DATA_PLAYER *pPlayerData = CSave::GetPlayerData(-1);
    SAVE_DATA_PLAYER::CharaScore *pScore;

    switch (pSelectMenu->m_nGameKind) {
        case 0:
            pScore = pPlayerData->m_CharaScore[0];
            break;
        case 1:
            pScore = pPlayerData->m_CharaScore[1];
            break;
        case 2:
            pScore = pPlayerData->m_CharaScore[2];
            break;
        case 3:
            pScore = pPlayerData->m_CharaScore[3];
            break;
        default:
            pScore = m_MixScore;
            break;
    }

    unsigned long nGroup = 0;
    long nOpenNum = 0;
    for (long i = 0; i < CHARA_BUTTON_MAX; i++) {
        if (pScore[i].m_nPlayNum != 0) {
            nOpenNum++;
            nGroup |= (unsigned char)(1 << s_CharaGroup[i]);
        }
    }

    switch ((unsigned char)nGroup) {
        case 1:
            m_nLayout = 0;
            break;
        case 3:
            m_nLayout = 1;
            break;
        case 7:
            m_nLayout = 2;
            break;
        case 11:
            m_nLayout = 2;
            break;
        case 15:
            m_nLayout = 3;
            break;
    }
}

void msw::SelectMenu::CCharSel::SetMixChara(int bMix)
{
    SAVE_DATA_PLAYER *pPlayerData = CSave::GetPlayerData(-1);

    for (long i = 0; i < SAVE_DATA_PLAYER::CHARA_SCORE_NUM; i++) {
        m_MixScore[i].m_nPlayNum = 0xFF;
    }

    // What every sport has open.
    for (long nSport = 0; nSport < 5; nSport++) {
        SAVE_DATA_PLAYER::CharaScore *pScore;
        switch (nSport) {
            case 0:
                pScore = pPlayerData->m_CharaScore[0];
                m_MixScore[CHARA_MII].m_nPlayNum &= pPlayerData->m_CharaScore[0][CHARA_MII].m_nPlayNum;
                break;
            case 1:
                pScore = pPlayerData->m_CharaScore[1];
                m_MixScore[CHARA_MII].m_nPlayNum &= pPlayerData->m_CharaScore[1][CHARA_MII].m_nPlayNum;
                break;
            case 2:
                pScore = pPlayerData->m_CharaScore[2];
                m_MixScore[CHARA_MII].m_nPlayNum &= pPlayerData->m_CharaScore[2][CHARA_MII].m_nPlayNum;
                break;
            case 3:
                pScore = pPlayerData->m_CharaScore[3];
                m_MixScore[CHARA_MII].m_nPlayNum &= pPlayerData->m_CharaScore[3][CHARA_MII].m_nPlayNum;
                break;
            default:
                continue;
        }
        for (long i = 0; i < CHARA_BUTTON_MAX; i++) {
            m_MixScore[i].m_nPlayNum &= pScore[i].m_nPlayNum;
            if (bMix) {
                m_MixScore[i].m_nPlayNum &= s_ColorMask;
            }
        }
    }
}

long msw::SelectMenu::CCharSel::SendEvent(long nEvent, void *)
{
    if (m_pPaneAccessor == 0) {
        return -1;
    }
    if (nEvent == EVENT_GET_LAYOUT) {
        return m_nLayout;
    }
    return 0;
}

void msw::SelectMenu::CCharSel::CallCharVoice(long nChara)
{
    long nVoice = GetCharaVoice(nChara);
    for (long i = 0; i < 3; i++) {
        Voice_Stop(nVoice + i);
    }
    Voice_Play(nVoice + (bcs::BCS_Rand() & 3), 1);
}

void msw::SelectMenu::CCharSel::CallbackButtonAnimIn(long)
{
}

void msw::SelectMenu::CCharSel::CallbackButtonAnimOut(long)
{
}

void msw::SelectMenu::CCharSel::CallbackButtonAnimMultiIn(long, long)
{
}

void msw::SelectMenu::CCharSel::CallbackButtonAnimMultiOut(long, long)
{
}

void msw::SelectMenu::CCharSel::CallbackButtonAnimPush(long)
{
}

void msw::SelectMenu::CCharSel::CallbackButtonAnimCancel(long)
{
}

void msw::SelectMenu::CCharSel::SetCharaObjPadSign(long nPad, long nIndex)
{
    long nNum = GetCharaNum();
    SetPadSign(nIndex, nPad, s_PadSignPos[nNum][nIndex]);
}

void msw::SelectMenu::CCharSel::SetCourtCharaObj(long nBtnKindId, long nMotion, long nMotionSub, long nPad)
{
    if (nPad >= 0) {
        if (nMotion >= 0) {
            SetCourtCharaObjImpl(nPad, nMotion, nMotionSub, GetSelectMenu()->Chara_FindNotDecideCharaIndex(nPad));
            return;
        }
        long nIndex = GetSelectMenu()->Chara_FindSelectCharaIndex(nPad);
        if (nIndex >= 0) {
            long nUnk = GetSelectMenu()->GetCharaSelectParam(nIndex)->m_Unk0C;
            if (nUnk != nBtnKindId && nUnk != 0) {
                nIndex = -1;
            }
        }
        if (nIndex < 0) {
            nIndex = GetSelectMenu()->Chara_FindNoneCharaIndex(nPad);
        }
        if (nIndex >= 0) {
            SetCourtCharaObjImpl(nPad, nMotion, nMotionSub, nIndex);
        }
    } else {
        long i;
        for (i = 0; i < 4; i++) {
            if (nBtnKindId == GetCurrentButtonKindId(i)) {
                break;
            }
        }
        if (i < 4) {
            long nIndex = GetSelectMenu()->Chara_FindNoneCharaIndex(i);
            if (nIndex >= 0) {
                SetCourtCharaObjImpl(i, nMotion, nMotionSub, nIndex);
            }
        }
    }
}

void msw::SelectMenu::CCharSel::SetCourtCharaObjImplEx(long, long nMotion, long nMotionSub, long nIndex)
{
    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
    if (pCourt == 0) {
        return;
    }

    long nNum = GetCharaNum();
    if (nIndex < 0) {
        return;
    }
    CSelectMenu::CharaSelectParam *pParam = GetSelectMenu()->GetCharaSelectParam(nIndex);
    long nPos = s_PadSignPos[nNum][nIndex];
    if (pParam == 0) {
        return;
    }
    if (nMotion >= 0) {
        pCourt->SetCharaModel(nIndex, pParam->m_nChara, pParam->m_nCharaSub, pParam->m_Unk13[0], nPos,
                              pParam->m_MiiSource, pParam->m_nMiiIndex, 0);
        if (nIndex >= 3) {
            nMotionSub |= 1;
        }
        pCourt->SetCharaMotion(nIndex, nMotion, nMotionSub, 1.0f, 8.0f, 0.0f, 0);
    } else {
        pCourt->SetCharaModel(nIndex, 0xFF, 0xFF, pParam->m_Unk13[0], nPos, RFLDataSource_Default, 0, 0);
    }
}

void msw::SelectMenu::CCharSel::SetCourtCharaObjImpl(long nPad, long nMotion, long nMotionSub, long nIndex)
{
    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
    if (pCourt == 0) {
        return;
    }

    long nNum = GetCharaNum();
    if (nIndex < 0) {
        return;
    }
    CSelectMenu::CharaSelectParam *pParam = GetSelectMenu()->GetCharaSelectParam(nIndex);
    long nPos = s_PadSignPos[nNum][nIndex];
    if (pParam == 0) {
        return;
    }
    if (nMotion >= 0) {
        pCourt->SetCharaModel(nIndex, pParam->m_nChara, pParam->m_nCharaSub, pParam->m_Unk13[0], nPos,
                              pParam->m_MiiSource, pParam->m_nMiiIndex, 0);
        if (nIndex >= 3) {
            nMotionSub |= 1;
        }
        pCourt->SetCharaMotion(nIndex, nMotion, nMotionSub, 1.0f, 8.0f, 0.0f, 0);
    } else {
        pCourt->SetCharaModel(nIndex, 0xFF, 0xFF, pParam->m_Unk13[0], nPos, RFLDataSource_Default, 0, 0);
    }
    if (nMotion < 0) {
        ClearPadSign(nIndex, 0);
        SetPadSign(nIndex, nPad, nPos);
    } else {
        ClearPadSign(nIndex, 0);
    }
}

void msw::SelectMenu::CCharSel::SetCourtCharaObjMotionEx(long nIndex, long nMotion, long nMotionSub)
{
    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
    if (pCourt != 0) {
        if (nIndex >= 3) {
            nMotionSub |= 1;
        }
        pCourt->SetCharaMotion(nIndex, nMotion, nMotionSub, 1.0f, 0.0f, 0.0f, 0);
    }
}

void msw::SelectMenu::CCharSel::SetPadSign(long nIndex, long nPad, long nPos)
{
    static const long camNo = 0;

    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
    if (pCourt != 0) {
        CChrSign *pSign = (CChrSign *)GetSelectMenu()->FindObj(OBJ_CHR_SIGN);
        if (pSign != 0) {
            pSign->ClearSign(nIndex);
            bcs::FVEC3 pos;
            bool bValid = (unsigned long)nPad <= 4;
            if (bValid && pCourt->GetNodePositionByPosId(nPos, pos) == 0) {
                // A little above the place.
                float fX = 0.0f;
                float fUp = 0.5f;
                float fZ = 0.0f;
                bcs::FVEC3 offset(fX, fUp, fZ);
                bcs::FVEC3Add(&pos, &pos, &offset);
                pSign->SetSign(nIndex, nPad, &pos, camNo);
            }
        }
    }
}

void msw::SelectMenu::CCharSel::ClearPadSign(long nIndex, long)
{
    CChrSign *pSign = (CChrSign *)GetSelectMenu()->FindObj(OBJ_CHR_SIGN);
    if (pSign != 0) {
        pSign->ClearSign(nIndex);
    }
}

void msw::SelectMenu::CCharSel::SetAllCourtCharaObj()
{
    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
    if (pCourt == 0) {
        return;
    }

    long nNum = GetCharaNum();
    for (long i = 0; i < 6; i++) {
        CSelectMenu::CharaSelectParam *pParam = GetSelectMenu()->GetCharaSelectParam(i);
        if (pParam == 0) {
            continue;
        }
        long nPos = s_PadSignPos[nNum][i];
        if (GetSelectMenu()->Chara_IsCharaDecideTemporary(i)) {
            long nFlag = 0;
            pCourt->SetCharaModel(i, pParam->m_nChara, pParam->m_nCharaSub, pParam->m_Unk13[0], nPos,
                                  pParam->m_MiiSource, pParam->m_nMiiIndex, 0);
            if (i >= 3) {
                nFlag |= 1;
            }
            pCourt->SetCharaMotion(i, 2, nFlag, 1.0f, 8.0f, 0.0f, 0);
        } else if (GetSelectMenu()->Chara_IsCharaDecideAlready(i)) {
            long nFlag = 2;
            pCourt->SetCharaModel(i, pParam->m_nChara, pParam->m_nCharaSub, pParam->m_Unk13[0], nPos,
                                  pParam->m_MiiSource, pParam->m_nMiiIndex, 0);
            if (i >= 3) {
                nFlag |= 1;
            }
            pCourt->SetCharaMotion(i, 3, nFlag, 1.0f, 8.0f, 0.0f, 0);
        } else if (GetSelectMenu()->Chara_IsCharaDecideSelect(i)) {
            long nFlag = 2;
            pCourt->SetCharaModel(i, pParam->m_nChara, pParam->m_nCharaSub, pParam->m_Unk13[0], nPos,
                                  pParam->m_MiiSource, pParam->m_nMiiIndex, 0);
            if (i >= 3) {
                nFlag |= 1;
            }
            pCourt->SetCharaMotion(i, 1, nFlag, 1.0f, 8.0f, 0.0f, 0);
        } else if (GetSelectMenu()->Chara_IsCharaDecideNone(i)) {
            pCourt->SetCharaModel(i, 0xFF, 0xFF, pParam->m_Unk13[0], nPos, RFLDataSource_Default, 0, 0);
        }
    }
    GetSelectMenu()->Chara_ChangeDecideTemporaryToDecideAlready();
}

void msw::SelectMenu::CCharSel::SetColorMarkerObj()
{
    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
    if (pCourt == 0) {
        return;
    }

    long nNum = GetCharaNum();
    int bNoCom = 0;
    unsigned char nKind = GetSelectMenu()->m_Unk9B;
    if ((long)nKind == 1 || (long)nKind == 4) {
        bNoCom = 1;
    }
    for (long i = 0; i < 6; i++) {
        CSelectMenu::CharaSelectParam *pParam = GetSelectMenu()->GetCharaSelectParam(i);
        if (pParam == 0) {
            continue;
        }
        unsigned char nPlayer = pParam->m_nPlayerKind;
        unsigned char nMarker = s_MarkerPos[nNum][i];
        if (nPlayer == 0xFF || (nPlayer == 4 && bNoCom)) {
            pCourt->SetMarkerDisp(nMarker, 0);
        } else {
            long nColor;
            if (nKind != 0) {
                if (nPlayer < 4) {
                    nColor = GetSelectMenu()->m_aPadSelect[nPlayer].m_nPlayerNo;
                } else {
                    nColor = 4;
                }
            } else {
                // The two teams of three.
                nColor = i >= 3;
            }
            pCourt->SetMarkerColor(nMarker, nColor);
            pCourt->SetMarkerDisp(nMarker, 1);
        }
    }
    pCourt->SetMarkerAlpha(0.0f);
}

void msw::SelectMenu::CCharSel::SetPadCharaObjMotion(long nPad, long nMotion, long nMotionSub)
{
    CCourtChr *pCourt = (CCourtChr *)GetSelectMenu()->FindObj(OBJ_COURT_CHR);
    if (pCourt == 0) {
        return;
    }

    GetCharaNum();
    long nIndex = GetSelectMenu()->Chara_FindNotDecideCharaIndex(nPad);
    if (nIndex >= 0 && GetSelectMenu()->GetCharaSelectParam(nIndex) != 0) {
        if (nIndex >= 3) {
            nMotionSub |= 1;
        }
        pCourt->SetCharaMotion(nIndex, nMotion, nMotionSub, 1.0f, 0.0f, 0.0f, 0);
    }
}
