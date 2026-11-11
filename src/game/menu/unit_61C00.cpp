#include "bcs/CMainApp.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/CSelectMenu.hpp"
#include "game/msw/SelectMenu/CCharaOk.hpp"
#include "game/msw/SelectMenu/CCourtChr.hpp"
#include "game/msw/SelectMenu/CCupChr.hpp"
#include "game/msw/SelectMenu/CCupSelect.hpp"
#include "game/msw/SelectMenu/CCupSelectTab.hpp"
#include "game/msw/menu/CVibEff.hpp"

// The cup select screen of CSelectMenu (MenuF): the difficulty tabs
// (SelectMenu::CCupSelectTab, menu object 0x34), the cup buttons
// (SelectMenu::CCupSelect, object 0x33), the cup on its stand
// (SelectMenu::CCupChr, object 0x41) and the "OK?" window
// (SelectMenu::CCharaOk, object 0x13). Names are guesses.

namespace {
    enum {
        OBJ_CHARA_OK = 0x13,
        OBJ_CUP_SELECT = 0x33,
        OBJ_CUP_SELECT_TAB = 0x34,
        OBJ_BG = 0x35,
        OBJ_COURT_CHR = 0x3B,
        OBJ_FADE = 0x3F,
        OBJ_CUP_CHR = 0x41,
        OBJ_BG_CHR = 0x42
    };

    enum {
        BTN_BACK = 1,
        BTN_OK_YES = 0x4E,
        BTN_OK_NO = 0x4F,
        BTN_CUP = 0x8010,     // to 0x8012, the three cups
        BTN_BG = 0x8020,      // and 0x8021, the two backgrounds
        BTN_LEVEL = 0x8030,   // to 0x8032, the difficulty tabs
        BTN_OPTION = 0x80A1
    };

    enum {
        PAD_LEFT = 0x8,
        PAD_RIGHT = 0x2,
        PAD_A = 0x10,
        PAD_B = 0x20,
        PAD_OPTION = 0x400
    };
}

void msw::CSelectMenu::Cup_In()
{
    long nLevel = m_nStage;
    unsigned char nCupCount = 0;
    _GetCupCount(m_nGameKind, &nCupCount);
    long nMode = m_nMode;
    SelectMenu::CCupSelectTab *pTab = (SelectMenu::CCupSelectTab *)FindObj(OBJ_CUP_SELECT_TAB);
    SelectMenu::CCupSelect *pCup = (SelectMenu::CCupSelect *)FindObj(OBJ_CUP_SELECT);

    pTab->m_nLevelNum = _DifficultyBitCount();
    Build(OBJ_BG);
    Build(OBJ_CUP_SELECT_TAB);
    Build(OBJ_CUP_SELECT);

    if (_DifficultyBitCount() == 0) {
        SendObjEventTrigger(0, 0x11, (void *)GetMessageLabelString("TournamentMenuMessage", "CUP_09_00"));
    } else {
        SendObjEventTrigger(0, 0x11, (void *)GetMessageLabelString("TournamentMenuMessage", "CUP_09_01"));
    }

    SelectMenu::CCourtChr *pCourt = (SelectMenu::CCourtChr *)FindObj(OBJ_COURT_CHR);
    if (pCourt) {
        pCourt->SetDispEnable(0);
        pCourt->SetCharaDispEnable(1);
    }

    Build(OBJ_CUP_CHR);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    _SetCupBit(nLevel);
    if (nLevel == 0xFF) {
        m_nPrevStage = m_nStage;
    }
    m_nStage = nLevel;
    pTab->SetDifficultySelect(nLevel);

    m_nCupModel = 1;
    long nCursor = pCup->SetDefaultMoveCursor(0, m_nCurrentBtnKindId[0][nMode]);
    long nBtnKindId = _CheckCupBit(nLevel, nCursor - BTN_CUP) + BTN_CUP;
    m_nCurrentBtnKindId[0][nMode] = nBtnKindId;
    pCup->SetCursorMarkerPosition(0, nBtnKindId);
    if (m_Unk23E == 1) {
        pCup->m_nPushButtonId[0] = nBtnKindId;
    }
    _BgChangeDisp(nBtnKindId);

    if (m_Unk9D == 0x2B) {
        SendObjEventTrigger(OBJ_CUP_CHR, SelectMenu::CCupChr::EVENT_PLAY_ANIM, (void *)0xA);
        m_nCupModel = 0;
        _SetCupCharaObj(this, m_nCupModel);
        m_nPrevCupModel = m_nCupModel;
        m_nStep = 1;
    } else {
        m_nStep = 0;
    }
}

