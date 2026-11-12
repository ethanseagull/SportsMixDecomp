// The player select screen: CSelectMenu::PlaterSelect_* (the game's
// spelling) and its menu object, SelectMenu::CSaveSelect. The four save
// slots can be chosen, a new player made in an empty one, or a player
// moved to another slot ("change") or deleted.
//
// The function-local statics (padNo0, objId, btnKindId, reset, recursive)
// come out in declaration order here and in reverse in the game, so their
// @LOCAL@ symbol names are exchanged in pairs (all hold the same bytes):
// static-order swap (see notes/decisions.md).
#include "game/msw/SelectMenu/CSaveSelect.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/CMiiMan.hpp"
#include "bcs/CPadWI.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuOption.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CMiiSelectRender.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/CSelectMenu.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/SAVE_DATA_PLAYER.hpp"
#include "game/msw/SelectMenu/CBgChr.hpp"
#include "game/msw/SelectMenu/CCourtChr.hpp"
#include "game/msw/SelectMenu/CPlusMinus.hpp"
#include "game/msw/SelectMenu/CTitle.hpp"
#include "game/msw/Unlock.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "revolution/gx/GXTexture.h"
#include "string.h"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The speed of the screen's opening and closing animations. A table,
    // not a scalar, as in game/menu/CWiFiTopA.cpp: the original converts
    // the value to float at run time and keeps the word, first in .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // The camera animations of the menu background the screen sets when a
    // player has been chosen (CCameraMenuBG::SetAnimTable; variant 1 with
    // the mix sports open). MenuF's CGamePadCheck has the same table.
    const msw::CCameraMenuBG::AnimInfo s_CameraAnimTable[] = {
        { { { "cat00_ST", "cat00_ST" }, { "MX_cat00_ST", "MX_cat00_ST" } }, 0, 0, 0, 0, 0, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_ST_HK", "cat00_ST_HK" }, { "MX_cat00_ST_HK", "MX_cat00_ST_HK" } }, 0, 0, 0, 0, 0, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_BK_VB", "cat00_BK_VB" }, { "MX_cat00_BK_VB", "MX_cat00_BK_VB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_VB_DB", "cat00_VB_DB" }, { "MX_cat00_VB_DB", "MX_cat00_VB_DB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_DB_HK", "cat00_DB_HK" }, { "MX_cat00_DB_HK", "MX_cat00_DB_HK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_HK_BK", "cat00_HK_BK" }, { "MX_cat00_HK_BK", "MX_cat00_HK_BK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_BK", "cat00_BK" }, { "MX_cat00_BK", "MX_cat00_BK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_VB", "cat00_VB" }, { "MX_cat00_VB", "MX_cat00_VB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_DB", "cat00_DB" }, { "MX_cat00_DB", "MX_cat00_DB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_HK", "cat00_HK" }, { "MX_cat00_HK", "MX_cat00_HK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_BK", "cat01_BK" }, { "MX_cat01_BK", "MX_cat01_BK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_VB", "cat01_VB" }, { "MX_cat01_VB", "MX_cat01_VB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_DB", "cat01_DB" }, { "MX_cat01_DB", "MX_cat01_DB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_HK", "cat01_HK" }, { "MX_cat01_HK", "MX_cat01_HK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat02_BK", "cat02_BK" }, { "cat02_BK", "cat02_BK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat02_VB", "cat02_VB" }, { "cat02_VB", "cat02_VB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat02_DB", "cat02_DB" }, { "cat02_DB", "cat02_DB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat02_HK", "cat02_HK" }, { "cat02_HK", "cat02_HK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_CL", "cat00_CL" }, { "MX_cat00_MX", "MX_cat00_MX" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_VB_DB", "cat00_VB_DB" }, { "MX_cat00_VB_MX", "MX_cat00_VB_MX" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_VB_DB", "cat00_VB_DB" }, { "MX_cat00_MX_DB", "MX_cat00_MX_DB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_CL", "cat01_CL" }, { "MX_cat01_MX", "MX_cat01_MX" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_pos00_cat01", "cat00_pos00_cat01" }, { "cat00_pos00_cat01", "cat00_pos00_cat01" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_pos01_cat01", "cat00_pos01_cat01" }, { "cat00_pos01_cat01", "cat00_pos01_cat01" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_pos02_cat01", "cat00_pos02_cat01" }, { "cat00_pos02_cat01", "cat00_pos02_cat01" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat00_pos03_cat01", "cat00_pos03_cat01" }, { "cat00_pos03_cat01", "cat00_pos03_cat01" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01", "cat01" }, { "cat01", "cat01" } }, 0, 1, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_cat00_pos00", "cat01_cat00_pos00" }, { "cat01_cat00_pos00", "cat01_cat00_pos00" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat02_BK", "cat02_BK" }, { "cat02_BK", "cat02_BK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat02_VB", "cat02_VB" }, { "cat02_VB", "cat02_VB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat02_DB", "cat02_DB" }, { "cat02_DB", "cat02_DB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat02_HK", "cat02_HK" }, { "cat02_HK", "cat02_HK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "m_cat03", "cat03" }, { "m_cat03", "cat03" } }, 0, 0, 0, 0, 1, 0, 14.0f, 0.0f, 2.0f },
        { { { "m_cat04", "cat04" }, { "m_cat04", "cat04" } }, 0, 0, 0, 0, 1, 1, 50.0f, 0.0f, 2.0f },
        { { { "m_cat05_S", "cat05_S" }, { "m_cat05_S", "cat05_S" } }, 0, 0, 0, 0, 1, 1, 50.0f, 0.0f, 2.0f },
        { { { "m_cat05_D", "cat05_D" }, { "m_cat05_D", "cat05_D" } }, 0, 0, 0, 0, 1, 1, 50.0f, 0.0f, 2.0f },
        { { { "m_cat06_S", "cat06_S" }, { "m_cat06_S", "cat06_S" } }, 0, 0, 0, 0, 1, 1, 50.0f, 0.0f, 2.0f },
        { { { "m_cat06_D", "cat06_D" }, { "m_cat06_D", "cat06_D" } }, 0, 0, 0, 0, 1, 1, 50.0f, 0.0f, 2.0f },
        { { { "cat00", "cat00" }, { "cat00", "cat00" } }, 0, 1, 0, 0, 1, 1, 50.0f, 0.0f, 2.0f },
        { { { "m_cat07", "cat07" }, { "m_cat07", "cat07" } }, 0, 0, 0, 0, 1, 1, 50.0f, 0.0f, 2.0f },
        { { { "m_cat08", "cat08" }, { "m_cat08", "cat08" } }, 0, 0, 0, 0, 1, 1, 50.0f, 0.0f, 2.0f },
        { { { "m_cat_ball", "cat_ball" }, { "m_cat_ball", "cat_ball" } }, 0, 0, 0, 0, 1, 1, 4.0f, 0.0f, 2.0f },
        { { { "cat01_BK_VB", "cat01_BK_VB" }, { "MX_cat01_BK_VB", "MX_cat01_BK_VB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_BK_DB", "cat01_BK_DB" }, { "MX_cat01_BK_DB", "MX_cat01_BK_DB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_BK_HK", "cat01_BK_HK" }, { "MX_cat01_BK_HK", "MX_cat01_BK_HK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_BK_MX", "cat01_BK_MX" }, { "MX_cat01_BK_MX", "MX_cat01_BK_MX" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_VB_BK", "cat01_VB_BK" }, { "MX_cat01_VB_BK", "MX_cat01_VB_BK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_VB_DB", "cat01_VB_DB" }, { "MX_cat01_VB_DB", "MX_cat01_VB_DB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_VB_HK", "cat01_VB_HK" }, { "MX_cat01_VB_HK", "MX_cat01_VB_HK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_VB_MX", "cat01_VB_MX" }, { "MX_cat01_VB_MX", "MX_cat01_VB_MX" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_DB_BK", "cat01_DB_BK" }, { "MX_cat01_DB_BK", "MX_cat01_DB_BK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_DB_VB", "cat01_DB_VB" }, { "MX_cat01_DB_VB", "MX_cat01_DB_VB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_DB_HK", "cat01_DB_HK" }, { "MX_cat01_DB_HK", "MX_cat01_DB_HK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_DB_MX", "cat01_DB_MX" }, { "MX_cat01_DB_MX", "MX_cat01_DB_MX" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_HK_BK", "cat01_HK_BK" }, { "MX_cat01_HK_BK", "MX_cat01_HK_BK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_HK_VB", "cat01_HK_VB" }, { "MX_cat01_HK_VB", "MX_cat01_HK_VB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_HK_DB", "cat01_HK_DB" }, { "MX_cat01_HK_DB", "MX_cat01_HK_DB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_HK_MX", "cat01_HK_MX" }, { "MX_cat01_HK_MX", "MX_cat01_HK_MX" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_MX_BK", "cat01_MX_BK" }, { "MX_cat01_MX_BK", "MX_cat01_MX_BK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_MX_VB", "cat01_MX_VB" }, { "MX_cat01_MX_VB", "MX_cat01_MX_VB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_MX_DB", "cat01_MX_DB" }, { "MX_cat01_MX_DB", "MX_cat01_MX_DB" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
        { { { "cat01_MX_HK", "cat01_MX_HK" }, { "MX_cat01_MX_HK", "MX_cat01_MX_HK" } }, 0, 0, 0, 0, 1, 1, 1.0f, 0.0f, 2.0f },
    };

    const long CAMERA_ANIM_NUM = sizeof(s_CameraAnimTable) / sizeof(s_CameraAnimTable[0]);

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_TASK = 1,
        OBJ_PLUS_MINUS = 2,
        OBJ_TOP_MENU = 3,
        OBJ_TITLE = 4,
        OBJ_SAVE_SELECT = 5,
        OBJ_BG_CHR = 0x3A,
        OBJ_COURT_CHR = 0x3B,
        OBJ_DWC_WINDOW = 0x3F,
        OBJ_UNK_40 = 0x40,
        OBJ_UNK_42 = 0x42
    };

    // Events sent to the menu objects.
    enum {
        EVENT_TOP_UNK_07 = 7,          // to OBJ_TOP
        EVENT_LINE_MESSAGE = 0x11,     // to OBJ_TOP: the help line
        EVENT_HIT = 0x13,
        EVENT_BACK = 0x20,
        EVENT_PUSH = 0x21,             // plus the pad number
        EVENT_CLOSE = 0x25,            // to OBJ_TASK and OBJ_PLUS_MINUS
        EVENT_CLOSE_CHECK = 0x26,
        EVENT_WINDOW_CLOSE = 0x32,     // to OBJ_DWC_WINDOW
        EVENT_BG_CHR_UNK_33 = 0x33,    // to OBJ_BG_CHR
        EVENT_PLUS_MINUS_HIDE = 0x48,  // to OBJ_PLUS_MINUS
        EVENT_PLUS_MINUS_SHOW = 0x49,  // to OBJ_PLUS_MINUS
        EVENT_TASK_UNK_54 = 0x54,      // to OBJ_TASK
        EVENT_TASK_UNK_55 = 0x55       // to OBJ_TASK
    };

    // The button kind ids of the screen.
    enum {
        BTN_KIND_NONE = 0,
        BTN_KIND_BACK = 1,
        BTN_KIND_SLOT_0 = 0x8020, // one per save slot
        BTN_KIND_PLUS = 0x80A2,   // the plus/minus buttons
        BTN_KIND_MINUS = 0x80A3
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_SETUP = 0,
        STEP_MAIN = 1,
        STEP_FADE = 2,
        STEP_NEW_ASK = 3,
        STEP_MOVE_WAIT = 4,
        STEP_CHANGE = 5,
        STEP_DELETE = 6,
        STEP_DELETE_ASK = 7,
        STEP_DELETE_DATA = 8,
        STEP_DELETE_INIT = 9,
        STEP_DELETE_WRITE = 10,
        STEP_DELETE_WRITE_WAIT = 11,
        STEP_DELETE_END = 12,
        STEP_MII_LOST = 13,
        STEP_NEW_SAVE = 14,
        STEP_NEW_SAVE_WAIT = 15,
        STEP_NEW_END = 16,
        STEP_NEW_TOP_MENU = 17
    };

    // Values of CSelectMenu::m_nMode / m_nNextMode used here.
    enum {
        MODE_MII_SEL = 3,
        MODE_TOP = 4
    };

    // Answers of the message windows (CCockpit::GetResult).
    enum {
        RESULT_YES = 1000,
        RESULT_NO = 1001
    };

    // The cockpit object of the message windows (CCockpit::IsEndObj).
    enum {
        COCKPIT_WINDOW = 0x1042
    };

    // Bits of CSelectMenu::m_nMiiSelFlags set here.
    enum {
        MII_SEL_NEW = 1,    // a new player is being made
        MII_SEL_CHANGE = 2, // a player's Mii is being changed
        MII_SEL_LOST = 3,   // a player's Mii was lost
        MII_SEL_BACK = 0x80
    };

    // Remote buttons of the unlock sequence (bcs::CPadWI).
    enum {
        PAD_1 = 0x200,
        PAD_2 = 0x2000,
        PAD_B = 0x4000
    };
}

