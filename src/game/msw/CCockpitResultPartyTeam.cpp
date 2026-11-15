// msw::CCockpitResultPartyTeam: the party game's team (co-op) result
// screen: the team total, each player's score with face icon and "1P".."4P"
// or "COM" label, three counts, the high score line and a new record mark.
// CCockpit::GetResultPartyTeamParameter fills it.
#include "game/msw/CCockpitResultPartyTeam.hpp"

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
        "prty_result_coop.brlyt",
        0,
    };

    // Animations of the whole layout.
    enum {
        ANIM_IN,
        ANIM_LOOP,
        ANIM_OUT,
        ANIM_NEW_RECORD,
        ANIM_NUM
    };

    const char *s_pAnimName[ANIM_NUM] = {
        "prty_result_coop_all_ctl_00.brlan",
        "prty_result_coop_all_ctl_01.brlan",
        "prty_result_coop_all_ctl_02.brlan",
        "prty_result_coop_new_record.brlan",
    };

    // Panes, by index into s_pPaneName.
    enum {
        PANE_TITLE_G_W,
        PANE_T_TITLE,
        PANE_T_TITLE_B,
        PANE_T_SUB,
        PANE_T_SCORE_COUNT,
        PANE_NEW_RECORD,
        PANE_1ST_NULL,  // to PANE_4TH_NULL
        PANE_2ND_NULL,
        PANE_3RD_NULL,
        PANE_4TH_NULL,
        PANE_T_SCORE_00, // to _03: the players' scores
        PANE_T_SCORE_01,
        PANE_T_SCORE_02,
        PANE_T_SCORE_03,
        PANE_T_SCORE_05, // to _07: the counts
        PANE_T_SCORE_06,
        PANE_T_SCORE_07,
        PANE_T_RECIEVE_COUNT,
        PANE_T_HIGH_SCORE,
        PANE_ICON_00,
        PANE_ICON_01,
        PANE_ICON_03,
        PANE_ICON_02,
        PANE_CNT_BG_00,
        PANE_CNT_BG_01,
        PANE_CNT_BG_02,
        PANE_ICON_DAMMY,
        PANE_1P_S,
        PANE_2P_S,
        PANE_3P_S,
        PANE_4P_S,
        PANE_COM_NULL_00,
        PANE_COM_NULL_01,
        PANE_COM_NULL_02,
        PANE_COM_NULL_03,
        PANE_FC1P, // to PANE_FC4P: names of the face pictures made in the icons
        PANE_FC2P,
        PANE_FC3P,
        PANE_FC4P,
        PANE_NUM
    };

    const char *s_pPaneName[PANE_NUM + 1] = {
        "Title_g_w",
        "t_title",
        "t_title_B",
        "t_sub",
        "t_score_count",
        "new_record",
        "1st_null",
        "2nd_null",
        "3rd_null",
        "4th_null",
        "t_score_00",
        "t_score_01",
        "t_score_02",
        "t_score_03",
        "t_score_05",
        "t_score_06",
        "t_score_07",
        "t_recieve_count",
        "t_high_score",
        "icon_00",
        "icon_01",
        "icon_03",
        "icon_02",
        "cnt_bg_00",
        "cnt_bg_01",
        "cnt_bg_02",
        "icon_dammy",
        "1p_s",
        "2p_s",
        "3p_s",
        "4p_s",
        "com_null_00",
        "com_null_01",
        "com_null_02",
        "com_null_03",
        "fc1p",
        "fc2p",
        "fc3p",
        "fc4p",
        0,
    };

    // Each player's group and score text box.
    const unsigned char s_PlayerScorePane[4][2] = {
        { PANE_1ST_NULL, PANE_T_SCORE_00 },
        { PANE_2ND_NULL, PANE_T_SCORE_01 },
        { PANE_3RD_NULL, PANE_T_SCORE_02 },
        { PANE_4TH_NULL, PANE_T_SCORE_03 },
    };

    // Each player's icon and the name of the face picture made in it.
    const unsigned char s_PlayerIconPane[4][2] = {
        { PANE_ICON_00, PANE_FC1P },
        { PANE_ICON_01, PANE_FC2P },
        { PANE_ICON_03, PANE_FC3P },
        { PANE_ICON_02, PANE_FC4P },
    };

    // Each count's background and text box.
    const unsigned char s_CountPane[3][2] = {
        { PANE_CNT_BG_00, PANE_T_SCORE_05 },
        { PANE_CNT_BG_01, PANE_T_SCORE_06 },
        { PANE_CNT_BG_02, PANE_T_SCORE_07 },
    };

    // By controlling player: the pad label and the COM label.
    const unsigned char s_PlayerLabelPane[4][2] = {
        { PANE_1P_S, PANE_COM_NULL_00 },
        { PANE_2P_S, PANE_COM_NULL_01 },
        { PANE_3P_S, PANE_COM_NULL_02 },
        { PANE_4P_S, PANE_COM_NULL_03 },
    };

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

