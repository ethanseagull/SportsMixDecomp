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

            // 0x44 to 0x98: slots past those of bcs::CAnmObj, read from the
            // DOL vtable of bcs::CAnmMotion, where none has a name. They are
            // probably bcs::CAnmObjMotBase's. Most forward to the motion
            // selected by the word at 0x2C. Only 0x64 and 0x98 are called
            // from a module so far (msw::SelectMenu::CCourtCharaObj::
            // StateCheck in MenuF, both with (3, 0)); the others are
            // placeholders that keep the slots in place.
            virtual void VFunc44();
            virtual void VFunc48();
            virtual void VFunc4C();
            virtual void VFunc50();
            virtual void VFunc54();
            virtual void VFunc58();
            virtual void VFunc5C();
            virtual void VFunc60();
            virtual long VFunc64(long, unsigned long);  // 0x64, compared with 2 by the caller
            virtual void VFunc68();
            virtual void VFunc6C();
            virtual void VFunc70();
            virtual void VFunc74();
            virtual void VFunc78();
            virtual void VFunc7C();
            virtual void VFunc80();
            virtual void VFunc84();
            virtual void VFunc88();
            virtual void VFunc8C();
            virtual void VFunc90();
            virtual void VFunc94();
            virtual int VFunc98(long, unsigned long);   // 0x98, tested for nonzero by the caller

            // The three floats are passed on unchanged in the DOL; their
            // names are guessed from msw::CMenuMdlTableSet::PlayImpl.
            void Play(nw4r::g3d::ResAnmChr res, unsigned long flag, float rate, float blend, float frame);
            // The same for an animation already loaded, by index.
            void Play(long idx, unsigned long flag, float rate, float blend, float frame);
            // Clears bit 0 of the word at 0x18.
            void Stop();
    };
}

#endif /* !BCS_CANMMOTION_HPP_ */
