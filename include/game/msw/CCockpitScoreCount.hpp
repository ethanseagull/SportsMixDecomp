#ifndef CCOCKPITSCORECOUNT_HPP_
#define CCOCKPITSCORECOUNT_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x98. A score count telop. Lives in the DOL; only the
    // constructor is declared so far (argument names guessed). The members
    // past CCockpitLayoutBase are not mapped.
    class CCockpitScoreCount : public CCockpitLayoutBase {
        public:
            CCockpitScoreCount(long nId, float fSortZ);
            virtual ~CCockpitScoreCount();
            virtual int Set();
            virtual void Remove();

        private:
            unsigned char m_Unk60[0x98 - 0x60];
    };
}

#endif /* !CCOCKPITSCORECOUNT_HPP_ */
