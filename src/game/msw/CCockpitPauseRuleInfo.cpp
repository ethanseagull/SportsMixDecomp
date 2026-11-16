// msw::CCockpitPauseRuleInfo: the page of the pause menu that shows the
// rules of the game, a title and two lines of message text. The pause menus
// fill the texts in once the layout exists (SetTitleText, SetMessageText).
#include "game/msw/CCockpitPauseLayoutBase.hpp"

#include <wchar.h>
#include "bcs/G2D/CPaneCtrl.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitPause.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"

namespace {
    // The layout of each kind: 0 the mission rule page, 1 the rule page.
    const char *s_pLayoutName[2] = {
        "pose_mission_rule.brlyt",
        "pose_rule.brlyt",
    };

    // Animations, by index into the kind's table.
    enum {
        ANIM_IN,
        ANIM_WAIT_IN, // (meaning guessed) played once the page is in
        ANIM_OUT,
        ANIM_START,   // (meaning guessed) played by Set
        ANIM_NUM
    };

    const char *s_pMissionRuleAnimName[ANIM_NUM] = {
        "pose_mission_rule_inout_00.brlan",
        "pose_mission_rule_inout_01.brlan",
        "pose_mission_rule_inout_02.brlan",
        "pose_mission_rule_inout_03.brlan",
    };

    const char *s_pRuleAnimName[ANIM_NUM] = {
        "pose_rule_inout_00.brlan",
        "pose_rule_inout_01.brlan",
        "pose_rule_inout_02.brlan",
        "pose_rule_inout_03.brlan",
    };

    const char **s_ppAnimName[2] = {
        s_pMissionRuleAnimName,
        s_pRuleAnimName,
    };

    // Panes, by index into s_pPaneName.
    enum {
        PANE_INOUT,
        PANE_TITLE,
        PANE_MES_00, // the message lines, MESSAGE_LINE_NUM of them
        PANE_NUM = PANE_MES_00 + 2
    };

    const long MESSAGE_LINE_NUM = 2;

    const char *s_pPaneName[PANE_NUM] = {
        "inout",
        "t_title",
        "t_mes",
        "t_mes_00",
    };

    // The resource file of the cockpit that holds the layout, its layer and
    // display list memory area.
    const long RES_FILE = 2;
    const long LAYER = 6;
    const long GDL_MEM_AREA = 1;

    // m_nState.
    enum {
        STATE_IN = 1,
        STATE_IN_WAIT = 2,
        STATE_SHOW = 3,
        STATE_OUT = 4,
        STATE_OUT_WAIT = 5
    };
}

msw::CCockpitPauseRuleInfo::CCockpitPauseRuleInfo(long nId, long nKind, float fSortZ)
    : CCockpitPauseLayoutBase(nId, fSortZ)
    , m_nKind(nKind)
{
    m_nStep = 1;
    m_nState = 0;
}

msw::CCockpitPauseRuleInfo::~CCockpitPauseRuleInfo()
{
    Remove();
}

void msw::CCockpitPauseRuleInfo::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_nStep == 1) {
        Set();
        m_nState = STATE_IN;
        m_nStep = 2;
    }
    StateControl();
}

void msw::CCockpitPauseRuleInfo::StateControl()
{
    switch (m_nState) {
        case STATE_IN: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl == 0) {
                // (meaning guessed) tells the owner the page is closed.
                m_Unk08 = 2;
                break;
            }
            pPaneCtrl->PlayAnim(ANIM_IN, 1.0f, 0.0f, 1, 1);
            m_nState = STATE_IN_WAIT;
            break;
        }

        case STATE_IN_WAIT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_INOUT, ANIM_IN)) {
                GetLytObj()->PlayAnim(ANIM_WAIT_IN, 1.0f, 0.0f, 1, 1);
                m_nState = STATE_SHOW;
            }
            break;

        case STATE_OUT:
            GetLytObj()->PlayAnim(ANIM_OUT, 1.0f, 0.0f, 1, 1);
            m_nState = STATE_OUT_WAIT;
            break;

        case STATE_OUT_WAIT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_INOUT, ANIM_OUT)) {
                m_Unk08 = 2;
            }
            break;
    }
}

int msw::CCockpitPauseRuleInfo::Set()
{
    CCockpit *pCockpit = CCockpit::GetTask();

    if (pCockpit == 0) {
        return -1;
    }

    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -2;
    }

    long nKind = m_nKind;
    const char *pArchiveName = pCockpit->GetArchiveFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor == 0) {
        return -3;
    }

    bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, LAYER, GDL_MEM_AREA);

    if (pPaneCtrl == 0) {
        return -5;
    }

    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
    pPaneCtrl->BuildLayout(0, s_pLayoutName[nKind]);
    pPaneCtrl->BuildAnim(0, s_ppAnimName[nKind], ANIM_NUM);
    pPaneCtrl->SetSortZ(CCockpitPause::GetPageSortZ(m_fSortZ));
    SetPaneAccessor(pPaneCtrl, s_pPaneName, PANE_NUM);
    SetTextBoxFromMessage(pPaneCtrl);
    pPaneCtrl->PlayAnim(ANIM_START, 1.0f, 0.0f, 1, 1);

    // Empty texts until the pause menu fills them in.
    SetTitleText(L"");
    for (long i = 0; i < MESSAGE_LINE_NUM; i++) {
        SetMessageText(i, L"");
    }

    m_nState = STATE_IN;
    return 0;
}

void msw::CCockpitPauseRuleInfo::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

long msw::CCockpitPauseRuleInfo::SendEvent(long, void *)
{
    return 0;
}

void msw::CCockpitPauseRuleInfo::SetTitleText(const wchar_t *pText)
{
    if (pText != 0 && m_pPaneAccessor != 0) {
        m_pPaneAccessor->SetTextBoxString(PANE_TITLE, pText, wcslen(pText));
    }
}

void msw::CCockpitPauseRuleInfo::SetMessageText(long nLine, const wchar_t *pText)
{
    if (pText != 0 && m_pPaneAccessor != 0 && (unsigned long)nLine <= MESSAGE_LINE_NUM - 1) {
        m_pPaneAccessor->SetTextBoxString(PANE_MES_00 + nLine, pText, wcslen(pText));
    }
}

void msw::CCockpitPauseRuleInfo::Disp()
{
}
