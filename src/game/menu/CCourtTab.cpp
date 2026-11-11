// The court select screen (CCourt, "me_bg_select") and the cup tabs shown
// over it (CCourtTab, "me_bg_select_w"). The two classes share the button
// description and the window animation speed.
#include "game/msw/SelectMenu/CCourt.hpp"
#include "game/msw/SelectMenu/CCourtTab.hpp"

#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/SelectMenu/CPlusMinus.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the windows' opening and closing animations. A
    // table, not a scalar, as in game/menu/CPartySelect.cpp: the original
    // converts the value to float at run time and keeps the word, first in
    // .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // [0] is CCourt's layout, [1] CCourtTab's.
    const char *const s_LayoutNames[2] = {
        "me_bg_select.brlyt",
        "me_bg_select_w.brlyt"
    };

    // CCourt's animations.
    enum {
        ANIM_BTN_00,
        ANIM_BTN_01,
        ANIM_BTN_02,
        ANIM_BTN_03,
        ANIM_BTN_04,
        ANIM_BC_00,       // the game kind's colour, enabled stage
        ANIM_BC_01,
        ANIM_BC_02,
        ANIM_BC_03,
        ANIM_BC_04,       // the game kind's colour, disabled stage
        ANIM_BC_05,
        ANIM_BC_06,
        ANIM_BC_07,
        ANIM_MULTI_W00,
        ANIM_MULTI_W01,   // window in
        ANIM_MULTI_W02,
        ANIM_MULTI_W03,   // window out
        ANIM_MULTI_W04,   // window in, coming back
        ANIM_MULTI_W05,   // window out, going back
        ANIM_CO_00,       // the stage buttons appear
        ANIM_CO_01,
        ANIM_CO_02,
        ANIM_CO_03,
        ANIM_BTN_VIS_00,
        ANIM_BTN_VIS_01,
        ANIM_BTN_VIS_02,
        ANIM_NUM
    };

    const char *s_AnimNames[ANIM_NUM] = {
        "me_bg_select_Btn_00.brlan",
        "me_bg_select_Btn_01.brlan",
        "me_bg_select_Btn_02.brlan",
        "me_bg_select_Btn_03.brlan",
        "me_bg_select_Btn_04.brlan",
        "me_bg_select_BC_00.brlan",
        "me_bg_select_BC_01.brlan",
        "me_bg_select_BC_02.brlan",
        "me_bg_select_BC_03.brlan",
        "me_bg_select_BC_04.brlan",
        "me_bg_select_BC_05.brlan",
        "me_bg_select_BC_06.brlan",
        "me_bg_select_BC_07.brlan",
        "me_bg_select_multi_w00.brlan",
        "me_bg_select_multi_w01.brlan",
        "me_bg_select_multi_w02.brlan",
        "me_bg_select_multi_w03.brlan",
        "me_bg_select_multi_w04.brlan",
        "me_bg_select_multi_w05.brlan",
        "me_bg_select_CO_00.brlan",
        "me_bg_select_CO_01.brlan",
        "me_bg_select_CO_02.brlan",
        "me_bg_select_CO_03.brlan",
        "me_bg_select_BTN_vis_00.brlan",
        "me_bg_select_BTN_vis_01.brlan",
        "me_bg_select_BTN_vis_02.brlan"
    };

    // CCourtTab's animations.
    enum {
        TAB_ANIM_TAB_VIS_00,
        TAB_ANIM_TAB_VIS_01,
        TAB_ANIM_TUB_ACT_00,  // tab off
        TAB_ANIM_TUB_ACT_01,  // pointer over a tab
        TAB_ANIM_TUB_ACT_02,  // tab selected
        TAB_ANIM_TUB_ACT_03,  // pointer leaves the selected tab
        TAB_ANIM_TUB_ACT_04,  // tab deselected
        TAB_ANIM_TUB_ACT_ICO, // pointer over the selected tab
        TAB_ANIM_MULTI_W00,
        TAB_ANIM_MULTI_W01,   // window in
        TAB_ANIM_MULTI_W02,
        TAB_ANIM_MULTI_W03,   // window out
        TAB_ANIM_MULTI_W04,   // window in, coming back
        TAB_ANIM_MULTI_W05,   // window out, going back
        TAB_ANIM_WIN_C_00,    // the game kind's colour
        TAB_ANIM_WIN_C_01,
        TAB_ANIM_WIN_C_02,
        TAB_ANIM_WIN_C_03,
        TAB_ANIM_ARROW_MOV_00,
        TAB_ANIM_ARROW_MOV_01,
        TAB_ANIM_GW_VIS_00,
        TAB_ANIM_GW_VIS_01,
        TAB_ANIM_NUM
    };

    const char *s_TabAnimNames[TAB_ANIM_NUM] = {
        "me_bg_select_w_Tab_vis_00.brlan",
        "me_bg_select_w_Tab_vis_01.brlan",
        "me_bg_select_w_Tub_act_00.brlan",
        "me_bg_select_w_Tub_act_01.brlan",
        "me_bg_select_w_Tub_act_02.brlan",
        "me_bg_select_w_Tub_act_03.brlan",
        "me_bg_select_w_Tub_act_04.brlan",
        "me_bg_select_w_Tub_act_ico.brlan",
        "me_bg_select_w_multi_w00.brlan",
        "me_bg_select_w_multi_w01.brlan",
        "me_bg_select_w_multi_w02.brlan",
        "me_bg_select_w_multi_w03.brlan",
        "me_bg_select_w_multi_w04.brlan",
        "me_bg_select_w_multi_w05.brlan",
        "me_bg_select_w_Win_c_00.brlan",
        "me_bg_select_w_Win_c_01.brlan",
        "me_bg_select_w_Win_c_02.brlan",
        "me_bg_select_w_Win_c_03.brlan",
        "me_bg_select_w_Arrow_mov_00.brlan",
        "me_bg_select_w_Arrow_mov_01.brlan",
        "me_bg_select_w_Gw_vis_00.brlan",
        "me_bg_select_w_Gw_vis_01.brlan"
    };

    // CCourt's panes; the two tables differ in the stage buttons (three
    // named a to c, or four named a to d).
    enum {
        PANE_W00,
        PANE_INOUT,
        PANE_BTN01,
        PANE_BTN02,
        PANE_BTN_ANI = 4,     // + stage
        PANE_BTN = 8,         // + stage
        PANE_T_BTN = 12,      // + stage: the stage name
        PANE_B_BTN = 16,      // + stage
        PANE_NEW_NULL = 32,   // + stage
        PANE_NEW_P = 36,      // + stage: the "new" mark
        PANE_NUM = 40
    };

    const char *s_PaneNames3[PANE_NUM] = {
        "W00", "inout", "BTN01", "BTN02",
        "BTN01_a_ani", "BTN01_b_ani", "BTN01_c_ani", "dummy",
        "BTN01_a", "BTN01_b", "BTN01_c", "dummy",
        "t_BTN01_a", "t_BTN01_b", "t_BTN01_c", "dummy",
        "b_BTN01_a", "b_BTN01_b", "b_BTN01_c", "dummy",
        "rule_Vis", "rule_ico", "b_rule_ico", "s_rule_ico",
        "omakase_Vis", "omakase_ico", "b_omakase_ico", "s_omakase_ico",
        "omakase_pm_Vis", "omakase_pm_ico", "b_omakase_pm_ico", "s_omakase_pm_ico",
        "BTN03_new_Null", "BTN02_new_Null", "BTN01_new_Null", "dummy",
        "NEW_p_01", "NEW_p_02", "NEW_p_03", "dummy"
    };

    const char *s_PaneNames4[PANE_NUM] = {
        "W00", "inout", "BTN01", "BTN02",
        "BTN02_a_ani", "BTN02_b_ani", "BTN02_c_ani", "BTN02_d_ani",
        "BTN02_a", "BTN02_b", "BTN02_c", "BTN02_d",
        "t_BTN02_a", "t_BTN02_b", "t_BTN02_c", "t_BTN02_d",
        "b_BTN02_a", "b_BTN02_b", "b_BTN02_c", "b_BTN02_d",
        "rule_Vis", "rule_ico", "b_rule_ico", "s_rule_ico",
        "omakase_Vis", "omakase_ico", "b_omakase_ico", "s_omakase_ico",
        "omakase_pm_Vis", "omakase_pm_ico", "b_omakase_pm_ico", "s_omakase_pm_ico",
        "BTN04_new_Null", "BTN05_new_Null", "BTN06_new_Null", "BTN07_new_Null",
        "NEW_p_01", "NEW_p_02", "NEW_p_03", "NEW_p_04"
    };

    // CCourtTab's panes, one table per number of tabs (none, 2, 3, 4).
    enum {
        TAB_PANE_WG_POS,
        TAB_PANE_WG_01,
        TAB_PANE_WG_02,
        TAB_PANE_G_W01_POS,
        TAB_PANE_TUB2_VIS,
        TAB_PANE_TUB3_VIS,
        TAB_PANE_TUB4_VIS,
        TAB_PANE_YAJI_L = 27, // the arrow before the first cup
        TAB_PANE_YAJI_R = 28, // the arrow after the last cup
        TAB_PANE_NUM = 42
    };

    // Only 36 entries, but the game passes TAB_PANE_NUM with it too, so
    // the accessor's name table runs into s_TabPaneNames2. Panes 36 to 41
    // are never used without tabs.
    const char *s_TabPaneNames1[36] = {
        "wg_pos", "wg_01", "wg_02", "dummy",
        "tub2_vis", "tub3_vis", "tub4_vis",
        "dummy", "dummy", "dummy", "dummy", "dummy",
        "dummy", "dummy", "dummy", "dummy", "dummy",
        "dummy", "dummy", "dummy", "dummy", "dummy",
        "dummy", "dummy", "dummy", "dummy", "dummy",
        "yaji_L", "yaji_R",
        "dummy", "dummy", "dummy", "dummy",
        "selecct_N", "selecct_H", "selecct_P"
    };

    const char *s_TabPaneNames2[TAB_PANE_NUM] = {
        "wg_pos", "wg_01", "wg_02", "g_w01_pos",
        "tub2_vis", "tub3_vis", "tub4_vis",
        "t2_01", "tub2_01", "tub2_win01", "tub2_01_ico", "b_tub2_01",
        "t2_02", "tub2_02", "tub2_win02", "tub2_02_ico", "b_tub2_02",
        "dummy", "dummy", "dummy", "dummy", "dummy",
        "dummy", "dummy", "dummy", "dummy", "dummy",
        "yaji_L", "yaji_R",
        "KINOKO_pic", "FLOWER_pic", "STAR_pic", "EXTRA_pic",
        "selecct_N", "selecct_H", "selecct_P",
        "HO_mini_l", "HO_mini_r", "BA_mini_l", "BA_mini_r", "DO_mini_l", "DO_mini_r"
    };

    const char *s_TabPaneNames3[TAB_PANE_NUM] = {
        "wg_pos", "wg_01", "wg_02", "g_w01_pos",
        "tub2_vis", "tub3_vis", "tub4_vis",
        "t3_01", "tub3_01", "tub3_win01", "tub3_01_ico", "b_tub3_01",
        "t3_02", "tub3_02", "tub3_win02", "tub3_02_ico", "b_tub3_02",
        "t3_03", "tub3_03", "tub3_win03", "tub3_03_ico", "b_tub3_03",
        "dummy", "dummy", "dummy", "dummy", "dummy",
        "yaji_L", "yaji_R",
        "KINOKO_pic", "FLOWER_pic", "STAR_pic", "EXTRA_pic",
        "selecct_N", "selecct_H", "selecct_P",
        "HO_mini_l", "HO_mini_r", "BA_mini_l", "BA_mini_r", "DO_mini_l", "DO_mini_r"
    };

    const char *s_TabPaneNames4[TAB_PANE_NUM] = {
        "wg_pos", "wg_01", "wg_02", "g_w01_pos",
        "tub2_vis", "tub3_vis", "tub4_vis",
        "t4_01", "tub4_01", "tub4_win01", "tub4_01_ico", "b_tub4_01",
        "t4_02", "tub4_02", "tub4_win02", "tub4_02_ico", "b_tub4_02",
        "t4_03", "tub4_03", "tub4_win03", "tub4_03_ico", "b_tub4_03",
        "t4_04", "tub4_04", "tub4_win04", "tub4_04_ico", "b_tub4_04",
        "yaji_L", "yaji_R",
        "KINOKO_pic", "FLOWER_pic", "STAR_pic", "EXTRA_pic",
        "selecct_N", "selecct_H", "selecct_P",
        "HO_mini_l", "HO_mini_r", "BA_mini_l", "BA_mini_r", "DO_mini_l", "DO_mini_r"
    };

    // A tab's icon pane and a picture pane that can be put into it.
    struct TabIcon {
        long m_nIconPane;
        long m_nPicPane;
    };

    // The pictures of the tabs: four cup marks, or (in the party games,
    // m_Unk9B == 2) the difficulty or mini-game marks. Each run is read
    // from its first entry; a tab takes the picture of the next enabled
    // cup.
    TabIcon s_TabIcons[14] = {
        { 10, 29 }, { 15, 30 }, { 20, 31 }, { 25, 32 },  // KINOKO .. EXTRA
        { 10, 33 }, { 15, 34 }, { 20, 35 }, { 25, 35 },  // selecct_N, H, P
        { 10, 38 }, { 15, 39 },                          // BA_mini
        { 10, 36 }, { 15, 37 },                          // HO_mini
        { 10, 40 }, { 15, 41 }                           // DO_mini
    };

    // Size 0x20. The constant part of a button's description.
    struct ButtonParam {
        unsigned char m_nBtnKindId;         // 0x00
        unsigned char m_nHitPane;           // 0x01
        unsigned char m_nAnim[5];           // 0x02
        unsigned short m_nNextBtnKindId[5]; // 0x08
        long m_nSeCode[2];                  // 0x14
        bool m_bUnk04;                      // 0x1C, flag 0x0004 of the part
        bool m_bStickMode;                  // 0x1D
    };

    // The stage buttons of CCourt.
    const ButtonParam s_StageButton3[3] = {
        { 8, 16, { 0, 1, 2, 3, 4 }, { 0x50, 0, 0x51, 0, 0 }, { 0xBBD, 0xBC0 }, true, true },
        { 9, 17, { 0, 1, 2, 3, 4 }, { 0x51, 0x50, 0x52, 0, 0 }, { 0xBBD, 0xBC0 }, true, true },
        { 10, 18, { 0, 1, 2, 3, 4 }, { 0x52, 0x51, 5, 0, 0 }, { 0xBBD, 0xBC0 }, true, true }
    };

    const ButtonParam s_StageButton4[4] = {
        { 8, 16, { 0, 1, 2, 3, 4 }, { 0x50, 0, 0x51, 0, 0 }, { 0xBBD, 0xBC0 }, true, true },
        { 9, 17, { 0, 1, 2, 3, 4 }, { 0x51, 0x50, 0x52, 0, 0 }, { 0xBBD, 0xBC0 }, true, true },
        { 10, 18, { 0, 1, 2, 3, 4 }, { 0x52, 0x51, 0x53, 0, 0 }, { 0xBBD, 0xBC0 }, true, true },
        { 11, 19, { 0, 1, 2, 3, 4 }, { 0x53, 0x52, 3, 0, 0 }, { 0xBBD, 0xBC0 }, true, true }
    };

    // With m_Unk9B == 4.
    const ButtonParam s_StageButton3B[3] = {
        { 8, 16, { 0, 1, 2, 3, 4 }, { 0x50, 0, 0x51, 0, 0 }, { 0xBBD, 0xBC0 }, true, true },
        { 9, 17, { 0, 1, 2, 3, 4 }, { 0x51, 0x50, 0x52, 0, 0 }, { 0xBBD, 0xBC0 }, true, true },
        { 10, 18, { 0, 1, 2, 3, 4 }, { 0x52, 0x51, 6, 0, 0 }, { 0xBBD, 0xBC0 }, true, true }
    };

    // The tabs of CCourtTab, by number of tabs.
    const ButtonParam s_TabButton2[2] = {
        { 7, 11, { 0, 0, 0, 0, 0 }, { 0x56, 0, 0, 0, 0 }, { 0xBCD, -1 }, false, false },
        { 12, 16, { 0, 0, 0, 0, 0 }, { 0x57, 0, 0, 0, 0 }, { 0xBCD, -1 }, false, true }
    };

    const ButtonParam s_TabButton3[3] = {
        { 7, 11, { 0, 0, 0, 0, 0 }, { 0x56, 0, 0, 0, 0 }, { 0xBCD, -1 }, false, false },
        { 12, 16, { 0, 0, 0, 0, 0 }, { 0x57, 0, 0, 0, 0 }, { 0xBCD, -1 }, false, true },
        { 17, 21, { 0, 0, 0, 0, 0 }, { 0x58, 0, 0, 0, 0 }, { 0xBCD, -1 }, false, true }
    };

    const ButtonParam s_TabButton4[4] = {
        { 7, 11, { 0, 0, 0, 0, 0 }, { 0x56, 0, 0, 0, 0 }, { 0xBCD, -1 }, false, false },
        { 12, 16, { 0, 0, 0, 0, 0 }, { 0x57, 0, 0, 0, 0 }, { 0xBCD, -1 }, false, true },
        { 17, 21, { 0, 0, 0, 0, 0 }, { 0x58, 0, 0, 0, 0 }, { 0xBCD, -1 }, false, true },
        { 22, 26, { 0, 0, 0, 0, 0 }, { 0x59, 0, 0, 0, 0 }, { 0xBCD, -1 }, false, true }
    };

    // The pane of each tab (t2_01 .. t4_04).
    const long s_TabPanes[4] = { 7, 12, 17, 22 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TASK = 1,
        OBJ_PLUS_MINUS = 2,
        OBJ_COURT = 0x31,
        OBJ_COURT_TAB = 0x32
    };

    // Events sent to and handled by the screens.
    enum {
        EVENT_HIT = 0x13,
        EVENT_BACK = 0x20,
        EVENT_PUSH = 0x21,     // plus the pad number
        EVENT_OUT = 0x25,
        EVENT_CLOSE_OK = 0x26,
        EVENT_CLOSE = 0x2E,
        EVENT_GET_CUP = 0x44
    };

    enum {
        BTN_KIND_STAGE_0 = 0x50,
        BTN_KIND_CUP_0 = 0x56
    };

    enum {
        SE_TAB_POINT = 0xBC0,
        SE_CUP_NEXT = 0xBCD,
        SE_CUP_PREV = 0xBCE
    };

    // CCourt's own state, between STATE_SET and STATE_ACTIVE: waiting
    // for the stage buttons' opening animation.
    enum {
        STATE_APPEAR = 6
    };
}

