// msw::CCockpitVolleyRhythmA and msw::CCockpitVolleyRhythmB: the cockpit
// layouts of the volley rhythm minigames. A is the score board (score, hi
// score, target score, combo and bonus, the score of each ball), B the
// receive count and the pop-up points of each player. Both classes are in
// one file: they share the float literals.
//
// B's pop-up code is the one of game/basket/CCockpitBasketHarapeko.cpp with
// two pop-ups per player instead of three.
#include "game/msw/CCockpitVolleyRhythmA.hpp"
#include "game/msw/CCockpitVolleyRhythmB.hpp"

#include <wchar.h>
#include "bcs/CMainApp.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/CDispTaskList.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace {

    // Events of SendEvent.
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // Values of m_nState.
    enum {
        STATE_START = 1,
        STATE_IN = 2,
        STATE_SHOW = 3,
        STATE_END = 4,
        STATE_END_WAIT = 5
    };

    // The resource file of the cockpit that holds both layouts.
    const long RES_FILE = 2;

    // ---- CCockpitVolleyRhythmA ----

    const char *const s_LayoutA = "VO_rhythm_A.brlyt";

    const char *s_AnimA[16] = {
        "VO_rhythm_A_bonus_00.brlan",
        "VO_rhythm_A_bonus_01.brlan",
        "VO_rhythm_A_bonus_02.brlan",
        "VO_rhythm_A_bonus_03.brlan",
        "VO_rhythm_A_comT_00.brlan",
        "VO_rhythm_A_comT_01.brlan",
        "VO_rhythm_A_co_00.brlan",
        "VO_rhythm_A_co_01.brlan",
        "VO_rhythm_A_co_02.brlan",
        "VO_rhythm_A_co_03.brlan",
        "VO_rhythm_A_num_00.brlan",
        "VO_rhythm_A_num_01.brlan",
        "VO_rhythm_A_num_02.brlan",
        "VO_rhythm_A_num_03.brlan",
        "VO_rhythm_A_ball_00.brlan",
        "VO_rhythm_A_Iux_00.brlan",
    };

    // Panes of A, by index into s_PaneNameA.
    enum {
        PANEA_combo_Null,   // 0x00
        PANEA_Sco_Null,     // 0x01
        PANEA_Hi_Sco_Null,  // 0x02
        PANEA_mov_1_Null,   // 0x03
        PANEA_mov_2_Null,   // 0x04
        PANEA_combo_pic,    // 0x05
        PANEA_bonus_Null,   // 0x06
        PANEA_c_k,          // 0x07
        PANEA_hi_sco_t,     // 0x08
        PANEA_sco_t,        // 0x09
        PANEA_sco_t_00,     // 0x0a
        PANEA_com_t,        // 0x0b
        PANEA_num_t_k,      // 0x0c
        PANEA_num_t,        // 0x0d
        PANEA_com_t_2,      // 0x0e, com_t again
        PANEA_Vo_Sco_A,     // 0x0f
        PANEA_Vo_Sco_B,     // 0x10
        PANEA_Vo_Sco_C,     // 0x11
        PANEA_Vo_Sco_D,     // 0x12
        PANEA_sco_A,        // 0x13
        PANEA_sco_B,        // 0x14
        PANEA_sco_C,        // 0x15
        PANEA_sco_D,        // 0x16
        PANEA_Vis_Null,     // 0x17
        PANEA_bo_A,         // 0x18
        PANEA_ico_B_ball,   // 0x19
        PANEA_bo_C,         // 0x1a
        PANEA_bo_D,         // 0x1b
        PANEA_bo_E,         // 0x1c
        PANEA_bo_F,         // 0x1d
        PANEA_ico_A,        // 0x1e
        PANEA_ico_B,        // 0x1f
        PANEA_ico_C,        // 0x20
        PANEA_ico_D,        // 0x21
        PANEA_te_Null,      // 0x22
        PANEA_te_yaji_Null, // 0x23
        PANEA_tr_m_Null,    // 0x24
        PANEA_te_cool,      // 0x25
        PANEA_te_great,     // 0x26
        PANEA_te_excellent, // 0x27
        PANEA_NUM
    };

    const char *s_PaneNameA[PANEA_NUM] = {
        "combo_Null",
        "Sco_Null",
        "Hi_Sco_Null",
        "mov_1_Null",
        "mov_2_Null",
        "combo_pic",
        "bonus_Null",
        "c_k",
        "hi_sco_t",
        "sco_t",
        "sco_t_00",
        "com_t",
        "num_t_k",
        "num_t",
        "com_t",
        "Vo_Sco_A",
        "Vo_Sco_B",
        "Vo_Sco_C",
        "Vo_Sco_D",
        "sco_A",
        "sco_B",
        "sco_C",
        "sco_D",
        "Vis_Null",
        "bo_A",
        "ico_B_ball",
        "bo_C",
        "bo_D",
        "bo_E",
        "bo_F",
        "ico_A",
        "ico_B",
        "ico_C",
        "ico_D",
        "te_Null",
        "te_yaji_Null",
        "tr_m_Null",
        "te_cool",
        "te_great",
        "te_excellent",
    };

    // The panes of each ball (four balls).
    struct BallPane {
        unsigned char nNull;  // animated in and out
        unsigned char nScore; // the score text
        unsigned char nIcon;  // where the ball icon goes
    };

    const BallPane s_BallPane[4] = {
        { PANEA_Vo_Sco_A, PANEA_sco_A, PANEA_ico_A },
        { PANEA_Vo_Sco_B, PANEA_sco_B, PANEA_ico_B },
        { PANEA_Vo_Sco_C, PANEA_sco_C, PANEA_ico_C },
        { PANEA_Vo_Sco_D, PANEA_sco_D, PANEA_ico_D },
    };

    // The ball icons SetBallIcon chooses from.
    const unsigned char s_BallIconPane[6] = {
        PANEA_bo_A, PANEA_ico_B_ball, PANEA_bo_C, PANEA_bo_D, PANEA_bo_E, PANEA_bo_F,
    };

    // The places of the score and hi score.
    const unsigned char s_ScorePosPane[2] = {
        PANEA_mov_1_Null, PANEA_mov_2_Null,
    };

    // The combo telops.
    const unsigned char s_ComboTelopPane[3] = {
        PANEA_te_cool, PANEA_te_great, PANEA_te_excellent,
    };

    // Animations of the score parts.
    enum {
        ANIM_IN = 6,
        ANIM_IN_WAIT = 7,
        ANIM_OUT = 8,
        ANIM_OUT_WAIT = 9
    };

    // Animations of the bonus pane.
    enum {
        ANIM_BONUS_UP = 0,
        ANIM_BONUS_UP_1 = 1,
        ANIM_BONUS_UP_2 = 2,
        ANIM_BONUS_UP_3 = 3
    };

    // The animation that follows nAnim of a score part, or -1.
    inline long GetNextAnim(long nAnim)
    {
        switch (nAnim) {
            case ANIM_IN:
                return ANIM_IN_WAIT;
            case ANIM_OUT:
                return ANIM_OUT_WAIT;
            default:
                return -1;
        }
    }
}

