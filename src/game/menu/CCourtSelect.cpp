#include "bcs/BCS_Rand.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/dwc/CDwcFriend.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CMenuOption.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/CSelectMenu.hpp"
#include "game/msw/SelectMenu/CBgDisplay.hpp"
#include "game/msw/SelectMenu/CCharaOk.hpp"
#include "game/msw/SelectMenu/CCourt.hpp"
#include "game/msw/SelectMenu/CCourtTab.hpp"
#include "game/msw/dwc/CFriend.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/CVibEff.hpp"
#include "stdio.h"
#include "string.h"
#include "wchar.h"

// The court (stage) select screen of CSelectMenu (MenuF): offline
// (Court_*), on Wi-Fi (WifiCourt_*) and for the party games
// (PartyCourt_*). Menu objects: the stage buttons (SelectMenu::CCourt,
// 0x31), the cup tabs over them (SelectMenu::CCourtTab, 0x32), the
// background display with the stage's movie and texts
// (SelectMenu::CBgDisplay, 0x3E) and the "OK?" window
// (SelectMenu::CCharaOk, 0x13). Names in this file are guesses.

namespace {
    enum {
        OBJ_TASK = 0,
        OBJ_UNK_01 = 1,
        OBJ_CHARA_OK = 0x13,
        OBJ_COURT = 0x31,
        OBJ_COURT_TAB = 0x32,
        OBJ_UNK_3A = 0x3A,
        OBJ_COURT_CHR = 0x3B,
        OBJ_BG_DISPLAY = 0x3E,
        OBJ_FADE = 0x3F,
        OBJ_BG_CHR = 0x42
    };

    enum {
        BTN_BACK = 1,
        BTN_OK_YES = 0x4E,
        BTN_OK_NO = 0x4F,
        BTN_STAGE = 0x50,      // to 0x53, the stage buttons
        BTN_CUP_TAB = 0x56,    // to 0x59, the cup tabs
        BTN_OTHER = 0x80A0,    // Wi-Fi: the other player chooses
        BTN_RANDOM = 0x80A2,
        BTN_OPTION = 0x80A3
    };

    enum {
        PAD_LEFT = 0x8,
        PAD_RIGHT = 0x2,
        PAD_A = 0x10,
        PAD_B = 0x20,
        PAD_OPTION = 0x400,
        PAD_RANDOM = 0x800
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_CAMERA = 1000,
        STEP_SELECT = 1001,
        STEP_TAB_CHANGED = 1002,
        STEP_RANDOM_INIT = 1003,
        STEP_RANDOM = 1004,
        STEP_UNUSED = 1005,
        STEP_OK_DELAY = 1006,
        STEP_OK_INIT = 1007,
        STEP_OK = 1008,
        STEP_OK_OUT = 1009,
        STEP_LAYOUT_OUT = 1010
    };

    // The number of jumps of the random stage draw. A table, not a scalar:
    // the original keeps the value first in .rodata, ahead of the strings
    // of the tables below (as s_WinAnimSpeed in game/menu/CEntry.cpp).
    const float s_RandomDecideTime[1] = { 8.0f };

    // The introduction texts of the stages of each sport (message group
    // "List_Intro_Text"), by stage number; numbers with no stage use the
    // first one.

const char *s_GimikTextBA[21] = {
        "INTRO_GIMIK_00_BA", "INTRO_GIMIK_00_BA", "INTRO_GIMIK_01_BA",
        "INTRO_GIMIK_02_BA", "INTRO_GIMIK_03_BA", "INTRO_GIMIK_04_BA",
        "INTRO_GIMIK_05_BA", "INTRO_GIMIK_06_BA", "INTRO_GIMIK_00_BA",
        "INTRO_GIMIK_07_BA", "INTRO_GIMIK_08_BA", "INTRO_GIMIK_09_BA",
        "INTRO_GIMIK_10_BA", "INTRO_GIMIK_00_BA", "INTRO_GIMIK_00_BA",
        "INTRO_GIMIK_11_BA", "INTRO_GIMIK_12_BA", "INTRO_GIMIK_13_BA",
        "INTRO_GIMIK_00_BA", "INTRO_GIMIK_00_BA", "INTRO_GIMIK_00_BA"
    };

    const char *s_GimikTextVO[21] = {
        "INTRO_GIMIK_00_VO", "INTRO_GIMIK_00_VO", "INTRO_GIMIK_01_VO",
        "INTRO_GIMIK_02_VO", "INTRO_GIMIK_03_VO", "INTRO_GIMIK_04_VO",
        "INTRO_GIMIK_05_VO", "INTRO_GIMIK_06_VO", "INTRO_GIMIK_00_VO",
        "INTRO_GIMIK_07_VO", "INTRO_GIMIK_08_VO", "INTRO_GIMIK_09_VO",
        "INTRO_GIMIK_10_VO", "INTRO_GIMIK_00_VO", "INTRO_GIMIK_00_VO",
        "INTRO_GIMIK_11_VO", "INTRO_GIMIK_12_VO", "INTRO_GIMIK_13_VO",
        "INTRO_GIMIK_00_VO", "INTRO_GIMIK_00_VO", "INTRO_GIMIK_00_VO"
    };

    const char *s_GimikTextDO[21] = {
        "INTRO_GIMIK_00_DO", "INTRO_GIMIK_00_DO", "INTRO_GIMIK_01_DO",
        "INTRO_GIMIK_02_DO", "INTRO_GIMIK_03_DO", "INTRO_GIMIK_04_DO",
        "INTRO_GIMIK_05_DO", "INTRO_GIMIK_06_DO", "INTRO_GIMIK_00_DO",
        "INTRO_GIMIK_07_DO", "INTRO_GIMIK_08_DO", "INTRO_GIMIK_09_DO",
        "INTRO_GIMIK_10_DO", "INTRO_GIMIK_00_DO", "INTRO_GIMIK_00_DO",
        "INTRO_GIMIK_11_DO", "INTRO_GIMIK_12_DO", "INTRO_GIMIK_13_DO",
        "INTRO_GIMIK_00_DO", "INTRO_GIMIK_00_DO", "INTRO_GIMIK_00_DO"
    };

    const char *s_GimikTextHO[21] = {
        "INTRO_GIMIK_00_HO", "INTRO_GIMIK_00_HO", "INTRO_GIMIK_01_HO",
        "INTRO_GIMIK_02_HO", "INTRO_GIMIK_03_HO", "INTRO_GIMIK_04_HO",
        "INTRO_GIMIK_05_HO", "INTRO_GIMIK_06_HO", "INTRO_GIMIK_00_HO",
        "INTRO_GIMIK_07_HO", "INTRO_GIMIK_08_HO", "INTRO_GIMIK_09_HO",
        "INTRO_GIMIK_10_HO", "INTRO_GIMIK_00_HO", "INTRO_GIMIK_00_HO",
        "INTRO_GIMIK_11_HO", "INTRO_GIMIK_12_HO", "INTRO_GIMIK_13_HO",
        "INTRO_GIMIK_00_HO", "INTRO_GIMIK_00_HO", "INTRO_GIMIK_00_HO"
    };

    const char **s_GimikText[4] = { s_GimikTextBA, s_GimikTextVO, s_GimikTextDO, s_GimikTextHO };

    // The texts of the cups, and of the party games' stages by cup and
    // stage.

    const char *s_CupText[4] = {
        "INTRO_CUP_KINOKO", "INTRO_CUP_FLOWER",
        "INTRO_CUP_STAR", "INTRO_CUP_EXTRA"
    };

    const char *s_PartyTextRR[12] = {
        "INTRO_PARTY_RR_00", "INTRO_PARTY_RR_01", "INTRO_PARTY_RR_02", "INTRO_PARTY_RR_03",
        "INTRO_PARTY_RR_04", "INTRO_PARTY_RR_05", "INTRO_PARTY_RR_06", "INTRO_PARTY_RR_07",
        "INTRO_PARTY_RR_08", "INTRO_PARTY_RR_09", "INTRO_PARTY_RR_10", "INTRO_PARTY_RR_11"
    };

    const char *s_PartyTextDD[6] = {
        "INTRO_PARTY_DD_00", "INTRO_PARTY_DD_01", "INTRO_PARTY_DD_02",
        "INTRO_PARTY_DD_03", "INTRO_PARTY_DD_04", "INTRO_PARTY_DD_05"
    };

    const char *s_PartyTextHB[6] = {
        "INTRO_PARTY_HB_00", "INTRO_PARTY_HB_01", "INTRO_PARTY_HB_02",
        "INTRO_PARTY_HB_03", "INTRO_PARTY_HB_04", "INTRO_PARTY_HB_05"
    };

    const char *s_PartyTextSS[6] = {
        "INTRO_PARTY_SS_00", "INTRO_PARTY_SS_02", "INTRO_PARTY_SS_01",
        "INTRO_PARTY_SS_03", "INTRO_PARTY_SS_05", "INTRO_PARTY_SS_04"
    };

    const char *s_PartyLevelDD[2] = {
        "INTRO_PARTY_DD_LV_NORMAL", "INTRO_PARTY_DD_LV_HARD"
    };