// ---------------------------------------------------------------------------
// CCourt

msw::SelectMenu::CCourt::CCourt(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
    m_Unk5C = 0;
    m_nCursorStage = 0;
    m_Unk64 = 4;
}

msw::SelectMenu::CCourt::~CCourt()
{
    Remove();
}

void msw::SelectMenu::CCourt::Main()
{
    StateControl();

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CCourt::StateControl()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CPartySelect.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_W00, ANIM_MULTI_W04, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_INOUT, ANIM_MULTI_W04, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_W00, ANIM_MULTI_W01, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_INOUT, ANIM_MULTI_W01, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN: {
            int bEnd = 0;
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_W00, ANIM_MULTI_W04)) {
                    bEnd = 1;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_W00, ANIM_MULTI_W01)) {
                    bEnd = 1;
                }
            }
            if (bEnd) {
                m_bSeEnable = 1;
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        }
        case STATE_ACTIVE:
            CheckButtonAnim();
            break;
        case STATE_APPEAR:
            if (m_bFourStages == 0) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_BTN01, ANIM_CO_00)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_BTN02, ANIM_CO_00)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()
                && GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, EVENT_CLOSE_OK, 0)
                && GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_CLOSE_OK, 0)) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_W00, ANIM_MULTI_W05, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                    m_pPaneAccessor->PlayAnimation(PANE_INOUT, ANIM_MULTI_W05, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_W00, ANIM_MULTI_W03, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                    m_pPaneAccessor->PlayAnimation(PANE_INOUT, ANIM_MULTI_W03, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                GetSelectMenu()->SendObjEventTrigger(OBJ_COURT_TAB, EVENT_CLOSE, 0);
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_OUT, 0);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (IsBackFrameOut(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_W00, ANIM_MULTI_W05)) {
                    m_Unk30 = STATE_NONE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_W00, ANIM_MULTI_W03)) {
                    m_Unk30 = STATE_NONE;
                }
            }
            break;
    }
}

