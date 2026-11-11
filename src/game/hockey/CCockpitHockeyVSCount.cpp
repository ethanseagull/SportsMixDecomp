// msw::CCockpitHockeyVSCount: the count board of the hockey party game's
// final (layout HO_crash_fin). It shows the faces of both teams and the
// rounds each team has won, then lights the round just won and shows
// "win", "lose" or "draw".
#include "game/msw/CCockpitHockeyVSCount.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/G2D/CLayoutDL.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameHockey.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace {

    const char *const s_pLayoutName = "HO_crash_fin.brlyt";

    // Animations, by index into s_pAnimName.
    enum {
        ANIM_ST_00,
        ANIM_ST_01,
        ANIM_ST_02,
        ANIM_ST_03,
        ANIM_ICO_00,
        ANIM_ICO_01,
        ANIM_ICO_02,
        ANIM_TE_00,
        ANIM_TE_01,
        ANIM_TE_02,
        ANIM_NUM
    };

    const char *s_pAnimName[ANIM_NUM] = {
        "HO_crash_fin_st_00.brlan",
        "HO_crash_fin_st_01.brlan",
        "HO_crash_fin_st_02.brlan",
        "HO_crash_fin_st_03.brlan",
        "HO_crash_fin_Vis_set_ico_00.brlan",
        "HO_crash_fin_Vis_set_ico_01.brlan",
        "HO_crash_fin_Vis_set_ico_02.brlan",
        "HO_crash_fin_te_00.brlan",
        "HO_crash_fin_te_01.brlan",
        "HO_crash_fin_te_02.brlan",
    };

    // Panes, by index into s_pPaneName.
    enum {
        PANE_FACE_L_R,
        PANE_STAR_L_R,
        PANE_FACE_L_L,
        PANE_STAR_L_L,
        PANE_FACE_L_C,
        PANE_STAR_L_C,
        PANE_FACE_R_R,
        PANE_STAR_R_R,
        PANE_FACE_R_L,
        PANE_STAR_R_L,
        PANE_FACE_R_C,
        PANE_STAR_R_C,
        PANE_PL_L,
        PANE_PL_R,
        PANE_1P,
        PANE_2P,
        PANE_3P,
        PANE_4P,
        PANE_COM,
        PANE_SOZAI_STAR,
        PANE_3ON3,
        PANE_SET,
        PANE_SET_COUNT_L,
        PANE_SET_COUNT_R,
        PANE_SET_ICO_LD01,
        PANE_SET_ICO_LD02,
        PANE_SET_ICO_LD03,
        PANE_SET_ICO_RD01,
        PANE_SET_ICO_RD02,
        PANE_SET_ICO_RD03,
        PANE_L_D01,
        PANE_L_D02,
        PANE_L_D03,
        PANE_R_D01,
        PANE_R_D02,
        PANE_R_D03,
        PANE_FONT_WIN,
        PANE_FONT_LOSE,
        PANE_FONT_DRAW,
        PANE_FC00, // the names of the face pictures made at run time
        PANE_FC01,
        PANE_FC02,
        PANE_FC10,
        PANE_FC11,
        PANE_FC12,
        PANE_ST00, // the names of the star copies
        PANE_ST01,
        PANE_ST02,
        PANE_ST10,
        PANE_ST11,
        PANE_ST12,
        PANE_NUM
    };

    const char *s_pPaneName[PANE_NUM + 1] = {
        "face_33_L_R",
        "star_33_L_R",
        "face_33_L_L",
        "star_33_L_L",
        "face_33_L_C",
        "star_33_L_C",
        "face_33_R_R",
        "star_33_R_R",
        "face_33_R_L",
        "star_33_R_L",
        "face_33_R_C",
        "star_33_R_C",
        "pl_L",
        "pl_R",
        "1P",
        "2P",
        "3P",
        "4P",
        "COM",
        "sozai_star",
        "3on3",
        "set",
        "set_countLNull_D",
        "set_countRNull_D",
        "set_get_icoLD01",
        "set_get_icoLD02",
        "set_get_icoLD03",
        "set_get_icoRD01",
        "set_get_icoRD02",
        "set_get_icoRD03",
        "L_D01",
        "L_D02",
        "L_D03",
        "R_D01",
        "R_D02",
        "R_D03",
        "FONT_WIN",
        "FONT_LOSE",
        "FONT_DRAW",
        "fc00",
        "fc01",
        "fc02",
        "fc10",
        "fc11",
        "fc12",
        "st00",
        "st01",
        "st02",
        "st10",
        "st11",
        "st12",
        0,
    };

    const long TEAM_NUM = 2;
    const long ICON_NUM = 3;  // faces per team
    const long COUNT_NUM = 3; // rounds per team

    // (no known user; in the original's .rodata in front of the tables)
    const long s_Unk4DC = 0;

    // Each face icon: the pane the face goes in, the pane its star goes
    // in, and the names of the face picture and of the star copy.
    const unsigned char s_FaceIconPane[TEAM_NUM][ICON_NUM][4] = {
        {
            { PANE_FACE_L_R, PANE_STAR_L_R, PANE_FC02, PANE_ST02 },
            { PANE_FACE_L_C, PANE_STAR_L_C, PANE_FC00, PANE_ST00 },
            { PANE_FACE_L_L, PANE_STAR_L_L, PANE_FC01, PANE_ST01 },
        },
        {
            { PANE_FACE_R_L, PANE_STAR_R_L, PANE_FC11, PANE_ST11 },
            { PANE_FACE_R_C, PANE_STAR_R_C, PANE_FC10, PANE_ST10 },
            { PANE_FACE_R_R, PANE_STAR_R_R, PANE_FC12, PANE_ST12 },
        },
    };

    // Where each team's player label goes.
    const unsigned char s_PlayerParentPane[TEAM_NUM] = { PANE_PL_L, PANE_PL_R };

    // The player labels: "1P" to "4P", then "COM".
    const unsigned char s_PlayerLabelPane[5] = { PANE_1P, PANE_2P, PANE_3P, PANE_4P, PANE_COM };

    // (no known user)
    const unsigned char s_Unk504[1] = { 3 };

    // Each team's round icons and the panes their effects attach to.
    const unsigned char s_CountIconPane[TEAM_NUM][COUNT_NUM] = {
        { PANE_SET_ICO_LD01, PANE_SET_ICO_LD02, PANE_SET_ICO_LD03 },
        { PANE_SET_ICO_RD01, PANE_SET_ICO_RD02, PANE_SET_ICO_RD03 },
    };
    const unsigned char s_CountEffectPane[TEAM_NUM][COUNT_NUM] = {
        { PANE_L_D01, PANE_L_D02, PANE_L_D03 },
        { PANE_R_D01, PANE_R_D02, PANE_R_D03 },
    };

    // The effect of a round icon lighting up, per team.
    const long s_CountEffectId[TEAM_NUM] = { 0x45E, 0x45F };

    // The result text, by m_nResult.
    enum {
        RESULT_WIN,  // only team 0's count went up
        RESULT_LOSE, // only team 1's count went up
        RESULT_DRAW  // both went up
    };
    const unsigned char s_ResultPane[3] = { PANE_FONT_WIN, PANE_FONT_LOSE, PANE_FONT_DRAW };

    // Events of SendEvent.
    const long EVENT_OUT = 10002;
    const long EVENT_SET_OUT = 10003; // pParam: nonzero to leave

    // The resource file of the cockpit that holds the layout.
    const long RES_FILE = 2;

    // m_nState while m_nStep is 2.
    enum {
        STATE_IN = 1,
        STATE_WAIT_COUNT = 2,
        STATE_WAIT_OUT = 4
    };

    // Stand-in for an unknown user of the two unread tables above
    // (notes/decisions.md, "Unreferenced data kept by dead references").
    inline void UnusedReferences()
    {
        (void)s_Unk4DC;
        (void)s_Unk504;
    }
}