void msw::CSelectMenu::PlaterSelect_Start()
{
    static const long objId = OBJ_SAVE_SELECT;
    static const long padNo0 = 0;

    Build(objId);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    SelectMenu::CSaveSelect *pObj = (SelectMenu::CSaveSelect *)FindObj(objId);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->SetUpSaveData();
        if (m_Unk23E == 1) {
            pObj->m_nPushButtonId[padNo0] = nBtnKindId;
            PlaterSelect_LineMessage(nBtnKindId);
        }
        m_nStep = STEP_SETUP;
    }
}

void msw::CSelectMenu::PlaterSelect_Save()
{
}

int msw::CSelectMenu::PlaterSelect_SetUp()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    m_pPlayerData = CSave::GetPlayerData(-1);
    m_pPlayerData->ClearSelectMii();

    for (long i = 0; i < 4; i++) {
        SAVE_DATA_PLAYER *pData = CSave::GetPlayerData(i);
        if (pData->IsValid()) {
            RFLStoreData storeData;
            if (bcs::CMiiMan::GetStoreData(storeData, pData->m_MiiSource, pData->m_MiiCreateId) != 0) {
                // The player's Mii is gone: say so, and go to the Mii
                // select screen for that player.
                WindowOK_Button("CharaEntryMessage", "PMAKE_09_00", pData->m_Name);
                m_nCurrentBtnKindId[padNo0][nMode] = BTN_KIND_SLOT_0 + i;
                CSave::SetUsePlayerID(i);
                PlaterSelect_MiiSetUp(i);
                SendObjEventTrigger(OBJ_TASK, EVENT_TASK_UNK_54, 0);
                m_nStep = STEP_MII_LOST;
                return -1;
            }
            PlaterSelect_NameUpDate(pData);
        }
    }
    return 0;
}

void msw::CSelectMenu::PlaterSelect_MiiSetUp(long nPlayer)
{
    static const long padNo0 = 0;

    m_pPlayerData = CSave::GetPlayerData(nPlayer);
    m_pPlayerData->ClearSelectMii();

    // The other players' own Miis cannot be chosen.
    for (long i = 0; i < 4; i++) {
        SAVE_DATA_PLAYER *pData = CSave::GetPlayerData(i);
        if (pData->IsValid()) {
            RFLStoreData storeData;
            if (bcs::CMiiMan::GetStoreData(storeData, pData->m_MiiSource, pData->m_MiiCreateId) == 0) {
                PlaterSelect_NameUpDate(pData);
                m_pPlayerData->SetSelectMii(m_pPlayerData->SearchMiiIndex(pData->m_MiiCreateId), 1);
            }
        }
    }
}

void msw::CSelectMenu::PlaterSelect_NameUpDate(SAVE_DATA_PLAYER *pData)
{
    if (pData->m_nMiiUnk == 0) {
        RFLStoreData storeData;
        bcs::CMiiMan::GetStoreData(storeData, pData->m_MiiSource, pData->m_MiiCreateId);
        RFLAdditionalInfo info;
        bcs::CMiiMan::GetAdditionalInfo(info, storeData);
        wcscpy(pData->m_Name, info.name);
    }
}

