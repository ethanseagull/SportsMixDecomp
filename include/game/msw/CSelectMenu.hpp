#ifndef CSELECTMENU_HPP_
#define CSELECTMENU_HPP_

#include "game/msw/CMenuTask.hpp"
#include "game/msw/CSave.hpp"

namespace msw {
    class CSelectMenuLayoutBase;

    // The task behind the SelectMenu screens (MenuF). Its constructor calls
    // CMenuTask's. Only the members used so far; the two name getters are
    // non-static and neither reads `this`. The class has a second
    // polymorphic part (its destructor has a `@68@` thunk) that is not
    // written here, so do not create or destroy one through this header.
    class CSelectMenu : public CMenuTask {
        public:
            CSelectMenu(bcs::CTask *pParent);
            virtual ~CSelectMenu();

            const char *GetArchiveFileName(long nArchive);
            const char *GetArchiveRootFolderName(long nArchive);
            // Added for MenuF game/menu/CEntryExplain.cpp. The archive id
            // of mode nMode from a byte table; the current mode when nMode
            // is negative.
            unsigned char GetLoadArchiveFileId(long nMode);

            // Added for MenuF game/menu/CSelectMenuLayoutBase.cpp (return
            // type guessed).
            int IsBackButtonMove(long nPad);

            // Added for MenuF game/menu/CInitGuide.cpp.
            void SetNextMode(long nMode, float fFadeTime);
            void SetBlockFadeOut(float fFadeTime);
            // The steps of the start-up guide screen.
            void InitGuide_In();
            void InitGuide_Loop();
            void InitGuide_Out();

            // Added for MenuF game/menu/CHowTo.cpp (return types guessed
            // where the callers drop the result).
            void SetFadeIn(float fFadeTime);
            void ChangeLeaderPad(long nPad);
            void SetPadDefault();
            long GetPrevMode();
            void GoBackToMode(long nPad, long nMode, CSelectMenuLayoutBase *pObj, long nUnk);
            void InputDataToMenuObj(CSelectMenuLayoutBase *pObj, long nUnk, long nPad, long nMode);
            void GameMode_SetTopChar(int nUnk);
            void GameMode_SetLineMessage(long nBtnKindId);
            void GameMode_ChangeStateInit(long nState);
            void GameMode_LoopStep1();
            void GameMode_LoopStep2();
            void GameMode_LoopStep100();
            void GameMode_LoopStep101();
            // The steps of the "how to play" screen (SelectMenu::CHowTo).
            void HowTo_In();
            void HowTo_Loop();
            void HowTo_LoopStep0();
            void HowTo_Out();

            // Data, added for the same unit. The class is 0xAB8 bytes (the
            // size layout.py finds); only two members are named so far.
            // The second polymorphic part mentioned above (a `@68@` thunk)
            // lies somewhere in the padding and is not modelled yet.
        private:
            // CMenuTask ends at 0x44 (see CMenuTask.hpp); this was written
            // as m_Unk4C[0x8] when CMenuTask was taken to be 0x4C bytes,
            // which put every member below 8 bytes low
            // (CSelectMenuLayoutBase::GetCurrentButtonKindId reads m_nMode
            // at 0x54 and the array at 0x9E). The `@68@` part may start here.
            unsigned char m_Unk44[0x10];     // 0x44

        public:
            // 0x54. (name guessed) The current screen: read by
            // GetPrevMode, GoBackToMode and most screen steps.
            long m_nMode;

        private:
            unsigned char m_Unk58[0x34];     // 0x58

        public:
            // (names guessed; added for game/menu/CHowTo.cpp)
            short m_nStep;                   // 0x8C, the sub-step of the screen's Loop
        private:
            unsigned char m_Unk8E[0xB];      // 0x8E
        public:
            unsigned char m_nNextMode;       // 0x99, passed to SetNextMode
        private:
            unsigned char m_Unk9A;           // 0x9A
        public:
            unsigned char m_Unk9B;           // 0x9B
        private:
            unsigned char m_Unk9C;           // 0x9C
        public:
            unsigned char m_Unk9D;           // 0x9D, compared with 4

        public:
            // 0x9E. (name guessed) The selected button kind id per pad and
            // mode (0x66 bytes per pad: 51 modes). Saved by Backup().
            unsigned short m_nCurrentBtnKindId[4][51];

        private:
            unsigned char m_Unk236[0x8];     // 0x236

        public:
            // (names guessed; added for game/menu/CHowTo.cpp)
            unsigned char m_Unk23E;          // 0x23E, compared with 1
        private:
            unsigned char m_Unk23F[0x19];    // 0x23F
        public:
            // 0x258. 0 to 3: picks the colour animation of the how-to
            // screen and is passed to CTournament::Tutorial.
            long m_nGameKind;
        private:
            unsigned char m_Unk25C[0x1F];    // 0x25C
        public:
            unsigned char m_Unk27B;          // 0x27B
        private:
            unsigned char m_Unk27C[0x494];   // 0x27C
        public:
            CSave::PlayerData *m_pPlayerData; // 0x710, CSave::GetPlayerData(-1)
        private:
            unsigned char m_Unk714[0x2EC];   // 0x714
        public:
            long m_UnkA00;                   // 0xA00
        private:
            unsigned char m_UnkA04[0xAB8 - 0xA04]; // 0xA04
    };

    // Defined in the same MenuF file as CSelectMenu. The text of message
    // pLabel in message group pGroup. Added for game/menu/CInitGuide.cpp.
    const wchar_t *GetMessageLabelString(const char *pGroup, const char *pLabel);
}

#endif /* !CSELECTMENU_HPP_ */