int msw::SelectMenu::CCourt::Set()
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
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_LayoutNames[0]);
            pPaneCtrl->BuildAnim(0, s_AnimNames, ANIM_NUM);

            long nBtnNum;
            const ButtonParam *pParams;
            // m_Unk9B is compared as a signed value (cmpwi), as in
            // game/menu/CPlusMinus.cpp.
            if ((long)pSelectMenu->m_Unk9B == 4) {
                SetPaneAccessor(pPaneCtrl, s_PaneNames3, PANE_NUM);
                pParams = s_StageButton3B;
                nBtnNum = 3;
            } else if (m_bFourStages == 0) {
                SetPaneAccessor(pPaneCtrl, s_PaneNames3, PANE_NUM);
                pParams = s_StageButton3;
                nBtnNum = 3;
            } else {
                SetPaneAccessor(pPaneCtrl, s_PaneNames4, PANE_NUM);
                pParams = s_StageButton4;
                nBtnNum = 4;
            }
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            for (long i = 0; i < nBtnNum; i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);

                const ButtonParam *pParam = &pParams[i];

                pPart->m_nBtnKindId = pParam->m_nBtnKindId;
                pPart->m_Unk16 = pParam->m_nHitPane;
                pPart->m_nCurrentBtnKindId = pParam->m_nBtnKindId;
                pPart->m_nAnim[0] = pParam->m_nAnim[1];
                pPart->m_nAnim[1] = pParam->m_nAnim[2];
                pPart->m_nAnim[2] = pParam->m_nAnim[3];
                pPart->m_nAnim[3] = pParam->m_nAnim[0];
                pPart->m_nAnim[4] = pParam->m_nAnim[4];
                pPart->m_nAnim[5] = pParam->m_nAnim[2];
                pPart->m_nNextBtnKindId[0] = pParam->m_nNextBtnKindId[0];
                pPart->m_nNextBtnKindId[1] = pParam->m_nNextBtnKindId[1];
                pPart->m_nNextBtnKindId[2] = pParam->m_nNextBtnKindId[2];
                pPart->m_nNextBtnKindId[3] = pParam->m_nNextBtnKindId[3];
                pPart->m_nNextBtnKindId[4] = pParam->m_nNextBtnKindId[4];
                pPart->m_nSeCode[0] = pParam->m_nSeCode[1];
                pPart->m_nSeCode[1] = pParam->m_nSeCode[0];
                pPart->m_nSeCode[2] = 1;
                // Written on the whole flag word, as in CButtonEntry.cpp.
                pPart->m_nFlags40 = pParam->m_bUnk04 ? (pPart->m_nFlags40 | 0x0004) : (pPart->m_nFlags40 & ~0x0004);
                pPart->m_nFlags40 = pParam->m_bStickMode ? (pPart->m_nFlags40 | 0x0020) : (pPart->m_nFlags40 & ~0x0020);
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            if ((long)pSelectMenu->m_Unk9B == 4 && pSelectMenu->m_bStarMii != 0) {
                m_bSendObj1 = 0;
            } else if (pSelectMenu->m_UnkA68 != 0) {
                m_bSendObj1 = 0;
            } else {
                m_bSendObj1 = 1;
            }

            long nStageNum;
            if (m_bFourStages == 0) {
                m_pPaneAccessor->PlayAnimation(PANE_BTN01, ANIM_CO_00, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_BTN02, ANIM_BTN_VIS_00, 1.0f, 0.0f, 1, 0);
                nStageNum = 3;
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_BTN01, ANIM_BTN_VIS_00, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_BTN02, ANIM_CO_00, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_BTN02, ANIM_BTN_VIS_01, 1.0f, 0.0f, 1, 0);
                nStageNum = 4;
            }

            long nGameKind = GetSelectMenu()->m_nGameKind;
            if (nGameKind == -1) {
                nGameKind = 0;
            }
            m_pPaneAccessor->PlayAnimation(PANE_W00, nGameKind + ANIM_BC_00, 1.0f, 0.0f, 1, 0);

            // Moves each "new" mark into its button and hides it.
            nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
            for (long i = 0; i < nStageNum; i++) {
                nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(PANE_NEW_NULL + i);
                nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(PANE_NEW_P + i);
                bcs::G2D::SetPaneParent(pPane, pParent, 1, 0);
                pPane->SetTranslate(zero);
                m_pPaneAccessor->SetPaneDisp(PANE_NEW_P + i, 0);
            }

            // The results are not used.
            m_pMenuPartList->FindObjectByCurrentBtnKindId(6);
            m_pMenuPartList->FindObjectByCurrentBtnKindId(5);
            m_pMenuPartList->FindObjectByCurrentBtnKindId(3);

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            ClearPushButtonId();
            m_Unk5C = 0;

            pSelectMenu->Build(OBJ_PLUS_MINUS);
            CPlusMinus *pPlusMinus = static_cast<CPlusMinus *>(pSelectMenu->FindObj(OBJ_PLUS_MINUS));
            if ((long)pSelectMenu->m_Unk9B == 4) {
                pPlusMinus->StartUp(0, 0, GetMessageLabelString("List_Plus_Minus_Button", "Minus_03"));
            } else if ((long)pSelectMenu->m_Unk9B == 2) {
                pPlusMinus->StartUp(0, 0, GetMessageLabelString("List_Plus_Minus_Button", "Minus_01"));
            } else {
                const wchar_t *pPlus = GetMessageLabelString("List_Plus_Minus_Button", "Plus_04");
                const wchar_t *pMinus = GetMessageLabelString("List_Plus_Minus_Button", "Minus_01");
                pPlusMinus->StartUp(2, pPlus, pMinus);
            }
        }
    }
    return 0;
}

