#ifndef MSW_CEVENTCHRCORE_HPP_
#define MSW_CEVENTCHRCORE_HPP_

#include "game/msw/CEventModel.hpp"

namespace msw {

    // A character model of an event scene (DOL class, 0xB4 bytes). Only
    // the constructor is declared so far.
    class CEventChrCore : public CEventModel {
        public:
            // (parameter names guessed) nType is the character
            // (CGame::GetCharType), nPlayer its player, pMotTable the
            // scene's motion table.
            CEventChrCore(long nType, long nPlayer, const short *pMotTable);

        private:
            unsigned char m_UnkB0[4]; // 0xB0
    };
}

#endif /* !MSW_CEVENTCHRCORE_HPP_ */