void msw::CSelectMenu::PlaterSelect_In()
{
    // Declared in this order so that the two words come out in the
    // game's order (see the static-order swap note above).
    static const long btnKindId = BTN_KIND_NONE;
    static const long objId = OBJ_SAVE_SELECT;

    m_Unk9B = 0;
    Scratch(OBJ_BG_CHR);
    Scratch(OBJ_COURT_CHR);
    Scratch(OBJ_UNK_40);
    Scratch(OBJ_UNK_42);

    SelectMenu::CTitle *pTitle = (SelectMenu::CTitle *)FindObj(OBJ_TITLE);
    if (pTitle != 0) {
        if (pTitle->m_Unk2C == 1) {
            pTitle->ButtonOff();
            pTitle->BGOn();
        } else if (pTitle->m_Unk2C == 0) {
            Build(OBJ_TITLE);
            pTitle->ButtonOff();
            pTitle->BGOn();
        }
    }
    CGamePadCheck::GetInstance()->EnableBatteryCheck(1);

    if (FindObj(objId) != 0) {
        if (m_nMiiSelFlags & MII_SEL_BACK) {
            m_nStep = STEP_SETUP;
        } else if (m_Unk9D == MODE_MII_SEL) {
            m_nStep = STEP_NEW_SAVE;
        } else {
            for (long nMode = 0; nMode < 51; nMode++) {
                for (long nPad = 0; nPad < 4; nPad++) {
                    SetHistoryButtonKindId(nMode, nPad, btnKindId);
                }
            }
            InitCharaSelectParam();
            SetCharaSelectSaveParam();
            m_nStep = STEP_SETUP;
        }
    }
}

void msw::CSelectMenu::PlaterSelect_Loop()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CSaveSelect *pObj = (SelectMenu::CSaveSelect *)FindObj(OBJ_SAVE_SELECT);
    LoopPlayMenuBGM(0);

    switch (m_nStep) {
        case STEP_SETUP:
            if (PlaterSelect_SetUp() == 0) {
                SendObjEventTrigger(OBJ_TASK, EVENT_TASK_UNK_55, 0);
                m_nMiiSelFlags = 0;
                PlaterSelect_Start();
                m_nStep = STEP_MAIN;
            }
            break;
        case STEP_MAIN:
            PlaterSelect_LoopStep();
            break;
        case STEP_FADE:
            // A player was chosen: on to the top menu once the fade is
            // over.
            if (!bcs::CMainApp::IsPlayMasterFade()) {
                SetNextMode(MODE_TOP, pObj);
                SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, 0);
                m_nGameKind = 0;

                int bMix = _IsOpenMixSportsFlag();
                SelectMenu::CBgChr *pBgChr = (SelectMenu::CBgChr *)FindObj(OBJ_BG_CHR);
                if (pBgChr != 0) {
                    pBgChr->SetMixModeFlag(bMix);
                }
                SelectMenu::CCourtChr *pCourtChr = (SelectMenu::CCourtChr *)FindObj(OBJ_COURT_CHR);
                if (pCourtChr != 0) {
                    pCourtChr->SetMixModeFlag(bMix);
                }
                pCourtChr = (SelectMenu::CCourtChr *)FindObj(OBJ_UNK_40);
                if (pCourtChr != 0) {
                    pCourtChr->SetMixModeFlag(bMix);
                }
                pCourtChr = (SelectMenu::CCourtChr *)FindObj(OBJ_UNK_42);
                if (pCourtChr != 0) {
                    pCourtChr->SetMixModeFlag(bMix);
                }
                Build(OBJ_BG_CHR);
                Build(OBJ_COURT_CHR);
                Build(OBJ_UNK_40);
                Build(OBJ_UNK_42);
                CCameraMenuBG::s_Instance.SetAnimTable(s_CameraAnimTable, CAMERA_ANIM_NUM, _IsOpenMixSportsFlag() == 1);
                SendObjEventTrigger(OBJ_BG_CHR, EVENT_BG_CHR_UNK_33, (void *)1);
                CMenuOption::s_Instance.GetPlayerSaveData(CSave::GetUsePlayerID(), 0);
            }
            break;
        case STEP_NEW_ASK: {
            // "Make a new player?"
            if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
                return;
            }
            long nPlayer = m_nCurrentBtnKindId[padNo0][nMode] - BTN_KIND_SLOT_0;
            switch (CCockpit::GetResult()) {
                case RESULT_YES:
                    m_nMiiSelFlags = MII_SEL_NEW;
                    m_Unk27D = 0;
                    m_nGameKind = -1;
                    PlaterSelect_MiiSetUp(nPlayer);
                    SetNextMode(MODE_MII_SEL, pObj);
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                    break;
                case RESULT_NO:
                    m_nStep = STEP_MAIN;
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                    break;
            }
            break;
        }
        case STEP_MOVE_WAIT: {
            // The slots move aside, then the change or delete step.
            if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
                return;
            }
            float fWait = m_fRandomMoveTime[0];
            fWait -= bcs::CMainApp::GetAppFrameRateF();
            if (fWait <= 0.0f) {
                if (m_nPlaterNextStep == STEP_CHANGE) {
                    pObj->OpenMove(0);
                } else {
                    pObj->OpenMove(1);
                }
                m_nStep = m_nPlaterNextStep;
            }
            m_fRandomMoveTime[0] = fWait;
            InputDataToMenuObj(pObj, padNo0, nPad, nMode);
            break;
        }
        case STEP_CHANGE:
            PlaterSelect_ChangeStep();
            break;
        case STEP_DELETE:
            PlaterSelect_DeleteStep();
            break;
        case STEP_DELETE_ASK:
            // "Delete this player?", asked twice.
            switch (CCockpit::GetResult()) {
                case RESULT_YES:
                    if (m_nDeleteAsk == 0) {
                        WindowYesNo_Button("CharaEntryMessage", "PMAKE_07_00", 0, 0, 0, 1);
                        m_nDeleteAsk = 1;
                    } else {
                        m_nStep = STEP_DELETE_DATA;
                        m_nDeleteAsk = 2;
                    }
                    break;
                case RESULT_NO:
                    m_nStep = STEP_DELETE;
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                    break;
            }
            break;
        case STEP_DELETE_DATA: {
            // The name is kept for the message.
            SAVE_DATA_PLAYER *pData = CSave::GetPlayerData(m_nDeletePlayer);
            memcpy(m_FriendNameWork, pData->m_Name, sizeof(m_FriendNameWork));
            pData->Init();
            m_nStep = STEP_DELETE_INIT;
            break;
        }
        case STEP_DELETE_INIT:
            CSave::SaveDataInit();
            m_nStep = STEP_DELETE_WRITE;
            break;
        case STEP_DELETE_WRITE:
            if (CSave::CanbeWrite()) {
                CSave::WriteAsync();
                m_nStep = STEP_DELETE_WRITE_WAIT;
            }
            break;
        case STEP_DELETE_WRITE_WAIT:
            if (CSave::IsComplete()) {
                WindowA_Button("CharaEntryMessage", "PMAKE_08", m_FriendNameWork, 0);
                m_nStep = STEP_DELETE_END;
            }
            break;
        case STEP_DELETE_END:
            if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
                return;
            }
            if (CCockpit::GetResult() == RESULT_YES) {
                pObj->CloseMove();
                PlaterSelect_LineMessage(0);
                SendObjEventTrigger(OBJ_TOP, EVENT_TOP_UNK_07, 0);
                m_nStep = STEP_SETUP;
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
            }
            break;
        case STEP_MII_LOST:
            if (CCockpit::GetResult() == RESULT_YES) {
                SendObjEventTrigger(OBJ_TASK, EVENT_TASK_UNK_55, 0);
                m_nMiiSelFlags = MII_SEL_LOST;
                m_Unk27D = 0;
                m_nGameKind = -1;
                SetNextMode(MODE_MII_SEL, 1.0f);
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
            }
            break;
        case STEP_NEW_SAVE: {
            static const long padNo0 = 0;

            // Back from the Mii select screen: store the new player.
            long nPlayer = m_nCurrentBtnKindId[padNo0][nMode] - BTN_KIND_SLOT_0;
            SAVE_DATA_PLAYER *pData = CSave::GetPlayerData(nPlayer);
            if (!(m_nMiiSelFlags & MII_SEL_CHANGE)) {
                CMenuOption::SetDefaultData(nPlayer);
            }
            unsigned long long nCreateId = Mii_GetMiiCreateId(0);
            pData->m_MiiCreateId = *(RFLCreateID *)&nCreateId;
            pData->m_MiiSource = Mii_GetMiiDataSource(0);
            pData->m_nMiiUnk = Mii_GetMiiMode(0);
            pData->m_nMiiIndex = Mii_GetMiiIndex(0);
            if (Mii_GetMiiMode(0)) {
                wcscpy(pData->m_Name, GetMessageString("List_Mii_Name", Mii_GetMiiIndex(0)));
            } else {
                RFLStoreData storeData;
                bcs::CMiiMan::GetStoreData(storeData, pData->m_MiiSource, pData->m_MiiCreateId);
                RFLAdditionalInfo info;
                bcs::CMiiMan::GetAdditionalInfo(info, storeData);
                wcscpy(pData->m_Name, info.name);
            }
            if (CSave::CanbeWrite()) {
                CSave::WriteAsync();
                m_nStep = STEP_NEW_SAVE_WAIT;
            }
            break;
        }
        case STEP_NEW_SAVE_WAIT:
            if (CSave::IsComplete()) {
                SAVE_DATA_PLAYER *pData = CSave::GetPlayerData(m_nCurrentBtnKindId[padNo0][nMode] - BTN_KIND_SLOT_0);
                if (m_nMiiSelFlags & MII_SEL_CHANGE) {
                    WindowA_Button("CharaEntryMessage", "PMAKE_11", 0, 0);
                } else {
                    WindowA_Button("CharaEntryMessage", "PMAKE_10", pData->m_Name, 0);
                }
                m_nStep = STEP_NEW_END;
            }
            break;
        case STEP_NEW_END:
            if (CCockpit::GetResult() == RESULT_YES) {
                if (Mii_GetMiiMode(0) == 0) {
                    WindowA_Button("CharaEntryMessage", "PMAKE_03", 0, 0);
                    m_nStep = STEP_NEW_END;
                } else if (m_nMiiSelFlags & MII_SEL_CHANGE) {
                    PlaterSelect_SaveClose();
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                } else {
                    m_fInputWait = 30.0f;
                    Build(OBJ_TOP_MENU);
                    m_nStep = STEP_NEW_TOP_MENU;
                }
                SAVE_DATA_PLAYER *pData = CSave::GetPlayerData(m_nCurrentBtnKindId[padNo0][nMode] - BTN_KIND_SLOT_0);
                Mii_CancelMiiParam(0, pData->m_MiiSource, pData->m_nMiiIndex);
            }
            break;
        case STEP_NEW_TOP_MENU:
            m_fInputWait -= GetFrameRateF();
            if (m_fInputWait < 0.0f) {
                m_fInputWait = 0.0f;
                if (CMenuPad::s_Instance.GetInput(nPad) & 0x10) {
                    menu::CSndEff::s_Instance.Play(0xBBB, 1);
                    PlaterSelect_SaveClose();
                }
            }
            break;
    }
}

