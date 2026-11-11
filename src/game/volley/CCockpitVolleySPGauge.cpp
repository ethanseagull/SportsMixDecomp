// msw::CCockpitVolleySPGauge: the volley game's special-shot gauges (one
// per team) with their input prompts, the coin flash when a gauge is full
// and the receive telops. The gauge code follows the other sports' cockpit
// coders (game/hockey/CCockpitHockeyCoder.cpp), which hold their gauges
// themselves.
#include "game/msw/CCockpitVolleySPGauge.hpp"

#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/cockpit/CockpitUtil.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace {

    // Events the cockpit sends (see game/tour/CCockpitTournament.cpp).
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // The resource file of the cockpit that holds the layouts.
    const long RES_FILE = 3;

    enum {
        TEAM_NUM = 2
    };

    // By layout: 0 normally, 1 on a two-way split screen.
    const char *const sLayoutName[] = {
        "VO_sp.brlyt",
        "VO_cockpit_03_sp.brlyt"
    };

    const char *sAnimNames0[] = {
        "VO_sp_Set_00.brlan",
        "VO_sp_Set_01.brlan",
        "VO_sp_Set_02.brlan",
        "VO_sp_Set_03.brlan",
        "VO_sp_Set_coin_00.brlan",
        "VO_sp_Set_coin_01.brlan",
        "VO_sp_Set_coin_03.brlan",
        "VO_sp_SP_bar_00.brlan",
        "VO_sp_SP_bar_01.brlan",
        "VO_sp_SP_cont.brlan",
        "VO_sp_SP_re_00.brlan",
        "VO_sp_SP_re_01.brlan",
        "VO_sp_SP_re_02.brlan",
        "VO_sp_SP_re_03.brlan",
        "VO_sp_SP_k_00.brlan",
        "VO_sp_SP_k_01.brlan"
    };

    const char *sAnimNames1[] = {
        "VO_cockpit_03_sp_Set_00.brlan",
        "VO_cockpit_03_sp_Set_01.brlan",
        "VO_cockpit_03_sp_Set_02.brlan",
        "VO_cockpit_03_sp_Set_03.brlan",
        "VO_cockpit_03_sp_Set_coin_00.brlan",
        "VO_cockpit_03_sp_Set_coin_01.brlan",
        "VO_cockpit_03_sp_Set_coin_03.brlan",
        "VO_cockpit_03_sp_SP_bar_00.brlan",
        "VO_cockpit_03_sp_SP_bar_01.brlan",
        "VO_cockpit_03_sp_SP_cont.brlan",
        "VO_cockpit_03_sp_SP_re_00.brlan",
        "VO_cockpit_03_sp_SP_re_01.brlan",
        "VO_cockpit_03_sp_SP_re_02.brlan",
        "VO_cockpit_03_sp_SP_re_03.brlan",
        "VO_cockpit_03_sp_SP_k_00.brlan",
        "VO_cockpit_03_sp_SP_k_01.brlan"
    };

    const char **sAnimNames[] = {
        sAnimNames0,
        sAnimNames1
    };

    // Animations (indexes into sAnimNames0/1).
    enum {
        ANIM_IN_WAIT = 2,
        ANIM_OUT = 4,
        ANIM_OUT_WAIT = 5,
        ANIM_IN = 6,
        ANIM_SP_BAR = 7,
        ANIM_SP_BAR_MAX = 8,
        ANIM_SP_CONT = 9,
        ANIM_SP_RE_00 = 10,
        ANIM_SP_RE_01 = 11,
        ANIM_SP_RE_02 = 12,
        ANIM_SP_RE_03 = 13,
        ANIM_SP_K = 14,
        ANIM_SP_K_MAX = 15,
        ANIM_NUM = 16
    };

    const char *sPaneNames[] = {
        "SP_L",
        "SP_R",
        "SP_l",
        "SP_r",
        "SP_lf",
        "SP_rf",
        "SP_l_ka",
        "SP_r_ka",
        "coinL_SP",
        "coinR_SP",
        "f_l",
        "f_r",
        "SP_cont_L",
        "btn_L_AB",
        "btn_L_12",
        "SP_cont_R",
        "btn_R_AB",
        "btn_R_12",
        "SP_mov_L",
        "SP_mov_R",
        "SP_RECEIVE_L",
        "SP_RECEIVE_R",
        "SP_bar_l",
        "SP_bar_r",
        "SP_lf_k",
        "SP_rf_k"
    };

    // Panes (indexes into sPaneNames), by team.
    enum {
        PANE_MOVE_L = 18,
        PANE_MOVE_R = 19,
        PANE_NUM = 26
    };

    const unsigned char sSPPane[TEAM_NUM] = { 0, 1 };
    const unsigned char sSPFlashPane[TEAM_NUM] = { 10, 11 };
    const unsigned char sSPCoinPane[TEAM_NUM] = { 8, 9 };
    const unsigned char sSPGaugePane[TEAM_NUM] = { 2, 3 };
    const unsigned char sSPGaugeUnkPane[TEAM_NUM] = { 4, 5 };
    const unsigned char sSPGaugeSubPane[TEAM_NUM] = { 6, 7 };
    const unsigned char sSPContPane[TEAM_NUM] = { 12, 15 };
    const unsigned char sSPButtonPane[TEAM_NUM][2] = {
        { 13, 14 },
        { 16, 17 }
    };
    const unsigned char sSPTelopPane[TEAM_NUM] = { 20, 21 };
    // Hidden in the nType 1 variant.
    const unsigned char sReceivePane[] = { 22, 23, 24, 25 };

    // The effect played on the coin when a gauge is full.
    const long EFFECT_SP_MAX = 0x459;
}

