#ifndef CCOCKPIT3DSCORECOUNT_HPP_
#define CCOCKPIT3DSCORECOUNT_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x130. A score counted up in 3D. Lives in the DOL; only what
    // BasketF's CCockpitBasket calls. The members past CCockpitLayoutBase
    // are not mapped.
    class CCockpit3dScoreCount : public CCockpitLayoutBase {
        public:
            // What StartScoreCount is given (0x38 bytes). Member names are
            // guesses; CCockpitBasket::SetTelop3DScoreCount fills the words
            // at 0x08 to 0x14 with 0, 0, 1 and 0.
            struct SET_PARAM {
                long nArg0;   // 0x00
                long nArg1;   // 0x04
                long nUnk08;  // 0x08
                long nUnk0C;  // 0x0C
                long nUnk10;  // 0x10
                long nUnk14;  // 0x14
                long nArg2;   // 0x18
                long nScore0; // 0x1C, never negative
                long nScore1; // 0x20, never negative
                long nArg5;   // 0x24
                long nArg6;   // 0x28
                long nArg7;   // 0x2C
                int bArg8;    // 0x30
                long nArg9;   // 0x34
            };

            CCockpit3dScoreCount(long nId, float fSortZ);
            virtual ~CCockpit3dScoreCount();
            virtual int Set();
            virtual void Remove();

            void StartScoreCount(const SET_PARAM &rParam); // return type guessed

        private:
            unsigned char m_Unk60[0x130 - 0x60];
    };
}

#endif /* !CCOCKPIT3DSCORECOUNT_HPP_ */
