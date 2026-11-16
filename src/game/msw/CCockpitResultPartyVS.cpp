// msw::CCockpitResultPartyVS: the party game's versus result screen: each
// player's score or time, face icon with "1P".."4P" or "COM" label, place
// mark and crown. CCockpit::GetResultPartyVSParameter fills it. Built like
// CCockpitResultPartyTeam.
#include "game/msw/CCockpitResultPartyVS.hpp"

#include <wchar.h>
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CPictureDL.hpp"
#include "bcs/G2D/CTexMap.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitResult.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "nw4r/lyt/Material.hpp"
#include "nw4r/lyt/Pane.hpp"

// (DOL, game/msw/CChrMan.cpp; no symbol, name not known) The face texture
// of character nChr for the result screens, or null.
extern "C" const _GXTexObj *fn_800EF0DC(long nChr);

namespace {

    const char *s_pLayoutName[] = {
        "prty_result_vs.brlyt",
        0,
    };

    // Animations of the whole layout.
    enum {
        ANIM_ALL_IN,
        ANIM_ALL_OUT,
        ANIM_ALL_STOP,
        ANIM_CROWN_GLOW,
        ANIM_SCORE_BAR_LARGE,
        ANIM_SCORE_BAR_NORMAL,
        ANIM_NUM
    };

    const char *s_pAnimName[ANIM_NUM] = {
        "prty_result_vs_all_in.brlan",
        "prty_result_vs_all_out.brlan",
        "prty_result_vs_all_stop.brlan",
        "prty_result_vs_crown_glow.brlan",
        "prty_result_vs_score_bar_larage.brlan",
        "prty_result_vs_score_bar_normal.brlan",
    };

    // Panes, by index into s_pPaneName.
    enum {
        PANE_TITLE_G_W,
        PANE_T_TITLE,
        PANE_T_TITLE_B,
        PANE_T_SUB,
        PANE_CROWN,
        PANE_CROWN_00, // to PANE_CROWN_02
        PANE_CROWN_01,
        PANE_CROWN_02,
        PANE_1ST_NULL, // to PANE_4TH_NULL: the player lines
        PANE_2ND_NULL,
        PANE_3RD_NULL,
        PANE_4TH_NULL,
        PANE_SCORE_NULL,
        PANE_NO_NULL_04,
        PANE_NO_NULL_05,
        PANE_NO_NULL_06,
        PANE_SCORE_00, // to PANE_SCORE_03
        PANE_SCORE_01,
        PANE_SCORE_02,
        PANE_SCORE_03,
        PANE_TIME_NULL_00,
        PANE_MIN_00,
        PANE_SEC_00,
        PANE_COMMA_00,
        PANE_TIME_NULL_01,
        PANE_MIN_01,
        PANE_SEC_01,
        PANE_COMMA_01,
        PANE_TIME_NULL_02,
        PANE_MIN_02,
        PANE_SEC_02,
        PANE_COMMA_02,
        PANE_TIME_NULL_03,
        PANE_MIN_03,
        PANE_SEC_03,
        PANE_COMMA_03,
        PANE_ICON_00,
        PANE_ICON_01,
        PANE_ICON_02,
        PANE_ICON_03,
        PANE_1ST,
        PANE_C_ST,
        PANE_2ND,
        PANE_CT_ND,
        PANE_3RD,
        PANE_CT_RD,
        PANE_4TH,
        PANE_CT_TH,
        PANE_1ST_G,
        PANE_C_ST_G,
        PANE_ICON_DAMMY,
        PANE_COM_S_00,
        PANE_COM_S_01,
        PANE_COM_S_02,
        PANE_COM_S_03,
        PANE_1P_S,
        PANE_2P_S,
        PANE_3P_S,
        PANE_4P_S,
        PANE_FC1P, // to PANE_FC4P: names of the face pictures made in the icons
        PANE_FC2P,
        PANE_FC3P,
        PANE_FC4P,
        PANE_RK1P0, // to PANE_RK4P1: names of the place marks made per player
        PANE_RK2P0,
        PANE_RK3P0,
        PANE_RK4P0,
        PANE_RK1P1,
        PANE_RK2P1,
        PANE_RK3P1,
        PANE_RK4P1,
        PANE_NUM
    };

