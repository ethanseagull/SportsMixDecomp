#include "game/msw/CSelectMenuLayoutBase.hpp"

#include "bcs/G2D/CLayoutDL.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "bcs/New.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/CInfoTabChange.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CMenuPartList.hpp"
#include "game/msw/CSelectMenu.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/CVibEff.hpp"

const float msw::CSelectMenuLayoutBase::s_DefaultOutAnimFrameSize = 30.0f;
const float msw::CSelectMenuLayoutBase::s_DefaultOutAnimOverTime = 4.0f;

namespace {
    // (name guessed; no symbol) Makes a CPaneCtrl on archive pAccessor and
    // hands it to the layout manager; null when either is missing. Inline in
    // the game: the same sequence is CCockpitLayoutBase::NewPaneCtrl.
    inline bcs::G2D::CPaneCtrl *NewPaneCtrl(nw4r::lyt::ArcResourceAccessor *pAccessor, long nUnk)
    {
        if (pAccessor == 0) {
            return 0;
        }

        bcs::G2D::CLytManager *pManager = bcs::G2D::CLytManager::GetInstance();
        // (0L: a plain 0 is ambiguous here with <new>'s placement form,
        // which <functional> brings in.)
        bcs::G2D::CPaneCtrl *pPaneCtrl = new (0L) bcs::G2D::CPaneCtrl;

        if (pPaneCtrl == 0) {
            return 0;
        }
        pManager->AddObject(pPaneCtrl, nUnk);
        pPaneCtrl->CreateLayoutObject(nUnk);
        pPaneCtrl->AttachArchiveResource(pAccessor);
        return pPaneCtrl;
    }
}

msw::CSelectMenuLayoutBase::CSelectMenuLayoutBase(long nId)
    : CMenuLinkBase(nId)
{
    m_pArcFont = 0;
    m_pPaneAccessor = 0;
    m_Unk40 = 0;
    ClearPushButtonId();
    m_Unk4C = 0;
    m_bSeEnable = 0;
    m_bVibEnable = 0;
    m_pMenuPartList = 0;
    m_Unk34 = 0;
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::CSelectMenuLayoutBase::~CSelectMenuLayoutBase()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteArcFont();
    if (m_Unk4C != 0) {
        delete[] m_Unk4C;
    }
    m_Unk4C = 0;
}

void msw::CSelectMenuLayoutBase::Main()
{
}

void msw::CSelectMenuLayoutBase::Disp()
{
}

void msw::CSelectMenuLayoutBase::SetVisible(int bVisible)
{
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    if (pPaneCtrl != 0) {
        pPaneCtrl->SetVisible(bVisible);
    }
}

long msw::CSelectMenuLayoutBase::SendEvent(long nEvent, void *pParam)
{
    return -1;
}

long msw::CSelectMenuLayoutBase::SetPaneAccessor(bcs::G2D::CPaneCtrl *pPaneCtrl, const char **ppPaneNames, long nNum)
{
    SetTextBoxFromMessage(pPaneCtrl);
    m_pPaneAccessor = new CMenuPaneAccessor(pPaneCtrl, ppPaneNames, nNum);
    return m_pPaneAccessor != 0 ? 0 : -1;
}

void msw::CSelectMenuLayoutBase::DeletePaneAccessor()
{
    if (m_pPaneAccessor != 0) {
        delete m_pPaneAccessor;
    }
    m_pPaneAccessor = 0;
}

void msw::CSelectMenuLayoutBase::DeleteArcFont()
{
    if (m_pArcFont != 0) {
        bcs::G2D::RemoveArchiveFont(m_pArcFont);
        m_pArcFont = 0;
    }
}

void msw::CSelectMenuLayoutBase::ClearPushButtonId()
{
    for (int i = 0; i < 4; i++) {
        m_nPushButtonId[i] = 0;
    }
}

long msw::CSelectMenuLayoutBase::GetCurrentButtonKindId(long nPad)
{
    CSelectMenu *pMenu = GetSelectMenu();
    if (pMenu == 0) {
        return 0;
    }
    return pMenu->m_nCurrentBtnKindId[nPad][pMenu->m_nMode];
}

