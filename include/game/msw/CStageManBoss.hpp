#ifndef MSW_CSTAGEMANBOSS_HPP_
#define MSW_CSTAGEMANBOSS_HPP_

#include "game/msw/CStageMan.hpp"

namespace msw {

    // The stage of the boss game (BossF). It adds no members to CStageMan
    // (size 0x1FC) and only fixes the dimensions of the court.
    class CStageManBoss : public CStageMan {
        public:
            CStageManBoss(bcs::CTask *pParent) : CStageMan(pParent) {}
            virtual ~CStageManBoss();

            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual float GetBaseCourtRadius_() const;

            // Creates the stage manager and loads stage `stageNo`.
            static CStageManBoss *CreateInstance(bcs::CTask *pParent, long stageNo);
    };
}

#endif /* !MSW_CSTAGEMANBOSS_HPP_ */