    const char *s_pPaneName[PANE_NUM + 1] = {
        "Title_g_w",
        "t_title",
        "t_title_B",
        "t_sub",
        "crown",
        "crown_00",
        "crown_01",
        "crown_02",
        "1st_null",
        "2nd_null",
        "3rd_null",
        "4th_null",
        "score_null",
        "no_null_04",
        "no_null_05",
        "no_null_06",
        "score_00",
        "score_01",
        "score_02",
        "score_03",
        "time_null_00",
        "min_00",
        "sec_00",
        "comma_00",
        "time_null_01",
        "min_01",
        "sec_01",
        "comma_01",
        "time_null_02",
        "min_02",
        "sec_02",
        "comma_02",
        "time_null_03",
        "min_03",
        "sec_03",
        "comma_03",
        "icon_00",
        "icon_01",
        "icon_02",
        "icon_03",
        "1st",
        "c_ST",
        "2nd",
        "ct_ND",
        "3rd",
        "ct_RD",
        "4th",
        "ct_TH",
        "1st_G",
        "c_ST_G",
        "icon_dammy",
        "com_s_00",
        "com_s_01",
        "com_s_02",
        "com_s_03",
        "1p_s",
        "2p_s",
        "3p_s",
        "4p_s",
        "fc1p",
        "fc2p",
        "fc3p",
        "fc4p",
        "rk1p0",
        "rk2p0",
        "rk3p0",
        "rk4p0",
        "rk1p1",
        "rk2p1",
        "rk3p1",
        "rk4p1",
        0,
    };

    // Each player's score group and text box.
    const unsigned char s_PlayerScorePane[4][2] = {
        { PANE_SCORE_NULL, PANE_SCORE_00 },
        { PANE_NO_NULL_04, PANE_SCORE_01 },
        { PANE_NO_NULL_05, PANE_SCORE_02 },
        { PANE_NO_NULL_06, PANE_SCORE_03 },
    };

    // Each player's time group and minutes, seconds and hundredths.
    const unsigned char s_PlayerTimePane[4][4] = {
        { PANE_TIME_NULL_00, PANE_MIN_00, PANE_SEC_00, PANE_COMMA_00 },
        { PANE_TIME_NULL_01, PANE_MIN_01, PANE_SEC_01, PANE_COMMA_01 },
        { PANE_TIME_NULL_02, PANE_MIN_02, PANE_SEC_02, PANE_COMMA_02 },
        { PANE_TIME_NULL_03, PANE_MIN_03, PANE_SEC_03, PANE_COMMA_03 },
    };

    // Each player's icon and the name of the face picture made in it.
    const unsigned char s_PlayerIconPane[4][2] = {
        { PANE_ICON_00, PANE_FC1P },
        { PANE_ICON_01, PANE_FC2P },
        { PANE_ICON_02, PANE_FC3P },
        { PANE_ICON_03, PANE_FC4P },
    };

    const unsigned char s_CrownPane[4] = {
        PANE_CROWN, PANE_CROWN_00, PANE_CROWN_01, PANE_CROWN_02,
    };

    // The player lines.
    const unsigned char s_PlayerLinePane[4] = {
        PANE_1ST_NULL, PANE_2ND_NULL, PANE_3RD_NULL, PANE_4TH_NULL,
    };

    // By player: the COM label and the pad label.
    const unsigned char s_ComLabelPane[4] = {
        PANE_COM_S_00, PANE_COM_S_01, PANE_COM_S_02, PANE_COM_S_03,
    };
    const unsigned char s_PadLabelPane[4] = {
        PANE_1P_S, PANE_2P_S, PANE_3P_S, PANE_4P_S,
    };

    // By player: the names of the copies of the place mark and its suffix.
    const unsigned char s_PlayerRankName[4][2] = {
        { PANE_RK1P0, PANE_RK1P1 },
        { PANE_RK2P0, PANE_RK2P1 },
        { PANE_RK3P0, PANE_RK3P1 },
        { PANE_RK4P0, PANE_RK4P1 },
    };

    // By place (4: first, gold): the mark and its suffix ("st", "nd"...).
    const unsigned char s_RankPane[5][2] = {
        { PANE_1ST, PANE_C_ST },
        { PANE_2ND, PANE_CT_ND },
        { PANE_3RD, PANE_CT_RD },
        { PANE_4TH, PANE_CT_TH },
        { PANE_1ST_G, PANE_C_ST_G },
    };