    const char *s_PartyLevelHB[2] = {
        "INTRO_PARTY_HB_LV_NORMAL", "INTRO_PARTY_HB_LV_HARD"
    };

    const char *s_PartyLevelSS[2] = {
        "INTRO_PARTY_SS_LV_NORMAL", "INTRO_PARTY_SS_LV_HARD"
    };

    const char *s_PartyCupRR[4] = {
        "INTRO_CUP_KINOKO", "INTRO_CUP_FLOWER",
        "INTRO_CUP_STAR", "INTRO_CUP_KINOKO"
    };

    // The movies of the stages of each sport, by stage number.

    const char *s_MovieBA[21] = {
        "movie/st01_BA.thp", "movie/st01_BA.thp", "movie/st02_BA.thp",
        "movie/st03_BA.thp", "movie/st04_BA.thp", "movie/st05_BA.thp",
        "movie/st06_BA.thp", "movie/st07_BA.thp", "movie/st01_BA.thp",
        "movie/st09_BA.thp", "movie/st10_BA.thp", "movie/st11_BA.thp",
        "movie/st12_BA.thp", "movie/st01_BA.thp", "movie/st01_BA.thp",
        "movie/st15_BA.thp", "movie/st16_BA.thp", "movie/st17_BA.thp",
        "movie/st01_BA.thp", "movie/st01_BA.thp", "movie/st01_BA.thp"
    };

    const char *s_MovieVO[21] = {
        "movie/st01_VO.thp", "movie/st01_VO.thp", "movie/st02_VO.thp",
        "movie/st03_VO.thp", "movie/st04_VO.thp", "movie/st05_VO.thp",
        "movie/st06_VO.thp", "movie/st07_VO.thp", "movie/st01_VO.thp",
        "movie/st09_VO.thp", "movie/st10_VO.thp", "movie/st11_VO.thp",
        "movie/st12_VO.thp", "movie/st01_VO.thp", "movie/st01_VO.thp",
        "movie/st15_VO.thp", "movie/st16_VO.thp", "movie/st17_VO.thp",
        "movie/st01_VO.thp", "movie/st01_VO.thp", "movie/st01_VO.thp"
    };

    const char *s_MovieDO[21] = {
        "movie/st01_DO.thp", "movie/st01_DO.thp", "movie/st02_DO.thp",
        "movie/st03_DO.thp", "movie/st04_DO.thp", "movie/st05_DO.thp",
        "movie/st06_DO.thp", "movie/st07_DO.thp", "movie/st01_DO.thp",
        "movie/st09_DO.thp", "movie/st10_DO.thp", "movie/st11_DO.thp",
        "movie/st12_DO.thp", "movie/st01_DO.thp", "movie/st01_DO.thp",
        "movie/st15_DO.thp", "movie/st16_DO.thp", "movie/st17_DO.thp",
        "movie/st01_DO.thp", "movie/st01_DO.thp", "movie/st01_DO.thp"
    };

    const char *s_MovieHO[21] = {
        "movie/st01_HO.thp", "movie/st01_HO.thp", "movie/st02_HO.thp",
        "movie/st03_HO.thp", "movie/st04_HO.thp", "movie/st05_HO.thp",
        "movie/st06_HO.thp", "movie/st07_HO.thp", "movie/st01_HO.thp",
        "movie/st09_HO.thp", "movie/st10_HO.thp", "movie/st11_HO.thp",
        "movie/st12_HO.thp", "movie/st01_HO.thp", "movie/st01_HO.thp",
        "movie/st15_HO.thp", "movie/st16_HO.thp", "movie/st17_HO.thp",
        "movie/st01_HO.thp", "movie/st01_HO.thp", "movie/st01_HO.thp"
    };

    const char **s_Movie[4] = { s_MovieBA, s_MovieVO, s_MovieDO, s_MovieHO };

    // The movies of the party games, by cup and stage, for each play
    // type (msw::_GetPartyPlayType).

    const char *s_PartyMovie70A[4] = {
        "movie/per_70_A.thp", "movie/per_71_A.thp",
        "movie/per_72_A.thp", "movie/per_72_A.thp"
    };

    const char *s_PartyMovie70B[4] = {
        "movie/per_70_B.thp", "movie/per_71_B.thp",
        "movie/per_72_B.thp", "movie/per_72_B.thp"
    };

    const char **s_PartyMovie70[4] = { s_PartyMovie70A, s_PartyMovie70B, s_PartyMovie70A, s_PartyMovie70A };

    const char *s_PartyMovie40VO[4] = {
        "movie/per_40_VO.thp", "movie/per_40_VO.thp",
        "movie/per_40_VO.thp", "movie/per_40_VO.thp"
    };

    const char **s_PartyMovie40[4] = { s_PartyMovie40VO, s_PartyMovie40VO, s_PartyMovie40VO, s_PartyMovie40VO };

    const char *s_PartyMovie55A[4] = {
        "movie/per_55_A.thp", "movie/per_56_A.thp",
        "movie/per_57_A.thp", "movie/per_57_A.thp"
    };

    const char *s_PartyMovie55B[4] = {
        "movie/per_55_B.thp", "movie/per_56_B.thp",
        "movie/per_57_B.thp", "movie/per_57_B.thp"
    };

    const char **s_PartyMovie55[4] = { s_PartyMovie55A, s_PartyMovie55B, s_PartyMovie55A, s_PartyMovie55A };

    const char *s_PartyMovie85A[4] = {
        "movie/per_85_A.thp", "movie/per_87_A.thp",
        "movie/per_86_A.thp", "movie/per_85_A.thp"
    };

    const char *s_PartyMovie85B[4] = {
        "movie/per_85_B.thp", "movie/per_87_B.thp",
        "movie/per_86_B.thp", "movie/per_85_B.thp"
    };

    const char **s_PartyMovie85[4] = { s_PartyMovie85A, s_PartyMovie85B, s_PartyMovie85A, s_PartyMovie85A };

    const char ***s_PartyMovie[8] = {
        s_PartyMovie70, s_PartyMovie70, s_PartyMovie40, s_PartyMovie40,
        s_PartyMovie55, s_PartyMovie55, s_PartyMovie85, s_PartyMovie85
    };

    // The music of each cup of a party game (passed with event 0x4E to the
    // background display), and of each stage of the rhythm game.
    const long s_PartyBgm[6] = { 0x40, 0x49, 0x52, 0x43, 0x4C, 0x55 };
    const long s_RhythmBgm[4][4] = {
        { 0x64, 0x65, 0x66, 0x6D },
        { 0x67, 0x68, 0x69, 0x6E },
        { 0x6A, 0x6B, 0x6C, 0x6F },
        { 0, 0, 0, 0 }
    };
    // The rhythm game's record (in the set-up data) of each stage.
    const long s_RhythmRecord[4][4] = {
        { 0, 1, 2, 9 },
        { 3, 4, 5, 0xA },
        { 6, 7, 8, 0xB },
        { 0, 0, 0, 0 }
    };

    // The rhythm game's records in the set-up data
    // (CSave::GetPlayerSetUpData), per number of players.
    struct RhythmRecord {
        long m_nScore;
        unsigned char m_Unk04[0xC];
    };

    struct SetUpDataRhythm {
        unsigned char m_Unk000[0x59C];
        RhythmRecord m_Record[5][12];
    };

    // The setters of CSelectMenu's stage (cup) and stage index: setting
    // 0xFF (none) keeps the old value.
    inline void SetStage(msw::CSelectMenu *pMenu, long nStage)
    {
        if (nStage == 0xFF) {
            pMenu->m_nPrevStage = pMenu->m_nStage;
        }
        pMenu->m_nStage = nStage;
    }

    inline void SetStageSub(msw::CSelectMenu *pMenu, long nStageSub)
    {
        if (nStageSub == 0xFF) {
            pMenu->m_nPrevStageSub = pMenu->m_nStageSub;
        }
        pMenu->m_nStageSub = nStageSub;
    }
}