void msw::CSelectMenu::Cup_Loop()
{
    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCupSelectTab *pTab = (SelectMenu::CCupSelectTab *)FindObj(OBJ_CUP_SELECT_TAB);
    SelectMenu::CCupSelect *pCup = (SelectMenu::CCupSelect *)FindObj(OBJ_CUP_SELECT);

    switch (m_nStep) {
    case 0: {
        CCameraMenuBG::s_Instance.PushBack(0x24, 1.0f, 0.0f);
        long nAnim = 9;
        if (m_nPlayMode == 6 || m_nPlayMode == 4) {
            nAnim = 0xF;
        }
        SendObjEventTrigger(OBJ_COURT_CHR, SelectMenu::CCourtChr::EVENT_PLAY_ANIM, (void *)nAnim);
        SendObjEventTrigger(OBJ_BG_CHR, 0x36, (void *)nAnim);
        SendObjEventTrigger(OBJ_CUP_CHR, SelectMenu::CCupChr::EVENT_PLAY_ANIM, (void *)0xA);
        m_nCupModel = 0;
        _SetCupCharaObj(this, m_nCupModel);
        m_nPrevCupModel = m_nCupModel;
        if (m_Unk23E == 0 && m_nCurrentBtnKindId[0][nMode] == 0) {
            SendObjEventTrigger(OBJ_BG, 0x2F, 0);
        }
        m_nStep = 1;
        break;
    }

    case 1:
        if (pCup && pCup->m_Unk30 == CSelectMenuLayoutBase::STATE_ACTIVE) {
            long nBtnKindId = m_nCurrentBtnKindId[0][nMode];
            if ((unsigned long)(nBtnKindId - BTN_CUP) <= 2 && m_nCupModel != nBtnKindId - BTN_CUP + 1) {
                m_nCupModel = nBtnKindId - BTN_CUP + 1;
                m_nStep = 10;
                break;
            }

            if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
                pCup->PushButton(0, nBtnKindId);
                switch (nBtnKindId) {
                case BTN_CUP:
                case BTN_CUP + 1:
                case BTN_CUP + 2:
                    if (nBtnKindId - BTN_CUP == 0xFF) {
                        m_nPrevStageSub = m_nStageSub;
                    }
                    m_nStageSub = nBtnKindId - BTN_CUP;
                    pCup->PushButton(0, nBtnKindId);
                    m_nStep = 0x1E;
                    break;
                case BTN_BG:
                case BTN_BG + 1:
                    _TabChange(nBtnKindId - BTN_BG);
                    pCup->CallbackButtonAnimIn(0);
                    _BgChangeDisp(nBtnKindId);
                    break;
                case BTN_LEVEL:
                case BTN_LEVEL + 1:
                case BTN_LEVEL + 2:
                    _TabChange(nBtnKindId - BTN_LEVEL);
                    pCup->CallbackButtonAnimIn(0);
                    break;
                case BTN_BACK:
                    SendObjEventTrigger(OBJ_CUP_CHR, SelectMenu::CCupChr::EVENT_PLAY_ANIM, (void *)0xD);
                    _SetCupCharaObj(this, 0);
                    m_nStep = 0x28;
                    break;
                case BTN_OPTION:
                    m_nCurrentBtnKindId[0][nMode] = 0;
                    SetNextMode(0x2B, pCup);
                    break;
                }
                break;
            }

            if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_OPTION) && m_nGameKind != 5) {
                if (m_nGameKind == 1 || m_nPlayerNum == 1) {
                    pCup->PushButton(0, BTN_OPTION);
                    m_nCurrentBtnKindId[0][nMode] = 0;
                    SetNextMode(0x2B, pCup);
                    break;
                }
            } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
                SendObjEventTrigger(OBJ_CUP_CHR, SelectMenu::CCupChr::EVENT_PLAY_ANIM, (void *)0xD);
                _SetCupCharaObj(this, 0);
                m_nStep = 0x28;
                break;
            }

            InputDataToMenuObj(pCup, 0, nPad, nMode);
            if (m_Unk23E == 0) {
                long nNewBtnKindId = m_nCurrentBtnKindId[0][nMode];
                if (nBtnKindId != nNewBtnKindId) {
                    _BgChangeDisp(nNewBtnKindId);
                }
            } else {
                int bTabChange = Cup_CheckTabChange();
                long nNewBtnKindId = m_nCurrentBtnKindId[0][nMode];
                if (bTabChange || nBtnKindId != nNewBtnKindId) {
                    // As _BgChangeDisp, without the background buttons.
                    pTab->AllStopTabAnim();
                    if ((unsigned long)(nNewBtnKindId - BTN_CUP) <= 2) {
                        _SetCupCharaObj(this, m_nCupModel);
                        SendObjEventTrigger(OBJ_BG, 0x2E, 0);
                    } else {
                        _SetCupCharaObj(this, 0);
                        SendObjEventTrigger(OBJ_BG, 0x2F, 0);
                    }
                }
            }
        }
        break;

    case 2:
        SetWhiteFadeOut(60.0f);
        m_nStep = 3;
        break;

    case 3:
        if (!bcs::CMainApp::IsPlayMasterFade()) {
            SetNextMode(m_nNextMode, pCup);
            SetNextMode(m_nNextMode, pTab);
        }
        break;

    case 10:
        if (m_nPrevCupModel < m_nCupModel) {
            SendObjEventTrigger(OBJ_CUP_CHR, SelectMenu::CCupChr::EVENT_PLAY_ANIM, (void *)0xB);
        } else {
            SendObjEventTrigger(OBJ_CUP_CHR, SelectMenu::CCupChr::EVENT_PLAY_ANIM, (void *)0xD);
        }
        _SetCupCharaObj(this, 0);
        m_nStep = 0xB;
        break;

    case 11: {
        SelectMenu::CCupChr *pCupChr = (SelectMenu::CCupChr *)FindObj(OBJ_CUP_CHR);
        if (pCupChr && pCupChr->IsAnimEnd()) {
            long nAnim = 9;
            if (m_nPlayMode == 6 || m_nPlayMode == 4) {
                nAnim = 0xF;
            }
            SendObjEventTrigger(OBJ_COURT_CHR, SelectMenu::CCourtChr::EVENT_PLAY_ANIM, (void *)nAnim);
            SendObjEventTrigger(OBJ_BG_CHR, 0x36, (void *)nAnim);
            if (m_nPrevCupModel < m_nCupModel) {
                SendObjEventTrigger(OBJ_CUP_CHR, SelectMenu::CCupChr::EVENT_PLAY_ANIM, (void *)0xA);
            } else {
                SendObjEventTrigger(OBJ_CUP_CHR, SelectMenu::CCupChr::EVENT_PLAY_ANIM, (void *)0xC);
            }
            _SetCupCharaObj(this, m_nCupModel);
            m_nPrevCupModel = m_nCupModel;
            m_nStep = 1;
        }
        break;
    }

    case 0x1E:
        Cup_OkButtonDelay();
        break;

    case 0x1F:
        Cup_OkButtonInit();
        break;

    case 0x20:
        Cup_OkButton();
        break;

    case 0x21:
        Cup_OkButtonOut();
        break;

    case 0x28: {
        SendObjEventTrigger(OBJ_BG, 0x2E, 0);
        CSelectMenuLayoutBase *pBg = (CSelectMenuLayoutBase *)FindObj(OBJ_BG);
        if (!pBg || pBg->m_Unk30 == CSelectMenuLayoutBase::STATE_NONE) {
            Court_CameraReverse();
            GoBackToMode(0, GetPrevMode(), pCup, 1);
        }
        break;
    }
    }
}