void msw::CSelectMenu::PlaterSelect_SaveClose()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    if (m_nMiiSelFlags & MII_SEL_CHANGE) {
        m_nStep = STEP_SETUP;
    } else {
        CSave::SetUsePlayerID(m_nCurrentBtnKindId[padNo0][nMode] - BTN_KIND_SLOT_0);
        SetWhiteFadeOut(60.0f);
        m_nStep = STEP_FADE;
    }
    m_nMiiSelFlags = 0;
}

void msw::CSelectMenu::PlaterSelect_LoopStep()
{
    static const long objId = OBJ_SAVE_SELECT;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    if (!CCockpit::IsEndObj(COCKPIT_WINDOW)) {
        return;
    }
    SelectMenu::CTitle *pTitle = (SelectMenu::CTitle *)FindObj(OBJ_TITLE);
    if (!CSave::IsComplete()) {
        return;
    }
    SelectMenu::CSaveSelect *pObj = (SelectMenu::CSaveSelect *)FindObj(objId);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    long nBtnKindId = rBtnKindId;
    if (CMenuPad::s_Instance.GetInput(nPad) & 0x10) {
        _GetModeChangeWaitTime();
        switch (nBtnKindId) {
            case BTN_KIND_SLOT_0:
            case BTN_KIND_SLOT_0 + 1:
            case BTN_KIND_SLOT_0 + 2:
            case BTN_KIND_SLOT_0 + 3: {
                pObj->PushButton(padNo0, nBtnKindId);
                rBtnKindId = nBtnKindId;
                SAVE_DATA_PLAYER *pData = CSave::GetPlayerData(nBtnKindId - BTN_KIND_SLOT_0);
                CSave::SetUsePlayerID(nBtnKindId - BTN_KIND_SLOT_0);
                if (pData->IsValid()) {
                    pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
                    SetWhiteFadeOut(60.0f);
                    m_nStep = STEP_FADE;
                } else {
                    WindowYesNo_Button("CharaEntryMessage", "PMAKE_01_00", 1, 0, 0, 1);
                    m_nStep = STEP_NEW_ASK;
                }
                break;
            }
            case BTN_KIND_BACK:
                PlaterSelect_BackTitle();
                SendObjEventTrigger(OBJ_TASK, EVENT_CLOSE, 0);
                pTitle->ButtonOn();
                break;
            case BTN_KIND_PLUS:
                pObj->PushButton(padNo0, BTN_KIND_PLUS);
                PlaterSelect_PlusMinus(padNo0, nBtnKindId, STEP_DELETE);
                break;
            case BTN_KIND_MINUS:
                pObj->PushButton(padNo0, BTN_KIND_MINUS);
                PlaterSelect_PlusMinus(padNo0, nBtnKindId, STEP_CHANGE);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x20) {
        PlaterSelect_BackTitle();
        SendObjEventTrigger(OBJ_TASK, EVENT_CLOSE, 0);
        pTitle->ButtonOn();
    } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x400) {
        pObj->PushButton(padNo0, BTN_KIND_MINUS);
        PlaterSelect_PlusMinus(padNo0, nBtnKindId, STEP_CHANGE);
    } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x800) {
        pObj->PushButton(padNo0, BTN_KIND_PLUS);
        PlaterSelect_PlusMinus(padNo0, nBtnKindId, STEP_DELETE);
    } else {
        // The unlock sequence on remote 0: B held and 1 three times, 2
        // held and 1 four times, both held and 1 five times. Any other
        // button starts it again.
        if (bcs::CPadWI::GetDown(0) & ~(PAD_1 | PAD_2 | PAD_B)) {
            m_nUnlockStep = 0;
            m_nUnlockCount = 0;
        }
        switch (m_nUnlockStep) {
            case 0:
                if (bcs::CPadWI::IsDown(PAD_B, 0) && !bcs::CPadWI::IsDown(PAD_2, 0)) {
                    if (bcs::CPadWI::IsTrg(PAD_1, 0)) {
                        m_nUnlockCount++;
                    }
                    if (m_nUnlockCount >= 3) {
                        m_nUnlockCount = 0;
                        m_nUnlockStep++;
                    }
                }
                break;
            case 1:
                if (bcs::CPadWI::IsDown(PAD_2, 0) && !bcs::CPadWI::IsDown(PAD_B, 0)) {
                    if (bcs::CPadWI::IsTrg(PAD_1, 0)) {
                        m_nUnlockCount++;
                    }
                    if (m_nUnlockCount >= 4) {
                        m_nUnlockCount = 0;
                        m_nUnlockStep++;
                    }
                } else if (bcs::CPadWI::IsTrg(PAD_1, 0)) {
                    m_nUnlockStep = 99;
                }
                break;
            case 2:
                if (bcs::CPadWI::IsDown(PAD_B, 0) && bcs::CPadWI::IsDown(PAD_2, 0)) {
                    if (bcs::CPadWI::IsTrg(PAD_1, 0)) {
                        m_nUnlockCount++;
                    }
                    if (m_nUnlockCount >= 5) {
                        if (nBtnKindId == BTN_KIND_NONE) {
                            m_nUnlockStep = 99;
                        } else {
                            if ((unsigned long)(nBtnKindId - BTN_KIND_SLOT_0) <= 3 &&
                                CSave::GetPlayerData(nBtnKindId - BTN_KIND_SLOT_0)->IsValid()) {
                                Unlock::FullOpen(nBtnKindId - BTN_KIND_SLOT_0);
                                menu::CSndEff::s_Instance.Play(0xBBD, 1);
                            }
                            m_nUnlockCount = 0;
                            m_nUnlockStep++;
                        }
                    }
                } else if (bcs::CPadWI::IsTrg(PAD_1, 0)) {
                    m_nUnlockStep = 99;
                }
                break;
            default:
                m_nUnlockStep = 0;
                m_nUnlockCount = 0;
                break;
        }
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
        PlaterSelect_LineMessage(0);
    }
}