msw::CCockpitVolleyRhythmA::CCockpitVolleyRhythmA(long nId, float fSortZ)
    : CCockpitVolleyLayoutBase(nId)
{
    m_fSortZ = fSortZ;
    m_nStep = 0;
    m_nState = 0;
    m_fBonusSpeed = 1.0f;
    m_nBallFlag = 0;
}

namespace {

    // The destructor stores both vtable pointers before it destroys the
    // base, which the compiler only does when the body calls something
    // (as in game/basket/CCockpitBasketHarapeko.cpp).
    inline void Nothing() {}
}

msw::CCockpitVolleyRhythmA::~CCockpitVolleyRhythmA()
{
    Nothing();
}

int msw::CCockpitVolleyRhythmA::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

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

    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
    pPaneCtrl->BuildLayout(0, s_LayoutA);
    pPaneCtrl->BuildAnim(0, s_AnimA, 16);
    pPaneCtrl->SetSortZ(m_fSortZ);
    SetPaneAccessor(pPaneCtrl, s_PaneNameA, PANEA_NUM);
    SetTextBoxFromMessage(pPaneCtrl);

    // A displayed value above its value counts down to it at once.
    m_Score.fValue = 0.0f;
    m_Score.fDisp = 999999.0f;
    m_HiScore.fValue = 0.0f;
    m_HiScore.fDisp = 999999.0f;
    m_TargetScore.fValue = 0.0f;
    m_TargetScore.fDisp = 999999.0f;
    m_Combo.fValue = 0.0f;
    m_Combo.fDisp = 999999.0f;

    SetBallScore(0, 0);
    SetBallScore(1, 0);
    SetBallScore(2, 0);
    SetBallScore(3, 0);
    SetAnimBallScore(0, ANIM_OUT_WAIT);
    SetAnimBallScore(1, ANIM_OUT_WAIT);
    SetAnimBallScore(2, ANIM_OUT_WAIT);
    SetAnimBallScore(3, ANIM_OUT_WAIT);

    SetBallIcon(2, 0, 0);
    SetBallIcon(2, 1, 0);
    SetBallIcon(2, 2, 0);
    SetBallIcon(2, 3, 0);
    SetBallIcon(2, 4, 0);
    SetBallIcon(2, 5, 0);
    SetScorePosition(0);
    pPaneCtrl->PlayAnim(14, 1.0f, 0.0f, 1, 1);
    SetComboTelop(-1);
    m_nStep = 1;
    m_nState = STATE_START;
    return 0;
}

void msw::CCockpitVolleyRhythmA::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

void msw::CCockpitVolleyRhythmA::Main()
{
    StateControl();
}

void msw::CCockpitVolleyRhythmA::StateControl()
{
    switch (m_nState) {
        case STATE_START:
            SetAnimBonus(ANIM_BONUS_UP_3, 1.0f);
            m_pPaneAccessor->PlayAnimation(PANEA_combo_pic, 4, 1.0f, 0.0f, 0, 1);
            for (long i = 0; i < 4; i++) {
                SetAnimBallScore(i, ANIM_OUT_WAIT);
            }
            m_pPaneAccessor->SetPaneDisp(PANEA_Vis_Null, 1);
            m_nState = STATE_IN;
            break;

        case STATE_IN:
            m_nState = STATE_SHOW;
            break;

        case STATE_SHOW:
            CountUpCheck();
            break;

        case STATE_END:
            m_nState = STATE_END_WAIT;
            break;

        case STATE_END_WAIT:
            // (meaning guessed) tells the owner the layout is done.
            m_Unk08 = 2;
            break;
    }
}

long msw::CCockpitVolleyRhythmA::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return -1;
}

// Shows fNew in pane nPane, but never more than fValue.
inline void msw::CCockpitVolleyRhythmA::SetCount(long nPane, float &rDisp, float fNew, float fValue)
{
    int bBelow = fValue > fNew;

    if (!bBelow) {
        fNew = fValue;
    }
    rDisp = fNew;
    m_pPaneAccessor->SetTextBoxString(nPane, (long)rDisp, 0);
}