void msw::CSelectMenu::Cup_Out()
{
    Scratch(OBJ_BG);
    Scratch(OBJ_CUP_SELECT_TAB);
    Scratch(OBJ_CUP_SELECT);
    Scratch(OBJ_CUP_CHR);
}

int msw::CSelectMenu::Cup_CheckTabChange()
{
    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCupSelectTab *pTab = (SelectMenu::CCupSelectTab *)FindObj(OBJ_CUP_SELECT_TAB);
    SelectMenu::CCupSelect *pCup = (SelectMenu::CCupSelect *)FindObj(OBJ_CUP_SELECT);
    if (!pCup || pCup->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return 0;
    }

    long nLevel = m_nStage;
    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_LEFT) {
        if (--nLevel < 0) {
            nLevel = 0;
        } else {
            m_nCurrentBtnKindId[0][nMode] = BTN_CUP;
            pCup->m_nPushButtonId[0] = BTN_CUP;
            pCup->SetCursorMarkerPosition(0, BTN_CUP);
        }
        int bChange = pTab->SetDifficultySelect(nLevel);
        pTab->GetCupBtnKindId(nLevel);
        if (nLevel == 0xFF) {
            m_nPrevStage = m_nStage;
        }
        m_nStage = nLevel;
        if (bChange) {
            _SetCupBit(nLevel);
            pCup->SetAppearanceAnim();
            pCup->CallbackButtonAnimIn(0);
            return 1;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_RIGHT) {
        nLevel++;
        if (nLevel > _DifficultyBitCount()) {
            nLevel = _DifficultyBitCount();
        } else {
            m_nCurrentBtnKindId[0][nMode] = BTN_CUP;
            pCup->m_nPushButtonId[0] = BTN_CUP;
            pCup->SetCursorMarkerPosition(0, BTN_CUP);
        }
        int bChange = pTab->SetDifficultySelect(nLevel);
        pTab->GetCupBtnKindId(nLevel);
        if (nLevel == 0xFF) {
            m_nPrevStage = m_nStage;
        }
        m_nStage = nLevel;
        if (bChange) {
            _SetCupBit(nLevel);
            pCup->SetAppearanceAnim();
            pCup->CallbackButtonAnimIn(0);
            return 1;
        }
    }
    return 0;
}