void msw::CSelectMenu::PlaterSelect_ChangeStep()
{
    static const long objId = OBJ_SAVE_SELECT;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    if (!CCockpit::IsEndObj(COCKPIT_WINDOW)) {
        return;
    }
    SelectMenu::CSaveSelect *pObj = (SelectMenu::CSaveSelect *)FindObj(objId);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    long nBtnKindId = rBtnKindId;
    if (CMenuPad::s_Instance.GetInput(nPad) & 0x10) {
        _GetModeChangeWaitTime();
        switch (nBtnKindId) {
            case BTN_KIND_SLOT_0:
            case BTN_KIND_SLOT_0 + 1:
            case BTN_KIND_SLOT_0 + 2:
            case BTN_KIND_SLOT_0 + 3:
                // Change the Mii of this player.
                m_nMiiSelFlags = MII_SEL_CHANGE;
                pObj->PushButton(padNo0, nBtnKindId);
                // The result is not used.
                CSave::GetPlayerData(nBtnKindId - BTN_KIND_SLOT_0);
                rBtnKindId = nBtnKindId;
                CSave::SetUsePlayerID(nBtnKindId - BTN_KIND_SLOT_0);
                pObj->m_pPaneAccessor->PlayAnimation(9, 0xC, 1.0f, 0.0f, 1, 1);
                pObj->m_nMoveStep = 0;
                m_Unk27D = 0;
                m_nGameKind = -1;
                PlaterSelect_MiiSetUp(nBtnKindId - BTN_KIND_SLOT_0);
                SetNextMode(MODE_MII_SEL, pObj);
                break;
            case BTN_KIND_BACK:
                pObj->PushButton(padNo0, nBtnKindId);
                pObj->CloseMove();
                PlaterSelect_LineMessage(0);
                SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_PLUS_MINUS_SHOW, (void *)2);
                SendObjEventTrigger(OBJ_TOP, EVENT_TOP_UNK_07, 0);
                m_nStep = STEP_MAIN;
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x20) {
        pObj->PushButton(padNo0, BTN_KIND_BACK);
        pObj->CloseMove();
        PlaterSelect_LineMessage(0);
        SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_PLUS_MINUS_SHOW, (void *)2);
        SendObjEventTrigger(OBJ_TOP, EVENT_TOP_UNK_07, 0);
        m_nStep = STEP_MAIN;
    } else {
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
    }
}

void msw::CSelectMenu::PlaterSelect_DeleteStep()
{
    static const long objId = OBJ_SAVE_SELECT;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    if (!CCockpit::IsEndObj(COCKPIT_WINDOW)) {
        return;
    }
    SelectMenu::CSaveSelect *pObj = (SelectMenu::CSaveSelect *)FindObj(objId);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & 0x10) {
        float fWait = _GetModeChangeWaitTime();
        switch (nBtnKindId) {
            case BTN_KIND_SLOT_0:
            case BTN_KIND_SLOT_0 + 1:
            case BTN_KIND_SLOT_0 + 2:
            case BTN_KIND_SLOT_0 + 3:
                pObj->PushButton(padNo0, nBtnKindId);
                m_nDeleteAsk = 0;
                m_nDeletePlayer = nBtnKindId - BTN_KIND_SLOT_0;
                m_nStep = STEP_DELETE_ASK;
                WindowYesNo_Button("CharaEntryMessage", "PMAKE_06_00", 0, CSave::GetPlayerData(m_nDeletePlayer)->m_Name,
                                   0, 1);
                break;
            case BTN_KIND_BACK:
                pObj->PushButton(padNo0, nBtnKindId);
                pObj->CloseMove();
                PlaterSelect_LineMessage(0);
                SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_PLUS_MINUS_SHOW, (void *)2);
                SendObjEventTrigger(OBJ_TOP, EVENT_TOP_UNK_07, 0);
                m_nStep = STEP_MAIN;
                break;
            case BTN_KIND_NONE:
                break;
            default:
                SetNextMode(nMode, fWait);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x20) {
        pObj->PushButton(padNo0, BTN_KIND_BACK);
        pObj->CloseMove();
        PlaterSelect_LineMessage(0);
        SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_PLUS_MINUS_SHOW, (void *)2);
        SendObjEventTrigger(OBJ_TOP, EVENT_TOP_UNK_07, 0);
        m_nStep = STEP_MAIN;
    } else {
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
    }
}

void msw::CSelectMenu::PlaterSelect_Out()
{
    if (m_nNextMode == MODE_TOP) {
        Scratch(OBJ_TITLE);
    }
    Scratch(OBJ_SAVE_SELECT);
    Scratch(OBJ_TOP_MENU);
    CGamePadCheck::GetInstance()->EnableBatteryCheck(0);
}

void msw::CSelectMenu::PlaterSelect_LineMessage(long)
{
    static const long padNo0 = 0;

    // The help line names the player under the cursor.
    long nBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
    if ((unsigned long)(nBtnKindId - BTN_KIND_SLOT_0) <= 3) {
        SAVE_DATA_PLAYER *pData = CSave::GetPlayerData(nBtnKindId - BTN_KIND_SLOT_0);
        if (pData->IsValid()) {
            wchar_t text[0x100];
            swprintf(text, 0xFF, GetMessageLabelString("TopMenuMessage", "P_SEL_03_00"), pData);
            SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, text);
        } else {
            SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE,
                                (void *)GetMessageLabelString("TopMenuMessage", "P_SEL_03_01"));
        }
    } else {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("TopMenuMessage", "P_SEL_03_02"));
    }
}

void msw::CSelectMenu::PlaterSelect_PlusMinus(long nPad, long nBtnKindId, long nStep)
{
    long nMode = m_nMode;
    SelectMenu::CSaveSelect *pObj = (SelectMenu::CSaveSelect *)FindObj(OBJ_SAVE_SELECT);

    // The players there are, the first of them, and whether the cursor
    // is on one.
    long nNum = 0;
    long nFirst = BTN_KIND_NONE;
    int bOnPlayer = 0;
    for (long i = 0; i < 4; i++) {
        if (CSave::GetPlayerData(i)->IsValid()) {
            if (nFirst == BTN_KIND_NONE) {
                nFirst = BTN_KIND_SLOT_0 + i;
            }
            if (nBtnKindId == BTN_KIND_SLOT_0 + i) {
                bOnPlayer = 1;
            }
            nNum++;
        }
    }
    if (nNum == 0) {
        return;
    }

    m_nPlaterNextStep = nStep;
    if (nStep == STEP_CHANGE) {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("TopMenuMessage", "P_SET_A_01"));
        pObj->OpenMove(0);
    } else {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("TopMenuMessage", "P_SET_B_01"));
        pObj->OpenMove(1);
    }
    SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_PLUS_MINUS_HIDE, 0);

    if (!bOnPlayer) {
        // The cursor goes to the first player once the slots have moved.
        if (m_aPadSelect[nPad].m_Unk00 == 1) {
            pObj->m_nPushButtonId[nPad] = nFirst;
        }
        pObj->SetCursorMarkerPosition(nPad, nFirst);
        m_nCurrentBtnKindId[nPad][nMode] = nFirst;
        m_fRandomMoveTime[nPad] = 5.0f;
        m_nStep = STEP_MOVE_WAIT;
    } else {
        m_nStep = m_nPlaterNextStep;
    }
}

