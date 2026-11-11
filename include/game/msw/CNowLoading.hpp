#ifndef CNOWLOADING_HPP_
#define CNOWLOADING_HPP_

namespace msw {

    // Only the members used so far. The one instance is kept in the DOL's
    // sbss (lbl_805C0C48); SetType and SetDisp do nothing without it.
    class CNowLoading {
        public:
            // Static: the body in the DOL stores into the global instance.
            static void SetType(long);
            // Added for MenuF game/menu/CMenu.cpp: shows or hides the
            // loading screen.
            static void SetDisp(int bDisp);

        private:
            static CNowLoading *s_pInstance; // (name guessed) lbl_805C0C48

            unsigned char m_Unk00[0x3C]; // 0x00
            long m_nType;                // 0x3C (name guessed)
            unsigned char m_Unk40[4];    // 0x40
            float m_fTime;               // 0x44 (name guessed) SetType clears it
    };
}

#endif /* !CNOWLOADING_HPP_ */
