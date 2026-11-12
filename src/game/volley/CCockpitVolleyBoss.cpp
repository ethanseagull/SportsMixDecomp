// msw::CCockpitVolleyBoss and msw::CCockpitVolleyLifeGauge: the volley boss
// match's cockpit layouts. The first shows the boss's life (with its line
// animations), the special coin and the time; the second the life gauge of
// the players' team with a face icon per character.
#include "game/msw/CCockpitVolleyBoss.hpp"
#include "game/msw/CCockpitVolleyLifeGauge.hpp"

#include <wchar.h>
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CTexMap.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/cockpit/CockpitUtil.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"

// msw::CCockpitVolleyBoss

namespace {

    // Events the cockpit sends (see game/tour/CCockpitTournament.cpp).
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // The resource file of the cockpit that holds the layouts.
    const long RES_FILE = 2;

    const char *sLayoutNames[] = {
        "VO_boss.brlyt",
        0
    };

    // Animations, by index into sAnimNames.
    enum {
        ANIM_COIN_00,
        ANIM_COIN_01,
        ANIM_COIN_02,
        ANIM_GAUGE_IN,        // Ga_00
        ANIM_GAUGE_IN_WAIT,   // Ga_01
        ANIM_GAUGE_OUT,       // Ga_02
        ANIM_GAUGE_OUT_WAIT,  // Ga_03
        ANIM_LINE_IN,         // HP_line_00
        ANIM_LINE_IN_WAIT,    // HP_line_01
        ANIM_LINE_OUT,        // HP_line_02, also the life gauge
        ANIM_LINE_OUT_WAIT,   // HP_line_03
        ANIM_SP,
        ANIM_TIME,
        ANIM_TIME_ALL,
        ANIM_VIS_COIN_00,
        ANIM_VIS_COIN_01,
        ANIM_NUM
    };

    const char *sAnimNames[ANIM_NUM] = {
        "VO_boss_Coin_00.brlan",
        "VO_boss_Coin_01.brlan",
        "VO_boss_Coin_02.brlan",
        "VO_boss_Ga_00.brlan",
        "VO_boss_Ga_01.brlan",
        "VO_boss_Ga_02.brlan",
        "VO_boss_Ga_03.brlan",
        "VO_boss_HP_line_00.brlan",
        "VO_boss_HP_line_01.brlan",
        "VO_boss_HP_line_02.brlan",
        "VO_boss_HP_line_03.brlan",
        "VO_boss_SP_00.brlan",
        "VO_boss_Time_00.brlan",
        "VO_boss_Time_01.brlan",
        "VO_boss_Vis_coin_00.brlan",
        "VO_boss_Vis_coin_01.brlan"
    };

    // Panes, by index into sPaneNames.
    enum {
        PANE_Boss_HP,
        PANE_line_L,
        PANE_hp_bar_1,
        PANE_hp_bar_2,
        PANE_hp,
        PANE_line_bar,
        PANE_coin_L_ani,
        PANE_coinL_anime,
        PANE_L_flame_pic,
        PANE_L_special_pic,
        PANE_coinL_SP,
        PANE_text_time_kage,
        PANE_text_time,
        PANE_text_time02_kage,
        PANE_text_time02,
        PANE_NUM
    };

    const char *sPaneNames[PANE_NUM + 1] = {
        "Boss_HP",
        "line_L",
        "hp_bar_1",
        "hp_bar_2",
        "hp",
        "line_bar",
        "coin_L_ani",
        "coinL_anime",
        "L_flame_pic",
        "L_special_pic",
        "coinL_SP",
        "text_time_kage",
        "text_time",
        "text_time02_kage",
        "text_time02",
        0
    };

    // m_nState.
    enum {
        STATE_IN = 1,
        STATE_GAUGE_IN = 2,
        STATE_SHOW = 3,
        STATE_OUT = 4,
        STATE_OUT_WAIT = 5
    };

    // SetCoinState.
    enum {
        COIN_STATE_0 = 0,
        COIN_STATE_1 = 1,
        COIN_STATE_2 = 2,
        COIN_STATE_SPECIAL = 3
    };

    // A stand-in for an unknown debug-only callee: keeps the vtable stores
    // of the destructor (notes/decisions.md).
    static inline void Nothing() {}
}

