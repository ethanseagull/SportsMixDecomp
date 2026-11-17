// msw::CCockpitDodgeOrDie: the board of the dodgeball "dodge or die" game.
// Each player has a life gauge (two bars, a face icon, a player or COM mark,
// a star for a star player and an "out" telop); the gauges are laid out in
// one of three ways. The clock is the one of CCockpitDodgeCoder.
#include "game/msw/CCockpitDodgeOrDie.hpp"

#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"

// The panes of one player's gauge (indexes into sPaneNames, -1 for none).
// The arrays of three are by face slot: a gauge shared by a team shows one
// face per member.
struct msw::CCockpitDodgeOrDie::LifePane {
    short nGauge;       // 0x00, slides in and plays the hit animations
    short nFace[3];     // 0x02, the parents of the face pictures
    short nMark[3];     // 0x08, the parent of the player or COM mark
    short nStarBase[3]; // 0x0E, the parents of the stars
    short nFacePic[3];  // 0x14, the face pictures (made when missing)
    short nStar[3];     // 0x1A, the stars (copied from cs_ico when missing)
    short nLife;        // 0x20, the first life bar
    short nLife2;       // 0x22, the second life bar
    short nOut;         // 0x24, the "out" telop
};

namespace {

    // Events the cockpit sends (see game/tour/CCockpitTournament.cpp).
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // The resource file of the cockpit that holds the layout.
    const long RES_FILE = 2;

    const char *sLayoutName[] = {
        "DO_dod.brlyt"
    };

    const char *sAnimNames[] = {
        "DO_dod_Cha_life.brlan",
        "DO_dod_Gauge_00.brlan",
        "DO_dod_Gauge_01.brlan",
        "DO_dod_Gauge_02.brlan",
        "DO_dod_Gauge_03.brlan",
        "DO_dod_Hit_00.brlan",
        "DO_dod_Hit_01.brlan",
        "DO_dod_Hit_02.brlan",
        "DO_dod_Hit_03.brlan",
        "DO_dod_Hit_04.brlan",
        "DO_dod_Out_00.brlan",
        "DO_dod_Out_01.brlan",
        "DO_dod_Out_02.brlan",
        "DO_dod_Vis_00.brlan",
        "DO_dod_Vis_01.brlan",
        "DO_dod_Time_00.brlan",
        "DO_dod_Time_01.brlan",
        "DO_dod_Time_vis_00.brlan",
        "DO_dod_Time_vis_01.brlan",
        "DO_dod_Time_vis_02.brlan",
        "DO_dod_TC_00.brlan"
    };

    // Animations (indexes into sAnimNames).
    enum {
        ANIM_LIFE = 0,
        ANIM_GAUGE_IN = 1,
        ANIM_GAUGE_IN_WAIT = 2,
        ANIM_GAUGE_OUT = 3,
        ANIM_GAUGE_OUT_WAIT = 4,
        ANIM_HIT_00 = 5,
        ANIM_HIT_02 = 7,
        ANIM_HIT_03 = 8,
        ANIM_HIT_04 = 9,
        ANIM_OUT_00 = 10,
        ANIM_OUT_01 = 11,
        ANIM_OUT_02 = 12,
        ANIM_TIME = 15,
        ANIM_TIME_ALL = 16,
        ANIM_TIME_NO = 17,
        ANIM_TIME_NO_HURRY = 18,
        ANIM_COUNTDOWN = 20
    };

