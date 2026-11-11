#ifndef BCS_CANMTEXSRT_HPP_
#define BCS_CANMTEXSRT_HPP_

#include "bcs/CAnmObjMat.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp"

namespace bcs {

    class CMdlObj;

    // Texture matrix animation. Size 0x34 (the modules allocate it). A DOL class.
    class CAnmTexSrt : public CAnmObjMat {
        public:
            CAnmTexSrt();
            virtual ~CAnmTexSrt();

            // Returns 1 on success. The last two parameters are not
            // understood; msw::CTableSetMdlObj passes 0 for both.
            int Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmTexSrt res, int, long);

        private:
            // (name guessed) Detaches the animation and frees what Create
            // allocated.
            void Destroy()
            {
                DetachFromParent();
                if (m_pResult != 0) {
                    delete[] m_pResult;
                }
                m_pResult = 0;
                m_nResultNum = 0;
                ResetFrame();
                ReleaseMatTable();
            }

            nw4r::g3d::ResAnmTexSrt m_Res;               // 0x28
            nw4r::g3d::TexSrtAnmResult *m_pResult;      // 0x2C, one per animated material
            unsigned long m_nResultNum;              // 0x30
    };
}

#endif /* !BCS_CANMTEXSRT_HPP_ */