msw::CCockpitVolleyBoss::CCockpitVolleyBoss(long nId, float fSortZ)
    : CCockpitVolleyLayoutBase(nId)
{
    m_fLayoutSortZ = fSortZ;
    m_nStep = 0;
    m_nState = 0;
}

msw::CCockpitVolleyBoss::~CCockpitVolleyBoss()
{
    Nothing();
}

int msw::CCockpitVolleyBoss::Set()
{
    CCockpit *pCockpit = CCockpit::GetTask();

    if (pCockpit == 0) {
        return -1;
    }

    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -2;
    }

    const char *pFileName = CCockpit::GetResourceFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor == 0) {
        return -3;
    }

    bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

    if (pPaneCtrl == 0) {
        return -5;
    }

    const char *pFontName1 = pLytCtrl->GetFontResourceName(1);

    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pFontName1);

    const char *pFontName2 = pLytCtrl->GetFontResourceName(2);

    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(2), pFontName2);
    pPaneCtrl->BuildLayout(0, sLayoutNames[0]);
    pPaneCtrl->BuildAnim(0, sAnimNames, ANIM_NUM);
    pPaneCtrl->SetSortZ(m_fLayoutSortZ);
    SetPaneAccessor(pPaneCtrl, sPaneNames, PANE_NUM);
    SetTextBoxFromMessage(pPaneCtrl);

    float fFrame = pPaneCtrl->GetAnimFrameMax(ANIM_LINE_OUT);

    if (fFrame > 0.0f) {
        m_fLifeFrame = fFrame;
    } else {
        m_fLifeFrame = 10.0f;
    }

    m_nStep = 1;
    m_nState = STATE_IN;
    return 0;
}

void msw::CCockpitVolleyBoss::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

void msw::CCockpitVolleyBoss::Main()
{
    switch (m_nState) {
        case STATE_GAUGE_IN:
            if (m_pPaneAccessor->GetPaneObj(PANE_Boss_HP)->IsAnimEnd(ANIM_GAUGE_IN)) {
                SetAnimHPLine(ANIM_LINE_IN);
                m_nState = STATE_SHOW;
            }
            break;
        case STATE_OUT:
            m_nState = STATE_OUT_WAIT;
            break;
        case STATE_OUT_WAIT:
            if (m_pPaneAccessor->GetPaneObj(PANE_Boss_HP)->IsAnimEnd(ANIM_GAUGE_OUT)) {
                // Done: the menu task deletes the object.
                m_Unk08 = 2;
            }
            break;
    }
}

long msw::CCockpitVolleyBoss::SendEvent(long, void *)
{
    return 0;
}

void msw::CCockpitVolleyBoss::SetAnimGauge(long nAnim)
{
    // static-order swap: the original names these @reset@0 and @recursive;
    // declared in this order the objects are in the original's places
    // (notes/decisions.md).
    static const int reset = 0;
    static const int recursive = 0;
    long nNext;

    switch (nAnim) {
        case ANIM_GAUGE_IN:
            nNext = ANIM_GAUGE_IN_WAIT;
            break;
        case ANIM_GAUGE_OUT:
            nNext = ANIM_GAUGE_OUT_WAIT;
            break;
        default:
            nNext = -1;
            break;
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_line_L);

    if (pObj != 0) {
        pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
        if (nNext >= 0) {
            pObj->PushNextAnimation(nNext, 1.0f, 0.0f, recursive, 1);
        }
    }

    pObj = m_pPaneAccessor->GetPaneObj(PANE_Boss_HP);

    if (pObj != 0) {
        pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
        if (nNext >= 0) {
            pObj->PushNextAnimation(nNext, 1.0f, 0.0f, recursive, 1);
        }
    }
}

void msw::CCockpitVolleyBoss::SetAnimHPLine(long nAnim)
{
    long nNext;

    switch (nAnim) {
        case ANIM_LINE_IN:
            nNext = ANIM_LINE_IN_WAIT;
            break;
        default:
            nNext = -1;
            break;
    }

    long paneIdTable[] = { PANE_hp, PANE_hp_bar_1, PANE_hp_bar_2, PANE_line_bar };
    static const long paneIdTableLength = 4;
    // static-order swap, as in SetAnimGauge.
    static const int reset = 0;
    static const int recursive = 0;

    for (long i = 0; i < paneIdTableLength; i++) {
        long nPaneId = paneIdTable[i];
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPaneId);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
            if (nNext >= 0) {
                pObj->PushNextAnimation(nNext, 1.0f, 0.0f, recursive, 1);
            }
        }
    }
}