    const char *sPaneNames[] = {
        "line_L",
        "line_R",
        "L_life_ga2",
        "L_life_ga1",
        "R_life_ga2",
        "R_life_ga1",
        "L_life2A",
        "L_life2B",
        "R_life2A",
        "R_life2B",
        "L_life1A",
        "R_life1A",
        "L_life2A_life",
        "L_life2A_life_2",
        "L_life2B_life",
        "L_life2B_life_2",
        "R_life2A_life",
        "R_life2A_life_2",
        "R_life2B_life",
        "R_life2B_life_2",
        "L_life1A_life",
        "L_life1A_life_2",
        "R_life1A_life",
        "R_life1A_life_2",
        "L_life2A_fe",
        "L_life2B_fe",
        "R_life2A_fe",
        "R_life2B_fe",
        "L1A_fe2_01",
        "L1A_fe2_02",
        "L1A_fe3_01",
        "L1A_fe3_02",
        "L1A_fe3_03",
        "R_life1A_fe",
        "R_life1A_fe",
        "Ph_01",
        "Ph_02",
        "Ph_03",
        "Ph_04",
        "L_life1A_fe_2",
        "L_life1A_fe_3",
        "time",
        "text_time",
        "text_time02",
        "Time_co",
        "time_t",
        "time_no",
        "t_waku",
        "2P",
        "3P",
        "4P",
        "com_01",
        "com_02",
        "com_03",
        "1P_03",
        "2P_02",
        "3P_01",
        "pf00",
        "pf01",
        "pf02",
        "pf03",
        "out_te_Null",
        "out_te_Null_02",
        "out_te_Null_03",
        "out_te_Null_04",
        "out_te_Null_05",
        "out_te_Null_06",
        "L_Ca_2A",
        "L_Ca_1A",
        "R_Ca_2A",
        "R_Ca_2B",
        "L_Ca_3A",
        "L_Ca_4A",
        "L_Ca_5A",
        "L_Ca_6A",
        "L_Ca_7A",
        "R_Ca_A1",
        "R_Ca_A1",
        "cs_ico",
        "star00",
        "star01",
        "star02",
        "star03"
    };

    // Number of panes in sPaneNames.
    const long PANE_NUM = 83;

    // Panes (indexes into sPaneNames) used by name in the code.
    enum {
        PANE_L_life_ga2 = 2,
        PANE_L_life_ga1 = 3,
        PANE_R_life_ga2 = 4,
        PANE_R_life_ga1 = 5,
        PANE_L_life1A_fe_2 = 39,
        PANE_L_life1A_fe_3 = 40,
        PANE_time = 41,
        PANE_text_time = 42,
        PANE_text_time02 = 43,
        PANE_Time_co = 44,
        PANE_time_t = 45,
        PANE_time_no = 46,
        PANE_t_waku = 47,
        PANE_cs_ico = 78
    };

    // Effects.
    enum {
        EFFECT_COUNTDOWN = 0x4D0
    };

    const long SIDE_NUM = 2;

    // The two halves of the board (they slide as the gauge part).
    const unsigned char sLinePane[SIDE_NUM] = { 0, 1 };

    typedef msw::CCockpitDodgeOrDie::LifePane LifePane;

    // The gauges, by layout (see SetLifeGaugeLayout). Layout 0: one gauge
    // per player.
    const LifePane sLifePane0[4] = {
        { 6, { 24, -1, -1 }, { -1, -1, -1 }, { 67, -1, -1 }, { 57, -1, -1 }, { 79, -1, -1 }, 12, 13, 64 },
        { 7, { 25, -1, -1 }, { 35, -1, -1 }, { 68, -1, -1 }, { 58, -1, -1 }, { 80, -1, -1 }, 14, 15, 65 },
        { 8, { 26, -1, -1 }, { 36, -1, -1 }, { 69, -1, -1 }, { 59, -1, -1 }, { 81, -1, -1 }, 16, 17, 61 },
        { 9, { 27, -1, -1 }, { 37, -1, -1 }, { 70, -1, -1 }, { 60, -1, -1 }, { 82, -1, -1 }, 18, 19, 62 }
    };
    // Layout 1, by side and gauge: the first two players share the left
    // gauge, the other two have one each.
    const LifePane sLifePane1[SIDE_NUM][3] = {
        {
            { 10, { 28, 29, -1 }, { -1, -1, -1 }, { 71, 72, -1 }, { 57, 58, -1 }, { 79, 80, -1 }, 20, 21, 66 }
        },
        {
            { 8, { 26, -1, -1 }, { 36, -1, -1 }, { 69, -1, -1 }, { 59, -1, -1 }, { 81, -1, -1 }, 16, 17, 61 },
            { 9, { 27, -1, -1 }, { 37, -1, -1 }, { 70, -1, -1 }, { 60, -1, -1 }, { 82, -1, -1 }, 18, 19, 62 }
        }
    };
    // Layout 2: the first three players share the left gauge, the fourth
    // has the right one.
    const LifePane sLifePane2[SIDE_NUM] = {
        { 10, { 30, 31, 32 }, { -1, -1, -1 }, { 73, 74, 75 }, { 57, 58, 59 }, { 79, 80, 81 }, 20, 21, 66 },
        { 11, { 34, -1, -1 }, { 38, -1, -1 }, { 77, -1, -1 }, { 60, -1, -1 }, { 82, -1, -1 }, 22, 23, 63 }
    };

