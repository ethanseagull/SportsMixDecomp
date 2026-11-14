// msw::CSignBoard: a board (a telop such as "deuce", "match point" or
// "game set") made of one layout from a fixed table. It plays the in
// animation, then the loop until told to go, then the out animation.
#include "game/msw/CSignBoard.hpp"

#include "bcs/EF/CEffect.hpp"
#include "bcs/G2D/CLayoutDL.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"

namespace {

    // Animations of a board.
    enum {
        ANIM_IN,
        ANIM_LOOP,
        ANIM_OUT,
        ANIM_NUM
    };

    // A board: its layout, the cockpit archive file it is in, its priority
    // in the layout manager, its animations and, for a few, an effect
    // attached to a pane (with the delay time of the effects).
    struct BoardRes {
        const char *pLayout;          // 0x00
        unsigned char nResFile;       // 0x04, for CCockpit::GetArchiveFileName
        unsigned char nPriority;      // 0x05
        unsigned char nAnimNum;       // 0x06
        const char *pAnim[ANIM_NUM];  // 0x08
        const char *pEffectPane;      // 0x14, null for none
        long nEffect;                 // 0x18
        float fEffectDelay;           // 0x1C
    };

    const BoardRes s_BoardRes[] = {
        { "TE_deuce.brlyt", 1, 0, 1, { "TE_deuce_Te_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "ck_gaameset.brlyt", 1, 0, 1, { "ck_gaameset.brlan", 0, 0 }, 0, 0, 0.0f },
        { "TE_match_point.brlyt", 1, 0, 1, { "TE_match_point_Te_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "ck_matchpoint2.brlyt", 1, 0, 3, { "ck_matchpoint2_start.brlan", "ck_matchpoint2_loop.brlan", "ck_matchpoint2_end.brlan" }, 0, 0, 0.0f },
        { "ck_matchpoint2_02.brlyt", 1, 0, 3, { "ck_matchpoint2_02_start.brlan", "ck_matchpoint2_02_loop.brlan", "ck_matchpoint2_02_end.brlan" }, 0, 0, 0.0f },
        { "ck_matchpoint3.brlyt", 1, 0, 1, { "ck_matchpoint3.brlan", 0, 0 }, 0, 0, 0.0f },
        { "ck_remocon1.brlyt", 1, 0, 3, { "ck_remocon1_start.brlan", "ck_remocon1_loop.brlan", "ck_remocon1_end.brlan" }, 0, 0, 0.0f },
        { "ck_remocon2.brlyt", 1, 0, 3, { "ck_remocon2_start.brlan", "ck_remocon2_loop.brlan", "ck_remocon2_end.brlan" }, 0, 0, 0.0f },
        { "ck_remocon3.brlyt", 1, 0, 3, { "ck_remocon3_start.brlan", "ck_remocon3_loop.brlan", "ck_remocon3_end.brlan" }, 0, 0, 0.0f },
        { "TE_sudden_death.brlyt", 1, 0, 1, { "TE_sudden_death_Te_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "TE_start.brlyt", 1, 0, 1, { "TE_start_Te_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "TE_set_point.brlyt", 1, 0, 1, { "TE_set_point_Te_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "TE_safe.brlyt", 1, 0, 1, { "TE_safe_Se_01.brlan", 0, 0 }, 0, 0, 0.0f },
        { "TE_safe.brlyt", 1, 0, 1, { "TE_safe_Se_01.brlan", 0, 0 }, 0, 0, 0.0f },
        { "TE_safe.brlyt", 1, 0, 1, { "TE_safe_Se_02.brlan", 0, 0 }, 0, 0, 0.0f },
        { "TE_goal.brlyt", 1, 0, 1, { "TE_goal_Go_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "TE_no_goal.brlyt", 1, 0, 1, { "TE_no_goal_No_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "TE_own_goal.brlyt", 1, 0, 1, { "TE_own_goal_Own_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "TE_out_vo.brlyt", 1, 0, 1, { "TE_out_vo_Te_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "TE_start_vo.brlyt", 1, 0, 1, { "TE_start_vo_Te_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "TE_lose.brlyt", 1, 0, 3, { "TE_lose_Te_00.brlan", "TE_lose_Te_02.brlan", "TE_lose_Te_01.brlan" }, 0, 0, 0.0f },
        { "TE_lose.brlyt", 1, 0, 3, { "TE_lose_Te2_00.brlan", "TE_lose_Te2_02.brlan", "TE_lose_Te2_01.brlan" }, 0, 0, 0.0f },
        { "TE_lose.brlyt", 1, 0, 3, { "TE_lose_Te3_00.brlan", "TE_lose_Te3_02.brlan", "TE_lose_Te3_01.brlan" }, 0, 0, 0.0f },
        { "TE_win.brlyt", 1, 0, 3, { "TE_win_Te_00.brlan", "TE_win_Te_02.brlan", "TE_win_Te_01.brlan" }, 0, 0, 0.0f },
        { "TE_win.brlyt", 1, 0, 3, { "TE_win_Te2_00.brlan", "TE_win_Te2_02.brlan", "TE_win_Te2_01.brlan" }, 0, 0, 0.0f },
        { "TE_win.brlyt", 1, 0, 3, { "TE_win_Te3_00.brlan", "TE_win_Te3_02.brlan", "TE_win_Te3_01.brlan" }, 0, 0, 0.0f },
        { "TE_draw.brlyt", 1, 0, 3, { "TE_draw_Te_00.brlan", "TE_draw_Te_02.brlan", "TE_draw_Te_01.brlan" }, 0, 0, 0.0f },
        { "TE_v_bingo.brlyt", 1, 0, 3, { "TE_v_bingo_TE_00.brlan", "TE_v_bingo_TE_01.brlan", "TE_v_bingo_TE_02.brlan" }, 0, 0, 0.0f },
        { "TE_perfect.brlyt", 1, 0, 3, { "TE_perfect_TE_00.brlan", "TE_perfect_TE_01.brlan", "TE_perfect_TE_02.brlan" }, "e_Null", 0x462, 20.0f },
        { "TE_new_record.brlyt", 2, 0, 3, { "TE_new_record_TE_00.brlan", "TE_new_record_TE_01.brlan", "TE_new_record_TE_02.brlan" }, 0, 0, 0.0f },
        { "TE_ura.brlyt", 1, 0, 3, { "TE_ura_TI_TE_01.brlan", "TE_ura_TI_TE_02.brlan", "TE_ura_TI_TE_03.brlan" }, 0, 0, 0.0f },
        { "TE_championship_point.brlyt", 1, 0, 1, { "TE_championship_point_Te_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "result_TE_win_lose.brlyt", 2, 0, 3, { "result_TE_win_lose_win_lose_00.brlan", "result_TE_win_lose_win_lose_01.brlan", "result_TE_win_lose_win_lose_02.brlan" }, 0, 0, 0.0f },
        { "result_TE_lose_win.brlyt", 2, 0, 3, { "result_TE_lose_win_lose_win00.brlan", "result_TE_lose_win_lose_win01.brlan", "result_TE_lose_win_lose_win02.brlan" }, 0, 0, 0.0f },
        { "result_TE_draw.brlyt", 2, 0, 3, { "result_TE_draw_DRAW_00.brlan", "result_TE_draw_DRAW_01.brlan", "result_TE_draw_DRAW_02.brlan" }, 0, 0, 0.0f },
        { "main_wait.brlyt", 0, 6, 2, { "main_wait_nowloading_00.brlan", "main_wait_nowloading.brlan", 0 }, 0, 0, 0.0f },
        { "me_wait.brlyt", 1, 6, 2, { "me_wait_nowloading_00.brlan", "me_wait_wifi_nowloading.brlan", 0 }, 0, 0, 0.0f },
        { "WIN_I.brlyt", 0, 0, 3, { "WIN_I_all_ctl_00.brlan", "WIN_I_all_ctl_01.brlan", "WIN_I_all_ctl_02.brlan" }, 0, 0, 0.0f },
        { "WIN_I.brlyt", 0, 6, 3, { "WIN_I_all_ctl_00.brlan", "WIN_I_all_ctl_01.brlan", "WIN_I_all_ctl_02.brlan" }, 0, 0, 0.0f },
        { "WIN_K.brlyt", 0, 0, 3, { "WIN_K_all_ctl_00.brlan", "WIN_K_all_ctl_01.brlan", "WIN_K_all_ctl_02.brlan" }, 0, 0, 0.0f },
        { "TO_text_boss.brlyt", 1, 0, 3, { "TO_text_boss_TE_00.brlan", "TO_text_boss_TE_01.brlan", "TO_text_boss_TE_02.brlan" }, 0, 0, 0.0f },
        { "end_title.brlyt", 1, 0, 3, { "end_title_all_00.brlan", "end_title_all_01.brlan", "end_title_all_02.brlan" }, 0, 0, 0.0f },
        { "TE_starroad_compleat.brlyt", 1, 0, 3, { "TE_starroad_compleat_Te_00.brlan", "TE_starroad_compleat_Te_01.brlan", "TE_starroad_compleat_Te_02.brlan" }, 0, 0, 0.0f },
        { "HO_cockpit_btn_a.brlyt", 2, 0, 3, { "HO_cockpit_btn_a_A_01.brlan", "HO_cockpit_btn_a_A_02.brlan", "HO_cockpit_btn_a_A_03.brlan" }, 0, 0, 0.0f },
        { "bb_v_terop_00.brlyt", 2, 0, 1, { "bb_v_terop_00_anim_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "bb_v_terop_01.brlyt", 2, 0, 1, { "bb_v_terop_01_anim_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "bb_v_terop_02.brlyt", 2, 0, 1, { "bb_v_terop_02_anim_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "vo_v_terop_00.brlyt", 2, 0, 1, { "vo_v_terop_00_anim_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "vo_v_terop_01.brlyt", 2, 0, 1, { "vo_v_terop_01_anim_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "vo_v_terop_02.brlyt", 2, 0, 1, { "vo_v_terop_02_anim_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "do_v_terop_00.brlyt", 2, 0, 1, { "do_v_terop_00_anim_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "do_v_terop_01.brlyt", 2, 0, 1, { "do_v_terop_01_anim_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "do_v_terop_02.brlyt", 2, 0, 1, { "do_v_terop_02_anim_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "ho_v_terop_00.brlyt", 2, 0, 1, { "ho_v_terop_00_anim_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "ho_v_terop_01.brlyt", 2, 0, 1, { "ho_v_terop_01_anim_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "ho_v_terop_02.brlyt", 2, 0, 1, { "ho_v_terop_02_anim_00.brlan", 0, 0 }, 0, 0, 0.0f },
        { "CO_press_start.brlyt", 1, 6, 3, { "CO_press_start_ti_01.brlan", "CO_press_start_ti_02.brlan", "CO_press_start_ti_03.brlan" }, 0, 0, 0.0f },
        { "act_unable.brlyt", 2, 6, 3, { "act_unable_all_ctl_00.brlan", "act_unable_all_ctl_02.brlan", "act_unable_all_ctl_03.brlan" }, 0, 0, 0.0f },
        { "end_licence.brlyt", 1, 0, 3, { "end_licence_all_ctl_00.brlan", "end_licence_all_ctl_01.brlan", "end_licence_all_ctl_02.brlan" }, 0, 0, 0.0f },
    };

    // The panes for CMenuPaneAccessor (none is looked up by number).
    const char *s_PaneName[1] = { "dummy" };

    // Events of SendEvent.
    enum {
        EVENT_OUT = 10002,        // play the out animation
        EVENT_WAIT_OUT = 10003,   // pParam != 0: go out after the in animation
        EVENT_IS_PLAYING = 10004, // returns whether the in animation is over
        EVENT_GET_FRAMES = 10005, // returns the frames of all animations
        EVENT_SET_RATE = 10006    // pParam points to the new animation rate
    };

    // Steps of Main (m_nStep).
    enum {
        STEP_NONE,
        STEP_IN,
        STEP_LOOP,
        STEP_OUT
    };
}

msw::CSignBoard::CSignBoard(long nId, long nType, float fSortZ)
    : CCockpitLayoutBase(nId, fSortZ)
    , CCockpitFrameOutBase(1.0f)
{
    m_nType = nType;
    Set();

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    if (pPaneCtrl != 0) {
        pPaneCtrl->PlayAnim(ANIM_IN, m_fUnk04, 0.0f, 1, 1);
    }
    m_nStep = STEP_IN;
}

msw::CSignBoard::~CSignBoard()
{
    Remove();
}

int msw::CSignBoard::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = CCockpit::GetTask();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const BoardRes *pRes = &s_BoardRes[m_nType];
    const char *pArchiveName = pCockpit->GetArchiveFileName(pRes->nResFile);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, pRes->nPriority, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(2), pLytCtrl->GetFontResourceName(2));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(3), pLytCtrl->GetFontResourceName(3));

            if (pPaneCtrl->BuildLayout(0, pRes->pLayout) < 0) {
                DeleteLytObj();
                return -2;
            }
            pPaneCtrl->BuildAnim(0, const_cast<const char **>(pRes->pAnim), pRes->nAnimNum);
            SetPaneAccessor(pPaneCtrl, s_PaneName, 1);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(m_fSortZ);

            if (m_nWaitFrameOut < 0) {
                SetWaitFrameOut(IsWaitAnim(pPaneCtrl, ANIM_LOOP));
            }

            if (pRes->pEffectPane != 0) {
                float fDelay = pRes->fEffectDelay;
                long nEffect = pRes->nEffect;
                bcs::G2D::CPaneObj *pPaneObj = m_pPaneAccessor->GetPaneObj(pRes->pEffectPane);
                if (pPaneObj != 0) {
                    unsigned long hEffect = CEffectCtrl::CreateAttachEffect(nEffect, pPaneObj, 0, -1, 0, 0, 0, 4, 0, 0);
                    CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
                    if (pCluster != 0) {
                        pCluster->CallEffectFunc(&bcs::EF::CEffect::SetDelayTime, fDelay);
                    }
                }
            }
        }
    }
    return 0;
}

void msw::CSignBoard::Remove()
{
    m_nStep = STEP_NONE;
}

void msw::CSignBoard::Main()
{
    switch (m_nStep) {
        case STEP_IN: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
            if (pPaneCtrl == 0) {
                m_Unk08 = 2;
                break;
            }
            if (pPaneCtrl->IsAnimEnd(ANIM_IN)) {
                if (s_BoardRes[m_nType].nAnimNum > ANIM_LOOP) {
                    pPaneCtrl->PlayAnim(ANIM_LOOP, m_fUnk04, 0.0f, 1, 1);
                    m_nStep = STEP_LOOP;
                } else {
                    m_Unk08 = 2;
                }
            }
            break;
        }
        case STEP_LOOP: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
            if (pPaneCtrl == 0) {
                m_Unk08 = 2;
                break;
            }
            if (IsWaitFrameOut() == 1) {
                if (s_BoardRes[m_nType].nAnimNum > ANIM_OUT) {
                    pPaneCtrl->PlayAnim(ANIM_OUT, m_fUnk04, 0.0f, 1, 1);
                    m_nStep = STEP_OUT;
                } else {
                    m_Unk08 = 2;
                }
            }
            break;
        }
        case STEP_OUT: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
            if (pPaneCtrl == 0 || pPaneCtrl->IsAnimEnd(ANIM_OUT)) {
                m_Unk08 = 2;
            }
            break;
        }
    }
}

long msw::CSignBoard::SendEvent(long nEvent, void *pParam)
{
    long nResult = 0;

    switch (nEvent) {
        case EVENT_IS_PLAYING:
            nResult = m_nStep != STEP_IN;
            break;
        case EVENT_OUT: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
            if (pPaneCtrl != 0) {
                pPaneCtrl->PlayAnim(ANIM_OUT, 1.0f, 0.0f, 1, 1);
                m_nStep = STEP_OUT;
            }
            break;
        }
        case EVENT_WAIT_OUT:
            SetWaitFrameOut(pParam != 0);
            break;
        case EVENT_GET_FRAMES: {
            nResult = 0;
            long nNum = s_BoardRes[m_nType].nAnimNum;
            for (long i = 0; i < nNum; i++) {
                bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
                nResult += pPaneCtrl == 0 ? 0 : (long)pPaneCtrl->GetAnimFrameSize(i);
            }
            break;
        }
        case EVENT_SET_RATE:
            if (pParam != 0) {
                m_fUnk04 = *(float *)pParam;
                bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
                if (pPaneCtrl != 0) {
                    long nAnim;
                    switch (m_nStep) {
                        case STEP_LOOP:
                            nAnim = ANIM_LOOP;
                            break;
                        case STEP_OUT:
                            nAnim = ANIM_OUT;
                            break;
                        default:
                            nAnim = ANIM_IN;
                            break;
                    }
                    pPaneCtrl->PlayAnim(nAnim, *(float *)pParam, 0.0f, 1, 1);
                }
                nResult = 1;
            }
            break;
    }
    return nResult;
}