void msw::CSelectMenu::Cup_ChangeStateInit(long nMode)
{
    SelectMenu::CCharaOk *pOk = (SelectMenu::CCharaOk *)FindObj(OBJ_CHARA_OK);
    if (pOk && pOk->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        pOk->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
        SendObjEventTrigger(OBJ_FADE, 0x32, 0);
    }
    m_nNextMode = nMode;
    m_nStep = 2;
}

long msw::CSelectMenu::_DifficultyBitCount()
{
    CSave::PlayerData *pData = CSave::GetPlayerData(-1);
    long nCount = 0;
    // Left unset for other game kinds, as in the game.
    unsigned char nBits;
    switch (m_nGameKind) {
    case 0:
        nBits = pData->m_Cup[0].m_nLevelBit;
        break;
    case 1:
        nBits = pData->m_Cup[1].m_nLevelBit;
        break;
    case 2:
        nBits = pData->m_Cup[2].m_nLevelBit;
        break;
    case 3:
        nBits = pData->m_Cup[3].m_nLevelBit;
        break;
    case 5:
        if (pData->m_Cup[0].m_nLevelBit & 8) {
            return 0;
        }
        break;
    }

    for (long i = 0; i < 3; i++) {
        if (nBits & 1) {
            nCount++;
        }
        nBits >>= 1;
    }
    return nCount - 1;
}

