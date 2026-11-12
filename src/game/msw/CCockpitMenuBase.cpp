#include "game/msw/CCockpitMenuBase.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitPointer.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CMainStep.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartList.hpp"

// This unit is a slice of a wider file: CCockpitButtonInfo,
// CCockpitWindowInfo, CCockpitPartsInfo, this class and
// CCockpitMenuBaseButtonA follow one another in the DOL (0x801ECE98 to
// 0x801EF868). The constructor (0x801EDAC8) and StickMoveCheck sit outside
// this slice. Step ids: 1 in, 2 loop, 3 out; m_nState 3 means the layout is
// in, 4 and 5 that it is going out.

namespace {
    // The cockpit object id of the pointer display.
    const long COCKPIT_OBJ_POINTER = 0x1016;
    // Button kind ids of a cockpit menu start here.
    const long BTN_KIND_ID_TOP = 0xCF;
    // CMainStep's state while the menus run.
    const long MAIN_STEP_MENU = 2;
}

msw::CCockpitMenuBase::~CCockpitMenuBase()
{
    if (m_nStep != 0) {
        EndMenuPadParameter(CMenuPadInfo::s_Instance.GetPadLeader());
    }
    if (m_Unk88 != 0) {
        CCockpit::RemovePointerObj();
    }
    if (CCockpitButtonInfo::m_nNum >= 1) {
        CCockpit *pCockpit = CCockpit::GetTask();
        if (pCockpit != 0) {
            pCockpit->CommitResult();
        }
    }
}

void msw::CCockpitMenuBase::Main()
{
    float fTimer = m_Unk8C;
    float fTime = fTimer - bcs::CMainApp::GetAppFrameRateF();
    if (fTime < 0.0f) {
        fTime = 0.0f;
    }
    m_Unk8C = fTime;

    StepControl();
    StateControl();
    MenuPartListStateCheck();
}

void msw::CCockpitMenuBase::StateControl()
{
}

void msw::CCockpitMenuBase::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

long msw::CCockpitMenuBase::SendEvent(long nEvent, void *pParam)
{
    return 0;
}

void msw::CCockpitMenuBase::StepControl()
{
    switch (m_nStep) {
        case 1:
            StepIn();
            return;
        case 2:
            StepLoop();
            return;
        case 3:
            StepOut();
            return;
    }
}

void msw::CCockpitMenuBase::InitMenuPadParameter(unsigned char nPadMask)
{
    if (CCockpitButtonInfo::m_nNum == 0) {
        return;
    }

    CMenuPadInfo *pInfo = &CMenuPadInfo::s_Instance;
    for (long i = 0; i < 4; i++) {
        unsigned short nBtnKindId = m_nDefaultBtnKindId[i];
        if (CMenuPad::s_Instance.IsInputPointerMode(i)) {
            nBtnKindId = 0;
        }
        pInfo->SetPadState(i, nBtnKindId);
    }

    if (m_Unk88 == 0) {
        return;
    }

    for (long i = 0; i < 4; i++) {
        CMenuPadInfo::PAD &rPad = pInfo->m_aPad[i];
        if (nPadMask & (1 << i)) {
            rPad.m_nFlags06 &= ~4;
            rPad.m_nFlags04 |= 4;
        } else {
            rPad.m_nFlags06 |= 4;
            rPad.m_nFlags04 &= ~4;
        }
        pInfo->m_aPad[i].m_nPlayerNo = pInfo->GetDefaultPlayerNo(i);
    }
}

void msw::CCockpitMenuBase::EndMenuPadParameter(long nPad)
{
    if (CCockpitButtonInfo::m_nNum == 0) {
        return;
    }
    if ((unsigned long)nPad > 3) {
        return;
    }

    CMenuPadInfo *pInfo = &CMenuPadInfo::s_Instance;
    pInfo->SetPadState(nPad, 0);
    if (m_Unk88 != 0) {
        pInfo->m_aPad[nPad].m_nPlayerNo = pInfo->GetDefaultPlayerNo(nPad);
        pInfo->m_aPad[nPad].m_nFlags06 |= 4;
        pInfo->m_aPad[nPad].m_nFlags04 &= ~4;
    }
}

