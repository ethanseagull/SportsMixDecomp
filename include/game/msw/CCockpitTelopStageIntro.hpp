#ifndef CCOCKPITTELOPSTAGEINTRO_HPP_
#define CCOCKPITTELOPSTAGEINTRO_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // The stage introduction telop. Lives in the DOL; only the factory is
    // declared so far (return type and argument names guessed).
    //
    // (added for DodgeF game/dodge/CCockpitDodge.cpp) Size 0x1F4, derives
    // CCockpitLayoutBase; nType picks the telop (1 to 4 in DodgeF). The
    // members past CCockpitLayoutBase are not mapped.
    class CCockpitTelopStageIntro : public CCockpitLayoutBase {
        public:
            CCockpitTelopStageIntro(long nId, float fSortZ, long nType);
            virtual ~CCockpitTelopStageIntro();
            virtual int Set();
            virtual void Remove();

            // Makes the telop for object nObjId.
            static CMenuLinkBase *CreateObj(long nObjId, float fSortZ);

        private:
            unsigned char m_Unk60[0x1F4 - 0x60];
    };
}

#endif /* !CCOCKPITTELOPSTAGEINTRO_HPP_ */
