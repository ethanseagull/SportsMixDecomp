#ifndef CGAMEEXDEMOMASKOBJ_HPP_
#define CGAMEEXDEMOMASKOBJ_HPP_

#include "bcs/CScnObj.hpp"

namespace msw {

    // A scene object made by msw::CGameExDemoBase (DOL, size 0x120: the
    // size passed to operator new there; vtable lbl_8048D444). Only the
    // members seen so far. The constructor is inline (CGameExDemoBase's
    // constructor stores its vtable and fields directly).
    class CGameExDemoMaskObj : public bcs::CScnObj {
        public:
            CGameExDemoMaskObj() : m_fPosRate(1.0f), m_nUnk11C(1) {}
            virtual ~CGameExDemoMaskObj();                          // fn_80254720
            // 0x0C: returns 1 (fn_80255994, emitted weak after
            // CGameExDemoBase's functions).
            virtual long GetObjType() const { return 1; }
            virtual void ObjProc(unsigned long, unsigned long, void *); // 0x1C, fn_8025478C
            // TODO: the original vtable is 0x10C bytes (no slot 0x10C) and
            // slots 0x10-0x18 and 0x54-0x108 hold overrides (fn_800269xx,
            // fn_8003B5xx; fn_8003B574 is slot 0x108), so this class likely
            // derives from an unknown class between it and bcs::CScnObj.
            // Declared only so the class is not abstract; not defined yet.
            virtual void UpdateFrame(float);

            // Stores rate clamped to [0, 1] and n.
            void SetPosRate(float rate, int n);

            // (name guessed; fn_8025486C) Draws the captured screen split in
            // two halves slid apart by the rate, for ObjProc task 7.
            void Draw();

        private:
            float m_fPosRate; // 0x118
            int m_nUnk11C;    // 0x11C
    };
}

#endif /* !CGAMEEXDEMOMASKOBJ_HPP_ */
