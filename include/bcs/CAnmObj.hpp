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
    // Size 0x0C. The material animations (bcs::CAnmMatClr, CAnmTexSrt,
    // CAnmTexPat) add a second base at 0x0C through bcs::CAnmObjMat. The
    // constructor and destructor are inline: the derived destructors call
    // bcs::CG3dObj's directly and store no vtable of this class.
    class CAnmObj : public CG3dObj {
        public:
            // Bit 2 of m_nAnmFlag. (name guessed) Cleared when the material
            // table is released and by Create.
            enum {
                ANMFLAG_4 = 4
            };

            CAnmObj() : m_nAnmFlag(0) {}
            virtual ~CAnmObj() {}
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

            // (names guessed) Inline in MenuF's msw::CChrMenuCtrl::CreateMdlObj,
            // which sets bit 3 of the word at 0x08 after Create.
            void OnFlag(unsigned long flag) { m_nAnmFlag |= flag; }

        protected:
            void OffAnmFlag(unsigned long flag) { m_nAnmFlag &= ~flag; }

        private:
            // 0x08 (name guessed). The class ends here: the 0x34-byte
            // material animations add bcs::CAnmObjMat's second base at 0x0C.
            unsigned long m_nAnmFlag;
    };
}

#endif /* !BCS_CANMOBJ_HPP_ */
