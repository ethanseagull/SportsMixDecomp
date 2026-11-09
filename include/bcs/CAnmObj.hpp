#ifndef BCS_CANMOBJ_HPP_
#define BCS_CANMOBJ_HPP_

#include "bcs/CG3dObj.hpp"

namespace bcs {

    // An animation attached to a bcs::CMdlObj. A DOL class without a single
    // named virtual function: the slots are read from the vtable of
    // bcs::CAnmMatClr, and the names and return types are guessed from the
    // bodies and from the callers (msw::CTableSetMdlObj::IsAnmLooped and
    // IsAnmEnd in MenuF call 0x38 and 0x40).
    //
    // The object also has a second vptr at 0x0C (another base). That is not
    // modelled, so this class must not be constructed or destroyed inline.
    class CAnmObj : public CG3dObj {
        public:
            virtual ~CAnmObj();
            virtual void ObjProc(unsigned long, unsigned long, void *); // 0x1C
            virtual void SetFrame(float frame);                         // 0x20 (name guessed) takes a float
            virtual float GetFrame() const;                             // 0x24, reads the float at 0x1C
            virtual float VFunc28() const;                              // 0x28, reads the float at 0x24
            virtual void SetRate(float rate);                           // 0x2C, writes the float at 0x20
            virtual float GetRate() const;                              // 0x30, reads the float at 0x20
            virtual void VFunc34();                                     // 0x34 (guessed)
            virtual int IsLooped() const;                               // 0x38, bit 1 of the word at 0x18
            virtual int VFunc3C() const;                                // 0x3C, bit 2
            virtual int IsEnd() const;                                  // 0x40, bit 3

        private:
            // 0x08 to 0x34. Where this class ends and bcs::CAnmMatClr and
            // bcs::CAnmTexSrt (both 0x34 bytes) begin is not known.
            unsigned char m_Unk08[0x2C];
    };
}

#endif /* !BCS_CANMOBJ_HPP_ */
