#ifndef BCS_CANMTEXPAT_HPP_
#define BCS_CANMTEXPAT_HPP_

#include "bcs/CAnmObjMat.hpp"
#include "nw4r/g3d/ResAnmTexPat.hpp"

namespace bcs {

    class CMdlObj;

    // Texture pattern animation. Size 0x34 (msw::CItemBall allocates it). A DOL class.
    class CAnmTexPat : public CAnmObjMat {
        public:
            CAnmTexPat();
            // Constructs and calls Create with the same arguments.
            CAnmTexPat(const CMdlObj *pMdl, nw4r::g3d::ResAnmTexPat res, int, long);
            virtual ~CAnmTexPat();

            // Returns 1 on success. The last two parameters are not
            // understood; msw::CItemBall passes 0 for both.
            int Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmTexPat res, int, long);

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

            nw4r::g3d::ResAnmTexPat m_Res;               // 0x28
            nw4r::g3d::TexPatAnmResult *m_pResult;      // 0x2C, one per animated material
            unsigned long m_nResultNum;              // 0x30
    };
}

#endif /* !BCS_CANMTEXPAT_HPP_ */