msw::CCockpitVolleySPGauge::CCockpitVolleySPGauge(long nId, long nType)
    : CCockpitVolleyLayoutBase(nId)
    , m_nType(nType)
{
    m_nStep = 0;
}

msw::CCockpitVolleySPGauge::~CCockpitVolleySPGauge()
{
    Remove();
}

void msw::CCockpitVolleySPGauge::Main()
{
    if (m_nStep == 1) {
        m_nStep = 2;
        m_pPaneAccessor->PlayAnimation(PANE_MOVE_L, ANIM_IN_WAIT, 1.0f, 0.0f, 0, 1);
        m_pPaneAccessor->PlayAnimation(PANE_MOVE_R, ANIM_IN_WAIT, 1.0f, 0.0f, 0, 1);
    }
}

void msw::CCockpitVolleySPGauge::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

int msw::CCockpitVolleySPGauge::Set()
{
    // static-order swap (notes/decisions.md): the original has the "@0"
    // suffix on len, not teamNum.
    static const long len = sizeof(sReceivePane) / sizeof(sReceivePane[0]);
    static const long teamNum = TEAM_NUM;

    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    long nLayout = 0;

    if (CGame::g_GameWork.m_nViewSplit == 2) {
        nLayout = 1;
    }

    const char *pFileName = CCockpit::GetResourceFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        long nType = m_nType;
        long nLayer = 0;

        if (nType == 1) {
            nLayer = 6;
        }

        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, nLayer, 0);

        if (pPaneCtrl != 0) {
            long i;

            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, sLayoutName[nLayout]);
            pPaneCtrl->BuildAnim(0, sAnimNames[nLayout], ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, sPaneNames, PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(nType == 1 ? -970.0f : 0.0f);
            m_EffectSet.Initialize();

            for (i = 0; i < teamNum; i++) {
                m_SPGauge[i].SetParam(pPaneCtrl, sSPGaugePane[i], sSPGaugeUnkPane[i], sSPGaugeSubPane[i], ANIM_SP_BAR,
                                      ANIM_SP_BAR_MAX, ANIM_SP_K, ANIM_SP_K_MAX);
                m_SPGauge[i].SetGaugeAnim(m_pPaneAccessor, 0.0f);
            }
            for (i = 0; i < teamNum; i++) {
                m_SPInputDisp[i].SetParam(i, sSPContPane[i], ANIM_SP_CONT, sSPButtonPane[i][0], sSPButtonPane[i][1]);
                m_SPInputDisp[i].SetDispAnim(m_pPaneAccessor, 0);
            }
            SetSPGaugeDisp(1);
            for (i = 0; i < teamNum; i++) {
                SetSPGaugeTelop(i, -1.0f);
            }
            if (nType == 1) {
                for (i = 0; i < len; i++) {
                    m_pPaneAccessor->SetPaneDisp(sReceivePane[i], 0);
                }
            }
        }
    }
    m_nStep = 1;
    return 0;
}

void msw::CCockpitVolleySPGauge::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_EffectSet.Finalize();
    m_nStep = 0;
}