// Inlined at its callers; the module link stripped this out-of-line copy,
// kept for its two static locals, whose @LOCAL@ symbols are still in the
// original .rodata (VolleyF symbols.txt).
void msw::CCockpitVolleyBoss::SetAnimLineBar(long nAnim)
{
    // static-order swap, as in SetAnimGauge.
    static const int reset = 0;
    static const int recursive = 0;
    long nNext;

    switch (nAnim) {
        case ANIM_LINE_IN:
            nNext = ANIM_LINE_IN_WAIT;
            break;
        case ANIM_LINE_OUT:
            nNext = ANIM_LINE_OUT_WAIT;
            break;
        default:
            nNext = -1;
            break;
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_line_bar);

    if (pObj != 0) {
        pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
        if (nNext >= 0) {
            pObj->PushNextAnimation(nNext, 1.0f, 0.0f, recursive, 1);
        }
    }
}

// Inlined at its callers; the module link stripped this out-of-line copy,
// kept for its two static locals, whose @LOCAL@ symbols are still in the
// original .rodata (VolleyF symbols.txt).
void msw::CCockpitVolleyBoss::SetAnimCoin(long nAnim)
{
    // static-order swap, as in SetAnimGauge.
    static const int reset = 0;
    static const int recursive = 0;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_coinL_anime);

    if (pObj != 0) {
        pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
    }
}

// Inlined at its callers; the module link stripped this out-of-line copy,
// kept for its two static locals, whose @LOCAL@ symbols are still in the
// original .rodata (VolleyF symbols.txt).
void msw::CCockpitVolleyBoss::SetAnimCoinSpecial(long nAnim)
{
    // static-order swap, as in SetAnimGauge.
    static const int reset = 0;
    static const int recursive = 0;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_L_flame_pic);

    if (pObj != 0) {
        pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
    }

    pObj = m_pPaneAccessor->GetPaneObj(PANE_L_special_pic);

    if (pObj != 0) {
        pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
    }
}

void msw::CCockpitVolleyBoss::SetBossLife(float fRate)
{
    static const long animId = ANIM_LINE_OUT;
    // static-order swap, as in SetAnimGauge.
    static const int reset = 0;
    static const int recursive = 0;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_hp);

    if (pObj != 0) {
        // A full gauge is frame 0.
        float fValue = 1.0f - fRate;

        if (fValue > 1.0f) {
            fValue = 1.0f;
        } else if (fValue < 0.0f) {
            fValue = 0.0f;
        }
        pObj->PlayAnimation(animId, 0.0f, fValue * m_fLifeFrame, recursive, reset, 1);
    }
}

void msw::CCockpitVolleyBoss::SetTimeValue(float fTime, int nArg, int bForce)
{
    cockpit::SetTimeValue(m_pPaneAccessor, fTime, nArg, bForce, ANIM_TIME, ANIM_TIME_ALL, PANE_text_time,
                          PANE_text_time_kage, -1, -1, 0);
}

int msw::CCockpitVolleyBoss::ScoreIn()
{
    if (m_nState == STATE_IN) {
        m_pPaneAccessor->SetPaneDisp(PANE_coin_L_ani, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_coinL_SP, 0);
        SetAnimCoin(ANIM_COIN_00);
        m_nState = STATE_GAUGE_IN;
    }
    SetAnimGauge(ANIM_GAUGE_IN);
    SetAnimLineBar(ANIM_LINE_IN);
    return 0;
}

int msw::CCockpitVolleyBoss::ScoreInWait()
{
    SetAnimGauge(ANIM_GAUGE_IN_WAIT);
    SetAnimLineBar(ANIM_LINE_IN_WAIT);
    return 0;
}

int msw::CCockpitVolleyBoss::ScoreOut()
{
    SetAnimGauge(ANIM_GAUGE_OUT);
    SetAnimLineBar(ANIM_LINE_OUT);
    return 0;
}

int msw::CCockpitVolleyBoss::ScoreOutWait()
{
    SetAnimGauge(ANIM_GAUGE_OUT_WAIT);
    SetAnimLineBar(ANIM_LINE_OUT_WAIT);
    return 0;
}