void msw::CCockpitMenuBase::CursorControl(long nPad)
{
    CMenuPadInfo *pInfo = &CMenuPadInfo::s_Instance;
    unsigned char nFlags = pInfo->GetPadFlags(nPad);
    if (!(nFlags & 1) || !(nFlags & 4)) {
        return;
    }

    CCockpitPointer *pPointer = CCockpit::GetPointerObj();
    if (pPointer == 0) {
        return;
    }

    long nKind = pInfo->GetPlayerNo(nPad);
    long nBtnKindId;
    if (CMenuPad::GetInputInstance()->IsInputPointerMode(nPad)) {
        bcs::FVEC3 pos;
        pPointer->GetPointerPos(nPad, &pos);
        nBtnKindId = HitCheck(pos);
        nKind &= ~0x40;
    } else {
        nBtnKindId = (unsigned short)StickMoveCheck(nPad, pInfo->GetPadState(nPad));
        nKind |= 0x40;
        CCockpit::PointerCursorCheck(nPad, this, pPointer);
    }

    if (OnCursorCheck(nPad, nBtnKindId) == 1) {
        pInfo->SetPadState(nPad, nBtnKindId);
    }
    pPointer->SetPointerKind(nPad, nKind);
}

void msw::CCockpitMenuBase::StepIn()
{
    if (m_nState != 3) {
        return;
    }
    if (CCockpit::GetTask() == 0) {
        return;
    }
    if (CGamePadCheck::m_PadCheckStap != 1) {
        return;
    }

    if (!CCockpit::IsEndObj(COCKPIT_OBJ_POINTER) || CMainStep::GetStep() == MAIN_STEP_MENU) {
        m_Unk88 = 0;
    } else {
        m_Unk88 = 1;
    }
    if (m_Unk88 != 0) {
        CCockpit::CreatePointerObj(990.0f);
    }

    long nLeader = CMenuPadInfo::s_Instance.GetPadLeader();
    InitMenuPadParameter(1 << nLeader);

    // Only the leading pad starts on a default button: move the first
    // default to it.
    for (long i = 0; i < 4; i++) {
        unsigned short nBtnKindId = m_nDefaultBtnKindId[i];
        if (nBtnKindId != 0) {
            if (nLeader != i) {
                for (long j = 0; j < 4; j++) {
                    unsigned short nId = (j == nLeader) ? nBtnKindId : (unsigned short)0;
                    m_nDefaultBtnKindId[j] = nId;
                    CMenuPadInfo::s_Instance.SetPadState(j, nId);
                }
            }
            break;
        }
    }

    m_nStep = 2;
}

void msw::CCockpitMenuBase::StepLoop()
{
    if (CCockpit::GetTask() == 0) {
        return;
    }

    CMenuPad *pPad = CMenuPad::GetInputInstance();
    float fWait = m_Unk8C;
    long nLeader = CMenuPadInfo::s_Instance.GetPadLeader();
    if (fWait == 0.0f && (pPad->GetInput(nLeader) & 0x10)) {
        long nButton = (unsigned short)PushButton(nLeader) - BTN_KIND_ID_TOP;
        SetResultButton(nButton);
        if ((unsigned long)nButton <= 3) {
            m_nStep = 3;
        }
    }
    CursorControl(nLeader);
}

void msw::CCockpitMenuBase::StepOut()
{
    long nLeader = CMenuPadInfo::s_Instance.GetPadLeader();
    EndMenuPadParameter(nLeader);

    CCockpit *pCockpit = CCockpit::GetTask();
    if (pCockpit != 0 && pCockpit->GetUnk70() == -1) {
        SetResultButton(m_nDefaultBtnKindId[nLeader] - BTN_KIND_ID_TOP);
    }
    if (m_Unk88 != 0) {
        CCockpit::RemovePointerObj();
    }
    DecideResult();
    m_nStep = 0;
}

void msw::CCockpitMenuBase::Setup()
{
    Set();
    WriteText();
    if (CCockpitButtonInfo::m_nNum > 0) {
        CCockpit::ClearResult(-1);
    }
}

void msw::CCockpitMenuBase::WriteText()
{
}

long msw::CCockpitMenuBase::PushButton(long nPad)
{
    unsigned short nBtnKindId = CMenuPadInfo::s_Instance.GetPadState(nPad);

    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        DecideResult();
        return nBtnKindId;
    }
    return 0;
}

long msw::CCockpitMenuBase::CancelMenu()
{
    m_nStep = 3;
    return 0;
}

void msw::CCockpitMenuBase::MenuOut()
{
    CancelMenu();
}

int msw::CCockpitMenuBase::IsMenuOut()
{
    int bOut = 0;
    if (m_nState == 4 || m_nState == 5 || m_nState == 0) {
        bOut = 1;
    }
    return bOut;
}
