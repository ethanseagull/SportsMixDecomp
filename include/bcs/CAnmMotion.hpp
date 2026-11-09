#ifndef BCS_CANMMOTION_HPP_
#define BCS_CANMMOTION_HPP_

#include "bcs/CAnmObj.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"

namespace bcs {

    // Skeletal animation, the object in a model's first animation slot. A
    // DOL class, larger than bcs::CAnmObj (its constructor writes up to
    // 0x48) and probably derived through bcs::CAnmObjMotBase, which is not
    // modelled. Only what the modules call is declared, so the class must
    // not be created from a module yet.
    class CAnmMotion : public CAnmObj {
        public:
            CAnmMotion();
            virtual ~CAnmMotion();

            // The three floats are passed on unchanged in the DOL; their
            // names are guessed from msw::CMenuMdlTableSet::PlayImpl.
            void Play(nw4r::g3d::ResAnmChr res, unsigned long flag, float rate, float blend, float frame);
            // Clears bit 0 of the word at 0x18.
            void Stop();
    };
}

#endif /* !BCS_CANMMOTION_HPP_ */