void msw::CSelectMenu::_SetCourtDisplay(long nCup, long nStage)
{
    if (m_nDispCup == nCup && m_nDispStage == nStage) {
        return;
    }
    m_nDispCup = nCup;
    m_nDispStage = nStage;

    SelectMenu::CBgDisplay *pBg = (SelectMenu::CBgDisplay *)FindObj(OBJ_BG_DISPLAY);
    if (pBg == 0) {
        return;
    }

    wchar_t aText8[0x40];
    wchar_t aText9[0x40];
    wchar_t aText12[0x40];
    BgDispParam param;
    param.m_nType = 0;
    param.m_nGameKind = m_nGameKind;
    param.m_nCup = m_nStage;
    param.m_nPartyStage = 0;

    if (nStage == -1) {
        param.m_pCupText = GetMessageLabelString("List_Intro_Text", s_CupText[nCup]);
        param.m_bStage = 0;
    } else {
        if (!_CheckStage(m_nGameKind, nCup, nStage)) {
            return;
        }
        const unsigned char *pStageNo = _GetStageNoInfo(m_nGameKind, nCup);
        unsigned char nNo = pStageNo[nStage];
        param.m_pMovieName = s_Movie[m_nGameKind][nNo];
        short nLabel = GetMessageLabelIndex("List_Stage", "COURT_0001");
        param.m_pStageName = GetMessageString("List_Stage", nLabel + nNo - 1);
        param.m_pIntroText = GetMessageLabelString("List_Intro_Text", s_GimikText[m_nGameKind][nNo]);

        CMenuOption *pOption = &CMenuOption::s_Instance;
        long nKind = RuleSel_GetOptionKindId();
        long nNum = pOption->GetMenuNum(nKind);
        for (long i = 0; i < nNum; i++) {
            long nId;
            CSave::PlayerData *pData = CSave::GetPlayerData(-1);
            if ((long)m_Unk9B == 4) {
                nId = CMenuOption::GetMenuDefaultId(nKind, i, pData->IsDifficult(m_nGameKind));
            } else {
                nId = pOption->GetMenuParameterId(nKind, i);
            }

            switch (pOption->GetTextIndex(nKind, i)) {
            case 8:
            case 11:
            case 15: {
                // Left unset for other game kinds, as in the game.
                const wchar_t *pHead;
                switch (m_nGameKind) {
                case 0:
                    pHead = GetMessageLabelString("OptionRuleList", "OptionRule_Disp_BA_00");
                    break;
                case 1:
                    pHead = GetMessageLabelString("OptionRuleList", "OptionRule_Disp_VO_00");
                    break;
                case 2:
                    pHead = GetMessageLabelString("OptionRuleList", "OptionRule_Disp_DO_00");
                    break;
                case 3:
                    pHead = GetMessageLabelString("OptionRuleList", "OptionRule_Disp_HO_00");
                    break;
                }
                const wchar_t *pValue = pOption->GetMenuParameterText(nKind, i, nId);
                swprintf(aText8, 0x40, L"%ls%ls%ls", pHead,
                         GetMessageLabelString("OptionRuleList", "OptionRule_Disp_ETC_00"), pValue);
                param.m_pRuleText0 = aText8;
                break;
            }
            case 0:
                param.m_pRuleText3 = pOption->GetMenuParameterText(nKind, i, nId);
                break;
            case 13:
                param.m_pRuleText2 = pOption->GetMenuParameterText(nKind, i, nId);
                break;
            case 1:
                if (nId == 0) {
                    param.m_nRuleFlag = 1;
                } else {
                    param.m_nRuleFlag = 0;
                }
                break;
            case 9:
                memset(aText9, 0, sizeof(aText9));
                swprintf(aText9, 0x40, L"%ls%ls%ls",
                         GetMessageLabelString("OptionRuleList", "OptionRule_Disp_VO_01"),
                         GetMessageLabelString("OptionRuleList", "OptionRule_Disp_ETC_00"),
                         pOption->GetMenuParameterText(nKind, i, nId));
                param.m_pRuleText4 = aText9;
                break;
            case 12:
                memset(aText12, 0, sizeof(aText12));
                swprintf(aText12, 0x40, L"%ls%ls%ls",
                         GetMessageLabelString("OptionRuleList", "OptionRule_Disp_DO_01"),
                         GetMessageLabelString("OptionRuleList", "OptionRule_Disp_ETC_00"),
                         pOption->GetMenuParameterText(nKind, i, nId));
                param.m_pRuleText1 = aText12;
                break;
            }
            param.m_bStage = 1;
        }
    }
    pBg->SetParameter(&param);
}

void msw::CSelectMenu::_SetPartyDisplay(long nCup, long nStage)
{
    if (m_nDispCup == nCup && m_nDispStage == nStage) {
        return;
    }
    m_nDispCup = nCup;
    m_nDispStage = nStage;

    SelectMenu::CBgDisplay *pBg = (SelectMenu::CBgDisplay *)FindObj(OBJ_BG_DISPLAY);
    if (pBg == 0) {
        return;
    }

    BgDispParam param;
    param.m_nType = 0;
    wchar_t aScore[0x20];
    char aLabel[0x20];
    const PartyStageNoInfo *pInfo = _GetPartyStageNoInfo(m_Unk9C);

    if (nStage == -1) {
        param.m_bStage = 0;
        param.m_nPartyStage = 0;
        switch (m_Unk9C) {
        case 1:
            param.m_pCupText = GetMessageLabelString("List_Intro_Text", s_PartyLevelHB[nCup]);
            param.m_nType = 1;
            param.m_nPartyCup2 = nCup;
            param.m_nPartyLevel = 0;
            break;
        case 5:
            param.m_pCupText = GetMessageLabelString("List_Intro_Text", s_PartyLevelDD[nCup]);
            param.m_nType = 1;
            param.m_nPartyCup2 = nCup;
            param.m_nPartyLevel = 1;
            break;
        case 7:
            param.m_pCupText = GetMessageLabelString("List_Intro_Text", s_PartyLevelSS[nCup]);
            param.m_nType = 1;
            param.m_nPartyCup2 = nCup;
            param.m_nPartyLevel = 2;
            break;
        case 3:
            param.m_pCupText = GetMessageLabelString("List_Intro_Text", s_PartyCupRR[nCup]);
            param.m_nType = 2;
            param.m_nPartyCup = nCup;
            break;
        }
    } else {
        if (!_CheckPartyStage(m_Unk9C, nCup, nStage)) {
            return;
        }

        long nType = 0;
        long nIndex = nCup * 3 + nStage;
        switch (m_Unk9C) {
        case 1:
            nType = 0;
            param.m_pIntroText = GetMessageLabelString("List_Intro_Text", s_PartyTextHB[nIndex]);
            break;
        case 5:
            nType = 4;
            param.m_pIntroText = GetMessageLabelString("List_Intro_Text", s_PartyTextDD[nIndex]);
            break;
        case 7:
            nType = 6;
            param.m_pIntroText = GetMessageLabelString("List_Intro_Text", s_PartyTextSS[nIndex]);
            break;
        case 3:
            nType = -1;
            param.m_pIntroText = GetMessageLabelString("List_Intro_Text", s_PartyTextRR[nCup * 4 + nStage]);
            break;
        }

        short nLabel = GetMessageLabelIndex("List_Stage", pInfo->m_Label);
        param.m_pStageName = GetMessageString("List_Stage", nLabel + pInfo->m_nStageNo[nCup][nStage]);
        param.m_bStage = 1;
        if (nType != -1) {
            param.m_pMovieName = s_PartyMovie[nType][nCup][nStage];
            param.m_nType = 1;
            param.m_nPartyCup2 = m_nStage;
            SendObjEventTrigger(OBJ_BG_DISPLAY, 0x4E, (void *)s_PartyBgm[nCup]);
        } else {
            param.m_nStage = nStage;
            sprintf(aLabel, "OptionRule_Disp_RR_00_0%d", m_nPlayerNum - 1);
            param.m_pScoreLabel = GetMessageLabelString("OptionRuleList", aLabel);
            const wchar_t *pHead = GetMessageLabelString("OptionRuleList", "OptionRule_Disp_RR_01");
            SetUpDataRhythm *pSetUp = (SetUpDataRhythm *)CSave::GetPlayerSetUpData(-1);
            swprintf(aScore, 0x20, L"%ls%ls%d", pHead,
                     GetMessageLabelString("OptionRuleList", "OptionRule_Disp_ETC_00"),
                     pSetUp->m_Record[m_nPlayerNum][s_RhythmRecord[nCup][nStage]].m_nScore);
            param.m_pScoreText = aScore;
            param.m_nType = 2;
            param.m_nPartyCup = nCup;
            param.m_nPartyStage = nStage + 1;
            SendObjEventTrigger(OBJ_BG_DISPLAY, 0x4E, (void *)s_RhythmBgm[nCup][nStage]);
        }
    }
    pBg->SetParameter(&param);
}