    // The gold first place.
    const long RANK_GOLD = 4;

    // Events of SendEvent.
    enum {
        EVENT_OUT = 10002,     // play the out animation
        EVENT_WAIT_OUT = 10003 // pParam != 0: the screen may leave
    };

    // m_nState.
    enum {
        STATE_IN = 1,
        STATE_IN_WAIT = 2,
        STATE_SHOW = 3,
        STATE_OUT = 4,
        STATE_OUT_WAIT = 5
    };

    // The resource file of the cockpit that holds the layout, its layer and
    // display list memory area.
    const long RES_FILE = 2;
    const long LAYER = 0;
    const long GDL_MEM_AREA = 1;

    // The largest number text box nPane has room for (9, 99, 999...); 0
    // when it is not a text box.
    long GetTextBoxMaxValue(msw::CMenuPaneAccessor *pPaneAccessor, long nPane)
    {
        bcs::G2D::CPaneObj *pObj = pPaneAccessor->GetPaneObj(nPane);

        if (pObj == 0) {
            return 0;
        }

        bcs::G2D::CTextBoxDL *pTextBox = nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pObj->GetPane());

        if (pTextBox == 0) {
            return 0;
        }

        long nLen = pTextBox->GetStringLength();
        long nMax = 1;
        for (long i = 0; i < nLen; i++) {
            nMax *= 10;
        }
        return nMax - 1;
    }
}

msw::CCockpitResultPartyVS::CCockpitResultPartyVS(long nId, long nType, float fSortZ)
    : CCockpitLayoutBase(nId, fSortZ)
    , CCockpitFrameOutBase(1.0f)
    , m_nType(nType)
{
    m_nStep = 1;
    m_nState = 0;
    m_Unk70 = 0;
    m_nScoreMax = 0;
    m_nMinuteMax = 0;
}

msw::CCockpitResultPartyVS::~CCockpitResultPartyVS()
{
    Remove();
}

void msw::CCockpitResultPartyVS::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_nStep == 1) {
        m_nState = STATE_IN;
        m_nStep = 2;
    }
    StateControl();
}

void msw::CCockpitResultPartyVS::StateControl()
{
    switch (m_nState) {
        case STATE_IN: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl == 0) {
                m_Unk08 = 2;
                break;
            }
            pPaneCtrl->PlayAnim(ANIM_ALL_IN, m_fUnk04, 0.0f, 1, 1);
            m_nState = STATE_IN_WAIT;
            break;
        }

        case STATE_IN_WAIT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_TITLE_G_W, ANIM_ALL_IN)) {
                GetLytObj()->PlayAnim(ANIM_ALL_STOP, m_fUnk04, 0.0f, 1, 1);
                m_nState = STATE_SHOW;
            }
            break;

        case STATE_OUT:
            if (IsWaitFrameOut() == 1) {
                GetLytObj()->PlayAnim(ANIM_ALL_OUT, m_fUnk04, 0.0f, 1, 1);
                m_nState = STATE_OUT_WAIT;
            }
            break;

        case STATE_OUT_WAIT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_TITLE_G_W, ANIM_ALL_OUT)) {
                // (meaning guessed) tells the owner the page is closed.
                m_Unk08 = 2;
            }
            break;
    }
}

int msw::CCockpitResultPartyVS::Set()
{
    CCockpit *pCockpit = CCockpit::GetTask();

    if (pCockpit == 0) {
        return -1;
    }

    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -2;
    }

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

    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(2), pLytCtrl->GetFontResourceName(2));
    pPaneCtrl->BuildLayout(0, s_pLayoutName[0]);
    pPaneCtrl->BuildAnim(0, s_pAnimName, ANIM_NUM);
    pPaneCtrl->SetSortZ(m_fSortZ);
    SetPaneAccessor(pPaneCtrl, s_pPaneName, PANE_NUM);

    m_nScoreMax = GetTextBoxMaxValue(m_pPaneAccessor, PANE_SCORE_00);
    m_nMinuteMax = GetTextBoxMaxValue(m_pPaneAccessor, PANE_MIN_00);

    SetTextBoxFromMessage(pPaneCtrl);

    // Where the player lines start, for SetPlayerLinePosition.
    for (long i = 0; i < 4; i++) {
        nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(s_PlayerLinePane[i]);

        if (pPane != 0) {
            nw4r::math::VEC3 pos = pPane->GetTranslate();
            m_LinePos[i] = pos;
        }
    }

    // The place marks are only templates for SetPlayerRank.
    for (long i = 0; i < 5; i++) {
        m_pPaneAccessor->SetPaneDisp(s_RankPane[i][0], 0);
        m_pPaneAccessor->SetPaneDisp(s_RankPane[i][1], 0);
    }

    m_nStep = 1;
    m_nState = STATE_IN;
    return 0;
}

