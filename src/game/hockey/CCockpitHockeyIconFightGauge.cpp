// msw::CCockpitHockeyIconFightGauge: the gauges shown over two fighting
// players, one per team. Each team's icon follows its player through a
// display task (CIconTask below).
#include "game/msw/CCockpitHockeyIconFightGauge.hpp"

#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/CDispTaskList.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace {

    // The layout, its animations and panes.
    struct LayoutRes {
        const char *pLayout;
        const char *pAnim[7];
        const char *pPane[6];
    } s_LayoutRes = {
        "f_gauge.brlyt",
        {
            "f_gauge_bar_00.brlan",
            "f_gauge_bar_01.brlan",
            "f_gauge_vis_00.brlan",
            "f_gauge_vis_01.brlan",
            "f_gauge_vis_02.brlan",
            "f_gauge_vis_03.brlan",
            "f_gauge_vis_00.brlan",
        },
        {
            "b_Null",
            "b_ani",
            "b_bar",
            "r_Null",
            "r_ani",
            "r_bar",
        },
    };

    const long ANIM_NUM = 7;
    const long PANE_NUM = 6;
    const long TEAM_NUM = 2;

    // Each team colour's panes (by index into s_LayoutRes.pPane): the
    // icon's null pane, its animated pane and the gauge bar.
    enum {
        ICON_NULL,
        ICON_ANI,
        ICON_BAR,
        ICON_PANE_NUM
    };

    const unsigned char s_IconPaneId[TEAM_NUM][ICON_PANE_NUM] = {
        { 0, 1, 2 },
        { 3, 4, 5 },
    };

    // Animations. The bar animations play on the bar pane, the others on
    // the animated pane.
    enum {
        ANIM_BAR_FILL = 0,  // stopped at the gauge frame
        ANIM_BAR_FULL = 1,
        ANIM_VIS_HIDE = 2,
        ANIM_VIS_OPEN = 3,
        ANIM_VIS_LOOP = 4,
        ANIM_VIS_CLOSE = 5,
        ANIM_VIS_START = 6
    };

    // The task kinds of the two icons are 10000 + team; SendEvent's event
    // number shows or hides the whole layout.
    const unsigned long ICON_KIND = 10000;
    const long EVENT_SET_VISIBLE = 10001;

    // (guess) The original destructors store the vtable pointers, which the
    // compiler only keeps when the body calls something; see
    // game/boss/CCockpitBossLayoutBase.cpp and notes/decisions.md.
    static inline void Nothing() {}
}

namespace msw {
    namespace {

        // One team's icon: keeps the icon's null pane (m_Unk18) at the screen
        // position of m_Pos. (class name guessed)
        class CIconTask : public menu::CDispTaskLink {
            public:
                CIconTask()
                {
                    m_bEnd = 0;
                    m_nAniPane = 0;
                }

                virtual ~CIconTask();
                virtual void StatusCheck(CMenuPaneAccessor *pAccessor);
                virtual void Disp(CMenuPaneAccessor *pAccessor, long nCamera);

                long m_nAniPane; // 0x30
        };

        CIconTask::~CIconTask()
        {
        }

        void CIconTask::StatusCheck(CMenuPaneAccessor *pAccessor)
        {
            // Shown from the first frame it has been placed.
            if (m_nFrame > 0) {
                pAccessor->SetPaneDisp(m_Unk18, 1);
            }
        }

        void CIconTask::Disp(CMenuPaneAccessor *pAccessor, long nCamera)
        {
            nw4r::lyt::Pane *pPane = pAccessor->GetPane(m_Unk18);

            if (pPane == 0) {
                m_bEnd = 1;
                return;
            }

            bcs::FVEC3 pos(m_Pos);
            bcs::FVEC3 screen;

            bcs::G2D::GetWorldScreenPosition(pos, nCamera, screen);
            // The original passes the FVEC3 straight on (FVEC3 is probably
            // derived from nw4r::math::VEC3 there).
            pPane->SetTranslate(reinterpret_cast<const nw4r::math::VEC3 &>(screen));
        }
    }
}

msw::CCockpitHockeyIconFightGauge::CCockpitHockeyIconFightGauge(long nId, float fSortZ, long nLayer)
    : CCockpitLayoutBase(nId, fSortZ)
{
    m_nLayer = nLayer;
}

msw::CCockpitHockeyIconFightGauge::~CCockpitHockeyIconFightGauge()
{
    Nothing();
}

void msw::CCockpitHockeyIconFightGauge::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);
}

void msw::CCockpitHockeyIconFightGauge::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, m_nLayer);
}

int msw::CCockpitHockeyIconFightGauge::Set()
{
    LayoutRes *pRes = &s_LayoutRes;

    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = CCockpit::GetResourceFileName(2);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        long nLayer;

        // default first: the original lays its block out right after the
        // compare chain, which this order reproduces (notes/toolchain.md).
        switch (m_nLayer) {
            default:
                nLayer = 1;
                break;
            case 1:
                nLayer = 2;
                break;
            case 2:
                nLayer = 3;
                break;
            case 3:
                nLayer = 4;
                break;
        }

        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, nLayer, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->BuildLayout(0, pRes->pLayout);
            pPaneCtrl->BuildAnim(0, pRes->pAnim, ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, pRes->pPane, PANE_NUM);
            pPaneCtrl->SetSortZ(m_fSortZ);

            for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
                CreateIcon(nTeam);
            }
            m_nStep = 1;
            m_nState = 1;
        }
    }
    return 0;
}

