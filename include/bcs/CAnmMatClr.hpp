#ifndef BCS_CANMMATCLR_HPP_
#define BCS_CANMMATCLR_HPP_

#include "bcs/CAnmObjMat.hpp"
#include "nw4r/g3d/ResAnmClr.hpp"
#include "nw4r/g3d/ResMdl.hpp"

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

            // 0x50 to 0x5C, as in bcs::CAnmTexSrt (names guessed; only
            // ChangeRes is called so far, by VolleyF's
            // CC15_VOLLEY::CcSiromaMagicCircleObj::Main). The result type of
            // GetResult is left open.
            virtual const void *GetResult(void *pResult, unsigned long idx);
            virtual int Bind(nw4r::g3d::ResMdl mdl);
            virtual int ChangeRes(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmClr res);
            virtual void Release();

        private:
            // (name guessed) Detaches the animation and frees what Create
            // allocated; the same as bcs::CAnmTexSrt's.
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

            // Laid out like CAnmTexSrt and CAnmTexPat, whose constructors
            // and destructors are copies of this class's.
            nw4r::g3d::ResAnmClr m_Res;  // 0x28
            nw4r::g3d::ClrAnmResult *m_pResult; // 0x2C, one per animated material
            unsigned long m_nResultNum;  // 0x30
    };
}

#endif /* !BCS_CANMMATCLR_HPP_ */
