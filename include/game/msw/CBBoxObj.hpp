#ifndef MSW_CBBOXOBJ_HPP_
#define MSW_CBBOXOBJ_HPP_

#include "bcs/CScnObj.hpp"

namespace nw4r {
    namespace math {
        struct AABB;
    }
}

namespace msw {

    // A box in the scene whose screen rectangle can be read back (MenuF).
    // Size 0x14C (layout.py). Only the members msw::SelectMenu::CBgChr
    // uses; argument names are guesses.
    class CBBoxObj : public bcs::CScnObj {
        public:
            CBBoxObj();
            virtual ~CBBoxObj();
            virtual void ObjProc(unsigned long, unsigned long, void *);
            // Declared only to make the class concrete: CBBoxObj's vtable
            // (MenuF, 67 words) ends before the slot bcs::CScnObj.hpp gives
            // it. Never called through this header.
            virtual void UpdateFrame(float);

            void SetSize(float fMinX, float fMaxX, float fMinY, float fMaxY, float fMinZ, float fMaxZ);
            // The box on the screen; 1 when it is valid.
            long GetResult(nw4r::math::AABB &rBox) const;

        private:
            unsigned char m_Unk118[0x34]; // 0x118
    };
}

#endif /* !MSW_CBBOXOBJ_HPP_ */