void msw::CCockpitVolleyRhythmA::CountUpCheck()
{
    float fRate = bcs::CMainApp::GetAppFrameRateF();

    // The scores count faster the further they are behind.
    float fValue = m_Score.fValue;
    float fDisp = m_Score.fDisp;

    if (fValue > fDisp) {
        float fDiff = fValue - fDisp;
        float fStep;

        if (fDiff < 1.0f) {
            fStep = 1.0f;
        } else {
            fStep = 1.0f + fDiff / 100.0f;
        }
        SetCount(PANEA_sco_t, fDisp, fDisp + fRate * fStep, fValue);
    } else if (fValue < fDisp) {
        SetCount(PANEA_sco_t, fDisp, fValue, fValue);
    }
    m_Score.fDisp = fDisp;

    fValue = m_HiScore.fValue;
    fDisp = m_HiScore.fDisp;

    if (fValue > fDisp) {
        float fDiff = fValue - fDisp;
        float fStep;

        if (fDiff < 1.0f) {
            fStep = 1.0f;
        } else {
            fStep = 1.0f + fDiff / 100.0f;
        }
        SetCount(PANEA_hi_sco_t, fDisp, fDisp + fRate * fStep, fValue);
    } else if (fValue < fDisp) {
        SetCount(PANEA_hi_sco_t, fDisp, fValue, fValue);
    }
    m_HiScore.fDisp = fDisp;

    fValue = m_TargetScore.fValue;
    fDisp = m_TargetScore.fDisp;

    if (fValue > fDisp) {
        float fDiff = fValue - fDisp;
        float fStep;

        if (fDiff < 1.0f) {
            fStep = 1.0f;
        } else {
            fStep = 1.0f + fDiff / 100.0f;
        }
        SetCount(PANEA_sco_t_00, fDisp, fDisp + fRate * fStep, fValue);
    } else if (fValue < fDisp) {
        SetCount(PANEA_sco_t_00, fDisp, fValue, fValue);
    }
    m_TargetScore.fDisp = fDisp;

    // The combo counts one per frame; every tenth combo plays the bonus.
    fValue = m_Combo.fValue;
    fDisp = m_Combo.fDisp;

    // Never used.
    m_pPaneAccessor->GetPaneObj(PANEA_c_k);

    if (fValue > fDisp) {
        fDisp += fRate;
        if (fDisp > fValue) {
            fDisp = fValue;
        }

        long nCombo = (long)fDisp;

        m_pPaneAccessor->SetTextBoxString(PANEA_com_t, nCombo, 0);
        if (nCombo > 0 && nCombo % 10 == 0) {
            SetAnimComboString(11);
            SetAnimBonus(ANIM_BONUS_UP, m_fBonusSpeed);
            m_pPaneAccessor->PlayAnimation(PANEA_combo_pic, 5, 1.0f, 0.0f, 0, 1);
        }
    } else if (fValue <= fDisp) {
        fDisp = fValue;
        m_pPaneAccessor->SetTextBoxString(PANEA_com_t, (long)fDisp, 0);
    }
    m_Combo.fDisp = fDisp;
}

void msw::CCockpitVolleyRhythmA::SetAnimScore(long nAnim)
{
    static const int recursive = 0;
    static const int reset = 1;
    static const long tblLen = 2;

    long nNext;

    switch (nAnim) {
        case ANIM_IN:
            nNext = ANIM_IN_WAIT;
            break;
        case ANIM_OUT:
            nNext = ANIM_OUT_WAIT;
            break;
        default:
            nNext = -1;
            break;
    }

    for (long i = 0; i < tblLen; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(s_ScorePosPane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
            if (nNext >= 0) {
                pObj->PushNextAnimation(nNext, 1.0f, 0.0f, recursive, 1);
            }
        }
    }
}

void msw::CCockpitVolleyRhythmA::SetAnimCombo(long nAnim)
{
    static const int recursive = 0;
    static const int reset = 1;
    static const long tblLen = 1;

    long nNext;

    switch (nAnim) {
        case ANIM_IN:
            nNext = ANIM_IN_WAIT;
            break;
        case ANIM_OUT:
            nNext = ANIM_OUT_WAIT;
            break;
        default:
            nNext = -1;
            break;
    }

    const long tbl[tblLen] = { PANEA_combo_Null };

    for (long i = 0; i < tblLen; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(tbl[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
            if (nNext >= 0) {
                pObj->PushNextAnimation(nNext, 1.0f, 0.0f, recursive, 1);
            }
        }
    }
}

void msw::CCockpitVolleyRhythmA::SetAnimBonus(long nAnim, float fSpeed)
{
    static const int recursive = 0;
    static const int reset = 1;
    static const long pushAnimNum = 3;
    static const long tblLen = 1;

    // The animations that follow, ended by -1.
    long nNext[pushAnimNum];
    float fNextSpeed[pushAnimNum];

    switch (nAnim) {
        case ANIM_BONUS_UP:
            nNext[0] = ANIM_BONUS_UP_1;
            nNext[1] = ANIM_BONUS_UP_2;
            nNext[2] = ANIM_BONUS_UP_3;
            fNextSpeed[0] = 1.0f / fSpeed;
            fNextSpeed[1] = 1.0f;
            fNextSpeed[2] = 1.0f;
            break;
        case ANIM_BONUS_UP_2:
            nNext[0] = ANIM_BONUS_UP_3;
            nNext[1] = -1;
            fNextSpeed[0] = 1.0f;
            break;
        default:
            nNext[0] = -1;
            break;
    }

    const long tbl[tblLen] = { PANEA_bonus_Null };

    for (long i = 0; i < tblLen; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(tbl[i]);

        if (pObj == 0) {
            continue;
        }
        pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
        for (long j = 0; j < pushAnimNum; j++) {
            if (nNext[j] < 0) {
                break;
            }
            pObj->PushNextAnimation(nNext[j], fNextSpeed[j], 0.0f, recursive, 1);
        }
    }
}

inline void msw::CCockpitVolleyRhythmA::SetAnimBallScore(long nBall, long nAnim)
{
    static const int recursive = 0;
    static const int reset = 1;

    long nNext = GetNextAnim(nAnim);
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(s_BallPane[nBall].nNull);

    if (pObj != 0) {
        pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
        if (nNext >= 0) {
            pObj->PushNextAnimation(nNext, 1.0f, 0.0f, recursive, 1);
        }
    }
}

// nAnim is the first of the combo string's animations (11), then 12, 13 and
// 10 follow.
inline void msw::CCockpitVolleyRhythmA::SetAnimComboString(long nAnim)
{
    static const int recursive = 0;
    static const int reset = 1;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANEA_com_t_2);

    if (pObj != 0) {
        pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
        pObj->PushNextAnimation(12, 1.0f, 0.0f, recursive, 1);
        pObj->PushNextAnimation(13, 1.0f, 0.0f, recursive, 1);
        pObj->PushNextAnimation(10, 1.0f, 0.0f, recursive, 1);
    }
}