void msw::CSelectMenu::_BuildCourtSelect(long nTabType, unsigned char nCupMask, int bParty)
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    m_nDispCup = 0xFFFF;
    m_nDispStage = 0xFFFF;
    if (m_nGameKind != m_nCourtGameKind) {
        m_nStage = 0;
        m_nStageSub = 0;
        m_nCurrentBtnKindId[padNo0][nMode] = 0;
        m_nCourtGameKind = m_nGameKind;
    }

    SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(OBJ_COURT);
    if (!bParty) {
        pCourt->m_bFourStages = 0;
    } else {
        // Four stages when any open cup has a fourth stage.
        int bFour = 0;
        unsigned char nCups = 0;
        _GetPartyCupCount(m_Unk9C, &nCups);
        for (long i = 0; i < 3; i++) {
            if ((nCups & (1 << i)) && (_GetPartyStageBit(m_Unk9C, i) & 8)) {
                bFour = 1;
                break;
            }
        }
        if (bFour) {
            pCourt->m_bFourStages = 1;
        } else {
            pCourt->m_bFourStages = 0;
        }
    }

    SelectMenu::CCourtTab *pTab = (SelectMenu::CCourtTab *)FindObj(OBJ_COURT_TAB);
    pTab->m_nTabType = nTabType;
    pTab->m_nCupMask = nCupMask;
    pTab->m_bParty = bParty;
    Build(OBJ_COURT_TAB);
    Build(OBJ_COURT);
    Build(OBJ_BG_DISPLAY);
    if (pCourt != 0 && pTab != 0) {
        long nBtnKindId = pCourt->SetDefaultMoveCursor(padNo0, m_nStageSub + BTN_STAGE);
        long nCup = m_nStage;
        SetStage(this, nCup);
        m_nCurrentBtnKindId[padNo0][nMode] = nBtnKindId;
        if (m_Unk23E == 1) {
            pCourt->m_nPushButtonId[padNo0] = nBtnKindId;
            if (!bParty) {
                _SetCourtDisplay(nCup, nBtnKindId - BTN_STAGE);
            } else {
                _SetPartyDisplay(nCup, nBtnKindId - BTN_STAGE);
            }
        } else if (!bParty) {
            _SetCourtDisplay(nCup, -1);
        } else {
            _SetPartyDisplay(nCup, -1);
        }
        pCourt->SetCursorMarkerPosition(padNo0, nBtnKindId);
        pCourt->SetOneTabAnim(nTabType);
        pTab->SetCupSelect(nCup);
        long nTabBtnKindId = pTab->GetCupBtnKindId(nCup);
        pTab->m_nPushButtonId[padNo0] = nTabBtnKindId;
        pTab->SetCursorMarkerPosition(padNo0, nTabBtnKindId);
        m_nStageSub = 0;

        if (!bParty) {
            long nGameKind = m_nGameKind;
            for (long i = 0; i < 4; i++) {
                const unsigned char *pStageNo = _GetStageNoInfo(nGameKind, i);
                for (long j = 0; j < 3; j++) {
                    pCourt->SetStageId(i, j, *pStageNo++);
                }
            }
        } else {
            const PartyStageNoInfo *pInfo = _GetPartyStageNoInfo(m_Unk9C);
            for (long i = 0; i < 4; i++) {
                for (long j = 0; j < 4; j++) {
                    pCourt->SetStageId(i, j, pInfo->m_nStageId[i][j]);
                }
            }
        }
        _SetStageName(nCup, bParty);
    }
}

void msw::CSelectMenu::_SetStageName(long nCup, int bParty)
{
    SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(OBJ_COURT);
    long nNum = 4;
    if (!pCourt->m_bFourStages) {
        nNum = 3;
    }

    if (!bParty) {
        const unsigned char *pStageNo = _GetStageNoInfo(m_nGameKind, nCup);
        unsigned char nBits = _GetStageBit(m_nGameKind, nCup);
        unsigned char nNewBits = _GetStageNewBit(m_nGameKind, nCup);
        for (long i = 0; i < nNum; i++) {
            unsigned char nNo = *pStageNo;
            short nLabel = GetMessageLabelIndex("List_Stage", "COURT_0001");
            pCourt->SetTextStageName(i, GetMessageString("List_Stage", nLabel + nNo - 1));
            if (nBits & 1) {
                pCourt->SetStageEnable(i, 1, nNewBits & 1);
            } else {
                pCourt->SetStageEnable(i, 0, nNewBits & 1);
            }
            nBits >>= 1;
            nNewBits >>= 1;
            pStageNo++;
        }
    } else {
        const PartyStageNoInfo *pInfo = _GetPartyStageNoInfo(m_Unk9C);
        unsigned char nBits = _GetPartyStageBit(m_Unk9C, nCup);
        unsigned char nNewBits = _GetPartyStageNewBit(m_Unk9C, nCup);
        for (long i = 0; i < nNum; i++) {
            short nLabel = GetMessageLabelIndex("List_Stage", pInfo->m_Label);
            pCourt->SetTextStageName(i, GetMessageString("List_Stage", nLabel + pInfo->m_nStageNo[nCup][i]));
            if (nBits & 1) {
                pCourt->SetStageEnable(i, 1, nNewBits & 1);
            } else {
                pCourt->SetStageEnable(i, 0, nNewBits & 1);
            }
            nBits >>= 1;
            nNewBits >>= 1;
        }
    }
}

void msw::CSelectMenu::Court_In()
{
    static const long padNo0 = 0;

    unsigned char nCupMask = 0;
    long nTabType = _GetCupCount(m_nGameKind, &nCupMask);
    SendObjEventTrigger(OBJ_BG_DISPLAY, 0x4D, 0);
    _BuildCourtSelect(nTabType, nCupMask, 0);
    SendObjEventTrigger(OBJ_TASK, 0x11, (void *)GetMessageLabelString("ExhibitionMenuMessage", "BG_04_00"));
    SendObjEventTrigger(OBJ_UNK_3A, 0x33, 0);
    SendObjEventTrigger(OBJ_COURT_CHR, 0x35, (void *)1);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    for (long i = 0; i < 4; i++) {
        if (i == CMenuPadInfo::s_Instance.SelectPadLeader(0)) {
            _DisablePointStick(i, 0);
        } else {
            _DisablePointStick(i, 1);
        }
    }

    m_nRandomDecideTime[padNo0] = 0;
    m_fRandomMoveTime[padNo0] = 5.0f;
    m_UnkA6C = 0;
    if (m_Unk9D == 0x2B) {
        m_nStep = STEP_SELECT;
    } else {
        m_nStep = STEP_CAMERA;
    }
}

void msw::CSelectMenu::Court_Loop()
{
    static const long objId = OBJ_COURT;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(objId);
    if (pCourt == 0 || pCourt->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    switch (m_nStep) {
    case STEP_CAMERA: {
        CCameraMenuBG &rCamera = CCameraMenuBG::s_Instance;
        if (rCamera.IsEndAnim()) {
            rCamera.PushBack(0x25, GetCameraSpeed(0x25), 0.0f);
            SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)0xA);
            SendObjEventTrigger(OBJ_BG_CHR, 0x36, (void *)0xA);
            m_nStep = STEP_SELECT;
        }
        break;
    }
    case STEP_SELECT:
        Court_NormalSelect(nMode);
        break;
    case STEP_TAB_CHANGED: {
        unsigned short nBtnKindId = pCourt->GetNextButtonKindId(BTN_STAGE);
        pCourt->m_nPushButtonId[padNo0] = nBtnKindId;
        pCourt->SetCursorMarkerPosition(padNo0, nBtnKindId);
        m_nCurrentBtnKindId[padNo0][nMode] = nBtnKindId;
        _SetCourtDisplay(m_nStage, nBtnKindId - BTN_STAGE);
        InputDataToMenuObj(pCourt, padNo0, nPad, nMode);
        m_nStep = STEP_SELECT;
        break;
    }
    case STEP_RANDOM_INIT:
        m_fRandomMoveTime[padNo0] = 5.0f;
        m_nRandomDecideTime[padNo0] = 0;
        m_nStep = STEP_RANDOM;
        break;
    case STEP_RANDOM:
        Court_RandomSelect(nMode);
        break;
    case STEP_OK_DELAY:
        Court_OkButtonDelay();
        break;
    case STEP_OK_INIT:
        Court_OkButtonInit();
        break;
    case STEP_OK:
        Court_OkButton();
        break;
    case STEP_OK_OUT:
        Court_OkButtonOut();
        break;
    case STEP_LAYOUT_OUT:
        Court_LayoutOutWait(nMode);
        break;
    }
}

void msw::CSelectMenu::Court_Out()
{
    Scratch(OBJ_CHARA_OK);
    Scratch(OBJ_COURT_TAB);
    Scratch(OBJ_COURT);
    Scratch(OBJ_BG_DISPLAY);
}

