#ifndef CGAMEEXDEMOBASE_HPP_
#define CGAMEEXDEMOBASE_HPP_

#include "bcs/CTask.hpp"

namespace bcs {
    class CCamera;
}

namespace msw {

    class CGameExDemoMaskObj;

    // Base of the game-explanation demos (DOL, size 0x54, vptr at 0;
    // vtable lbl_8048D420). DodgeF and VolleyF derive from it. The virtuals
    // are declared in vtable order. Names marked guessed have no exported
    // name.
    class CGameExDemoBase : public bcs::CTask {
        public:
            CGameExDemoBase(bcs::CTask *pParent);
            virtual ~CGameExDemoBase();
            virtual void Main();
            virtual void Disp() {}
            // Whether the demo may go to its next step (step 1 only).
            virtual int CanbeNext();
            // Goes to the next step if CanbeNext allows it.
            virtual void SetNext();
            // 0x20 (name guessed, fn_80255178): called first by Main; empty
            // here.
            virtual void MainProc();

        protected:
            enum { CAMERA_NUM = 2 };

            long m_nStep;                          // 0x34 (name guessed)
            unsigned long m_Unk38;                 // 0x38
            float m_fUnk3C;                        // 0x3C
            CGameExDemoMaskObj *m_pMask;           // 0x40 (name guessed)
            // One camera and view angle per view (names guessed): Main sets
            // view i's fovy while camera i exists.
            bcs::CCamera *m_apCamera[CAMERA_NUM];  // 0x44
            float m_afFovy[CAMERA_NUM];            // 0x4C
    };
}

#endif /* !CGAMEEXDEMOBASE_HPP_ */