void msw::CSelectMenu::PlaterSelect_BackTitle()
{
    static const long padNo0 = 0;

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_SAVE_SELECT);
    ClearMenuHistory();
    GoBackToMode(padNo0, 1, pObj, 1);
}

// The names in this namespace are guesses. Defined here because the
// strings follow the CSelectMenu steps' constants in .rodata.
namespace {

    // The animations of the screen, by index into the names the button
    // file lists (the names are not in the module).
    enum {
        ANIM_SLOT_IDLE = 0,
        ANIM_SLOT_FILLED = 5,
        ANIM_SLOT_EMPTY = 6,
        ANIM_SLOT_OFF = 7,
        ANIM_WIN_IN = 8,
        ANIM_WIN_OUT_BACK = 9,
        ANIM_WIN_IN_2 = 0xA,
        ANIM_WIN_OUT = 0xC,
        ANIM_NEW = 0xE,
        ANIM_NEW_IN = 0xF,
        ANIM_NEW_LOOP = 0x10,
        ANIM_NEW_OUT = 0x11,
        ANIM_SLOT_UNK_12 = 0x12,
        ANIM_SLOT_ON = 0x14,
        ANIM_MOVE_END = 0x15,
        ANIM_MOVE = 0x16,
        ANIM_MOVE_IN = 0x17,
        ANIM_MOVE_SIDE = 0x18
    };

    enum {
        PANE_WIN = 0,
        PANE_MOVE_CURSOR = 9,
        PANE_HELP_TEXT = 0x2E,
        PANE_ROOT = 0x2F
    };

    const char *const s_LayoutName = "me_save.brlyt";
    const char *const s_ButtonFileName = "me_save.btn";

    // The panes of each slot (by slot): the slot itself, where its
    // contents hang, and the three versions of them (empty, a player
    // without and with a friend code).
    long s_SlotPane[4] = { 2, 4, 6, 8 };
    long s_SlotPosPane[4] = { 0x16, 0x17, 0x18, 0x19 };
    long s_NewPane[4] = { 0x12, 0x13, 0x14, 0x15 };
    long s_PlayerPane[4] = { 0xE, 0xF, 0x10, 0x11 };
    long s_PlayerCodePane[4] = { 0xA, 0xB, 0xC, 0xD };
    // The name text pane, with and without a friend code.
    long s_CodeNamePane[4] = { 0x1B, 0x1E, 0x21, 0x24 };
    long s_NamePane[4] = { 0x27, 0x29, 0x2B, 0x2D };
    // The names of the Mii picture panes made, and the panes they go in.
    const char *s_FaceName[4] = { "FACE01", "FACE02", "FACE03", "FACE04" };
    long s_CodeFacePane[4] = { 0x1A, 0x1D, 0x20, 0x23 };
    long s_FacePane[4] = { 0x26, 0x28, 0x2A, 0x2C };
    // The friend code text pane.
    long s_CodePane[4] = { 0x1C, 0x1F, 0x22, 0x25 };
    // Where the cup marks hang, and the cup marks (by slot, then cup).
    long s_CupPosPane[4] = { 0x30, 0x35, 0x3A, 0x3F };
    long s_CupPane[4][4] = {
        { 0x31, 0x32, 0x33, 0x34 },
        { 0x36, 0x37, 0x38, 0x39 },
        { 0x3B, 0x3C, 0x3D, 0x3E },
        { 0x40, 0x41, 0x42, 0x43 },
    };
}

msw::SelectMenu::CSaveSelect::CSaveSelect(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
    for (long i = 0; i < 4; i++) {
        m_pMiiRender[i] = 0;
        m_pMiiPane[i] = 0;
    }
}

msw::SelectMenu::CSaveSelect::~CSaveSelect()
{
    Remove();
}

void msw::SelectMenu::CSaveSelect::Main()
{
    if (m_pPaneAccessor == 0) {
        m_Unk30 = STATE_NONE;
    }

    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CTeam.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                if (GetSelectMenu()->m_nStep == STEP_DELETE_WRITE) {
                    // Coming back after a delete: the slots are still aside.
                    m_pPaneAccessor->SetPaneDisp(PANE_MOVE_CURSOR, 1);
                    m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_MOVE_IN, s_WinAnimSpeed[0], 0.0f, 1, 1);
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_MOVE_IN, s_WinAnimSpeed[0], 0.0f, 1, 0);
                    m_nMoveStep = 1;
                }
                m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_IN_2, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN_2, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                    if (GetSelectMenu()->m_nStep == STEP_DELETE_WRITE) {
                        for (long i = 0; i < 4; i++) {
                            if (!CSave::GetPlayerData(i)->IsValid()) {
                                CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_SLOT_0 + i);
                                if (pPart != 0) {
                                    pPart->m_nFlags40 |= 1;
                                    pPart->ResetButton();
                                    m_pPaneAccessor->PlayAnimation(s_SlotPane[i], ANIM_SLOT_OFF, 1.0f, 0.0f, 1, 1);
                                    m_pPaneAccessor->PlayAnimation(s_SlotPane[i], ANIM_SLOT_IDLE, 1.0f, 0.0f, 1, 1);
                                    m_pPaneAccessor->PlayAnimation(s_SlotPane[i], ANIM_SLOT_UNK_12, 1.0f, 0.0f, 1, 1);
                                }
                            }
                        }
                        m_nMoveStep = 2;
                    }
                    m_Unk30 = STATE_ACTIVE;
                }
            } else if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN_2)) {
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            // The cups each player has cleared.
            for (long i = 0; i < 4; i++) {
                if (CSave::GetPlayerData(i)->IsValid()) {
                    for (long nCup = 0; nCup < 4; nCup++) {
                        if (CTournament::IsCupClear(3, nCup, 0, i)) {
                            m_pPaneAccessor->SetPaneDisp(s_CupPane[i][nCup], 1);
                        } else {
                            m_pPaneAccessor->SetPaneDisp(s_CupPane[i][nCup], 0);
                        }
                    }
                }
            }
            break;
        case STATE_MOVE:
            if (MoveItem()) {
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd() &&
                GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, EVENT_CLOSE_CHECK, 0) != 0 &&
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_CLOSE_CHECK, 0) != 0) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                    m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                    m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_CLOSE, 0);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (IsBackFrameOut(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            } else if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT)) {
                m_Unk30 = STATE_NONE;
            }
            break;
        default:
            return;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

int msw::SelectMenu::CSaveSelect::MoveItem()
{
    switch (m_nMoveStep) {
        case 1:
            // Aside.
            m_pPaneAccessor->SetPaneDisp(PANE_MOVE_CURSOR, 1);
            m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_IN, 1.0f, 20.0f, 1, 0);
            m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_MOVE, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_MOVE, 1.0f, 0.0f, 1, 0);
            m_nMoveStep = 2;
            break;
        case 2:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_MOVE)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_MOVE_IN, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_MOVE_SIDE, 1.0f, 0.0f, 1, 0);
                // The empty slots cannot be chosen meanwhile.
                for (long i = 0; i < 4; i++) {
                    if (!CSave::GetPlayerData(i)->IsValid()) {
                        CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_SLOT_0 + i);
                        if (pPart != 0) {
                            pPart->m_nFlags40 |= 1;
                            pPart->ResetButton();
                            m_pPaneAccessor->PlayAnimation(s_SlotPane[i], ANIM_SLOT_OFF, 1.0f, 0.0f, 1, 1);
                            m_pPaneAccessor->PlayAnimation(s_SlotPane[i], ANIM_SLOT_IDLE, 1.0f, 0.0f, 1, 1);
                            m_pPaneAccessor->PlayAnimation(s_SlotPane[i], ANIM_SLOT_UNK_12, 1.0f, 0.0f, 1, 1);
                        }
                    }
                }
                return 1;
            }
            break;
        case 3:
            // Back.
            m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_IN, 1.0f, 20.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_MOVE, -1.0f, 8.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_MOVE, -1.0f, 8.0f, 1, 1);
            m_nMoveStep = 4;
            break;
        case 4:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_MOVE)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_MOVE_END, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->SetPaneDisp(PANE_MOVE_CURSOR, 0);
                for (long i = 0; i < 4; i++) {
                    if (!CSave::GetPlayerData(i)->IsValid()) {
                        CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_SLOT_0 + i);
                        if (pPart != 0) {
                            pPart->m_nFlags40 &= ~1;
                            m_pPaneAccessor->PlayAnimation(s_SlotPane[i], ANIM_SLOT_EMPTY, 1.0f, 0.0f, 1, 1);
                            m_pPaneAccessor->PlayAnimation(s_SlotPane[i], ANIM_SLOT_ON, 1.0f, 0.0f, 1, 1);
                        }
                    }
                }
                m_nMoveStep = 0;
                return 1;
            }
            break;
    }
    return 0;
}

