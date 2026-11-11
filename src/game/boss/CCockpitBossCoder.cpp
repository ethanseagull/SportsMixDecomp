// msw::CCockpitBossCoder: the boss game's main cockpit (the players' life
// gauges with face icons and labels, the boss's life gauge, the coin
// gauge, the "out" telops), and msw::CCockpitBossBallPower, the ball power
// counter shown with it.
//
// The other sports' coders (CCockpitBasketCoder and so on) are the same
// code with their own tables.
#include "game/msw/CCockpitBossCoder.hpp"
#include "game/msw/CCockpitBossBallPower.hpp"

#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/cockpit/CockpitUtil.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"

namespace {

    // Events the cockpit sends (see game/tour/CCockpitTournament.cpp).
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // The resource file of the cockpit that holds the layouts.
    const long RES_FILE = 2;

    const char *sLayoutNames[] = {
        "BOSS_cockpit.brlyt",
        0
    };

    const char *sAnimNames[] = {
        "BOSS_cockpit_B_ber_00.brlan",
        "BOSS_cockpit_B_ber_01.brlan",
        "BOSS_cockpit_B_ber_02.brlan",
        "BOSS_cockpit_change_00.brlan",
        "BOSS_cockpit_change_01.brlan",
        "BOSS_cockpit_change_02.brlan",
        "BOSS_cockpit_change_03.brlan",
        "BOSS_cockpit_Cha_life.brlan",
        "BOSS_cockpit_Gauge_00.brlan",
        "BOSS_cockpit_Gauge_01.brlan",
        "BOSS_cockpit_Gauge_02.brlan",
        "BOSS_cockpit_Gauge_03.brlan",
        "BOSS_cockpit_Hit_00.brlan",
        "BOSS_cockpit_Hit_01.brlan",
        "BOSS_cockpit_Hit_02.brlan",
        "BOSS_cockpit_Hit_03.brlan",
        "BOSS_cockpit_Hit_04.brlan",
        "BOSS_cockpit_Out_00.brlan",
        "BOSS_cockpit_Out_01.brlan",
        "BOSS_cockpit_Out_02.brlan",
        "BOSS_cockpit_SP_bar_00.brlan",
        "BOSS_cockpit_SP_bar_01.brlan",
        "BOSS_cockpit_SP_bar_02.brlan",
        "BOSS_cockpit_SP_cont.brlan",
        "BOSS_cockpit_Vis_00.brlan",
        "BOSS_cockpit_Vis_01.brlan",
        "BOSS_cockpit_SC_00.brlan",
        "BOSS_cockpit_SC_01.brlan",
        "BOSS_cockpit_SC_02.brlan",
        "BOSS_cockpit_SC_03.brlan",
        "BOSS_cockpit_SP_k_00.brlan",
        "BOSS_cockpit_SP_k_01.brlan"
    };

    // Animations (indexes into sAnimNames).
    enum {
        ANIM_BOSS_GAUGE_FULL = 1,
        ANIM_BOSS_GAUGE = 2,
        ANIM_LIFE = 7,
        ANIM_GAUGE_IN = 8,
        ANIM_GAUGE_IN_WAIT = 9,
        ANIM_GAUGE_OUT = 10,
        ANIM_GAUGE_OUT_WAIT = 11,
        ANIM_HIT_0 = 12,
        ANIM_HIT_1 = 14,
        ANIM_HIT_2 = 16,
        ANIM_HIT_3 = 15,
        ANIM_OUT_HIDE = 17,
        ANIM_OUT_SHOW = 18,
        ANIM_OUT_WAIT = 19,
        ANIM_SP_BAR = 20,
        ANIM_SP_BAR_MAX = 21,
        ANIM_SP_CONT = 23,
        ANIM_LABEL_COM = 4,
        ANIM_LABEL_PLAYER_OFF = 5,
        ANIM_LABEL_PLAYER = 6,
        ANIM_COIN_UP = 27,
        ANIM_COIN_UP_WAIT = 28,
        ANIM_COIN_DOWN = 29,
        ANIM_COIN_DOWN_WAIT = 26,
        ANIM_SP_K = 30,
        ANIM_SP_K_MAX = 31
    };