void msw::CCockpitVolleyBoss::SetCoinState(long nState)
{
    switch (nState) {
        case COIN_STATE_0:
            m_pPaneAccessor->SetPaneDisp(PANE_coin_L_ani, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_coinL_SP, 0);
            SetAnimCoin(ANIM_COIN_00);
            break;
        case COIN_STATE_1:
            m_pPaneAccessor->SetPaneDisp(PANE_coin_L_ani, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_coinL_SP, 0);
            SetAnimCoin(ANIM_COIN_01);
            break;
        case COIN_STATE_2:
            m_pPaneAccessor->SetPaneDisp(PANE_coin_L_ani, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_coinL_SP, 0);
            SetAnimCoin(ANIM_COIN_02);
            break;
        case COIN_STATE_SPECIAL:
            m_pPaneAccessor->SetPaneDisp(PANE_coin_L_ani, 0);
            m_pPaneAccessor->SetPaneDisp(PANE_coinL_SP, 1);
            SetAnimCoinSpecial(ANIM_SP);
            break;
    }
}

long msw::CCockpitVolleyBoss::CancelMenu()
{
    m_nState = STATE_OUT;
    return 0;
}

// msw::CCockpitVolleyLifeGauge

namespace {

    const char *const sLifeLayoutNames[] = {
        "VO_boss_6.brlyt"
    };

    // Animations, by index into sLifeAnimNames.
    enum {
        ANIM_LIFE_GAUGE_IN,       // Ga_00
        ANIM_LIFE_GAUGE_IN_WAIT,  // Ga_01
        ANIM_LIFE_GAUGE_OUT,      // Ga_02
        ANIM_LIFE_GAUGE_OUT_WAIT, // Ga_03
        ANIM_LIFE_HIT_00,
        ANIM_LIFE_HIT_01,
        ANIM_LIFE_00,             // Life_00 to Life_10
        ANIM_LIFE_BREAK_00 = ANIM_LIFE_00 + 11,
        ANIM_LIFE_NUM = ANIM_LIFE_BREAK_00 + 3
    };

    const char *sLifeAnimNames[ANIM_LIFE_NUM] = {
        "VO_boss_6_Ga_00.brlan",
        "VO_boss_6_Ga_01.brlan",
        "VO_boss_6_Ga_02.brlan",
        "VO_boss_6_Ga_03.brlan",
        "VO_boss_6_Hit_00.brlan",
        "VO_boss_6_Hit_01.brlan",
        "VO_boss_6_Life_00.brlan",
        "VO_boss_6_Life_01.brlan",
        "VO_boss_6_Life_02.brlan",
        "VO_boss_6_Life_03.brlan",
        "VO_boss_6_Life_04.brlan",
        "VO_boss_6_Life_05.brlan",
        "VO_boss_6_Life_06.brlan",
        "VO_boss_6_Life_07.brlan",
        "VO_boss_6_Life_08.brlan",
        "VO_boss_6_Life_09.brlan",
        "VO_boss_6_Life_10.brlan",
        "VO_boss_6_Life_break_00.brlan",
        "VO_boss_6_Life_break_01.brlan",
        "VO_boss_6_Life_break_02.brlan"
    };

    // Panes, by index into sLifePaneNames.
    enum {
        LPANE_line_L,
        LPANE_L_life_ga1_A,
        LPANE_L_life1A,
        LPANE_L_life1A_fe_1,
        LPANE_L_life1A_fe_2,
        LPANE_L_life1A_fe_3,
        LPANE_L_safe_ga1_A,
        LPANE_L_life_ga1,
        LPANE_L_life1A_life_1,
        LPANE_L_life1A_num = LPANE_L_life1A_life_1 + 6,
        LPANE_L_life1A_life1p,
        LPANE_IcFace00 = LPANE_L_life1A_life1p + 6,
        LPANE_NUM = LPANE_IcFace00 + 3
    };

    const char *sLifePaneNames[LPANE_NUM] = {
        "line_L",
        "L_life_ga1_A",
        "L_life1A",
        "L_life1A_fe_1",
        "L_life1A_fe_2",
        "L_life1A_fe_3",
        "L_safe_ga1_A",
        "L_life_ga1",
        "L_life1A_life_1",
        "L_life1A_life_2",
        "L_life1A_life_3",
        "L_life1A_life_4",
        "L_life1A_life_5",
        "L_life1A_life_6",
        "L_life1A_num",
        "L_life1A_life1p",
        "L_life1A_life2p",
        "L_life1A_life3p",
        "L_life1A_life4p",
        "L_life1A_life5p",
        "L_life1A_life6p",
        "IcFace00",
        "IcFace01",
        "IcFace02"
    };

