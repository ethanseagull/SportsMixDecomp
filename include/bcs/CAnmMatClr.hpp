#ifndef BCS_CANMMATCLR_HPP_
#define BCS_CANMMATCLR_HPP_

#include "bcs/CAnmObj.hpp"
#include "nw4r/g3d/ResAnmClr.hpp"

namespace bcs {

    class CMdlObj;

    // Material colour animation. Size 0x34 (the modules allocate it). A DOL
    // class.
    class CAnmMatClr : public CAnmObj {
        public:
            CAnmMatClr();
            virtual ~CAnmMatClr();

            // Returns 1 on success. The last two parameters are not
            // understood; msw::CTableSetMdlObj passes 0 for both.
            int Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmClr res, int, long);
    };
}

#endif /* !BCS_CANMMATCLR_HPP_ */
