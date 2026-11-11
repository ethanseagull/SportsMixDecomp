#ifndef MSW_CBBOXOBJ_HPP_
#define MSW_CBBOXOBJ_HPP_

#include "bcs/CScnObj.hpp"
#include "nw4r/math/types.h"

namespace msw {

    // A box in the scene whose screen rectangle can be read back (MenuF).
    // Size 0x14C (layout.py). Defined in game/menu/CTopMenuChrObj.cpp;
    // argument names are guesses. Its vtable is bcs::CScnObj's (67 words)
    // with the destructor and ObjProc replaced.
    class CBBoxObj : public bcs::CScnObj {
        public:
            CBBoxObj();
            virtual ~CBBoxObj();
            virtual void ObjProc(unsigned long, unsigned long, void *);

            // The box in the object's space; each pair is sorted.
            void SetSize(float fMinX, float fMaxX, float fMinY, float fMaxY, float fMinZ, float fMaxZ);
            // The box on the screen; 1 when it is valid.
            long GetResult(nw4r::math::AABB &rBox) const;

        private:
            // Projects the box with view nView's camera (ObjProc task 4).
            void CalcView_(long nView, const bcs::FMTX34 *pViewMtx);

            nw4r::math::AABB m_Box;    // 0x118
            nw4r::math::AABB m_Result; // 0x130, on the screen
            long m_bValid;             // 0x148, m_Result has been computed
    };
}

#endif /* !MSW_CBBOXOBJ_HPP_ */