    const char *sPaneNames[] = {
        "g_Null",
        "life_A",
        "life_B",
        "gA_a",
        "gA_b",
        "gA_c",
        "gB_a",
        "gB_b",
        "gA_a_01_life",
        "gA_b_01_life",
        "gA_c_01_life",
        "gB_a_01_life",
        "gB_b_01_life",
        "gA_a_01_life_2",
        "gA_b_01_life_2",
        "gA_c_01_life_2",
        "gB_a_01_life_2",
        "gB_b_01_life_2",
        "B_hp_Null",
        "B_hp",
        "boss_01",
        "boss_02",
        "gB_a_01_fe",
        "gB_b_01_fe",
        "gA_a_01_fe",
        "gA_b_01_fe",
        "gA_c_01_fe",
        "Ph_00",
        "Ph_01",
        "Ph_02",
        "Ph_03",
        "Ph_04",
        "1P",
        "2P",
        "3P",
        "4P",
        "com_01",
        "com_02",
        "com_03",
        "SP_Null",
        "SP_01",
        "SP_01k",
        "f_l",
        "coin_SP",
        "coin_SP",
        "pow_Null",
        "power_Null",
        "p_up_g",
        "p_coin",
        "out_te_Null",
        "out_te_Null_02",
        "out_te_Null_03",
        "out_te_Null_04",
        "out_te_Null_05",
        "SP_cont",
        "btn_AB",
        "btn_12",
        "gA_Ca_a",
        "gA_Ca_b",
        "gA_Ca_c",
        "gA_Cb_a",
        "gA_Cb_b",
        "cs_ico",
        "sc_p02",
        "IcFace00",
        "IcFace01",
        "IcFace02",
        "IcPNo00",
        "IcPNo01",
        "IcPNo02",
        "L_StPic1",
        "L_StPic2",
        "L_StPic3"
    };

    // Panes (indexes into sPaneNames) used by name in the code.
    enum {
        PANE_g_Null = 0,
        PANE_life_A = 1,
        PANE_life_B = 2,
        PANE_B_hp_Null = 18,
        PANE_B_hp = 19,
        PANE_boss_01 = 20,
        PANE_boss_02 = 21,
        PANE_SP_Null = 39,
        PANE_f_l = 42,
        PANE_coin_SP = 43,
        PANE_coin_SP_2 = 44,
        PANE_cs_ico = 62
    };

    // The panes of one player's gauge (indexes into sPaneNames).
    struct LifePane {
        short nGauge;       // the gauge group, animated as a whole
        short nFaceParent;  // where the face picture goes
        short nLabelParent; // where the "1P"/"COM" label goes
        short nFace;        // name of the face picture
        short nLabel;       // name of the label copy
        short nLife;        // the life bar
        short nLife2;       // the second life bar
        short nOutTelop;    // the "out" telop
        short nStarParent;  // where the star goes
        short nStar;        // name of the star copy
    };

    // By layout (SetLifeGaugeLayout) and player.
    const LifePane sLifePane[2][3] = {
        {
            { 6, 22, 27, 64, 67, 11, 16, 52, 60, 70 },
            { 7, 23, 28, 65, 68, 12, 17, 53, 61, 71 },
            { 0 }
        },
        {
            { 3, 24, 29, 64, 67, 8, 13, 49, 57, 70 },
            { 4, 25, 30, 65, 68, 9, 14, 50, 58, 71 },
            { 5, 26, 31, 66, 69, 10, 15, 51, 59, 72 }
        }
    };

    // The labels: a human player's ("1P" to "4P"), then the computer's.
    const unsigned char sPlayerLabelPane[4] = { 32, 33, 34, 35 };
    const unsigned char sComLabelPane[3] = { 36, 37, 38 };

    // The coin gauge of each team: its panes (the second one slides with
    // the gauge parts).
    struct CoinPane {
        unsigned char nNull;
        unsigned char nPower;
        unsigned char nUp;
    };

    const CoinPane sCoinPane[1] = {
        { 45, 46, 47 }
    };

    // The special shot prompt of each team.
    const unsigned char sSPContPane[1] = { 54 };
    const unsigned char sSPFlashPane[1] = { 42 };
    const unsigned char sSPButtonPane[1][2] = {
        { 55, 56 } // with a nunchaku, with a remote alone
    };
    // The coin counter of each team.
    const unsigned char sCoinCountPane[1] = { 63 };
}

msw::CCockpitBossCoder::CCockpitBossCoder(long nId)
    : CCockpitBossLayoutBase(nId)
{
    m_nStep = 0;
    m_nPlayerNum = 0;
    m_nLayout = 1;
}

msw::CCockpitBossCoder::~CCockpitBossCoder()
{
    Remove();
}

void msw::CCockpitBossCoder::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_nStep == 1) {
        SetupGame();
        SetFaceIcon(GetLytObj());
        m_nStep = 2;
    }
}

void msw::CCockpitBossCoder::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

