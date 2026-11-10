#ifndef BCS_CANMTEXPAT_HPP_
#define BCS_CANMTEXPAT_HPP_

#include "bcs/CAnmObj.hpp"
#include "nw4r/g3d/ResAnmTexPat.hpp"

namespace bcs {

    class CMdlObj;

    // Texture pattern animation. Size 0x34 (msw::CItemBall allocates it). A
    // DOL class.
    class CAnmTexPat : public CAnmObj {
        public:
            CAnmTexPat();
            virtual ~CAnmTexPat();
            // Returns 1 on success, like CAnmTexSrt::Create. The last two
            // parameters are not understood; msw::CItemBall passes 0 for both.
            int Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmTexPat res, int, long);
    };
}

#endif /* !BCS_CANMTEXPAT_HPP_ */
