// The status windows of the character select screen.
#include "game/msw/SelectMenu/CCharaStatus.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/G2D/CLayoutBase.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/SelectMenu/CCharSel.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the window animations; converted to float at run
    // time, as in game/menu/CTeam.cpp.
    const long s_WinAnimSpeed[1] = { 2 };

    enum {
        ANIM_VIS_00,
        ANIM_VIS_01,
        ANIM_GAUGE_00,
        ANIM_GAUGE_01,
        ANIM_GAUGE_02,
        ANIM_GAUGE_03,
        ANIM_CHA_PO_00,
        ANIM_CHA_PO_01,
        ANIM_CHA_S_00,
        ANIM_CHA_S_01,
        ANIM_CHA_S_02,
        ANIM_FRAME_BACK_IN,
        ANIM_FRAME_BACK_OUT,
        ANIM_FRAME_IN,
        ANIM_FRAME_LOOP,
        ANIM_FRAME_OUT,
        ANIM_FRAME_OFF,
        ANIM_CO_00,
        ANIM_CO_01,
        ANIM_CO_02,
        ANIM_CO_03,
        ANIM_CO_04,
        ANIM_NUM
    };

    // The panes; the slot panes are a base plus the slot's number (0 to 5).
    enum {
        PANE_STA_NULL,
        PANE_NULL,       // "46_Null" or "23_Null"
        PANE_GAUGE = 2,
        PANE_G = 8,
        PANE_P_BAR = 14,
        PANE_S_BAR = 20,
        PANE_T_BAR = 26,
        PANE_T_CHARA = 32,
        PANE_PLAYER = 38,
        PANE_T_TIP = 44,
        PANE_NUM = 50
    };

    enum {
        CHARA_NUM = 20,
        CHARA_NONE = 0xFF,
        PLAYER_COM = 4
    };

    // The menu object of the character select screen.
    enum {
        OBJ_CHARSEL = 0x45
    };

    // What the character select screen is asked.
    enum {
        EVENT_IS_WIDE = 0x4F
    };

    const char *const s_LayoutName = "me_cha_status.brlyt";

    // Per character, the message numbers of its name in "List_Character",
    // by variant.
    long s_CharaNameMsg[CHARA_NUM][4] = {
        { 0, -1, -1, -1 },
        { 1, -1, -1, -1 },
        { 2, 2, -1, -1 },
        { 3, 3, -1, -1 },
        { 4, 4, 4, 4 },
        { 5, -1, -1, -1 },
        { 6, -1, -1, -1 },
        { 7, -1, -1, -1 },
        { 8, -1, -1, -1 },
        { 9, 9, 9, 9 },
        { 10, -1, -1, -1 },
        { 11, -1, -1, -1 },
        { 12, -1, -1, -1 },
        { 13, -1, -1, -1 },
        { 14, 14, -1, -1 },
        { 15, 15, -1, -1 },
        { 16, 25, 26, -1 },
        { 17, 17, -1, -1 },
        { 18, -1, -1, -1 },
        { 19, -1, -1, -1 },
    };

    const char *s_AnimNames[ANIM_NUM] = {
        "me_cha_status_Vis_00.brlan",
        "me_cha_status_Vis_01.brlan",
        "me_cha_status_Gauge_00.brlan",
        "me_cha_status_Gauge_01.brlan",
        "me_cha_status_Gauge_02.brlan",
        "me_cha_status_Gauge_03.brlan",
        "me_cha_status_Cha_po_00.brlan",
        "me_cha_status_Cha_po_01.brlan",
        "me_cha_status_Cha_s_00.brlan",
        "me_cha_status_Cha_s_01.brlan",
        "me_cha_status_Cha_s_02.brlan",
        "me_cha_status_frame_back_in.brlan",
        "me_cha_status_frame_back_out.brlan",
        "me_cha_status_frame_in.brlan",
        "me_cha_status_frame_loop.brlan",
        "me_cha_status_frame_out.brlan",
        "me_cha_status_frame_off.brlan",
        "me_cha_status_co_00.brlan",
        "me_cha_status_co_01.brlan",
        "me_cha_status_co_02.brlan",
        "me_cha_status_co_03.brlan",
        "me_cha_status_co_04.brlan",
    };

    // The panes with four to six slots.
    const char *s_PaneNames46[PANE_NUM] = {
        "STA_Null", "46_Null",
        "GAUGE_01", "GAUGE_02", "GAUGE_03", "GAUGE_04", "GAUGE_05", "GAUGE_06",
        "G_01", "G_02", "G_03", "G_04", "G_05", "G_06",
        "P_bar01", "P_bar02", "P_bar03", "P_bar04", "P_bar05", "P_bar06",
        "S_bar01", "S_bar02", "S_bar03", "S_bar04", "S_bar05", "S_bar06",
        "T_bar01", "T_bar02", "T_bar03", "T_bar04", "T_bar05", "T_bar06",
        "t_chara01", "t_chara02", "t_chara03", "t_chara04", "t_chara05", "t_chara06",
        "PLAYER01", "PLAYER02", "PLAYER03", "PLAYER04", "PLAYER05", "PLAYER06",
        "t_tip01", "t_tip02", "t_tip03", "t_tip04", "t_tip05", "t_tip06",
    };

    // The panes with up to three slots.
    const char *s_PaneNames23[PANE_NUM] = {
        "STA_Null", "23_Null",
        "GAUGE_07", "GAUGE_08", "GAUGE_09", "dummy", "dummy", "dummy",
        "G_07", "G_08", "G_09", "dummy", "dummy", "dummy",
        "P_bar07", "P_bar08", "P_bar09", "dummy", "dummy", "dummy",
        "S_bar07", "S_bar08", "S_bar09", "dummy", "dummy", "dummy",
        "T_bar07", "T_bar08", "T_bar09", "dummy", "dummy", "dummy",
        "t_chara07", "t_chara08", "t_chara09", "dummy", "dummy", "dummy",
        "PLAYER07", "PLAYER08", "PLAYER09", "dummy", "dummy", "dummy",
        "t_tip07", "t_tip08", "t_tip09", "dummy", "dummy", "dummy",
    };

    // Per character: the three gauges (0 to 7) and the type (0 to 4).
    unsigned char s_CharaParam[22][4] = {
        { 3, 3, 3, 0 },
        { 2, 3, 4, 0 },
        { 1, 3, 5, 1 },
        { 2, 2, 5, 1 },
        { 3, 4, 2, 0 },
        { 5, 2, 2, 2 },
        { 2, 2, 5, 1 },
        { 5, 1, 3, 2 },
        { 1, 5, 3, 3 },
        { 2, 5, 2, 3 },
        { 6, 1, 2, 2 },
        { 3, 5, 1, 3 },
        { 1, 2, 6, 4 },
        { 1, 7, 1, 4 },
        { 3, 4, 3, 0 },
        { 2, 3, 5, 1 },
        { 6, 2, 1, 4 },
        { 2, 2, 6, 4 },
        { 1, 2, 2, 0 },
        { 3, 3, 3, 0 },
    };

    // The player labels copied into the slots: a pane and its copy's name,
    // then the label's background and its copy's name.
    const char *s_LabelNames[10][4] = {
        { "COM", "cpCom", "com_bg_L", "cpComR" },
        { "1P", "cp1P", "P_bg_L", "cp1P_L" },
        { "2P", "cp2P", "P_bg_L", "cp2P_L" },
        { "3P", "cp3P", "P_bg_L", "cp3P_L" },
        { "4P", "cp4P", "P_bg_L", "cp4P_L" },
        { "COM", "cpCom", "com_bg_R", "cpComR" },
        { "1P", "cp1P", "P_bg_R", "cp1P_R" },
        { "2P", "cp2P", "P_bg_R", "cp2P_R" },
        { "3P", "cp3P", "P_bg_R", "cp3P_R" },
        { "4P", "cp4P", "P_bg_R", "cp4P_R" },
    };

    // Size 0x10. One player label: copied from s_LabelNames[m_nLabel]
    // under pane m_nParent; pane m_nPane then plays m_nAnim.
    struct Label {
        long m_nParent; // -1 ends the list
        long m_nLabel;
        long m_nPane;
        long m_nAnim;
    };

    Label s_Label00[6] = {
        { 39, 1, 9, 17 }, { 38, 0, 8, 21 }, { 40, 0, 10, 21 }, { 42, 5, 12, 21 }, { 41, 5, 11, 21 }, { 43, 5, 13, 21 }
    };
    Label s_Label01[6] = {
        { 39, 1, 9, 17 }, { 38, 0, 8, 21 }, { 40, 0, 10, 21 }, { 42, 7, 12, 18 }, { 41, 5, 11, 21 }, { 43, 5, 13, 21 }
    };
    Label s_Label02[6] = {
        { 38, 1, 8, 21 }, { 39, 0, 9, 21 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label03[6] = {
        { 38, 1, 8, 17 }, { 39, 2, 9, 18 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label04[6] = {
        { 38, 1, 8, 17 }, { 39, 0, 9, 21 }, { 41, 5, 11, 21 }, { 40, 5, 10, 21 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label05[6] = {
        { 38, 1, 8, 17 }, { 39, 0, 9, 21 }, { 41, 7, 11, 18 }, { 40, 5, 10, 21 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label06[6] = {
        { 38, 1, 8, 17 }, { 39, 2, 9, 18 }, { 40, 5, 10, 21 }, { 41, 5, 11, 21 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label07[6] = {
        { 38, 1, 8, 17 }, { 39, 2, 9, 18 }, { 40, 5, 10, 21 }, { 41, 8, 11, 19 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label08[6] = {
        { 38, 1, 8, 17 }, { 39, 2, 9, 18 }, { 40, 9, 10, 20 }, { 41, 8, 11, 19 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label09[6] = {
        { 39, 1, 9, 17 }, { 38, 2, 8, 18 }, { 40, 3, 10, 19 }, { 42, 5, 12, 21 }, { 41, 5, 11, 21 }, { 43, 5, 13, 21 }
    };
    Label s_Label10[6] = {
        { 39, 1, 9, 17 }, { 38, 2, 8, 18 }, { 40, 3, 10, 19 }, { 42, 9, 12, 20 }, { 41, 5, 11, 21 }, { 43, 5, 13, 21 }
    };
    Label s_Label11[6] = {
        { 39, 1, 9, 17 }, { 38, 2, 8, 18 }, { 40, 0, 10, 21 }, { 42, 8, 12, 19 }, { 41, 5, 11, 21 }, { 43, 9, 13, 20 }
    };
    Label s_Label12[6] = {
        { 39, 1, 9, 17 }, { 38, 2, 8, 18 }, { 40, 0, 10, 21 }, { 42, 8, 12, 19 }, { 41, 5, 11, 21 }, { 43, 5, 13, 21 }
    };
    Label s_Label13[6] = {
        { 39, 1, 9, 17 }, { 38, 0, 8, 21 }, { 40, 0, 10, 21 }, { 42, 7, 12, 18 }, { 41, 5, 11, 21 }, { 43, 8, 13, 19 }
    };
    Label s_Label14[6] = {
        { 38, 1, 8, 17 }, { 39, 3, 9, 19 }, { 41, 7, 11, 18 }, { 40, 5, 10, 21 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label15[6] = {
        { 38, 1, 8, 17 }, { 39, 0, 9, 21 }, { 40, 8, 10, 19 }, { 41, 7, 11, 18 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label16[6] = {
        { 38, 1, 8, 17 }, { 39, 3, 9, 19 }, { 40, 9, 10, 20 }, { 41, 7, 11, 18 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label17[6] = {
        { 38, 1, 8, 17 }, { 39, 4, 9, 20 }, { 40, 8, 10, 19 }, { 41, 7, 11, 18 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label18[6] = {
        { 39, 1, 9, 17 }, { 38, 2, 8, 18 }, { 40, 0, 10, 21 }, { 42, 5, 12, 21 }, { 41, 5, 11, 21 }, { 43, 5, 13, 21 }
    };
    Label s_Label19[6] = {
        { 39, 1, 9, 17 }, { 38, 0, 8, 21 }, { 40, 0, 10, 21 }, { 42, 7, 12, 18 }, { 41, 9, 11, 20 }, { 43, 8, 13, 19 }
    };
    Label s_Label20[6] = {
        { 39, 1, 9, 17 }, { 38, 3, 8, 19 }, { 40, 0, 10, 21 }, { 42, 7, 12, 18 }, { 41, 5, 11, 21 }, { 43, 5, 13, 21 }
    };
    Label s_Label21[6] = {
        { 39, 1, 9, 17 }, { 38, 2, 8, 18 }, { 40, 4, 10, 20 }, { 42, 8, 12, 19 }, { 41, 5, 11, 21 }, { 43, 5, 13, 21 }
    };
    Label s_Label22[6] = {
        { 39, 1, 9, 17 }, { 38, 3, 8, 19 }, { 40, 4, 10, 20 }, { 42, 7, 12, 18 }, { 41, 5, 11, 21 }, { 43, 5, 13, 21 }
    };

    // The labels by play mode (CSelectMenu::m_nPlayMode).
    Label *s_PlayModeLabel[26] = {
        s_Label00, s_Label01, s_Label02, s_Label03, s_Label04, s_Label05, s_Label06,
        s_Label07, s_Label08, s_Label09, s_Label10, 0, 0, 0,
        s_Label11, s_Label12, s_Label13, s_Label15, s_Label18, s_Label19, s_Label14,
        s_Label20, s_Label16, s_Label17, s_Label21, s_Label22,
    };

    // The labels in modes 1 and 4 (CSelectMenu::m_Unk9B), by player count.
    Label s_Label1P[6] = {
        { 39, 1, 9, 17 }, { 38, 0, 8, 21 }, { 40, 0, 10, 21 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label1PMode[6] = {
        { 38, 1, 8, 17 }, { 39, 0, 9, 21 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label2P[6] = {
        { 38, 1, 8, 17 }, { 39, 2, 9, 18 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_Label3P[6] = {
        { 39, 1, 9, 17 }, { 38, 2, 8, 18 }, { 40, 3, 10, 19 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };

    // The labels in mode 2, by player count.
    Label s_LabelTeam1P[6] = {
        { 38, 1, 8, 17 }, { 39, 0, 9, 21 }, { 40, 5, 10, 21 }, { 41, 5, 11, 21 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_LabelTeam2P[6] = {
        { 38, 1, 8, 17 }, { 39, 2, 9, 18 }, { 40, 5, 10, 21 }, { 41, 5, 11, 21 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_LabelTeam3P[6] = {
        { 38, 1, 8, 17 }, { 39, 2, 9, 18 }, { 40, 8, 10, 19 }, { 41, 5, 11, 21 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };
    Label s_LabelTeam4P[6] = {
        { 38, 1, 8, 17 }, { 39, 2, 9, 18 }, { 40, 8, 10, 19 }, { 41, 9, 11, 20 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }
    };

    // The status slot each character slot is shown in, by play mode; -1
    // for none.
    const long s_StatusSlot[26][6] = {
        { 1, 0, 2, 4, 5, 3 },
        { 1, 0, 2, 4, 5, 3 },
        { 0, 1, -1, -1, -1, -1 },
        { 0, 1, -1, -1, -1, -1 },
        { 0, 1, -1, 3, 2, -1 },
        { 0, 1, -1, 3, 2, -1 },
        { 0, 1, -1, 3, 2, -1 },
        { 0, 1, -1, 3, 2, -1 },
        { 0, 1, -1, 3, 2, -1 },
        { 1, 0, 2, 4, 5, 3 },
        { 1, 0, 2, 4, 5, 3 },
        { -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1 },
        { 1, 0, 2, 4, 5, 3 },
        { 1, 0, 2, 4, 5, 3 },
        { 1, 0, 2, 4, 5, 3 },
        { 0, 1, -1, 3, 2, -1 },
        { 1, 0, 2, 4, 5, 3 },
        { 1, 0, 2, 4, 5, 3 },
        { 0, 1, -1, 3, 2, -1 },
        { 1, 0, 2, 4, 5, 3 },
        { 0, 1, -1, 3, 2, -1 },
        { 0, 1, -1, 3, 2, -1 },
        { 1, 0, 2, 4, 5, 3 },
        { 1, 0, 2, 4, 5, 3 },
    };

    // Runs a gauge to nValue (0 to 5) out of 5; 7 is over the top.
    inline void PlayGaugeAnim(bcs::G2D::CPaneObj *pObj, unsigned char nValue, float fFrameMax)
    {
        float fFrame = fFrameMax * (nValue / 5.0f);
        if (nValue == 7) {
            pObj->PlayAnimation(ANIM_GAUGE_03, fFrame / 10.0f, 0.0f, 1, 1, 1);
        } else {
            pObj->PlayAnimation(ANIM_GAUGE_00, fFrame / 10.0f, 0.0f, 1, 1, 1);
        }
    }

    // The steps of a slot in DispCharaStatus.
    enum {
        DISP_SET,
        DISP_UPDATE,
        DISP_CLEAR
    };
}

msw::SelectMenu::CCharaStatus::CCharaStatus(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk78 = 0;
    Initialize();
}

msw::SelectMenu::CCharaStatus::~CCharaStatus()
{
    Remove();
}

void msw::SelectMenu::CCharaStatus::Main()
{
    if (m_Unk2C == 0) {
        return;
    }

    m_fWait -= bcs::CMainApp::GetAppFrameRateF();
    if (m_fWait > 0.0f) {
        return;
    }
    m_fWait = 0.0f;

    DispCharaStatus();

    switch (m_Unk30) {
        case STATE_SET: {
            CSelectMenu *pSelectMenu = GetSelectMenu();
            Label *pLabel = 0;
            long nMode = pSelectMenu->m_Unk9B;
            long nPlayerNum = pSelectMenu->m_nPlayerNum;
            long nPlayMode = pSelectMenu->m_nPlayMode;

            if (nMode == 1 || nMode == 4) {
                switch (nPlayerNum) {
                    case 1:
                        if (nPlayMode != 0) {
                            pLabel = s_Label1PMode;
                        } else {
                            pLabel = s_Label1P;
                        }
                        break;
                    case 2:
                        pLabel = s_Label2P;
                        break;
                    case 3:
                        pLabel = s_Label3P;
                        break;
                }
            } else if (nMode == 2) {
                switch (nPlayerNum) {
                    case 1:
                        pLabel = s_LabelTeam1P;
                        break;
                    case 2:
                        pLabel = s_LabelTeam2P;
                        break;
                    case 3:
                        pLabel = s_LabelTeam3P;
                        break;
                    case 4:
                        pLabel = s_LabelTeam4P;
                        break;
                }
            } else {
                pLabel = s_PlayModeLabel[nPlayMode];
            }

            for (long i = 0; i < SLOT_NUM; i++) {
                if (pLabel->m_nParent == -1) {
                    continue;
                }

                nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane((unsigned char)pLabel->m_nParent);
                nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
                long nLabel = pLabel->m_nLabel;
                bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
                const char *const *ppNames = s_LabelNames[nLabel];

                bcs::G2D::CPaneObj *pObj =
                    bcs::G2D::DuplicatePaneObj(pPaneCtrl, ppNames[0], ppNames[1], s_LayoutName, 0);
                pPaneCtrl->AppendPaneObj(pObj, (const char *)0);
                nw4r::lyt::Pane *pText = pObj->GetPane();
                pText->SetTranslate(zero);

                pObj = bcs::G2D::DuplicatePaneObj(pPaneCtrl, ppNames[2], ppNames[3], s_LayoutName, 0);
                pPaneCtrl->AppendPaneObj(pObj, (const char *)0);
                nw4r::lyt::Pane *pBg = pObj->GetPane();
                pBg->SetTranslate(zero);

                bcs::G2D::SetPaneParent(pText, pBg, 1, 0);
                bcs::G2D::SetPaneParent(pBg, pParent, 1, 0);

                if (pLabel->m_nAnim > 0) {
                    m_pPaneAccessor->PlayAnimation(pLabel->m_nPane, pLabel->m_nAnim, s_WinAnimSpeed[0], 0.0f, 1, 0);
                }
                pLabel++;
            }

            // static-order swap (notes/decisions.md): the game numbers
            // `recursive` before `reset` but stores it after.
            static const int reset = 1;
            static const int recursive = 0;
            m_pPaneAccessor->PlayAnimation(PANE_STA_NULL, ANIM_FRAME_IN, 1.0f, 0.0f, recursive, reset);
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_STA_NULL, ANIM_FRAME_IN)) {
                GetSelectMenu()->FindObj(OBJ_CHARSEL);
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_CLOSE: {
            // static-order swap (notes/decisions.md): the game numbers
            // `recursive` before `reset` but stores it after.
            static const int reset = 1;
            static const int recursive = 0;
            m_pPaneAccessor->PlayAnimation(PANE_STA_NULL, ANIM_FRAME_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);

            CCharSel *pCharSel = static_cast<CCharSel *>(GetSelectMenu()->FindObj(OBJ_CHARSEL));
            for (long i = 0; i < SLOT_NUM; i++) {
                CSelectMenu::CharaSelectParam *pParam = GetSelectMenu()->GetCharaSelectParam(i);
                if (pParam->m_Unk00 != 0xFF || pParam->m_nPlayerKind != 0xFF) {
                    pCharSel->ClearPadSign(i, pParam->m_nPlayerKind);
                }
            }
            m_Unk30 = STATE_OUT;
            break;
        }
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_STA_NULL, ANIM_FRAME_OUT)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }
}

void msw::SelectMenu::CCharaStatus::Disp()
{
}

inline void msw::SelectMenu::CCharaStatus::ClearSlots(unsigned char nSignChara)
{
    m_nArrangeNum = 0;
    for (long i = 0; i < SLOT_NUM; i++) {
        m_fGaugeTime[i] = 0.0f;
        m_nDispChara[i] = CHARA_NONE;
        m_nDispState[i] = 0;
        m_nDispStep[i] = DISP_SET;
        m_nSignChara[i] = nSignChara;
    }
}

void msw::SelectMenu::CCharaStatus::Initialize()
{
    m_Unk5C = 0;
    m_Unk60 = 0;
    m_Unk64 = 0;
    m_Unk68 = 0;
    m_Unk78 = 0;
    m_Unk7C = 0;
    m_Unk2C = 0;
    // ClearSlots(CHARA_NONE) written out: through the helper the game's
    // Initialize becomes small enough to be inlined into the constructor.
    m_nArrangeNum = 0;
    for (long i = 0; i < SLOT_NUM; i++) {
        m_fGaugeTime[i] = 0.0f;
        m_nDispChara[i] = CHARA_NONE;
        m_nDispState[i] = 0;
        m_nDispStep[i] = DISP_SET;
        m_nSignChara[i] = CHARA_NONE;
    }
}

int msw::SelectMenu::CCharaStatus::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(pSelectMenu->GetLoadArchiveFileId(-1));
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            m_nArrangeNum = GetArrangementCount();
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            pPaneCtrl->BuildAnim(0, s_AnimNames, ANIM_NUM);
            if (m_nArrangeNum < 4) {
                SetPaneAccessor(pPaneCtrl, s_PaneNames23, PANE_NUM);
            } else {
                SetPaneAccessor(pPaneCtrl, s_PaneNames46, PANE_NUM);
            }
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(90.0f);
        }
    }

    // The windows go under one of two panes, depending on the character
    // select screen.
    nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane("STA_Null", 1, 0);
    nw4r::lyt::Pane *pParent;
    if (GetSelectMenu()->SendObjEventTrigger(OBJ_CHARSEL, EVENT_IS_WIDE, 0) == 0) {
        pParent = m_pPaneAccessor->GetPane("STA_2_Null", 1, 0);
    } else {
        pParent = m_pPaneAccessor->GetPane("STA_3_Null", 1, 0);
    }
    bcs::G2D::SetPaneParent(pPane, pParent, 1, 0);
    pPane->SetTranslate(zero);

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_STA_NULL);
    if (m_nArrangeNum < 4) {
        pObj->PlayAnimation(ANIM_VIS_00, 1.0f, 0.0f, 1, 1, 1);
    } else {
        pObj->PlayAnimation(ANIM_VIS_01, 1.0f, 0.0f, 1, 1, 1);
    }

    pObj = m_pPaneAccessor->GetPaneObj(PANE_NULL);
    switch (m_nArrangeNum) {
        case 2:
            pObj->PlayAnimation(ANIM_CHA_PO_00, 1.0f, 0.0f, 1, 1, 1);
            break;
        case 3:
            pObj->PlayAnimation(ANIM_CHA_PO_01, 1.0f, 0.0f, 1, 1, 1);
            break;
        case 4:
            pObj->PlayAnimation(ANIM_CHA_PO_00, 1.0f, 0.0f, 1, 1, 1);
            break;
        case 6:
            pObj->PlayAnimation(ANIM_CHA_PO_01, 1.0f, 0.0f, 1, 1, 1);
            break;
    }

    for (long i = PANE_GAUGE; i <= PANE_GAUGE + SLOT_NUM - 1; i++) {
        pObj = m_pPaneAccessor->GetPaneObj(i);
        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_CHA_S_00, 1.0f, 0.0f, 1, 1, 1);
        }
    }

    pSelectMenu->FindObj(OBJ_CHARSEL);
    ClearSlots(0xFE);
    m_Unk2C = 1;
    m_Unk30 = STATE_SET;
    m_fWait = 60.0f;
    return 0;
}

void msw::SelectMenu::CCharaStatus::Remove()
{
    if (m_Unk2C != 0) {
        DeletePaneAccessor();
        DeleteLytObj();
        DeleteArcFont();
        m_Unk2C = 0;
    }
}

long msw::SelectMenu::CCharaStatus::GetArrangementCount()
{
    long nCount = 0;
    int bNoCom = 0;
    long nMode = GetSelectMenu()->m_Unk9B;
    if (nMode == 1 || nMode == 4) {
        bNoCom = 1;
    }

    for (long i = 0; i < SLOT_NUM; i++) {
        CSelectMenu::CharaSelectParam *pParam = GetSelectMenu()->GetCharaSelectParam(i);
        if (pParam != 0 && pParam->m_nPlayerKind != 0xFF) {
            if (pParam->m_nPlayerKind != PLAYER_COM || !bNoCom) {
                nCount++;
            }
        }
    }
    return nCount;
}

void msw::SelectMenu::CCharaStatus::DispCharaStatus()
{
    CCharSel *pCharSel = static_cast<CCharSel *>(GetSelectMenu()->FindObj(OBJ_CHARSEL));
    const long *pStatusSlot = s_StatusSlot[GetSelectMenu()->m_nPlayMode];

    for (long i = 0; i < SLOT_NUM; i++) {
        CSelectMenu::CharaSelectParam *pParam = GetSelectMenu()->GetCharaSelectParam(i);
        if (pParam->m_Unk00 == 0xFF && pParam->m_nPlayerKind == 0xFF) {
            continue;
        }

        if (pParam->m_nChara >= CHARA_NUM && pParam->m_nChara < CHARA_NONE) {
            continue;
        }
        long nMode = GetSelectMenu()->m_Unk9B;
        if ((nMode == 1 || nMode == 4) && i >= 3) {
            continue;
        }

        // The pad sign shows while the slot has no character.
        if (pParam->m_nChara != m_nSignChara[i]) {
            if (pParam->m_nChara == CHARA_NONE) {
                pCharSel->SetCharaObjPadSign(pParam->m_Unk08, i);
            } else {
                pCharSel->ClearPadSign(i, pParam->m_Unk08);
            }
            m_nSignChara[i] = pParam->m_nChara;
        }

        long nSlot = pStatusSlot[i];
        if (nSlot == -1) {
            continue;
        }

        switch (m_nDispStep[i]) {
            case DISP_SET: {
                if (pParam->m_nCharaSub != 0xFF) {
                    const wchar_t *pName = GetMessageString(
                        "List_Character", s_CharaNameMsg[pParam->m_nChara][pParam->m_nCharaSub]);
                    m_pPaneAccessor->SetTextBoxString(PANE_T_CHARA + nSlot, pName, wcslen(pName));
                }

                if (pParam->m_nChara != CHARA_NONE) {
                    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_GAUGE + nSlot);
                    if (pObj == 0) {
                        break;
                    }
                    pObj->PlayAnimation(ANIM_CHA_S_01, 1.0f, 0.0f, 1, 1, 1);

                    if (pParam->m_nState == 1) {
                        pCharSel->SetCourtCharaObjImplEx(0, 1, 2, i);
                    }
                    if (pParam->m_nState >= 2) {
                        long nMode = GetSelectMenu()->m_Unk9B;
                        if (nMode == 1 || nMode == 4) {
                            pCharSel->SetCourtCharaObjImplEx(0, 2, 0, i);
                        } else if (pParam->m_nChara != 19) {
                            pCharSel->SetCourtCharaObjImplEx(0, 2, 0, i);
                        }
                    }
                    m_nDispState[i] = pParam->m_nState;

                    unsigned long nSub = pParam->m_nCharaSub;
                    if (nSub == 0xFF) {
                        nSub = 0;
                    }
                    const wchar_t *pName = GetMessageString("List_Character", s_CharaNameMsg[pParam->m_nChara][nSub]);
                    m_pPaneAccessor->SetTextBoxString(PANE_T_CHARA + nSlot, pName, wcslen(pName));

                    const wchar_t *pType;
                    switch (s_CharaParam[pParam->m_nChara][3]) {
                        case 0:
                            pType = GetMessageLabelString("ExhibitionMenuMessage", "CHA_03_00");
                            break;
                        case 1:
                            pType = GetMessageLabelString("ExhibitionMenuMessage", "CHA_03_03");
                            break;
                        case 2:
                            pType = GetMessageLabelString("ExhibitionMenuMessage", "CHA_03_01");
                            break;
                        case 3:
                            pType = GetMessageLabelString("ExhibitionMenuMessage", "CHA_03_02");
                            break;
                        case 4:
                            pType = GetMessageLabelString("ExhibitionMenuMessage", "CHA_03_04");
                            break;
                    }
                    m_pPaneAccessor->SetTextBoxString(PANE_T_TIP + nSlot, pType, wcslen(pType));

                    float fFrameMax = GetLytObj()->GetAnimFrameMax(ANIM_GAUGE_00);
                    pObj = m_pPaneAccessor->GetPaneObj(PANE_P_BAR + nSlot);
                    PlayGaugeAnim(pObj, s_CharaParam[pParam->m_nChara][0], fFrameMax);
                    pObj = m_pPaneAccessor->GetPaneObj(PANE_S_BAR + nSlot);
                    PlayGaugeAnim(pObj, s_CharaParam[pParam->m_nChara][1], fFrameMax);
                    pObj = m_pPaneAccessor->GetPaneObj(PANE_T_BAR + nSlot);
                    PlayGaugeAnim(pObj, s_CharaParam[pParam->m_nChara][2], fFrameMax);

                    m_fGaugeTime[i] = fFrameMax;
                    m_nDispChara[i] = pParam->m_nChara;
                    m_nDispState[i] = pParam->m_nState;
                    m_nDispStep[i]++;
                } else {
                    pCharSel->SetCourtCharaObjImplEx(0, -1, 0, i);
                }
                break;
            }
            case DISP_UPDATE:
                if (pParam->m_nCharaSub != 0xFF) {
                    const wchar_t *pName = GetMessageString(
                        "List_Character", s_CharaNameMsg[pParam->m_nChara][pParam->m_nCharaSub]);
                    m_pPaneAccessor->SetTextBoxString(PANE_T_CHARA + nSlot, pName, wcslen(pName));
                }

                if (m_fGaugeTime[i] != 0.0f) {
                    m_fGaugeTime[i] -= bcs::CMainApp::GetAppFrameRateF();
                    if (m_fGaugeTime[i] <= 0.0f) {
                        m_pPaneAccessor->GetPaneObj(PANE_P_BAR + nSlot)->StopAllAnimation(1);
                        m_pPaneAccessor->GetPaneObj(PANE_S_BAR + nSlot)->StopAllAnimation(1);
                        m_pPaneAccessor->GetPaneObj(PANE_T_BAR + nSlot)->StopAllAnimation(1);
                        m_fGaugeTime[i] = 0.0f;
                    }
                }

                if (pParam->m_nState != m_nDispState[i]) {
                    if (pParam->m_nState == 1) {
                        pCharSel->SetCourtCharaObjMotionEx(i, 1, 2);
                        pCharSel->SetCourtCharaObj(pParam->m_Unk0C, 1, 2, pParam->m_Unk00);
                    }
                    if (pParam->m_nState >= 2) {
                        pCharSel->SetCourtCharaObjMotionEx(i, 2, 0);
                    }
                    m_nDispState[i] = pParam->m_nState;
                }

                if (pParam->m_nChara != m_nDispChara[i]) {
                    m_nDispStep[i]++;
                }
                break;
            case DISP_CLEAR: {
                pCharSel->SetCourtCharaObjImplEx(0, -1, 0, i);
                bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_GAUGE + nSlot);
                if (pObj != 0) {
                    pObj->PlayAnimation(ANIM_CHA_S_00, 1.0f, 0.0f, 1, 1, 1);
                }
                m_nDispStep[i] = DISP_SET;
                break;
            }
        }
    }
}

long msw::SelectMenu::CCharaStatus::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    return 0;
}

long msw::SelectMenu::CCharaStatus::StickMoveCheck(long nPad, long nBtnKindId)
{
    return 0;
}

long msw::SelectMenu::CCharaStatus::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CCharaStatus::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharaStatus::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharaStatus::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharaStatus::HitCheck(bcs::FVEC3 *pPos)
{
    return 0;
}

