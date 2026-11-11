#ifndef BCS_CCAPTURE_HPP_
#define BCS_CCAPTURE_HPP_

#include "revolution/gx/GXTexture.h"

namespace bcs {

    // A copy of the frame buffer kept as a texture. A DOL class; only what
    // msw::CGameExDemoMaskObj (game/msw/CGameExDemoBase.cpp) uses is
    // written: it loads the texture object at 0xC.
    class CCapture {
        public:
            // The capture nIndex, or null.
            static CCapture *GetInstance(long nIndex);

            // (name guessed)
            const GXTexObj *GetTexObj() const { return &m_TexObj; }

        private:
            unsigned char m_Unk00[0xC];
            GXTexObj m_TexObj; // 0x0C
    };
}

#endif /* !BCS_CCAPTURE_HPP_ */
