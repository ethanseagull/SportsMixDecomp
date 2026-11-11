#ifndef BCS_CWORLDENV_HPP_
#define BCS_CWORLDENV_HPP_

namespace bcs {

    // Only the members used so far. Both functions are static: their bodies
    // in the DOL take the view number in r3.
    class CWorldEnv {
        public:
            static void SetViewFovy(long view, float fovy);
            static float GetViewFovy(long view);
            // How many views the screen is split into. Return type guessed.
            static long GetViewSplitNum();

            // 1 selects the second set of camera data in the tournament
            // (msw::CTournamentCamera::GetDataMode). Type guessed.
            static long m_vatViewAspectType;
            // (added for HockeyF CChrManHockey::SetCamIdAssign_) How the
            // screen is split: 1 and 2 two views, 3 four views (guessed from
            // the camera tables chosen by it). Type guessed.
            static long m_vstViewSplitType;
    };
}

#endif /* !BCS_CWORLDENV_HPP_ */