int msw::CCockpitVolleyRhythmA::AllOut()
{
    ScoreOut();
    CoinOut();
    ComboOut();
    BallScoreOut(0);
    BallScoreOut(1);
    BallScoreOut(2);
    BallScoreOut(3);
    return 0;
}

int msw::CCockpitVolleyRhythmA::AllIn()
{
    ScoreIn();
    CoinIn();
    ComboIn();
    BallScoreIn(0);
    BallScoreIn(1);
    BallScoreIn(2);
    BallScoreIn(3);
    return 0;
}

int msw::CCockpitVolleyRhythmA::AllOutWait()
{
    ScoreOutWait();
    CoinOutWait();
    ComboOutWait();
    BallScoreOutWait(0);
    BallScoreOutWait(1);
    BallScoreOutWait(2);
    BallScoreOutWait(3);
    return 0;
}

int msw::CCockpitVolleyRhythmA::AllInWait()
{
    ScoreInWait();
    CoinInWait();
    ComboInWait();
    BallScoreInWait(0);
    BallScoreInWait(1);
    BallScoreInWait(2);
    BallScoreInWait(3);
    return 0;
}

int msw::CCockpitVolleyRhythmA::ScoreIn()
{
    SetAnimScore(ANIM_IN);
    return 0;
}

int msw::CCockpitVolleyRhythmA::ScoreInWait()
{
    SetAnimScore(ANIM_IN_WAIT);
    return 0;
}

int msw::CCockpitVolleyRhythmA::ScoreOut()
{
    SetAnimScore(ANIM_OUT);
    return 0;
}

int msw::CCockpitVolleyRhythmA::ScoreOutWait()
{
    SetAnimScore(ANIM_OUT_WAIT);
    return 0;
}

int msw::CCockpitVolleyRhythmA::ComboIn()
{
    SetAnimCombo(ANIM_IN);
    return 0;
}

int msw::CCockpitVolleyRhythmA::ComboInWait()
{
    SetAnimCombo(ANIM_IN_WAIT);
    return 0;
}

int msw::CCockpitVolleyRhythmA::ComboOut()
{
    SetAnimCombo(ANIM_OUT);
    return 0;
}

int msw::CCockpitVolleyRhythmA::ComboOutWait()
{
    SetAnimCombo(ANIM_OUT_WAIT);
    return 0;
}

// A ball's score moves only while its icon is shown.
int msw::CCockpitVolleyRhythmA::BallScoreIn(long nBall)
{
    if ((1 << nBall) & m_nBallFlag) {
        SetAnimBallScore(nBall, ANIM_IN);
    }
    return 0;
}

int msw::CCockpitVolleyRhythmA::BallScoreInWait(long nBall)
{
    if ((1 << nBall) & m_nBallFlag) {
        SetAnimBallScore(nBall, ANIM_IN_WAIT);
    }
    return 0;
}

int msw::CCockpitVolleyRhythmA::BallScoreOut(long nBall)
{
    if ((1 << nBall) & m_nBallFlag) {
        SetAnimBallScore(nBall, ANIM_OUT);
    }
    return 0;
}

int msw::CCockpitVolleyRhythmA::BallScoreOutWait(long nBall)
{
    if ((1 << nBall) & m_nBallFlag) {
        SetAnimBallScore(nBall, ANIM_OUT_WAIT);
    }
    return 0;
}

long msw::CCockpitVolleyRhythmA::SetScore(long nScore)
{
    static const long min = 0;
    static const long max = 9999;

    if (nScore < min) {
        nScore = min;
    } else if (nScore > max) {
        nScore = max;
    }
    m_Score.fValue = nScore;
    return nScore;
}

long msw::CCockpitVolleyRhythmA::SetTargetScore(long nScore, int bReset)
{
    static const long min = 0;
    static const long max = 9999;

    if (nScore < min) {
        nScore = min;
    } else if (nScore > max) {
        nScore = max;
    }
    m_TargetScore.fValue = nScore;
    if (bReset) {
        m_TargetScore.fDisp = 99999.0f;
    }
    return nScore;
}

long msw::CCockpitVolleyRhythmA::SetHiScore(long nScore, int bReset)
{
    static const long min = 0;
    static const long max = 9999;

    if (nScore < min) {
        nScore = min;
    } else if (nScore > max) {
        nScore = max;
    }
    m_HiScore.fValue = nScore;
    if (bReset) {
        m_HiScore.fDisp = 99999.0f;
    }
    return nScore;
}

// A negative combo resets the combo.
long msw::CCockpitVolleyRhythmA::SetCombo(long nCombo)
{
    static const long min = 0;
    static const long max = 999;

    if (nCombo < 0) {
        SetAnimBonus(ANIM_BONUS_UP_3, 1.0f);
        m_Combo.fDisp = m_Combo.fValue = 0.0f;
        return (long)m_Combo.fValue;
    }

    if (nCombo < min) {
        nCombo = min;
    } else if (nCombo > max) {
        nCombo = max;
    }
    m_Combo.fValue = nCombo;
    return nCombo;
}