msw::CCockpitResultPartyTeam::CCockpitResultPartyTeam(long nId, long nType, float fSortZ)
    : CCockpitLayoutBase(nId, fSortZ)
    , CCockpitFrameOutBase(1.0f)
    , m_nType(nType)
{
    m_nStep = 1;
    m_nState = 0;
    m_nPlayStyle = 0;
    m_nTotalScoreMax = 0;
    m_nCountMax = 0;
    m_nPlayerScoreMax = 0;
}

msw::CCockpitResultPartyTeam::~CCockpitResultPartyTeam()
{
    Remove();
}

void msw::CCockpitResultPartyTeam::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_nStep == 1) {
        m_nState = STATE_IN;
        m_nStep = 2;
    }
    StateControl();
}

void msw::CCockpitResultPartyTeam::StateControl()
{
    switch (m_nState) {
        case STATE_IN: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl == 0) {
                m_Unk08 = 2;
                break;
            }
            pPaneCtrl->PlayAnim(ANIM_IN, m_fUnk04, 0.0f, 1, 1);
            m_nState = STATE_IN_WAIT;
            break;
        }

        case STATE_IN_WAIT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_TITLE_G_W, ANIM_IN)) {
                GetLytObj()->PlayAnim(ANIM_LOOP, m_fUnk04, 0.0f, 1, 1);
                m_nState = STATE_SHOW;
            }
            break;

        case STATE_OUT:
            if (IsWaitFrameOut() == 1) {
                GetLytObj()->PlayAnim(ANIM_OUT, m_fUnk04, 0.0f, 1, 1);
                m_nState = STATE_OUT_WAIT;
            }
            break;

        case STATE_OUT_WAIT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_TITLE_G_W, ANIM_OUT)) {
                // (meaning guessed) tells the owner the page is closed.
                m_Unk08 = 2;
            }
            break;
    }
}

int msw::CCockpitResultPartyTeam::Set()
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

    m_nTotalScoreMax = GetTextBoxMaxValue(m_pPaneAccessor, PANE_T_SCORE_COUNT);
    m_nCountMax = GetTextBoxMaxValue(m_pPaneAccessor, PANE_T_SCORE_05);
    m_nPlayerScoreMax = GetTextBoxMaxValue(m_pPaneAccessor, PANE_T_SCORE_00);

    SetTextBoxFromMessage(pPaneCtrl);
    m_nStep = 1;
    m_nState = STATE_IN;
    return 0;
}

void msw::CCockpitResultPartyTeam::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

