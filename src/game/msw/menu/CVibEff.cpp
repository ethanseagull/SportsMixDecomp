#include "game/msw/menu/CVibEff.hpp"
#include "game/msw/CGameVib.hpp"

// NOTE: only the EU slice of this file. Its 0.0f constant is shared with
// CSndBGM before it and CVibEff::MainLoop after it, so the real file is
// wider.

namespace msw {
    namespace menu {

        void CVibEff::Initialize()
        {
            m_bEnable = 1;
            m_fWait[0] = 0.0f;
            m_fWait[1] = 0.0f;
            m_fWait[2] = 0.0f;
            m_fWait[3] = 0.0f;
            m_fInterval = 15.0f;
        }

        void CVibEff::Finalize()
        {
            m_bEnable = 0;
        }

        void CVibEff::Play(long nPattern, long nUnk, unsigned long nPadMask)
        {
            if (m_bEnable) {
                for (int i = 0; i < 4; i++) {
                    if ((nPadMask & (1 << i)) && m_fWait[i] == 0.0f) {
                        CGameVib::Start(0, nPattern, nUnk, i);
                        m_fWait[i] = m_fInterval;
                    }
                }
            }
        }

        void CVibEff::Stop(unsigned long nPadMask)
        {
            for (int i = 0; i < 4; i++) {
                if (nPadMask & (1 << i)) {
                    CGameVib::Stop(i);
                }
            }
        }
    }
}
