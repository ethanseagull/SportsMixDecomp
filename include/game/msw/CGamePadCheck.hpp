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

            // Added for MenuF game/menu/CEntry.cpp (return types and
            // argument names guessed). The battery warning, and the window
            // asking for the right controllers: three message names and
            // the number of players.
            void EnableBatteryCheck(int bEnable);
            void RemoveControllerTypeErrorMessage();
            void WindowControllerError(char *pGroup, char *pLabel0, char *pLabel1, long nPlayerNum, long nUnk);
            void WindowControllerErrorClose();
            void SetDispCursorFrameCount(long nCount);

            // (DOL .sbss) The step of the pad check; msw::CMenu leaves its
            // start-up state only while it is below 2. (type guessed)
            static long m_PadCheckStap;

            unsigned char m_Unk00[0x38];
            // 0x38. (added for game/menu/CTitle.cpp) Cleared when the title
            // screen opens, set to 1 when A is pressed on it.
            long m_Unk38;
            // 0x3C. (added for game/menu/CEntry.cpp) Set to the number of
            // players when the entry screen closes, to 1 when it goes back.
            long m_Unk3C;
            unsigned char m_Unk40[0x6C];
            long m_UnkAC; // 0xAC, set to 1 once at start-up
            unsigned char m_UnkB0[0x18];
    };
}

#endif /* !CGAMEPADCHECK_HPP_ */
