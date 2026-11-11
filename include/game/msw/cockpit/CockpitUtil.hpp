#ifndef MSW_COCKPIT_COCKPITUTIL_HPP_
#define MSW_COCKPIT_COCKPITUTIL_HPP_

namespace bcs {
    namespace G2D {
        class CPaneCtrl;
    }
}

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace msw {
    class CChrCore;
    class CMenuPaneAccessor;

    namespace cockpit {

        // Free helpers of the cockpits. Live in the DOL (0x801DFE40); the
        // file name is a guess.

        // Added for BossF game/boss/CCockpitBossCoder.cpp: the leader of
        // team nTeam. (It only branches to CChrMan::GetTeamLeader.)
        CChrCore *GetTeamLeaderChrCore(long nTeam);

        // Added for VolleyF game/volley/CCockpitVolleyCoder.cpp (return type
        // and argument names guessed): gives text pane pPane of layout
        // pLayoutName in pPaneCtrl the font resource pFontName.
        void SetTextFont(bcs::G2D::CPaneCtrl *pPaneCtrl, nw4r::lyt::Pane *pPane, const char *pLayoutName,
                         const char *pFontName);

        // Added for HockeyF game/hockey/CCockpitHockeyCoder.cpp (argument
        // names guessed): the special-shot telop of pane nPane; the four
        // panes after it are the telop's parts. The coder passes -1.0f to
        // hide it.
        void SetSPGaugeTelop(CMenuPaneAccessor *pAccessor, long nPane, long nPane1, long nPane2, long nPane3,
                             long nPane4, float fRate);
    }
}

#endif /* !MSW_COCKPIT_COCKPITUTIL_HPP_ */
