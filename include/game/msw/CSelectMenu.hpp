#ifndef CSELECTMENU_HPP_
#define CSELECTMENU_HPP_

#include "game/msw/CMenuTask.hpp"
#include "game/msw/CSave.hpp"
#include "revolution/rfl.h"

namespace msw {
    class CSelectMenuLayoutBase;

    namespace dwc {
        class CGameDwc;
        class CFriend;
    }

    // The task behind the SelectMenu screens (MenuF). Its constructor calls
    // CMenuTask's. Only the members used so far; the two name getters are
    // non-static and neither reads `this`. The class has a second
    // polymorphic part (its destructor has a `@68@` thunk) that is not
    // written here, so do not create or destroy one through this header.
    // (msw::CMenu does `new CSelectMenu`: that only needs the size, 0xAB8,
    // and the out-of-line constructor.)
    class CSelectMenu : public CMenuTask {
        public:
            CSelectMenu(bcs::CTask *pParent);
            virtual ~CSelectMenu();
            // Overrides of bcs::CTask's (vtable slots 3 and 4).
            virtual void Main();
            virtual void Disp();

            // Added for MenuF game/menu/CMenu.cpp: start loading what the
            // select menu needs, and whether that has finished (return type
            // guessed).
            static void InitLoad();
            static int IsCompleteOfInitLoad();

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

            // Added for MenuF game/menu/CPlayerSelect.cpp. The steps of the
            // Wi-Fi player selection screen (SelectMenu::CPlayerSelect).
            void WifiPlayerSelect_In();
            void WifiPlayerSelect_Loop();
            void WifiPlayerSelect_Out();

            // Added for MenuF game/menu/CPlayerInfo.cpp. The steps of the
            // Wi-Fi player information window (SelectMenu::CPlayerInfo).
            void WifiPlayerInfo_In();
            void WifiPlayerInfo_Loop();
            void WifiPlayerInfo_Out();

            // Added for MenuF game/menu/CPartySelect.cpp. The steps of the
            // party game selection screen (SelectMenu::CPartySelect).
            void PartySelect_In();
            void PartySelect_Loop();
            void PartySelect_Loop0();
            void PartySelect_ChangeStateInit(long nMode);
            void PartySelect_Loop1();
            void PartySelect_Out();

            // Added for MenuF game/menu/CWiFiFrindVS.cpp (return types
            // guessed). The steps of the friend match screen
            // (SelectMenu::CWiFiFrindVS), and what they call.
            void FriendVS_In();
            void FriendVS_Loop();
            void FriendVS_Out();
            void CancelVsFriend();
            void FriendVSWait_SendSetStep(long nStep);

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

            // Added for MenuF game/menu/CTitle.cpp (return types and
            // argument names guessed). The steps of the title screen
            // (SelectMenu::CTitle), and what they call.
            void Title_In();
            void Title_Loop();
            void Title_Out();
            void Title_SetUpRandGame();
            void RemoveObj(long nObjId);
            // The music of screen nUnk, 0 for none.
            long GetMenuBGMId(long nUnk);
            void StopMenuBGM();
            void Chara_SetDecideChara(long nPad, long nChara, long nUnk0, long nUnk1, long nUnk2);
            // The index of a character pad nPad has not decided yet, or a
            // negative value when all are.
            long Chara_FindNotDecideCharaIndex(long nPad);
            void SetCharaSelectSaveParam();

            // Added for MenuF game/menu/CEntry.cpp (return types guessed).
            // The steps of the entry screen (SelectMenu::CEntry), offline
            // and on Wi-Fi, and what they call.
            void Entry_In();
            void Entry_Loop();
            void Entry_LoopStep0();
            void Entry_Out();
            void WifiEntry_In();
            void WifiEntry_Loop();
            void WifiEntry_LoopStep0();
            void WifiEntry_Out();
            void WifiEntry_PadEntry();
            void WifiEntry_ChangeStateInit();
            void Entry_NextStateExhibition(long nBtnKindId);
            void Entry_NextStateTournament(long nBtnKindId);
            void Entry_NextStateParty(long nBtnKindId);
            // 0 when pads 0 to nNum - 1 all have a remote, else -1.
            long Entry_CheckConnectPad(long nNum);
            void Entry_ChangeStateInit(long nMode);
            void Entry_LoopStep1();
            void Entry_LoopStep10();
            void Entry_LoopStep11();
            void Entry_LoopStep12();
            void Entry_LoopStep20();
            void Entry_LoopStep21();
            void Entry_LoopStep22();
            void Entry_LoopStep23();
            void ClearMenuHistory();
            void PushMenuHistory(unsigned char nUnk0, unsigned char nUnk1, unsigned short nUnk2);
            void _DisablePointStick(long nPad, int bUnk);

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
        public:
            // 0x9C. (added for game/menu/CPartySelect.cpp) Set to 1, 3, 5
            // or 7 when one of the four party games is chosen.
            unsigned char m_Unk9C;
            unsigned char m_Unk9D;           // 0x9D, compared with 4