void msw::CSelectMenu::_SetCupBit(long nLevel)
{
    CSave::PlayerData *pData = CSave::GetPlayerData(-1);
    SelectMenu::CCupSelect *pCup = (SelectMenu::CCupSelect *)FindObj(OBJ_CUP_SELECT);
    if (!pCup) {
        return;
    }

    // Left unset for other game kinds, as in the game.
    unsigned char nCupBit;
    unsigned char nNewBit;
    switch (m_nGameKind) {
    case 0:
        nCupBit = pData->m_Cup[0].m_nCupBit[nLevel];
        nNewBit = pData->m_Cup[0].m_nCupNewBit[nLevel];
        break;
    case 1:
        nCupBit = pData->m_Cup[1].m_nCupBit[nLevel];
        nNewBit = pData->m_Cup[1].m_nCupNewBit[nLevel];
        break;
    case 2:
        nCupBit = pData->m_Cup[2].m_nCupBit[nLevel];
        nNewBit = pData->m_Cup[2].m_nCupNewBit[nLevel];
        break;
    case 3:
        nCupBit = pData->m_Cup[3].m_nCupBit[nLevel];
        nNewBit = pData->m_Cup[3].m_nCupNewBit[nLevel];
        break;
    case 5:
        nCupBit = pData->m_Cup[0].m_nCupBit[3];
        nNewBit = pData->m_Cup[0].m_nCupNewBit[3];
        break;
    }

    for (long i = 0; i < 3; i++) {
        if (nCupBit & 1) {
            pCup->SetStageEnable(i, 1, nNewBit & 1);
        } else {
            pCup->SetStageEnable(i, 0, nNewBit & 1);
        }
        nCupBit >>= 1;
        nNewBit >>= 1;
    }
}

long msw::CSelectMenu::_CheckCupBit(long nLevel, long nCup)
{
    CSave::PlayerData *pData = CSave::GetPlayerData(-1);
    // Left unset for other game kinds, as in the game.
    unsigned char nCupBit;
    switch (m_nGameKind) {
    case 0:
        nCupBit = pData->m_Cup[0].m_nCupBit[nLevel];
        break;
    case 1:
        nCupBit = pData->m_Cup[1].m_nCupBit[nLevel];
        break;
    case 2:
        nCupBit = pData->m_Cup[2].m_nCupBit[nLevel];
        break;
    case 3:
        nCupBit = pData->m_Cup[3].m_nCupBit[nLevel];
        break;
    case 5:
        nCupBit = pData->m_Cup[0].m_nCupBit[3];
        break;
    }

    if (nCupBit & (1 << nCup)) {
        return nCup;
    }
    for (long i = 0; i < 3; i++) {
        if (nCupBit & 1) {
            return i;
        }
        nCupBit >>= 1;
    }
    return 0;
}

