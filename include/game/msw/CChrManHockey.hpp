#ifndef MSW_CCHRMANHOCKEY_HPP_
#define MSW_CCHRMANHOCKEY_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrMan.hpp"

namespace msw {

    class CNpcBossPackun;

    // INCOMPLETE: the character manager of the hockey game (HockeyF). No
    // members are declared, so it must not be instantiated.
    class CChrManHockey : public CChrMan {
        public:
            // Makes the boss Packun nIdx at rPos (CStageHockey07::CreateGoal).
            // (return type guessed from the stage keeping the result, as
            // CChrManBasket's does)
            CNpcBossPackun *CreateBossPackun_(const bcs::FVEC3 &rPos, long nIdx);

            // The instance as this class (guessed inline: the callers call
            // CChrMan::GetInstance and use the result directly).
            static CChrManHockey *GetInstance() { return static_cast<CChrManHockey *>(CChrMan::GetInstance()); }
    };
}

#endif /* !MSW_CCHRMANHOCKEY_HPP_ */