void msw::CSelectMenu::Court_NormalSelect(long nMode)
{
    static const long objId = OBJ_COURT;
    static const long padNo0 = 0;

    long nStage = m_nStage;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCourtTab *pTab = (SelectMenu::CCourtTab *)FindObj(OBJ_COURT_TAB);
    SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(objId);
    if (pCourt == 0 || pCourt->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CCameraMenuBG::s_Instance.IsEndAnim()) {
        SetFadeIn(10.0f);
    }

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        pCourt->PushButton(padNo0, nBtnKindId);
        switch (nBtnKindId) {
        case BTN_STAGE:
        case BTN_STAGE + 1:
        case BTN_STAGE + 2:
        case BTN_STAGE + 3:
            if (_CheckStage(m_nGameKind, nStage, nBtnKindId - BTN_STAGE)) {
                pTab->AllStopTabAnim();
                SetStageSub(this, nBtnKindId - BTN_STAGE);
                m_nStep = STEP_OK_DELAY;
            }
            break;
        case BTN_CUP_TAB:
        case BTN_CUP_TAB + 1:
        case BTN_CUP_TAB + 2:
        case BTN_CUP_TAB + 3: {
            long nCup = pTab->m_nTabCup[nBtnKindId - BTN_CUP_TAB];
            if (_GetCupStageNum(m_nGameKind, nCup) >= 0) {
                int bChange = pTab->SetCupSelect(nCup);
                SetPreviousButtonKindIdWithoutNone(padNo0, pCourt->GetNextButtonKindId(BTN_STAGE), pTab);
                SetStage(this, nCup);
                if (bChange) {
                    _SetStageName(nCup, 0);
                    pCourt->SetAppearanceAnim();
                    pCourt->CallbackButtonAnimIn(0);
                    _SetCourtDisplay(nCup, -1);
                }
            }
            break;
        }
        case BTN_BACK:
            Court_CameraReverse();
            GoBackToMode(padNo0, GetPrevMode(), pCourt, 1);
            break;
        case BTN_RANDOM:
            CMenuPadInfo::s_Instance.m_aPad[padNo0].m_nFlags06 |= 0x04;
            m_nStep = STEP_RANDOM_INIT;
            break;
        case BTN_OPTION:
            SetStageSub(this, nBtnKindId - BTN_STAGE);
            m_nCurrentBtnKindId[padNo0][nMode] = 0;
            Court_ChangeStateInit(0x2B);
            break;
        }
    } else if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_B) && pCourt->m_bSendObj1) {
        Court_CameraReverse();
        GoBackToMode(padNo0, GetPrevMode(), pCourt, 1);
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_OPTION) {
        pCourt->PushButton(padNo0, BTN_OPTION);
        SetStageSub(this, nBtnKindId - BTN_STAGE);
        m_nCurrentBtnKindId[padNo0][nMode] = 0;
        Court_ChangeStateInit(0x2B);
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_RANDOM) {
        pCourt->PushButton(padNo0, BTN_RANDOM);
        CMenuPadInfo::s_Instance.m_aPad[padNo0].m_nFlags06 |= 0x04;
        m_nStep = STEP_RANDOM_INIT;
    } else {
        InputDataToMenuObj(pCourt, padNo0, nPad, nMode);
        if (!Court_CheckTabChange(nMode, 0)) {
            unsigned short nNewBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
            if (m_aPadSelect[nPad].m_Unk00 == 0) {
                if ((unsigned long)(nNewBtnKindId - BTN_STAGE) > 3) {
                    _SetCourtDisplay(nStage, -1);
                }
            } else if (m_aPadSelect[nPad].m_Unk00 == 1) {
                pTab->AllStopTabAnim();
            }
            if ((unsigned long)(nNewBtnKindId - BTN_STAGE) <= 3) {
                _SetCourtDisplay(m_nStage, nNewBtnKindId - BTN_STAGE);
            }
        }
    }
}

int msw::CSelectMenu::Court_CheckTabChange(long nMode, int bParty)
{
    static const long objId = OBJ_COURT;
    static const long padNo0 = 0;

    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(objId);
    SelectMenu::CCourtTab *pTab = (SelectMenu::CCourtTab *)FindObj(OBJ_COURT_TAB);
    if (pCourt == 0 || pCourt->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE || pTab == 0) {
        return 0;
    }

    long nCup = m_nStage;
    if (pCourt->GetStageButtonKindId(nBtnKindId) >= 0) {
        unsigned char nCupMask = 0;
        if (!bParty) {
            _GetCupCount(m_nGameKind, &nCupMask);
        } else {
            _GetPartyCupCount(m_Unk9C, &nCupMask);
        }

        // To the next open cup on the left or the right.
        if (CMenuPad::s_Instance.GetInput(nPad) & PAD_LEFT) {
            for (long i = nCup - 1; i >= 0; i--) {
                if (nCupMask & (1 << i)) {
                    nCup = i;
                    break;
                }
            }
        } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_RIGHT) {
            for (long i = nCup + 1; i < 4; i++) {
                if (nCupMask & (1 << i)) {
                    nCup = i;
                    break;
                }
            }
        }

        int bChange = pTab->SetCupSelect(nCup);
        SetStage(this, nCup);
        if (bChange) {
            _SetStageName(nCup, bParty);
            pCourt->SetAppearanceAnim();
            m_nStep = STEP_TAB_CHANGED;
            return 1;
        }
    }
    return 0;
}

void msw::CSelectMenu::Court_RandomSelect(long nMode)
{
    static const long padNo0 = 0;

    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(OBJ_COURT);
    SelectMenu::CCourtTab *pTab = (SelectMenu::CCourtTab *)FindObj(OBJ_COURT_TAB);
    if (pCourt == 0 || pCourt->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE || pTab == 0) {
        return;
    }

    // The cursor jumps to a random stage every few frames; after eight
    // jumps the stage under it is taken.
    int bMove = 0;
    int bDecide = 0;
    float fRate = bcs::CMainApp::GetAppFrameRateF();
    if (m_nRandomDecideTime[padNo0] >= s_RandomDecideTime[0]) {
        bDecide = 1;
    }
    float fTime = m_fRandomMoveTime[padNo0] - fRate;
    if (fTime <= 0.0f) {
        bMove = 1;
    }

    if (bMove == 1 || bDecide == 1) {
        long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
        if (bDecide == 0) {
            long nStage;
            long nStageSub;
            Court_RandomSelect(nStage, nStageSub);
            pCourt->m_nPushButtonId[padNo0] = nStageSub + BTN_STAGE;
            pCourt->SetCursorMarkerPosition(padNo0, nStageSub + BTN_STAGE);
            m_nCurrentBtnKindId[padNo0][nMode] = nStageSub + BTN_STAGE;
            pTab->SetCupSelect(nStage);
            _SetStageName(nStage, 0);
            SetStage(this, nStage);
            SetStageSub(this, nStageSub);
            m_nRandomDecideTime[padNo0]++;
        } else {
            pCourt->PushButton(padNo0, nBtnKindId);
            m_nStep = STEP_OK_DELAY;
            return;
        }
    }

    if (fTime <= 0.0f) {
        fTime = 5.0f;
    }
    m_fRandomMoveTime[padNo0] = fTime;
    InputDataToMenuObj(pCourt, padNo0, nPad, nMode);
    pTab->AllStopTabAnim();
}

void msw::CSelectMenu::Court_LayoutOutWait(long nMode)
{
    static const long objId = OBJ_COURT;
    static const long padNo0 = 0;

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(objId);
    if (pObj == 0 || pObj->m_pPaneAccessor != 0) {
        SetNextMode(m_nNextMode, pObj);
    }
    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_COURT_TAB);
    if (pObj == 0 || pObj->m_pPaneAccessor != 0) {
        SetNextMode(m_nNextMode, pObj);
    }
    if (m_nNextMode == 0x31) {
        m_fUnk94 = 150.0f;
    }
}

void msw::CSelectMenu::Court_ChangeStateInit(long nMode)
{
    SelectMenu::CCharaOk *pOk = (SelectMenu::CCharaOk *)FindObj(OBJ_CHARA_OK);
    if (pOk != 0 && pOk->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        pOk->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
        SendObjEventTrigger(OBJ_FADE, 0x32, 0);
    }
    m_nNextMode = nMode;
    m_nStep = STEP_LAYOUT_OUT;
}

void msw::CSelectMenu::Court_EndCamera()
{
    SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)0x11);
    SendObjEventTrigger(OBJ_COURT_CHR, 0x38, (void *)0x12);
    CCameraMenuBG::s_Instance.PushBack(0x27, GetCameraSpeed(0x27), 0.0f);
    CCameraMenuBG::s_Instance.PushBack(0x28, GetCameraSpeed(0x28), 0.0f);
}

void msw::CSelectMenu::Court_CameraReverse()
{
    CCameraMenuBG *pCamera = &CCameraMenuBG::s_Instance;
    long nAnim;
    switch ((long)m_Unk9B) {
    default:
        nAnim = 0x25;
        break;
    case 1:
        nAnim = 0x24;
        break;
    case 2:
        nAnim = 0x25;
        break;
    case 4:
        nAnim = 0x24;
        break;
    }
    pCamera->PushBack(nAnim, -1.0f * GetCameraSpeed(nAnim), 0.0f);

    switch ((long)m_Unk9B) {
    default:
        nAnim = 0x1F;
        break;
    case 1:
        nAnim = 0x1E;
        break;
    case 2:
        nAnim = 0x1F;
        break;
    case 4:
        nAnim = 0x1E;
        break;
    }
    if (nAnim == 0x1E && (m_nPlayMode == 4 || m_nPlayMode == 6)) {
        nAnim = 0x24;
    }
    SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)nAnim);
    SendObjEventTrigger(OBJ_BG_CHR, 0x36, (void *)nAnim);
}

void msw::CSelectMenu::WifiCourt_In()
{
    static const long padNo0 = 0;

    long nStage = m_nStage;
    unsigned char nCupMask = 0;
    long nTabType = _GetCupCount(m_nGameKind, &nCupMask);
    if (nStage == 0xFF) {
        m_nStage = 0;
    }
    if ((long)m_nStageSub == 0xFF) {
        m_nStageSub = 0;
    }
    SendObjEventTrigger(OBJ_BG_DISPLAY, 0x4D, 0);
    _BuildCourtSelect(nTabType, nCupMask, 0);
    SendObjEventTrigger(OBJ_TASK, 0x11, (void *)GetMessageLabelString("ExhibitionMenuMessage", "BG_04_00"));
    SendObjEventTrigger(OBJ_UNK_3A, 0x33, 0);
    SendObjEventTrigger(OBJ_COURT_CHR, 0x35, (void *)1);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    for (long i = 0; i < 4; i++) {
        if (i == CMenuPadInfo::s_Instance.SelectPadLeader(0)) {
            _DisablePointStick(i, 0);
        } else {
            _DisablePointStick(i, 1);
        }
    }

    m_fRandomMoveTime[padNo0] = 5.0f;
    m_nRandomDecideTime[padNo0] = 0;
    WifiCourt_SetUpDisplay();
    if (m_bStarMii) {
        dwc::CGameDwc::SetUpTimer(10.0f, 0);
    }
    if (SendObjEventTrigger(OBJ_UNK_01, 0x2C, 0) != 0 || m_Unk9D == 0x2A) {
        m_nStep = STEP_SELECT;
    } else {
        m_nStep = STEP_CAMERA;
    }
}

