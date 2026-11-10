#ifndef MSW_CBALLHOCKEY_HPP_
#define MSW_CBALLHOCKEY_HPP_

#include "game/msw/CBall.hpp"

namespace msw {

    // The ball of the hockey game (HockeyF), size 0x484 (the allocation in
    // CBallManHockey::Create_): CBall plus 0x16C bytes of its own.
    //
    // INCOMPLETE: only what game/hockey/CBallManHockey.cpp needs to create one.
    // The class's virtual functions (322 vtable words) and members are not
    // declared yet.
    class CBallHockey : public CBall {
        public:
            CBallHockey(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallHockey();

        private:
            unsigned char m_Unk318[0x16C]; // 0x318
    };
}

#endif /* !MSW_CBALLHOCKEY_HPP_ */
