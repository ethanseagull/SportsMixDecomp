#include "game/msw/CCockpitLayoutBase.hpp"

#include "bcs/G2D/CLayoutBase.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitPartMove.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/CMenuPartBase.hpp"
#include "game/msw/CMenuPartList.hpp"

msw::CCockpitLayoutBase::CCockpitLayoutBase(long nId)
    : CMenuLinkBase(nId)
{
    m_nStep = 0;
    m_nState = 0;
    m_pPaneAccessor = 0;
    m_pMenuPartList = 0;
    for (int i = 0; i < 4; i++) {
        m_nDefaultBtnKindId[i] = 0;
    }
}

msw::CCockpitLayoutBase::CCockpitLayoutBase(long nId, float fSortZ)
    : CMenuLinkBase(nId)
    , m_fSortZ(fSortZ)
{
    m_nStep = 0;
    m_nState = 0;
    m_pPaneAccessor = 0;
    m_pMenuPartList = 0;
    for (int i = 0; i < 4; i++) {
        m_nDefaultBtnKindId[i] = 0;
    }
}

msw::CCockpitLayoutBase::~CCockpitLayoutBase()
{
    if (m_pMenuPartList != 0) {
        delete m_pMenuPartList;
    }
    m_pMenuPartList = 0;
    DeletePaneAccessor();
}

void msw::CCockpitLayoutBase::Main()
{
}

void msw::CCockpitLayoutBase::Disp()
{
}

void msw::CCockpitLayoutBase::SetVisible(int bVisible)
{
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    if (pPaneCtrl != 0) {
        pPaneCtrl->SetVisible(bVisible);
    }
}

long msw::CCockpitLayoutBase::SendEvent(long nEvent, void *pParam)
{
    return -1;
}

long msw::CCockpitLayoutBase::SetPaneAccessor(bcs::G2D::CPaneCtrl *pPaneCtrl, const char **ppPaneNames, long nNum)
{
    m_pPaneAccessor = new CMenuPaneAccessor(pPaneCtrl, ppPaneNames, nNum);
    return m_pPaneAccessor != 0 ? 0 : -1;
}

void msw::CCockpitLayoutBase::DeletePaneAccessor()
{
    if (m_pPaneAccessor != 0) {
        delete m_pPaneAccessor;
    }
    m_pPaneAccessor = 0;
}

void msw::CCockpitLayoutBase::DeleteMenuPartList()
{
    if (m_pMenuPartList != 0) {
        delete m_pMenuPartList;
        m_pMenuPartList = 0;
    }
}

void msw::CCockpitLayoutBase::MenuPartListStateCheck()
{
    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::CCockpitLayoutBase::SetDefaultBtnKindId(const unsigned short *pBtnKindId)
{
    for (int i = 0; i < 4; i++) {
        m_nDefaultBtnKindId[i] = pBtnKindId[i];
        m_nBtnKindId[i] = pBtnKindId[i];
    }
}

unsigned short msw::CCockpitLayoutBase::HitCheck(bcs::FVEC3 &rPos)
{
    return m_pMenuPartList->CheckHitButton(&rPos);
}

int msw::CCockpitLayoutBase::OnCursorCheck(long nPad, unsigned short nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);

    if (CMenuPadInfo::s_Instance.GetCursorMode() == 1) {
        if (pPart != 0) {
            m_nBtnKindId[nPad] = nBtnKindId;
        }
        return 1;
    }
    return pPart != 0;
}

long msw::CCockpitLayoutBase::StickMoveCheck(long nPad, unsigned short nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        return m_nBtnKindId[nPad];
    }
    return m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

int msw::CCockpitLayoutBase::IsButtonKindOfStickMode(unsigned short nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::CCockpitLayoutBase::CheckButtonAnim()
{
    // guess: a local, so the list is read before GetPadStates as in the original
    CMenuPartList *pList = m_pMenuPartList;
    pList->CheckButtonAnim(CMenuPadInfo::s_Instance.GetPadStates(), 0);
    return 0;
}

long msw::CCockpitLayoutBase::PushButton(long nPad)
{
    unsigned short nBtnKindId = CMenuPadInfo::s_Instance.GetPadState(nPad);

    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        m_nState = 4;
        return nBtnKindId;
    }
    return 0;
}

long msw::CCockpitLayoutBase::CancelMenu()
{
    if (m_Unk08 != 0) {
        return -1;
    }
    if (GetLytObj() != 0 && m_pPaneAccessor != 0) {
        return CancelMenuImpl();
    }
    m_Unk08 = 2;
    return 0;
}

long msw::CCockpitLayoutBase::CancelMenuImpl()
{
    m_nState = 4;
    return 0;
}

void msw::CCockpitLayoutBase::SetTextBoxFromMessage(bcs::G2D::CLayoutBase *pLayout)
{
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    pLytCtrl->SetTextBoxFromMessage(pLayout, pLayout->GetLayout()->m_pMessageName);
}

int msw::CCockpitFrameOutBase::IsWaitAnim(bcs::G2D::CPaneCtrl *pPaneCtrl, long nAnim)
{
    if (pPaneCtrl == 0) {
        return 0;
    }
    if (pPaneCtrl->IsAnimLoop(nAnim)) {
        return 0;
    }
    return !(pPaneCtrl->GetAnimFrameSize(nAnim) < 2.0f);
}

// msw::CCockpitPartMove is part of this file in the original: its code
// follows IsWaitAnim, its vtable comes before CCockpitLayoutBase's in .data,
// and its static initialiser comes before this file's destructor thunk.

msw::CCockpitPartMove msw::CCockpitPartMove::s_Instance;

msw::CCockpitPartMove::~CCockpitPartMove() {}

void msw::CCockpitPartMove::Initialize()
{
    nw4r::ut::List_Init(&m_List, 0);
}

void msw::CCockpitPartMove::Finalize()
{
    for (;;) {
        PartMoveInfo *pInfo = static_cast<PartMoveInfo *>(nw4r::ut::List_GetPrev(&m_List, 0));
        if (pInfo == 0) {
            break;
        }
        nw4r::ut::List_Remove(&m_List, pInfo);
        delete pInfo;
    }
}

long msw::CCockpitPartMove::Push(long nPart, long nArg0, long nArg1)
{
    PartMoveInfo *pInfo = new PartMoveInfo;
    if (pInfo == 0) {
        return -1;
    }
    pInfo->m_nPart = nPart;
    pInfo->m_nArg0 = nArg0;
    pInfo->m_nArg1 = nArg1;
    nw4r::ut::List_Append(&m_List, pInfo);
    return 0;
}

long msw::CCockpitPartMove::Pop(PartMoveInfo &rInfo)
{
    PartMoveInfo *pInfo = static_cast<PartMoveInfo *>(nw4r::ut::List_GetNext(&m_List, 0));
    if (pInfo == 0) {
        return -1;
    }
    // The original reads the last field first.
    long nArg1 = pInfo->m_nArg1;
    long nArg0 = pInfo->m_nArg0;
    long nPart = pInfo->m_nPart;
    rInfo.m_nPart = nPart;
    rInfo.m_nArg0 = nArg0;
    rInfo.m_nArg1 = nArg1;
    nw4r::ut::List_Remove(&m_List, pInfo);
    delete pInfo;
    return 0;
}