void msw::SelectMenu::CCourt::Remove()
{
    GetSelectMenu()->Scratch(OBJ_PLUS_MINUS);
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CCourt::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_COURT_TAB, EVENT_HIT, pPos);
    if (nResult == 0) {
        nResult = m_pMenuPartList->CheckHitButton(pPos);
        if (nResult == 0 && m_bSendObj1 != 0) {
            nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, EVENT_HIT, pPos);
        }
    }
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_HIT, pPos);
    }
    return nResult;
}

long msw::SelectMenu::CCourt::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCourt::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCourt::SetCursorMarkerPosition(long, long nBtnKindId)
{
    long nStage = GetStageButtonKindId(nBtnKindId);
    if (nStage >= 0) {
        m_nCursorStage = nStage;
    }
    return 0;
}

long msw::SelectMenu::CCourt::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CCourt::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CCourt::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_STAGE_0;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CCourt::IsButtonKindOfStickMode(long nBtnKindId)
{
    if (m_pMenuPartList == 0) {
        return 0;
    }

    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CCourt::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }

    long nEvent = nPad + EVENT_PUSH;
    if (m_bSendObj1 != 0) {
        GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, nEvent, (void *)nBtnKindId);
    }
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, nEvent, (void *)nBtnKindId);
    return 0;
}