void msw::CSelectMenu::WifiCourt_Loop()
{
    static const long objId = OBJ_COURT;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(objId);
    if (pCourt == 0 || pCourt->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    switch (m_nStep) {
    case STEP_CAMERA: {
        CCameraMenuBG &rCamera = CCameraMenuBG::s_Instance;
        if (rCamera.IsEndAnim()) {
            rCamera.PushBack(0x24, GetCameraSpeed(0x24), 0.0f);
            if (m_nPlayMode == 4 || m_nPlayMode == 6) {
                SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)0xF);
                SendObjEventTrigger(OBJ_BG_CHR, 0x36, (void *)0xF);
            } else {
                SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)9);
                SendObjEventTrigger(OBJ_BG_CHR, 0x36, (void *)9);
            }
            m_nStep = STEP_SELECT;
        }
        break;
    }
    case STEP_SELECT:
        WifiCourt_NormalSelect(nMode);
        break;
    case STEP_TAB_CHANGED: {
        unsigned short nBtnKindId = pCourt->GetNextButtonKindId(BTN_STAGE);
        pCourt->m_nPushButtonId[padNo0] = nBtnKindId;
        pCourt->SetCursorMarkerPosition(padNo0, nBtnKindId);
        m_nCurrentBtnKindId[padNo0][nMode] = nBtnKindId;
        _SetCourtDisplay(m_nStage, nBtnKindId - BTN_STAGE);
        InputDataToMenuObj(pCourt, padNo0, nPad, nMode);
        m_nStep = STEP_SELECT;
        break;
    }
    case STEP_OK_DELAY:
        Court_OkButtonDelay();
        break;
    case STEP_OK_INIT:
        Court_OkButtonInit();
        break;
    case STEP_OK:
        Court_OkButton();
        break;
    case STEP_OK_OUT:
        Court_OkButtonOut();
        break;
    case STEP_LAYOUT_OUT:
        WifiCourt_LayoutOutWait(nMode);
        break;
    }
}

void msw::CSelectMenu::WifiCourt_Out()
{
    Scratch(OBJ_CHARA_OK);
    Scratch(OBJ_COURT_TAB);
    Scratch(OBJ_COURT);
    Scratch(OBJ_BG_DISPLAY);
}

void msw::CSelectMenu::WifiCourt_NormalSelect(long nMode)
{
    static const long objId = OBJ_COURT;
    static const long padNo0 = 0;

    long nStage = m_nStage;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCourtTab *pTab = (SelectMenu::CCourtTab *)FindObj(OBJ_COURT_TAB);
    SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(objId);
    if (pCourt == 0 || pCourt->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    // Out of time: the other player chooses.
    if (m_bStarMii && dwc::CGameDwc::CountTimer()) {
        dwc::CGameDwc::CloseTimer();
        pCourt->PushButton(padNo0, 6);
        SetStage(this, 0xFF);
        SetStageSub(this, 0xFF);
        Court_ChangeStateInit(0x28);
        return;
    }

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        pCourt->PushButton(padNo0, nBtnKindId);
        switch (nBtnKindId) {
        case BTN_STAGE:
        case BTN_STAGE + 1:
        case BTN_STAGE + 2:
        case BTN_STAGE + 3:
            if (_CheckStage(m_nGameKind, nStage, nBtnKindId - BTN_STAGE)) {
                SetStageSub(this, nBtnKindId - BTN_STAGE);
                m_nStep = STEP_OK_DELAY;
            }
            break;
        case BTN_CUP_TAB:
        case BTN_CUP_TAB + 1:
        case BTN_CUP_TAB + 2:
        case BTN_CUP_TAB + 3: {
            long nCup = pTab->m_nTabCup[nBtnKindId - BTN_CUP_TAB];
            if (_GetCupStageNum(m_nGameKind, nCup) >= 0) {
                int bChange = pTab->SetCupSelect(nCup);
                SetPreviousButtonKindIdWithoutNone(padNo0, pCourt->GetNextButtonKindId(BTN_STAGE), pTab);
                SetStage(this, nCup);
                if (bChange) {
                    _SetStageName(nCup, 0);
                    pCourt->SetAppearanceAnim();
                    pCourt->CallbackButtonAnimIn(0);
                    _SetCourtDisplay(nCup, -1);
                }
            }
            break;
        }
        case BTN_BACK:
            Court_CameraReverse();
            dwc::CGameDwc::CloseTimer();
            GoBackToMode(padNo0, GetPrevMode(), pCourt, 1);
            break;
        case BTN_OTHER:
            WifiCourt_SetOtherSelect(padNo0, nMode);
            break;
        }
    } else if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_B) && pCourt->m_bSendObj1) {
        if (!m_bStarMii) {
            Court_CameraReverse();
            dwc::CGameDwc::CloseTimer();
            GoBackToMode(padNo0, GetPrevMode(), pCourt, 1);
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_RANDOM) {
        pCourt->PushButton(padNo0, BTN_OTHER);
        WifiCourt_SetOtherSelect(padNo0, nMode);
    } else {
        if (!m_bStarMii) {
            if (m_pVsFriend != 0 && !m_pVsFriend->IsFriendMatchChallenger()) {
                CancelVsFriend();
            }
            if (m_pVsFriend != 0) {
                SetNextMode(0x2A, pCourt);
                return;
            }
        }
        InputDataToMenuObj(pCourt, padNo0, nPad, nMode);
        if (!Court_CheckTabChange(nMode, 0)) {
            unsigned short nNewBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
            if (m_aPadSelect[nPad].m_Unk00 == 0) {
                if ((unsigned long)(nNewBtnKindId - BTN_STAGE) > 3) {
                    _SetCourtDisplay(nStage, -1);
                }
            } else if (m_aPadSelect[nPad].m_Unk00 == 1) {
                pTab->AllStopTabAnim();
            }
            if ((unsigned long)(nNewBtnKindId - BTN_STAGE) <= 3) {
                _SetCourtDisplay(m_nStage, nNewBtnKindId - BTN_STAGE);
            }
        }
    }
}

void msw::CSelectMenu::WifiCourt_LayoutOutWait(long nMode)
{
    static const long objId = OBJ_COURT;
    static const long padNo0 = 0;

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(objId);
    if (pObj == 0 || pObj->m_pPaneAccessor != 0) {
        SetNextMode(m_nNextMode, pObj);
    }
    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_COURT_TAB);
    if (pObj == 0 || pObj->m_pPaneAccessor != 0) {
        SetNextMode(m_nNextMode, pObj);
    }
}

void msw::CSelectMenu::WifiCourt_SetUpDisplay()
{
    SelectMenu::CBgDisplay *pBg = (SelectMenu::CBgDisplay *)FindObj(OBJ_BG_DISPLAY);
    if (pBg != 0) {
        CMenuOption::s_Instance.GetMenuNum(RuleSel_GetOptionKindId());
    }
}

void msw::CSelectMenu::WifiCourt_SetOtherSelect(long nPad, long nMode)
{
    SetStage(this, 0xFF);
    SetStageSub(this, 0xFF);
    m_nCurrentBtnKindId[nPad][nMode] = 5;
    m_nStep = STEP_OK_DELAY;
}

void msw::CSelectMenu::PartyCourt_In()
{
    static const long padNo0 = 0;

    FindObj(OBJ_COURT);
    long nStage = m_nStage;
    unsigned char nCupMask = 0;
    long nTabType = _GetPartyCupCount(m_Unk9C, &nCupMask);
    if (nStage >= nTabType) {
        SetStage(this, nTabType);
    }

    // The rhythm game shows the records of its stages.
    int bRhythm = 0;
    if ((long)m_Unk9B == 2 && m_Unk9C == 3) {
        bRhythm = 1;
    }
    if (bRhythm) {
        SendObjEventTrigger(OBJ_BG_DISPLAY, 0x4D, (void *)1);
    } else {
        SendObjEventTrigger(OBJ_BG_DISPLAY, 0x4D, (void *)0);
    }
    _BuildCourtSelect(nTabType, nCupMask, 1);
    if (bRhythm) {
        SendObjEventTrigger(OBJ_TASK, 0x11, (void *)GetMessageLabelString("ExhibitionMenuMessage", "BG_RR_05"));
    } else {
        SendObjEventTrigger(OBJ_TASK, 0x11, (void *)GetMessageLabelString("ExhibitionMenuMessage", "BG_04_01"));
    }
    SendObjEventTrigger(OBJ_UNK_3A, 0x33, 0);
    SendObjEventTrigger(OBJ_COURT_CHR, 0x35, (void *)1);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    SelectMenu::CBgDisplay *pBg = (SelectMenu::CBgDisplay *)FindObj(OBJ_BG_DISPLAY);
    if (pBg != 0) {
        long nNum = CMenuOption::s_Instance.GetMenuNum(RuleSel_GetOptionKindId());
        CSave::GetPlayerSetUpData(-1);
        if (nNum == 0) {
            long nBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
            pBg->SetMessageTextNum(4);
            PartyCourt_SetRhythmScore(nBtnKindId);
        }
    }

    for (long i = 0; i < 4; i++) {
        if (i == CMenuPadInfo::s_Instance.SelectPadLeader(0)) {
            _DisablePointStick(i, 0);
        } else {
            _DisablePointStick(i, 1);
        }
    }

    m_fRandomMoveTime[padNo0] = 5.0f;
    m_nRandomDecideTime[padNo0] = 0;
    if (m_Unk9D == 0x2B) {
        m_nStep = STEP_SELECT;
    } else {
        m_nStep = STEP_CAMERA;
    }
}

