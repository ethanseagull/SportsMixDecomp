#ifndef CGAMEPADCHECK_HPP_
#define CGAMEPADCHECK_HPP_

namespace msw {

    // Size 0xC8 (what Initialize allocates). Only the members used so far.
    class CGamePadCheck {
        public:
            static void Initialize();
            static void Finalize();
            static CGamePadCheck *GetInstance();

            unsigned char m_Unk00[0xAC];
            long m_UnkAC; // 0xAC, set to 1 once at start-up
            unsigned char m_UnkB0[0x18];
    };
}

#endif /* !CGAMEPADCHECK_HPP_ */