bcs::G2D::CPaneCtrl *msw::CSelectMenuLayoutBase::CreateLytObj(nw4r::lyt::ArcResourceAccessor *pAccessor, long nUnk,
                                                              long nGDLMemArea)
{
    bcs::G2D::CPaneCtrl *pPaneCtrl;

    if (CLytCtrl::GetInstance() == 0) {
        pPaneCtrl = 0;
    } else {
        pPaneCtrl = NewPaneCtrl(pAccessor, nUnk);
        if (pPaneCtrl != 0) {
            m_nHandle[m_nNum] = pPaneCtrl->m_nId;
            pPaneCtrl->GetLayout()->SetGDLMemArea(nGDLMemArea);
            m_nNum++;
        }
    }
    return pPaneCtrl;
}

void msw::CSelectMenuLayoutBase::CloseMenu()
{
    m_Unk30 = STATE_CLOSE;
}

unsigned long msw::CSelectMenuLayoutBase::SE_Play(long nSe, long nUnk)
{
    if (m_bSeEnable != 0) {
        return menu::CSndEff::s_Instance.Play(nSe, nUnk);
    }
    return 0;
}

unsigned long msw::CSelectMenuLayoutBase::Voice_Play(long nVoice, long nUnk)
{
    if (m_bSeEnable != 0) {
        return menu::CSndEff::s_Instance.Play(nVoice, nUnk);
    }
    return 0;
}

void msw::CSelectMenuLayoutBase::Voice_Stop(long nVoice)
{
    if (m_bSeEnable == 0) {
        return;
    }
    menu::CSndEff::s_Instance.Stop(nVoice);
}

void msw::CSelectMenuLayoutBase::Vib_Start(long nPad, long nPattern, unsigned long nUnk)
{
    if (m_bVibEnable == 0) {
        return;
    }
    menu::CVibEff::s_Instance.Play(nPad, nPattern, nUnk);
}

void msw::CSelectMenuLayoutBase::DeleteMenuPartList()
{
    if (m_pMenuPartList != 0) {
        delete m_pMenuPartList;
        m_pMenuPartList = 0;
    }
}

void msw::CSelectMenuLayoutBase::SetTextBoxFromMessage(bcs::G2D::CLayoutBase *pLayout)
{
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    pLytCtrl->SetTextBoxFromMessage(pLayout, pLayout->GetLayout()->m_pMessageName);
}

int msw::CSelectMenuLayoutBase::IsBackFrameIn(long nPad)
{
    CSelectMenu *pMenu = GetSelectMenu();
    if (pMenu == 0) {
        return 0;
    }
    return pMenu->IsBackButtonMove(nPad);
}

int msw::CSelectMenuLayoutBase::IsBackFrameOut(long nPad)
{
    return GetCurrentButtonKindId(nPad) == 1;
}

// msw::CMenuPartButton is in this file too: its callbacks are bound to
// CSelectMenuLayoutBase members, and the std::tr1::function instances they
// create are emitted at the end of this file (MenuF .text 0x1271C..0x14204,
// merged 2026-11-12).

namespace {
    // (name guessed; no symbol) Calls fCallBack when it is set. A helper in
    // the game: the argument is read before the test, as a by-value
    // parameter is.
    template <class F, class A1>
    inline void CallBack(const F &fCallBack, A1 a1)
    {
        if (fCallBack) {
            fCallBack(a1);
        }
    }

    template <class F, class A1, class A2>
    inline void CallBack(const F &fCallBack, A1 a1, A2 a2)
    {
        if (fCallBack) {
            fCallBack(a1, a2);
        }
    }
}

msw::CMenuPartButton::CMenuPartButton(long nId)
    : CMenuPartBase(nId)
{
}

// The game's destructor has no call to ~CMenuPartBase, and this file carries
// a weak copy of CMenuPartBase::Disp: both are inline in the game's header.
// CMenuPartBase.hpp declares them so (empty bodies), and the empty base
// destructor inlines to nothing here.
msw::CMenuPartButton::~CMenuPartButton()
{
}