void msw::SelectMenu::CSaveSelect::Disp()
{
}

int msw::SelectMenu::CSaveSelect::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(pSelectMenu->GetLoadArchiveFileId(-1));
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            ArchiveBuildLoader(s_ButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(3));
            pPaneCtrl->BuildLayout(0, s_LayoutName);
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
                SetUpButton(pPart, i, 0);
                pPart->m_bUnk40_0004 = 1;
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            ClearPushButtonId();
            m_nMoveStep = 0;
        }
    }
    return 0;
}

void msw::SelectMenu::CSaveSelect::Remove()
{
    GetSelectMenu()->Scratch(OBJ_PLUS_MINUS);
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    for (long i = 0; i < 4; i++) {
        if (m_pMiiRender[i] != 0) {
            m_pMiiRender[i]->Delete();
            m_pMiiRender[i] = 0;
        }
        m_pMiiPane[i] = 0;
    }
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CSaveSelect::HitCheck(bcs::FVEC3 *pPos)
{
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    CMenuPad *pPad = CMenuPad::GetInstance();

    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, EVENT_HIT, pPos);
    }
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_HIT, pPos);
        if (nResult < 0) {
            nResult = 0;
        }
    }
    if (pPad->IsInputPointerMode(nPad)) {
        NewAnimation(nResult - BTN_KIND_SLOT_0);
    }
    return nResult;
}

long msw::SelectMenu::CSaveSelect::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CSaveSelect::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CSaveSelect::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CSaveSelect::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CSaveSelect::StickMoveCheck(long nPad, long nBtnKindId)
{
    long nNext;
    long nCurrent = nBtnKindId;
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nCurrent = BTN_KIND_SLOT_0;
    }

    unsigned long nInput = CMenuPad::s_Instance.GetInput(nPad);
    nNext = (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, nInput);

    // Nothing to move to in that direction: try the buttons beside the
    // next one (from the slots to the plus/minus buttons and back).
    if (nNext == nCurrent) {
        CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nCurrent);
        if ((nInput & 1) && pPart->m_nNextBtnKindId[1] != 0) {
            nNext = GetSideButton(m_pMenuPartList->FindObjectByCurrentBtnKindId(pPart->m_nNextBtnKindId[1]),
                                  3, 4);
        }
        if ((nInput & 4) && pPart->m_nNextBtnKindId[2] != 0) {
            nNext = GetSideButton(m_pMenuPartList->FindObjectByCurrentBtnKindId(pPart->m_nNextBtnKindId[2]),
                                  3, 4);
        }
        if ((nInput & 8) && pPart->m_nNextBtnKindId[3] != 0) {
            nNext = GetSideButton(m_pMenuPartList->FindObjectByCurrentBtnKindId(pPart->m_nNextBtnKindId[3]),
                                  1, 2);
        }
        if ((nInput & 2) && pPart->m_nNextBtnKindId[4] != 0) {
            nNext = GetSideButton(m_pMenuPartList->FindObjectByCurrentBtnKindId(pPart->m_nNextBtnKindId[4]),
                                  1, 2);
        }
    }

    if (!CMenuPad::s_Instance.IsInputPointerMode(nPad)) {
        NewAnimation(nNext - BTN_KIND_SLOT_0);
    }
    return nNext;
}

long msw::SelectMenu::CSaveSelect::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_SLOT_0;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CSaveSelect::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CSaveSelect::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    long nEvent = nPad + EVENT_PUSH;
    GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, nEvent, (void *)nBtnKindId);
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, nEvent, (void *)nBtnKindId);
    return 0;
}