void msw::CSelectMenu::PartyCourt_Loop()
{
    static const long objId = OBJ_COURT;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(objId);
    if (pCourt == 0 || pCourt->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    switch (m_nStep) {
    case STEP_CAMERA: {
        CCameraMenuBG &rCamera = CCameraMenuBG::s_Instance;
        if (rCamera.IsEndAnim()) {
            rCamera.Play(0x25, 1.0f, 0.0f);
            SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)0xA);
            SendObjEventTrigger(OBJ_BG_CHR, 0x36, (void *)0xA);
            m_nStep = STEP_SELECT;
        }
        break;
    }
    case STEP_SELECT:
        PartyCourt_NormalSelect(nMode);
        break;
    case STEP_TAB_CHANGED: {
        unsigned short nBtnKindId = pCourt->GetNextButtonKindId(BTN_STAGE);
        pCourt->m_nPushButtonId[padNo0] = nBtnKindId;
        pCourt->SetCursorMarkerPosition(padNo0, nBtnKindId);
        m_nCurrentBtnKindId[padNo0][nMode] = nBtnKindId;
        _SetPartyDisplay(m_nStage, nBtnKindId - BTN_STAGE);
        InputDataToMenuObj(pCourt, padNo0, nPad, nMode);
        m_nStep = STEP_SELECT;
        break;
    }
    case STEP_RANDOM_INIT:
        m_fRandomMoveTime[padNo0] = 5.0f;
        m_nRandomDecideTime[padNo0] = 0;
        m_nStep = STEP_RANDOM;
        break;
    case STEP_RANDOM:
        PartyCourt_RandomSelect(nMode);
        break;
    case STEP_OK_DELAY: {
        long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
        if (nBtnKindId != 0) {
            SetStageSub(this, nBtnKindId - BTN_STAGE);
            pCourt->PushButton(padNo0, nBtnKindId);
            Court_OkButtonDelay();
        }
        InputDataToMenuObj(pCourt, padNo0, nPad, nMode);
        break;
    }
    case STEP_OK_INIT:
        Court_OkButtonInit();
        break;
    case STEP_OK:
        Court_OkButton();
        break;
    case STEP_OK_OUT:
        Court_OkButtonOut();
        break;
    case STEP_LAYOUT_OUT:
        PartyCourt_LayoutOutWait(nMode);
        break;
    }
}

void msw::CSelectMenu::PartyCourt_Out()
{
    Scratch(OBJ_CHARA_OK);
    Scratch(OBJ_COURT_TAB);
    Scratch(OBJ_COURT);
    Scratch(OBJ_BG_DISPLAY);
}

void msw::CSelectMenu::PartyCourt_NormalSelect(long nMode)
{
    // Not read: the original has this constant here too (next mode 0x31,
    // perhaps of stripped code).
    static const long nextState = 0x31;
    static const long objId = OBJ_COURT;
    static const long padNo0 = 0;

    long nStage = m_nStage;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCourtTab *pTab = (SelectMenu::CCourtTab *)FindObj(OBJ_COURT_TAB);
    SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(objId);
    if (pCourt == 0 || pCourt->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CCameraMenuBG::s_Instance.IsEndAnim()) {
        SetFadeIn(10.0f);
    }

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        pCourt->PushButton(padNo0, nBtnKindId);
        switch (nBtnKindId) {
        case BTN_STAGE:
        case BTN_STAGE + 1:
        case BTN_STAGE + 2:
        case BTN_STAGE + 3:
            if (_CheckPartyStage(m_Unk9C, nStage, nBtnKindId - BTN_STAGE)) {
                SetStageSub(this, nBtnKindId - BTN_STAGE);
                m_nStep = STEP_OK_DELAY;
            }
            break;
        case BTN_CUP_TAB:
        case BTN_CUP_TAB + 1:
        case BTN_CUP_TAB + 2:
        case BTN_CUP_TAB + 3: {
            long nCup = pTab->m_nTabCup[nBtnKindId - BTN_CUP_TAB];
            if (_GetCupStageNum(m_nGameKind, nCup) >= 0) {
                int bChange = pTab->SetCupSelect(nCup);
                SetPreviousButtonKindIdWithoutNone(padNo0, pCourt->GetNextButtonKindId(BTN_STAGE), pTab);
                SetStage(this, nCup);
                if (bChange) {
                    _SetStageName(nCup, 1);
                    pCourt->SetAppearanceAnim();
                    pCourt->CallbackButtonAnimIn(0);
                    _SetPartyDisplay(nCup, -1);
                }
            }
            break;
        }
        case BTN_BACK:
            Court_CameraReverse();
            GoBackToMode(padNo0, GetPrevMode(), pCourt, 1);
            PartyCourt_ChangeStateInit(m_nNextMode);
            SendObjEventTrigger(OBJ_BG_DISPLAY, 0x4E, (void *)-1);
            PlayMenuBGM(2);
            break;
        case BTN_OTHER:
            CMenuPadInfo::s_Instance.m_aPad[padNo0].m_nFlags06 |= 0x04;
            m_nStep = STEP_RANDOM_INIT;
            break;
        }
    } else if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_B) && pCourt->m_bSendObj1) {
        Court_CameraReverse();
        GoBackToMode(padNo0, GetPrevMode(), pCourt, 1);
        PartyCourt_ChangeStateInit(m_nNextMode);
        SendObjEventTrigger(OBJ_BG_DISPLAY, 0x4E, (void *)-1);
        PlayMenuBGM(2);
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_RANDOM) {
        pCourt->PushButton(padNo0, BTN_OTHER);
        CMenuPadInfo::s_Instance.m_aPad[padNo0].m_nFlags06 |= 0x04;
        m_nStep = STEP_RANDOM_INIT;
    } else {
        InputDataToMenuObj(pCourt, padNo0, nPad, nMode);
        if (!Court_CheckTabChange(nMode, 1)) {
            unsigned short nNewBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
            if (m_aPadSelect[nPad].m_Unk00 == 0) {
                if ((unsigned long)(nNewBtnKindId - BTN_STAGE) > 3) {
                    _SetPartyDisplay(m_nStage, -1);
                }
            } else if (m_aPadSelect[nPad].m_Unk00 == 1) {
                pTab->AllStopTabAnim();
            }
            if ((unsigned long)(nNewBtnKindId - BTN_STAGE) <= 3) {
                _SetPartyDisplay(m_nStage, nNewBtnKindId - BTN_STAGE);
            }
        }
    }
}

void msw::CSelectMenu::PartyCourt_RandomSelect(long nMode)
{
    static const long padNo0 = 0;

    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(OBJ_COURT);
    SelectMenu::CCourtTab *pTab = (SelectMenu::CCourtTab *)FindObj(OBJ_COURT_TAB);
    if (pCourt == 0 || pCourt->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE || pTab == 0) {
        return;
    }

    // As Court_RandomSelect, over the party game's stages.
    int bMove = 0;
    int bDecide = 0;
    float fRate = bcs::CMainApp::GetAppFrameRateF();
    if (m_nRandomDecideTime[padNo0] >= s_RandomDecideTime[0]) {
        bDecide = 1;
    }
    float fTime = m_fRandomMoveTime[padNo0] - fRate;
    if (fTime <= 0.0f) {
        bMove = 1;
    }

    if (bMove == 1 || bDecide == 1) {
        long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
        if (bDecide == 0) {
            long nStage;
            long nStageSub;
            while (true) {
                unsigned char nCupMask = 0;
                nStage = bcs::BCS_RandRange(0L, 3L);
                nStageSub = bcs::BCS_RandRange(0L, 3L);
                if (_GetPartyCupCount(m_Unk9C, &nCupMask) == 0) {
                    // One cup: its first stage when it has a single one.
                    nStage = _BeginBit(nCupMask);
                    if (_CupStageBitCount(_GetPartyStageBit(m_Unk9C, nStage)) == 0) {
                        nStageSub = _BeginBit(_GetPartyStageBit(m_Unk9C, nStage));
                        continue;
                    }
                    if ((1 << nStageSub) & _GetPartyStageBit(m_Unk9C, nStage)) {
                        break;
                    }
                } else if (((1 << nStage) & nCupMask) && ((1 << nStageSub) & _GetPartyStageBit(m_Unk9C, nStage))) {
                    break;
                }
            }
            pCourt->m_nPushButtonId[padNo0] = nStageSub + BTN_STAGE;
            pCourt->SetCursorMarkerPosition(padNo0, nStageSub + BTN_STAGE);
            m_nCurrentBtnKindId[padNo0][nMode] = nStageSub + BTN_STAGE;
            pTab->SetCupSelect(nStage);
            _SetStageName(nStage, 1);
            SetStage(this, nStage);
            SetStageSub(this, nStageSub);
            m_nRandomDecideTime[padNo0]++;
        } else {
            pCourt->PushButton(padNo0, nBtnKindId);
            m_nStep = STEP_OK_DELAY;
            return;
        }
    }

    if (fTime <= 0.0f) {
        fTime = 5.0f;
    }
    m_fRandomMoveTime[padNo0] = fTime;
    InputDataToMenuObj(pCourt, padNo0, nPad, nMode);
    pTab->AllStopTabAnim();
}

