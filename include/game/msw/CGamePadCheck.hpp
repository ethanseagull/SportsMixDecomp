#ifndef CGAMEPADCHECK_HPP_
#define CGAMEPADCHECK_HPP_

namespace msw {

    // Size 0xC8 (what Initialize allocates). Only the members used so far.
    class CGamePadCheck {
        public:
            static void Initialize();
            static void Finalize();
            static CGamePadCheck *GetInstance();
            // Added for MenuF game/menu/CInitGuide.cpp; static (the body
            // reads the instance from a global).
            static void CleanUp();

            // (DOL .sbss) The step of the pad check; msw::CMenu leaves its
            // start-up state only while it is below 2. (type guessed)
            static long m_PadCheckStap;

            unsigned char m_Unk00[0xAC];
            long m_UnkAC; // 0xAC, set to 1 once at start-up
            unsigned char m_UnkB0[0x18];
    };
}

#endif /* !CGAMEPADCHECK_HPP_ */
