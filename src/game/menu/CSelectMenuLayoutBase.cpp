#include "game/msw/CSelectMenuLayoutBase.hpp"

#include "bcs/G2D/CLayoutDL.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "bcs/New.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
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
        bcs::G2D::CPaneCtrl *pPaneCtrl = new (0) bcs::G2D::CPaneCtrl;

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
