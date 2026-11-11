#ifndef CCOCKPITTELOPSTAGEINTRO_HPP_
#define CCOCKPITTELOPSTAGEINTRO_HPP_

namespace msw {
    class CMenuLinkBase;

    // The stage introduction telop. Lives in the DOL; only the factory is
    // declared so far (return type and argument names guessed).
    class CCockpitTelopStageIntro {
        public:
            // Makes the telop for object nObjId.
            static CMenuLinkBase *CreateObj(long nObjId, float fSortZ);
    };
}

#endif /* !CCOCKPITTELOPSTAGEINTRO_HPP_ */