void msw::CSelectMenu::PartyCourt_LayoutOutWait(long nMode)
{
    static const long objId = OBJ_COURT;
    static const long padNo0 = 0;

    if (!CCameraMenuBG::s_Instance.IsEndAnim()) {
        return;
    }
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(objId);
    if (pObj == 0 || pObj->m_pPaneAccessor != 0) {
        SetNextMode(m_nNextMode, pObj);
    }
    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_COURT_TAB);
    if (pObj == 0 || pObj->m_pPaneAccessor != 0) {
        SetNextMode(m_nNextMode, pObj);
    }
}

void msw::CSelectMenu::PartyCourt_ChangeStateInit(long nMode)
{
    SelectMenu::CCharaOk *pOk = (SelectMenu::CCharaOk *)FindObj(OBJ_CHARA_OK);
    if (pOk != 0 && pOk->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        pOk->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
        SendObjEventTrigger(OBJ_FADE, 0x32, 0);
    }
    m_nNextMode = nMode;
    m_nStep = STEP_LAYOUT_OUT;
}

void msw::CSelectMenu::PartyCourt_SetRhythmScore(long nBtnKindId)
{
    // What is left of it: every result is dropped, as in the game.
    if (m_Unk9C == 3) {
        CSave::GetPlayerSetUpData(-1);
        FindObj(OBJ_BG_DISPLAY);
        ((SelectMenu::CCourt *)FindObj(OBJ_COURT))->GetStageButtonKindId(nBtnKindId);
        GetMessageLabelIndex("ExhibitionMenuMessage", "BG_RR_01");
    }
}

void msw::CSelectMenu::Court_OkButtonDelay()
{
    ((SelectMenu::CCourtTab *)FindObj(OBJ_COURT_TAB))->AllStopTabAnim();
    menu::CVibEff::s_Instance.SetEnable(1);
    SendObjEventTrigger(OBJ_FADE, 0x31, (void *)0x78);
    m_fEntryErrorWait = 15.0f;
    SetFadeIn(10.0f);
    m_nStep = STEP_OK_INIT;
}

void msw::CSelectMenu::Court_OkButtonInit()
{
    static const long objId = OBJ_CHARA_OK;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    if (m_fEntryErrorWait > 0.0f) {
        return;
    }

    SelectMenu::CCharaOk *pOk = (SelectMenu::CCharaOk *)FindObj(objId);
    pOk->m_bCharaMode = 0;
    Build(objId);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    for (long i = 0; i < 4; i++) {
        if (i == CMenuPadInfo::s_Instance.SelectPadLeader(0)) {
            _DisablePointStick(i, 0);
        } else {
            _DisablePointStick(i, 1);
        }
    }
    if (pOk != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
        pOk->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = BTN_OK_YES;
        pOk->m_nPushButtonId[padNo0] = BTN_OK_YES;
        m_nStep = STEP_OK;
    }

    switch ((long)m_Unk9B) {
    case 4:
        if ((long)m_nStage != 0xFF) {
            _SetCourtDisplay(m_nStage, m_nStageSub);
        }
        break;
    case 0:
        _SetCourtDisplay(m_nStage, m_nStageSub);
        break;
    case 2:
        _SetPartyDisplay(m_nStage, m_nStageSub);
        break;
    }
}

void msw::CSelectMenu::Court_OkButton()
{
    static const long padNo0 = 0;
    static const long objId = OBJ_CHARA_OK;

    long nMode = m_nMode;
    SelectMenu::CCourtTab *pTab = (SelectMenu::CCourtTab *)FindObj(OBJ_COURT_TAB);
    SelectMenu::CCharaOk *pOk = (SelectMenu::CCharaOk *)FindObj(objId);
    if (pOk == 0 || pOk->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    pTab->AllStopTabAnim();
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    // Out of time on Wi-Fi: the other player chooses.
    if ((long)m_Unk9B == 4 && m_bStarMii && dwc::CGameDwc::CountTimer()) {
        dwc::CGameDwc::CloseTimer();
        SendObjEventTrigger(OBJ_UNK_01, 0x2B, 0);
        SetStage(this, 0xFF);
        SetStageSub(this, 0xFF);
        m_nCurrentBtnKindId[padNo0][nMode] = 5;
        if (!m_bStarMii) {
            Court_ChangeStateInit(0x21);
        } else {
            Court_ChangeStateInit(0x28);
        }
        return;
    }

    int bCancel = 0;
    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        _GetModeChangeWaitTime();
        switch (nBtnKindId) {
        case BTN_OK_YES:
            dwc::CGameDwc::CloseTimer();
            pOk->PushNext();
            pOk->PushButton(padNo0, nBtnKindId);
            SendObjEventTrigger(OBJ_UNK_01, 0x52, 0);
            SendObjEventTrigger(OBJ_UNK_01, 0x2B, 0);
            switch ((long)m_Unk9B) {
            case 4:
                if (!m_bStarMii) {
                    Court_ChangeStateInit(0x21);
                } else {
                    Court_ChangeStateInit(0x28);
                }
                break;
            case 0:
                Court_ChangeStateInit(0x31);
                Court_EndCamera();
                break;
            case 2:
                Court_ChangeStateInit(0x31);
                Court_EndCamera();
                break;
            }
            return;
        case BTN_OK_NO:
            bCancel = 1;
            break;
        default:
            return;
        }
    }

    if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_B) || bCancel) {
        pOk->PushCancel();
        pOk->PushButton(padNo0, BTN_OK_NO);
        pOk->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
        SendObjEventTrigger(OBJ_FADE, 0x32, 0);
        m_nStep = STEP_OK_OUT;
        return;
    }

    if (!m_bStarMii) {
        if (m_pVsFriend != 0 && !m_pVsFriend->IsFriendMatchChallenger()) {
            CancelVsFriend();
        }
        if (m_pVsFriend != 0) {
            SendObjEventTrigger(OBJ_FADE, 0x32, 0);
            SetNextMode(0x2A, pOk);
            return;
        }
    }
    InputDataToMenuObj(pOk, padNo0, nPad, nMode);
}

void msw::CSelectMenu::Court_OkButtonOut()
{
    SelectMenu::CCharaOk *pOk = (SelectMenu::CCharaOk *)FindObj(OBJ_CHARA_OK);
    if (pOk != 0 && pOk->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        return;
    }

    Scratch(OBJ_CHARA_OK);
    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    // Back to the stage that was chosen before the window.
    if (m_nStage == 0xFF) {
        m_nStage = m_nPrevStage;
    }
    if (m_nStageSub == 0xFF) {
        m_nStageSub = m_nPrevStageSub;
    }
    if ((long)m_nStageSub != 0xFF) {
        SelectMenu::CCourt *pCourt = (SelectMenu::CCourt *)FindObj(OBJ_COURT);
        unsigned short nBtnKindId = m_nStageSub + BTN_STAGE;
        m_nCupBtnKindId[nPad] = nBtnKindId;
        m_nCurrentBtnKindId[nPad][nMode] = nBtnKindId;
        pCourt->m_nPushButtonId[nPad] = nBtnKindId;
        pCourt->SetCursorMarkerPosition(nPad, nBtnKindId);
    }
    m_nStep = STEP_SELECT;
}

void msw::CSelectMenu::Court_RandomSelect(long &rStage, long &rStageSub)
{
    while (true) {
        unsigned char nCupMask = 0;
        rStage = bcs::BCS_RandRange(0L, 3L);
        rStageSub = bcs::BCS_RandRange(0L, 2L);
        long nTabType = _GetCupCount(m_nGameKind, &nCupMask);
        if (nTabType == 0) {
            // One cup: its first stage when it has a single one.
            rStage = _BeginBit(nCupMask);
            long nStageNum = _GetCupStageNum(m_nGameKind, rStage);
            if (nStageNum == 0) {
                unsigned char nStageBits = _GetStageBit(m_nGameKind, rStage);
                rStageSub = _BeginBit(nStageBits);
                continue;
            }
            if ((1 << rStageSub) & _GetStageBit(m_nGameKind, rStage)) {
                break;
            }
        } else if (((1 << rStage) & nCupMask) && ((1 << rStageSub) & _GetStageBit(m_nGameKind, rStage))) {
            break;
        }
    }
}