    // The mark of a player by pad number minus 1, and of a COM by player
    // slot minus 1 ("2P" to "4P", "com_01" to "com_03").
    const unsigned char sPadMarkPane[6] = { 48, 49, 50, 51, 52, 53 };
    const unsigned char sComMarkPane[4] = { 51, 52, 53 };
}

inline const msw::CCockpitDodgeOrDie::LifePane *msw::CCockpitDodgeOrDie::GetLifePane(long nNo)
{
    switch (m_nLayout) {
        case 0:
            return &sLifePane0[nNo];
        case 1:
            switch (nNo) {
                case 2:
                    return &sLifePane1[1][0];
                case 3:
                    return &sLifePane1[1][1];
            }
            return &sLifePane1[0][0];
        case 2:
            if (nNo == 3) {
                return &sLifePane2[1];
            }
            return &sLifePane2[0];
    }
    return 0;
}

msw::CCockpitDodgeOrDie::CCockpitDodgeOrDie(long nId)
    : CCockpitDodgeLayoutBase(nId)
{
    m_nStep = 0;
    m_nPlayerNum = 4;
    m_nLayout = 0;
}

msw::CCockpitDodgeOrDie::~CCockpitDodgeOrDie()
{
    Remove();
}

void msw::CCockpitDodgeOrDie::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_nStep == 1) {
        SetupGame();
        m_nStep = 2;
    }
}

void msw::CCockpitDodgeOrDie::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

int msw::CCockpitDodgeOrDie::Set()
{
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
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(2), pLytCtrl->GetFontResourceName(2));
            pPaneCtrl->BuildLayout(0, sLayoutName[0]);
            pPaneCtrl->BuildAnim(0, sAnimNames, sizeof(sAnimNames) / sizeof(sAnimNames[0]));
            SetPaneAccessor(pPaneCtrl, sPaneNames, PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(0.0f);
            pPaneCtrl->SetVisible(0);
            SetLifeGaugeLayout(m_nLayout);
        }
    }
    m_EffectSet.Initialize();
    m_nStep = 1;
    return 0;
}

void msw::CCockpitDodgeOrDie::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_EffectSet.Finalize();
    m_nStep = 0;
}

long msw::CCockpitDodgeOrDie::SendEvent(long nEvent, void *pParam)
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

void msw::CCockpitDodgeOrDie::SetupGame()
{
    static const long animId = ANIM_HIT_00;

    GaugeInWait();

    long nNum = m_nPlayerNum;

    for (long i = 0; i < nNum; i++) {
        m_pPaneAccessor->PlayAnimation(GetLifePane(i)->nGauge, animId, 1.0f, 0.0f, 1, 1);
    }
}

inline void msw::CCockpitDodgeOrDie::PlayAnimGauge(long nAnim)
{
    for (long i = 0; i < SIDE_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sLinePane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
        }
    }

    bcs::G2D::CPaneObj *pTime = m_pPaneAccessor->GetPaneObj(PANE_time);

    if (pTime != 0) {
        pTime->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
    }
}

