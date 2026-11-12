#ifndef BCS_CMDLREF_HPP_
#define BCS_CMDLREF_HPP_

#include "bcs/CMdlCopyBase.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {

    // A model drawn with the pose of another (SetLink), mirrored in Y. A DOL
    // class, size 0x1D8 (all members are CMdlCopyBase's), vtable
    // lbl_80467944.
    class CMdlRef : public CMdlCopyBase {
        public:
            CMdlRef();
            // Builds the model linked to pMdl at once. (meanings of the last
            // three arguments not known; msw::CItemBall passes 0 for all)
            CMdlRef(const CMdlObj *pMdl, nw4r::g3d::ResMdl mdl, unsigned long, unsigned long, long);
            virtual ~CMdlRef();

            // (meanings of the last two arguments not known) Non-zero when
            // the model was built.
            int Create(nw4r::g3d::ResMdl mdl, unsigned long, long);

            // Copies the node matrices of pSrc mirrored in Y (scale 1, -1,
            // 1); slot 0x118.
            virtual void CopyNodeMtx(const CMdlObj *pSrc);

        private:
            // (name guessed) Swaps front and back culling of every material,
            // since the mirrored model turns its faces inside out. Inline in
            // both constructor and Create.
            void FlipCullMode();
    };
}

#endif /* !BCS_CMDLREF_HPP_ */
