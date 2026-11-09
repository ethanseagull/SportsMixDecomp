#ifndef CMENUPADINFO_HPP_
#define CMENUPADINFO_HPP_

namespace msw {

    // Size 0x50 (the size of s_Instance). Lives in the DOL. Only the member
    // used so far.
    class CMenuPadInfo {
        public:
            // (name guessed; no symbol) The pad that leads the menus. The
            // DOL's own SelectPadLeader(int) returns the same word.
            long GetPadLeader() const { return m_nPadLeader; }

            static CMenuPadInfo s_Instance;

        private:
            unsigned char m_Unk00[0x44]; // 0x00
            long m_nPadLeader;           // 0x44 (name guessed)
            unsigned char m_Unk48[8];    // 0x48
    };
}

#endif /* !CMENUPADINFO_HPP_ */
