#ifndef MSW_CGENPRERENDGRP_HPP_
#define MSW_CGENPRERENDGRP_HPP_

#include "bcs/CScnGroup.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CMdlObj;
}

namespace msw {

    // A scene group that renders a model into a texture (DOL class). Size
    // 0x164 (msw::CMiiRenderBase allocates that much). Only what MenuF's
    // game/menu/CTopMenuChrObj.cpp uses; names of the members and of the
    // constructor's arguments are guesses.
    class CGenPrerendGrp : public bcs::CScnGroup {
        public:
            CGenPrerendGrp(bcs::CMdlObj *pMdl, long nWidth, long nHeight, int);

            // The rendered image.
            void *GetTexImg() const { return m_pTexImg; }
            void SetMode(long nMode) { m_nMode = nMode; }
            // The camera the model is rendered with.
            void SetCameraPos(const bcs::FVEC3 &rPos) { m_vCameraPos = rPos; }
            void SetCameraTarget(const bcs::FVEC3 &rTarget) { m_vCameraTarget = rTarget; }

        private:
            unsigned char m_Unk124[0x4];  // 0x124
            void *m_pTexImg;              // 0x128
            unsigned char m_Unk12C[0x8];  // 0x12C
            long m_nMode;                 // 0x134
            bcs::FVEC3 m_vCameraPos;      // 0x138
            bcs::FVEC3 m_vCameraTarget;   // 0x144
            unsigned char m_Unk150[0x14]; // 0x150
    };
}

#endif /* !MSW_CGENPRERENDGRP_HPP_ */