int msw::CCockpitBossCoder::Set()
{
    static const long playerLen = 4;
    static const long comLen = 3;

    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = CCockpit::GetResourceFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

        if (pPaneCtrl != 0) {
            const char *pFontName = pLytCtrl->GetFontResourceName(1);

            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pFontName);
            pPaneCtrl->BuildLayout(0, sLayoutNames[0]);
            pPaneCtrl->BuildAnim(0, sAnimNames, sizeof(sAnimNames) / sizeof(sAnimNames[0]));
            SetPaneAccessor(pPaneCtrl, sPaneNames, sizeof(sPaneNames) / sizeof(sPaneNames[0]));
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(0.0f);
            pPaneCtrl->SetVisible(0);

            m_SPGauge[0].SetParam(pPaneCtrl, 40, 40, 41, ANIM_SP_BAR, ANIM_SP_BAR_MAX, ANIM_SP_K, ANIM_SP_K_MAX);
            m_SPGauge[0].SetGaugeAnim(m_pPaneAccessor, 0.0f);
            m_SPInputDisp[0].SetParam(0, sSPContPane[0], ANIM_SP_CONT, sSPButtonPane[0][0], sSPButtonPane[0][1]);
            m_SPInputDisp[0].SetDispAnim(m_pPaneAccessor, 0);

            long i;

            for (i = 0; i < playerLen; i++) {
                m_pPaneAccessor->SetPaneDisp(sPlayerLabelPane[i], 0);
            }
            for (i = 0; i < comLen; i++) {
                m_pPaneAccessor->SetPaneDisp(sComLabelPane[i], 0);
            }
            m_EffectSet.Initialize();
        }
    }
    return 0;
}

void msw::CCockpitBossCoder::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_EffectSet.Finalize();
    m_nStep = 0;
}

long msw::CCockpitBossCoder::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
        m_EffectSet.SetDispFlag(bVisible);
    }
    return 0;
}

int msw::CCockpitBossCoder::SetupGame()
{
    static const long animId = ANIM_HIT_0;
    static const long teamNo = 0;

    LifeGaugeIn();
    BossGaugeIn();
    CoinIn();

    long nPlayerNum = m_nPlayerNum;
    long i;

    for (i = 0; i < nPlayerNum; i++) {
        m_pPaneAccessor->PlayAnimation(sLifePane[m_nLayout][i].nGauge, animId, 1.0f, 0.0f, 1, 1);
    }
    for (i = 0; i < nPlayerNum; i++) {
        SetOutTelop(i, 0);
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sCoinPane[teamNo].nPower);

    if (pObj != 0) {
        pObj->PlayAnimation(ANIM_COIN_DOWN, 1.0f, 0.0f, 1, 1, 1);
        pObj->PushNextAnimation(ANIM_COIN_DOWN_WAIT, 1.0f, 0.0f, 1, 1);
    }
    return 0;
}