void msw::CCockpitHockeyIconFightGauge::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

long msw::CCockpitHockeyIconFightGauge::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_VISIBLE) {
        int bVisible = (int)pParam;
        bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

        if (pPaneCtrl != 0) {
            pPaneCtrl->SetVisible(bVisible);
        }
    }
    return 0;
}

long msw::CCockpitHockeyIconFightGauge::CreateIcon(long nTeam)
{
    if ((unsigned long)nTeam > 1) {
        return -1;
    }

    unsigned long nKind = ICON_KIND + nTeam;
    const unsigned char *pPaneId = s_IconPaneId[CGame::GetTeamColorIdx(nTeam)];
    long nNullPane = pPaneId[ICON_NULL];
    long nAniPane = pPaneId[ICON_ANI];
    CIconTask *pTask = new CIconTask;

    pTask->m_Unk18 = nNullPane;
    pTask->m_nAniPane = nAniPane;
    pTask->m_nKind = nKind;
    m_DispTaskList.PushBack(pTask);

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nNullPane);

    if (pObj != 0) {
        // static-order swap (notes/decisions.md): the game numbers
        // `recursive` before `reset` but stores it after.
        static const int reset = 1;
        static const int recursive = 1;

        pObj->GetPane()->SetVisible(true);
        pObj->StopAllAnimation(1);
        nw4r::lyt::Pane *pRoot = m_pPaneAccessor->GetPane("RootPane", 0, 0);

        bcs::G2D::SetPaneParent(pObj->GetPane(), pRoot, 1, 0);
        pObj->PlayAnimation(ANIM_VIS_HIDE, 1.0f, 0.0f, recursive, reset, 1);
    }
    return 0;
}

long msw::CCockpitHockeyIconFightGauge::SetIcon(long nTeam, const bcs::FVEC3 &rPos)
{
    if ((unsigned long)nTeam > 1) {
        return -1;
    }

    const unsigned char *pPaneId = s_IconPaneId[CGame::GetTeamColorIdx(nTeam)];
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(pPaneId[ICON_ANI]);

    if (pObj != 0 && pObj->IsAnimPlay(ANIM_VIS_HIDE)) {
        // static-order swap, as in CreateIcon.
        static const int reset = 1;
        static const int recursive = 0;

        pObj->PlayAnimation(ANIM_VIS_START, 1.0f, 0.0f, recursive, reset, 1);
        pObj->PushNextAnimation(ANIM_VIS_OPEN, 1.0f, 0.0f, recursive, reset);
        pObj->PushNextAnimation(ANIM_VIS_LOOP, 1.0f, 0.0f, recursive, reset);
    }

    menu::CDispTaskLink *pTask = m_DispTaskList.FindNextKindObj(ICON_KIND + nTeam, 0);

    if (pTask != 0) {
        pTask->SetPos(rPos);
        // Shown again from the next frame on (CIconTask::StatusCheck).
        pTask->m_nFrame = 0;
    }
    return 0;
}

long msw::CCockpitHockeyIconFightGauge::ClearIcon(long nTeam, int bClose)
{
    if ((unsigned long)nTeam > 1) {
        return -1;
    }

    const unsigned char *pPaneId = s_IconPaneId[CGame::GetTeamColorIdx(nTeam)];
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(pPaneId[ICON_ANI]);

    if (pObj != 0) {
        // static-order swap, as in CreateIcon.
        static const int reset = 1;
        static const int recursive = 0;

        if (bClose != 0 && (pObj->IsAnimPlay(ANIM_VIS_OPEN) || pObj->IsAnimPlay(ANIM_VIS_LOOP))) {
            pObj->PlayAnimation(ANIM_VIS_CLOSE, 1.0f, 0.0f, recursive, reset, 1);
            pObj->PushNextAnimation(ANIM_VIS_HIDE, 1.0f, 0.0f, recursive, reset);
            return 0;
        }
        pObj->PlayAnimation(ANIM_VIS_HIDE, 1.0f, 0.0f, recursive, reset, 1);
    }
    return 0;
}

long msw::CCockpitHockeyIconFightGauge::SetGaugeBar(long nTeam, float fRatio)
{
    if ((unsigned long)nTeam > 1) {
        return -1;
    }

    // static-order swap: the game has `recursive`, `reset@0`,
    // `gaugeFrameNum@1`, stored in the reverse order.
    static const float gaugeFrameNum = 10.0f;
    static const int reset = 1;
    static const int recursive = 0;
    const unsigned char *pPaneId = s_IconPaneId[nTeam];

    if (fRatio >= 1.0f) {
        m_pPaneAccessor->PlayAnimation(pPaneId[ICON_BAR], ANIM_BAR_FULL, 1.0f, 0.0f, recursive, reset);
    } else {
        float fFrame = fRatio < 0.0f ? 0.0f : gaugeFrameNum * fRatio;

        m_pPaneAccessor->PlayAnimation(pPaneId[ICON_BAR], ANIM_BAR_FILL, 0.0f, fFrame, recursive, reset);
    }
    return 0;
}