long msw::CCockpitVolleyRhythmA::SetBonus(long nBonus)
{
    static const long min = 0;
    static const long max = 999;

    if (nBonus < min) {
        nBonus = min;
    } else if (nBonus > max) {
        nBonus = max;
    }
    m_pPaneAccessor->SetTextBoxString(PANEA_num_t, nBonus, 0);
    m_pPaneAccessor->SetTextBoxString(PANEA_num_t_k, nBonus, 0);
    return nBonus;
}

long msw::CCockpitVolleyRhythmA::SetBonusDisp(float fSpeed)
{
    if (fSpeed < 1.0f) {
        fSpeed = 1.0f;
    }
    m_fBonusSpeed = fSpeed;
    return 0;
}

long msw::CCockpitVolleyRhythmA::SetBallScore(long nBall, long nScore)
{
    static const long min = -99;
    static const long max = 99;

    if (nScore < min) {
        nScore = min;
    } else if (nScore > max) {
        nScore = max;
    }

    long nPane = s_BallPane[nBall].nScore;
    wchar_t text[5] = L"+xxx";

    if (nScore >= 0) {
        menu::GetNumericText(nScore, L"+%d", text);
    } else {
        menu::GetNumericText(nScore, L"%d", text);
    }
    m_pPaneAccessor->SetTextBoxString(nPane, text, wcslen(text));
    return nScore;
}

void msw::CCockpitVolleyRhythmA::SetBallIcon(long nBall, long nIcon, int bArg)
{
    if (nIcon >= 0 && nIcon < sizeof(s_BallIconPane)) {
        nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(s_BallPane[nBall].nIcon);
        nw4r::lyt::Pane *pIcon = m_pPaneAccessor->GetPane(s_BallIconPane[nIcon]);

        if (pParent != 0) {
            // Hide the icon shown before.
            nw4r::ut::LinkList<nw4r::lyt::Pane, 4> &rList = pParent->GetChildList();

            for (nw4r::ut::LinkList<nw4r::lyt::Pane, 4>::Iterator it = rList.GetBeginIter();
                 it != rList.GetEndIter(); ++it) {
                (*it).SetVisible(false);
            }

            if (pIcon != 0) {
                bcs::G2D::SetPaneParent(pIcon, pParent, 1, 0);
                pIcon->SetVisible(bArg == 1);
                pIcon->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
            }
        }

        if (bArg == 1) {
            m_nBallFlag |= 1 << nBall;
        } else {
            m_nBallFlag &= ~(1 << nBall);
        }
    } else {
        m_nBallFlag &= ~(1 << nBall);
    }
}

// nPos 1: the score alone in the first place; else the hi score in the first
// and the score in the second.
void msw::CCockpitVolleyRhythmA::SetScorePosition(long nPos)
{
    static const long tblLength = 2;

    long nPane[tblLength];

    if (nPos == 1) {
        m_pPaneAccessor->SetPaneDisp(PANEA_Hi_Sco_Null, 0);
        nPane[0] = PANEA_Sco_Null;
        nPane[1] = -1;
    } else {
        nPane[0] = PANEA_Hi_Sco_Null;
        nPane[1] = PANEA_Sco_Null;
    }

    for (long i = 0; i < tblLength; i++) {
        if (nPane[i] < 0) {
            continue;
        }

        nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(nPane[i]);
        nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(s_ScorePosPane[i]);

        if (pPane != 0 && pParent != 0) {
            bcs::G2D::SetPaneParent(pPane, pParent, 1, 0);
            pPane->SetVisible(true);
            pPane->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
        }
    }
}

void msw::CCockpitVolleyRhythmA::SetComboTelop(long nTelop)
{
    static const long tblLen = 3;

    int bShow = 0;

    for (long i = 0; i < tblLen; i++) {
        if (i == nTelop) {
            m_pPaneAccessor->SetPaneDisp(s_ComboTelopPane[i], 1);
            bShow = 1;
        } else {
            m_pPaneAccessor->SetPaneDisp(s_ComboTelopPane[i], 0);
        }
    }

    if (bShow == 1) {
        static const int recursive = 1;
        static const int reset = 1;

        m_pPaneAccessor->SetPaneDisp(PANEA_te_Null, 1);
        m_pPaneAccessor->PlayAnimation(PANEA_te_Null, 15, 1.0f, 0.0f, recursive, reset);
    } else {
        static const int recursive = 1;

        m_pPaneAccessor->SetPaneDisp(PANEA_te_Null, 0);
        m_pPaneAccessor->StopAnimation(PANEA_te_Null, 15, recursive);
    }
}

long msw::CCockpitVolleyRhythmA::CancelMenu()
{
    m_nState = STATE_END;
    return 0;
}

namespace msw {
    namespace {

        // (name guessed; the class has no symbols) A display task that
        // moves a pop-up point's text pane over a point of the court, once.
        // Made by CCockpitVolleyRhythmB::SetPointUpText.
        class CPointUpTextTask : public menu::CDispTaskLink {
            public:
                CPointUpTextTask()
                {
                    m_bEnd = 0;
                }

                virtual ~CPointUpTextTask();
                virtual void StatusCheck(CMenuPaneAccessor *pAccessor);
                virtual void Disp(CMenuPaneAccessor *pAccessor, long nView);
        };

        CPointUpTextTask::~CPointUpTextTask()
        {
        }

        void CPointUpTextTask::StatusCheck(CMenuPaneAccessor *)
        {
        }

        void CPointUpTextTask::Disp(CMenuPaneAccessor *pAccessor, long nView)
        {
            bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);

            if (pObj != 0) {
                bcs::FVEC3 screenPos;

                menu::GetWorldScreenPosition(m_Pos, nView, screenPos);
                // The text pane sits in a null pane; that one is moved.
                pObj->GetPane()->GetParent()->SetTranslate(reinterpret_cast<const nw4r::math::VEC3 &>(screenPos));
            }
            m_bEnd = 1;
        }
    }
}