void msw::CSelectMenu::_TabChange(long nLevel)
{
    SelectMenu::CCupSelectTab *pTab = (SelectMenu::CCupSelectTab *)FindObj(OBJ_CUP_SELECT_TAB);
    SelectMenu::CCupSelect *pCup = (SelectMenu::CCupSelect *)FindObj(OBJ_CUP_SELECT);
    if (!pCup || pCup->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    int bChange = pTab->SetDifficultySelect(nLevel);
    SetPreviousButtonKindIdWithoutNone(0, pTab->GetCupBtnKindId(nLevel), pTab);
    if (nLevel == 0xFF) {
        m_nPrevStage = m_nStage;
    }
    m_nStage = nLevel;
    if (bChange) {
        _SetCupBit(nLevel);
        pCup->SetAppearanceAnim();
    }
}

void msw::CSelectMenu::_SetCupCharaObj(CSelectMenu *pMenu, long nCup)
{
    SelectMenu::CCupChr *pCupChr = (SelectMenu::CCupChr *)pMenu->FindObj(OBJ_CUP_CHR);
    if (pCupChr) {
        long nKind = m_nGameKind;
        if (nKind == 5) {
            nKind = 4;
        }
        pCupChr->SetCharaModel(0, nCup, nKind, 0);
    }
}

void msw::CSelectMenu::Cup_OkButtonDelay()
{
    ((SelectMenu::CCupSelectTab *)FindObj(OBJ_CUP_SELECT_TAB))->AllStopTabAnim();
    menu::CVibEff::s_Instance.SetEnable(1);
    SendObjEventTrigger(OBJ_FADE, 0x31, (void *)0x78);
    m_fEntryErrorWait = 15.0f;
    m_nStep = 0x1F;
}

void msw::CSelectMenu::Cup_OkButtonInit()
{
    long nMode = m_nMode;
    if (m_fEntryErrorWait > 0.0f) {
        return;
    }

    SelectMenu::CCharaOk *pOk = (SelectMenu::CCharaOk *)FindObj(OBJ_CHARA_OK);
    pOk->m_bCharaMode = 0;
    Build(OBJ_CHARA_OK);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    // The two cursor flags of pad 0 (see CMenuPadInfo::IsUsedCursor).
    CMenuPadInfo::s_Instance.m_aPad[0].m_nFlags06 &= ~0x01;
    CMenuPadInfo::s_Instance.m_aPad[0].m_nFlags06 &= ~0x02;
    if (pOk) {
        pOk->SetDefaultMoveCursor(0, m_nCurrentBtnKindId[0][nMode]);
        m_nCurrentBtnKindId[0][nMode] = BTN_OK_YES;
        pOk->m_nPushButtonId[0] = BTN_OK_YES;
        m_nStep = 0x20;
    }
}

void msw::CSelectMenu::Cup_OkButton()
{
    long nMode = m_nMode;
    SelectMenu::CCupSelectTab *pTab = (SelectMenu::CCupSelectTab *)FindObj(OBJ_CUP_SELECT_TAB);
    SelectMenu::CCharaOk *pOk = (SelectMenu::CCharaOk *)FindObj(OBJ_CHARA_OK);
    if (!pOk || pOk->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    pTab->AllStopTabAnim();
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    int bCancel = 0;
    unsigned short nBtnKindId = m_nCurrentBtnKindId[0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        SendObjEventTrigger(1, 0x52, 0);
        _GetModeChangeWaitTime();
        switch (nBtnKindId) {
        case BTN_OK_YES:
            SendObjEventTrigger(1, 0x2B, 0);
            pOk->PushNext();
            pOk->PushButton(0, nBtnKindId);
            Cup_ChangeStateInit(0x31);
            return;
        case BTN_OK_NO:
            bCancel = 1;
            break;
        }
    }

    if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_B) || bCancel) {
        pOk->PushCancel();
        pOk->PushButton(0, BTN_OK_NO);
        pOk->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
        SendObjEventTrigger(OBJ_FADE, 0x32, 0);
        m_nStep = 0x21;
        return;
    }

    InputDataToMenuObj(pOk, 0, nPad, nMode);
}

void msw::CSelectMenu::Cup_OkButtonOut()
{
    SelectMenu::CCharaOk *pOk = (SelectMenu::CCharaOk *)FindObj(OBJ_CHARA_OK);
    if (pOk && pOk->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        return;
    }

    Scratch(OBJ_CHARA_OK);
    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCupSelect *pCup = (SelectMenu::CCupSelect *)FindObj(OBJ_CUP_SELECT);
    unsigned short nBtnKindId = m_nStageSub + BTN_CUP;
    m_nCupBtnKindId[nPad] = nBtnKindId;
    m_nCurrentBtnKindId[nPad][nMode] = nBtnKindId;
    pCup->m_nPushButtonId[nPad] = nBtnKindId;
    pCup->SetCursorMarkerPosition(nPad, nBtnKindId);
    m_nStep = 1;
}

void msw::CSelectMenu::_BgChangeDisp(long nBtnKindId)
{
    switch (nBtnKindId) {
    case BTN_BG:
        SendObjEventTrigger(OBJ_BG, 0x50, (void *)0);
        _SetCupCharaObj(this, 0);
        break;
    case BTN_BG + 1:
        SendObjEventTrigger(OBJ_BG, 0x50, (void *)1);
        _SetCupCharaObj(this, 0);
        break;
    default:
        if ((unsigned long)(nBtnKindId - BTN_CUP) <= 2) {
            _SetCupCharaObj(this, m_nCupModel);
            SendObjEventTrigger(OBJ_BG, 0x2E, 0);
        } else {
            _SetCupCharaObj(this, 0);
            SendObjEventTrigger(OBJ_BG, 0x2F, 0);
        }
        break;
    }
}