long msw::CCockpitVolleySPGauge::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return 0;
}

// The gauge has no score part.
int msw::CCockpitVolleySPGauge::ScoreOut()
{
    return 0;
}

int msw::CCockpitVolleySPGauge::ScoreIn()
{
    return 0;
}

int msw::CCockpitVolleySPGauge::ScoreOutWait()
{
    return 0;
}

int msw::CCockpitVolleySPGauge::ScoreInWait()
{
    return 0;
}

// The gauges move with the coin part.
int msw::CCockpitVolleySPGauge::CoinOut()
{
    m_pPaneAccessor->PlayAnimation(PANE_MOVE_L, ANIM_OUT, 1.0f, 0.0f, 0, 1);
    m_pPaneAccessor->PlayAnimation(PANE_MOVE_R, ANIM_OUT, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitVolleySPGauge::CoinIn()
{
    m_pPaneAccessor->PlayAnimation(PANE_MOVE_L, ANIM_IN, 1.0f, 0.0f, 0, 1);
    m_pPaneAccessor->PlayAnimation(PANE_MOVE_R, ANIM_IN, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitVolleySPGauge::CoinOutWait()
{
    m_pPaneAccessor->PlayAnimation(PANE_MOVE_L, ANIM_OUT_WAIT, 1.0f, 0.0f, 0, 1);
    m_pPaneAccessor->PlayAnimation(PANE_MOVE_R, ANIM_OUT_WAIT, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitVolleySPGauge::CoinInWait()
{
    m_pPaneAccessor->PlayAnimation(PANE_MOVE_L, ANIM_IN_WAIT, 1.0f, 0.0f, 0, 1);
    m_pPaneAccessor->PlayAnimation(PANE_MOVE_R, ANIM_IN_WAIT, 1.0f, 0.0f, 0, 1);
    return 0;
}

void msw::CCockpitVolleySPGauge::SetCoinMaxState(long nTeam, long nState)
{
    if (nTeam >= TEAM_NUM) {
        return;
    }
    if (m_nType == 1) {
        return;
    }

    bcs::G2D::CPaneObj *pCoinObj = m_pPaneAccessor->GetPaneObj(sSPCoinPane[nTeam]);

    switch (nState) {
        case 5:
            if (pCoinObj != 0) {
                pCoinObj->GetPane()->SetVisible(false);
            }
            m_SPGauge[nTeam].SetGaugeMaxAnim(m_pPaneAccessor, 0);
            m_EffectSet.RemoveEffect(nTeam, 0);
            break;
        case 6:
            if (pCoinObj != 0) {
                pCoinObj->GetPane()->SetVisible(true);
            }
            m_SPGauge[nTeam].SetGaugeMaxAnim(m_pPaneAccessor, 1);
            m_EffectSet.SetEffect(nTeam, 0, EFFECT_SP_MAX, m_pPaneAccessor->GetPaneObj(sSPFlashPane[nTeam]));
            break;
    }
}

void msw::CCockpitVolleySPGauge::SetSPGauge(long nTeam, float fRate)
{
    // The gauge moves in fifths.
    static const float f1_5 = 0.2f;

    if (nTeam >= TEAM_NUM) {
        return;
    }
    if (m_nType == 1) {
        return;
    }
    m_SPGauge[nTeam].SetGaugeAnim(m_pPaneAccessor, f1_5 * (int)(fRate / f1_5));
}

void msw::CCockpitVolleySPGauge::SetSPControllerState(long nTeam, int bDisp)
{
    if (nTeam < TEAM_NUM) {
        m_SPInputDisp[nTeam].SetDispAnim(m_pPaneAccessor, bDisp);
    }
}

void msw::CCockpitVolleySPGauge::SetSPGaugeDisp(int bDisp)
{
    m_pPaneAccessor->SetPaneDisp(sSPPane[0], bDisp);
    m_pPaneAccessor->SetPaneDisp(sSPPane[1], bDisp);
}

void msw::CCockpitVolleySPGauge::SetSPGaugeTelop(long nTeam, float fRate)
{
    cockpit::SetSPGaugeTelop(m_pPaneAccessor, sSPTelopPane[nTeam], ANIM_SP_RE_01, ANIM_SP_RE_02, ANIM_SP_RE_03,
                             ANIM_SP_RE_00, fRate);
}