void msw::SelectMenu::CCourt::SetTextStageName(long nStage, const wchar_t *pText)
{
    if (pText != 0) {
        SetTextPaneString(PANE_T_BTN + nStage, pText);
    }
}

void msw::SelectMenu::CCourt::SetStageId(long nStage, long nIdx, long nStageId)
{
    m_nStageId[nStage][nIdx] = nStageId;
}

void msw::SelectMenu::CCourt::SetStageEnable(long nStage, int bEnable, unsigned char bDisp)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_STAGE_0 + nStage);
    // The game uses the part before checking it.
    pPart->ClearPadNoBits();
    pPart->SetButtonOut();
    if (pPart == 0) {
        return;
    }

    if (bEnable == 1) {
        // Flag 0x0001 marks a disabled part.
        pPart->m_nFlags40 &= 0xFFFE;
        long nGameKind = GetSelectMenu()->m_nGameKind;
        if (nGameKind == -1) {
            nGameKind = 0;
        }
        m_pPaneAccessor->PlayAnimation(PANE_BTN_ANI + nStage, nGameKind + ANIM_BC_00, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_NEW_P + nStage, bDisp);
    } else {
        pPart->m_nFlags40 |= 0x0001;
        m_pPaneAccessor->PlayAnimation(PANE_BTN_ANI + nStage, ANIM_BTN_00, 1.0f, 0.0f, 1, 0);
        long nGameKind = GetSelectMenu()->m_nGameKind;
        if (nGameKind == -1) {
            nGameKind = 0;
        }
        m_pPaneAccessor->PlayAnimation(PANE_BTN_ANI + nStage, nGameKind + ANIM_BC_04, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_NEW_P + nStage, 0);
    }
}

long msw::SelectMenu::CCourt::GetStageButtonKindId(long nBtnKindId)
{
    switch (nBtnKindId) {
        case BTN_KIND_STAGE_0:
            return 0;
        case BTN_KIND_STAGE_0 + 1:
            return 1;
        case BTN_KIND_STAGE_0 + 2:
            return 2;
        case BTN_KIND_STAGE_0 + 3:
            return 3;
    }
    return -1;
}

void msw::SelectMenu::CCourt::CallbackButtonAnimIn(long)
{
}

void msw::SelectMenu::CCourt::SetAppearanceAnim()
{
    if (m_bFourStages == 0) {
        m_pPaneAccessor->PlayAnimation(PANE_BTN01, ANIM_CO_00, 1.0f, 0.0f, 1, 0);
    } else {
        m_pPaneAccessor->PlayAnimation(PANE_BTN02, ANIM_CO_00, 1.0f, 0.0f, 1, 0);
    }
    m_Unk30 = STATE_APPEAR;
}

void msw::SelectMenu::CCourt::SetOneTabAnim(long nTab)
{
    if (m_bFourStages != 0) {
        return;
    }

    if (nTab == 0) {
        m_pPaneAccessor->PlayAnimation(PANE_BTN01, ANIM_BTN_VIS_01, 1.0f, 0.0f, 1, 0);
    } else {
        m_pPaneAccessor->PlayAnimation(PANE_BTN01, ANIM_BTN_VIS_02, 1.0f, 0.0f, 1, 0);
    }
}

