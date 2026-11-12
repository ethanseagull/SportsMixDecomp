#ifndef BCS_CMDLCLONE_HPP_
#define BCS_CMDLCLONE_HPP_

#include "bcs/CMdlCopyBase.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {

    // (added for BasketF's CChrCoreBasketNinja) A model drawn as a copy of
    // another. A DOL class, allocated with 0x1D8 bytes like bcs::CMdlRef;
    // that it derives from CMdlCopyBase is a guess from that size. Only what
    // the modules use is declared.
    class CMdlClone : public CMdlCopyBase {
        public:
            // (meanings of the last three arguments not known; the ninja
            // passes 0 for all)
            CMdlClone(const CMdlObj *pMdl, nw4r::g3d::ResMdl mdl, unsigned long, unsigned long, long);
            virtual ~CMdlClone();

            // (name guessed) Sets bits of the word at 0x1CC; the ninja's
            // copies set 0x40000 when they start to vanish.
            // (CMdlCopyBase::m_nLinkFlag, at 0x1CC)
            void OnCloneFlag(unsigned long nBit) { m_nLinkFlag |= nBit; }
    };
}

#endif /* !BCS_CMDLCLONE_HPP_ */