void msw::CCockpitResultPartyVS::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

long msw::CCockpitResultPartyVS::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_OUT: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl != 0) {
                pPaneCtrl->PlayAnim(ANIM_ALL_OUT, 1.0f, 0.0f, 1, 1);
                m_nState = STATE_OUT_WAIT;
            }
            break;
        }
        case EVENT_WAIT_OUT:
            SetWaitFrameOut(pParam != 0);
            break;
    }
    return 0;
}

void msw::CCockpitResultPartyVS::SetSubTitleText(const wchar_t *pText)
{
    if (pText != 0) {
        m_pPaneAccessor->SetTextBoxString(PANE_T_SUB, pText, wcslen(pText));
    }
}

void msw::CCockpitResultPartyVS::SetPlayerScore(long nPlayer, long nScore)
{
    long nGroupPane = s_PlayerScorePane[nPlayer][0];

    if (nScore >= 0) {
        m_pPaneAccessor->SetPaneDisp(nGroupPane, 1);
        m_pPaneAccessor->SetTextBoxString(s_PlayerScorePane[nPlayer][1], nScore > m_nScoreMax ? m_nScoreMax : nScore, 0);
    } else {
        m_pPaneAccessor->SetPaneDisp(nGroupPane, 0);
    }
}

void msw::CCockpitResultPartyVS::SetPlayerTime(long nPlayer, float fFrame)
{
    long nFrame = fFrame;
    long nHundredth = nFrame % 60 * 100 / 60;
    long nSecond = nFrame / 60 % 60;
    long nMinute = nFrame / 3600;
    const unsigned char *pPane = s_PlayerTimePane[nPlayer];
    long nGroupPane = pPane[0];

    if (fFrame >= 0.0f) {
        m_pPaneAccessor->SetPaneDisp(nGroupPane, 1);
        m_pPaneAccessor->SetTextBoxString(pPane[1], nMinute > m_nMinuteMax ? m_nMinuteMax : nMinute, 0);
        m_pPaneAccessor->SetTextBoxString(pPane[2], nSecond, 2);
        m_pPaneAccessor->SetTextBoxString(pPane[3], nHundredth, 2);
    } else {
        m_pPaneAccessor->SetPaneDisp(nGroupPane, 0);
    }
}

long msw::CCockpitResultPartyVS::SetPlayerIcon(long nPlayer, long nChr)
{
    nw4r::lyt::Pane *pIcon = m_pPaneAccessor->GetPane(s_PlayerIconPane[nPlayer][0]);

    if (pIcon == 0) {
        return -1;
    }

    CChrCore *pChr = CChrMan::GetChrCore(nChr);

    if (pChr == 0) {
        return -10;
    }

    nw4r::lyt::Pane *pSrcPane = m_pPaneAccessor->GetPane(PANE_ICON_DAMMY);

    if (pSrcPane == 0) {
        return -2;
    }

    // A copy of the dummy icon, named after the player, in the player's icon.
    nw4r::lyt::Pane *pFace = bcs::G2D::DuplicatePane(GetLytObj(), pSrcPane, s_pPaneName[PANE_FC1P + nPlayer],
                                                     s_pLayoutName[0], 0);

    if (pFace == 0) {
        return -4;
    }
    pIcon->AppendChild(pFace);

    // It shows the character's face.
    bcs::G2D::CPictureDL *pPicture = nw4r::ut::DynamicCast<bcs::G2D::CPictureDL *>(pFace);

    if (pPicture != 0) {
        const _GXTexObj *pTexObj = fn_800EF0DC(nChr);

        if (pTexObj != 0 && pPicture->GetMaterial()->GetTextureNum() != 0) {
            bcs::G2D::CTexMap texMap(*pTexObj);

            pPicture->GetMaterial()->SetTexture(0, texMap);
        }
    }

    pFace->SetBasePositionV(2);
    pFace->SetVisible(true);
    pFace->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, 0.0f));

    // The pad label of a human player, else "COM".
    if (pChr->IsPadCtrl()) {
        m_pPaneAccessor->SetPaneDisp(s_PadLabelPane[nPlayer], 1);
        m_pPaneAccessor->SetPaneDisp(s_ComLabelPane[nPlayer], 0);
    } else {
        m_pPaneAccessor->SetPaneDisp(s_PadLabelPane[nPlayer], 0);
        m_pPaneAccessor->SetPaneDisp(s_ComLabelPane[nPlayer], 1);
    }
    return 0;
}