msw::CCockpitHockeyVSCount::CCockpitHockeyVSCount(long nId, float fSortZ)
    : CCockpitLayoutBase(nId, fSortZ)
    , CCockpitFrameOutBase(1.0f)
{
    m_nStep = 1;
    Initialize();
}

void msw::CCockpitHockeyVSCount::Initialize()
{
    static const float delayTime = 30.0f;

    m_nWaitFrameOut = 0;
    m_nNewCount[0] = -1;
    m_nNewCount[1] = -1;
    m_fDelay = delayTime;
}

msw::CCockpitHockeyVSCount::~CCockpitHockeyVSCount()
{
    Remove();
}

int msw::CCockpitHockeyVSCount::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = CCockpit::GetTask();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pArchiveName = pCockpit->GetArchiveFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor == 0) {
        return -2;
    }

    bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

    if (pPaneCtrl != 0) {
        pPaneCtrl->BuildLayout(0, s_pLayoutName);
        pPaneCtrl->BuildAnim(0, s_pAnimName, ANIM_NUM);
        SetPaneAccessor(pPaneCtrl, s_pPaneName, PANE_NUM);
        pPaneCtrl->SetSortZ(m_fSortZ);
        pPaneCtrl->PlayAnim(ANIM_ST_01, 1.0f, 0.0f, 1, 1);
        pPaneCtrl->PlayAnim(ANIM_TE_00, 1.0f, 0.0f, 1, 1);
    }
    m_nStep = 1;
    return 0;
}

void msw::CCockpitHockeyVSCount::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
}