namespace {

    // ---- CCockpitVolleyRhythmB ----

    // The layout and its animations.
    const char *s_LayoutB = "VO_rhythm_B.brlyt";

    const char *s_AnimB[9] = {
        "VO_rhythm_B_mov_00.brlan",
        "VO_rhythm_B_mov_01.brlan",
        "VO_rhythm_B_mov_02.brlan",
        "VO_rhythm_B_mov_03.brlan",
        "VO_rhythm_B_ca_00.brlan",
        "VO_rhythm_B_ca_01.brlan",
        "VO_rhythm_B_ca_02.brlan",
        "VO_rhythm_B_ca_03.brlan",
        "VO_rhythm_B_ca_04.brlan",
    };

    // Panes of B, by index into s_PaneNameB.
    enum {
        PANEB_P_Null,    // 0x00
        PANEB_P_1,       // 0x01
        PANEB_p1_num,    // 0x02
        PANEB_p2_num,    // 0x03
        PANEB_ca,        // 0x04
        PANEB_ca_t,      // 0x05
        PANEB_F_Null,    // 0x06
        PANEB_f_01,      // 0x07
        PANEB_f_02,      // 0x08
        PANEB_dNulSd00,  // 0x09
        PANEB_dNulSd01,  // 0x0a
        PANEB_dNulSd10,  // 0x0b
        PANEB_dNulSd11,  // 0x0c
        PANEB_dNulSd20,  // 0x0d
        PANEB_dNulSd21,  // 0x0e
        PANEB_dNulSd30,  // 0x0f
        PANEB_dNulSd31,  // 0x10
        PANEB_dTxtSd00,  // 0x11
        PANEB_dTxtSd01,  // 0x12
        PANEB_dTxtSd10,  // 0x13
        PANEB_dTxtSd11,  // 0x14
        PANEB_dTxtSd20,  // 0x15
        PANEB_dTxtSd21,  // 0x16
        PANEB_dTxtSd30,  // 0x17
        PANEB_dTxtSd31,  // 0x18
        PANEB_dNulSd00a, // 0x19
        PANEB_dNulSd01a, // 0x1a
        PANEB_dNulSd10a, // 0x1b
        PANEB_dNulSd11a, // 0x1c
        PANEB_dNulSd20a, // 0x1d
        PANEB_dNulSd21a, // 0x1e
        PANEB_dNulSd30a, // 0x1f
        PANEB_dNulSd31a, // 0x20
        PANEB_dPicSd00a, // 0x21
        PANEB_dPicSd01a, // 0x22
        PANEB_dPicSd10a, // 0x23
        PANEB_dPicSd11a, // 0x24
        PANEB_dPicSd20a, // 0x25
        PANEB_dPicSd21a, // 0x26
        PANEB_dPicSd30a, // 0x27
        PANEB_dPicSd31a, // 0x28
        PANEB_dPicSd00b, // 0x29
        PANEB_dPicSd01b, // 0x2a
        PANEB_dPicSd10b, // 0x2b
        PANEB_dPicSd11b, // 0x2c
        PANEB_dPicSd20b, // 0x2d
        PANEB_dPicSd21b, // 0x2e
        PANEB_dPicSd30b, // 0x2f
        PANEB_dPicSd31b, // 0x30
        PANEB_NUM
    };

    const char *s_PaneNameB[PANEB_NUM] = {
        "P_Null",
        "P_1",
        "p1_num",
        "p2_num",
        "ca",
        "ca_t",
        "F_Null",
        "f_01",
        "f_02",
        "dNulSd00",
        "dNulSd01",
        "dNulSd10",
        "dNulSd11",
        "dNulSd20",
        "dNulSd21",
        "dNulSd30",
        "dNulSd31",
        "dTxtSd00",
        "dTxtSd01",
        "dTxtSd10",
        "dTxtSd11",
        "dTxtSd20",
        "dTxtSd21",
        "dTxtSd30",
        "dTxtSd31",
        "dNulSd00a",
        "dNulSd01a",
        "dNulSd10a",
        "dNulSd11a",
        "dNulSd20a",
        "dNulSd21a",
        "dNulSd30a",
        "dNulSd31a",
        "dPicSd00a",
        "dPicSd01a",
        "dPicSd10a",
        "dPicSd11a",
        "dPicSd20a",
        "dPicSd21a",
        "dPicSd30a",
        "dPicSd31a",
        "dPicSd00b",
        "dPicSd01b",
        "dPicSd10b",
        "dPicSd11b",
        "dPicSd20b",
        "dPicSd21b",
        "dPicSd30b",
        "dPicSd31b",
    };

    // The pop-up panes of each player (two pop-ups each).
    struct PlayerPane {
        unsigned char nCountUpNull[2];
        unsigned char nCountUpText[2];
        unsigned char nCountUpFrame[2];
        unsigned char nCountUpPicA[2];
        unsigned char nCountUpPicB[2];
        unsigned char nPlusAnim;  // pop-up animation of a gain
        unsigned char nMinusAnim; // pop-up animation of a loss
    };

