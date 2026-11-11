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
            // 0x10C: the slot holds fn_8003B574 (body not looked at yet).
            virtual void UpdateFrame(float);

            // Stores rate clamped to [0, 1] and n.
            void SetPosRate(float rate, int n);

        private:
            float m_fPosRate; // 0x118
            int m_nUnk11C;    // 0x11C
    };
}

#endif /* !CGAMEEXDEMOMASKOBJ_HPP_ */
