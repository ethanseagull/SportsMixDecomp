#ifndef CTMSCRIPT_HPP_
#define CTMSCRIPT_HPP_

namespace msw {

    // The tournament's rule scripts (TourF game/tour/CTmScript.cpp). Only the
    // members TournamentRule calls; return types and parameter names
    // guessed.
    class CTmScript {
        public:
            void SetupRule(long nGameMode, long nRule, long, long nLevel, int);
            // Picks the stage; returns it.
            long SetupExcludeStage(long nStage, long nExclude, long nLevel, int);
            void SetupPanel(long nPanel, long nLevel, int);
            void SetupItemParam(long nItem, long nLevel, int);
    };
}

#endif /* !CTMSCRIPT_HPP_ */