long msw::CCockpitResultPartyTeam::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_OUT: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl != 0) {
                pPaneCtrl->PlayAnim(ANIM_OUT, 1.0f, 0.0f, 1, 1);
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

void msw::CCockpitResultPartyTeam::SetSubTitleText(const wchar_t *pText)
{
    if (pText != 0) {
        m_pPaneAccessor->SetTextBoxString(PANE_T_SUB, pText, wcslen(pText));
    }
}

void msw::CCockpitResultPartyTeam::SetRankText(const wchar_t *pText)
{
    if (pText != 0) {
        m_pPaneAccessor->SetTextBoxString(PANE_T_HIGH_SCORE, pText, wcslen(pText));
    }
}

void msw::CCockpitResultPartyTeam::SetTotalScore(long nScore)
{
    if (nScore >= 0) {
        m_pPaneAccessor->SetTextBoxString(PANE_T_SCORE_COUNT, nScore > m_nTotalScoreMax ? m_nTotalScoreMax : nScore, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_T_SCORE_COUNT, 1);
    } else {
        m_pPaneAccessor->SetPaneDisp(PANE_T_SCORE_COUNT, 0);
    }
}

void msw::CCockpitResultPartyTeam::SetPlayerScore(long nPlayer, long nScore)
{
    long nGroupPane = s_PlayerScorePane[nPlayer][0];

    if (nScore >= 0) {
        m_pPaneAccessor->SetPaneDisp(nGroupPane, 1);
        m_pPaneAccessor->SetTextBoxString(s_PlayerScorePane[nPlayer][1], nScore > m_nPlayerScoreMax ? m_nPlayerScoreMax : nScore, 0);
    } else {
        m_pPaneAccessor->SetPaneDisp(nGroupPane, 0);
    }
}

long msw::CCockpitResultPartyTeam::SetPlayerIcon(long nPlayer, long nChr)
{
    CChrCore *pChr = CChrMan::GetChrCore(nChr);

    if (pChr == 0) {
        return -10;
    }

    long nCtrlPlayer = pChr->GetCtrlPlayerNo();

    const unsigned char *pIconPane = s_PlayerIconPane[nPlayer];
    nw4r::lyt::Pane *pIcon = m_pPaneAccessor->GetPane(pIconPane[0]);

    if (pIcon == 0) {
        return -1;
    }

    nw4r::lyt::Pane *pSrcPane = m_pPaneAccessor->GetPane(PANE_ICON_DAMMY);

    if (pSrcPane == 0) {
        return -2;
    }

    // A copy of the dummy icon, named after the player, in the player's icon.
    nw4r::lyt::Pane *pFace = bcs::G2D::DuplicatePane(GetLytObj(), pSrcPane, s_pPaneName[pIconPane[1]],
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
    int bPadCtrl = pChr->IsPadCtrl();

    m_pPaneAccessor->SetPaneDisp(s_PlayerLabelPane[nCtrlPlayer][0], bPadCtrl != 0);
    m_pPaneAccessor->SetPaneDisp(s_PlayerLabelPane[nCtrlPlayer][1], bPadCtrl == 0);
    return 0;
}

void msw::CCockpitResultPartyTeam::SetCount(long nCount, long nValue)
{
    long nGroupPane = s_CountPane[nCount][0];

    if (nValue >= 0) {
        m_pPaneAccessor->SetPaneDisp(nGroupPane, 1);
        m_pPaneAccessor->SetTextBoxString(s_CountPane[nCount][1], nValue > m_nCountMax ? m_nCountMax : nValue, 0);
    } else {
        m_pPaneAccessor->SetPaneDisp(nGroupPane, 0);
    }
}

void msw::CCockpitResultPartyTeam::SetNewRecord(int bNewRecord)
{
    if (bNewRecord) {
        m_pPaneAccessor->PlayAnimation(PANE_NEW_RECORD, ANIM_NEW_RECORD, 1.0f, 0.0f, 0, 1);
    } else {
        m_pPaneAccessor->SetPaneDisp(PANE_NEW_RECORD, 0);
    }
}

void msw::CCockpitResultPartyTeam::SetDelayResultButton()
{
    float fDelay = 30.0f;

    cockpit::CDelayResultButtonTask *pTask = new cockpit::CDelayResultButtonTask;
    pTask->SetKind(2000);
    pTask->m_fDelay = fDelay;
    m_DispTaskList.PushBack(pTask);
    CCockpit::ClearResult(-1);
}

void msw::CCockpitResultPartyTeam::Disp()
{
}
