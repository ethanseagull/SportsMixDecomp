#include "bcs/CLoadMan.hpp"
#include "bcs/CMiiMan.hpp"
#include "bcs/dwc/CDwcFriendList.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Util.hpp"
#include "bcs/CWifiPadBase.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/G2D/CLytResource.hpp"
#include "bcs/G2D/CMenuMdlScnGroup.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CChrMenuCtrl.hpp"
#include "game/msw/CDepthOfFieldValue.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CMainStep.hpp"
#include "game/msw/CMenu.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/CNowLoading.hpp"
#include "game/msw/CStageManMenu.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitMenuWinBtn.hpp"
#include "game/msw/CDataWork.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CMenuOption.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/CSelectMenu.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/SelectMenu/CBackIcon.hpp"
#include "game/msw/SelectMenu/CBackground.hpp"
#include "game/msw/SelectMenu/CBgChr.hpp"
#include "game/msw/SelectMenu/CBgDisplay.hpp"
#include "game/msw/SelectMenu/CButton0.hpp"
#include "game/msw/SelectMenu/CButton1.hpp"
#include "game/msw/SelectMenu/CButton1Code.hpp"
#include "game/msw/SelectMenu/CButton2.hpp"
#include "game/msw/SelectMenu/CButtonA.hpp"
#include "game/msw/SelectMenu/CButtonACode.hpp"
#include "game/msw/SelectMenu/CCharSel.hpp"
#include "game/msw/SelectMenu/CCharaMdl.hpp"
#include "game/msw/SelectMenu/CCharaLevel.hpp"
#include "game/msw/SelectMenu/CCharaLevelStar.hpp"
#include "game/msw/SelectMenu/CCharaOk.hpp"
#include "game/msw/SelectMenu/CCharaStatus.hpp"
#include "game/msw/SelectMenu/CChrCaption.hpp"
#include "game/msw/SelectMenu/CChrSign.hpp"
#include "game/msw/SelectMenu/CCourt.hpp"
#include "game/msw/SelectMenu/CCourtChr.hpp"
#include "game/msw/SelectMenu/CCourtTab.hpp"
#include "game/msw/SelectMenu/CCupChr.hpp"
#include "game/msw/SelectMenu/CCupSelect.hpp"
#include "game/msw/SelectMenu/CCupSelectTab.hpp"
#include "game/msw/SelectMenu/CEntry.hpp"
#include "game/msw/SelectMenu/CEntryExplain.hpp"
#include "game/msw/SelectMenu/CFade.hpp"
#include "game/msw/SelectMenu/CFriendCode.hpp"
#include "game/msw/SelectMenu/CFriendInfo.hpp"
#include "game/msw/SelectMenu/CFriendList.hpp"
#include "game/msw/SelectMenu/CGameSelect.hpp"
#include "game/msw/SelectMenu/CHowTo.hpp"
#include "game/msw/SelectMenu/CInitGuide.hpp"
#include "game/msw/SelectMenu/CLevelMessage.hpp"
#include "game/msw/SelectMenu/CMiiSel.hpp"
#include "game/msw/SelectMenu/CMode.hpp"
#include "game/msw/SelectMenu/CPartyHelpPage.hpp"
#include "game/msw/SelectMenu/CPartyInfo.hpp"
#include "game/msw/SelectMenu/CPartyMEHelpBaseDodge.hpp"
#include "game/msw/SelectMenu/CPartyMEHelpBossPackun.hpp"
#include "game/msw/SelectMenu/CPartyMEHelpCrushSkate.hpp"
#include "game/msw/SelectMenu/CPartyMEHelpRhythmReceive.hpp"
#include "game/msw/SelectMenu/CPartySelect.hpp"
#include "game/msw/SelectMenu/CPlayMode.hpp"
#include "game/msw/SelectMenu/CPlayerInfo.hpp"
#include "game/msw/SelectMenu/CPlayerSelect.hpp"
#include "game/msw/SelectMenu/CPlusMinus.hpp"
#include "game/msw/SelectMenu/CRuleInfo.hpp"
#include "game/msw/SelectMenu/CRuleSel.hpp"
#include "game/msw/SelectMenu/CSaveSelect.hpp"
#include "game/msw/SelectMenu/CScoreBook.hpp"
#include "game/msw/SelectMenu/CScoreTop.hpp"
#include "game/msw/SelectMenu/CTeam.hpp"
#include "game/msw/SelectMenu/CTitle.hpp"
#include "game/msw/SelectMenu/CTopMenu.hpp"
#include "game/msw/SelectMenu/CTrofy.hpp"
#include "game/msw/SelectMenu/CWiFiFrindVS.hpp"
#include "game/msw/SelectMenu/CWiFiGame.hpp"
#include "game/msw/SelectMenu/CWiFiTopA.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/CMessage.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "game/msw/menu/CSndBGM.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/CStateSave.hpp"
#include "game/msw/menu/CVibEff.hpp"
#include "FastCast.hpp"
#include "stdio.h"
#include "string.h"
#include "wchar.h"

// msw::CSelectMenu (MenuF): the task behind every select menu screen. This
// file holds its construction, set-up and main loop, the state machine
// (bcs::CFsm<CSelectMenu>, one state per screen), the menu history, the
// character and Mii choices, the archives the screens load, the message
// windows and the small screens (end, time over, two test screens). The
// screens' own steps are in the other MenuF files. The game's file name is
// not known; the unit was guessed as CGamePadCheck.cpp from a few
// CGamePadCheck inlines it emits. Names in this file are guesses unless a
// symbol gives them.

namespace msw {

    // A menu object of class T with id nObjId; CSelectMenu::CreateObj
    // makes every screen object through a table of these.
    template <class T>
    CMenuLinkBase *newMenuClass(long nObjId)
    {
        return new T(nObjId);
    }

    // The part of the player's set-up data (CSave::GetPlayerSetUpData)
    // the cup and stage helpers read. Size 0xC per sport, then the party
    // games and the tournament records. Names guessed.
    struct SetUpCupInfo {
        unsigned char m_nCupBits;         // 0x00, the cups that are open
        unsigned char m_nStageBits[4];    // 0x01, per cup: the stages open
        unsigned char m_nStageNewBits[4]; // 0x05, per cup: the stages new
        unsigned char m_Unk09[3];         // 0x09
    };

    struct SetUpData {
        unsigned char m_Unk000[0x500];
        // 0x500 to 0x52F: the four sports (basketball, volleyball,
        // dodgeball, hockey: CSelectMenu::m_nGameKind 0 to 3).
        SetUpCupInfo m_Basket;            // 0x500
        SetUpCupInfo m_Volley;            // 0x50C
        SetUpCupInfo m_Dodge;             // 0x518
        SetUpCupInfo m_Hockey;            // 0x524
        // 0x530 to 0x55F: the party games, CSelectMenu::m_Unk9C 3, 1, 7
        // and 5.
        SetUpCupInfo m_Party3;            // 0x530
        SetUpCupInfo m_Party1;            // 0x53C
        SetUpCupInfo m_Party7;            // 0x548
        SetUpCupInfo m_Party5;            // 0x554
        // 0x560 to 0x58F: the four sports again (the mode 1 records; game
        // kind 5 shares the first).
        SetUpCupInfo m_Mode1Basket;       // 0x560
        SetUpCupInfo m_Mode1Volley;       // 0x56C
        SetUpCupInfo m_Mode1Dodge;        // 0x578
        SetUpCupInfo m_Mode1Hockey;       // 0x584
    };

    namespace SelectMenu {
        // (no header) Menu object 0x10 of the character select screen.
        // Nothing in the game defines it any more: the call to
        // PushCancelButton below is one of the module's three imports that
        // resolve nowhere (the loader points them at _unresolved).
        class CCharaSelect : public CSelectMenuLayoutBase {
            public:
                void PushCancelButton(long nPad, long nBtnKindId);
        };
    }

    // (declared here: no other file uses it) The cup record of a mode.
    SetUpCupInfo *_GetStageInfo(long nMode, long nGame, long nParty);

    static inline SetUpData *GetSetUpData()
    {
        return (SetUpData *)CSave::GetPlayerSetUpData(-1);
    }

    // (no symbol of its own; the name is that of its static local) The
    // game parameter of stage nStage of cup nCup of party game nParty.
    // The table is looked up on the first call only and kept, whatever
    // nParty later calls pass.
    inline long _GetPartyGameStageParam(long nParty, long nCup, long nStage)
    {
        static const PartyStageNoInfo *param = _GetPartyStageNoInfo(nParty);
        return param->m_nRecord[nCup][nStage];
    }
}

namespace {
    // The stages of each sport's cups: [sport][cup][stage], the ids
    // _GetStageNoInfo returns rows of (0x10: not playable).
    const unsigned char s_StageNoInfo[4][4][3] = {
        { { 0x01, 0x02, 0x05 }, { 0x06, 0x11, 0x07 }, { 0x0A, 0x0B, 0x10 }, { 0x03, 0x09, 0x0F } },
        { { 0x01, 0x02, 0x03 }, { 0x05, 0x06, 0x11 }, { 0x0A, 0x0B, 0x10 }, { 0x09, 0x0C, 0x0F } },
        { { 0x01, 0x02, 0x03 }, { 0x05, 0x04, 0x07 }, { 0x09, 0x0B, 0x10 }, { 0x11, 0x0C, 0x0F } },
        { { 0x01, 0x04, 0x03 }, { 0x11, 0x09, 0x07 }, { 0x0A, 0x0C, 0x10 }, { 0x02, 0x0F, 0x0B } }
    };

    // The archives of the screens (CSelectMenu::s_FileInputTable is
    // built from these by InitLoad, with the language folder put in).
    const bcs::CLoadMan::FILE_INPUT s_FileInputInit[msw::CSelectMenu::ARCHIVE_NUM] = {
        { "%s/select/main.bin", 0, 2, 4, 1, -32, 0 },
        { "%s/select/selMain.bin", 0, 2, 4, 1, -32, 0 },
        { "%s/select/selTitle.bin", 1, 2, 4, 1, -32, 0 },
        { "%s/select/selEnt.bin", 1, 2, 4, 1, -32, 0 },
        { "%s/select/selCha.bin", 1, 2, 4, 1, -32, 0 },
        { "%s/select/selBgSe.bin", 1, 2, 4, 1, -32, 0 },
        { "%s/select/selMult.bin", 1, 2, 4, 1, -32, 0 },
        { "%s/select/selOpt.bin", 1, 2, 4, 1, -32, 0 },
        { "%s/select/selMii.bin", 1, 2, 4, 1, -32, 0 },
        { "%s/select/selExNo.bin", 1, 2, 4, 1, -32, 0 },
        { "%s/select/selFrCo.bin", 1, 2, 4, 1, -32, 0 },
        { "%s/select/selScor.bin", 1, 2, 4, 1, -32, 0 },
        { "%s/select/selWiFi.bin", 1, 2, 4, 1, -32, 0 },
        { "%s/select/selRule.bin", 1, 2, 4, 1, -32, 0 },
    };
}

// The folder the archives are opened under.
static const char *s_pArchiveRootFolder = "arc";

namespace {
    // The archive (s_FileInputTable entry) each screen needs.
    const unsigned char s_LoadArchiveFileId[msw::CSelectMenu::STATE_NUM + 1] = {
        0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x0B, 0x0B, 0x03, 0x01, 0x01, 0x06, 0x06, 0x04, 0x04,
        0x05, 0x08, 0x01, 0x09, 0x01, 0x01, 0x0C, 0x0C, 0x0C, 0x08, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x04,
        0x01, 0x0C, 0x0C, 0x0A, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x07, 0x01, 0x0D, 0x01, 0x01,
        0x01
    };

