#ifndef BCS_CWORLDENV_HPP_
#define BCS_CWORLDENV_HPP_

namespace bcs {

    // Only the members used so far. Both functions are static: their bodies
    // in the DOL take the view number in r3.
    class CWorldEnv {
        public:
            // How the screen is split into views. Named in the mangled names
            // of bcs::CRenderMan::ReqPushViewSplitType and
            // ReqPopViewSplitType; only 0 (taken to be one full-screen view)
            // is passed so far. (enumerator names guessed)
            enum ViewSplitType {
                VIEW_SPLIT_1
            };

            static void SetViewFovy(long view, float fovy);
            static float GetViewFovy(long view);
            // How many views the screen is split into. Return type guessed.
            static long GetViewSplitNum();

            // 1 selects the second set of camera data in the tournament
            // (msw::CTournamentCamera::GetDataMode). Type guessed.
            static long m_vatViewAspectType;
    };
}

#endif /* !BCS_CWORLDENV_HPP_ */
