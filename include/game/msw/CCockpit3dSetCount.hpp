#ifndef CCOCKPIT3DSETCOUNT_HPP_
#define CCOCKPIT3DSETCOUNT_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0xF4. The sets won, counted in 3D. Lives in the DOL; only what
    // DodgeF's CCockpitDodge calls (argument names guessed). The members
    // past CCockpitLayoutBase are not mapped.
    class CCockpit3dSetCount : public CCockpitLayoutBase {
        public:
            CCockpit3dSetCount(long nId, float fSortZ);
            virtual ~CCockpit3dSetCount();
            virtual int Set();
            virtual void Remove();

            void SetSetCountData(long, long, long, long, long); // return type guessed

        private:
            unsigned char m_Unk60[0xF4 - 0x60];
    };
}

#endif /* !CCOCKPIT3DSETCOUNT_HPP_ */
