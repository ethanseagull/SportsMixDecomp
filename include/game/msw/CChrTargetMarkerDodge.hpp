#ifndef MSW_CCHRTARGETMARKERDODGE_HPP_
#define MSW_CCHRTARGETMARKERDODGE_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CMdlObj;
}

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

    // Size 0xA8: bcs::CTask at 0x00, bcs::CFsm<CChrArtsTargetMarkerDodge>
    // at 0x34 (vptr at 0x78), own members from 0x7C. The virtuals after the
    // CFsm part are in the order of __vt__Q23msw25CChrArtsTargetMarkerDodge;
    // DodgeF's CC14_DODGE::CcNinjaCloneMarker (game/dodge/CcNinjaCloneBody.cpp)
    // overrides the first two.
    class CChrArtsTargetMarkerDodge : public bcs::CTask, public bcs::CFsm<CChrArtsTargetMarkerDodge> {
        public:
            CChrArtsTargetMarkerDodge(bcs::CTask *pParent, long nChr, int nType);
            virtual ~CChrArtsTargetMarkerDodge();
            virtual void Main();

        protected:
            // 2: show, 3: hide (as the ninja's clone marker answers; the
            // meaning of the others is not known).
            virtual int CheckAppear_();
            // Where the marker goes.
            virtual void GetTargetPos_(bcs::FVEC3 &rPos);
            virtual void MoveToTargetPos_(const bcs::FVEC3 &rPos, float);
            virtual void UpdateColor_();

            long m_nChr;           // 0x7C, the character's index (constructor argument)
            bcs::CMdlObj *m_pMdl;  // 0x80, the marker's model
            unsigned char m_Unk84[0xA8 - 0x84];
    };
}

#endif /* !MSW_CCHRTARGETMARKERDODGE_HPP_ */