        public:
            // 0x9E. (name guessed) The selected button kind id per pad and
            // mode (0x66 bytes per pad: 51 modes). Saved by Backup().
            unsigned short m_nCurrentBtnKindId[4][51];

        private:
            unsigned char m_Unk236[0x8];     // 0x236

        public:
            // 0x23E. (added for game/menu/CTitle.cpp; names guessed) Five
            // bytes per pad, written and copied as a whole by
            // CGamePadCheck.
            struct PadSelect {
                unsigned char m_Unk00;       // 0x00
                unsigned char m_Unk01;       // 0x01
                unsigned char m_Unk02;       // 0x02
                unsigned char m_Unk03;       // 0x03, set to 0x80 by Title_In
                unsigned char m_nPlayerNo;   // 0x04, from CMenuPadInfo::GetDefaultPlayerNo
            };

            union {
                // (names guessed; added for game/menu/CHowTo.cpp)
                unsigned char m_Unk23E;      // 0x23E, compared with 1
                PadSelect m_aPadSelect[4];   // 0x23E
            };
        private:
            unsigned char m_Unk252[0x6];     // 0x252
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
            unsigned char m_Unk26D;          // 0x26D
            // 0x26E to 0x271. (names guessed; added for
            // game/menu/CTitle.cpp) The stage of the attract demo's match,
            // drawn by Title_SetUpRandGame: m_nStage (0 to 3) first, then
            // m_nStageSub, an index into what _GetStageNoInfo returns for
            // it. Each has a copy of its old value.
            unsigned char m_nStageSub;       // 0x26E
            unsigned char m_nPrevStageSub;   // 0x26F
            unsigned char m_nStage;          // 0x270
            unsigned char m_nPrevStage;      // 0x271
            unsigned char m_Unk272[0x9];     // 0x272
        public:
            unsigned char m_Unk27B;          // 0x27B
        private:
            unsigned char m_Unk27C;          // 0x27C
        public:
            // (added for game/menu/CEntry.cpp; names guessed)
            // 0x27D. Set to 1 when a Wi-Fi entry goes on to the character
            // select screen.
            unsigned char m_Unk27D;
        private:
            unsigned char m_Unk27E[0x2];     // 0x27E
        public:
            // 0x280. The player number of each pad that has entered, 0xFF
            // for the others.
            unsigned char m_nPadPlayer[4];
        private:
            unsigned char m_Unk284[0x41F];   // 0x284
        public:
            // 0x6A3 to 0x6B3. (names guessed; added for
            // game/menu/CPlusMinus.cpp) The Mii _SetStarMii gives the
            // other pads, passed to Mii_SelectParam.
            unsigned char m_nStarMiiUnk;     // 0x6A3
            long m_nStarMiiIndex;            // 0x6A4
            RFLDataSource m_StarMiiSource;   // 0x6A8
            RFLCreateID m_StarMiiCreateId;   // 0x6AC
        private:
            unsigned char m_Unk6B4[0x18];    // 0x6B4
        public:
            // 0x6CC. (added for game/menu/CEntry.cpp; name guessed) Frames
            // the controller error window of the entry screen waits.
            float m_fEntryErrorWait;
        private:
            unsigned char m_Unk6D0[0x2C];    // 0x6D0
        public:
            // 0x6FC to 0x707. (names guessed; added for
            // game/menu/CWiFiFrindVS.cpp) The Wi-Fi connection, the friend
            // the friend match is with, and a copy of it kept by
            // FriendVS_In.
            dwc::CGameDwc *m_pGameDwc;       // 0x6FC
            dwc::CFriend *m_pVsFriend;       // 0x700
            dwc::CFriend *m_pVsFriendSaved;  // 0x704
        private:
            unsigned char m_Unk708[0x8];     // 0x708
        public:
            CSave::PlayerData *m_pPlayerData; // 0x710, CSave::GetPlayerData(-1)
        private:
            unsigned char m_Unk714[0x48];    // 0x714
        public:
            // 0x75C. (added for game/menu/CWiFiFrindVS.cpp) 0x1EC bytes
            // FriendVS_Loop clears before the friend match starts.
            unsigned char m_FriendVSWaitWork[0x1EC];
        private:
            unsigned char m_Unk948[0x14];    // 0x948
        public:
            // 0x95C. (name guessed; added for game/menu/CTeam.cpp) Frames
            // left before the team screen takes stick input again.
            float m_fInputWait;
            // (names guessed; added for game/menu/CTitle.cpp)
            // 0x960. Which opening movie the title screen plays next;
            // flipped between 0 and 1 each time.
            unsigned char m_nTitleMovie;
        private:
            unsigned char m_Unk961[0x3];     // 0x961
        public:
            // 0x964. Frames left before the title screen gives way to the
            // attract demo once its music has stopped.
            float m_fTitleDemoWait;
        private:
            unsigned char m_Unk968[0x20];    // 0x968
        public:
            // 0x988. (name guessed; added for game/menu/CPlusMinus.cpp)
            // Non-zero when _SetStarMii gives Miis.
            long m_bStarMii;
        private:
            unsigned char m_Unk98C[0x74];    // 0x98C
        public:
            long m_UnkA00;                   // 0xA00
        private:
            unsigned char m_UnkA04[0x58];    // 0xA04
        public:
            // 0xA5C. (added for game/menu/CEntry.cpp; name guessed) Set by
            // Entry_In when the camera is still moving in; cleared once
            // it has stopped.
            long m_bEntryCameraWait;
        private:
            unsigned char m_UnkA60[0x8];     // 0xA60
        public:
            // 0xA68. (added for game/menu/CCourtTab.cpp) When non-zero,
            // CCourt does not pass its events on to menu object 1.
            long m_UnkA68;
        private:
            unsigned char m_UnkA6C[0xC];     // 0xA6C
        public:
            // 0xA78. (added for game/menu/CWiFiFrindVS.cpp) Set by
            // FriendVS_Loop to whether m_Unk23E is 1 when a friend match
            // is accepted.
            long m_UnkA78;
        private:
            unsigned char m_UnkA7C[0xAB8 - 0xA7C]; // 0xA7C
    };