long msw::CCockpitHockeyVSCount::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_OUT:
            m_nWaitFrameOut = 1;
            break;
        case EVENT_SET_OUT:
            SetWaitFrameOut(pParam != 0);
            break;
    }
    return 0;
}

void msw::CCockpitHockeyVSCount::Main()
{
    switch (m_nStep) {
        case 1:
            Set();
            InitParameters();
            if (m_pPaneAccessor->GetPaneObj(PANE_3ON3) == 0) {
                m_Unk08 = 2;
                return;
            }
            m_nStep = 2;
            m_nState = STATE_IN;
            break;
        case 2:
            switch (m_nState) {
                case STATE_IN: {
                    bcs::G2D::CPaneObj *pBoard = m_pPaneAccessor->GetPaneObj(PANE_3ON3);
                    if (pBoard->IsAnimEnd(ANIM_ST_01)) {
                        pBoard->PlayAnimation(ANIM_ST_02, 1.0f, 0.0f, 1, 1, 1);
                        m_pPaneAccessor->PlayAnimation(PANE_SET, ANIM_ST_02, 1.0f, 0.0f, 0, 1);
                        m_nState = STATE_WAIT_COUNT;
                    }
                    break;
                }
                case STATE_WAIT_COUNT: {
                    float fDelay = m_fDelay;
                    fDelay -= bcs::CMainApp::GetAppFrameRateF();
                    if (fDelay <= 0.0f) {
                        m_nState = STATE_WAIT_OUT;
                        SetCountEffect(0, m_nNewCount[0]);
                        SetCountEffect(1, m_nNewCount[1]);
                        m_pPaneAccessor->PlayAnimation(s_ResultPane[m_nResult], ANIM_TE_01, 1.0f, 0.0f, 1, 1);
                    }
                    m_fDelay = fDelay;
                    break;
                }
                case STATE_WAIT_OUT:
                    if (m_nWaitFrameOut != 0) {
                        static const long animId = ANIM_ST_03;
                        m_pPaneAccessor->GetPaneObj(PANE_3ON3)->PlayAnimation(animId, 1.0f, 0.0f, 1, 1, 1);
                        m_pPaneAccessor->PlayAnimation(PANE_SET, animId, 1.0f, 0.0f, 0, 1);
                        m_pPaneAccessor->PlayAnimation(s_ResultPane[m_nResult], animId, 1.0f, 0.0f, 1, 1);
                        m_nStep = 3;
                    }
                    break;
            }
            break;
    }
}

long msw::CCockpitHockeyVSCount::SetFaceIcon(long nTeam, long nIdx, long nChr, int bStar)
{
    const unsigned char *pIcon = s_FaceIconPane[nTeam][nIdx];
    unsigned char nPictureName = pIcon[2];
    nw4r::lyt::Pane *pFace = m_pPaneAccessor->GetPane(pIcon[0]);

    if (pFace == 0) {
        return -1;
    }

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    bcs::G2D::SetPaneSizeFromParent(
        menu::CreateFaceIconPicturePane(nChr, pPaneCtrl, pFace, m_pPaneAccessor->GetPaneName(nPictureName)));

    if (bStar != 0) {
        static const unsigned char srcStarPaneId = PANE_SOZAI_STAR;
        nw4r::lyt::Pane *pSrc = m_pPaneAccessor->GetPane(srcStarPaneId);
        if (pSrc == 0) {
            return -2;
        }
        nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(pIcon[1]);
        if (pParent == 0) {
            return -3;
        }
        nw4r::lyt::Pane *pStar =
            bcs::G2D::DuplicatePane(pPaneCtrl, pSrc, m_pPaneAccessor->GetPaneName(pIcon[3]), s_pLayoutName, 0);
        if (pStar != 0) {
            pParent->AppendChild(pStar);
            const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
            pStar->SetTranslate(zero);
        }
    }
    return 0;
}

long msw::CCockpitHockeyVSCount::AttachPlayerNo(long nTeam0, long nTeam1)
{
    long anPlayer[TEAM_NUM] = { nTeam0, nTeam1 };
    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);

    for (long i = 0; i < TEAM_NUM; i++) {
        long nPlayer = anPlayer[i];
        if (nPlayer >= 0) {
            bcs::G2D::CPaneObj *pLabel = m_pPaneAccessor->GetPaneObj(s_PlayerLabelPane[nPlayer]);
            if (pLabel != 0) {
                nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(s_PlayerParentPane[i]);
                if (pParent != 0) {
                    bcs::G2D::SetPaneParent(pLabel->GetPane(), pParent, 1, 0);
                    pLabel->GetPane()->SetTranslate(zero);
                }
            }
        }
    }
    return 0;
}

