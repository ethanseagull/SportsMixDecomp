#ifndef MSW_CBALLBOSS_HPP_
#define MSW_CBALLBOSS_HPP_

#include "game/msw/CBall.hpp"

namespace msw {

    // The ball of the boss game (BossF), size 0x448: CBall plus 0x130 bytes
    // of its own (read from the constructor).
    //
    // INCOMPLETE: only what game/boss/CBallManBoss.cpp needs to create one.
    // The class overrides most of CBall's virtual functions and adds eight
    // (slots 0x4F4 to 0x514 of __vt__Q23msw9CBallBoss); none of that is
    // declared yet, and the members are not mapped.
    class CBallBoss : public CBall {
        public:
            CBallBoss(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallBoss();

        private:
            unsigned char m_Unk318[0x130]; // 0x318
    };
}

#endif /* !MSW_CBALLBOSS_HPP_ */
