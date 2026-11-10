#include "game/msw/cockpit/CSPGaugeInputDisp.hpp"

namespace msw {
    namespace cockpit {

        void CSPGaugeInputDisp::Initialize()
        {
            m_Frame0 = 0;
            m_Alpha = 0xFF;
        }

        void CSPGaugeInputDisp::Finalize()
        {
        }

        void CSPGaugeInputDisp::SetParam(unsigned char kind, unsigned short frame0, unsigned char alpha,
                                         unsigned short frame1, unsigned short frame2)
        {
            m_Kind = kind;
            m_Frame0 = frame0;
            m_Alpha = alpha;
            m_Frame1 = frame1;
            m_Frame2 = frame2;
        }
    }
}