    // The stages of the four party games with cups (_GetPartyStageNoInfo).
    const msw::PartyStageNoInfo s_PartyStageNoInfo[4] = {
        { { { 0x0, 0x3, 0x6, 0x0 }, { 0x1, 0x4, 0x7, 0x0 }, { 0x0, 0x0, 0x0, 0x0 }, { 0x0, 0x0, 0x0, 0x0 } },
          { { 0xA, 0xB, 0xC, 0x0 }, { 0x0, 0x0, 0x0, 0x0 } },
          { { 0, 0, 0, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
          { { 0x46, 0x47, 0x48, 0x0 }, { 0x46, 0x47, 0x48, 0x0 }, { 0x0, 0x0, 0x0, 0x0 }, { 0x0, 0x0, 0x0, 0x0 } },
          "COURT__HUNGRY_BOSSPACKUN_0001" },
        { { { 0x0, 0x1, 0x2, 0x9 }, { 0x3, 0x4, 0x5, 0xA }, { 0x6, 0x7, 0x8, 0xB }, { 0x0, 0x0, 0x0, 0x0 } },
          { { 0xD, 0xE, 0xF, 0x10 }, { 0x0, 0x0, 0x0, 0x0 } },
          { { 0, 1, 2, 9 }, { 3, 4, 5, 10 }, { 6, 7, 8, 11 }, { -1, -1, -1, -1 } },
          { { 0x28, 0x2A, 0x29, 0x28 }, { 0x28, 0x2A, 0x29, 0x29 }, { 0x28, 0x2A, 0x29, 0x2A }, { 0x0, 0x0, 0x0, 0x0 } },
          "COURT__RHYTHM_RECEIVE__0001" },
        { { { 0x0, 0x3, 0x6, 0x0 }, { 0x1, 0x4, 0x7, 0x0 }, { 0x0, 0x0, 0x0, 0x0 }, { 0x0, 0x0, 0x0, 0x0 } },
          { { 0xA, 0xB, 0xC, 0x0 }, { 0x0, 0x0, 0x0, 0x0 } },
          { { 0, 0, 0, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
          { { 0x37, 0x38, 0x39, 0x0 }, { 0x37, 0x38, 0x39, 0x0 }, { 0x0, 0x0, 0x0, 0x0 }, { 0x0, 0x0, 0x0, 0x0 } },
          "COURT__DODGE_OR_DIE_0001" },
        { { { 0x0, 0x6, 0x3, 0x0 }, { 0x1, 0x7, 0x4, 0x0 }, { 0x0, 0x0, 0x0, 0x0 }, { 0x0, 0x0, 0x0, 0x0 } },
          { { 0xA, 0xB, 0xC, 0x0 }, { 0x0, 0x0, 0x0, 0x0 } },
          { { 0, 0, 0, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
          { { 0x55, 0x57, 0x56, 0x0 }, { 0x55, 0x57, 0x56, 0x0 }, { 0x0, 0x0, 0x0, 0x0 }, { 0x0, 0x0, 0x0, 0x0 } },
          "COURT__CRUSH_SKATE_0001" },
    };

    // The character each character button gives back when the last
    // choice is cancelled (Chara_CancelLastDecideChara).
    struct CharaButton {
        unsigned short m_nBtnKindId;
        unsigned char m_nChara;
        unsigned char m_nColor;
        unsigned char m_nColorSub;
    };

    const CharaButton s_CharaButton[] = {
        { 0x38, 0x0, 0, 0 },
        { 0x39, 0x2, 0, 1 },
        { 0x3A, 0x5, 0, 0 },
        { 0x3B, 0x8, 0, 0 },
        { 0x3C, 0xE, 0, 0 },
        { 0x3D, 0x10, 0, 2 },
        { 0x3E, 0x1, 0, 0 },
        { 0x3F, 0x3, 0, 1 },
        { 0x40, 0x7, 0, 0 },
        { 0x41, 0xB, 0, 0 },
        { 0x42, 0xF, 0, 1 },
        { 0x43, 0xC, 0, 0 },
        { 0x44, 0x4, 0, 3 },
        { 0x45, 0x6, 0, 0 },
        { 0x46, 0xA, 0, 0 },
        { 0x47, 0x9, 0, 3 },
        { 0x48, 0x11, 0, 0 },
        { 0x49, 0xD, 0, 0 },
        { 0x4A, 0x13, 255, 0 },
    };

    // Who takes the six character slots in each play mode
    // (InitCharaSelectParam): per team and slot the pad entry, the player
    // kind and the value of CharaSelectParam::m_Unk08 (0xFF: no slot).
    struct CharaSlotSetUp {
        unsigned char m_nEntry[2][3];
        unsigned char m_nPlayerKind[2][3];
        unsigned char m_nUnk08[2][3];
        unsigned char m_Unk12[2]; // 0x12, always 0 (entries are 0x14 bytes)
    };

    const CharaSlotSetUp s_CharaSlotSetUp[] = {
        /* 0x131C */ { { { 0x0, 0x0, 0xFF }, { 0x0, 0x0, 0xFF } }, { { 0x4, 0x4, 0xFF }, { 0x4, 0x4, 0xFF } }, { { 0x4, 0x4, 0xFF }, { 0x4, 0x4, 0xFF } } },
        /* 0x1330 */ { { { 0x0, 0x0, 0x0 }, { 0x0, 0x0, 0x0 } }, { { 0x4, 0x4, 0x4 }, { 0x4, 0x4, 0x4 } }, { { 0x4, 0x4, 0x4 }, { 0x4, 0x4, 0x4 } } },
        /* 0x1344 */ { { { 0x0, 0x0, 0xFF }, { 0x0, 0x0, 0xFF } }, { { 0x0, 0x0, 0xFF }, { 0x4, 0x4, 0xFF } }, { { 0x0, 0x4, 0xFF }, { 0x4, 0x4, 0xFF } } },
        /* 0x1358 */ { { { 0x0, 0x0, 0x0 }, { 0x0, 0x0, 0x0 } }, { { 0x0, 0x0, 0x0 }, { 0x4, 0x4, 0x4 } }, { { 0x0, 0x4, 0x4 }, { 0x4, 0x4, 0x4 } } },
        /* 0x136C */ { { { 0x0, 0x1, 0xFF }, { 0x0, 0x0, 0xFF } }, { { 0x0, 0x1, 0xFF }, { 0x4, 0x4, 0xFF } }, { { 0x0, 0x1, 0xFF }, { 0x4, 0x4, 0xFF } } },
        /* 0x1380 */ { { { 0x0, 0x0, 0xFF }, { 0x1, 0x1, 0xFF } }, { { 0x0, 0x0, 0xFF }, { 0x1, 0x1, 0xFF } }, { { 0x0, 0x4, 0xFF }, { 0x1, 0x4, 0xFF } } },
        /* 0x1394 */ { { { 0x0, 0x0, 0x0 }, { 0x1, 0x1, 0x1 } }, { { 0x0, 0x0, 0x0 }, { 0x1, 0x1, 0x1 } }, { { 0x0, 0x4, 0x4 }, { 0x1, 0x4, 0x4 } } },
        /* 0x13A8 */ { { { 0x0, 0x1, 0x2 }, { 0x0, 0x0, 0x0 } }, { { 0x0, 0x1, 0x2 }, { 0x4, 0x4, 0x4 } }, { { 0x0, 0x1, 0x2 }, { 0x4, 0x4, 0x4 } } },
        /* 0x13BC */ { { { 0x0, 0x1, 0xFF }, { 0x2, 0x2, 0xFF } }, { { 0x0, 0x1, 0xFF }, { 0x2, 0x4, 0xFF } }, { { 0x0, 0x1, 0xFF }, { 0x2, 0x4, 0xFF } } },
        /* 0x13D0 */ { { { 0x0, 0x2, 0xFF }, { 0x1, 0x1, 0xFF } }, { { 0x0, 0x2, 0xFF }, { 0x1, 0x4, 0xFF } }, { { 0x0, 0x2, 0xFF }, { 0x1, 0x4, 0xFF } } },
        /* 0x13E4 */ { { { 0x0, 0x0, 0xFF }, { 0x1, 0x2, 0xFF } }, { { 0x0, 0x0, 0xFF }, { 0x1, 0x2, 0xFF } }, { { 0x0, 0x4, 0xFF }, { 0x1, 0x2, 0xFF } } },
        /* 0x13F8 */ { { { 0x0, 0x1, 0xFF }, { 0x2, 0x3, 0xFF } }, { { 0x0, 0x1, 0xFF }, { 0x2, 0x3, 0xFF } }, { { 0x0, 0x1, 0xFF }, { 0x2, 0x3, 0xFF } } },
        /* 0x140C */ { { { 0x0, 0x2, 0xFF }, { 0x1, 0x3, 0xFF } }, { { 0x0, 0x2, 0xFF }, { 0x1, 0x3, 0xFF } }, { { 0x0, 0x2, 0xFF }, { 0x1, 0x3, 0xFF } } },
        /* 0x1420 */ { { { 0x0, 0x3, 0xFF }, { 0x1, 0x2, 0xFF } }, { { 0x0, 0x3, 0xFF }, { 0x1, 0x2, 0xFF } }, { { 0x0, 0x3, 0xFF }, { 0x1, 0x2, 0xFF } } },
        /* 0x1434 */ { { { 0x0, 0x1, 0x2 }, { 0x3, 0x3, 0x3 } }, { { 0x0, 0x1, 0x2 }, { 0x3, 0x4, 0x4 } }, { { 0x0, 0x1, 0x2 }, { 0x3, 0x4, 0x4 } } },
        /* 0x1448 */ { { { 0x0, 0x1, 0x0 }, { 0x2, 0x3, 0x2 } }, { { 0x0, 0x1, 0x4 }, { 0x2, 0x3, 0x4 } }, { { 0x0, 0x1, 0x4 }, { 0x2, 0x3, 0x4 } } },
        /* 0x145C */ { { { 0x0, 0x1, 0x0 }, { 0x2, 0x2, 0x2 } }, { { 0x0, 0x1, 0x4 }, { 0x2, 0x4, 0x4 } }, { { 0x0, 0x1, 0x4 }, { 0x2, 0x4, 0x4 } } },
        /* 0x1470 */ { { { 0x0, 0x0, 0x0 }, { 0x1, 0x2, 0x1 } }, { { 0x0, 0x4, 0x4 }, { 0x1, 0x2, 0x4 } }, { { 0x0, 0x4, 0x4 }, { 0x1, 0x2, 0x4 } } },
        /* 0x1484 */ { { { 0x0, 0x1, 0x0 }, { 0x0, 0x0, 0x0 } }, { { 0x0, 0x1, 0x4 }, { 0x4, 0x4, 0x4 } }, { { 0x0, 0x1, 0x4 }, { 0x4, 0x4, 0x4 } } },
        /* 0x1498 */ { { { 0x0, 0x0, 0x0 }, { 0x1, 0x2, 0x3 } }, { { 0x0, 0x4, 0x4 }, { 0x1, 0x2, 0x3 } }, { { 0x0, 0x4, 0x4 }, { 0x1, 0x2, 0x3 } } },
        /* 0x14AC */ { { { 0x0, 0x2, 0x0 }, { 0x1, 0x1, 0x1 } }, { { 0x0, 0x2, 0x4 }, { 0x1, 0x4, 0x4 } }, { { 0x0, 0x2, 0x4 }, { 0x1, 0x4, 0x4 } } },
        /* 0x14C0 */ { { { 0x0, 0x1, 0x3 }, { 0x2, 0x2, 0x2 } }, { { 0x0, 0x1, 0x3 }, { 0x2, 0x4, 0x4 } }, { { 0x0, 0x1, 0x3 }, { 0x2, 0x4, 0x4 } } },
        /* 0x14D4 */ { { { 0x0, 0x2, 0x3 }, { 0x1, 0x1, 0x1 } }, { { 0x0, 0x2, 0x3 }, { 0x1, 0x4, 0x4 } }, { { 0x0, 0x2, 0x3 }, { 0x1, 0x4, 0x4 } } },
        /* 0x14E8 */ { { { 0x0, 0x0, 0xFF }, { 0x0, 0x0, 0xFF } }, { { 0x0, 0x4, 0xFF }, { 0x4, 0x4, 0xFF } }, { { 0x0, 0x4, 0xFF }, { 0x4, 0x4, 0xFF } } },
        /* 0x14FC */ { { { 0x0, 0x1, 0xFF }, { 0x0, 0x0, 0xFF } }, { { 0x0, 0x1, 0xFF }, { 0x4, 0x4, 0xFF } }, { { 0x0, 0x1, 0xFF }, { 0x4, 0x4, 0xFF } } },
        /* 0x1510 */ { { { 0x0, 0x1, 0xFF }, { 0x0, 0x2, 0xFF } }, { { 0x0, 0x1, 0xFF }, { 0x4, 0x2, 0xFF } }, { { 0x0, 0x1, 0xFF }, { 0x4, 0x2, 0xFF } } },
        /* 0x1524 */ { { { 0x0, 0x1, 0xFF }, { 0x3, 0x2, 0xFF } }, { { 0x0, 0x1, 0xFF }, { 0x3, 0x2, 0xFF } }, { { 0x0, 0x1, 0xFF }, { 0x3, 0x2, 0xFF } } },
        /* 0x1538 */ { { { 0xFF, 0xFF, 0xFF }, { 0xFF, 0xFF, 0xFF } }, { { 0xFF, 0xFF, 0xFF }, { 0xFF, 0xFF, 0xFF } }, { { 0xFF, 0xFF, 0xFF }, { 0xFF, 0xFF, 0xFF } } },
    };
}

// The camera animation file of the menu background.
static const char *s_pCameraFileName = "cam_data.brres";

namespace {
    // The camera animations of the menu background (variant 1 with the mix
    // sports open). game/menu/CSaveSelect.cpp has the same table.
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

    // The speed of each camera animation (GetCameraSpeed).
    const float s_CameraSpeed[CAMERA_ANIM_NUM] = {
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f,
        2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };

    // The order of the characters of each team in the game (SelectEnd_Out).
    const unsigned char s_TeamOrder[9][6] = {
        { 0, 3, 1, 2, 5, 4 }, { 0, 3, 1, 2, 5, 4 }, { 0, 3, 1, 2, 5, 4 },
        { 0, 6, 2, 4, 1, 5 }, { 0, 6, 2, 4, 1, 5 }, { 0, 3, 1, 2, 5, 4 },
        { 0, 3, 1, 2, 5, 4 }, { 0, 3, 1, 2, 5, 4 }, { 0, 3, 1, 2, 5, 4 }
    };

    // The menu music: its sequence and whether it is started at once
    // (else only loaded).
    const msw::CSelectMenu::MenuBGM s_MenuBGM[] = {
        { 0, 1 }, { 0xBB7, 0 }, { 0xBB8, 0 }, { 1, 1 }, { 0xBB9, 0 }, { 3, 1 }, { 1, 1 }
    };

    const long MENU_BGM_NUM = sizeof(s_MenuBGM) / sizeof(s_MenuBGM[0]);

    // The message number of each character's name in "List_Character",
    // by character and variant (SelectEnd_Out; -1: no such variant).
    long s_CharaNameMsg[20][4] = {
        { 0, -1, -1, -1 },
        { 1, -1, -1, -1 },
        { 2, 2, -1, -1 },
        { 3, 3, -1, -1 },
        { 4, 4, 4, 4 },
        { 5, -1, -1, -1 },
        { 6, -1, -1, -1 },
        { 7, -1, -1, -1 },
        { 8, -1, -1, -1 },
        { 9, 9, 9, 9 },
        { 10, -1, -1, -1 },
        { 11, -1, -1, -1 },
        { 12, -1, -1, -1 },
        { 13, -1, -1, -1 },
        { 14, 14, -1, -1 },
        { 15, 15, -1, -1 },
        { 16, 25, 26, -1 },
        { 17, 17, -1, -1 },
        { 18, -1, -1, -1 },
        { 19, -1, -1, -1 }
    };

    // The menu objects CSelectMenu::CreateObj can make, by id.
    msw::CMenuLinkBase *(*s_NewMenuClassTable[])(long) = {
        msw::newMenuClass<msw::SelectMenu::CBackground>, // 0x00
        msw::newMenuClass<msw::SelectMenu::CBackIcon>, // 0x01
        msw::newMenuClass<msw::SelectMenu::CPlusMinus>, // 0x02
        msw::newMenuClass<msw::SelectMenu::CInitGuide>, // 0x03
        msw::newMenuClass<msw::SelectMenu::CTitle>, // 0x04
        msw::newMenuClass<msw::SelectMenu::CSaveSelect>, // 0x05
        0, // 0x06
        0, // 0x07
        msw::newMenuClass<msw::SelectMenu::CMode>, // 0x08
        msw::newMenuClass<msw::SelectMenu::CEntry>, // 0x09
        msw::newMenuClass<msw::SelectMenu::CHowTo>, // 0x0A
        0, // 0x0B
        0, // 0x0C
        msw::newMenuClass<msw::SelectMenu::CPlayMode>, // 0x0D
        msw::newMenuClass<msw::SelectMenu::CTeam>, // 0x0E
        msw::newMenuClass<msw::SelectMenu::CGameSelect>, // 0x0F
        0, // 0x10
        0, // 0x11
        msw::newMenuClass<msw::SelectMenu::CCharaStatus>, // 0x12
        msw::newMenuClass<msw::SelectMenu::CCharaOk>, // 0x13
        msw::newMenuClass<msw::SelectMenu::CCharaLevel>, // 0x14
        msw::newMenuClass<msw::SelectMenu::CCharaLevelStar>, // 0x15
        0, // 0x16
        msw::newMenuClass<msw::SelectMenu::CMiiSel>, // 0x17
        0, // 0x18
        msw::newMenuClass<msw::SelectMenu::CPartySelect>, // 0x19
        msw::newMenuClass<msw::SelectMenu::CPartyInfo>, // 0x1A
        0, // 0x1B
        0, // 0x1C
        0, // 0x1D
        msw::newMenuClass<msw::SelectMenu::CFriendCode>, // 0x1E
        msw::newMenuClass<msw::SelectMenu::CPlayerSelect>, // 0x1F
        msw::newMenuClass<msw::SelectMenu::CPlayerInfo>, // 0x20
        msw::newMenuClass<msw::SelectMenu::CButton1>, // 0x21
        msw::newMenuClass<msw::SelectMenu::CButton2>, // 0x22
        msw::newMenuClass<msw::SelectMenu::CWiFiTopA>, // 0x23
        0, // 0x24
        msw::newMenuClass<msw::SelectMenu::CWiFiGame>, // 0x25
        0, // 0x26
        0, // 0x27
        msw::newMenuClass<msw::SelectMenu::CFriendList>, // 0x28
        msw::newMenuClass<msw::SelectMenu::CFriendInfo>, // 0x29
        msw::newMenuClass<msw::SelectMenu::CNoFriendInfo>, // 0x2A
        msw::newMenuClass<msw::SelectMenu::CWiFiFrindVS>, // 0x2B
        msw::newMenuClass<msw::SelectMenu::CButton1Code>, // 0x2C
        msw::newMenuClass<msw::SelectMenu::CButtonA>, // 0x2D
        msw::newMenuClass<msw::SelectMenu::CButtonACode>, // 0x2E
        msw::newMenuClass<msw::SelectMenu::CButtonACode2>, // 0x2F
        msw::newMenuClass<msw::SelectMenu::CButton0>, // 0x30
        msw::newMenuClass<msw::SelectMenu::CCourt>, // 0x31
        msw::newMenuClass<msw::SelectMenu::CCourtTab>, // 0x32
        msw::newMenuClass<msw::SelectMenu::CCupSelect>, // 0x33
        msw::newMenuClass<msw::SelectMenu::CCupSelectTab>, // 0x34
        msw::newMenuClass<msw::SelectMenu::CLevelMessage>, // 0x35
        msw::newMenuClass<msw::SelectMenu::CEntryExplain>, // 0x36
        0, // 0x37
        0, // 0x38
        0, // 0x39
        msw::newMenuClass<msw::SelectMenu::CBgChrTop>, // 0x3A
        msw::newMenuClass<msw::SelectMenu::CCourtChr>, // 0x3B
        msw::newMenuClass<msw::SelectMenu::CChrCaption>, // 0x3C
        msw::newMenuClass<msw::SelectMenu::CChrSign>, // 0x3D
        msw::newMenuClass<msw::SelectMenu::CBgDisplay>, // 0x3E
        msw::newMenuClass<msw::SelectMenu::CFade>, // 0x3F
        msw::newMenuClass<msw::SelectMenu::CCourtBall>, // 0x40
        msw::newMenuClass<msw::SelectMenu::CCupChr>, // 0x41
        msw::newMenuClass<msw::SelectMenu::CCourtChrBall>, // 0x42
        msw::newMenuClass<msw::SelectMenu::CScoreTop>, // 0x43
        msw::newMenuClass<msw::SelectMenu::CScoreBook>, // 0x44
        msw::newMenuClass<msw::SelectMenu::CCharSel>, // 0x45
        msw::newMenuClass<msw::SelectMenu::CTopMenu>, // 0x46
        msw::newMenuClass<msw::SelectMenu::CPartyMEHelpBossPackun>, // 0x47
        msw::newMenuClass<msw::SelectMenu::CPartyMEHelpRhythmReceive>, // 0x48
        msw::newMenuClass<msw::SelectMenu::CPartyMEHelpBaseDodge>, // 0x49
        msw::newMenuClass<msw::SelectMenu::CPartyMEHelpCrushSkate>, // 0x4A
        msw::newMenuClass<msw::SelectMenu::CPartyHelpPage>, // 0x4B
        msw::newMenuClass<msw::SelectMenu::CRuleSel>, // 0x4C
        msw::newMenuClass<msw::SelectMenu::CTrofy>, // 0x4D
        msw::newMenuClass<msw::SelectMenu::CRuleInfo>, // 0x4E
    };
}

bcs::CLoadMan::FILE_INPUT msw::CSelectMenu::s_FileInputTable[msw::CSelectMenu::ARCHIVE_NUM];

// The play mode of a party game for nPlayerNum players.
long msw::_GetPartyPlayType(long nPlayerNum)
{
    switch (nPlayerNum) {
        case 1:
            return 4;
        case 2:
            return 6;
        case 3:
            return 7;
        case 4:
            return 8;
    }
    return nPlayerNum;
}

float msw::_GetModeChangeWaitTime()
{
    return CSelectMenuLayoutBase::s_DefaultOutAnimOverTime;
}

long msw::_GetPlayModeFromButtonId(long nBtnKindId, long nPlayerNum)
{
    long aPlayMode[4][4] = {
        { 4, 0, -1, -1 },
        { 6, 0x12, 5, 1 },
        { -1, 9, 7, 0xF },
        { -1, -1, 8, 0xE }
    };

    switch (nBtnKindId) {
        case 0x32:
            return aPlayMode[nPlayerNum - 1][0];
        case 0x33:
            return aPlayMode[nPlayerNum - 1][1];
        case 0x34:
            return aPlayMode[nPlayerNum - 1][2];
        case 0x35:
            return aPlayMode[nPlayerNum - 1][3];
    }
    return -2;
}

// The number of bits set in the low four bits of nBits, less one.
long msw::_CupStageBitCount(unsigned char nBits)
{
    long nCount = 0;
    for (long i = 0; i < 4; i++) {
        if (nBits & 1) {
            nCount++;
        }
        nBits >>= 1;
    }
    return nCount - 1;
}

unsigned char msw::_GetCupCount(long nGame, unsigned char *pCount)
{
    SetUpData *pSetUp = GetSetUpData();
    switch (nGame) {
        case 0:
            *pCount = pSetUp->m_Basket.m_nCupBits;
            break;
        case 1:
            *pCount = pSetUp->m_Volley.m_nCupBits;
            break;
        case 2:
            *pCount = pSetUp->m_Dodge.m_nCupBits;
            break;
        case 3:
            *pCount = pSetUp->m_Hockey.m_nCupBits;
            break;
    }
    return _CupStageBitCount(*pCount);
}

unsigned char msw::_GetStageBit(long nGame, long nCup)
{
    SetUpData *pSetUp = GetSetUpData();
    unsigned char nBits = 0;
    switch (nGame) {
        case 0:
            nBits = pSetUp->m_Basket.m_nStageBits[nCup];
            break;
        case 1:
            nBits = pSetUp->m_Volley.m_nStageBits[nCup];
            break;
        case 2:
            nBits = pSetUp->m_Dodge.m_nStageBits[nCup];
            break;
        case 3:
            nBits = pSetUp->m_Hockey.m_nStageBits[nCup];
            break;
    }
    // The first stage of a cup is always open.
    if (nBits == 0) {
        nBits = 1;
    }
    return nBits;
}

long msw::_CheckStage(long nGame, long nCup, long nStage)
{
    unsigned char nBits = _GetStageBit(nGame, nCup);
    if (nBits & (1 << nStage)) {
        return 1;
    }
    return 0;
}

unsigned char msw::_GetStageNewBit(long nGame, long nCup)
{
    SetUpData *pSetUp = GetSetUpData();
    unsigned char nBits = 0;
    switch (nGame) {
        case 0:
            nBits = pSetUp->m_Basket.m_nStageNewBits[nCup];
            break;
        case 1:
            nBits = pSetUp->m_Volley.m_nStageNewBits[nCup];
            break;
        case 2:
            nBits = pSetUp->m_Dodge.m_nStageNewBits[nCup];
            break;
        case 3:
            nBits = pSetUp->m_Hockey.m_nStageNewBits[nCup];
            break;
    }
    return nBits;
}

const unsigned char *msw::_GetStageNoInfo(long nGame, long nCup)
{
    const unsigned char (*pTable)[4][3] = s_StageNoInfo;
    const unsigned char (*pInfo)[3] = 0;
    switch (nGame) {
        case 0:
            pInfo = pTable[0];
            break;
        case 1:
            pInfo = pTable[1];
            break;
        case 2:
            pInfo = pTable[2];
            break;
        case 3:
            pInfo = pTable[3];
            break;
    }
    return pInfo[nCup];
}

unsigned char msw::_GetPartyCupCount(long nParty, unsigned char *pCount)
{
    SetUpData *pSetUp = GetSetUpData();
    switch (nParty) {
        case 1:
            *pCount = pSetUp->m_Party1.m_nCupBits;
            break;
        case 3:
            *pCount = pSetUp->m_Party3.m_nCupBits;
            break;
        case 7:
            *pCount = pSetUp->m_Party7.m_nCupBits;
            break;
        case 5:
            *pCount = pSetUp->m_Party5.m_nCupBits;
            break;
    }
    return _CupStageBitCount(*pCount);
}

unsigned char msw::_GetPartyStageBit(long nParty, long nCup)
{
    SetUpData *pSetUp = GetSetUpData();
    unsigned char nBits = 0;
    switch (nParty) {
        case 1:
            nBits = pSetUp->m_Party1.m_nStageBits[nCup];
            break;
        case 3:
            nBits = pSetUp->m_Party3.m_nStageBits[nCup];
            break;
        case 7:
            nBits = pSetUp->m_Party7.m_nStageBits[nCup];
            break;
        case 5:
            nBits = pSetUp->m_Party5.m_nStageBits[nCup];
            break;
    }
    if (nBits == 0) {
        nBits = 1;
    }
    return nBits;
}

long msw::_CheckPartyStage(long nParty, long nCup, long nStage)
{
    unsigned char nBits = _GetPartyStageBit(nParty, nCup);
    if (nBits & (1 << nStage)) {
        return 1;
    }
    return 0;
}

unsigned char msw::_GetPartyStageNewBit(long nParty, long nCup)
{
    SetUpData *pSetUp = GetSetUpData();
    unsigned char nBits = 0;
    switch (nParty) {
        case 1:
            nBits = pSetUp->m_Party1.m_nStageNewBits[nCup];
            break;
        case 3:
            nBits = pSetUp->m_Party3.m_nStageNewBits[nCup];
            break;
        case 7:
            nBits = pSetUp->m_Party7.m_nStageNewBits[nCup];
            break;
        case 5:
            nBits = pSetUp->m_Party5.m_nStageNewBits[nCup];
            break;
    }
    return nBits;
}

const msw::PartyStageNoInfo *msw::_GetPartyStageNoInfo(long nParty)
{
    long nIndex;
    switch (nParty) {
        case 1:
            nIndex = 0;
            break;
        case 2:
            nIndex = 0;
            break;
        case 3:
            nIndex = 1;
            break;
        case 4:
            nIndex = 0;
            break;
        case 5:
            nIndex = 2;
            break;
        case 6:
            nIndex = 0;
            break;
        case 7:
            nIndex = 3;
            break;
        case 8:
            nIndex = 0;
            break;
        default:
            nIndex = 0;
            break;
    }
    return &s_PartyStageNoInfo[nIndex];
}

// The cup record of mode nMode: the party games' (mode 2, by party game
// nParty), the second set (mode 1) or the sports' (by game kind nGame).
msw::SetUpCupInfo *msw::_GetStageInfo(long nMode, long nGame, long nParty)
{
    SetUpData *pSetUp = GetSetUpData();
    SetUpCupInfo *pInfo = 0;
    if (nMode == 2) {
        switch (nParty) {
            case 3:
                pInfo = &pSetUp->m_Party3;
                break;
            case 1:
                pInfo = &pSetUp->m_Party1;
                break;
            case 7:
                pInfo = &pSetUp->m_Party7;
                break;
            case 5:
                pInfo = &pSetUp->m_Party5;
                break;
        }
    } else if (nMode == 1) {
        switch (nGame) {
            case 0:
                pInfo = &pSetUp->m_Mode1Basket;
                break;
            case 1:
                pInfo = &pSetUp->m_Mode1Volley;
                break;
            case 2:
                pInfo = &pSetUp->m_Mode1Dodge;
                break;
            case 3:
                pInfo = &pSetUp->m_Mode1Hockey;
                break;
            case 5:
                pInfo = &pSetUp->m_Mode1Basket;
                break;
        }
    } else {
        switch (nGame) {
            case 0:
                pInfo = &pSetUp->m_Basket;
                break;
            case 1:
                pInfo = &pSetUp->m_Volley;
                break;
            case 2:
                pInfo = &pSetUp->m_Dodge;
                break;
            case 3:
                pInfo = &pSetUp->m_Hockey;
                break;
        }
    }
    return pInfo;
}

long msw::_GetCupStageNum(long nGame, long nCup)
{
    return _CupStageBitCount(_GetStageBit(nGame, nCup));
}

// The lowest of the low four bits set in nBits, or -1.
long msw::_BeginBit(unsigned char nBits)
{
    for (long i = 0; i < 4; i++) {
        if (nBits & 1) {
            return i;
        }
        nBits >>= 1;
    }
    return -1;
}

const wchar_t *msw::GetMessageLabelString(const char *pGroup, const char *pLabel)
{
    menu::CMessage &rMessage = menu::CMessage::s_Instance;
    return rMessage.GetTextByLabel(rMessage.GetFileIndex(pGroup), pLabel);
}

const wchar_t *msw::GetMessageString(const char *pGroup, long nIndex)
{
    menu::CMessage &rMessage = menu::CMessage::s_Instance;
    return rMessage.GetText(rMessage.GetFileIndex(pGroup), nIndex);
}

short msw::GetMessageLabelIndex(const char *pGroup, const char *pLabel)
{
    menu::CMessage &rMessage = menu::CMessage::s_Instance;
    return rMessage.GetLabelIndex(rMessage.GetFileIndex(pGroup), pLabel);
}

int msw::_IsOpenMixSportsFlag()
{
    SAVE_DATA_PLAYER *pData = CSave::GetPlayerData(-1);
    int bOpen = pData != 0;
    if (bOpen) {
        // Bit 0x08 of the flags: the fifth (mix) sport is open.
        bOpen = (pData->m_nFlags5C50 >> 3) & 1;
    }
    return bOpen;
}

msw::CSelectMenu::CSelectMenu(bcs::CTask *pParent)
    : CMenuTask(pParent)
{
    dwc::CGameDwc *pDwc = dwc::CGameDwc::GetInstance();
    if (pDwc != 0) {
        pDwc->m_UnkDD4 = 0;
    }
    Initialize();
}

msw::CSelectMenu::~CSelectMenu()
{
    Finalize();
}

// The state of screen 0x32: nothing.
void msw::CSelectMenu::SelectNull_In()
{
}

void msw::CSelectMenu::SelectNull_Loop()
{
}

void msw::CSelectMenu::SelectNull_Out()
{
}

void msw::CSelectMenu::Disp()
{
    ModListDispLoop();
}

// Makes menu object nObjId from s_NewMenuClassTable and adds it to the
// task: -2 when it exists already, -3 when the table has no class for it.
long msw::CSelectMenu::CreateObj(long nObjId)
{
    if (FindObj(nObjId) != 0) {
        return -2;
    }
    CMenuLinkBase *(*pNew)(long) = s_NewMenuClassTable[nObjId];
    CMenuLinkBase *pObj;
    if (pNew != 0) {
        pObj = pNew(nObjId);
    } else {
        return -3;
    }
    return AddObj(pObj);
}

void msw::CSelectMenu::RemoveObj(long nObjId)
{
    CMenuLinkBase *pObj = FindObj(nObjId);
    if (pObj != 0) {
        m_LinkList.RemoveObject(pObj);
    }
}

const char *msw::CSelectMenu::GetArchiveFileName(long nArchive)
{
    return s_FileInputTable[nArchive].m_szPath;
}

const char *msw::CSelectMenu::GetArchiveRootFolderName(long nArchive)
{
    return s_pArchiveRootFolder;
}

bcs::CLoadMan::FILE_INPUT *msw::CSelectMenu::GetArchiveFileInfo(long nArchive)
{
    return &s_FileInputTable[nArchive];
}

unsigned char msw::CSelectMenu::GetLoadArchiveFileId(long nMode)
{
    return s_LoadArchiveFileId[nMode < 0 ? m_nState : nMode];
}

void msw::CSelectMenu::SetNextMode(long nMode, float fFadeTime)
{
    m_Unk98 = 1;
    m_nNextMode = nMode;
    m_fUnk94 = fFadeTime;
}

void msw::CSelectMenu::SetNextMode(long nMode, CSelectMenuLayoutBase *pObj)
{
    m_Unk98 = 1;
    m_nNextMode = nMode;
    if (pObj != 0) {
        m_fUnk94 = pObj->GetOutAnimFrameSize();
        pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
    }
}

float msw::CSelectMenuLayoutBase::GetOutAnimFrameSize()
{
    return s_DefaultOutAnimFrameSize;
}

// Follows the leading pad: marks the others, sends every pad's cursor
// state to the cockpit and notes which pads use the pointer.
void msw::CSelectMenu::PadStateCheck()
{
    CMenuPadInfo &rInfo = CMenuPadInfo::s_Instance;
    if (m_Unk27B == 0) {
        long nLeader = rInfo.m_nPadLeader;
        long nNewLeader = rInfo.SelectPadLeader(0);
        if (nLeader != nNewLeader) {
            ChangeLeaderPad(nNewLeader);
        }
    }

    CMenuPadInfo::PAD *pPad = CMenuPadInfo::s_Instance.m_aPad;
    for (long i = 0; i < 4; i++, pPad++) {
        unsigned char nFlags = pPad->m_nFlags04;
        unsigned char nCursor = 0x80;
        if ((nFlags & 1) && ((nFlags & 4) || i == rInfo.m_nPadLeader)) {
            nCursor = m_aPadSelect[i].m_Unk03;
        }
        CCockpit::SendObjEventTrigger(0x1016, 0x2715 + i, (void *)(nCursor | m_aPadSelect[i].m_nPlayerNo));
    }

    for (long i = 0; i < 4; i++) {
        m_aPadSelect[i].m_Unk01 = m_aPadSelect[i].m_Unk00;
        if (CMenuPad::s_Instance.IsInputPointerMode(i)) {
            m_aPadSelect[i].m_Unk00 = 0;
        } else {
            m_aPadSelect[i].m_Unk00 = 1;
        }
    }
}

// Makes pad nPad the one that leads the menus.
void msw::CSelectMenu::ChangeLeaderPad(long nPad)
{
    CMenuPadInfo::PAD *pPad = CMenuPadInfo::s_Instance.m_aPad;
    for (long i = 0; i < 4; i++, pPad++) {
        pPad->SetLeader(i == nPad);
        m_aPadSelect[i].m_Unk03 = i == nPad ? 0 : 0x40;
    }
}

// Moves the cursor of player nPad on pObj from button nOld to nNew.
inline void msw::CSelectMenu::MoveButtonCursor(CSelectMenuLayoutBase *pObj, long nPad, unsigned short &rCurrent,
                                               unsigned short nOld, short nNew)
{
    SetPreviousButtonKindIdWithoutNone(nPad, nOld, pObj);
    rCurrent = nNew;
    pObj->ClearCursorMarkerPosition(nPad, nOld);
    pObj->m_nPushButtonId[nPad] = nNew;
    pObj->SetCursorMarkerPosition(nPad, nNew);
}

// Passes the pointer or the stick of controller nPadNo to menu object
// pObj for player nPad on screen nMode: moves the player's cursor between
// the buttons and keeps m_nCurrentBtnKindId up to date.
void msw::CSelectMenu::InputDataToMenuObj(CSelectMenuLayoutBase *pObj, long nPad, long nPadNo, long nMode)
{
    CMenuPadInfo &rInfo = CMenuPadInfo::s_Instance;
    CMenuPadInfo::PAD &rPad = rInfo.m_aPad[nPadNo];
    if (!(rPad.m_nFlags04 & 4) && nPadNo != rInfo.m_nPadLeader) {
        return;
    }

    PadSelect &rSelect = m_aPadSelect[nPadNo];
    unsigned short &rCurrent = m_nCurrentBtnKindId[nPad][nMode];
    unsigned short nCurrent = rCurrent;
    int bPointer = rSelect.m_Unk00 == 0;
    if (rSelect.m_Unk00 != rSelect.m_Unk01 || rSelect.m_Unk02 != 0) {
        m_aPadSelect[nPadNo].m_Unk02 = 0;
        if (!(rPad.m_nFlags06 & 1)) {
            unsigned char nCursor = m_aPadSelect[nPadNo].m_Unk03;
            unsigned char nNew = nCursor | 0x40;
            if (bPointer == 1) {
                nNew = nCursor & ~0x40;
            }
            m_aPadSelect[nPadNo].m_Unk03 = nNew;
        }
        pObj->SetCursorMarkerVisible(nPadNo, 1);
    }

    if (rPad.m_nFlags06 & 4) {
        return;
    }

    if (bPointer == 1) {
        bcs::FVEC3 pos;
        CCockpit::SendObjEventTrigger(0x1016, nPadNo + 0x2711, &pos);
        if (rInfo.m_nCursorMode == 0) {
            if (!(rPad.m_nFlags06 & 1)) {
                short nHit = pObj->HitCheck(&pos);
                if (nHit != 0 && pObj->IsMarkerOnPointer(nPad, nHit)) {
                    MoveButtonCursor(pObj, nPad, rCurrent, nCurrent, nHit);
                }
            }
        } else if (!(rPad.m_nFlags06 & 1)) {
            short nHit = pObj->HitCheck(&pos);
            if (pObj->IsMarkerOnPointer(nPad, nHit)) {
                MoveButtonCursor(pObj, nPad, rCurrent, nCurrent, nHit);
            }
        }
    } else {
        if (!(rPad.m_nFlags06 & 2)) {
            if (pObj->IsButtonKindOfStickMode(nCurrent)) {
                short nMove = pObj->StickMoveCheck(nPadNo, nCurrent);
                if (nMove != nCurrent) {
                    MoveButtonCursor(pObj, nPad, rCurrent, nCurrent, nMove);
                }
            } else {
                unsigned short nPrev = m_nCupBtnKindId[nPad];
                if (!pObj->IsButtonKindOfStickMode(nPrev)) {
                    nPrev = pObj->SetDefaultMoveCursor(nPad, nPrev);
                }
                rCurrent = nPrev;
                pObj->m_nPushButtonId[nPad] = nPrev;
                pObj->SetCursorMarkerPosition(nPad, (short)nPrev);
            }
        }
        bcs::FVEC3 pos;
        CCockpit::SendObjEventTrigger(0x1016, nPadNo + 0x2711, &pos);
        unsigned short nHit = pObj->HitCheck(&pos);
        if (nHit != 0 && pObj->IsMarkerOnPointer(nPad, (short)nHit)) {
            CMenuPad::s_Instance.m_nPointerOnObj[nPad] = 1;
        }
    }
}

long msw::CSelectMenuLayoutBase::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::CSelectMenuLayoutBase::HitCheck(bcs::FVEC3 *pPos)
{
    return 0;
}

int msw::CSelectMenuLayoutBase::IsMarkerOnPointer(long, long)
{
    return 1;
}

long msw::CSelectMenuLayoutBase::ClearCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::CSelectMenuLayoutBase::SetCursorMarkerPosition(long, long)
{
    return 0;
}

int msw::CSelectMenuLayoutBase::IsButtonKindOfStickMode(long nBtnKindId)
{
    return 1;
}

long msw::CSelectMenuLayoutBase::StickMoveCheck(long nPad, long nBtnKindId)
{
    return 0;
}

long msw::CSelectMenuLayoutBase::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    return 0;
}

// Leaves the screen for screen nMode with the back button of pad nPad.
void msw::CSelectMenu::GoBackToMode(long nPad, long nMode, CSelectMenuLayoutBase *pObj, long nUnk)
{
    m_nCurrentBtnKindId[nPad][m_nState] = 1;
    if (pObj != 0) {
        pObj->PushButton(nPad, nUnk);
    }
    SetNextMode(nMode, pObj);
}

long msw::CSelectMenuLayoutBase::PushButton(long nPad, long nBtnKindId)
{
    return 0;
}

// The same, only when there is a menu object.
void msw::CSelectMenu::SetCancelBackMode(long nPad, long nMode, CSelectMenuLayoutBase *pObj, long nUnk)
{
    m_nCurrentBtnKindId[nPad][m_nState] = 1;
    if (pObj != 0) {
        pObj->PushButton(nPad, nUnk);
        SetNextMode(nMode, pObj);
    }
}

// The menu history: the screens passed through, newest first, as pairs
// of a screen and the screen it went on to.
void msw::CSelectMenu::ClearMenuHistory()
{
    m_nHistoryNum = 0;
    for (long i = 0; i < HISTORY_NUM; i++) {
        m_History[i].m_nState = 0xFF;
        m_History[i].m_nNextState = 0xFF;
    }
}

void msw::CSelectMenu::PushMenuHistory(unsigned char nState, unsigned char nNextState, unsigned short nBtnKindId)
{
    static const long index = 0;

    if (nState == nNextState) {
        return;
    }
    if (m_nHistoryNum > 0 && m_History[index].m_nState == nState && m_History[index].m_nNextState == nNextState) {
        return;
    }
    for (long i = m_nHistoryNum - 1; i >= 0; i--) {
        m_History[i + 1] = m_History[i];
    }
    m_History[index].m_nState = nState;
    m_History[index].m_nNextState = nNextState;
    m_nHistoryNum++;
    if ((unsigned long)m_nHistoryNum > HISTORY_NUM) {
        m_nHistoryNum = HISTORY_NUM;
    }
}

msw::CSelectMenu::History *msw::CSelectMenu::GetMenuHistory(unsigned char nState)
{
    for (long i = m_nHistoryNum - 1; i >= 0; i--) {
        if (nState == m_History[i].m_nState) {
            return &m_History[i];
        }
    }
    return 0;
}

msw::CSelectMenu::History *msw::CSelectMenu::GetMenuHistoryByNextState(unsigned char nNextState)
{
    for (long i = m_nHistoryNum - 1; i >= 0; i--) {
        if (nNextState == m_History[i].m_nNextState) {
            return &m_History[i];
        }
    }
    return 0;
}

// The screen before screen nState, skipping the ones that are not kept
// in the history (IsPushStateHistory); 1 when there is none.
unsigned char msw::CSelectMenu::GetPrevMode(unsigned char nState)
{
    long i;
    for (i = 0; i < m_nHistoryNum; i++) {
        if (m_History[i].m_nNextState == nState) {
            break;
        }
    }
    for (; i < m_nHistoryNum; i++) {
        if (m_History[i].m_nNextState == nState) {
            unsigned char nPrev = m_History[i].m_nState;
            int bPush = IsPushStateHistory(nPrev);
            if (bPush == 0) {
                nState = nPrev;
                continue;
            }
            return nPrev;
        }
    }
    return 1;
}

long msw::CSelectMenu::GetPrevMode()
{
    long nState = m_nState;
    unsigned char nPrev = GetPrevMode((unsigned char)nState);
    PopHistoryByState(nPrev, nState);
    return nPrev;
}

// Drops the history up to and including the entry nState -> nNextState.
void msw::CSelectMenu::PopHistoryByState(unsigned char nState, unsigned char nNextState)
{
    for (long i = 0; i < m_nHistoryNum; i++) {
        if (m_History[i].m_nState == nState && m_History[i].m_nNextState == nNextState) {
            long nRest = m_nHistoryNum - (i + 1);
            History *pDst = m_History;
            History *pSrc = &m_History[i + 1];
            for (long j = 0; j < nRest; j++) {
                pDst[j] = pSrc[j];
            }
            m_nHistoryNum = nRest;
            return;
        }
    }
}

unsigned short msw::CSelectMenu::GetHistoryButtonKindId(unsigned char nState, long nPad)
{
    return m_HistoryBtnKindId[nState].m_nPad[nPad];
}

void msw::CSelectMenu::SetHistoryButtonKindId(unsigned char nState, long nPad, unsigned short nBtnKindId)
{
    m_HistoryBtnKindId[nState].m_nPad[nPad] = nBtnKindId;
}

// Clears the remembered buttons of screen nState and every later one.
void msw::CSelectMenu::ClearHistoryButtonKindId(long nState)
{
    static const unsigned short btnKindId = 0;

    for (long i = nState; i < STATE_NUM; i++) {
        for (long j = 0; j < 4; j++) {
            m_HistoryBtnKindId[(unsigned char)i].m_nPad[j] = btnKindId;
        }
    }
}

// The players per team of the play mode.
long msw::CSelectMenu::GetTeamPlayerNum()
{
    switch (m_nPlayMode) {
        case 0:
        case 1:
        case 9:
        case 10:
        case 13:
        case 14:
        case 15:
        case 16:
            return 3;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 12:
        case 17:
            return 2;
        case 2:
        case 3:
        case 11:
            return 1;
    }
    return 1;
}

// The character slots (m_aCharaSelectParam). A slot belongs to a pad
// entry (m_Unk00, or the player kind m_nPlayerKind on the Wi-Fi and
// tournament menus) and is empty (state 0), being chosen (1), decided (2)
// or decided for now (3).
int msw::CSelectMenu::Chara_IsCharaDecide(long nIndex)
{
    return m_aCharaSelectParam[nIndex].m_nState >= 2U;
}

int msw::CSelectMenu::Chara_IsAllDecide(long nEntry)
{
    for (long i = 0; i < 6; i++) {
        switch (m_Unk9B) {
            case 0:
            case 2:
                if (nEntry == m_aCharaSelectParam[i].m_Unk00 && Chara_IsCharaDecide(i) == 0) {
                    return 0;
                }
                break;
            case 4:
                if (nEntry == m_aCharaSelectParam[i].m_nPlayerKind && Chara_IsCharaDecide(i) == 0) {
                    return 0;
                }
                break;
            default:
                if (nEntry == m_aCharaSelectParam[i].m_nPlayerKind && Chara_IsCharaDecide(i) == 0) {
                    return 0;
                }
                break;
        }
    }
    return 1;
}

// Gives character nChara (colour nColor: variant nColor / 2, shade
// nColor % 2; 0xFF none) to the first undecided slot of entry nEntry and
// puts it in state nState.
long msw::CSelectMenu::Chara_SetDecideChara(long nEntry, long nChara, long nColor, long nUnk0C, long nState)
{
    long nCharaSub = nColor >> 1;
    long nShade = nColor & 1;
    if (nColor == 0xFF) {
        nCharaSub = 0xFF;
        nShade = 0;
    }
    for (long i = 0; i < 6; i++) {
        if (nEntry == m_aCharaSelectParam[i].m_Unk00 && Chara_IsCharaDecide(i) == 0) {
            CharaSelectParam &rParam = m_aCharaSelectParam[i];
            rParam.m_nChara = nChara;
            rParam.m_nCharaSub = nCharaSub;
            rParam.m_Unk13[0] = nShade;
            rParam.m_nState = nState;
            rParam.m_Unk0C = nUnk0C;
            return i;
        }
    }
    return -1;
}

// The same, and plays the character model's decide motion.
long msw::CSelectMenu::Chara_SetDecideCharaWithMotion(long nEntry, long nChara, long nColor, long nBtnKindId)
{
    long nIndex = Chara_SetDecideChara(nEntry, nChara, nColor, nBtnKindId, 2);
    if (nIndex >= 0) {
        SelectMenu::CCharaMdl *pMdl = (SelectMenu::CCharaMdl *)FindObj(0x1C);
        if (pMdl != 0) {
            pMdl->SetStateChrMenuObj(nIndex, 1);
        }
    }
    return nIndex;
}

// Takes back character nChara from entry nEntry: empties the last slot
// being chosen, then puts the last decided slot with that character back
// to being chosen. Returns that slot, -1 for none.
long msw::CSelectMenu::Chara_CancelDecideChara(long nEntry, long nChara)
{
    long nIndex = -1;
    for (long i = 5; i >= 0; i--) {
        CharaSelectParam &rParam = m_aCharaSelectParam[i];
        if (nEntry == rParam.m_Unk00 && rParam.m_nState == 1) {
            CharaSelectParam &rClear = m_aCharaSelectParam[i];
            rClear.m_nState = 0;
            rClear.m_nChara = 0xFF;
            rClear.m_nCharaSub = 0xFF;
            rClear.m_Unk13[0] = 0;
            rClear.m_MiiSource = RFLDataSource_Default;
            rClear.m_nMiiIndex = 0;
            rClear.m_nMiiMode = 0xFF;
            rClear.m_Unk0C = 0;
            break;
        }
    }
    for (long i = 5; i >= 0; i--) {
        CharaSelectParam &rParam = m_aCharaSelectParam[i];
        if (nEntry == rParam.m_Unk00 && nChara == rParam.m_nChara && Chara_IsCharaDecide(i) == 1) {
            CharaSelectParam &rSlot = m_aCharaSelectParam[i];
            if ((unsigned char)(rSlot.m_nChara - 0x13) <= 5 && rSlot.m_nMiiMode != 0xFF) {
                rSlot.m_nCharaSub = 0xFF;
                rSlot.m_Unk13[0] = 0;
                rSlot.m_MiiSource = RFLDataSource_Default;
                rSlot.m_nMiiIndex = 0;
                rSlot.m_nMiiMode = 0xFF;
            }
            m_aCharaSelectParam[i].m_nState = 1;
            nIndex = i;
            break;
        }
    }
    return nIndex;
}

long msw::CSelectMenu::Chara_CancelDecideCharaWithMotion(long nEntry, long nChara)
{
    long nIndex = Chara_CancelDecideChara(nEntry, nChara);
    if (nIndex >= 0) {
        SelectMenu::CCharaMdl *pMdl = (SelectMenu::CCharaMdl *)FindObj(0x1C);
        if (pMdl != 0) {
            pMdl->SetStateChrMenuObj(nIndex, 0);
        }
    }
    return nIndex;
}

// Puts character nChara (colour nColor, -1 or 0xFF none) in the first
// undecided slot of entry nPad as being chosen; bShade forces the second
// shade.
void msw::CSelectMenu::Chara_SetSelectChara(long nPad, long nChara, long nColor, int bShade)
{
    long nCharaSub = nColor >> 1;
    long nShade = nColor & 1;
    if (nColor == 0xFF || nColor == -1) {
        nCharaSub = 0xFF;
        nShade = 0;
    }
    if (bShade != 0) {
        nShade = 1;
    }
    for (long i = 0; i < 6; i++) {
        if (nPad == m_aCharaSelectParam[i].m_Unk00 && Chara_IsCharaDecide(i) == 0) {
            CharaSelectParam &rParam = m_aCharaSelectParam[i];
            rParam.m_nChara = nChara;
            rParam.m_nCharaSub = nCharaSub;
            rParam.m_Unk13[0] = nShade;
            rParam.m_nState = 1;
            rParam.m_MiiSource = RFLDataSource_Default;
            rParam.m_nMiiIndex = 0;
            rParam.m_nMiiMode = 0xFF;
            rParam.m_Unk0C = 0;
            break;
        }
    }
}

// -1 when character nChara cannot be taken by entry nPad: another entry
// has it, or the entry has decided it already. The Mii (0x13) is always
// free.
long msw::CSelectMenu::Chara_CheckSelectChara(long nPad, long nChara, long nColor)
{
    if (nChara == 0x13) {
        return 1;
    }
    for (long i = 0; i < 6; i++) {
        CharaSelectParam &rParam = m_aCharaSelectParam[i];
        if (nPad == rParam.m_Unk00) {
            if (nChara == rParam.m_nChara && rParam.m_nState == 2) {
                return -1;
            }
        } else if (nChara == rParam.m_nChara) {
            return -1;
        }
    }
    return 1;
}

long msw::CSelectMenu::Chara_CheckSelectChara(long nPad, long nChara)
{
    for (long i = 0; i < 6; i++) {
        CharaSelectParam &rParam = m_aCharaSelectParam[i];
        if (nPad == rParam.m_Unk00) {
            if (nChara == rParam.m_nChara && rParam.m_nState == 2) {
                return -1;
            }
        } else if (nChara == rParam.m_nChara) {
            return -1;
        }
    }
    return 1;
}

long msw::CSelectMenu::Chara_GetDecideCharaNum(long nEntry)
{
    long nNum = 0;
    for (long i = 0; i < 6; i++) {
        if (nEntry == m_aCharaSelectParam[i].m_Unk00 && Chara_IsCharaDecide(i) == 1) {
            nNum++;
        }
    }
    return nNum;
}

msw::CSelectMenu::CharaSelectParam *msw::CSelectMenu::Chara_GetLastDecideChara(long nEntry)
{
    for (long i = 5; i >= 0; i--) {
        if (nEntry == m_aCharaSelectParam[i].m_Unk00 && Chara_IsCharaDecide(i) == 1) {
            return &m_aCharaSelectParam[i];
        }
    }
    return 0;
}

long msw::CSelectMenu::Chara_GetSaveCharaIndex(long nIndex, long &rEntry, long &rChara, long &rCharaSub, long &rColor,
                                               long &rPlayerKind)
{
    rEntry = m_aCharaSaveParam[nIndex].m_Unk00;
    rChara = m_aCharaSaveParam[nIndex].m_nChara;
    rCharaSub = m_aCharaSaveParam[nIndex].m_nCharaSub;
    rColor = m_aCharaSaveParam[nIndex].m_Unk13[0];
    rPlayerKind = m_aCharaSelectParam[nIndex].m_nPlayerKind;
    return 0;
}


// Gives the Mii (mode nMiiMode, index nIndex of source, createId) to the
// first undecided slot of pad nPad.
void msw::CSelectMenu::Mii_SelectParam(long nPad, unsigned char nMiiMode, long nIndex, RFLDataSource source,
                                       RFLCreateID createId)
{
    for (long i = 0; i < 6; i++) {
        if (nPad == m_aCharaSelectParam[i].m_Unk00 && Chara_IsCharaDecide(i) == 0) {
            if (m_nCharaColor[nPad] & 0xAA) {
                m_aCharaSelectParam[i].m_Unk13[0] = 1;
            } else {
                m_aCharaSelectParam[i].m_Unk13[0] = 0;
            }
            CharaSelectParam &rParam = m_aCharaSelectParam[i];
            rParam.m_nCharaSub = 0;
            rParam.m_nMiiMode = nMiiMode;
            rParam.m_nMiiIndex = nIndex;
            rParam.m_MiiSource = source;
            rParam.m_MiiCreateId = createId;
            return;
        }
    }
}

unsigned char msw::CSelectMenu::Mii_GetMiiMode(long nIndex)
{
    return m_aCharaSelectParam[nIndex].m_nMiiMode;
}

RFLDataSource msw::CSelectMenu::Mii_GetMiiDataSource(long nIndex)
{
    return m_aCharaSelectParam[nIndex].m_MiiSource;
}

long msw::CSelectMenu::Mii_GetMiiIndex(long nIndex)
{
    return m_aCharaSelectParam[nIndex].m_nMiiIndex;
}

unsigned long long msw::CSelectMenu::Mii_GetMiiCreateId(long nIndex)
{
    return *(unsigned long long *)&m_aCharaSelectParam[nIndex].m_MiiCreateId;
}

// Decides the Mii (0x13) for entry nEntry.
void msw::CSelectMenu::Mii_SetMiiParam(long nEntry, unsigned char nMiiMode, long nIndex, RFLDataSource source,
                                       RFLCreateID createId)
{
    long nSlot = Chara_SetDecideChara(nEntry, 0x13, 0, 0x4A, 3);
    if (m_nCharaColor[nEntry] & 0xAA) {
        m_aCharaSelectParam[nSlot].m_Unk13[0] = 1;
    }
    CharaSelectParam &rParam = m_aCharaSelectParam[nSlot];
    rParam.m_nMiiMode = nMiiMode;
    rParam.m_nMiiIndex = nIndex;
    rParam.m_MiiSource = source;
    rParam.m_MiiCreateId = createId;
}

// Drops the Mii nMiiIndex of source from entry nEntry: the slots being
// chosen are emptied, the decided ones go back to being chosen.
void msw::CSelectMenu::Mii_CancelMiiParam(long nEntry, RFLDataSource source, long nMiiIndex)
{
    static const unsigned char chrType = 0x13;

    for (long i = 5; i >= 0; i--) {
        CharaSelectParam &rParam = m_aCharaSelectParam[i];
        if (nEntry == rParam.m_Unk00 && rParam.m_nChara == chrType && rParam.m_nState == 1) {
            rParam.m_nState = 0;
            rParam.m_nChara = 0xFF;
            rParam.m_nCharaSub = 0xFF;
            rParam.m_Unk13[0] = 0;
            rParam.m_Unk0C = 0;
            rParam.m_MiiSource = RFLDataSource_Default;
            rParam.m_nMiiIndex = 0;
            rParam.m_nMiiMode = 0xFF;
        }
    }
    for (long i = 5; i >= 0; i--) {
        CharaSelectParam &rParam = m_aCharaSelectParam[i];
        if (nEntry == rParam.m_Unk00 && rParam.m_nChara == chrType && source == rParam.m_MiiSource &&
            nMiiIndex == rParam.m_nMiiIndex && Chara_IsCharaDecide(i)) {
            rParam.m_nState = 1;
            rParam.m_nChara = 0xFF;
            rParam.m_nCharaSub = 0xFF;
            rParam.m_Unk13[0] = 0;
            rParam.m_Unk0C = 0;
            rParam.m_MiiSource = RFLDataSource_Default;
            rParam.m_nMiiIndex = 0;
            rParam.m_nMiiMode = 0xFF;
        }
    }
}

unsigned char msw::CSelectMenu::Mii_GetSaveMiiMode(long nIndex)
{
    return m_aCharaSaveParam[nIndex].m_nMiiMode;
}

RFLDataSource msw::CSelectMenu::Mii_GetSaveMiiDataSource(long nIndex)
{
    return m_aCharaSaveParam[nIndex].m_MiiSource;
}

long msw::CSelectMenu::Mii_GetSaveMiiIndex(long nIndex)
{
    return m_aCharaSaveParam[nIndex].m_nMiiIndex;
}

unsigned long long msw::CSelectMenu::Mii_GetSaveMiiCreateId(long nIndex)
{
    return *(unsigned long long *)&m_aCharaSaveParam[nIndex].m_MiiCreateId;
}

// The second player's Mii in a two-player Wi-Fi game.
void msw::CSelectMenu::WiFi_Set2PMiiParam(unsigned char nMiiMode, long nIndex, RFLDataSource source,
                                          RFLCreateID createId)
{
    m_nStarMiiUnk = nMiiMode;
    m_nStarMiiIndex = nIndex;
    m_StarMiiSource = source;
    m_StarMiiCreateId = createId;
}

RFLStoreData msw::CSelectMenu::WiFi_Get2PMiiStoreData()
{
    RFLStoreData data;
    bcs::CMiiMan::GetStoreData(data, m_StarMiiSource, m_StarMiiCreateId);
    return data;
}

wchar_t *msw::CSelectMenu::WiFi_Get2PMiiName()
{
    RFLStoreData data;
    bcs::CMiiMan::GetStoreData(data, m_StarMiiSource, m_StarMiiCreateId);
    const wchar_t *pName;
    if (m_nStarMiiUnk != 0) {
        pName = GetMessageString("List_Mii_Name", m_nStarMiiIndex);
    } else {
        RFLAdditionalInfo info;
        bcs::CMiiMan::GetAdditionalInfo(info, data);
        pName = info.name;
    }
    wcscpy(m_Wifi2PName, pName);
    return m_Wifi2PName;
}

wchar_t *msw::CSelectMenu::GetMiiName(RFLStoreData data)
{
    SAVE_DATA_PLAYER *pPlayer = CSave::GetPlayerData(-1);
    SAVE_DATA_PLAYER::MiiData *pMii = pPlayer->GetIndexMii(pPlayer->SearchMiiIndex(data));
    const wchar_t *pName;
    if (pMii->m_Source != RFLDataSource_Official) {
        pName = GetMessageString("List_Mii_Name", pMii->m_nNameId);
    } else {
        RFLAdditionalInfo info;
        bcs::CMiiMan::GetAdditionalInfo(info, data);
        pName = info.name;
    }
    wcscpy(m_MiiNameWork, pName);
    return m_MiiNameWork;
}

// Sets up the six character slots for the play mode: who owns each slot
// (s_CharaSlotSetUp, by pad entry) and empties them.
void msw::CSelectMenu::InitCharaSelectParam()
{
    const CharaSlotSetUp *pTable = s_CharaSlotSetUp;
    const CharaSlotSetUp *pSetUp;
    unsigned char nPlayMode = m_nPlayMode;
    if ((long)m_Unk9B == 2) {
        switch (nPlayMode) {
            case 4:
                pSetUp = &pTable[23];
                break;
            case 6:
                pSetUp = &pTable[24];
                break;
            case 7:
                pSetUp = &pTable[25];
                break;
            case 8:
                pSetUp = &pTable[26];
                break;
            case 0:
                pSetUp = &pTable[3];
                break;
            case 5:
                pSetUp = &pTable[5];
                break;
            case 1:
                pSetUp = &pTable[6];
                break;
            case 9:
                pSetUp = &pTable[7];
                break;
            default:
                pSetUp = &pTable[27];
                break;
        }
    } else {
        switch (nPlayMode) {
            case 12:
                pSetUp = &pTable[0];
                break;
            case 13:
                pSetUp = &pTable[1];
                break;
            case 0:
                pSetUp = &pTable[3];
                break;
            case 1:
                pSetUp = &pTable[6];
                break;
            case 4:
                pSetUp = &pTable[2];
                break;
            case 5:
                pSetUp = &pTable[5];
                break;
            case 6:
                pSetUp = &pTable[4];
                break;
            case 7:
                pSetUp = &pTable[8];
                break;
            case 8:
                pSetUp = &pTable[11];
                break;
            case 9:
                pSetUp = &pTable[7];
                break;
            case 10:
                pSetUp = &pTable[14];
                break;
            case 14:
                pSetUp = &pTable[15];
                break;
            case 15:
                pSetUp = &pTable[16];
                break;
            case 16:
                pSetUp = &pTable[17];
                break;
            case 17:
                pSetUp = &pTable[10];
                break;
            case 18:
                pSetUp = &pTable[18];
                break;
            case 19:
                pSetUp = &pTable[19];
                break;
            case 20:
                pSetUp = &pTable[9];
                break;
            case 21:
                pSetUp = &pTable[20];
                break;
            case 22:
                pSetUp = &pTable[12];
                break;
            case 23:
                pSetUp = &pTable[13];
                break;
            case 24:
                pSetUp = &pTable[21];
                break;
            case 25:
                pSetUp = &pTable[22];
                break;
            default:
                pSetUp = &pTable[27];
                break;
        }
    }

    for (long nTeam = 0; nTeam < 2; nTeam++) {
        for (long i = 0; i < 3; i++) {
            long nSlot = nTeam * 3 + i;
            long nEntry = GetPadEntryId(pSetUp->m_nEntry[nTeam][i]);
            CharaSelectParam &rParam = m_aCharaSelectParam[nSlot];
            rParam.m_Unk00 = nEntry;
            rParam.m_nPlayerKind = GetPadEntryId(pSetUp->m_nPlayerKind[nTeam][i]);
            rParam.m_Unk08 = pSetUp->m_nUnk08[nTeam][i];
            rParam.m_nState = 0;
            rParam.m_nChara = 0xFF;
            rParam.m_nCharaSub = 0xFF;
            rParam.m_Unk13[0] = 0;
            rParam.m_Unk0C = 0;
            rParam.m_MiiSource = RFLDataSource_Default;
            rParam.m_nMiiIndex = 0;
            rParam.m_nMiiMode = 0xFF;
        }
    }
    for (long i = 0; i < 4; i++) {
        m_HistoryBtnKindId[0xE].m_nPad[i] = 0;
    }
}

void msw::CSelectMenu::ChangeTeamCharaSelectParam()
{
    InitCharaSelectParam();
}

// Keeps the slots as they are now (Chara_GetSaveCharaIndex and the
// Mii_GetSave* getters read the copy).
void msw::CSelectMenu::SetCharaSelectSaveParam()
{
    m_aCharaSaveParam[0] = m_aCharaSelectParam[0];
    m_aCharaSaveParam[1] = m_aCharaSelectParam[1];
    m_aCharaSaveParam[2] = m_aCharaSelectParam[2];
    m_aCharaSaveParam[3] = m_aCharaSelectParam[3];
    m_aCharaSaveParam[4] = m_aCharaSelectParam[4];
    m_aCharaSaveParam[5] = m_aCharaSelectParam[5];
}

int msw::CSelectMenu::IsPushStateHistory(long nState)
{
    if (nState == 0x11 || nState == 0x2F || nState == 0x31) {
        return 0;
    }
    return 1;
}

long msw::CSelectMenu::GetPlayerEntryId(long nPad)
{
    return m_nPadPlayer[nPad];
}

// The entry of controller nPad (unchanged above 3 or without an entry).
long msw::CSelectMenu::GetPadEntryId(long nPad)
{
    if ((unsigned long)nPad > 3) {
        return nPad;
    }
    for (long i = 0; i < 4; i++) {
        if (nPad == m_nPadPlayer[i]) {
            return i;
        }
    }
    return nPad;
}

// Remembers button nBtnKindId as player nPad's previous button, unless it
// is none or one pObj's stick cannot reach.
void msw::CSelectMenu::SetPreviousButtonKindIdWithoutNone(long nPad, unsigned short nBtnKindId,
                                                          CSelectMenuLayoutBase *pObj)
{
    if (nBtnKindId != 0 && pObj->IsButtonKindOfStickMode(nBtnKindId)) {
        m_nCupBtnKindId[nPad] = nBtnKindId;
    }
}

long msw::CSelectMenu::Chara_FindNotDecideCharaIndex(long nEntry)
{
    for (long i = 0; i < 6; i++) {
        switch (m_Unk9B) {
            case 0:
            case 2:
                if (nEntry == m_aCharaSelectParam[i].m_Unk00 && Chara_IsCharaDecide(i) == 0) {
                    return i;
                }
                break;
            default:
                if (nEntry == m_aCharaSelectParam[i].m_nPlayerKind && Chara_IsCharaDecide(i) == 0) {
                    return i;
                }
                break;
        }
    }
    return -1;
}

long msw::CSelectMenu::Chara_FindSelectCharaIndex(long nEntry)
{
    for (long i = 0; i < 6; i++) {
        if (nEntry == m_aCharaSelectParam[i].m_Unk00 && Chara_IsCharaDecideSelect(i) == 1) {
            return i;
        }
    }
    return -1;
}

long msw::CSelectMenu::Chara_FindNoneCharaIndex(long nEntry)
{
    for (long i = 0; i < 6; i++) {
        if (nEntry == m_aCharaSelectParam[i].m_Unk00 && Chara_IsCharaDecideNone(i) == 1) {
            return i;
        }
    }
    return -1;
}

long msw::CSelectMenu::Chara_FindLastCharaIndex(long nEntry)
{
    for (long i = 5; i >= 0; i--) {
        if (nEntry == m_aCharaSelectParam[i].m_Unk00 && m_aCharaSelectParam[i].m_nState != 0) {
            return i;
        }
    }
    return -1;
}

msw::CSelectMenu::CharaSelectParam *msw::CSelectMenu::GetCharaSelectParam(long nIndex)
{
    if ((unsigned long)nIndex <= 5) {
        return &m_aCharaSelectParam[nIndex];
    }
    return 0;
}

int msw::CSelectMenu::Chara_IsCharaDecideNone(long nIndex)
{
    return m_aCharaSelectParam[nIndex].m_nState == 0;
}

int msw::CSelectMenu::Chara_IsCharaDecideAlready(long nIndex)
{
    return m_aCharaSelectParam[nIndex].m_nState == 2;
}

int msw::CSelectMenu::Chara_IsCharaDecideSelect(long nIndex)
{
    return m_aCharaSelectParam[nIndex].m_nState == 1;
}

int msw::CSelectMenu::Chara_IsCharaDecideTemporary(long nIndex)
{
    return m_aCharaSelectParam[nIndex].m_nState == 3;
}

void msw::CSelectMenu::Chara_ChangeDecideTemporaryToDecideAlready()
{
    static const long num = 3;

    for (long i = 0; i < 6; i++) {
        if (m_aCharaSelectParam[i].m_nState == num) {
            m_aCharaSelectParam[i].m_nState = 2;
        }
    }
}

// Every pad that has decided all its characters gives its last one back
// (the back button on the character select screen).
void msw::CSelectMenu::Chara_CancelLastDecideChara()
{
    long nState = m_nState;
    SelectMenu::CCharaSelect *pSelect = (SelectMenu::CCharaSelect *)FindObj(0x10);
    for (long nPad = 0; nPad < 4; nPad++) {
        if (!Chara_IsAllDecide(nPad)) {
            continue;
        }
        CharaSelectParam *pParam = Chara_GetLastDecideChara(nPad);
        if (pParam == 0) {
            continue;
        }
        unsigned char nChara = pParam->m_nChara;
        unsigned short nBtnKindId = -1;
        for (long i = 0; i < 19; i++) {
            if (nChara == s_CharaButton[i].m_nChara) {
                nBtnKindId = s_CharaButton[i].m_nBtnKindId;
                break;
            }
        }
        m_nCurrentBtnKindId[nPad][nState] = nBtnKindId;
        if (pSelect != 0) {
            pSelect->m_nPushButtonId[nPad] = nBtnKindId;
            pSelect->SetCursorMarkerPosition(nPad, nBtnKindId);
        }
        Chara_CancelDecideChara(nPad, nChara);
        if (pSelect != 0) {
            pSelect->PushCancelButton(nPad, nBtnKindId);
        }
    }
}

void msw::CSelectMenu::SetBlockFadeOut(float fFadeTime)
{
    float fSpeed = 1.0f;
    if (m_Unk27F == 0) {
        fSpeed = 1.0f / fFadeTime;
    }
    bcs::CMainApp::StartMasterFade(0, fSpeed);
    m_Unk27F = 1;
}

void msw::CSelectMenu::SetWhiteFadeOut(float fFadeTime)
{
    float fSpeed = 1.0f;
    if (m_Unk27F == 0) {
        fSpeed = 1.0f / fFadeTime;
    }
    bcs::CMainApp::StartMasterFade(1, fSpeed);
    m_Unk27F = 2;
}

void msw::CSelectMenu::SetFadeIn(float fFadeTime)
{
    float fSpeed = 1.0f;
    if (m_Unk27F != 0) {
        fSpeed = 1.0f / fFadeTime;
    }
    bcs::CMainApp::StartMasterFade(4, fSpeed);
    m_Unk27F = 0;
}

float msw::CSelectMenu::GetBgmFadeOutTime()
{
    return 30.0f;
}

void msw::CSelectMenu::DecrementDelayFrameCount()
{
    float fWait = m_fEntryErrorWait;
    fWait -= bcs::CMainApp::GetAppFrameRateF();
    if (fWait < 0.0f) {
        fWait = 0.0f;
    }
    m_fEntryErrorWait = fWait;
}

int msw::CSelectMenu::IsBackButtonMove(long nPad)
{
    return m_nCurrentBtnKindId[nPad][m_Unk9D] == 1;
}

const msw::CSelectMenu::MenuBGM *msw::CSelectMenu::GetMenuBGMId(long nBgm)
{
    if (nBgm >= MENU_BGM_NUM) {
        return 0;
    }
    return &s_MenuBGM[nBgm];
}

float msw::CSelectMenu::GetMenuBGMFadeoutTime()
{
    return 60.0f;
}

// Starts menu music nBgm unless it is playing already.
void msw::CSelectMenu::PlayMenuBGM(long nBgm)
{
    const MenuBGM *pBgm = GetMenuBGMId(nBgm);
    menu::CSndBGM &rBGM = menu::CSndBGM::s_Instance;
    if (pBgm != 0 && pBgm->m_nSeq != rBGM.GetSeq()) {
        StopMenuBGM();
        if (pBgm->m_bPlay == 0) {
            rBGM.SeqLoad(pBgm->m_nSeq, 1);
        } else {
            menu::CSndBGM::s_Instance.Play(pBgm->m_nSeq);
        }
    }
}

void msw::CSelectMenu::PlayMenuBGMToTop(long nBgm)
{
}

void msw::CSelectMenu::StopMenuBGM()
{
    menu::CSndBGM &rBGM = menu::CSndBGM::s_Instance;
    if (rBGM.GetTaskId() != 0) {
        rBGM.Stop(rBGM.GetTaskId(), GetMenuBGMFadeoutTime());
    }
}

// Starts menu music nBgm again when it has stopped.
void msw::CSelectMenu::LoopPlayMenuBGM(long nBgm)
{
    const MenuBGM *pBgm = GetMenuBGMId(nBgm);
    menu::CSndBGM &rBGM = menu::CSndBGM::s_Instance;
    if (pBgm != 0 && pBgm->m_nSeq == rBGM.GetSeq() && !rBGM.IsPlayNow(rBGM.GetTaskId())) {
        if (pBgm->m_bPlay == 0) {
            rBGM.SeqLoad(pBgm->m_nSeq, 1);
        } else {
            menu::CSndBGM::s_Instance.Play(pBgm->m_nSeq);
        }
    }
}

void msw::CSelectMenu::PlayMenuSE(long nSe)
{
    menu::CSndEff::s_Instance.Play(nSe, 1);
}

void msw::CSelectMenu::Load(long nArchive)
{
    if (nArchive >= 0) {
        bcs::CLoadMan::FILE_INPUT *pFile = &s_FileInputTable[nArchive];
        if (pFile->m_nId & 1) {
            bcs::G2D::CLytResource *pRes = bcs::G2D::CLytResource::GetInstance();
            if (pRes != 0) {
                pRes->LoadDataASync(pFile);
            }
        }
    }
}

void msw::CSelectMenu::UnLoad(long nArchive)
{
    if (nArchive >= 0) {
        bcs::CLoadMan::FILE_INPUT *pFile = &s_FileInputTable[nArchive];
        if (pFile->m_nId & 1) {
            bcs::G2D::CLytResource *pRes = bcs::G2D::CLytResource::GetInstance();
            if (pRes != 0) {
                pRes->Cleanup(pFile->m_szPath);
            }
            bcs::CLoadMan::UnLoad(pFile);
        }
    }
}

int msw::CSelectMenu::IsLoaded(long nArchive)
{
    if (bcs::G2D::CLytResource::GetInstance() == 0) {
        return 0;
    }
    bcs::CLoadMan::FILE_INPUT *pFile = &s_FileInputTable[nArchive];
    if (!(pFile->m_nId & 1)) {
        return 1;
    }
    return bcs::CLoadMan::IsLoaded(pFile->m_szPath, pFile->m_nId);
}

int msw::CSelectMenu::IsComplete(long nArchive)
{
    bcs::G2D::CLytResource *pRes = bcs::G2D::CLytResource::GetInstance();
    if (pRes == 0) {
        return 0;
    }
    bcs::CLoadMan::FILE_INPUT *pFile = &s_FileInputTable[nArchive];
    if (!(pFile->m_nId & 1)) {
        return 1;
    }
    return pRes->IsCompleteDataASync(pFile) >= 0;
}

int msw::CSelectMenu::IsReading(long nArchive)
{
    bcs::CLoadMan::FILE_INPUT *pFile = &s_FileInputTable[nArchive];
    if (pFile->m_nId & 1) {
        return bcs::CLoadMan::IsReading(pFile->m_szPath, pFile->m_nId);
    }
    return 1;
}

// Builds the archive table (the path in the language's folder) and
// starts loading what the first screen needs.
void msw::CSelectMenu::InitLoad()
{
    const char *pFolder = menu::GetLocalizeFolderName();
    const bcs::CLoadMan::FILE_INPUT *pSrc = s_FileInputInit;
    bcs::CLoadMan::FILE_INPUT *pDst = s_FileInputTable;
    for (long i = 0; i < ARCHIVE_NUM; i++, pSrc++, pDst++) {
        sprintf(pDst->m_szPath, pSrc->m_szPath, pFolder);
        pDst->m_nId = pSrc->m_nId;
        pDst->m_nFlags = pSrc->m_nFlags;
        pDst->m_Unk108 = pSrc->m_Unk108;
        pDst->m_nAlign = pSrc->m_nAlign;
        pDst->m_Unk10C = pSrc->m_Unk10C;
        pDst->m_Unk106 = pSrc->m_Unk106;
    }

    Load(2);
    switch (menu::CStateSave::GetInstance()->GetStartMenuMode()) {
        case 6:
        case 7:
        case 8:
            Load(12);
            break;
        case 3:
            Load(4);
            break;
        case 4:
            Load(5);
            break;
    }
}

int msw::CSelectMenu::IsCompleteOfInitLoad()
{
    switch (menu::CStateSave::GetInstance()->GetStartMenuMode()) {
        case 6:
        case 7:
        case 8:
            if (!IsComplete(12)) {
                return 0;
            }
            break;
        case 3:
            if (!IsComplete(4)) {
                return 0;
            }
            break;
        case 4:
            if (!IsComplete(5)) {
                return 0;
            }
            break;
        default:
            if (!IsComplete(2)) {
                return 0;
            }
            break;
    }
    return 1;
}

float msw::CSelectMenu::GetCameraSpeed(long nAnim)
{
    return s_CameraSpeed[nAnim];
}

// Sets up the menu: the menu singletons, the state machine (one state per
// screen), the menu objects, the camera, then the first screen, chosen by
// where the menu was entered from (menu::CStateSave).
void msw::CSelectMenu::Initialize()
{
    static const long camNo = 0;

    int bTopMenuIn = 0;
    CMenuPad::GetInstance()->ResetNoInputCountTime(7200.0f);
    menu::CSndEff::GetInstance()->Initialize();
    menu::CSndBGM::GetInstance()->Initialize();
    menu::CVibEff::GetInstance()->Initialize();
    CChrMenuCtrl::GetInstance()->Initialize();
    bcs::G2D::CMenuMdlScnGroup::GetPersProjInfo()->Reset();
    CMenuOption::GetInstance()->Initialize();

    InitFsm(this, STATE_NUM, 0);
    SetStateFunc(0x00, &CSelectMenu::InitGuide_In, &CSelectMenu::InitGuide_Loop, &CSelectMenu::InitGuide_Out);
    SetStateFunc(0x01, &CSelectMenu::Title_In, &CSelectMenu::Title_Loop, &CSelectMenu::Title_Out);
    SetStateFunc(0x02, &CSelectMenu::PlaterSelect_In, &CSelectMenu::PlaterSelect_Loop, &CSelectMenu::PlaterSelect_Out);
    SetStateFunc(0x03, &CSelectMenu::MiiSel_In, &CSelectMenu::MiiSel_Loop, &CSelectMenu::MiiSel_Out);
    SetStateFunc(0x04, &CSelectMenu::TopMenu_In, &CSelectMenu::TopMenu_Loop, &CSelectMenu::TopMenu_Out);
    SetStateFunc(0x05, &CSelectMenu::GameMode_In, &CSelectMenu::GameMode_Loop, &CSelectMenu::GameMode_Out);
    SetStateFunc(0x06, &CSelectMenu::HowTo_In, &CSelectMenu::HowTo_Loop, &CSelectMenu::HowTo_Out);
    SetStateFunc(0x07, &CSelectMenu::ScoreTop_In, &CSelectMenu::ScoreTop_Loop, &CSelectMenu::ScoreTop_Out);
    SetStateFunc(0x08, &CSelectMenu::ScoreBook_In, &CSelectMenu::ScoreBook_Loop, &CSelectMenu::ScoreBook_Out);
    SetStateFunc(0x09, &CSelectMenu::Entry_In, &CSelectMenu::Entry_Loop, &CSelectMenu::Entry_Out);
    SetStateFunc(0x0C, &CSelectMenu::PlayMode_In, &CSelectMenu::PlayMode_Loop, &CSelectMenu::PlayMode_Out);
    SetStateFunc(0x0D, &CSelectMenu::Team_In, &CSelectMenu::Team_Loop, &CSelectMenu::Team_Out);
    SetStateFunc(0x0E, &CSelectMenu::CharSel_In, &CSelectMenu::CharSel_Loop, &CSelectMenu::CharSel_Out);
    SetStateFunc(0x10, &CSelectMenu::Court_In, &CSelectMenu::Court_Loop, &CSelectMenu::Court_Out);
    SetStateFunc(0x11, &CSelectMenu::MiiSel_In, &CSelectMenu::MiiSel_Loop, &CSelectMenu::MiiSel_Out);
    SetStateFunc(0x12, &CSelectMenu::PartySelect_In, &CSelectMenu::PartySelect_Loop, &CSelectMenu::PartySelect_Out);
    SetStateFunc(0x13, &CSelectMenu::PartyInfo_In, &CSelectMenu::PartyInfo_Loop, &CSelectMenu::PartyInfo_Out);
    SetStateFunc(0x15, &CSelectMenu::Cup_In, &CSelectMenu::Cup_Loop, &CSelectMenu::Cup_Out);
    SetStateFunc(0x16, &CSelectMenu::WifiEntry_In, &CSelectMenu::WifiEntry_Loop, &CSelectMenu::WifiEntry_Out);
    SetStateFunc(0x19, &CSelectMenu::MiiSel_In, &CSelectMenu::MiiSel_Loop, &CSelectMenu::MiiSel_Out);
    SetStateFunc(0x1A, &CSelectMenu::WifiConnect_In, &CSelectMenu::WifiConnect_Loop, &CSelectMenu::WifiConnect_Out);
    SetStateFunc(0x1E, &CSelectMenu::WifiGame_In, &CSelectMenu::WifiGame_Loop, &CSelectMenu::WifiGame_Out);
    SetStateFunc(0x20, &CSelectMenu::WifiCourt_In, &CSelectMenu::WifiCourt_Loop, &CSelectMenu::WifiCourt_Out);
    SetStateFunc(0x21, &CSelectMenu::WifiWait_In, &CSelectMenu::WifiWait_Loop, &CSelectMenu::WifiWait_Out);
    SetStateFunc(0x23, &CSelectMenu::FriendCode_In, &CSelectMenu::FriendCode_Loop, &CSelectMenu::FriendCode_Out);
    SetStateFunc(0x24, &CSelectMenu::FriendList_In, &CSelectMenu::FriendList_Loop, &CSelectMenu::FriendList_Out);
    SetStateFunc(0x25, &CSelectMenu::FriendPlayerInfo_In, &CSelectMenu::FriendPlayerInfo_Loop,
                 &CSelectMenu::FriendPlayerInfo_Out);
    SetStateFunc(0x26, &CSelectMenu::FriendInfo_In, &CSelectMenu::FriendInfo_Loop, &CSelectMenu::FriendInfo_Out);
    SetStateFunc(0x27, &CSelectMenu::NoFriendInfo_In, &CSelectMenu::NoFriendInfo_Loop, &CSelectMenu::NoFriendInfo_Out);
    SetStateFunc(0x2A, &CSelectMenu::FriendVS_In, &CSelectMenu::FriendVS_Loop, &CSelectMenu::FriendVS_Out);
    SetStateFunc(0x28, &CSelectMenu::FriendVSWait_In, &CSelectMenu::FriendVSWait_Loop, &CSelectMenu::FriendVSWait_Out);
    SetStateFunc(0x2B, &CSelectMenu::RuleSel_In, &CSelectMenu::RuleSel_Loop, &CSelectMenu::RuleSel_Out);
    SetStateFunc(0x2C, &CSelectMenu::PartyCourt_In, &CSelectMenu::PartyCourt_Loop, &CSelectMenu::PartyCourt_Out);
    SetStateFunc(0x2D, &CSelectMenu::RuleInfo_In, &CSelectMenu::RuleInfo_Loop, &CSelectMenu::RuleInfo_Out);
    SetStateFunc(0x2F, &CSelectMenu::Test_In, &CSelectMenu::Test_Loop, &CSelectMenu::Test_Out);
    SetStateFunc(0x30, &CSelectMenu::Test2_In, &CSelectMenu::Test2_Loop, &CSelectMenu::Test2_Out);
    SetStateFunc(0x2E, &CSelectMenu::TimeOver_In, &CSelectMenu::TimeOver_Loop, &CSelectMenu::TimeOver_Out);
    SetStateFunc(0x31, &CSelectMenu::SelectEnd_In, &CSelectMenu::SelectEnd_Loop, &CSelectMenu::SelectEnd_Out);
    SetStateFunc(0x32, &CSelectMenu::SelectNull_In, &CSelectMenu::SelectNull_Loop, &CSelectMenu::SelectNull_Out);
    if (CDataWork::GetInstance()->GetCntInfoDemo() == 0) {
        ChangeState(0);
    } else {
        ChangeState(1);
    }

    InitVariable();
    for (long i = 0; i < 0x4F; i++) {
        CreateObj(i);
    }
    CCockpit::CreatePointerObj(995.0f);

    m_pGameDwc = dwc::CGameDwc::GetInstance();
    if (m_pGameDwc != 0 && m_pGameDwc->m_Dwc.IsConnect()) {
        m_pFriendList = m_pGameDwc->GetFriendList();
    } else {
        m_pFriendList = 0;
    }

    CCameraMenuBG *pCamera = CCameraMenuBG::GetInstance();
    nw4r::g3d::ResFile file(pCamera->GetResFileFromEfResource(s_pCameraFileName));
    pCamera->Initialize(camNo, file);

    SelectCourtBackOff(0);
    CGamePadCheck::Initialize();
    Build(0x3F);
    m_bWifiWaitCourt = 0;

    if (Restore() == 1) {
        switch (menu::CStateSave::GetInstance()->GetStartMenuMode()) {
            case 6:
                // Back from a Wi-Fi match: the match-making screen.
                m_bWifiWaitCourt = 1;
                RestorePlayMode();
                RestoreWiFiData();
                PlayMenuBGM(4);
                ChangeState(0x21);
                CGamePadCheck::GetInstance()->CheckControllerCount(GetPlayerNum());
                menu::CStateSave::GetInstance()->SetStartMenuMode(0);
                CGamePadCheck::PadStateRestore();
                break;
            case 7:
                // Back from a friend match.
                SetBlockFadeOut(1.0f);
                PlayMenuBGM(4);
                ChangeState(0x28);
                bcs::CPU_CLEAR(m_FriendVSWaitWork, sizeof(m_FriendVSWaitWork));
                FriendVSWait_SendSetStep(0x10);
                SetMatchFrind(m_pGameDwc->GetPlayMode());
                CGamePadCheck::GetInstance()->CheckControllerCount(GetPlayerNum());
                menu::CStateSave::GetInstance()->SetStartMenuMode(0);
                CGamePadCheck::PadStateRestore();
                if (GetCursorInputMode(0) == 1) {
                    m_UnkA78 = 1;
                } else {
                    m_UnkA78 = 0;
                }
                break;
            case 8:
                // Back to the Wi-Fi top screen.
                PlayMenuBGM(4);
                ChangeState(0x1A);
                CGamePadCheck::GetInstance()->CheckControllerCount(GetPlayerNum());
                menu::CStateSave::GetInstance()->SetStartMenuMode(0);
                CGamePadCheck::PadStateRestore();
                break;
            case 3:
                // Back to the character select screen.
                if (GetMenuMode() == 2) {
                    PlayMenuBGM(2);
                } else {
                    PlayMenuBGM(1);
                }
                ChangeState(0xE);
                break;
            case 4:
                // Back to the court select screen.
                if (GetMenuMode() == 2) {
                    ChangeState(0x2C);
                    PlayMenuBGM(2);
                } else {
                    ChangeState(0x10);
                    PlayMenuBGM(1);
                }
                SetBlockFadeOut(1.0f);
                bTopMenuIn = 1;
                SelectCourtBackOff(0);
                menu::CStateSave::GetInstance()->SetStartMenuMode(0);
                break;
            case 2:
                // Back to the top menu.
                SetWhiteFadeOut(1.0f);
                PlayMenuBGM(1);
                ChangeState(4);
                SetModeEnd(0);
                SetEndWaitTime(0.0f);
                SetNextMode(0UL);
                SetPrevModeState(0);
                SetPlayerNum(1);
                ClearHistoryButtonKindId(5);
                CGamePadCheck::GetInstance()->CheckControllerCount(GetPlayerNum());
                menu::CStateSave::GetInstance()->SetStartMenuMode(0);
                break;
            case 5:
                // Back to the how-to screen.
                CGamePadCheck::SetPadCheckDisable(0);
                SetBlockFadeOut(1.0f);
                PlayMenuBGM(1);
                ChangeState(6);
                menu::CStateSave::GetInstance()->SetStartMenuMode(0);
                break;
            case 1:
                ChangeState(1);
                menu::CStateSave::GetInstance()->SetStartMenuMode(0);
                break;
        }

        int bMix = _IsOpenMixSportsFlag();
        SelectMenu::CBgChrTop *pBgChr = GetObj<SelectMenu::CBgChrTop>(0x3A);
        if (pBgChr != 0) {
            pBgChr->SetMixModeFlag(bMix);
        }
        SelectMenu::CCourtChr *pCourtChr = GetObj<SelectMenu::CCourtChr>(0x3B);
        if (pCourtChr != 0) {
            pCourtChr->SetMixModeFlag(bMix);
        }
        SelectMenu::CCourtBall *pCourtBall = GetObj<SelectMenu::CCourtBall>(0x40);
        if (pCourtBall != 0) {
            pCourtBall->SetMixModeFlag(bMix);
        }
        SelectMenu::CCourtChrBall *pCourtChrBall = GetObj<SelectMenu::CCourtChrBall>(0x42);
        if (pCourtChrBall != 0) {
            pCourtChrBall->SetMixModeFlag(bMix);
        }
        Build(0x3A);
        Build(0x3B);
        Build(0x40);
        Build(0x42);
        CCameraMenuBG::GetInstance()->SetAnimTable(s_CameraAnimTable, CAMERA_ANIM_NUM, bMix == 1);
    }

    Build(0);
    CStageMan *pStage = new CStageManMenu(this);
    pStage->Init();
    m_fUnkA10 = 0.0f;
    m_nMiiSelFlags = 0;
    m_pVsFriend = 0;
    m_fFriendMatchWait = 0.0f;
    m_UnkA00 = 0;
    m_Unk70C = 0;
    m_nUnlockStep = 0;
    m_nUnlockCount = 0;
    if (GetCupId() == 0xFF) {
        SetCupId(0);
        SetStageId(0);
    }
    dwc::CGameDwc::InitErrorStat();
    CNowLoading::SetDisp(0);
    Build(1);
    if (bTopMenuIn != 0) {
        SendObjEvent(1, 0x56);
    }
    CDataWork::GetInstance()->SetCntInfoDemo(1);
}

// Puts every member back to its start value.
void msw::CSelectMenu::InitVariable()
{
    for (long i = 0; i < 4; i++) {
        for (long j = 0; j < 0x33; j++) {
            m_nCurrentBtnKindId[i][j] = 0;
        }
    }
    m_Unk98 = 0;
    m_fUnk94 = 0.0f;
    m_nNextMode = 0;
    m_Unk9D = 0;
    m_nCourtGameKind = -1;
    m_Unk9B = 0;
    m_nGameKind = 0;
    m_Unk9C = 0;
    m_nPlayerNum = 0;
    m_nPlayMode = 0;
    m_Unk26D = 0x12;
    m_Unk27A = 3;
    m_Unk269 = 0;
    m_Unk26C = 0;
    m_nStageSub = 0;
    m_nStage = 0;
    m_Unk27E = 0;
    m_Unk27F = 0;
    m_Unk27D = 0;
    m_Unk9A = 0;

    CMenuPadInfo::PAD *pPad = CMenuPadInfo::s_Instance.m_aPad;
    for (long i = 0; i < 4; i++) {
        m_aPadSelect[i].m_Unk00 = 1;
        m_aPadSelect[i].m_Unk01 = 0;
        m_aPadSelect[i].m_Unk02 = 1;
        m_aPadSelect[i].m_nPlayerNo = i & 0x3F;
        CMenuPadInfo::s_Instance.m_aPad[i].m_nPlayerNo = CMenuPadInfo::s_Instance.GetDefaultPlayerNo(i);
        m_aPadSelect[i].m_Unk03 = 0x80;
        m_nPadPlayer[i] = i;
        pPad[i].m_nFlags04 &= ~4;
        pPad[i].m_nFlags06 = (pPad[i].m_nFlags06 | 4) & ~1 & ~2;
    }
    for (long i = 0; i < 4; i++) {
        long nPlayerNo = CMenuPadInfo::s_Instance.GetDefaultPlayerNo(i);
        m_aPadSelect[i].m_nPlayerNo = nPlayerNo & 0x3F;
        CMenuPadInfo::s_Instance.m_aPad[i].m_nPlayerNo = nPlayerNo;
        int bLeader = i == 0;
        if (bLeader) {
            pPad[i].m_nFlags04 |= 4;
        } else {
            pPad[i].m_nFlags04 &= ~4;
        }
        m_aPadSelect[i].m_Unk03 = bLeader ? 0 : 0x40;
    }

    ClearMenuHistory();
    ClearHistoryButtonKindId(0);
    m_fEntryErrorWait = 0.0f;
    for (long i = 0; i < 4; i++) {
        m_fRandomMoveTime[i] = -1.0f;
        m_nRandomDecideTime[i] = -1;
    }
    for (long i = 0; i < 6; i++) {
        m_Unk6B4[i] = 0;
    }
    InitCharaSelectParam();
    SetCharaSelectSaveParam();
}

// Frees what Initialize made.
void msw::CSelectMenu::Finalize()
{
    for (long i = 0; i < 0x4F; i++) {
        CMenuLinkBase *pObj = FindObj(i);
        if (pObj != 0) {
            m_LinkList.RemoveObject(pObj);
        }
    }
    CMenuOption::s_Instance.Finalize();
    CChrMenuCtrl::s_Instance.Finalize();
    menu::CSndEff::s_Instance.Finalize();
    menu::CSndBGM::s_Instance.Finalize();
    menu::CVibEff::s_Instance.Finalize();
    CCockpit::RemovePointerObj();
    menu::CSndBGM &rBGM = menu::CSndBGM::s_Instance;
    if (rBGM.GetTaskId() != 0) {
        rBGM.Stop(rBGM.GetTaskId(), 0.0f);
    }
    bcs::G2D::CMenuMdlScnGroup::SetDefaultPersProjInfo();
    CCameraMenuBG::s_Instance.Finalize();
    CDepthOfFieldValue::s_Instance.Finalize();
    CNowLoading::SetDisp(0);
    for (long i = 0; i < ARCHIVE_NUM; i++) {
        UnLoad(i);
    }
}

// One frame: the screen change (with the archive it needs), the current
// screen's step, the menu objects, the camera and the friend match check.
void msw::CSelectMenu::Main()
{
    if (WifiErrorCheck() == 0) {
        return;
    }
    if (CGamePadCheck::m_PadCheckStap >= 2 && dwc::CGameDwc::GetInstance() == 0) {
        return;
    }

    DecrementDelayFrameCount();
    menu::CSndBGM::s_Instance.MainLoop();
    menu::CVibEff::s_Instance.MainLoop();
    PadStateCheck();

    if ((m_Unk98 != 0) == 1) {
        float fWait = m_fUnk94;
        fWait -= bcs::CMainApp::GetAppFrameRateF();
        if (fWait <= 0.0f) {
            fWait = 0.0f;
            if (IsComplete(s_LoadArchiveFileId[m_nNextMode])) {
                unsigned char nArchive = s_LoadArchiveFileId[m_nState];
                m_Unk98 = 0;
                if (s_LoadArchiveFileId[m_nNextMode] != nArchive) {
                    UnLoad(nArchive);
                }
                long nNext = m_nNextMode;
                long nPad = 0;
                long nState = m_nState;
                int bPush = 1;
                if (nNext == 0x11) {
                    nPad = m_Unk27D;
                } else {
                    for (long i = 0; i < 4; i++) {
                        if (m_nCurrentBtnKindId[i][nState] == 1 && nNext != 0x2A) {
                            bPush = 0;
                        }
                    }
                }
                if (bPush == 1) {
                    PushMenuHistory(nState, nNext, m_nCurrentBtnKindId[nPad][nState]);
                }
                for (long i = 0; i < 4; i++) {
                    unsigned short nBtnKindId = m_nCurrentBtnKindId[i][m_nState];
                    if (nBtnKindId > 1) {
                        m_HistoryBtnKindId[(unsigned char)nState].m_nPad[i] = nBtnKindId;
                    }
                    m_aPadSelect[i].m_Unk02 = 1;
                    m_nCurrentBtnKindId[i][nNext] = m_HistoryBtnKindId[(unsigned char)nNext].m_nPad[i];
                }
                m_Unk9D = nState;
                ChangeState(nNext);
            } else if (!IsReading(s_LoadArchiveFileId[m_nNextMode])) {
                unsigned char nArchive = s_LoadArchiveFileId[m_nNextMode];
                if (nArchive != s_LoadArchiveFileId[m_nState]) {
                    Load(nArchive);
                }
            }
        } else {
            unsigned char nArchive = s_LoadArchiveFileId[m_nNextMode];
            if (nArchive != s_LoadArchiveFileId[m_nState] && !IsLoaded(nArchive)) {
                Load(s_LoadArchiveFileId[m_nNextMode]);
            }
        }
        m_fUnk94 = fWait;
    }
    if (m_Unk98 == 0) {
        ExecState();
    }
    ModListMainLoop();
    CCameraMenuBG::s_Instance.MainLoop();
    CDepthOfFieldValue::s_Instance.Main();
    WifiFriendMatchChallenger();
}

// The end of the select menu: the screen fades out and the state machine
// waits in screen 0x32 while SelectEnd_Out starts the game.
void msw::CSelectMenu::SelectEnd_In()
{
}

void msw::CSelectMenu::SelectEnd_Loop()
{
    m_pPlayerData = CSave::GetPlayerData(-1);
    Scratch(1);
    float fWait = 60.0f;
    if (m_Unk9B != 1) {
        if (CEventDemo::m_bDemonstration) {
            SetBlockFadeOut(20.0f);
        } else {
            SetWhiteFadeOut(fWait);
        }
    }
    SetNextMode(0x32, fWait);
}

namespace {
    // Whether play mode nPlayMode (CSelectMenu::m_nPlayMode) lets the
    // players change characters; for the others both teams' character
    // change is switched off.
    inline int IsChrChangePlayMode(unsigned char nPlayMode)
    {
        switch (nPlayMode) {
            case 0:
            case 1:
            case 4:
            case 5:
            case 7:
            case 10:
            case 15:
            case 16:
            case 17:
            case 19:
            case 20:
            case 21:
            case 24:
            case 25:
                return 1;
        }
        return 0;
    }

    // The CMainStep step that plays game kind nGameKind.
    inline long GetGameStep(long nGameKind)
    {
        long nStep;
        switch (nGameKind) {
            default:
                nStep = 5;
                break;
            case 1:
                nStep = 6;
                break;
            case 2:
                nStep = 7;
                break;
            case 3:
                nStep = 8;
                break;
        }
        return nStep;
    }

    // nSec seconds in frames, as a float.
    inline float SecToFrameF(unsigned short nSec)
    {
        unsigned short nFrame = nSec * 60;
        return U16ToF32(&nFrame);
    }
}

// Sets the game up from the menus' choices and ends the select menu: the
// game mode, type, stage and characters (with their names and Miis), the
// rules and computer levels, the step CMenu goes to and the pads. A
// tournament that is only being continued (m_UnkA00) goes straight on.
void msw::CSelectMenu::SelectEnd_Out()
{
    // The stage whose party game runs without rule option 2.
    static const unsigned char stageCoronaMt = 0x56;

    CMenu *pMenu = (CMenu *)GetParent();
    if ((long)m_Unk9B == 1 && m_UnkA00 == 1) {
        pMenu->SetNextStep(3);
        Backup();
        Delete();
        return;
    }

    CGame::SetDefaultParam();
    if (m_nGameKind == 5) {
        CGame::SetGameMode(CGame::GAME_MODE_BASKET);
    } else {
        CGame::SetGameMode((CGame::GAME_MODE)m_nGameKind);
    }
    unsigned char nPlayMode = m_nPlayMode;
    CGame::SetPlayType((CGame::GAME_PLAY_TYPE)nPlayMode);

    long nGameType;
    switch (m_Unk9B) {
        case 3:
            nGameType = 1;
            break;
        case 2:
            switch (m_Unk9C) {
                case 3:
                    nGameType = 2;
                    break;
                case 1:
                    nGameType = 2;
                    break;
                case 7:
                    nGameType = 2;
                    break;
                case 5:
                    nGameType = 2;
                    break;
                default:
                    nGameType = 3;
                    break;
            }
            break;
        case 1:
            nGameType = 0;
            if (m_nGameKind == 5) {
                CGame::SetSportsMode(CGame::SPORTS_MODE_MIX);
            }
            break;
        default:
            nGameType = 0;
            break;
    }
    CGame::SetGameType((CGame::GAME_TYPE)nGameType);

    long nStage;
    switch (m_Unk9B) {
        case 1:
            nStage = m_nStage + 0x1F;
            break;
        case 3:
            nStage = 0x14;
            break;
        case 2: {
            SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(0x31);
            if (pCourt != 0) {
                nStage = pCourt->m_nStageId[m_nStage][m_nStageSub];
            } else {
                switch (m_Unk9C) {
                    case 3:
                        nStage = 0x28;
                        break;
                    case 1:
                        nStage = 0x46;
                        break;
                    case 7:
                        nStage = 0x55;
                        break;
                    default:
                        nStage = 0;
                        break;
                }
            }
            break;
        }
        default:
            nStage = _GetStageNoInfo(m_nGameKind, m_nStage)[m_nStageSub];
            break;
    }
    CGame::SetStage(nStage);

    // The stage is no longer new.
    if (m_pPlayerData != 0) {
        SetUpCupInfo *pInfo = _GetStageInfo(m_Unk9B, m_nGameKind, m_Unk9C);
        if (m_nGameKind == 5) {
            pInfo->m_nStageNewBits[3] &= (unsigned char)~(unsigned char)(1 << m_nStageSub);
        } else {
            pInfo->m_nStageNewBits[m_nStage] &= (unsigned char)~(unsigned char)(1 << m_nStageSub);
        }
    }

    // Both Wi-Fi players draw the same random numbers.
    if ((long)m_Unk9B == 4) {
        const WiFiSingleData *pData = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(0);
        bcs::BCS_SRand(pData->m_nSeed);
    }
    CGame::SetStageParam(bcs::BCS_Rand());
    CGame::SetPadAssignSemiAuto();
    for (long i = 0; i < 8; i++) {
        CGame::SetCharAssignPlayerNo(i, CGame::GetCharAssignPadNo(i));
    }

    // The characters.
    long nPlayerNum = 0;
    switch (m_Unk9B) {
        case 0:
        case 3:
            for (long i = 0; i < 6; i++) {
                long nChara = m_aCharaSaveParam[i].m_nChara;
                long nColor = m_aCharaSaveParam[i].m_nCharaSub;
                unsigned char nExFlag = m_aCharaSaveParam[i].m_Unk13[0];
                if (nChara >= 0x19) {
                    continue;
                }
                if (nExFlag != 0) {
                    CGame::AddCharExFlag(i, 4);
                } else {
                    CGame::ResetCharExFlag(i, 4);
                }
                if (nChara < 0x13) {
                    if (nColor == 0xFF) {
                        nColor = 0;
                    }
                    CGame::SetCharType(i, nChara);
                    CGame::SetCharColor(i, nColor);
                    long nMsg = s_CharaNameMsg[nChara][nColor];
                    CGame::SetCharName(i, GetMessageString("List_Character", nMsg), 0x20);
                } else {
                    RFLStoreData data;
                    long nType;
                    if (RFLGetStoreData(&data, m_aCharaSaveParam[i].m_MiiSource, m_aCharaSaveParam[i].m_nMiiIndex) == RFLErrcode_Success
                        && (nType = CGame::GetMiiChrType(data)) != -1) {
                        CGame::SetMiiStoreData(i, data);
                        CGame::SetCharType(i, nType);
                        CGame::SetCharColor(i, nColor);
                        CGame::SetCharName(i, GetMiiName(data), 0x20);
                    } else {
                        CGame::SetCharType(i, -1);
                        CGame::SetCharColor(i, 0);
                    }
                }
            }
            break;
        case 2:
            for (long i = 0; i < 6; i++) {
                long nChara = m_aCharaSaveParam[i].m_nChara;
                long nColor = m_aCharaSaveParam[i].m_nCharaSub;
                unsigned char nExFlag = m_aCharaSaveParam[i].m_Unk13[0];
                if (nChara >= 0x19) {
                    continue;
                }
                if (nExFlag != 0) {
                    CGame::AddCharExFlag(i, 4);
                } else {
                    CGame::ResetCharExFlag(i, 4);
                }
                long nChr = s_TeamOrder[m_Unk9C][i];
                if (nChara < 0x13) {
                    if (nColor == 0xFF) {
                        nColor = 0;
                    }
                    if (nChara > 0x19) {
                        nChara = -1;
                    }
                    if (nColor > 6) {
                        nColor = 0;
                    }
                    CGame::SetCharType(nChr, nChara);
                    CGame::SetCharColor(nChr, nColor);
                    long nMsg = s_CharaNameMsg[nChara][nColor];
                    CGame::SetCharName(nChr, GetMessageString("List_Character", nMsg), 0x20);
                } else {
                    RFLStoreData data;
                    long nType;
                    if (RFLGetStoreData(&data, m_aCharaSaveParam[i].m_MiiSource, m_aCharaSaveParam[i].m_nMiiIndex) == RFLErrcode_Success
                        && (nType = CGame::GetMiiChrType(data)) != -1) {
                        CGame::SetMiiStoreData(nChr, data);
                        CGame::SetCharType(nChr, nType);
                        CGame::SetCharColor(nChr, nColor);
                        CGame::SetCharName(nChr, GetMiiName(data), 0x20);
                    } else {
                        CGame::SetCharType(nChr, -1);
                        CGame::SetCharColor(nChr, 0);
                    }
                }
            }
            break;
        case 1: {
            CTournament::Initialize(1);
            CTournament::SetGameMode(m_nGameKind);
            char nLevel;
            switch (m_nStage) {
                case 0:
                    nLevel = 0;
                    break;
                case 1:
                    nLevel = 1;
                    break;
                case 2:
                    nLevel = 2;
                    break;
                case 3:
                    nLevel = 3;
                    break;
                default:
                    nLevel = 0;
                    break;
            }
            CTournament::SetLevel(nLevel);
            CTournament::SetCup(m_nStageSub + 1);
            CTournament::SetState(0);
            // One player per team: slots 0, 2 and 4.
            for (long i = 0; i < 6; i += 2, nPlayerNum++) {
                long nChara = m_aCharaSaveParam[i].m_nChara;
                long nColor = m_aCharaSaveParam[i].m_nCharaSub;
                unsigned char nExFlag = m_aCharaSaveParam[i].m_Unk13[0];
                if (nChara >= 0x19) {
                    continue;
                }
                if (nExFlag != 0) {
                    CGame::AddCharExFlag(i, 4);
                } else {
                    CGame::ResetCharExFlag(i, 4);
                }
                if (nChara < 0x13) {
                    if (nColor == 0xFF) {
                        nColor = 0;
                    }
                    CTournament::SetPlayer(nPlayerNum, nChara, nColor, 0);
                    long nMsg = s_CharaNameMsg[nChara][nColor];
                    CTournament::SetPlayerName(nPlayerNum, GetMessageString("List_Character", nMsg), 0x20);
                } else {
                    RFLStoreData data;
                    long nType;
                    if (RFLGetStoreData(&data, m_aCharaSaveParam[i].m_MiiSource, m_aCharaSaveParam[i].m_nMiiIndex) == RFLErrcode_Success
                        && (nType = CGame::GetMiiChrType(data)) != -1) {
                        CTournament::SetPlayerMiiStoreData(nPlayerNum, data);
                        CTournament::SetPlayer(nPlayerNum, nType, nColor, 0);
                        CTournament::SetPlayerName(nPlayerNum, GetMiiName(data), 0x20);
                    } else {
                        CGame::SetCharType(nPlayerNum, -1);
                        CGame::SetCharColor(nPlayerNum, 0);
                    }
                }
            }
            CTournament::SetPlayType(nPlayMode);
            CTournament::Setup(-1);
            break;
        }
        case 4: {
            // Wi-Fi: characters 0, 2 and 4 are player 0's, 1, 3 and 5
            // the other side's.
            m_nWifiMiiChara[0] = -1;
            m_nWifiMiiChara[1] = -1;
            CGame::g_GameWork.m_Unk151 = 1;
            const WiFiSingleData *pData0 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(0);
            const WiFiSingleData *pData1 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(1);
            for (long i = 0; i < 6; i++) {
                long nAid = i & 1;
                const WiFiSingleData *pData = nAid != 0 ? pData1 : pData0;
                long nSlot = i >> 1;
                if (pData->m_nChara[nSlot] >= 0x19) {
                    continue;
                }
                if (pData->m_nCharaUnk[nSlot] != 0) {
                    CGame::AddCharExFlag(i, 4);
                } else {
                    CGame::ResetCharExFlag(i, 4);
                }
                if (nAid == m_pGameDwc->m_Dwc.GetMyAID()) {
                    m_pPlayerData->GetCharInfo(m_nGameKind, pData->m_nChara[nSlot])->m_nNew = 0;
                }
                if (pData->m_nChara[nSlot] < 0x13) {
                    unsigned long nColor = pData->m_nCharaSub[nSlot];
                    if (nColor == 0xFF) {
                        nColor = 0;
                    }
                    CGame::SetCharType(i, pData->m_nChara[nSlot]);
                    CGame::SetCharColor(i, nColor);
                    long nMsg = s_CharaNameMsg[pData->m_nChara[nSlot]][nColor];
                    CGame::SetCharName(i, GetMessageString("List_Character", nMsg), 0x20);
                } else {
                    RFLStoreData data = pData->m_CharaMii[nSlot];
                    if (CGame::GetMiiChrType(data) != -1) {
                        WiFiMiiCharSetName(&data, i);
                        RFLAdditionalInfo info;
                        bcs::CMiiMan::GetAdditionalInfo(info, data);
                        CGame::SetMiiStoreData(i, data);
                        if (info.sex) {
                            CGame::SetCharType(i, 0x14);
                        } else {
                            CGame::SetCharType(i, 0x13);
                        }
                        CGame::SetCharColor(i, 0);
                    } else {
                        CGame::SetCharType(i, -1);
                        CGame::SetCharColor(i, 0);
                    }
                }
            }
            if (m_pPlayerData->m_nOtherMii != 0 && m_bStarMii == 0) {
                WiFiExChangeMii();
            }
            CGame::SetCharType(6, -1);
            CGame::SetCharType(7, -1);
            CGame::SetMyTeamIdx(m_pGameDwc->m_Dwc.GetMyAID() & 1);
            CGame::g_GameWork.m_Unk151 = 1;
            m_pGameDwc->m_UnkDCC = 1;
            m_pGameDwc->m_UnkDC8 = 1;
            break;
        }
    }

    // The characters played are no longer new.
    if ((long)m_Unk9B != 4 && m_nGameKind != 5 && m_pPlayerData != 0) {
        if ((long)m_Unk9B == 1) {
            for (long i = 0; i < nPlayerNum; i++) {
                m_pPlayerData->GetCharInfo(m_nGameKind, CTournament::GetPlayerType(i))->m_nNew = 0;
            }
        } else {
            for (long i = 0; i < 6; i++) {
                m_pPlayerData->GetCharInfo(m_nGameKind, CGame::GetCharType(i))->m_nNew = 0;
            }
        }
    }

    // The rules.
    CGame::g_GameWork.m_nUnk180 = 0;
    CGame::SetParamSemiAuto();
    if (m_pPlayerData != 0) {
        CMenuOption::s_Instance.GetPlayerSaveData(CSave::GetUsePlayerID(), m_pPlayerData->IsDifficult(m_nGameKind));
    }
    switch (m_Unk9B) {
        case 4: {
            // Wi-Fi: the default rules, the computers on level 6.
            CMenuOption &rOption = CMenuOption::s_Instance;
            long nKind = RuleSel_GetOptionKindId();
            long nNum = rOption.GetMenuNum(nKind);
            rOption.AllocTemporary();
            rOption.BackupTemporary();
            for (long i = 0; i < nNum; i++) {
                rOption.SetMenuParameterId(nKind, i, CMenuOption::GetMenuDefaultId(nKind, i, 2));
            }
            rOption.SetGameParameter(nKind);
            rOption.RestoreTemporary();
            rOption.FreeTemporary();
            CGame::SetTeamComLevel(0, 6.0f);
            CGame::SetTeamComLevel(1, 6.0f);
            CGame::SetQPanelLevel((CGame::QPANEL_LEVEL)3);

            long nAid = m_pGameDwc->m_Dwc.GetMyAID();
            const WiFiSingleData *pData = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(nAid);
            if (pData->m_nChrChangeFlags & (1 << m_nGameKind)) {
                CGame::SetTeamChrChangeCtrlType(nAid, 1);
            } else {
                CGame::SetTeamChrChangeCtrlType(nAid, 0);
            }
            if (m_nGameKind == 1) {
                if (pData->m_Unk1EA == 0) {
                    CGame::SetTeamAutoSpikeJumpType(nAid, 1);
                } else {
                    CGame::SetTeamAutoSpikeJumpType(nAid, 0);
                }
            }
            nAid = m_pGameDwc->m_Dwc.GetEmAID();
            pData = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(nAid);
            if (pData->m_nChrChangeFlags & (1 << m_nGameKind)) {
                CGame::SetTeamChrChangeCtrlType(nAid, 1);
            } else {
                CGame::SetTeamChrChangeCtrlType(nAid, 0);
            }
            if (m_nGameKind == 1) {
                if (pData->m_Unk1EA == 0) {
                    CGame::SetTeamAutoSpikeJumpType(nAid, 1);
                } else {
                    CGame::SetTeamAutoSpikeJumpType(nAid, 0);
                }
            } else if (m_nGameKind == 2) {
                CGame::SetMaxShotTime(SecToFrameF(20));
            }
            CGame::EnableRuleOptionFlag(0x100, 0);
            CGame::EnableRuleOptionFlag(0x200, 0);
            CGame::SetGameLevel(CGame::GAME_LEVEL_HARD);
            break;
        }
        case 2: {
            long nParam = _GetPartyGameStageParam(m_Unk9C, m_nStage, m_nStageSub);
            switch (m_Unk9C) {
                case 3:
                    CGame::SetPartyGameParam(0, nParam);
                    CGame::EnableRuleOptionFlag(2, 1);
                    break;
                case 7:
                    CGame::SetPartyGameParam(0, nParam);
                    CGame::EnableRuleOptionFlag(2, nStage != stageCoronaMt);
                    break;
                case 1:
                    CGame::SetPartyGameParam(0, nParam);
                    CGame::EnableRuleOptionFlag(2, 1);
                    break;
                case 5:
                    CGame::SetPartyGameParam(0, nParam);
                    break;
                default:
                    CGame::EnableRuleOptionFlag(2, 1);
                    break;
            }
            CMenuOption::s_Instance.SetGameParameter(RuleSel_GetOptionKindId());
            CGame::SetGameLevel(CGame::GAME_LEVEL_HARD);
            break;
        }
        case 1:
        case 3:
            CMenuOption::s_Instance.SetGameParameter(RuleSel_GetOptionKindId());
            if ((long)m_nPlayerNum != 1) {
                CGame::g_GameWork.m_nUnk1C = 0;
                CGame::SetTeamChrChangeCtrlType(0, 0);
                CGame::SetTeamChrChangeCtrlType(1, 0);
            }
            if (m_pPlayerData->IsFirstPlay(m_Unk9B, m_nGameKind) != 0
                && m_pPlayerData->IsDifficult(m_nGameKind) == 0) {
                m_pPlayerData->SetDifficult(m_nGameKind, 1);
            }
            break;
        default:
            if (CEventDemo::m_bDemonstration) {
                // The attract demo: the default rules, computers on 5.5.
                CMenuOption &rOption = CMenuOption::s_Instance;
                long nKind = RuleSel_GetOptionKindId();
                long nNum = rOption.GetMenuNum(nKind);
                rOption.AllocTemporary();
                rOption.BackupTemporary();
                for (long i = 0; i < nNum; i++) {
                    rOption.SetMenuParameterId(nKind, i, CMenuOption::GetMenuDefaultId(nKind, i, 1));
                }
                rOption.SetGameParameter(nKind);
                rOption.RestoreTemporary();
                rOption.FreeTemporary();
                CGame::SetTeamComLevel(0, 5.5f);
                CGame::SetTeamComLevel(1, 5.5f);
                CGame::EnableRuleOptionFlag(0x100, 0);
                CGame::EnableRuleOptionFlag(0x200, 0);
                CGame::g_GameWork.m_nUnk180 = 1;
            } else {
                CMenuOption::s_Instance.SetGameParameter(RuleSel_GetOptionKindId());
                if (!IsChrChangePlayMode(m_nPlayMode)) {
                    CGame::SetTeamChrChangeCtrlType(0, 0);
                    CGame::SetTeamChrChangeCtrlType(1, 0);
                }
                switch (m_pPlayerData->IsDifficult(m_nGameKind)) {
                    case 0:
                        CGame::SetTeamComLevel(0, 3.0f);
                        CGame::SetTeamComLevel(1, 3.0f);
                        if (m_pPlayerData->IsFirstPlay(m_Unk9B, m_nGameKind) != 0) {
                            m_pPlayerData->SetDifficult(m_nGameKind, 1);
                            if (CGame::g_GameWork.m_nQPanelLevel < 2) {
                                CGame::SetQPanelLevel((CGame::QPANEL_LEVEL)0);
                            }
                        }
                        CGame::SetGameLevel(CGame::GAME_LEVEL_EASY);
                        break;
                    case 1:
                        CGame::SetTeamComLevel(0, 4.0f);
                        CGame::SetTeamComLevel(1, 4.0f);
                        CGame::EnableRuleOptionFlag(0x200, 0);
                        CGame::SetGameLevel(CGame::GAME_LEVEL_NORMAL);
                        break;
                    case 2:
                        CGame::SetTeamComLevel(0, 5.5f);
                        CGame::SetTeamComLevel(1, 5.5f);
                        CGame::EnableRuleOptionFlag(0x200, 0);
                        CGame::SetGameLevel(CGame::GAME_LEVEL_HARD);
                        break;
                    case 3:
                        CGame::SetTeamComLevel(0, 7.0f);
                        CGame::SetTeamComLevel(1, 7.0f);
                        CGame::EnableRuleOptionFlag(0x200, 0);
                        CGame::SetGameLevel(CGame::GAME_LEVEL_EXPERT);
                        break;
                }
            }
            break;
    }

    // Where CMenu goes next: the tournament, or the game itself.
    switch (m_Unk9B) {
        case 0:
        case 4:
            pMenu->SetNextStep(GetGameStep(m_nGameKind));
            break;
        case 2:
        case 3:
            pMenu->SetNextStep(GetGameStep(m_nGameKind));
            break;
        case 1:
            pMenu->SetNextStep(3);
            break;
    }

    // The pads: a nunchaku (1) unless the pad's style is not 0.
    if ((long)m_Unk9B == 4) {
        long nNum = bcs::CWifiPadBase::GetOnlinePadTotal();
        for (long i = 0; i < nNum; i++) {
            long nAid = bcs::CWifiPadBase::GetAIDFromOnlinePadID(i);
            long nPad = bcs::CWifiPadBase::GetLocalPadIDFromOnlinePadID(i);
            const WiFiSingleData *pData = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(nAid);
            if (pData->m_nPadStyle[nPad] == 0) {
                CGame::SetPadDefaultType(i, 1);
            } else {
                CGame::SetPadDefaultType(i, 0);
            }
        }
    } else {
        for (long i = 0; i < 4; i++) {
            if (CMenuPad::s_Instance.GetPadStyle(i) == 0) {
                CGame::SetPadDefaultType(i, 1);
            } else {
                CGame::SetPadDefaultType(i, 0);
            }
        }
    }

    Backup();
    dwc::CGameDwc *pGameDwc = dwc::CGameDwc::GetInstance();
    if (pGameDwc != 0) {
        pGameDwc->m_UnkDD4 = 1;
    }
    CGamePadCheck::SetPadCheckDisable(0);
    CGamePadCheck::GetInstance()->RemoveControllerTypeErrorMessage();
    Delete();
}

// A random guest Mii (0 to 2 for a male Mii, 3 to 5 for a female one) to
// stand in for data, -1 when data is empty.
long msw::CSelectMenu::WiFiMiiSexRand(RFLStoreData data)
{
    unsigned char nAny = 0;
    for (long i = 0; i < sizeof(data.data); i++) {
        nAny |= data.data[i];
    }
    if (nAny != 0) {
        RFLAdditionalInfo info;
        bcs::CMiiMan::GetAdditionalInfo(info, data);
        if (info.sex == 0) {
            return bcs::BCS_RandRange(0L, 2L);
        }
        return bcs::BCS_RandRange(3L, 5L);
    }
    return -1;
}

// Gives character nChara the name of whichever Wi-Fi player's Mii *pData
// is.
void msw::CSelectMenu::WiFiMiiCharSetName(RFLStoreData *pData, long nChara)
{
    dwc::CGameDwc::PlayerInfo &rOther = m_pGameDwc->m_PlayerInfo[m_pGameDwc->m_Dwc.GetEmAID()];
    RFLStoreData data = rOther.m_StoreData;
    RFLStoreData data2P = rOther.m_StoreData2P;
    if (memcmp(&data, pData, sizeof(RFLStoreData)) == 0) {
        CGame::SetCharName(nChara, rOther.m_Name, 0x20);
        m_nWifiMiiChara[0] = nChara;
        return;
    }
    if (memcmp(&data2P, pData, sizeof(RFLStoreData)) == 0) {
        CGame::SetCharName(nChara, rOther.m_Name2P, 0x20);
        m_nWifiMiiChara[1] = nChara;
        return;
    }

    dwc::CGameDwc::PlayerInfo &rMine = m_pGameDwc->m_PlayerInfo[m_pGameDwc->m_Dwc.GetMyAID()];
    RFLStoreData myData = rMine.m_StoreData;
    RFLStoreData myData2P = rMine.m_StoreData2P;
    if (memcmp(&myData, pData, sizeof(RFLStoreData)) == 0) {
        CGame::SetCharName(nChara, rMine.m_Name, 0x20);
        return;
    }
    if (memcmp(&myData2P, pData, sizeof(RFLStoreData)) == 0) {
        CGame::SetCharName(nChara, rMine.m_Name2P, 0x20);
    }
}

// The opponent's Miis that are not guest Miis are replaced with guest
// Miis of the same sex (with their names) for the game.
void msw::CSelectMenu::WiFiExChangeMii()
{
    long nOther = m_pGameDwc->m_Dwc.GetEmAID();
    RFLStoreData data = m_pGameDwc->m_PlayerInfo[nOther].m_StoreData;
    if (!CheckGuestMii(data)) {
        long nGuest = WiFiMiiSexRand(data);
        if (nGuest >= 0) {
            RFLStoreData guest;
            bcs::CMiiMan::GetStoreData(guest, RFLDataSource_Default, nGuest);
            m_pGameDwc->m_PlayerInfo[nOther].m_StoreData = guest;
            m_pGameDwc->SetPlayerName(nOther, GetMessageString("List_Mii_Name", nGuest));
            if (m_nWifiMiiChara[0] >= 0) {
                CGame::SetMiiStoreData(m_nWifiMiiChara[0], guest);
            }
        }
    }

    RFLStoreData data2P = m_pGameDwc->m_PlayerInfo[nOther].m_StoreData2P;
    if (!CheckGuestMii(data2P)) {
        long nGuest = WiFiMiiSexRand(data2P);
        if (nGuest >= 0) {
            RFLStoreData guest;
            bcs::CMiiMan::GetStoreData(guest, RFLDataSource_Default, nGuest);
            m_pGameDwc->m_PlayerInfo[nOther].m_StoreData2P = guest;
            m_pGameDwc->SetPlayerName2P(nOther, GetMessageString("List_Mii_Name", nGuest));
            if (m_nWifiMiiChara[1] >= 0) {
                CGame::SetMiiStoreData(m_nWifiMiiChara[1], guest);
            }
        }
    }
}

// Whether data is one of the six guest Miis.
int msw::CSelectMenu::CheckGuestMii(RFLStoreData data)
{
    for (long i = 0; i < 6; i++) {
        RFLStoreData guest;
        bcs::CMiiMan::GetStoreData(guest, RFLDataSource_Default, i);
        if (memcmp(&data, &guest, sizeof(RFLStoreData)) == 0) {
            return 1;
        }
    }
    return 0;
}

// Puts the rules of the current game back to their defaults for
// difficulty nLevel.
void msw::CSelectMenu::SetDefaultOption(long nLevel)
{
    long nKind = RuleSel_GetOptionKindId();
    CMenuOption &rOption = CMenuOption::s_Instance;
    long nNum = rOption.GetMenuNum(nKind);
    rOption.AllocTemporary();
    rOption.BackupTemporary();
    for (long i = 0; i < nNum; i++) {
        rOption.SetMenuParameterId(nKind, i, CMenuOption::GetMenuDefaultId(nKind, i, nLevel));
    }
    rOption.SetGameParameter(nKind);
    rOption.RestoreTemporary();
    rOption.FreeTemporary();
}

// The game's default controls for each pad: type 1 for a remote alone.
void msw::CSelectMenu::SetPadDefault()
{
    for (long i = 0; i < 4; i++) {
        if (CMenuPad::s_Instance.GetPadStyle(i) == 0) {
            CGame::SetPadDefaultType(i, 1);
        } else {
            CGame::SetPadDefaultType(i, 0);
        }
    }
}

// The message windows of the menus (cockpit object 0x101A, made in the
// cockpit task): the texts come from message pLabel of group pGroup and
// the labels after it.
void msw::CSelectMenu::WindowYesNo_Button(char *pGroup, char *pLabel, int bNoFirst, wchar_t *pText0, wchar_t *pText1,
                                          int nUnk)
{
    static const long objId = 0x101A;

    wchar_t aText[0x100];
    SendObjEventTrigger(0x3F, 0x31, (void *)0x78);
    CCockpit *pCockpit = CCockpit::GetTask();
    CCockpitMenuWinBtn2 *pWin = new CCockpitMenuWinBtn2(objId, 0.0f, 1);
    pCockpit->AddObj(pWin);
    short nLabel = GetMessageLabelIndex(pGroup, pLabel);
    pWin->m_Unk90 = nUnk;
    pWin->SetMenuResultId(0, 1000);
    pWin->SetMenuResultId(1, 1001);
    pWin->SetButtonText(0, GetMessageString(pGroup, nLabel + 1));
    pWin->SetButtonText(1, GetMessageString(pGroup, nLabel + 2));
    swprintf(aText, 0xFF, GetMessageString(pGroup, nLabel), pText0, pText1);
    pWin->SetWindowText(0, aText);
    pWin->SetButtonSECodeArray(0, 0xBBB, 0xBC0);
    pWin->SetButtonSECodeArray(0, 0xBBC, 0xBC0);
    if (bNoFirst != 0) {
        pWin->SetDefaultCursorFlag(0);
    } else {
        pWin->SetDefaultCursorFlag(1);
    }
    pWin->Setup();
    m_pWindow = pWin;
}

void msw::CSelectMenu::WindowYesNo_Close()
{
    if (m_pWindow != 0) {
        if (!CCockpit::IsEndObj(0x101A)) {
            CCockpitMenuWinBtn2 *pWin = static_cast<CCockpitMenuWinBtn2 *>(m_pWindow);
            pWin->m_Unk90 = 1;
            pWin->MenuOut();
        }
        m_pWindow = 0;
    }
}

void msw::CSelectMenu::WindowOK_Button(char *pGroup, char *pLabel, wchar_t *pText)
{
    static const long objId = 0x101A;

    wchar_t aText[0x100];
    SendObjEventTrigger(0x3F, 0x31, (void *)0x78);
    CCockpit *pCockpit = CCockpit::GetTask();
    CCockpitMenuWinBtn1 *pWin = new CCockpitMenuWinBtn1(objId, 0.0f, 1);
    pCockpit->AddObj(pWin);
    short nLabel = GetMessageLabelIndex(pGroup, pLabel);
    pWin->SetMenuResultId(0, 1000);
    pWin->SetButtonText(0, GetMessageString(pGroup, nLabel + 1));
    swprintf(aText, 0xFF, GetMessageString(pGroup, nLabel), pText);
    pWin->SetWindowText(0, aText);
    pWin->SetWindowFontSize(0, 1);
    pWin->SetButtonSECodeArray(0, 0xBBB, 0xBC0);
    pWin->m_Unk88 = 0;
    pWin->SetDefaultCursorFlag(0);
    pWin->Setup();
    m_pWindow = pWin;
}

void msw::CSelectMenu::WindowOK_Close()
{
    if (m_pWindow != 0) {
        if (!CCockpit::IsEndObj(0x101A)) {
            static_cast<CCockpitMenuWinBtn1 *>(m_pWindow)->MenuOut();
        }
        m_pWindow = 0;
    }
}

void msw::CSelectMenu::WindowA_Button(char *pGroup, char *pLabel, wchar_t *pText0, wchar_t *pText1)
{
    static const long objId = 0x101A;

    wchar_t aText[0x100];
    SendObjEventTrigger(0x3F, 0x31, (void *)0x78);
    CCockpit *pCockpit = CCockpit::GetTask();
    CCockpitMenuWinBtnA *pWin = new CCockpitMenuWinBtnA(objId, 0.0f, 1);
    pCockpit->AddObj(pWin);
    short nLabel = GetMessageLabelIndex(pGroup, pLabel);
    pWin->SetMenuResultId(0, 1000);
    swprintf(aText, 0xFF, GetMessageString(pGroup, nLabel), pText0, pText1);
    pWin->SetWindowText(0, aText);
    pWin->SetButtonSECodeArray(0, 0xBBB, 0xBC0);
    pWin->m_Unk88 = 0;
    pWin->Setup();
    m_pWindow = pWin;
}

void msw::CSelectMenu::WindowA_Close()
{
    if (m_pWindow != 0) {
        if (!CCockpit::IsEndObj(0x101A)) {
            static_cast<CCockpitMenuWinBtnA *>(m_pWindow)->MenuOut();
        }
        m_pWindow = 0;
    }
}

// A window without a button; bWait adds the waiting mark (cockpit object
// 0x103F).
void msw::CSelectMenu::Window_Message(char *pGroup, char *pLabel, int bWait)
{
    static const long objId = 0x101A;

    CCockpit *pCockpit = CCockpit::GetTask();
    CCockpitMenuWinBtn0 *pWin = new CCockpitMenuWinBtn0(objId, 0.0f, 0);
    pCockpit->AddObj(pWin);
    pWin->SetWindowText(0, GetMessageLabelString(pGroup, pLabel));
    pWin->m_Unk88 = 0;
    pWin->Setup();
    if (bWait != 0) {
        CCockpit::CreateObj(0x103F, 0);
    }
    m_pWindow = pWin;
}

void msw::CSelectMenu::Window_Close()
{
    if (m_pWindow != 0) {
        if (!CCockpit::IsEndObj(0x101A)) {
            static_cast<CCockpitMenuWinBtn0 *>(m_pWindow)->MenuOut();
        }
        CCockpit::RemoveObj(0x103F);
        m_pWindow = 0;
    }
}

wchar_t *msw::CSelectMenu::ExchangeFriendCode(unsigned long long nCode)
{
    swprintf(m_MessageWork, 0x10, L"%04d-%04d-%04d", (unsigned long)(nCode / 10000 / 10000 % 10000),
             (unsigned long)(nCode / 10000 % 10000), (unsigned long)(nCode % 10000));
    return m_MessageWork;
}

// While the Wi-Fi connection reports an error the menu stops (0): the
// screen goes back through the history to the title, or the game returns
// to the start of the Wi-Fi menus.
long msw::CSelectMenu::WifiErrorCheck()
{
    if (dwc::CGameDwc::m_ErrorStep == 0) {
        return 1;
    }
    CMenuLinkBase *pObj = FindObj(0xFF);
    if (pObj != 0) {
        m_LinkList.RemoveObject(pObj);
    }
    if (dwc::CGameDwc::m_ErrorStep == 10) {
        ClearMenuHistory();
        PushMenuHistory(1, 2, 0);
        PushMenuHistory(2, 4, 0);
        if (dwc::CGameDwc::GetErrorType() == -2 || dwc::CGameDwc::GetInstance() == 0) {
            Backup();
            menu::CStateSave::GetInstance()->SetStartMenuMode(2);
            CMainStep::ChangeStep(2);
            CGame::SetReturnStep(0);
        } else if (dwc::CGameDwc::GetErrorType() == -1) {
            Backup();
            menu::CStateSave::GetInstance()->SetStartMenuMode(8);
            CMainStep::ChangeStep(2);
            CGame::SetReturnStep(0);
            CancelVsFriend();
        }
    }
    return 0;
}

// Every three seconds: looks for a friend who wants a match.
void msw::CSelectMenu::WifiFriendMatchChallenger()
{
    if (CGamePadCheck::m_PadCheckStap < 2) {
        m_fFriendMatchWait -= GetFrameRateF();
        if (m_fFriendMatchWait < 0.0f) {
            m_fFriendMatchWait = 0.0f;
            if (dwc::CGameDwc::GetInstance() != 0 && m_pGameDwc->m_Dwc.IsConnect() &&
                m_pGameDwc->m_Dwc.CanbeRefuseFriendMatch() && m_pVsFriend == 0) {
                m_pVsFriend = (dwc::CFriend *)m_pFriendList->SearchFriendMatchChallengerAll();
            }
        }
    }
}

void msw::CSelectMenu::CancelVsFriend()
{
    m_pVsFriend = 0;
    m_fFriendMatchWait = 180.0f;
}

int msw::CSelectMenu::CheckCharChange()
{
    switch (m_nPlayMode) {
        case 0:
        case 1:
        case 4:
        case 5:
        case 7:
        case 10:
        case 15:
        case 16:
        case 17:
        case 19:
        case 20:
        case 21:
        case 24:
        case 25:
            return 1;
    }
    return 0;
}

// Screen 0x2E: nothing was pressed for too long; back to the title.
void msw::CSelectMenu::TimeOver_In()
{
    CMenuPad::s_Instance.ResetNoInputCountTime();
    menu::CSndBGM &rBGM = menu::CSndBGM::s_Instance;
    if (rBGM.GetTaskId() != 0) {
        rBGM.Stop(rBGM.GetTaskId(), 0.0f);
    }
}

void msw::CSelectMenu::TimeOver_Loop()
{
    float fTime = CSelectMenuLayoutBase::s_DefaultOutAnimOverTime;
    SetWhiteFadeOut(fTime);
    SetNextMode(1, fTime);
    CMenuPadInfo::s_Instance.m_aPad[0].m_nFlags06 &= ~1 & ~2;
}

void msw::CSelectMenu::TimeOver_Out()
{
}

// Two test screens left in the game (0x2F and 0x30).
void msw::CSelectMenu::Test_In()
{
    SendObjEventTrigger(0, 3, 0);
    Build(0x3B);
    SendObjEventTrigger(0x3A, 0x33, 0);
    SendObjEventTrigger(0x3B, 0x35, (void *)1);
    SendObjEventTrigger(0, 0, (void *)L"BgDisp\x306E\x30C6\x30B9\x30C8");
    SendObjEventTrigger(0x3E, 0x4D, 0);
    Build(0x3E);
    CCameraMenuBG::s_Instance.SetAnimTable(s_CameraAnimTable, CAMERA_ANIM_NUM, _IsOpenMixSportsFlag() == 1);
    CCameraMenuBG::s_Instance.Play(0x25, GetCameraSpeed(0x25), 0.0f);
    SendObjEventTrigger(0x3B, 0x36, (void *)0xA);
    SelectMenu::CBgDisplay *pBg = (SelectMenu::CBgDisplay *)FindObj(0x3E);
    if (pBg != 0) {
        BgDispParam param;
        param.m_nStage = 0;
        param.m_pStageName = L"\x30EA\x30BA\x30E0\x30E0\x30E0\x30E0";
        param.m_pIntroText = L"\x30EA\x30BA\x30E0\x30E0\x30E0\x306E\x8AAC\x660E\x3044\x3044";
        param.m_pScoreLabel = L"1 Player";
        param.m_pScoreText = L"Hi Score : 9875";
        param.m_nType = 2;
        param.m_bStage = 1;
        param.m_nPartyCup = 0;
        param.m_nPartyStage = 1;
        pBg->SetParameter(&param);
    }
}

void msw::CSelectMenu::Test_Loop()
{
    // Unused: the original's .rodata holds these four function-local
    // constants of the test screen here (probably left from removed debug code).
    static const unsigned char level = 1;
    static const unsigned char level_ = 1;
    static const unsigned char rank = 1;
    static const unsigned char optMode = 3;

    FindObj(0x3E);
}

void msw::CSelectMenu::Test_Out()
{
}

void msw::CSelectMenu::Test2_In()
{
    static const long objId = 0x1082;

    SendObjEventTrigger(0, 3, 0);
    SendObjEventTrigger(0, 2, 0);
    SendObjEventTrigger(0x3A, 0x33, 0);
    SendObjEventTrigger(0x3B, 0x35, 0);
    CCockpit *pCockpit = CCockpit::GetTask();
    CockpitItemInfoTab *pTab = new CockpitItemInfoTab(objId, 3, 0.0f);
    pCockpit->AddObj(pTab);
    pTab->Set();
}

void msw::CSelectMenu::Test2_Loop()
{
}

void msw::CSelectMenu::Test2_Out()
{
}

namespace {
    // Copies of a character slot to and from menu::CStateSave (member by
    // member, as the original does).
    inline void SaveSlot(msw::menu::CStateSave::SelectSlot &rDst, const msw::CSelectMenu::CharaSelectParam &rSrc)
    {
        rDst.m_Unk00 = rSrc.m_Unk00;
        rDst.m_nPlayerKind = rSrc.m_nPlayerKind;
        rDst.m_Unk08 = rSrc.m_Unk08;
        rDst.m_Unk0C = rSrc.m_Unk0C;
        rDst.m_nChara = rSrc.m_nChara;
        rDst.m_nCharaSub = rSrc.m_nCharaSub;
        rDst.m_nState = rSrc.m_nState;
        rDst.m_nColor = rSrc.m_Unk13[0];
        rDst.m_Unk14 = rSrc.m_Unk14;
        rDst.m_Unk16 = rSrc.m_Unk16;
        rDst.m_nMiiMode = rSrc.m_nMiiMode;
        rDst.m_nMiiIndex = rSrc.m_nMiiIndex;
        rDst.m_MiiSource = rSrc.m_MiiSource;
        rDst.m_MiiCreateId = rSrc.m_MiiCreateId;
    }

    inline void LoadSlot(msw::CSelectMenu::CharaSelectParam &rDst, const msw::menu::CStateSave::SelectSlot &rSrc)
    {
        rDst.m_Unk00 = rSrc.m_Unk00;
        rDst.m_nPlayerKind = rSrc.m_nPlayerKind;
        rDst.m_Unk08 = rSrc.m_Unk08;
        rDst.m_Unk0C = rSrc.m_Unk0C;
        rDst.m_nChara = rSrc.m_nChara;
        rDst.m_nCharaSub = rSrc.m_nCharaSub;
        rDst.m_nState = rSrc.m_nState;
        rDst.m_Unk13[0] = rSrc.m_nColor;
        rDst.m_Unk14 = rSrc.m_Unk14;
        rDst.m_Unk16 = rSrc.m_Unk16;
        rDst.m_nMiiMode = rSrc.m_nMiiMode;
        rDst.m_nMiiIndex = rSrc.m_nMiiIndex;
        rDst.m_MiiSource = rSrc.m_MiiSource;
        rDst.m_MiiCreateId = rSrc.m_MiiCreateId;
    }
}

// Keeps the state of the menu in menu::CStateSave while a game runs.
void msw::CSelectMenu::Backup()
{
    menu::CStateSave *pSave = menu::CStateSave::GetInstance();
    pSave->Initialize();
    CMenuPadInfo::PAD *pPad = CMenuPadInfo::s_Instance.m_aPad;
    for (long i = 0; i < 4; i++) {
        pSave->m_bPadLeader[i] = (pPad[i].m_nFlags04 >> 2) & 1;
    }
    pSave->m_nStep = m_nStep;
    pSave->m_fWait = m_fUnk94;
    pSave->m_bModeEnd = m_Unk98;
    pSave->m_nNextMode = m_nNextMode;
    pSave->m_Unk22 = m_Unk9A;
    pSave->m_nMenuMode = m_Unk9B;
    pSave->m_Unk24 = m_Unk9C;
    pSave->m_nPrevModeState = m_Unk9D;
    for (long i = 0; i < 4; i++) {
        for (long j = 0; j < STATE_NUM; j++) {
            pSave->m_nCurrentBtnKindId[i][j] = m_nCurrentBtnKindId[i][j];
        }
    }
    for (long i = 0; i < 4; i++) {
        pSave->m_nPrevBtnKindId[i] = m_nCupBtnKindId[i];
        pSave->m_PadSelect[i][0] = m_aPadSelect[i].m_Unk00;
        pSave->m_PadSelect[i][1] = m_aPadSelect[i].m_Unk01;
        pSave->m_PadSelect[i][2] = m_aPadSelect[i].m_Unk02;
        pSave->m_PadSelect[i][3] = m_aPadSelect[i].m_Unk03;
        pSave->m_PadSelect[i][4] = m_aPadSelect[i].m_nPlayerNo;
        pSave->m_nCharaColor[i] = m_nCharaColor[i];
    }
    pSave->m_nGameKind = m_nGameKind;
    pSave->m_nWifiGameKind = m_nWifiGameKind;
    pSave->m_nPlayerNum = m_nPlayerNum;
    pSave->m_Unk44D = m_Unk269;
    pSave->m_nPlayMode = m_nPlayMode;
    pSave->m_Unk44F = m_Unk26B;
    pSave->m_Unk450 = m_Unk26C;
    pSave->m_Unk451 = m_Unk26D;
    pSave->m_nStageSub = m_nStageSub;
    pSave->m_nStage = m_nStage;
    pSave->m_nCourtGameKind = m_nCourtGameKind;
    pSave->m_Unk45E[0] = m_Unk27A;
    pSave->m_Unk45E[1] = m_Unk27B;
    pSave->m_Unk45E[2] = m_Unk27C;
    pSave->m_Unk45E[3] = m_Unk27D;
    pSave->m_Unk45E[4] = m_Unk27E;
    for (long i = 0; i < 4; i++) {
        pSave->m_nPadPlayer[i] = m_nPadPlayer[i];
    }
    for (long i = 0; i < HISTORY_NUM; i++) {
        pSave->m_History[i][0] = m_History[i].m_nState;
        pSave->m_History[i][1] = m_History[i].m_nNextState;
    }
    for (long i = 0; i < STATE_NUM; i++) {
        pSave->m_HistoryBtnKindId[i] = m_HistoryBtnKindId[i];
    }
    pSave->m_nHistoryNum = m_nHistoryNum;
    for (long i = 0; i < 6; i++) {
        SaveSlot(pSave->m_aCharaSelectParam[i], m_aCharaSelectParam[i]);
    }
    for (long i = 0; i < 6; i++) {
        SaveSlot(pSave->m_aCharaSaveParam[i], m_aCharaSaveParam[i]);
    }
    // The original indexes the two random-draw arrays one past their
    // ends here (it copies the first word after each, the same word on
    // each pass): what that saves is m_nRandomDecideTime[0] and m_Unk688.
    pSave->m_nRandomDecideTime[0] = m_nRandomDecideTime[0];
    pSave->m_Unk688 = m_Unk688;
    pSave->m_Wifi2P.m_Unk00 = m_Unk68C[0];
    pSave->m_Wifi2P.m_nPlayerKind = m_Unk68C[1];
    pSave->m_Wifi2P.m_Unk08 = m_Unk68C[2];
    pSave->m_Wifi2P.m_Unk0C = m_Unk68C[3];
    pSave->m_Wifi2P.m_nChara = m_Unk69C[0];
    pSave->m_Wifi2P.m_nCharaSub = m_Unk69C[1];
    pSave->m_Wifi2P.m_nState = m_Unk69C[2];
    pSave->m_Wifi2P.m_nColor = m_Unk69C[3];
    pSave->m_Wifi2P.m_Unk14 = m_Unk6A0;
    pSave->m_Wifi2P.m_Unk16 = m_Unk6A2;
    pSave->m_Wifi2P.m_nMiiMode = m_nStarMiiUnk;
    pSave->m_Wifi2P.m_nMiiIndex = m_nStarMiiIndex;
    pSave->m_Wifi2P.m_MiiSource = m_StarMiiSource;
    pSave->m_Wifi2P.m_MiiCreateId = m_StarMiiCreateId;

    SelectMenu::CBgChr *pBgChr = (SelectMenu::CBgChr *)FindObj(0x3A);
    if (pBgChr != 0) {
        pSave->m_nTopChara = pBgChr->GetPosChara(0);
    }
    pSave->m_bStarMii = m_bStarMii;
    pSave->m_Unk708 = (long)m_Unk708;
    pSave->m_pVsFriendSaved = (long)m_pVsFriendSaved;
    if (dwc::CGameDwc::GetInstance() != 0) {
        if (CGamePadCheck::GetInstance() != 0) {
            for (long i = 0; i < 4; i++) {
                CGamePadCheck::m_PadBackState[i] = CGamePadCheck::GetInstance()->m_PadState[i];
            }
        }
        pSave->m_nWifiSavedGameKind = m_nWifiSavedGameKind;
        pSave->m_nWifiSavedStageSub = m_nWifiSavedStageSub;
        pSave->m_nWifiSavedStage = m_nWifiSavedStage;
    }
}

// Puts back what Backup kept; 0 when there is nothing to put back.
int msw::CSelectMenu::Restore()
{
    menu::CStateSave *pSave = menu::CStateSave::GetInstance();
    if (pSave->IsActive() == 0) {
        return 0;
    }
    if (pSave->GetStartMenuMode() == 0) {
        pSave->Finalize();
        return 0;
    }
    CMenuPadInfo::PAD *pPad = CMenuPadInfo::s_Instance.m_aPad;
    for (long i = 0; i < 4; i++) {
        if (pSave->m_bPadLeader[i] != 0) {
            pPad[i].m_nFlags04 |= 4;
        } else {
            pPad[i].m_nFlags04 &= ~4;
        }
    }
    m_nStep = pSave->m_nStep;
    m_fUnk94 = pSave->m_fWait;
    m_Unk98 = pSave->m_bModeEnd;
    m_nNextMode = pSave->m_nNextMode;
    m_Unk9A = pSave->m_Unk22;
    m_Unk9B = pSave->m_nMenuMode;
    m_Unk9C = pSave->m_Unk24;
    m_Unk9D = pSave->m_nPrevModeState;
    for (long i = 0; i < 4; i++) {
        for (long j = 0; j < STATE_NUM; j++) {
            m_nCurrentBtnKindId[i][j] = pSave->m_nCurrentBtnKindId[i][j];
        }
    }
    for (long i = 0; i < 4; i++) {
        m_nCupBtnKindId[i] = pSave->m_nPrevBtnKindId[i];
        m_aPadSelect[i].m_Unk00 = pSave->m_PadSelect[i][0];
        m_aPadSelect[i].m_Unk01 = pSave->m_PadSelect[i][1];
        m_aPadSelect[i].m_Unk02 = pSave->m_PadSelect[i][2];
        m_aPadSelect[i].m_Unk03 = pSave->m_PadSelect[i][3];
        m_aPadSelect[i].m_nPlayerNo = pSave->m_PadSelect[i][4];
        m_nCharaColor[i] = pSave->m_nCharaColor[i];
    }
    m_nGameKind = pSave->m_nGameKind;
    m_nWifiGameKind = pSave->m_nWifiGameKind;
    m_nPlayerNum = pSave->m_nPlayerNum;
    m_Unk269 = pSave->m_Unk44D;
    m_nPlayMode = pSave->m_nPlayMode;
    m_Unk26B = pSave->m_Unk44F;
    m_Unk26C = pSave->m_Unk450;
    m_Unk26D = pSave->m_Unk451;
    m_nStageSub = pSave->m_nStageSub;
    m_nStage = pSave->m_nStage;
    m_nCourtGameKind = pSave->m_nCourtGameKind;
    m_Unk27A = pSave->m_Unk45E[0];
    m_Unk27B = pSave->m_Unk45E[1];
    m_Unk27C = pSave->m_Unk45E[2];
    m_Unk27D = pSave->m_Unk45E[3];
    m_Unk27E = pSave->m_Unk45E[4];
    for (long i = 0; i < 4; i++) {
        m_nPadPlayer[i] = pSave->m_nPadPlayer[i];
    }
    for (long i = 0; i < HISTORY_NUM; i++) {
        m_History[i].m_nState = pSave->m_History[i][0];
        m_History[i].m_nNextState = pSave->m_History[i][1];
    }
    for (long i = 0; i < STATE_NUM; i++) {
        m_HistoryBtnKindId[i] = pSave->m_HistoryBtnKindId[i];
    }
    m_nHistoryNum = pSave->m_nHistoryNum;
    for (long i = 0; i < 6; i++) {
        LoadSlot(m_aCharaSelectParam[i], pSave->m_aCharaSelectParam[i]);
    }
    for (long i = 0; i < 6; i++) {
        LoadSlot(m_aCharaSaveParam[i], pSave->m_aCharaSaveParam[i]);
    }
    // As in Backup, the original puts back only these two words of the
    // random draw.
    m_nRandomDecideTime[0] = pSave->m_nRandomDecideTime[0];
    m_Unk688 = pSave->m_Unk688;
    m_Unk68C[0] = pSave->m_Wifi2P.m_Unk00;
    m_Unk68C[1] = pSave->m_Wifi2P.m_nPlayerKind;
    m_Unk68C[2] = pSave->m_Wifi2P.m_Unk08;
    m_Unk68C[3] = pSave->m_Wifi2P.m_Unk0C;
    m_Unk69C[0] = pSave->m_Wifi2P.m_nChara;
    m_Unk69C[1] = pSave->m_Wifi2P.m_nCharaSub;
    m_Unk69C[2] = pSave->m_Wifi2P.m_nState;
    m_Unk69C[3] = pSave->m_Wifi2P.m_nColor;
    m_Unk6A0 = pSave->m_Wifi2P.m_Unk14;
    m_Unk6A2 = pSave->m_Wifi2P.m_Unk16;
    m_nStarMiiUnk = pSave->m_Wifi2P.m_nMiiMode;
    m_nStarMiiIndex = pSave->m_Wifi2P.m_nMiiIndex;
    m_StarMiiSource = pSave->m_Wifi2P.m_MiiSource;
    m_StarMiiCreateId = pSave->m_Wifi2P.m_MiiCreateId;

    SelectMenu::CBgChr *pBgChr = (SelectMenu::CBgChr *)FindObj(0x3A);
    if (pBgChr != 0) {
        pBgChr->SetPosChara(pSave->m_nTopChara);
        pBgChr->SetSelectGameChara();
    }
    m_bStarMii = pSave->m_bStarMii;
    m_Unk708 = (dwc::CFriend *)pSave->m_Unk708;
    m_pVsFriendSaved = (dwc::CFriend *)pSave->m_pVsFriendSaved;
    pSave->Finalize();
    if (dwc::CGameDwc::GetInstance() != 0) {
        m_nWifiSavedGameKind = pSave->m_nWifiSavedGameKind;
        m_nWifiSavedStageSub = pSave->m_nWifiSavedStageSub;
        m_nWifiSavedStage = pSave->m_nWifiSavedStage;
    }
    return 1;
}