    // The face picture of each character.
    unsigned char sFacePane[3] = { LPANE_IcFace00, LPANE_IcFace00 + 1, LPANE_IcFace00 + 2 };

    // Number of teams with a gauge.
    const long TEAM_NUM = 1;

    // The gauge of each team, slid in and out.
    const unsigned char sGaugePane[TEAM_NUM] = { LPANE_line_L };

    // The panes of one character's gauge (only nFaceParent is read).
    struct ChrPane {
        unsigned char nGauge;
        unsigned char nFaceParent;
        unsigned char nLife;
        unsigned char nSafe;
    };

    // By the number of characters, then character.
    const ChrPane sChrPane[3][3] = {
        {
            { 1, 3, 2, 6 }
        },
        {
            { 1, 3, 2, 6 },
            { 1, 4, 2, 6 }
        },
        {
            { 1, 3, 2, 6 },
            { 1, 4, 2, 6 },
            { 1, 5, 2, 6 }
        }
    };

    // The six life marks of a team and its life count.
    const long LIFE_MARK_NUM = 6;

    struct LifePane {
        unsigned char nMark[LIFE_MARK_NUM];
        unsigned char nMarkP[LIFE_MARK_NUM]; // not read
        unsigned char nNum;
    };

    const LifePane sLifePane[TEAM_NUM] = {
        { { 8, 9, 10, 11, 12, 13 }, { 15, 16, 17, 18, 19, 20 }, LPANE_L_life1A_num }
    };

    // Life mark animations by the number of full bars. SetLifeValue reads
    // row 2 too (up to 18 points), past the end of the table, as the
    // original does: it gets zeros there.
    const unsigned char sLifeMarkAnim[2][5] = {
        { 10, 11, 12, 18, 9 },
        { 6, 7, 8, 17, 6 }
    };
}

msw::CCockpitVolleyLifeGauge::CCockpitVolleyLifeGauge(long nId, float fSortZ)
    : CCockpitVolleyLayoutBase(nId)
{
    m_fLayoutSortZ = fSortZ;
    m_nStep = 0;
    m_nState = 0;
}

msw::CCockpitVolleyLifeGauge::~CCockpitVolleyLifeGauge()
{
    Remove();
}

void msw::CCockpitVolleyLifeGauge::Main()
{
    switch (m_nState) {
        case 1:
            for (long i = 0; i < TEAM_NUM; i++) {
                bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sGaugePane[i]);

                if (pObj != 0) {
                    pObj->PlayAnimation(ANIM_LIFE_GAUGE_OUT_WAIT, 1.0f, 0.0f, 0, 1, 1);
                }
            }
            SetLifeValue(0, 0, 6);
            m_nState = 2;
            break;
        case 2:
            m_nState = 3;
            break;
        case 4:
            m_nState = 5;
            break;
        case 5:
            // Done: the menu task deletes the object.
            m_Unk08 = 2;
            break;
    }
}

void msw::CCockpitVolleyLifeGauge::Disp()
{
}