    const PlayerPane s_PlayerPane[4] = {
        {
            { PANEB_dNulSd00, PANEB_dNulSd01 },
            { PANEB_dTxtSd00, PANEB_dTxtSd01 },
            { PANEB_dNulSd00a, PANEB_dNulSd01a },
            { PANEB_dPicSd00a, PANEB_dPicSd01a },
            { PANEB_dPicSd00b, PANEB_dPicSd01b },
            4, 8,
        },
        {
            { PANEB_dNulSd10, PANEB_dNulSd11 },
            { PANEB_dTxtSd10, PANEB_dTxtSd11 },
            { PANEB_dNulSd10a, PANEB_dNulSd11a },
            { PANEB_dPicSd10a, PANEB_dPicSd11a },
            { PANEB_dPicSd10b, PANEB_dPicSd11b },
            5, 8,
        },
        {
            { PANEB_dNulSd20, PANEB_dNulSd21 },
            { PANEB_dTxtSd20, PANEB_dTxtSd21 },
            { PANEB_dNulSd20a, PANEB_dNulSd21a },
            { PANEB_dPicSd20a, PANEB_dPicSd21a },
            { PANEB_dPicSd20b, PANEB_dPicSd21b },
            6, 8,
        },
        {
            { PANEB_dNulSd30, PANEB_dNulSd31 },
            { PANEB_dTxtSd30, PANEB_dTxtSd31 },
            { PANEB_dNulSd30a, PANEB_dNulSd31a },
            { PANEB_dPicSd30a, PANEB_dPicSd31a },
            { PANEB_dPicSd30b, PANEB_dPicSd31b },
            7, 8,
        },
    };

    // Kinds of the display tasks (plus the player number).
    enum {
        TASK_POINT_UP = 2000
    };
}

const unsigned char msw::CCockpitVolleyRhythmB::s_CountUpPaneNum = 2;

msw::CCockpitVolleyRhythmB::CCockpitVolleyRhythmB(long nId, float fSortZ)
    : CCockpitVolleyLayoutBase(nId)
{
    m_fSortZ = fSortZ;
    m_nCountUpIdx[0] = 0;
    m_nCountUpIdx[1] = 0;
    m_nCountUpIdx[2] = 0;
    m_nCountUpIdx[3] = 0;
    m_nPlayerNum = 0;
    m_nStep = 0;
    m_nState = 0;
}

msw::CCockpitVolleyRhythmB::~CCockpitVolleyRhythmB()
{
    Nothing();
}

int msw::CCockpitVolleyRhythmB::Set()
{
    static const long teamChrNum = 4;

    if (m_pPaneAccessor != 0) {
        return 0;
    }

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

    pPaneCtrl->BuildLayout(0, s_LayoutB);
    pPaneCtrl->BuildAnim(0, s_AnimB, 9);
    pPaneCtrl->SetSortZ(m_fSortZ);
    pPaneCtrl->SetVisible(0);
    SetPaneAccessor(pPaneCtrl, s_PaneNameB, PANEB_NUM);
    SetTextBoxFromMessage(pPaneCtrl);
    m_nPlayerNum = teamChrNum;
    CreatePointUp(pPaneCtrl);
    m_nStep = 1;
    m_nState = STATE_START;
    return 0;
}

void msw::CCockpitVolleyRhythmB::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

void msw::CCockpitVolleyRhythmB::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    switch (m_nState) {
        case STATE_START:
            m_nState = STATE_IN;
            break;

        case STATE_IN:
            m_nState = STATE_SHOW;
            break;

        case STATE_END:
            m_nState = STATE_END_WAIT;
            break;

        case STATE_END_WAIT:
            // (meaning guessed) tells the owner the layout is done.
            m_Unk08 = 2;
            break;
    }
}

void msw::CCockpitVolleyRhythmB::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

long msw::CCockpitVolleyRhythmB::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return -1;
}

