#include "game/msw/CCockpitTournamentMenuWinTrophy.hpp"

#include "bcs/G2D/CPaneObj.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartBase.hpp"
#include "game/msw/menu/PadStyle.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/CMenuPartList.hpp"

namespace {

    enum {
        PANE_WINDOW_S,
        PANE_WINDOW,
        PANE_A_BTN,
        PANE_TWO_BTN,
        PANE_WINDOW_LINE,
        PANE_TEXT,
        PANE_NUM
    };

    enum {
        ANIM_A_BTN_0,
        ANIM_A_BTN_1,
        ANIM_A_BTN_2,
        ANIM_IN,
        ANIM_LOOP,
        ANIM_OUT,
        ANIM_SCROLL,
        ANIM_NUM
    };

    enum {
        BUTTON_NUM = 1
    };

    // What m_nState goes through.
    enum {
        STATE_SET = 1,
        STATE_IN,
        STATE_WAIT,
        STATE_PUSH,
        STATE_OUT
    };

    const char *s_pLayoutName = "TO_WIN_I.brlyt";

    const char *s_pAnimName[ANIM_NUM] = {
        "TO_WIN_I_A_btn.brlan",
        "TO_WIN_I_A_btn.brlan",
        "TO_WIN_I_A_btn.brlan",
        "TO_WIN_I_inout_00.brlan",
        "TO_WIN_I_inout_01.brlan",
        "TO_WIN_I_inout_02.brlan",
        "TO_WIN_I_scroll.brlan",
    };

    const char *s_pPaneName[PANE_NUM] = {
        "Window_s",
        "Window",
        "A_btn",
        "two_btn",
        "Windou_line",
        "t_Window_00",
    };

    const msw::CMenuPartBase::Param s_ButtonParam[BUTTON_NUM] = {
        { 1, 0xFFFF, 0xFFFF, { 1, 1, 1, 1, 2 }, { 0xCF, 0, 0, 0, 0 } },
    };

    const short s_WindowTextPaneId[4] = { PANE_TEXT, 0, 0, 0 };

    const unsigned short s_DefaultBtnKindId[4] = { 0xCF, 0, 0, 0 };

    // (guessed) See the destructor.
    inline void DestructorBody() {}
}

msw::CCockpitTournamentMenuWinTrophy::CCockpitTournamentMenuWinTrophy(long nId, float fSortZ, unsigned long)
    : CCockpitMenuBaseButtonA(nId, fSortZ, BUTTON_NUM, 1, 0)
{
    SetWindowTextPaneId(s_WindowTextPaneId);
    SetDefaultBtnKindId(s_DefaultBtnKindId);
}

msw::CCockpitTournamentMenuWinTrophy::~CCockpitTournamentMenuWinTrophy()
{
    // The original body is not empty: the compiler stores the vtable
    // pointers here, which it leaves out when a destructor has no statement.
    // Only a call brings them back (';', '(void)0;' and '{}' do not), so the
    // original called something that was inlined to nothing. What it was is
    // not known.
    DestructorBody();
}

int msw::CCockpitTournamentMenuWinTrophy::Set()
{
    CCockpit *pCockpit = CCockpit::GetTask();

    if (pCockpit == 0) {
        return -1;
    }

    const char *pArchiveName;
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -2;
    }

    pArchiveName = pCockpit->GetArchiveFileName(1);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor == 0) {
        return -3;
    }

    bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 6, 0);

    if (pPaneCtrl == 0) {
        return -5;
    }

    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
    pPaneCtrl->BuildLayout(0, s_pLayoutName);
    pPaneCtrl->BuildAnim(0, s_pAnimName, ANIM_NUM);
    pPaneCtrl->SetSortZ(m_fSortZ);
    SetPaneAccessor(pPaneCtrl, s_pPaneName, PANE_NUM);

    CMenuPartList *pMenuPartList = new CMenuPartList;

    pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;
    for (long i = 0; i < BUTTON_NUM; i++) {
        CMenuPartBase *pPart = new CMenuPartBase(i);

        pPart->m_pList = pMenuPartList;
        pMenuPartList->Add(pPart);

        // The original checks the index here; probably an inlined lookup.
        const CMenuPartBase::Param *pParam = i < BUTTON_NUM ? &s_ButtonParam[i] : 0;

        pPart->m_nBtnKindId = pParam->m_nBtnKindId;
        pPart->m_Unk16 = pParam->m_Unk02;
        pPart->m_nCurrentBtnKindId = pParam->m_nBtnKindId;
        pPart->m_nAnim[0] = pParam->m_nAnim[0];
        pPart->m_nAnim[1] = pParam->m_nAnim[1];
        pPart->m_nAnim[2] = pParam->m_nAnim[2];
        pPart->m_nAnim[3] = pParam->m_nAnim[3];
        pPart->m_nAnim[4] = pParam->m_nAnim[4];
        pPart->m_nNextBtnKindId[0] = pParam->m_nNextBtnKindId[0];
        pPart->m_nSeCode[0] = 0xBC0;
        pPart->m_nSeCode[1] = 0xBBB;
        pPart->m_nSeCode[2] = 1;
        pPart->m_bUnk40_0004 = 0;
        pPart->m_bUnk40_0020 = 0;
        pPart->InitButtonAnim();
    }
    m_pMenuPartList = pMenuPartList;

    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    nw4r::lyt::Pane *pPaneABtn = m_pPaneAccessor->GetPane(PANE_A_BTN);
    nw4r::lyt::Pane *pPaneTwoBtn = m_pPaneAccessor->GetPane(PANE_TWO_BTN);

    menu::SetDispPaneByPadStyle(pPaneABtn, pPaneTwoBtn, CMenuPad::s_Instance.GetPadStyle(nPad));
    m_nStep = 1;
    m_nState = STATE_SET;
    return 0;
}

void msw::CCockpitTournamentMenuWinTrophy::WriteText()
{
    InfoParameterInitialize(this, s_WindowTextPaneId, m_pPaneAccessor, 0);
}

void msw::CCockpitTournamentMenuWinTrophy::StateControl()
{
    switch (m_nState) {
        case STATE_SET: {
            bcs::G2D::CPaneObj *pPaneObj = m_pPaneAccessor->GetPaneObj(PANE_WINDOW_S);

            if (pPaneObj != 0) {
                pPaneObj->PlayAnimation(ANIM_IN, 1.0f, 0.0f, 1, 0, 1);
            }
            m_pPaneAccessor->PlayAnimation(PANE_WINDOW_LINE, ANIM_SCROLL, 1.0f, 0.0f, 1, 0);
            m_nState = STATE_IN;
            break;
        }

        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WINDOW_S, ANIM_IN)) {
                m_nState = STATE_WAIT;
            }
            break;

        case STATE_WAIT:
            CheckButtonAnim();
            break;

        case STATE_PUSH:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                bcs::G2D::CPaneObj *pPaneObj = m_pPaneAccessor->GetPaneObj(PANE_WINDOW_S);

                if (pPaneObj != 0) {
                    pPaneObj->PlayAnimation(ANIM_OUT, 1.0f, 0.0f, 1, 0, 1);
                }
                m_nState = STATE_OUT;
            }
            break;

        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WINDOW_S, ANIM_OUT)) {
                m_Unk08 = 2;
            }
            break;
    }
}