void msw::SelectMenu::CSaveSelect::SetUpSaveData()
{
    CSelectMenu *pSelectMenu = GetSelectMenu();
    bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
    nw4r::math::VEC3 trans(0.0f, 0.0f, 0.0f);

    m_pPaneAccessor->SetPaneDisp(PANE_MOVE_CURSOR, 0);
    for (long i = 0; i < 4; i++) {
        m_pPaneAccessor->SetPaneDisp(s_NewPane[i], 0);
        m_pPaneAccessor->SetPaneDisp(s_PlayerPane[i], 0);
        m_pPaneAccessor->SetPaneDisp(s_PlayerCodePane[i], 0);
        for (long nCup = 0; nCup < 4; nCup++) {
            m_pPaneAccessor->SetPaneDisp(s_CupPane[i][nCup], 0);
        }
        m_nNewAnim[i] = 0xFF;
    }

    int bPlayer = 0;
    for (long i = 0; i < 4; i++) {
        SAVE_DATA_PLAYER *pData = CSave::GetPlayerData(i);
        nw4r::lyt::Pane *pPosPane;
        nw4r::lyt::Pane *pPane;
        if (pData->IsValid()) {
            if (pData->m_nFriendCode != 0) {
                // A player with a friend code.
                pPosPane = m_pPaneAccessor->GetPane(s_SlotPosPane[i]);
                pPane = m_pPaneAccessor->GetPane(s_PlayerCodePane[i]);
                m_pPaneAccessor->SetPaneDisp(s_PlayerCodePane[i], 1);
                m_pPaneAccessor->PlayAnimation(s_SlotPane[i], ANIM_SLOT_FILLED, 1.0f, 0.0f, 1, 0);
                SetTextPaneString(s_CodeNamePane[i], pData->m_Name);

                wchar_t text[0x10];
                nw4r::lyt::Pane *pFacePane = m_pPaneAccessor->GetPane(s_CodeFacePane[i]);
                nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(s_CodeFacePane[i])->GetPane()->GetSize();
                RFLStoreData storeData;
                SAVE_DATA_PLAYER *pMii = CSave::GetPlayerData(i);
                bcs::CMiiMan::GetStoreData(storeData, pMii->m_MiiSource, pMii->m_MiiCreateId);
                if (m_pMiiRender[i] != 0) {
                    m_pMiiRender[i]->Delete();
                }
                m_pMiiRender[i] = new CMiiSelectRender(0, storeData, size.width, size.height);
                GXTexObj texObj;
                GXInitTexObj(&texObj, m_pMiiRender[i]->GetTexImg(), (long)size.width, (long)size.height, GX_TF_RGB5A3,
                             GX_CLAMP, GX_CLAMP, 0);
                GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
                if (m_pMiiPane[i] == 0) {
                    m_pMiiPane[i] = menu::CreateMiiPicturePane(&texObj, pLytObj, pFacePane, s_FaceName[i]);
                } else {
                    bcs::G2D::ReplacePanePicture(m_pMiiPane[i], &texObj, 0);
                }

                // The friend code, as 0000-0000-0000.
                unsigned long long nCode = pData->m_nFriendCode;
                swprintf(text, 0x10, L"%04d-%04d-%04d", (unsigned long)(nCode / 10000 / 10000 % 10000),
                         (unsigned long)(nCode / 10000 % 10000), (unsigned long)(nCode % 10000));
                SetTextPaneString(s_CodePane[i], text);
            } else {
                // A player without one.
                pPosPane = m_pPaneAccessor->GetPane(s_SlotPosPane[i]);
                pPane = m_pPaneAccessor->GetPane(s_PlayerPane[i]);
                m_pPaneAccessor->SetPaneDisp(s_PlayerPane[i], 1);
                m_pPaneAccessor->PlayAnimation(s_SlotPane[i], ANIM_SLOT_FILLED, 1.0f, 0.0f, 1, 0);
                SetTextPaneString(s_NamePane[i], pData->m_Name);

                nw4r::lyt::Pane *pFacePane = m_pPaneAccessor->GetPane(s_FacePane[i]);
                nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(s_FacePane[i])->GetPane()->GetSize();
                RFLStoreData storeData;
                SAVE_DATA_PLAYER *pMii = CSave::GetPlayerData(i);
                bcs::CMiiMan::GetStoreData(storeData, pMii->m_MiiSource, pMii->m_MiiCreateId);
                if (m_pMiiRender[i] != 0) {
                    m_pMiiRender[i]->Delete();
                }
                m_pMiiRender[i] = new CMiiSelectRender(0, storeData, size.width, size.height);
                GXTexObj texObj;
                GXInitTexObj(&texObj, m_pMiiRender[i]->GetTexImg(), (long)size.width, (long)size.height, GX_TF_RGB5A3,
                             GX_CLAMP, GX_CLAMP, 0);
                GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
                if (m_pMiiPane[i] == 0) {
                    m_pMiiPane[i] = menu::CreateMiiPicturePane(&texObj, pLytObj, pFacePane, s_FaceName[i]);
                } else {
                    bcs::G2D::ReplacePanePicture(m_pMiiPane[i], &texObj, 0);
                }
            }

            // The cups the player has cleared.
            nw4r::lyt::Pane *pCupPane = m_pPaneAccessor->GetPane(s_CupPosPane[i]);
            bcs::G2D::SetPaneParent(pCupPane, pPosPane, 1, 0);
            pCupPane->SetTranslate(trans);
            for (long nCup = 0; nCup < 4; nCup++) {
                if (CTournament::IsCupClear(3, nCup, 0, i)) {
                    m_pPaneAccessor->SetPaneDisp(s_CupPane[i][nCup], 1);
                } else {
                    m_pPaneAccessor->SetPaneDisp(s_CupPane[i][nCup], 0);
                }
            }
            bPlayer = 1;
        } else {
            // An empty slot: "new player".
            pPosPane = m_pPaneAccessor->GetPane(s_SlotPosPane[i]);
            pPane = m_pPaneAccessor->GetPane(s_NewPane[i]);
            m_pPaneAccessor->SetPaneDisp(s_NewPane[i], 1);
            m_pPaneAccessor->PlayAnimation(s_SlotPane[i], ANIM_SLOT_EMPTY, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->PlayAnimation(s_NewPane[i], ANIM_NEW, 1.0f, 0.0f, 1, 0);
        }
        bcs::G2D::SetPaneParent(pPane, pPosPane, 1, 0);
        pPane->SetTranslate(trans);
        m_pPaneAccessor->PlayAnimation(s_SlotPane[i], ANIM_SLOT_UNK_12, 0.0f, 0.0f, 1, 0);
    }
    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_MOVE_END, 1.0f, 0.0f, 1, 0);

    // The plus/minus buttons (change and delete) only with a player.
    SelectMenu::CPlusMinus *pPlusMinus = (SelectMenu::CPlusMinus *)pSelectMenu->FindObj(OBJ_PLUS_MINUS);
    if (bPlayer) {
        pSelectMenu->Build(OBJ_PLUS_MINUS);
        const wchar_t *pPlus = GetMessageLabelString("List_Plus_Minus_Button", "Plus_00");
        const wchar_t *pMinus = GetMessageLabelString("List_Plus_Minus_Button", "Minus_00");
        pPlusMinus->StartUp(2, pPlus, pMinus);
    } else {
        GetSelectMenu()->Scratch(OBJ_PLUS_MINUS);
    }
}

void msw::SelectMenu::CSaveSelect::NewAnimation(long nPlayer)
{
    long nSelect = nPlayer;
    if (nPlayer < 0) {
        nSelect = -1;
    }

    for (long i = 0; i < 4; i++) {
        if (CSave::GetPlayerData(i)->IsValid()) {
            continue;
        }
        if (nSelect == i && m_nNewAnim[i] == 0xFF) {
            m_nNewAnim[i] = 0;
        }
        switch (m_nNewAnim[i]) {
            case 0:
                m_pPaneAccessor->PlayAnimation(s_NewPane[i], ANIM_NEW_IN, 1.0f, 0.0f, 1, 1);
                m_nNewAnim[i] = 1;
                break;
            case 1:
                if (m_pPaneAccessor->IsAnimEnd(s_NewPane[i], ANIM_NEW_IN)) {
                    m_pPaneAccessor->PlayAnimation(s_NewPane[i], ANIM_NEW_LOOP, 1.0f, 0.0f, 1, 1);
                    m_nNewAnim[i] = 2;
                }
                break;
            case 2:
                if (nSelect != i) {
                    m_nNewAnim[i] = 3;
                }
                break;
            case 3:
                m_pPaneAccessor->PlayAnimation(s_NewPane[i], ANIM_NEW_OUT, 1.0f, 0.0f, 1, 1);
                m_nNewAnim[i] = 4;
                break;
            case 4:
                if (m_pPaneAccessor->IsAnimEnd(s_NewPane[i], ANIM_NEW_OUT)) {
                    m_pPaneAccessor->PlayAnimation(s_NewPane[i], ANIM_NEW, 1.0f, 0.0f, 1, 1);
                    m_nNewAnim[i] = 0xFF;
                }
                break;
        }
    }
}

// Inlined into Main() although defined after it; the out-of-line copy is
// not called and the link drops it. Defined down here because its local
// static follows Main()'s constants in .rodata.
void msw::SelectMenu::CSaveSelect::CheckButtonAnim()
{
    static const int backEvent = EVENT_BACK;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, backEvent, m_nPushButtonId);
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, backEvent, m_nPushButtonId);
}

// Inlined into its callers, as CheckButtonAnim() into Main().
void msw::SelectMenu::CSaveSelect::SetTextPaneString(long nPane, const wchar_t *pText)
{
    static const bool recursive = true;

    if (pText != 0) {
        CMenuPaneAccessor *pAccessor = m_pPaneAccessor;
        nw4r::lyt::Pane *pPane =
            (unsigned long)nPane < pAccessor->m_nNum ? pAccessor->GetPane(pAccessor->GetPaneName(nPane), recursive, 0) : 0;
        if (pPane != 0) {
            m_pPaneAccessor->SetTextBoxString(nPane, pText, wcslen(pText));
        }
    }
}

void msw::SelectMenu::CSaveSelect::OpenMove(int bDelete)
{
    if (m_nMoveStep == 0) {
        m_nMoveStep = 1;
        m_Unk30 = STATE_MOVE;
        SetTextPaneString(PANE_HELP_TEXT,
                          GetMessageLabelString("TopMenuMessage", bDelete ? "P_SET_B_00" : "P_SET_A_00"));
    }
}

// Inlined into StickMoveCheck(), as CheckButtonAnim() into Main().
long msw::SelectMenu::CSaveSelect::GetSideButton(CMenuPartBase *pPart, long nNext0, long nNext1)
{
    unsigned short nId = pPart->m_nNextBtnKindId[nNext0];
    if (nId != 0) {
        CMenuPartBase *pSide = m_pMenuPartList->FindObjectByCurrentBtnKindId(nId);
        if (!(pSide->m_nFlags40 & 1)) {
            return pSide->m_nNextBtnKindId[0];
        }
    }
    nId = pPart->m_nNextBtnKindId[nNext1];
    if (nId != 0) {
        CMenuPartBase *pSide = m_pMenuPartList->FindObjectByCurrentBtnKindId(nId);
        if (!(pSide->m_nFlags40 & 1)) {
            return pSide->m_nNextBtnKindId[0];
        }
    }
    return 0;
}