// The life gauges and the coin gauge's power pane slide together.
int msw::CCockpitBossCoder::LifeGaugeOut()
{
    m_pPaneAccessor->PlayAnimation(PANE_g_Null, ANIM_GAUGE_OUT, 1.0f, 0.0f, 0, 1);
    m_pPaneAccessor->PlayAnimation(sCoinPane[0].nPower, ANIM_GAUGE_OUT, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitBossCoder::LifeGaugeIn()
{
    m_pPaneAccessor->PlayAnimation(PANE_g_Null, ANIM_GAUGE_IN, 1.0f, 0.0f, 0, 1);
    m_pPaneAccessor->PlayAnimation(sCoinPane[0].nPower, ANIM_GAUGE_IN, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitBossCoder::LifeGaugeOutWait()
{
    m_pPaneAccessor->PlayAnimation(PANE_g_Null, ANIM_GAUGE_OUT_WAIT, 1.0f, 0.0f, 0, 1);
    m_pPaneAccessor->PlayAnimation(sCoinPane[0].nPower, ANIM_GAUGE_OUT_WAIT, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitBossCoder::LifeGaugeInWait()
{
    m_pPaneAccessor->PlayAnimation(PANE_g_Null, ANIM_GAUGE_IN_WAIT, 1.0f, 0.0f, 0, 1);
    m_pPaneAccessor->PlayAnimation(sCoinPane[0].nPower, ANIM_GAUGE_IN_WAIT, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitBossCoder::BossGaugeOut()
{
    m_pPaneAccessor->PlayAnimation(PANE_B_hp_Null, ANIM_GAUGE_OUT, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitBossCoder::BossGaugeIn()
{
    m_pPaneAccessor->PlayAnimation(PANE_B_hp_Null, ANIM_GAUGE_IN, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitBossCoder::BossGaugeOutWait()
{
    m_pPaneAccessor->PlayAnimation(PANE_B_hp_Null, ANIM_GAUGE_OUT_WAIT, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitBossCoder::BossGaugeInWait()
{
    m_pPaneAccessor->PlayAnimation(PANE_B_hp_Null, ANIM_GAUGE_IN_WAIT, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitBossCoder::CoinOut()
{
    m_pPaneAccessor->PlayAnimation(PANE_SP_Null, ANIM_GAUGE_OUT, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitBossCoder::CoinIn()
{
    m_pPaneAccessor->PlayAnimation(PANE_SP_Null, ANIM_GAUGE_IN, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitBossCoder::CoinOutWait()
{
    m_pPaneAccessor->PlayAnimation(PANE_SP_Null, ANIM_GAUGE_OUT_WAIT, 1.0f, 0.0f, 0, 1);
    return 0;
}

int msw::CCockpitBossCoder::CoinInWait()
{
    m_pPaneAccessor->PlayAnimation(PANE_SP_Null, ANIM_GAUGE_IN_WAIT, 1.0f, 0.0f, 0, 1);
    return 0;
}

void msw::CCockpitBossCoder::SetLifeValue(long nPlayer, float fLife, float fLife2)
{
    static const long animId = ANIM_LIFE;
    static const float speed = 0.0f;
    static const int recursive = 1;
    static const int reset = 1;

    if (m_pPaneAccessor == 0) {
        return;
    }

    if (fLife > 1.0f) {
        fLife = 1.0f;
    } else if (fLife < 0.0f) {
        fLife = 0.0f;
    }
    if (fLife2 > 1.0f) {
        fLife2 = 1.0f;
    } else if (fLife2 < 0.0f) {
        fLife2 = 0.0f;
    }

    const LifePane *pPane = &sLifePane[m_nLayout][nPlayer];
    bcs::G2D::CPaneObj *pLife = m_pPaneAccessor->GetPaneObj(pPane->nLife);
    bcs::G2D::CPaneObj *pLife2 = m_pPaneAccessor->GetPaneObj(pPane->nLife2);

    if (pLife != 0) {
        // A full gauge is frame 0.
        float fFrameMax = pLife->GetAnimMaxFrame(animId);
        float fFrame = fFrameMax * (1.0f - fLife);
        float fFrame2 = fFrameMax * (1.0f - fLife2);

        pLife->PlayAnimation(animId, speed, fFrame, recursive, reset, 1);
        if (pLife2 != 0) {
            pLife2->PlayAnimation(animId, speed, fFrame2, recursive, reset, 1);
        }
    }
}

void msw::CCockpitBossCoder::SetLifeGaugeAnim(long nPlayer, long nState)
{
    long nAnim;

    switch (nState) {
        case 1:
            nAnim = ANIM_HIT_1;
            break;
        case 2:
            nAnim = ANIM_HIT_2;
            break;
        case 3:
            nAnim = ANIM_HIT_3;
            break;
        default:
            nAnim = ANIM_HIT_0;
            break;
    }
    m_pPaneAccessor->PlayAnimation(sLifePane[m_nLayout][nPlayer].nGauge, nAnim, 1.0f, 0.0f, 1, 1);
}

void msw::CCockpitBossCoder::SetLifeGaugeLayout(long nLayout)
{
    m_nLayout = nLayout;

    switch (nLayout) {
        case 0:
            m_nPlayerNum = 2;
            m_pPaneAccessor->SetPaneDisp(PANE_life_B, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_life_A, 0);
            break;
        case 1:
            m_nPlayerNum = 3;
            m_pPaneAccessor->SetPaneDisp(PANE_life_B, 0);
            m_pPaneAccessor->SetPaneDisp(PANE_life_A, 1);
            break;
    }

    if (m_nStep == 0) {
        m_nStep = 1;
    }
}

void msw::CCockpitBossCoder::SetSPGauge(float fRate)
{
    if (fRate > 1.0f) {
        fRate = 1.0f;
    } else if (fRate < 0.0f) {
        fRate = 0.0f;
    }
    m_SPGauge[0].SetGaugeAnim(m_pPaneAccessor, fRate);
}

void msw::CCockpitBossCoder::SetSPGaugeDisp(int bDisp)
{
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_SP_Null);

    if (pObj != 0) {
        pObj->StopAllAnimation(1);
        pObj->GetPane()->SetVisible(bDisp == 1);
    }
}

void msw::CCockpitBossCoder::SetSPControllerState(long nTeam, int bDisp)
{
    if (nTeam < 1) {
        m_SPInputDisp[nTeam].SetDispAnim(m_pPaneAccessor, bDisp);
    }
}

void msw::CCockpitBossCoder::SetCoinValue(long nTeam, long nDiff)
{
    if (nDiff > 0) {
        static const int recursive = 1;
        static const int reset = 1;

        bcs::G2D::CPaneObj *pCount = m_pPaneAccessor->GetPaneObj(sCoinCountPane[nTeam]);

        if (pCount != 0 && !pCount->IsAnimPlay(ANIM_COIN_UP) && !pCount->IsAnimPlay(ANIM_COIN_UP_WAIT)) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sCoinPane[nTeam].nPower);

            if (pObj != 0) {
                pObj->PlayAnimation(ANIM_COIN_UP, 1.0f, 0.0f, recursive, reset, 1);
                pObj->PushNextAnimation(ANIM_COIN_UP_WAIT, 1.0f, 0.0f, recursive, reset);
            }
        }
    } else {
        static const int recursive = 1;
        static const int reset = 1;

        bcs::G2D::CPaneObj *pCount = m_pPaneAccessor->GetPaneObj(sCoinCountPane[nTeam]);

        if (pCount != 0 && !pCount->IsAnimPlay(ANIM_COIN_DOWN) && !pCount->IsAnimPlay(ANIM_COIN_DOWN_WAIT)) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sCoinPane[nTeam].nPower);

            if (pObj != 0) {
                pObj->PlayAnimation(ANIM_COIN_DOWN, 1.0f, 0.0f, recursive, reset, 1);
                pObj->PushNextAnimation(ANIM_COIN_DOWN_WAIT, 1.0f, 0.0f, recursive, reset);
            }
        }
    }
}

void msw::CCockpitBossCoder::SetCoinMaxState(long nState)
{
    static const long teamNo = 0;

    // The second coin pane is looked up and not used.
    m_pPaneAccessor->GetPaneObj(PANE_coin_SP_2);

    bcs::G2D::CPaneObj *pCoin = m_pPaneAccessor->GetPaneObj(PANE_coin_SP);

    switch (nState) {
        case 5:
            if (pCoin != 0) {
                pCoin->GetPane()->SetVisible(false);
            }
            m_SPGauge[teamNo].SetGaugeMaxAnim(m_pPaneAccessor, 0);
            m_EffectSet.RemoveEffect(0, 0);
            break;
        case 6:
            if (pCoin != 0) {
                pCoin->GetPane()->SetVisible(true);
            }
            m_SPGauge[teamNo].SetGaugeMaxAnim(m_pPaneAccessor, 1);
            m_EffectSet.SetEffect(0, 0, 0x459, m_pPaneAccessor->GetPaneObj(PANE_f_l));
            break;
    }
}

void msw::CCockpitBossCoder::SetCoinState(long, long)
{
}

void msw::CCockpitBossCoder::SetBossLifePercent(float fRate)
{
    static const int recursive = 0;
    static const int reset = 1;
    static const float speed = 0.0f;
    static const float frameMax = 30.0f;

    if (m_pPaneAccessor == 0) {
        return;
    }

    if (fRate >= 1.0f) {
        m_pPaneAccessor->PlayAnimation(PANE_B_hp, ANIM_BOSS_GAUGE_FULL, 1.0f, 0.0f, recursive, reset);
    } else {
        if (fRate < 0.0f) {
            fRate = 0.0f;
        }
        m_pPaneAccessor->PlayAnimation(PANE_B_hp, ANIM_BOSS_GAUGE, speed, frameMax - frameMax * fRate, recursive,
                                       reset);
    }
}

void msw::CCockpitBossCoder::SetBossIcon(long nIcon)
{
    int bDisp1;
    int bDisp2;

    if (nIcon > 0) {
        bDisp1 = 0;
        bDisp2 = 1;
    } else if (nIcon == 0) {
        bDisp1 = 1;
        bDisp2 = 0;
    } else {
        bDisp1 = 0;
        bDisp2 = 0;
    }
    m_pPaneAccessor->SetPaneDisp(PANE_boss_01, bDisp1);
    m_pPaneAccessor->SetPaneDisp(PANE_boss_02, bDisp2);
}

void msw::CCockpitBossCoder::SetFaceIcon()
{
    SetFaceIcon(GetLytObj());
}

void msw::CCockpitBossCoder::SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    if (pPaneCtrl == 0) {
        return;
    }

    // The characters of the players' team.
    long chrNo[] = { 0, 2, 4 };
    static const long TeamNum = 1;

    for (long nTeam = 0; nTeam < TeamNum; nTeam++) {
        for (long i = 0; i < m_nPlayerNum; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(chrNo[i]);

            if (pChr != 0 && !pChr->IsDisable()) {
                SetFaceIcon(pPaneCtrl, nTeam, i, pChr);
            }
        }
    }
}

void msw::CCockpitBossCoder::SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl, long nTeam, long nPlayer,
                                         const CChrCore *pChr)
{
    static const long texMapId = 0;
    static const int recursive = 0;
    static const int reset = 1;
    static const int recursive2 = 0;
    static const int reset2 = 1;

    long nPlayerNum = m_nPlayerNum;
    const LifePane *pPane = &sLifePane[m_nLayout][nPlayer];

    if (pPane == 0) {
        return;
    }

    // The face picture.
    nw4r::lyt::Pane *pFaceParent = m_pPaneAccessor->GetPane(pPane->nFaceParent);

    if (pFaceParent == 0) {
        return;
    }

    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    long nFace = pPane->nFace;
    nw4r::lyt::Pane *pFace = m_pPaneAccessor->GetPane(nFace);

    if (pFace == 0) {
        pFace = menu::CreateFaceIconPicturePane(pChr->GetPlayerNo(), pPaneCtrl, pFaceParent, sPaneNames[nFace]);
        bcs::G2D::SetPaneSizeFromParent(pFace);
    } else {
        const _GXTexObj *pTexObj = pChr->GetCockpitIconTexObj();

        if (pTexObj != 0) {
            bcs::G2D::ReplacePanePicture(pFace, pTexObj, texMapId);
        }
    }

    // The label: the pad of a human player, else "COM".
    nw4r::lyt::Pane *pLabelParent = m_pPaneAccessor->GetPane(pPane->nLabelParent);

    if (pLabelParent == 0) {
        return;
    }

    long nLabelSrc;

    if (pChr->IsPadCtrl()) {
        nLabelSrc = sPlayerLabelPane[pChr->GetCtrlPlayerNo()];
    } else {
        nLabelSrc = sComLabelPane[0];
    }

    nw4r::lyt::Pane *pLabelSrc = m_pPaneAccessor->GetPane(nLabelSrc);

    if (pLabelSrc == 0) {
        return;
    }

    const CChrCore *pLeader = cockpit::GetTeamLeaderChrCore(nTeam);
    long nLabel = pPane->nLabel;
    nw4r::lyt::Pane *pLabel = m_pPaneAccessor->GetPane(nLabel);

    if (pLabel == 0) {
        pLabel = bcs::G2D::DuplicatePane(pPaneCtrl, pLabelSrc, sPaneNames[nLabel], sLayoutNames[0], 0);
        bcs::G2D::SetPaneParent(pLabel, pLabelParent, 1, 0);
        pLabel->SetVisible(true);
        pLabel->SetTranslate(zero);

        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(pLabelParent);

        if (pObj != 0 && nPlayerNum > 1) {
            pObj->PlayAnimation((pChr->IsPadCtrl() || pChr == pLeader) ? ANIM_LABEL_PLAYER : ANIM_LABEL_PLAYER_OFF,
                                1.0f, 0.0f, recursive, reset, 1);
        }
    } else {
        // The label changes only while a human player leads the team.
        if (pLeader != 0 && !pLeader->IsPadCtrl()) {
            return;
        }

        if (nLabelSrc != sComLabelPane[0]) {
            bcs::G2D::ReplacePanePicture(pLabel, pLabelSrc);
            pLabel->SetSize(pLabelSrc->GetSize());
            for (long i = 0; i < 4; i++) {
                pLabel->SetVtxColor(i, pLabelSrc->GetVtxColor(i));
            }
        }

        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(pLabelParent);

        if (pObj != 0 && nPlayerNum > 1) {
            long nAnim = -1;

            if (nLabelSrc == sComLabelPane[0]) {
                if (pObj->IsAnimPlay(ANIM_LABEL_PLAYER)) {
                    nAnim = ANIM_LABEL_COM;
                }
            } else if (!pObj->IsAnimPlay(ANIM_LABEL_PLAYER)) {
                nAnim = ANIM_LABEL_PLAYER;
            }
            if (nAnim >= 0) {
                pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive2, reset2, 1);
            }
        }
    }

    // The star of a star player: a copy of cs_ico.
    nw4r::lyt::Pane *pStarSrc = m_pPaneAccessor->GetPane(PANE_cs_ico);
    nw4r::lyt::Pane *pStarParent = m_pPaneAccessor->GetPane(pPane->nStarParent);

    if (pStarSrc == 0 || pStarParent == 0) {
        return;
    }

    long nStar = pPane->nStar;
    nw4r::lyt::Pane *pStar = m_pPaneAccessor->GetPane(nStar);

    if (pStar == 0) {
        pStar = bcs::G2D::DuplicatePane(pPaneCtrl, pStarSrc, sPaneNames[nStar], sLayoutNames[0], 0);
        bcs::G2D::SetPaneParent(pStar, pStarParent, 1, 0);
        bcs::G2D::SetPaneSizeFromParent(pStar);
        pStar->SetTranslate(zero);
    }

    if (pStar != 0) {
        pStar->SetVisible(pChr->IsStarPlayer() != 0);
    }
}

int msw::CCockpitBossCoder::SetTelopPowerUp(long, long)
{
    return 0;
}

void msw::CCockpitBossCoder::SetOutTelop(long nPlayer, int bOut)
{
    static const int recursive = 0;
    static const int reset = 1;
    static const long animId0 = ANIM_OUT_SHOW;
    static const long animId1 = ANIM_OUT_WAIT;
    static const long animId = ANIM_OUT_HIDE;

    if (m_pPaneAccessor == 0 || nPlayer >= m_nPlayerNum) {
        return;
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sLifePane[m_nLayout][nPlayer].nOutTelop);

    if (pObj == 0) {
        return;
    }

    nw4r::lyt::Pane *pPane = pObj->GetPane()->GetParent();

    if (bOut) {
        pObj->PlayAnimation(animId0, 1.0f, 0.0f, recursive, reset, 1);
        pObj->PushNextAnimation(animId1, 1.0f, 0.0f, recursive, reset);
        if (pPane != 0) {
            pPane->SetVisible(true);
        }
    } else {
        pObj->PlayAnimation(animId, 1.0f, 0.0f, recursive, reset, 1);
        if (pPane != 0) {
            pPane->SetVisible(false);
        }
    }
}

// msw::CCockpitBossBallPower

namespace {

    const char *sPowerLayoutNames[] = {
        "BOSS_cockpit_power_p.brlyt"
    };

    const char *sPowerAnimNames[] = {
        "BOSS_cockpit_power_p_Gauge_00.brlan",
        "BOSS_cockpit_power_p_Gauge_01.brlan",
        "BOSS_cockpit_power_p_Gauge_02.brlan",
        "BOSS_cockpit_power_p_Gauge_03.brlan",
        "BOSS_cockpit_power_p_B_p_00.brlan",
        "BOSS_cockpit_power_p_B_p_01.brlan",
        "BOSS_cockpit_power_p_B_p_02.brlan",
        0
    };

    const char *sPowerPaneNames[] = {
        "pow_Null",
        "power_Null",
        "po_te_g",
        "po_te_s"
    };

    // Animations (indexes into sPowerAnimNames).
    enum {
        ANIM_POWER_IN = 0,
        ANIM_POWER_IN_WAIT = 1,
        ANIM_POWER_OUT = 2,
        ANIM_POWER_OUT_WAIT = 3,
        ANIM_POWER_NORMAL = 4,
        ANIM_POWER_UP = 5,
        ANIM_POWER_UP2 = 6
    };

    // The resource layer of the layout object.
    const long LAYER_POWER = 6;

    // The panes of each team's counter (indexes into sPowerPaneNames).
    struct PowerPane {
        unsigned char nNull;
        unsigned char nGauge;
        unsigned char nText;
    };

    const PowerPane sPowerPane[1] = {
        { 0, 1, 2 }
    };

    // The shadow text, hidden.
    const long PANE_po_te_s = 3;
}

msw::CCockpitBossBallPower::CCockpitBossBallPower(long nId)
    : CCockpitBossLayoutBase(nId)
{
    m_nStep = 0;
}

msw::CCockpitBossBallPower::~CCockpitBossBallPower()
{
    Remove();
}

void msw::CCockpitBossBallPower::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_nStep == 1) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerPane[0].nGauge);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_POWER_IN_WAIT, 1.0f, 0.0f, 0, 1, 1);
        }
        m_nStep = 2;
    }
}

void msw::CCockpitBossBallPower::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

int msw::CCockpitBossBallPower::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    switch (CGame::g_GameWork.m_Unk0B) {
        case 2:
        case 3:
            m_nUnk60 = 3;
            break;
        case 4:
        case 5:
            m_nUnk60 = 5;
            break;
        default:
            m_nUnk60 = 1;
            break;
    }

    const char *pFileName = CCockpit::GetResourceFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, LAYER_POWER, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->BuildLayout(0, sPowerLayoutNames[0]);
            pPaneCtrl->BuildAnim(0, sPowerAnimNames, 7);
            SetPaneAccessor(pPaneCtrl, sPowerPaneNames, sizeof(sPowerPaneNames) / sizeof(sPowerPaneNames[0]));
            pPaneCtrl->SetSortZ(-900.0f);
            m_pPaneAccessor->SetPaneDisp(PANE_po_te_s, 0);
        }
    }
    m_nStep = 1;
    return 0;
}

void msw::CCockpitBossBallPower::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
}

long msw::CCockpitBossBallPower::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return 0;
}

int msw::CCockpitBossBallPower::LifeGaugeOut()
{
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerPane[0].nGauge);

    if (pObj != 0) {
        pObj->PlayAnimation(ANIM_POWER_OUT, 1.0f, 0.0f, 0, 1, 1);
    }
    return 0;
}