    // Defined in the same MenuF file as CSelectMenu. The text of message
    // pLabel in message group pGroup. Added for game/menu/CInitGuide.cpp.
    const wchar_t *GetMessageLabelString(const char *pGroup, const char *pLabel);
    // (added for game/menu/CPlayerSelect.cpp) The text of message nIndex
    // in message group pGroup.
    const wchar_t *GetMessageString(const char *pGroup, long nIndex);

    // (added for game/menu/CTeam.cpp) Returns
    // CSelectMenuLayoutBase::s_DefaultOutAnimOverTime.
    float _GetModeChangeWaitTime();

    // Defined in another MenuF file (added for game/menu/CPlayMode.cpp; the
    // argument names are guesses). The play mode of button nBtnKindId of
    // the play mode screen for nPlayerNum players, or a negative value.
    long _GetPlayModeFromButtonId(long nBtnKindId, long nPlayerNum);

    // Defined in another MenuF file (added for game/menu/CEntry.cpp; the
    // argument name is a guess). The play mode of a party game for
    // nPlayerNum players.
    long _GetPartyPlayType(long nPlayerNum);

    // Defined in another MenuF file (added for game/menu/CTitle.cpp; the
    // argument names are guesses). A table of stage ids for game kind
    // nGame; an entry of 0x10 is not playable.
    const unsigned char *_GetStageNoInfo(long nGame, long nIndex);
}

#endif /* !CSELECTMENU_HPP_ */