void msw::CCockpitVolleyRhythmB::SetAnimGauge(long nAnim)
{
    static const int recursive = 0;
    static const int reset = 0;
    static const long tblLen = 1;

    long nNext;

    switch (nAnim) {
        case 0:
            nNext = 1;
            break;
        case 2:
            nNext = 3;
            break;
        default:
            nNext = -1;
            break;
    }

    const long tbl[tblLen] = { PANEB_P_Null };

    for (long i = 0; i < tblLen; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(tbl[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
            if (nNext >= 0) {
                pObj->PushNextAnimation(nNext, 1.0f, 0.0f, recursive, 1);
            }
        }
    }
}

int msw::CCockpitVolleyRhythmB::ScoreIn()
{
    SetAnimGauge(0);
    return 0;
}

int msw::CCockpitVolleyRhythmB::ScoreInWait()
{
    SetAnimGauge(1);
    return 0;
}

int msw::CCockpitVolleyRhythmB::ScoreOut()
{
    SetAnimGauge(2);
    return 0;
}

int msw::CCockpitVolleyRhythmB::ScoreOutWait()
{
    SetAnimGauge(3);
    return 0;
}

// Counts from 100 go to the second text box.
void msw::CCockpitVolleyRhythmB::SetReceiveCount(long nCount)
{
    long nShowPane;
    long nHidePane;

    if (nCount > 99) {
        nShowPane = PANEB_p2_num;
        nHidePane = PANEB_p1_num;
    } else {
        nShowPane = PANEB_p1_num;
        nHidePane = PANEB_p2_num;
    }
    m_pPaneAccessor->SetTextBoxString(nShowPane, nCount, 0);
    m_pPaneAccessor->SetPaneDisp(nShowPane, 1);
    m_pPaneAccessor->SetPaneDisp(nHidePane, 0);
}

long msw::CCockpitVolleyRhythmB::CancelMenu()
{
    m_nState = STATE_END;
    return 0;
}

void msw::CCockpitVolleyRhythmB::CreatePointUp(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    static const int animUse = 1;

    if (pPaneCtrl == 0) {
        return;
    }

    nw4r::lyt::Pane *pNullSrc = m_pPaneAccessor->GetPane(PANEB_ca);
    nw4r::lyt::Pane *pTextSrc = m_pPaneAccessor->GetPane(PANEB_ca_t);
    nw4r::lyt::Pane *pFrameSrc = m_pPaneAccessor->GetPane(PANEB_F_Null);
    nw4r::lyt::Pane *pPicASrc = m_pPaneAccessor->GetPane(PANEB_f_01);
    nw4r::lyt::Pane *pPicBSrc = m_pPaneAccessor->GetPane(PANEB_f_02);

    if (pNullSrc == 0 || pTextSrc == 0 || pFrameSrc == 0 || pPicASrc == 0 || pPicBSrc == 0) {
        return;
    }

    // The pop-ups are placed in screen coordinates: the origin goes to the
    // top left corner.
    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    const int nWidth = bcs::CMainApp::g_Rmode.fbWidth;
    const int nHeight = bcs::CMainApp::g_Rmode.efbHeight;
    const nw4r::math::VEC3 corner(-(0.5f * nWidth), 0.5f * nHeight, 0.0f);

    pNullSrc->SetVisible(true);
    pNullSrc->SetTranslate(corner);
    pNullSrc->SetLocationAdjust(false);
    pTextSrc->SetVisible(false);

    const PlayerPane *pPlayerPane = s_PlayerPane;
    long nPlayerNum = m_nPlayerNum;

    for (long i = 0; i < nPlayerNum; i++, pPlayerPane++) {
        const unsigned char *pTextPane = pPlayerPane->nCountUpText;
        const unsigned char *pFramePane = pPlayerPane->nCountUpFrame;
        const unsigned char *pPicAPane = pPlayerPane->nCountUpPicA;
        const unsigned char *pPicBPane = pPlayerPane->nCountUpPicB;
        const unsigned char *pNullPane = pPlayerPane->nCountUpNull;

        for (long j = 0; j < s_CountUpPaneNum; j++) {
            bcs::G2D::CPaneObj *pNullObj = bcs::G2D::DuplicatePaneObj(
                pPaneCtrl, pNullSrc, s_PaneNameB[pNullPane[j]], s_LayoutB, 0);

            if (pNullObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pNullObj, pNullSrc->GetName());
            pNullObj->GetPane()->SetVisible(false);
            pNullObj->GetPane()->SetTranslate(zero);

            bcs::G2D::CPaneObj *pFrameObj = bcs::G2D::DuplicatePaneObj(
                pPaneCtrl, pFrameSrc, s_PaneNameB[pFramePane[j]], s_LayoutB, animUse);

            if (pFrameObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pFrameObj, *pNullObj);
            pFrameObj->GetPane()->SetVisible(true);

            bcs::G2D::CPaneObj *pObj = bcs::G2D::DuplicatePaneObj(
                pPaneCtrl, pPicASrc, s_PaneNameB[pPicAPane[j]], s_LayoutB, animUse);

            if (pObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pObj, *pFrameObj);
            pObj->GetPane()->SetVisible(true);

            pObj = bcs::G2D::DuplicatePaneObj(pPaneCtrl, pPicBSrc, s_PaneNameB[pPicBPane[j]],
                                              s_LayoutB, animUse);

            if (pObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pObj, *pFrameObj);
            pObj->GetPane()->SetVisible(true);

            pObj = bcs::G2D::DuplicatePaneObj(pPaneCtrl, pTextSrc, s_PaneNameB[pTextPane[j]],
                                              s_LayoutB, animUse);

            if (pObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pObj, *pNullObj);
            pObj->GetPane()->SetVisible(true);
            pObj->GetPane()->SetTranslate(zero);
        }
    }
}

long msw::CCockpitVolleyRhythmB::SetPointUpText(long nPlayer, const bcs::FVEC3 &rPos, long nPoint)
{
    long nIdx = m_nCountUpIdx[nPlayer];
    long nTextPane = s_PlayerPane[nPlayer].nCountUpText[nIdx];
    long nFramePane = s_PlayerPane[nPlayer].nCountUpFrame[nIdx];
    long nShown = nPoint;

    if (nShown >= 1000) {
        nShown = 999;
    } else if (nShown <= -1000) {
        nShown = -999;
    }

    if (nShown >= 0) {
        wchar_t text[5] = L"+xxx";

        menu::GetNumericText(nShown, L"+%d", text);
        m_pPaneAccessor->SetTextBoxString(nTextPane, text, wcslen(text));
    } else {
        wchar_t text[5] = L"-xxx";

        menu::GetNumericText(-nShown, L"-%d", text);
        m_pPaneAccessor->SetTextBoxString(nTextPane, text, wcslen(text));
    }

    CPointUpTextTask *pTask = new CPointUpTextTask;

    pTask->SetPane(nTextPane);
    pTask->SetPos(rPos);
    pTask->SetKind(nPlayer + TASK_POINT_UP);
    m_DispTaskList.PushBack(pTask);

    long nAnim;

    if (nPoint >= 0) {
        nAnim = s_PlayerPane[nPlayer].nPlusAnim;
    } else {
        nAnim = s_PlayerPane[nPlayer].nMinusAnim;
    }

    static const int recursive = 1;
    static const int reset = 1;

    m_pPaneAccessor->PlayAnimation(nTextPane, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(nFramePane, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->SetPaneDisp(s_PlayerPane[nPlayer].nCountUpNull[nIdx], 1);

    nIdx++;
    if (nIdx >= 2) {
        nIdx = 0;
    }
    m_nCountUpIdx[nPlayer] = nIdx;
    return 0;
}

long msw::CCockpitVolleyRhythmB::ClearPointUpText(long nPlayer)
{
    static const int recursive = 1;

    menu::CDispTaskLink *pLink = m_DispTaskList.FindNextKindObj(nPlayer + TASK_POINT_UP, 0);

    if (pLink != 0) {
        m_DispTaskList.RemoveObject(pLink);
    }

    for (long i = 0; i < 2; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(s_PlayerPane[nPlayer].nCountUpNull[i]);

        if (pObj != 0) {
            pObj->StopAllAnimation(recursive);
            pObj->GetPane()->SetVisible(false);
        }
    }
    return 0;
}
