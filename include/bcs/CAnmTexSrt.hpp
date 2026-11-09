#ifndef BCS_CANMTEXSRT_HPP_
#define BCS_CANMTEXSRT_HPP_

#include "bcs/CAnmObj.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp"

namespace bcs {

    class CMdlObj;

    // Texture matrix animation. Size 0x34 (the modules allocate it). A DOL
    // class.
    class CAnmTexSrt : public CAnmObj {
        public:
            CAnmTexSrt();
            virtual ~CAnmTexSrt();

            // Returns 1 on success. The last two parameters are not
            // understood; msw::CTableSetMdlObj passes 0 for both.
            int Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmTexSrt res, int, long);
    };
}

#endif /* !BCS_CANMTEXSRT_HPP_ */
