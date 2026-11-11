#ifndef MSW_CNPCBEHEMOTHBOSS_HPP_
#define MSW_CNPCBEHEMOTHBOSS_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CNpcBase.hpp"

namespace msw {

    class CChrMan;

    // INCOMPLETE: the boss of the boss game (BossF), size 0x36C (what
    // CChrManBoss::Create_ allocates). Only the constructor is declared.
    class CNpcBehemothBoss : public CNpcBase {
        public:
            // (parameter names guessed) CChrManBoss::Create_ passes itself,
            // the start position and rotation, and party game parameter 0.
            CNpcBehemothBoss(CChrMan *pMan, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, long nParam);

        protected:
            unsigned char m_Unk188[0x36C - 0x188];
    };
}

#endif /* !MSW_CNPCBEHEMOTHBOSS_HPP_ */
