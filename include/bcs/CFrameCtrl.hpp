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
                FLAG_LOOP = 1,
                // (names guessed; added for the motion classes) Set by
                // SetFrame: the frame wrapped (kept until cleared), wrapped
                // in this call, and stopped at an end of a non-looping
                // animation.
                FLAG_LOOPED = 2,
                FLAG_WRAP = 4,
                FLAG_END = 8
            };

            CFrameCtrl() : m_nFlag(0), m_fFrame(0.0f), m_fRate(1.0f), m_fEndFrame(0.0f) {}

            void SetFrame(float frame);

            // (DOL 0x80007088) Not inline: other files of the DOL call it
            // (bcs::CCameraAnm at 0x80012EB4, code at 0x80014100 and
            // 0x801FE408); defined in CAnmMotion.cpp right after SetFrame.
            void SetLoop(bool bLoop);

            unsigned long m_nFlag; // 0x00
            float m_fFrame;        // 0x04
            float m_fRate;         // 0x08
            float m_fEndFrame;     // 0x0C
    };
}

#endif /* !BCS_CFRAMECTRL_HPP_ */
