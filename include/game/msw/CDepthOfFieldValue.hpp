#ifndef CDEPTHOFFIELDVALUE_HPP_
#define CDEPTHOFFIELDVALUE_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    // Moves the depth-of-field setting of the renderer from one value to
    // another over a number of frames. Size 0x40, one static instance.
    class CDepthOfFieldValue {
        public:
            // (names guessed) What bcs::CRenderMan::SetDepthOfFieldParam takes.
            // What the three floats mean is not known; the effect is off
            // while the second one is not above zero.
            struct Param {
                long m_Type;    // 0x00
                float m_Value0; // 0x04
                float m_Value1; // 0x08
                float m_Value2; // 0x0C
            };

            CDepthOfFieldValue()
            {
            }

            void Finalize();
            void Main();

            Param m_Start;    // 0x00
            Param m_End;      // 0x10
            Param m_Cur;      // 0x20
            float m_Duration; // 0x30
            float m_Elapsed;  // 0x34
            float m_Speed;    // 0x38, elapsed time per frame

            virtual ~CDepthOfFieldValue();

            static CDepthOfFieldValue s_Instance;
    };
}

#endif /* !CDEPTHOFFIELDVALUE_HPP_ */
