#ifndef BCS_CFRAMECTRL_HPP_
#define BCS_CFRAMECTRL_HPP_

namespace bcs {

    // Frame counter of an animation, embedded in the animation objects at
    // 0x18. A DOL class; the name and member names are guessed. SetFrame
    // (0x80006F20) wraps the frame into [0, m_fEndFrame) when bit 0 (loop) is
    // set and sets bits 1 and 2 when the end is passed.
    class CFrameCtrl {
        public:
            enum {
                FLAG_LOOP = 1
            };

            CFrameCtrl() : m_nFlag(0), m_fFrame(0.0f), m_fRate(1.0f), m_fEndFrame(0.0f) {}

            void SetFrame(float frame);

            void SetLoop(bool bLoop)
            {
                if (bLoop) {
                    m_nFlag |= FLAG_LOOP;
                } else {
                    m_nFlag &= ~FLAG_LOOP;
                }
            }

            unsigned long m_nFlag; // 0x00
            float m_fFrame;        // 0x04
            float m_fRate;         // 0x08
            float m_fEndFrame;     // 0x0C
    };
}

#endif /* !BCS_CFRAMECTRL_HPP_ */