long msw::CMenuPartButton::SetButtonIn()
{
    long nResult = CMenuPartBase::SetButtonIn();
    CallBack(m_CallBackAnimIn, m_nNextBtnKindId[0]);
    return nResult;
}

long msw::CMenuPartButton::SetButtonOut()
{
    long nResult = CMenuPartBase::SetButtonOut();
    CallBack(m_CallBackAnimOut, m_nNextBtnKindId[0]);
    return nResult;
}

long msw::CMenuPartButton::SetButtonPush()
{
    long nResult = CMenuPartBase::SetButtonPush();
    CallBack(m_CallBackAnimPush, m_nNextBtnKindId[0]);
    return nResult;
}

long msw::CMenuPartButton::SetButtonPushCancel()
{
    long nResult = CMenuPartBase::SetButtonPushCancel();
    CallBack(m_CallBackAnimCancel, m_nNextBtnKindId[0]);
    return nResult;
}

long msw::CMenuPartButton::SetButtonMultiIn(long nArg)
{
    long nResult = CMenuPartBase::SetButtonMultiIn(nArg);
    CallBack(m_CallBackAnimMultiIn, nArg, m_nNextBtnKindId[0]);
    return nResult;
}

long msw::CMenuPartButton::SetButtonMultiOut(long nArg)
{
    long nResult = CMenuPartBase::SetButtonMultiOut(nArg);
    CallBack(m_CallBackAnimMultiOut, nArg, m_nNextBtnKindId[0]);
    return nResult;
}

void msw::CMenuPartButton::SetCallBackAnimIn(CSelectMenuLayoutBase *pLayout)
{
    m_CallBackAnimIn = std::tr1::bind(&CSelectMenuLayoutBase::CallbackButtonAnimIn, pLayout,
                                      std::tr1::placeholders::_1);
}

void msw::CMenuPartButton::SetCallBackAnimOut(CSelectMenuLayoutBase *pLayout)
{
    m_CallBackAnimOut = std::tr1::bind(&CSelectMenuLayoutBase::CallbackButtonAnimOut, pLayout,
                                       std::tr1::placeholders::_1);
}

void msw::CMenuPartButton::SetCallBackAnimPush(CSelectMenuLayoutBase *pLayout)
{
    m_CallBackAnimPush = std::tr1::bind(&CSelectMenuLayoutBase::CallbackButtonAnimPush, pLayout,
                                        std::tr1::placeholders::_1);
}

void msw::CMenuPartButton::SetCallBackAnimCancel(CSelectMenuLayoutBase *pLayout)
{
    m_CallBackAnimCancel = std::tr1::bind(&CSelectMenuLayoutBase::CallbackButtonAnimCancel, pLayout,
                                          std::tr1::placeholders::_1);
}

void msw::CMenuPartButton::SetCallBackAnimMultiIn(CSelectMenuLayoutBase *pLayout)
{
    m_CallBackAnimMultiIn = std::tr1::bind(&CSelectMenuLayoutBase::CallbackButtonAnimMultiIn, pLayout,
                                           std::tr1::placeholders::_1, std::tr1::placeholders::_2);
}

void msw::CMenuPartButton::SetCallBackAnimMultiOut(CSelectMenuLayoutBase *pLayout)
{
    m_CallBackAnimMultiOut = std::tr1::bind(&CSelectMenuLayoutBase::CallbackButtonAnimMultiOut, pLayout,
                                            std::tr1::placeholders::_1, std::tr1::placeholders::_2);
}

long msw::CInfoTabChange::IncTabNo(long nTabNo)
{
    nTabNo++;
    if (nTabNo >= m_nTabNum) {
        nTabNo = 0;
    }
    m_nTabNo = nTabNo;
    return nTabNo;
}

long msw::CInfoTabChange::DecTabNo(long nTabNo)
{
    nTabNo--;
    if (nTabNo < 0) {
        nTabNo = m_nTabNum - 1;
    }
    m_nTabNo = nTabNo;
    return nTabNo;
}
