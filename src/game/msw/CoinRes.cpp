#include "game/msw/CoinRes.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace msw {
    namespace CoinRes {

        namespace {

            const int ANIM_NUM = 11;
            const int TEAM_NUM = 2;

            // Layout file of each E_COINLAYOUTID.
            const char *s_LayoutResource[] = {
                "coin.brlyt",
                "VO_cockpit_02_coin.brlyt",
                "VO_cockpit_03_coin.brlyt",
            };

            // Animation files of each layout.
            const char *s_AnimResource00[ANIM_NUM] = {
                "coin_Coin_ani_00.brlan",
                "coin_Coin_ani_01.brlan",
                "coin_Coin_ani_02.brlan",
                "coin_Coin_ani_03.brlan",
                "coin_Set_00.brlan",
                "coin_Set_01.brlan",
                "coin_Set_02.brlan",
                "coin_Set_03.brlan",
                "coin_Set_coin_00.brlan",
                "coin_Set_coin_01.brlan",
                "coin_Set_coin_03.brlan",
            };

            const char *s_AnimResource01[ANIM_NUM] = {
                "VO_cockpit_02_coin_Coin_ani_00.brlan",
                "VO_cockpit_02_coin_Coin_ani_01.brlan",
                "VO_cockpit_02_coin_Coin_ani_02.brlan",
                "VO_cockpit_02_coin_Coin_ani_03.brlan",
                "VO_cockpit_02_coin_Set_00.brlan",
                "VO_cockpit_02_coin_Set_01.brlan",
                "VO_cockpit_02_coin_Set_02.brlan",
                "VO_cockpit_02_coin_Set_03.brlan",
                "VO_cockpit_02_coin_Set_coin_00.brlan",
                "VO_cockpit_02_coin_Set_coin_01.brlan",
                "VO_cockpit_02_coin_Set_coin_03.brlan",
            };

            const char *s_AnimResource02[ANIM_NUM] = {
                "VO_cockpit_03_coin_Coin_ani_00.brlan",
                "VO_cockpit_03_coin_Coin_ani_01.brlan",
                "VO_cockpit_03_coin_Coin_ani_02.brlan",
                "VO_cockpit_03_coin_Coin_ani_03.brlan",
                "VO_cockpit_03_coin_Set_00.brlan",
                "VO_cockpit_03_coin_Set_01.brlan",
                "VO_cockpit_03_coin_Set_02.brlan",
                "VO_cockpit_03_coin_Set_03.brlan",
                "VO_cockpit_03_coin_Set_coin_00.brlan",
                "VO_cockpit_03_coin_Set_coin_01.brlan",
                "VO_cockpit_03_coin_Set_coin_03.brlan",
            };

            // Null-terminated in the original.
            const char **s_AnimResource[] = {
                s_AnimResource00,
                s_AnimResource01,
                s_AnimResource02,
                0,
            };

            // Pane names, shared by all three layouts. The per-team tables
            // below index into it.
            const char *s_PaneName[] = {
                "coin_L",           // 0
                "coin_R",           // 1
                "tn_t_coin_L",      // 2
                "tn_t_coin_L02",    // 3
                "tn_t_coin_R",      // 4
                "tn_t_coin_R02",    // 5
                "coin_L_ani",       // 6
                "coin_R_ani",       // 7
                "L_null",           // 8
                "R_null",           // 9
                "coin_L_vis",       // 10
                "coin_R_vis",       // 11
                "DO_coin",          // 12
                "VO_BA_HO_coin",    // 13
                "BA_HO_te",         // 14
                "VO_te",            // 15
                "L_pw_up_g",        // 16
                "R_pw_up_g",        // 17
            };

            const unsigned char s_CoinPane[TEAM_NUM] = {0, 1};
            const unsigned char s_CoinDispPane[TEAM_NUM] = {10, 11};
            const unsigned char s_CoinAnimPane[TEAM_NUM] = {6, 7};
            const unsigned char s_Coin3DPane[TEAM_NUM] = {8, 9};
            const unsigned char s_CoinTelopPane[TEAM_NUM] = {16, 17};

            // Telop parent panes.
            const unsigned char PANE_TELOP_ATTACH = 14;
            const unsigned char PANE_TELOP_ATTACH_VOLLEY = 15;

        }

        const char *GetLayoutResource(E_COINLAYOUTID nId)
        {
            return s_LayoutResource[nId];
        }

        const char **GetLayoutAnimResource(E_COINLAYOUTID nId)
        {
            return s_AnimResource[nId];
        }

        long GetLayoutAnimResourceNum(E_COINLAYOUTID nId)
        {
            return ANIM_NUM;
        }

        const char **GetLayoutPaneNameTable(E_COINLAYOUTID nId)
        {
            return s_PaneName;
        }

        long GetTeamNum()
        {
            return TEAM_NUM;
        }

        unsigned char GetCoinPane(long nTeam)
        {
            return s_CoinPane[nTeam];
        }

        unsigned char GetCoinDispPane(long nTeam)
        {
            return s_CoinDispPane[nTeam];
        }

        unsigned char GetCoinAnimPane(long nTeam)
        {
            return s_CoinAnimPane[nTeam];
        }

        unsigned char GetCoin3DPane(long nTeam)
        {
            return s_Coin3DPane[nTeam];
        }

        unsigned char GetCoinTelopPane(long nTeam)
        {
            return s_CoinTelopPane[nTeam];
        }

        // Moves each team's counter display so that it sits at the height
        // of pane nPane, measured in its parent's frame.
        void SetCoinPosition(CMenuPaneAccessor *pAccessor, unsigned char nPane)
        {
            if (pAccessor == 0) {
                return;
            }

            nw4r::lyt::Pane *pBase = pAccessor->GetPane(nPane);
            if (pBase == 0) {
                return;
            }

            nw4r::math::VEC3 basePos = pBase->GetTranslate();
            for (long i = 0; i < GetTeamNum(); i++) {
                nw4r::lyt::Pane *pDisp = pAccessor->GetPane(GetCoinDispPane(i));
                if (pDisp != 0) {
                    nw4r::math::VEC3 pos = pDisp->GetTranslate();
                    nw4r::math::VEC3 parentPos = pDisp->GetParent()->GetTranslate();
                    pos.y = basePos.y - parentPos.y;
                    pDisp->SetTranslate(pos);
                }
            }
        }

        unsigned char GetTelopAttachPane(long nMode)
        {
            return nMode == 1 ? PANE_TELOP_ATTACH_VOLLEY : PANE_TELOP_ATTACH;
        }

    }
}