unsigned short msw::SelectMenu::CCourt::GetNextButtonKindId(long nBtnKindId)
{
    for (;;) {
        CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
        if (pPart == 0) {
            return 0;
        }
        if (!pPart->m_bUnk40_0001) {
            return nBtnKindId;
        }
        nBtnKindId = pPart->m_nNextBtnKindId[2];
    }
}

// Inlined into StateControl(); defined down here because its local static
// follows Set()'s float literals in .rodata.
inline void msw::SelectMenu::CCourt::CheckButtonAnim()
{
    static const int backEvent = EVENT_BACK;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    if (m_bSendObj1 != 0) {
        GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, backEvent, m_nPushButtonId);
    }
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, backEvent, m_nPushButtonId);
}

// Inlined into SetTextStageName().
inline void msw::SelectMenu::CCourt::SetTextPaneString(long nPane, const wchar_t *pText)
{
    static const bool recursive = true;

    if (pText == 0) {
        return;
    }

    CMenuPaneAccessor *pAccessor = m_pPaneAccessor;
    nw4r::lyt::Pane *pPane = (unsigned long)nPane < pAccessor->m_nNum
        ? pAccessor->GetPane(pAccessor->GetPaneName(nPane), recursive, 0) : 0;
    if (pPane != 0) {
        m_pPaneAccessor->SetTextBoxString(nPane, pText, wcslen(pText));
    }
}

// ---------------------------------------------------------------------------
// CCourtTab

msw::SelectMenu::CCourtTab::CCourtTab(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
    m_nCup = -1;
    m_Unk64 = 4;
    m_Unk6B = 0;
}

msw::SelectMenu::CCourtTab::~CCourtTab()
{
    Remove();
}

void msw::SelectMenu::CCourtTab::Main()
{
    StateControl();
}

void msw::SelectMenu::CCourtTab::StateControl()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CPartySelect.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_POS, TAB_ANIM_MULTI_W04, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_POS, TAB_ANIM_MULTI_W01, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN: {
            int bEnd = 0;
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(TAB_PANE_WG_POS, TAB_ANIM_MULTI_W04)) {
                    bEnd = 1;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(TAB_PANE_WG_POS, TAB_ANIM_MULTI_W01)) {
                    bEnd = 1;
                }
            }
            if (bEnd) {
                m_bSeEnable = 1;
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        }
        case STATE_ACTIVE:
            if (m_nTabType != 0) {
                m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
            }
            break;
        case STATE_CLOSE: {
            // Waits for the court screen to start closing.
            CCourt *pCourt = static_cast<CCourt *>(GetSelectMenu()->FindObj(OBJ_COURT));
            if (GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, EVENT_CLOSE_OK, 0) && pCourt->m_Unk30 == STATE_OUT) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_POS, TAB_ANIM_MULTI_W05, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_POS, TAB_ANIM_MULTI_W03, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                m_Unk30 = STATE_OUT;
            }
            break;
        }
        case STATE_OUT: {
            // The game waits for the same animation either way.
            int bEnd = 0;
            if (IsBackFrameOut(0)) {
                if (m_pPaneAccessor->IsAnimEnd(TAB_PANE_WG_POS, TAB_ANIM_MULTI_W03)) {
                    bEnd = 1;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(TAB_PANE_WG_POS, TAB_ANIM_MULTI_W03)) {
                    bEnd = 1;
                }
            }
            if (bEnd) {
                m_nCup = -1;
                m_Unk30 = STATE_NONE;
            }
            break;
        }
    }
}