void msw::CCockpitHockeyVSCount::InitSetCountPane(long nTeam0, long nTeam1)
{
    if (m_pPaneAccessor != 0) {
        long anCount[TEAM_NUM] = { nTeam0, nTeam1 };
        for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
            const unsigned char *pIcon = s_CountIconPane[nTeam];
            for (long i = 0; i < COUNT_NUM; i++) {
                // static-order swap (notes/decisions.md): the original
                // numbers this `reset` after another one.
                static const int reset = 1;
                static const int recursive = 0;
                m_pPaneAccessor->PlayAnimation(pIcon[i], i < anCount[nTeam] ? ANIM_ICO_01 : ANIM_ICO_00,
                                               1.0f, 0.0f, recursive, reset);
            }
        }
    }
}

void msw::CCockpitHockeyVSCount::SetCountEffect(long nTeam, long nCount)
{
    if ((unsigned long)nCount <= COUNT_NUM - 1) {
        static const int reset = 1;
        static const int recursive = 0;
        static const long animId = ANIM_ICO_02;
        m_pPaneAccessor->PlayAnimation(s_CountIconPane[nTeam][nCount], animId, 1.0f, 0.0f, recursive, reset);
        bcs::G2D::CPaneObj *pPaneObj = m_pPaneAccessor->GetPaneObj(s_CountEffectPane[nTeam][nCount]);
        if (pPaneObj != 0) {
            CEffectCtrl::CreateAttachEffect(s_CountEffectId[nTeam], pPaneObj, 0, -1, 0, 0, 0, 4, 0, 0);
        }
    }
}

void msw::CCockpitHockeyVSCount::InitParameters()
{
    CGameHockey *pGame = static_cast<CGameHockey *>(CGame::m_pInstance);
    long nPlayChr0 = pGame->GetKSMissionPlayChrIdx(0);
    pGame->GetKSMissionWinTeamNo();
    long nPlayChr1 = pGame->GetKSMissionPlayChrIdx(1);

    // The characters in the order their faces are placed: each team's
    // mission character first.
    long anOrder[8];
    anOrder[0] = 0;
    anOrder[1] = 1;
    anOrder[2] = 2;
    anOrder[3] = 3;
    anOrder[4] = 4;
    anOrder[5] = 5;
    anOrder[6] = 6;
    anOrder[7] = 7;
    if (nPlayChr0 == 4) {
        anOrder[0] = 4;
        anOrder[2] = 2;
        anOrder[4] = 0;
    }
    if (nPlayChr0 == 2) {
        anOrder[0] = 2;
        anOrder[2] = 0;
        anOrder[4] = 4;
    }
    if (nPlayChr0 == 0) {
        anOrder[0] = 0;
        anOrder[2] = 2;
        anOrder[4] = 4;
    }
    if (nPlayChr1 == 5) {
        anOrder[1] = 5;
        anOrder[3] = 3;
        anOrder[5] = 1;
    }
    if (nPlayChr1 == 3) {
        anOrder[1] = 3;
        anOrder[3] = 1;
        anOrder[5] = 5;
    }
    if (nPlayChr1 == 1) {
        anOrder[1] = 1;
        anOrder[3] = 3;
        anOrder[5] = 5;
    }

    long anIcon[TEAM_NUM] = { 0, 0 };
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(anOrder[i]);
        if (pChr != 0 && !pChr->IsDisable()) {
            long nChr = pChr->GetPlayerNo();
            long nTeam = pChr->GetTeamNo();
            SetFaceIcon(nTeam, anIcon[nTeam], nChr, CGame::IsCharEx_StarChar(nChr));
            anIcon[nTeam]++;
            if (nPlayChr0 == pChr->GetPlayerNo()) {
                long nPad = pChr->GetPadNo();
                if (nPad < 0) {
                    nPad = 4;
                }
                AttachPlayerNo(nPad, 4);
            }
        }
    }

    // The rounds won before this one and now.
    long nPrev0 = CGame::GetTeamPeriodScore(0, 0);
    long nPrev1 = CGame::GetTeamPeriodScore(1, 0);
    long nNow0 = CGame::GetTeamPeriodScore(0, 1);
    long nNow1 = CGame::GetTeamPeriodScore(1, 1);

    InitSetCountPane(nPrev0, nPrev1);
    // (the original converts the icon index through a float)
    if (nNow0 >= 1 && nPrev0 != nNow0) {
        m_nNewCount[0] = (long)(float)(nNow0 - 1);
    }
    if (nNow1 >= 1 && nPrev1 != nNow1) {
        m_nNewCount[1] = (long)(float)(nNow1 - 1);
    }

    m_nUnk6C = 0;
    if (nPrev1 != nNow1) {
        if (nPrev0 != nNow0) {
            m_nResult = RESULT_DRAW;
        } else {
            m_nResult = RESULT_LOSE;
        }
    } else {
        m_nResult = RESULT_WIN;
    }
    UnusedReferences();
}
