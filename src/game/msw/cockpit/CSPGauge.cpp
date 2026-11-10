#include "game/msw/cockpit/CSPGauge.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"

namespace msw {
    namespace cockpit {

        void CSPGauge::Initialize()
        {
            m_nPane = 0xFFFF;
            m_nUnk02 = 0xFFFF;
            m_nAnim = 0xFF;
            m_nMaxAnim = 0xFF;
            m_fFrameMax = 10.0f;
            m_fRate = 0.0f;
        }

        void CSPGauge::Finalize()
        {
        }

        void CSPGauge::SetParam(bcs::G2D::CPaneCtrl *pPaneCtrl, unsigned short nPane, unsigned short nUnk02,
                                unsigned short nSubPane, unsigned char nAnim, unsigned char nMaxAnim,
                                unsigned char nSubAnim, unsigned char nSubMaxAnim)
        {
            if (pPaneCtrl == 0) {
                return;
            }

            m_nPane = nPane;
            m_nUnk02 = nUnk02;
            m_nSubPane = nSubPane;
            m_nAnim = nAnim;
            m_nMaxAnim = nMaxAnim;
            m_nSubAnim = nSubAnim;
            m_nSubMaxAnim = nSubMaxAnim;

            float fFrameMax = pPaneCtrl->GetAnimFrameMax(nAnim);
            if (fFrameMax > 0.0f) {
                m_fFrameMax = fFrameMax;
            }
        }

        void CSPGauge::SetGaugeAnim(CMenuPaneAccessor *pAccessor, float fRate)
        {
            if (m_nPane == 0xFFFF) {
                return;
            }

            float fFrame = fRate;
            if (fRate > 1.0f) {
                fFrame = 1.0f;
            } else if (fRate < 0.0f) {
                fFrame = 0.0f;
            }
            fFrame *= m_fFrameMax;
            m_fRate = fRate;
            pAccessor->PlayAnimation(m_nPane, m_nAnim, 0.0f, fFrame, 1, 1);
        }

        // Outside the EU .text slice (0x801BC144, in the gap after it).
        void CSPGauge::SetGaugeMaxAnim(CMenuPaneAccessor *pAccessor, int bMax)
        {
            if (bMax) {
                pAccessor->PlayAnimation(m_nPane, m_nMaxAnim, 1.0f, 0.0f, 1, 1);
                pAccessor->PlayAnimation(m_nSubPane, m_nSubMaxAnim, 1.0f, 0.0f, 0, 1);
            } else {
                SetGaugeAnim(pAccessor, m_fRate);
                pAccessor->PlayAnimation(m_nSubPane, m_nSubAnim, 1.0f, 0.0f, 0, 1);
            }
        }
    }
}