int msw::SelectMenu::CCourtTab::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    unsigned char nTabType = m_nTabType;
    const char *pFileName = pSelectMenu->GetArchiveFileName(pSelectMenu->GetLoadArchiveFileId(-1));
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        // The game asks for the instance again here.
        CLytCtrl *pFontCtrl = CLytCtrl::GetInstance();
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pFontCtrl->GetArchiveFont(0), pFontCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pFontCtrl->GetArchiveFont(1), pFontCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_LayoutNames[1]);
            pPaneCtrl->BuildAnim(0, s_TabAnimNames, TAB_ANIM_NUM);

            long nBtnNum = 0;
            const ButtonParam *pParams = 0;
            switch (nTabType) {
                case 0:
                    SetPaneAccessor(pPaneCtrl, s_TabPaneNames1, TAB_PANE_NUM);
                    break;
                case 1:
                    SetPaneAccessor(pPaneCtrl, s_TabPaneNames2, TAB_PANE_NUM);
                    pParams = s_TabButton2;
                    nBtnNum = 2;
                    break;
                case 2:
                    SetPaneAccessor(pPaneCtrl, s_TabPaneNames3, TAB_PANE_NUM);
                    pParams = s_TabButton3;
                    nBtnNum = 3;
                    break;
                case 3:
                    SetPaneAccessor(pPaneCtrl, s_TabPaneNames4, TAB_PANE_NUM);
                    pParams = s_TabButton4;
                    nBtnNum = 4;
                    break;
            }
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);

            if (pParams != 0) {
                CMenuPartList *pMenuPartList = new CMenuPartList;

                pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

                for (long i = 0; i < nBtnNum; i++) {
                    CMenuPartButton *pPart = new CMenuPartButton(i);

                    pPart->m_pList = pMenuPartList;
                    pMenuPartList->Add(pPart);

                    const ButtonParam *pParam = &pParams[i];

                    pPart->m_nBtnKindId = pParam->m_nBtnKindId;
                    pPart->m_Unk16 = pParam->m_nHitPane;
                    pPart->m_nCurrentBtnKindId = pParam->m_nBtnKindId;
                    pPart->m_nAnim[0] = pParam->m_nAnim[1];
                    pPart->m_nAnim[1] = pParam->m_nAnim[2];
                    pPart->m_nAnim[2] = pParam->m_nAnim[3];
                    pPart->m_nAnim[3] = pParam->m_nAnim[0];
                    pPart->m_nAnim[4] = pParam->m_nAnim[4];
                    pPart->m_nAnim[5] = pParam->m_nAnim[2];
                    pPart->m_nNextBtnKindId[0] = pParam->m_nNextBtnKindId[0];
                    pPart->m_nNextBtnKindId[1] = pParam->m_nNextBtnKindId[1];
                    pPart->m_nNextBtnKindId[2] = pParam->m_nNextBtnKindId[2];
                    pPart->m_nNextBtnKindId[3] = pParam->m_nNextBtnKindId[3];
                    pPart->m_nNextBtnKindId[4] = pParam->m_nNextBtnKindId[4];
                    pPart->m_nSeCode[0] = pParam->m_nSeCode[1];
                    pPart->m_nSeCode[1] = pParam->m_nSeCode[0];
                    pPart->m_nSeCode[2] = 1;
                    // Written on the whole flag word, as in CButtonEntry.cpp.
                    pPart->m_nFlags40 = pParam->m_bUnk04 ? (pPart->m_nFlags40 | 0x0004) : (pPart->m_nFlags40 & ~0x0004);
                    pPart->m_nFlags40 = pParam->m_bStickMode ? (pPart->m_nFlags40 | 0x0020) : (pPart->m_nFlags40 & ~0x0020);
                    pPart->SetCallBackAnimIn(this);
                    pPart->InitButtonAnim();
                }
                m_pMenuPartList = pMenuPartList;
            }
        }

        long nCupNum = 0;
        switch (nTabType) {
            case 0:
                m_pPaneAccessor->PlayAnimation(TAB_PANE_TUB2_VIS, TAB_ANIM_TAB_VIS_00, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_TUB3_VIS, TAB_ANIM_TAB_VIS_00, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_TUB4_VIS, TAB_ANIM_TAB_VIS_00, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_YAJI_L, TAB_ANIM_ARROW_MOV_01, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_YAJI_R, TAB_ANIM_ARROW_MOV_01, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_01, TAB_ANIM_GW_VIS_01, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_02, TAB_ANIM_GW_VIS_00, 1.0f, 0.0f, 1, 1);
                break;
            case 1:
                m_pPaneAccessor->PlayAnimation(TAB_PANE_TUB2_VIS, TAB_ANIM_TAB_VIS_01, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_TUB3_VIS, TAB_ANIM_TAB_VIS_00, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_TUB4_VIS, TAB_ANIM_TAB_VIS_00, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_01, TAB_ANIM_GW_VIS_00, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_02, TAB_ANIM_GW_VIS_01, 1.0f, 0.0f, 1, 1);
                nCupNum = 2;
                break;
            case 2:
                m_pPaneAccessor->PlayAnimation(TAB_PANE_TUB2_VIS, TAB_ANIM_TAB_VIS_00, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_TUB3_VIS, TAB_ANIM_TAB_VIS_01, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_TUB4_VIS, TAB_ANIM_TAB_VIS_00, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_01, TAB_ANIM_GW_VIS_00, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_02, TAB_ANIM_GW_VIS_01, 1.0f, 0.0f, 1, 1);
                nCupNum = 3;
                break;
            case 3:
                m_pPaneAccessor->PlayAnimation(TAB_PANE_TUB2_VIS, TAB_ANIM_TAB_VIS_00, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_TUB3_VIS, TAB_ANIM_TAB_VIS_00, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_TUB4_VIS, TAB_ANIM_TAB_VIS_01, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_01, TAB_ANIM_GW_VIS_00, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_02, TAB_ANIM_GW_VIS_01, 1.0f, 0.0f, 1, 1);
                nCupNum = 4;
                break;
        }

        long nGameKind = GetSelectMenu()->m_nGameKind;
        if (nGameKind <= -1) {
            nGameKind = 0;
        }
        m_pPaneAccessor->PlayAnimation(TAB_PANE_WG_POS, nGameKind + TAB_ANIM_WIN_C_00, 1.0f, 0.0f, 1, 1);

        nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
        long nIcon = 0;
        if ((long)pSelectMenu->m_Unk9B == 2) {
            switch (pSelectMenu->m_Unk9C) {
                case 3:
                    nIcon = 4;
                    break;
                case 1:
                    nIcon = 8;
                    break;
                case 7:
                    nIcon = 10;
                    break;
                case 5:
                    nIcon = 12;
                    break;
                default:
                    nIcon = 4;
                    break;
            }
        }

        // Puts the picture of each enabled cup into the next tab's icon.
        const TabIcon *pIcons = &s_TabIcons[nIcon];
        const TabIcon *pPic = pIcons;
        unsigned char nMask = m_nCupMask;
        long nCup = 0;
        for (long i = 0; i < 4; i++) {
            m_nTabCup[i] = 0;
        }
        for (long i = 0; i < nCupNum; i++) {
            nw4r::lyt::Pane *pIcon = m_pPaneAccessor->GetPane(pIcons[i].m_nIconPane);
            nw4r::lyt::Pane *pPane = 0;
            while (pPane == 0) {
                if (nMask & 1) {
                    pPane = m_pPaneAccessor->GetPane(pPic->m_nPicPane);
                    m_nTabCup[i] = nCup;
                }
                nMask >>= 1;
                pPic++;
                nCup++;
            }
            bcs::G2D::SetPaneParent(pPane, pIcon, 1, 0);
            pPane->SetTranslate(zero);
        }

        if (nTabType != 0) {
            m_pPaneAccessor->PlayAnimation(TAB_PANE_YAJI_L, TAB_ANIM_ARROW_MOV_00, 1.0f, 0.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(TAB_PANE_YAJI_R, TAB_ANIM_ARROW_MOV_00, 1.0f, 0.0f, 1, 1);
        }

        m_bLeftArrow = 0;
        m_bRightArrow = 0;
        m_Unk2C = 1;
        m_Unk30 = STATE_SET;
        ClearPushButtonId();
        m_bVibEnable = 1;
        m_nCup = -1;
    }
    return 0;
}

void msw::SelectMenu::CCourtTab::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

inline long msw::SelectMenu::CCourtTab::GetEnabledCupIndex(long nCup) const
{
    long nIndex = 0;
    for (long i = 0; i < nCup; i++) {
        if ((1 << i) & m_nCupMask) {
            nIndex++;
        }
    }
    return nIndex;
}

