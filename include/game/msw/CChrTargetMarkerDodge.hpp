#ifndef MSW_CCHRTARGETMARKERDODGE_HPP_
#define MSW_CCHRTARGETMARKERDODGE_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // INCOMPLETE: the target markers of the dodgeball game (DodgeF,
    // game/dodge/CChrArtsTargetMarkerDodge.cpp). Both are tasks with a state
    // machine (bcs::CFsm of their own class); only the constructors and the
    // sizes are known here (what CChrManDodge::CreateMarker_ allocates). The
    // destructors are declared so that the vtables are not emitted from here.
    // (parameter names guessed: the manager, the character, and 0 or 1)

    // Size 0xA4.
    class CChrTargetMarkerDodge : public bcs::CTask {
        public:
            CChrTargetMarkerDodge(bcs::CTask *pParent, long nChr, int nType);
            virtual ~CChrTargetMarkerDodge();
            virtual void Main();

        protected:
            unsigned char m_Unk34[0xA4 - 0x34];
    };

    // Size 0xA8.
    class CChrArtsTargetMarkerDodge : public bcs::CTask {
        public:
            CChrArtsTargetMarkerDodge(bcs::CTask *pParent, long nChr, int nType);
            virtual ~CChrArtsTargetMarkerDodge();
            virtual void Main();

        protected:
            unsigned char m_Unk34[0xA8 - 0x34];
    };
}

#endif /* !MSW_CCHRTARGETMARKERDODGE_HPP_ */