int msw::CCockpitVolleyLifeGauge::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();

    m_nChrNum = CChrMan::GetChrCoreNum() >> 1;

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

            long nChrNum = m_nChrNum;

            pPaneCtrl->BuildLayout(0, sLifeLayoutNames[0]);
            pPaneCtrl->BuildAnim(0, sLifeAnimNames, ANIM_LIFE_NUM);
            SetPaneAccessor(pPaneCtrl, sLifePaneNames, LPANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(m_fLayoutSortZ);

            const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);

            for (long i = 0; i < nChrNum; i++) {
                bcs::G2D::CPaneObj *pIconObj = m_pPaneAccessor->GetPaneObj(sChrPane[nChrNum - 1][i].nFaceParent);

                if (pIconObj == 0) {
                    continue;
                }

                CChrCore *pChr = CChrMan::GetChrCore(i * 2);

                if (pChr == 0) {
                    continue;
                }

                // The face picture, in the icon pane.
                long nChrId = pChr->GetChrIdS();
                const char *pPictureName = sLifePaneNames[sFacePane[i]];
                bcs::G2D::CPaneObj *pPictureObj = 0;
                const _GXTexObj *pTexObj = pChr->GetCockpitIconTexObj();

                if (pTexObj != 0) {
                    bcs::G2D::CTexMap texMap(*pTexObj);
                    nw4r::lyt::Pane *pPicturePane = bcs::G2D::CreatePicturePane(pPictureName, texMap);

                    if (pPicturePane != 0) {
                        pPictureObj = bcs::G2D::CreatePaneObj(pPaneCtrl, pPicturePane);
                    }
                }

                if (pPictureObj != 0) {
                    pPaneCtrl->AppendPaneObj(pPictureObj, *pIconObj);
                    pPictureObj->GetPane()->SetSize(pIconObj->GetPane()->GetSize());
                    pPictureObj->GetPane()->SetScale(menu::GetCockpitFaceIconScale(nChrId));
                    pPictureObj->GetPane()->SetVisible(true);
                }
            }
        }
        m_nStep = 1;
        m_nState = 1;
    }
    return 0;
}

void msw::CCockpitVolleyLifeGauge::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

long msw::CCockpitVolleyLifeGauge::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return 0;
}

int msw::CCockpitVolleyLifeGauge::ScoreOut()
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sGaugePane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_LIFE_GAUGE_OUT, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    return 0;
}

int msw::CCockpitVolleyLifeGauge::ScoreIn()
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sGaugePane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_LIFE_GAUGE_IN, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    return 0;
}

int msw::CCockpitVolleyLifeGauge::ScoreOutWait()
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sGaugePane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_LIFE_GAUGE_OUT_WAIT, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    return 0;
}

int msw::CCockpitVolleyLifeGauge::ScoreInWait()
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sGaugePane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_LIFE_GAUGE_IN_WAIT, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    return 0;
}

void msw::CCockpitVolleyLifeGauge::SetLifeValue(long, long nNo, long nLife)
{
    if (nNo >= m_nChrNum) {
        return;
    }

    // Six marks per bar.
    long nBar = (nLife - 1) / LIFE_MARK_NUM;

    if (nBar < 0) {
        nBar = 0;
    }
    if (nBar >= 2) {
        nBar = 2;
    }

    long nRest = nLife - nBar * LIFE_MARK_NUM;

    if (nRest < 0) {
        nRest = 0;
    }

    long nPrevBar = nBar - 1;

    for (long i = 0; i < LIFE_MARK_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sLifePane[0].nMark[i]);

        if (pObj != 0) {
            long nAnim;

            if (i < nRest) {
                nAnim = sLifeMarkAnim[nBar][0];
            } else if (nBar > 0) {
                nAnim = sLifeMarkAnim[nPrevBar][0];
            } else {
                nAnim = sLifeMarkAnim[nBar][2];
            }
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
        }
    }

    long nCount = 0;

    if (nLife > 0) {
        nCount = nLife;
    }
    SetCountText(sLifePane[0].nNum, nCount);
}

void msw::CCockpitVolleyLifeGauge::SetCountText(long nPane, long nValue)
{
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

    if (pObj == 0) {
        return;
    }

    if (nValue >= 0) {
        bcs::G2D::CTextBoxDL *pTextBox = nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pObj->GetPane());

        if (pTextBox == 0) {
            return;
        }

        // The digits and the terminator.
        long nLen = 2;

        for (long n = nValue; n >= 10; n /= 10) {
            nLen++;
        }

        wchar_t *pBuf = (wchar_t *)__alloca(nLen * sizeof(wchar_t));

        swprintf(pBuf, nLen, L"%d", nValue);
        pTextBox->AllocStringBuffer(nLen);
        pTextBox->SetString(pBuf, 0);
        pTextBox->SetVisible(true);
    } else {
        pObj->GetPane()->SetVisible(false);
    }
}

// Nothing in VolleyF calls this; the module link removed it, its constant
// is still in .rodata. The body is a guess.
int msw::CCockpitVolleyLifeGauge::IsEndGaugeLineAnim(long nAnim)
{
    static const long teamNo = 0;

    return m_pPaneAccessor->IsAnimEnd(sGaugePane[teamNo], nAnim);
}
