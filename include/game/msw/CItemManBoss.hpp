#ifndef MSW_CITEMMANBOSS_HPP_
#define MSW_CITEMMANBOSS_HPP_

#include "game/msw/CItemMan.hpp"

namespace msw {

    // The item manager of the boss game (BossF). No members of its own. The
    // empty destructor is declared explicitly so that its weak copy is
    // emitted at its original place in the file.
    class CItemManBoss : public CItemMan {
        public:
            CItemManBoss(bcs::CTask *pParent) : CItemMan(pParent) {}
            virtual ~CItemManBoss() {}

            virtual CItem *CreateItem(long type, const bcs::FVEC3 &rPos);
            virtual void ExParamInitialize_();
    };
}

#endif /* !MSW_CITEMMANBOSS_HPP_ */