int msw::CCockpitDodgeOrDie::GaugeOut()
{
    PlayAnimGauge(ANIM_GAUGE_OUT);
    return 0;
}

int msw::CCockpitDodgeOrDie::GaugeIn()
{
    PlayAnimGauge(ANIM_GAUGE_IN);
    return 0;
}

int msw::CCockpitDodgeOrDie::GaugeOutWait()
{
    PlayAnimGauge(ANIM_GAUGE_OUT_WAIT);
    return 0;
}

int msw::CCockpitDodgeOrDie::GaugeInWait()
{
    PlayAnimGauge(ANIM_GAUGE_IN_WAIT);
    return 0;
}

void msw::CCockpitDodgeOrDie::SetTimeDisp(int bDisp)
{
    m_pPaneAccessor->SetPaneDisp(PANE_time, bDisp);
}

void msw::CCockpitDodgeOrDie::SetLifeDisp(int bDisp)
{
    for (long i = 0; i < SIDE_NUM; i++) {
        m_pPaneAccessor->SetPaneDisp(sLinePane[i], bDisp);
    }
}

void msw::CCockpitDodgeOrDie::SetTimeValue(float fTime, int nArg, int bArg)
{
    m_TimeCount.SetTimeValue(m_pPaneAccessor, fTime, nArg, ANIM_TIME, ANIM_TIME_ALL, PANE_text_time,
                             PANE_text_time02, bArg);

    long nSec = m_TimeCount.GetSec(fTime);

    if (nSec == m_TimeCount.GetShowSec()) {
        return;
    }

    // The countdown of the last seconds.
    m_TimeCount.SetCountDown(m_pPaneAccessor, nSec, PANE_Time_co, PANE_time_t, ANIM_COUNTDOWN);
    m_EffectSet.RemoveEffect(0, 0);
    if (nSec >= 1 && nSec <= 9) {
        m_EffectSet.SetEffect(0, 0, EFFECT_COUNTDOWN, m_pPaneAccessor->GetPaneObj(PANE_t_waku));
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_time_no);

    if (pObj != 0) {
        if (nSec <= 10 && !pObj->IsAnimPlay(ANIM_TIME_NO_HURRY)) {
            pObj->PlayAnimation(ANIM_TIME_NO_HURRY, 1.0f, 0.0f, 0, 1, 1);
        } else if (nSec > 10 && !pObj->IsAnimPlay(ANIM_TIME_NO)) {
            pObj->PlayAnimation(ANIM_TIME_NO, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    m_TimeCount.SetShowSec(nSec);
}

void msw::CCockpitDodgeOrDie::LifeGaugeAnim(long nIdx, long nNo, long nAnim, float fSpeed, float fFrame)
{
    // static-order swap (notes/decisions.md): declared in reverse.
    static const int reset = 1;
    static const int recursive = 1;

    m_pPaneAccessor->PlayAnimation(GetLifePane(nNo)->nGauge, nAnim, fSpeed, fFrame, recursive, reset);
}

void msw::CCockpitDodgeOrDie::SetLifeValue(long nIdx, long nNo, float fRate, float fRate2)
{
    // static-order swap (notes/decisions.md): declared in reverse.
    static const int reset = 1;
    static const int recursive = 1;
    static const float speed = 0.0f;
    static const long animId = ANIM_LIFE;

    const LifePane *pPane = GetLifePane(nNo);

    if (pPane == 0) {
        return;
    }

    float fLife = fRate;

    if (fRate > 1.0f) {
        fLife = 1.0f;
    } else if (fRate < 0.0f) {
        fLife = 0.0f;
    }

    float fLife2 = fRate2;

    if (fRate2 > 1.0f) {
        fLife2 = 1.0f;
    } else if (fRate2 < 0.0f) {
        fLife2 = 0.0f;
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(pPane->nLife);
    bcs::G2D::CPaneObj *pObj2 = m_pPaneAccessor->GetPaneObj(pPane->nLife2);

    if (pObj == 0) {
        return;
    }

    // The bars run backwards: a full life is frame 0.
    float fMax = pObj->GetAnimMaxFrame(animId);
    float fFrame = fMax * (1.0f - fLife);
    float fFrame2 = fMax * (1.0f - fLife2);

    pObj->PlayAnimation(animId, speed, fFrame, recursive, reset, 1);
    if (pObj2 != 0) {
        pObj2->PlayAnimation(animId, speed, fFrame2, recursive, reset, 1);
    }
}

void msw::CCockpitDodgeOrDie::SetFaceIcon(long nIdx, long nNo, const CChrCore *pChr)
{
    if (pChr == 0) {
        return;
    }

    const LifePane *pPane = GetLifePane(nNo);

    if (pPane == 0) {
        return;
    }

    nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    long nSlot;

    // The face slot of the player on a shared gauge.
    switch (m_nLayout) {
        default:
            nSlot = 0;
            break;
        case 1:
            switch (nNo) {
                case 0:
                case 1:
                    nSlot = nNo;
                    break;
                default:
                    nSlot = 0;
                    break;
            }
            break;
        case 2:
            nSlot = (unsigned long)nNo <= 2 ? nNo : 0;
            break;
    }

    // The face picture, made once.
    nw4r::lyt::Pane *pFace = m_pPaneAccessor->GetPane(pPane->nFace[nSlot]);

    if (pFace != 0) {
        long nPic = pPane->nFacePic[nSlot];

        if (m_pPaneAccessor->GetPane(nPic) == 0) {
            nw4r::lyt::Pane *pPic =
                menu::CreateFaceIconPicturePane(pChr->GetPlayerNo(), GetLytObj(), pFace, sPaneNames[nPic]);

            bcs::G2D::SetPaneSizeFromParent(pPic);
            pPic->SetScale(menu::GetCockpitFaceIconScale(pChr->GetChrIdS()));
        }
    }

    // The player or COM mark.
    nw4r::lyt::Pane *pMarkBase = 0;
    nw4r::lyt::Pane *pMark = 0;

    switch (m_nLayout) {
        case 1:
            if (!pChr->IsPadCtrl() && pPane->nMark[0] != -1) {
                pMarkBase = m_pPaneAccessor->GetPane(pPane->nMark[0]);
                pMark = m_pPaneAccessor->GetPane(sComMarkPane[nNo - 1]);
            }
            break;
        case 2:
            if (!pChr->IsPadCtrl() && pPane->nMark[0] != -1) {
                pMarkBase = m_pPaneAccessor->GetPane(pPane->nMark[0]);
                pMark = m_pPaneAccessor->GetPane(sComMarkPane[nNo - 1]);
            }
            break;
        default:
            if (pPane->nMark[0] != -1) {
                pMarkBase = m_pPaneAccessor->GetPane(pPane->nMark[0]);
                if (pChr->IsPadCtrl()) {
                    long nPad = pChr->GetPadNo();

                    if (nPad > 0) {
                        pMark = m_pPaneAccessor->GetPane(sPadMarkPane[nPad - 1]);
                    }
                } else if (nNo > 0) {
                    pMark = m_pPaneAccessor->GetPane(sComMarkPane[nNo - 1]);
                }
            }
            break;
    }

    bcs::G2D::SetPaneParent(pMark, pMarkBase, 1, 0);
    if (pMark != 0) {
        pMark->SetTranslate(zero);
        pMark->SetVisible(true);
    }

    // The star, a copy of cs_ico made once, shown for a star player.
    nw4r::lyt::Pane *pStarSrc = m_pPaneAccessor->GetPane(PANE_cs_ico);
    nw4r::lyt::Pane *pStarBase = m_pPaneAccessor->GetPane(pPane->nStarBase[nSlot]);

    if (pStarSrc != 0 && pStarBase != 0) {
        long nStar = pPane->nStar[nSlot];
        nw4r::lyt::Pane *pStar = m_pPaneAccessor->GetPane(nStar);

        if (pStar == 0) {
            pStar = bcs::G2D::DuplicatePane(GetLytObj(), pStarSrc, sPaneNames[nStar], sLayoutName[0], 0);
            bcs::G2D::SetPaneParent(pStar, pStarBase, 1, 0);
            bcs::G2D::SetPaneSizeFromParent(pStar);
            pStar->SetTranslate(zero);
        }
        if (pStar != 0) {
            pStar->SetVisible(pChr->IsStarPlayer() != 0);
        }
    }
}

void msw::CCockpitDodgeOrDie::SetLifeGaugeAnim(long nIdx, long nNo, long nState)
{
    if (GetLifePane(nNo) == 0) {
        return;
    }

    long nAnim;

    switch (nState) {
        default:
            nAnim = ANIM_HIT_00;
            break;
        case 1:
            nAnim = ANIM_HIT_02;
            break;
        case 2:
            nAnim = ANIM_HIT_04;
            break;
        case 3:
            nAnim = ANIM_HIT_03;
            break;
    }
    LifeGaugeAnim(nIdx, nNo, nAnim, 1.0f, 0.0f);
}

void msw::CCockpitDodgeOrDie::SetLifeGaugeLayout(long nLayout)
{
    m_nLayout = nLayout;

    switch (nLayout) {
        case 0:
            m_pPaneAccessor->SetPaneDisp(PANE_L_life_ga2, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_R_life_ga2, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_L_life_ga1, 0);
            m_pPaneAccessor->SetPaneDisp(PANE_R_life_ga1, 0);
            break;
        case 1:
            m_pPaneAccessor->SetPaneDisp(PANE_L_life_ga1, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_R_life_ga2, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_L_life_ga2, 0);
            m_pPaneAccessor->SetPaneDisp(PANE_R_life_ga1, 0);
            m_pPaneAccessor->SetPaneDisp(PANE_L_life1A_fe_2, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_L_life1A_fe_3, 0);
            break;
        case 2:
            m_pPaneAccessor->SetPaneDisp(PANE_L_life_ga1, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_R_life_ga1, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_L_life_ga2, 0);
            m_pPaneAccessor->SetPaneDisp(PANE_R_life_ga2, 0);
            m_pPaneAccessor->SetPaneDisp(PANE_L_life1A_fe_2, 0);
            m_pPaneAccessor->SetPaneDisp(PANE_L_life1A_fe_3, 1);
            break;
    }
}

void msw::CCockpitDodgeOrDie::SetOutTelop(long nIdx, long nNo, int bOut)
{
    // static-order swap (notes/decisions.md): declared in reverse.
    static const long animId = ANIM_OUT_00;
    static const long animId1 = ANIM_OUT_02;
    static const long animId0 = ANIM_OUT_01;
    static const int reset = 1;
    static const int recursive = 0;

    CMenuPaneAccessor *pAccessor = m_pPaneAccessor;

    if (pAccessor == 0) {
        return;
    }

    const LifePane *pPane = GetLifePane(nNo);

    if (pPane == 0) {
        return;
    }

    bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(pPane->nOut);

    if (pObj == 0) {
        return;
    }

    // The telop's parent is shown while the player is out.
    nw4r::lyt::Pane *pParent = pObj->GetPane()->GetParent();

    if (bOut) {
        pObj->PlayAnimation(animId0, 1.0f, 0.0f, recursive, reset, 1);
        pObj->PushNextAnimation(animId1, 1.0f, 0.0f, recursive, reset);
        if (pParent != 0) {
            pParent->SetVisible(true);
        }
    } else {
        pObj->PlayAnimation(animId, 1.0f, 0.0f, recursive, reset, 1);
        if (pParent != 0) {
            pParent->SetVisible(false);
        }
    }
}