int msw::CCockpitBossBallPower::LifeGaugeIn()
{
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerPane[0].nGauge);

    if (pObj != 0) {
        pObj->PlayAnimation(ANIM_POWER_IN, 1.0f, 0.0f, 0, 1, 1);
    }
    return 0;
}

int msw::CCockpitBossBallPower::LifeGaugeOutWait()
{
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerPane[0].nGauge);

    if (pObj != 0) {
        pObj->PlayAnimation(ANIM_POWER_OUT_WAIT, 1.0f, 0.0f, 0, 1, 1);
    }
    return 0;
}

int msw::CCockpitBossBallPower::LifeGaugeInWait()
{
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerPane[0].nGauge);

    if (pObj != 0) {
        pObj->PlayAnimation(ANIM_POWER_IN_WAIT, 1.0f, 0.0f, 0, 1, 1);
    }
    return 0;
}

int msw::CCockpitBossBallPower::SetBallPowerValue(long nTeam, short nValue)
{
    m_pPaneAccessor->SetTextBoxString(sPowerPane[nTeam].nText, nValue, 0);
    return 0;
}

int msw::CCockpitBossBallPower::SetBallPowerValueColor(long nTeam, int bColor)
{
    ChangeTextFont(sPowerPane[nTeam].nText, bColor);
    return 0;
}

