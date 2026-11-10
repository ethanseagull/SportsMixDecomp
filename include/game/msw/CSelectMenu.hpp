#ifndef CSELECTMENU_HPP_
#define CSELECTMENU_HPP_

#include "game/msw/CMenuTask.hpp"
#include "game/msw/CSave.hpp"
#include "revolution/rfl.h"

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

            // Added for MenuF game/menu/CScoreTop.cpp (return types guessed
            // where the callers drop the result).
            void SetNextMode(long nMode, CSelectMenuLayoutBase *pObj);
            void PlayMenuBGM(long nBgm);
            void SetWhiteFadeOut(float fFadeTime);
            // The steps of the score book's first page (SelectMenu::CScoreTop).
            void ScoreTop_In();
            void ScoreTop_Loop();
            void ScoreTop_Out();

            // Added for MenuF game/menu/CPlayMode.cpp (return types guessed
            // where the callers drop the result).
            float GetCameraSpeed(long nAnim);
            void InitCharaSelectParam();
            // The steps of the play mode screen (SelectMenu::CPlayMode).
            void PlayMode_In();
            void PlayMode_Loop();
            void PlayMode_Loop0();
            void PlayMode_ChangeStateInit(long nMode);
            void PlayMode_Loop1();
            void PlayMode_Out();

            // Added for MenuF game/menu/CTeam.cpp (return types guessed).
            void ChangeTeamCharaSelectParam();
            // The steps of the team screen (SelectMenu::CTeam).
            void Team_In();
            void Team_Loop();
            void Team_Loop0();
            void Team_ChangeStateInit(long nMode);
            void Team_Loop1();
            void Team_Out();

            // Added for MenuF game/menu/CPlusMinus.cpp (defined there; the
            // return types and argument names are guesses). Whether pad
            // nPad uses a cursor (either of two flags of CMenuPadInfo).
            int _GetUsedCursol(long nPad);
            // Gives player nPad the Mii of the star mode, or the default
            // character outside it.
            void _SetStarMii(long nPad);
            void Mii_SelectParam(long nPad, unsigned char nUnk, long nIndex, RFLDataSource source,
                                 RFLCreateID createId);
            void Chara_SetSelectChara(long nPad, long nChara, long nUnk, int bUnk);

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
            unsigned char m_Unk25C[0x8];     // 0x25C
        public:
            // 0x264. (name guessed; added for game/menu/CScoreTop.cpp) A
            // saved m_nGameKind: the score book keeps the game kind it was
            // entered with here and puts it back on leaving.
            long m_nSavedGameKind;
        public:
            // (names guessed; added for game/menu/CPlayMode.cpp)
            unsigned char m_nPlayerNum;      // 0x268, 1 to 4
            unsigned char m_Unk269;          // 0x269
            unsigned char m_nPlayMode;       // 0x26A, from _GetPlayModeFromButtonId
            unsigned char m_Unk26B;          // 0x26B
            unsigned char m_Unk26C;          // 0x26C, cleared when going to mode 0xD
                                             // (game/menu/CTeam.cpp: the index of
                                             // m_nPlayMode in its team group)
            unsigned char m_Unk26D[0xE];     // 0x26D
        public:
            unsigned char m_Unk27B;          // 0x27B
        private:
            unsigned char m_Unk27C[0x427];   // 0x27C
        public:
            // 0x6A3 to 0x6B3. (names guessed; added for
            // game/menu/CPlusMinus.cpp) The Mii _SetStarMii gives the
            // other pads, passed to Mii_SelectParam.
            unsigned char m_nStarMiiUnk;     // 0x6A3
            long m_nStarMiiIndex;            // 0x6A4
            RFLDataSource m_StarMiiSource;   // 0x6A8
            RFLCreateID m_StarMiiCreateId;   // 0x6AC
        private:
            unsigned char m_Unk6B4[0x5C];    // 0x6B4
        public:
            CSave::PlayerData *m_pPlayerData; // 0x710, CSave::GetPlayerData(-1)
        private:
            unsigned char m_Unk714[0x248];   // 0x714
        public:
            // 0x95C. (name guessed; added for game/menu/CTeam.cpp) Frames
            // left before the team screen takes stick input again.
            float m_fInputWait;
        private:
            unsigned char m_Unk960[0x28];    // 0x960
        public:
            // 0x988. (name guessed; added for game/menu/CPlusMinus.cpp)
            // Non-zero when _SetStarMii gives Miis.
            long m_bStarMii;
        private:
            unsigned char m_Unk98C[0x74];    // 0x98C
        public:
            long m_UnkA00;                   // 0xA00
        private:
            unsigned char m_UnkA04[0xAB8 - 0xA04]; // 0xA04
    };

    // Defined in the same MenuF file as CSelectMenu. The text of message
    // pLabel in message group pGroup. Added for game/menu/CInitGuide.cpp.
    const wchar_t *GetMessageLabelString(const char *pGroup, const char *pLabel);

    // (added for game/menu/CTeam.cpp) Returns
    // CSelectMenuLayoutBase::s_DefaultOutAnimOverTime.
    float _GetModeChangeWaitTime();

    // Defined in another MenuF file (added for game/menu/CPlayMode.cpp; the
    // argument names are guesses). The play mode of button nBtnKindId of
    // the play mode screen for nPlayerNum players, or a negative value.
    long _GetPlayModeFromButtonId(long nBtnKindId, long nPlayerNum);
}

#endif /* !CSELECTMENU_HPP_ */
