#ifndef CCOCKPITMENUBASEBUTTONA_HPP_
#define CCOCKPITMENUBASEBUTTONA_HPP_

#include "game/msw/CCockpitMenuBase.hpp"

namespace msw {

    // Size 0x9C. A menu that is answered with the A button. Lives in the
    // DOL. It has no destructor of its own: the one in its vtable only calls
    // CCockpitMenuBase's and stores no vtable.
    class CCockpitMenuBaseButtonA : public CCockpitMenuBase {
        public:
            CCockpitMenuBaseButtonA(long nId, float fSortZ, long nButtonNum, long nWindowNum, long nPartsNum);
            virtual void StepIn();
            virtual void StepLoop();
            virtual void StepOut();
            virtual void InitMenuPadParameter(unsigned char);

        protected:
            unsigned long m_Unk98; // 0x98, the constructor stores 0x10
    };
}

#endif /* !CCOCKPITMENUBASEBUTTONA_HPP_ */