void msw::CCockpitResultPartyVS::SetCrown(long nCrown, int bDisp)
{
    long nPane = s_CrownPane[nCrown];

    if (bDisp) {
        m_pPaneAccessor->PlayAnimation(nPane, ANIM_CROWN_GLOW, 1.0f, 0.0f, 1, 1);
    } else {
        m_pPaneAccessor->StopAnimation(nPane, ANIM_CROWN_GLOW, 1);
        m_pPaneAccessor->SetPaneDisp(nPane, 0);
    }
}

void msw::CCockpitResultPartyVS::SetPlayerRank(long nPlayer, long nRank)
{
    // Unused; the original stores these three zeros to its stack frame.
    nw4r::math::VEC3 unused(0.0f, 0.0f, 0.0f);
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    nw4r::lyt::Pane *pMark = m_pPaneAccessor->GetPane(s_RankPane[nRank][0]);
    nw4r::lyt::Pane *pSuffix = m_pPaneAccessor->GetPane(s_RankPane[nRank][1]);

    if (pMark != 0 && pSuffix != 0) {
        // Copies of the place mark and its suffix, named after the player.
        nw4r::lyt::Pane *pNewMark = bcs::G2D::DuplicatePane(pPaneCtrl, pMark, s_pPaneName[s_PlayerRankName[nPlayer][0]],
                                                            s_pLayoutName[0], 0);
        nw4r::lyt::Pane *pNewSuffix = bcs::G2D::DuplicatePane(pPaneCtrl, pSuffix, s_pPaneName[s_PlayerRankName[nPlayer][1]],
                                                              s_pLayoutName[0], 0);

        if (pNewMark != 0 && pNewSuffix != 0) {
            // Put where the mark of the player's line is.
            nw4r::lyt::Pane *pPlace = m_pPaneAccessor->GetPane(s_RankPane[nPlayer][0]);

            if (pPlace != 0) {
                bcs::G2D::SetPaneParent(pNewMark, pPlace->GetParent(), 1, 0);
                bcs::G2D::SetPaneParent(pNewSuffix, pNewMark, 1, 0);
                pNewMark->SetTranslate(pPlace->GetTranslate());
            }
        }
    }

    m_pPaneAccessor->PlayAnimation(s_PlayerLinePane[nPlayer],
                                   nRank == RANK_GOLD ? ANIM_SCORE_BAR_LARGE : ANIM_SCORE_BAR_NORMAL, 1.0f, 0.0f,
                                   1, 0);
}

long msw::CCockpitResultPartyVS::SetPlayerLinePosition(long nPlayer, long nLine)
{
    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(s_PlayerLinePane[nPlayer]);

    if (pPane != 0) {
        pPane->SetTranslate(m_LinePos[nLine]);
    }
    return 0;
}

void msw::CCockpitResultPartyVS::SetDelayResultButton()
{
    float fDelay = 30.0f;

    cockpit::CDelayResultButtonTask *pTask = new cockpit::CDelayResultButtonTask;
    pTask->SetKind(2000);
    pTask->m_fDelay = fDelay;
    m_DispTaskList.PushBack(pTask);
    CCockpit::ClearResult(-1);
}

void msw::CCockpitResultPartyVS::Disp()
{
}