int msw::CCockpitBossBallPower::SetBallPowerState(long nTeam, long nState)
{
    static const int recursive = 0;
    static const int reset = 1;

    long nAnim;

    if (nState == 1) {
        nAnim = ANIM_POWER_UP;
    } else if (nState == 2) {
        nAnim = ANIM_POWER_UP2;
    } else {
        nAnim = ANIM_POWER_NORMAL;
    }
    m_pPaneAccessor->PlayAnimation(sPowerPane[nTeam].nText, nAnim, 1.0f, 0.0f, recursive, reset);
    return 0;
}

void msw::CCockpitBossBallPower::ChangeTextFont(long nPane, int bColor)
{
    // Looked up and not used.
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

    if (CLytCtrl::GetInstance() == 0) {
        return;
    }

    nw4r::lyt::ResourceAccessor *pResAccessor =
        CLytCtrl::GetInstance()->GetLayoutResourceAccesor(0, sPowerLayoutNames[0]);

    if (pResAccessor == 0) {
        return;
    }

    void *pFont = pResAccessor->GetResource('font', bColor ? "FOT-RS_NO_01.brfnt" : "FOT-RS_NO_02.brfnt", 0);

    if (pFont == 0) {
        return;
    }

    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(nPane);

    if (pPane == 0) {
        return;
    }

    bcs::G2D::CTextBoxDL *pTextBox = nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pPane);

    if (pTextBox != 0) {
        // Setting the font resets the size: keep it.
        nw4r::lyt::Size size = pTextBox->GetFontSize();

        pTextBox->SetFontByResource(pFont);
        pTextBox->SetFontSize(size);
    }
}

long msw::CCockpitBossBallPower::GetTargetInfo(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize)
{
    if (nTeam >= 1) {
        return -1;
    }

    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(sPowerPane[nTeam].nText);

    if (pPane == 0) {
        return -2;
    }

    rSize = pPane->GetSize();

    nw4r::math::MTX34 mtx = pPane->GetGlobalMtx();

    rPos.x = mtx._03;
    rPos.y = mtx._13;
    rPos.z = mtx._23;
    return 0;
}
