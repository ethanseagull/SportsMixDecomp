#ifndef BCS_G2D_CPANEDL_HPP_
#define BCS_G2D_CPANEDL_HPP_

#include "nw4r/lyt/Pane.hpp"
#include "nw4r/math/types.h"

namespace bcs {
    class CScnObj;

    namespace G2D {

        // (class name guessed; added for DOL game/bcs/G2D/CLineStripDL.cpp)
        // The second base of the engine's display-list panes: something
        // drawn from a GD display list. Lives in the DOL. Its own vtable is
        // lbl_8046603C (out-of-line destructor fn_8002F1E4), built on a base
        // with vtable lbl_80466CC8 (constructor fn_800450D4, destructor
        // fn_80045120) that holds nine 12-byte entries from 0x00 and their
        // count at 0x6C; both are folded into this class until a unit needs
        // them. Its data comes before its first virtual, so the vptr is at
        // 0x74. The slot names are placeholders, except where the function
        // is read: 0x2C draws with a matrix (fn_8002F278), 0x30 sets the
        // GD current matrix to the identity slots (fn_8002F848). 0x28 is
        // pure.
        class CDLDrawBase {
            private:
                unsigned char m_Unk00[0x74];

            public:
                virtual ~CDLDrawBase();                                         // 0x08
                virtual void DLUnk0C();
                virtual void DLUnk10();
                virtual void DLUnk14();
                virtual void DLUnk18();
                virtual void DLUnk1C();
                virtual void DLUnk20();
                // 0x24: the derived pane's own (CPaneDL's one override serves
                // both bases, through the thunk at 0x8003B784).
                virtual const nw4r::ut::detail::RuntimeTypeInfo *GetRuntimeTypeInfo() const;
                // 0x28: draws the object (CPaneDL's is fn_80039CF4, reached
                // here through the thunk at 0x8003B774).
                virtual void DrawDLObj() = 0;
                virtual void DrawDL(const nw4r::math::MTX34 &rMtx, unsigned long nArg); // 0x2C
                virtual void SetCurrentMtxDL();                                 // 0x30

            private:
                // 0x78. Detached and handed to the delayed delete on
                // destruction.
                CScnObj *m_pScnObj;
        };

        // (class name guessed) The base of bcs::G2D::CLineStripDL and
        // bcs::G2D::CScissorDL: a layout pane that is also drawn from a
        // display list. Lives in the DOL: constructor fn_80039BE4,
        // destructor fn_80039C48, vtable lbl_80465C00. Size 0x170. Its
        // vtable is the pane's 27 slots, the second base's part, then two
        // more: 0xA8 (fn_80039CF4, DrawDLObj) draws a visible pane through
        // the second base and remembers the global z, 0xAC is pure and
        // writes the pane's display list (each derived class's).
        class CPaneDL : public nw4r::lyt::Pane, public CDLDrawBase {
            public:
                CPaneDL();
                virtual ~CPaneDL();

                virtual const nw4r::ut::detail::RuntimeTypeInfo *GetRuntimeTypeInfo() const;

                // Overrides the second base's, so it also gets its own slot
                // after that base's part (0xA8).
                virtual void DrawDLObj();
                virtual void MakeDL() = 0;   // 0xAC

            private:
                float m_fGlobalZ;                 // 0x154
                unsigned char m_Unk158[0x14];
                unsigned long m_Unk16C;           // 0x16C, passed to DrawDL
        };
    }
}

#endif /* !BCS_G2D_CPANEDL_HPP_ */
