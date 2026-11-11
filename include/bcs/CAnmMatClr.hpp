#ifndef BCS_CANMMATCLR_HPP_
#define BCS_CANMMATCLR_HPP_

#include "bcs/CAnmObjMat.hpp"
#include "nw4r/g3d/ResAnmClr.hpp"

namespace bcs {

    class CMdlObj;

    // Material colour animation. Size 0x34 (the modules allocate it). A DOL
    // class.
    class CAnmMatClr : public CAnmObjMat {
        public:
            CAnmMatClr();
            virtual ~CAnmMatClr();

            // Returns 1 on success. The last two parameters are not
            // understood; msw::CTableSetMdlObj passes 0 for both.
            int Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmClr res, int, long);

        private:
            // Laid out like CAnmTexSrt and CAnmTexPat, whose constructors
            // and destructors are copies of this class's.
            nw4r::g3d::ResAnmClr m_Res;  // 0x28
            void *m_pResult;             // 0x2C, one per animated material
            unsigned long m_nResultNum;  // 0x30
    };
}

#endif /* !BCS_CANMMATCLR_HPP_ */