int msw::SelectMenu::CCourtTab::SetCupSelect(long nCup)
{
    long nPrevCup = m_nCup;
    if (nCup == nPrevCup) {
        return 0;
    }

    // The arrows are hidden at the first and the last cup.
    if (nCup == 0 && m_bLeftArrow == 0) {
        m_pPaneAccessor->PlayAnimation(TAB_PANE_YAJI_L, TAB_ANIM_ARROW_MOV_01, 1.0f, 0.0f, 1, 0);
        m_bLeftArrow = 1;
    } else if (m_bLeftArrow != 0) {
        m_pPaneAccessor->PlayAnimation(TAB_PANE_YAJI_L, TAB_ANIM_ARROW_MOV_00, 1.0f, 0.0f, 1, 0);
        m_bLeftArrow = 0;
    }
    if (nCup == m_nTabType && m_bRightArrow == 0) {
        m_pPaneAccessor->PlayAnimation(TAB_PANE_YAJI_R, TAB_ANIM_ARROW_MOV_01, 1.0f, 0.0f, 1, 0);
        m_bRightArrow = 1;
    } else if (m_bRightArrow != 0) {
        m_pPaneAccessor->PlayAnimation(TAB_PANE_YAJI_R, TAB_ANIM_ARROW_MOV_00, 1.0f, 0.0f, 1, 0);
        m_bRightArrow = 0;
    }

    if (CMenuPad::s_Instance.IsInputPointerMode(0)) {
        SE_Play(SE_CUP_NEXT, 1);
    } else if (nPrevCup > nCup) {
        SE_Play(SE_CUP_PREV, 1);
    } else {
        SE_Play(SE_CUP_NEXT, 1);
    }

    m_nCup = nCup;
    long nIndex = GetEnabledCupIndex(nCup);

    const ButtonParam *pParam;
    switch (m_nTabType) {
        case 1:
            pParam = s_TabButton2;
            break;
        case 2:
            pParam = s_TabButton3;
            break;
        case 3:
            pParam = s_TabButton4;
            break;
        default:
            return 0;
    }

    const long *pTabPane = s_TabPanes;
    for (long i = 0; i <= m_nTabType; pTabPane++, pParam++, i++) {
        m_pPaneAccessor->StopAnimation(*pTabPane, TAB_ANIM_TUB_ACT_01, 1);
        if (nIndex == i) {
            m_pPaneAccessor->PlayAnimation(pParam->m_nBtnKindId, TAB_ANIM_TUB_ACT_02, 1.0f, 0.0f, 1, 0);
        } else if (nPrevCup == i) {
            m_pPaneAccessor->PlayAnimation(pParam->m_nBtnKindId, TAB_ANIM_TUB_ACT_04, 1.0f, 0.0f, 1, 0);
        } else {
            m_pPaneAccessor->PlayAnimation(pParam->m_nBtnKindId, TAB_ANIM_TUB_ACT_00, 1.0f, 0.0f, 1, 0);
        }
    }
    return 1;
}

long msw::SelectMenu::CCourtTab::GetCupBtnKindId(long nCup)
{
    return nCup + BTN_KIND_CUP_0;
}

long msw::SelectMenu::CCourtTab::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }
    if (m_nTabType == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    if (GetSelectMenu()->m_aPadSelect[nPad].m_Unk03 & 0x40) {
        return nResult;
    }
    if (!CMenuPad::s_Instance.IsInputPointerMode(nPad)) {
        return nResult;
    }

    long nIndex = GetEnabledCupIndex(m_nCup);
    const long *pTabPane = s_TabPanes;
    for (long i = 0; i < 4; i++, pTabPane++) {
        long nBtnKindId = GetCupBtnKindId(i);
        if (m_bTabPointed[i] == 0 && nBtnKindId == nResult) {
            if (i == nIndex) {
                m_pPaneAccessor->PlayAnimation(*pTabPane, TAB_ANIM_TUB_ACT_ICO, 1.0f, 0.0f, 1, 0);
            } else {
                m_pPaneAccessor->PlayAnimation(*pTabPane, TAB_ANIM_TUB_ACT_01, 1.0f, 0.0f, 1, 0);
            }
            Vib_Start(1, 1, 1);
            SE_Play(SE_TAB_POINT, 1);
            m_bTabPointed[i] = 1;
        }
        if (m_bTabPointed[i] == 1 && nBtnKindId != nResult) {
            m_pPaneAccessor->StopAnimation(*pTabPane, TAB_ANIM_TUB_ACT_ICO, 1);
            m_pPaneAccessor->StopAnimation(*pTabPane, TAB_ANIM_TUB_ACT_01, 1);
            if (i == nIndex) {
                m_pPaneAccessor->PlayAnimation(*pTabPane, TAB_ANIM_TUB_ACT_03, 1.0f, 0.0f, 1, 0);
            } else {
                m_pPaneAccessor->PlayAnimation(*pTabPane, TAB_ANIM_TUB_ACT_00, 1.0f, 0.0f, 1, 0);
            }
            m_bTabPointed[i] = 0;
        }
    }
    return nResult;
}

long msw::SelectMenu::CCourtTab::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCourtTab::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCourtTab::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CCourtTab::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CCourtTab::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CCourtTab::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_CUP_0;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CCourtTab::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

void msw::SelectMenu::CCourtTab::CallbackButtonAnimIn(long)
{
}

long msw::SelectMenu::CCourtTab::SendEvent(long nEvent, void *pParam)
{
    long nResult = -1;
    switch (nEvent) {
        case EVENT_HIT:
            nResult = HitCheck(static_cast<bcs::FVEC3 *>(pParam));
            break;
        case EVENT_GET_CUP:
            nResult = m_nCup;
            break;
        case EVENT_CLOSE:
            if (m_Unk30 < STATE_CLOSE) {
                m_Unk30 = STATE_CLOSE;
            }
            break;
        default:
            nResult = 0;
            break;
    }
    return nResult;
}

long msw::SelectMenu::CCourtTab::PushButton(long nPad, long nBtnKindId)
{
    m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad);
    return 0;
}

void msw::SelectMenu::CCourtTab::AllStopTabAnim()
{
    for (long i = 0; i < 4; i++) {
        m_pPaneAccessor->StopAnimation(s_TabPanes[i], TAB_ANIM_TUB_ACT_ICO, 1);
        m_pPaneAccessor->StopAnimation(s_TabPanes[i], TAB_ANIM_TUB_ACT_01, 1);
        m_bTabPointed[i] = 0;
    }
}
