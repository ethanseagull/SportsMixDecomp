#ifndef CSELECTMENU_HPP_
#define CSELECTMENU_HPP_

#include "game/msw/CMenuTask.hpp"
#include "game/msw/CSave.hpp"
#include "revolution/rfl.h"

namespace bcs {
    namespace dwc {
        class CDwcFriendList;
    }
}

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
            // (added for game/menu/CTournamentLoad.cpp) The other steps of
            // the game mode screen (SelectMenu::CMode) and of the window
            // that offers to continue a saved tournament
            // (SelectMenu::CTournamentLoad).
            void GameMode_In();
            void GameMode_Loop();
            void GameMode_LoopStep0();
            void GameMode_LoopStep30();
            void GameMode_Out();
            void GameMode_TournamentCheckConnectPad();
            void GameMode_TournamentBackCheck();
            // Defined in another MenuF file (return type guessed). The
            // option kind of the rules of the current game.
            long RuleSel_GetOptionKindId();
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

            // Added for MenuF game/menu/unit_4BB48.cpp (the game's file name
            // is SelectMenuWiFiWait.cpp; return types guessed where the
            // callers drop the result). The steps of the Wi-Fi match-making
            // screen (anybody match), and what they call.
            void WifiWait_In();
            void WifiWait_Loop();
            void WifiWait_Out();
            // Puts the player's rating in the current game kind (the mean
            // of the four for none) into m_MySingleData.
            void WifiWait_SetGameSkill();
            // Settles the game kind and the stage from both players' data.
            void WifiWait_GameModeStage();
            void WifiWait_SelectStage();
            // 1 when both players have sent the same stage; otherwise picks
            // one of the two, sends it and returns 0.
            long WifiWait_StageCheck();
            // 1 when every player has sent step nStep or a later one, 0 when
            // not yet, -1 when the match is lost.
            long WifiWait_Agreement(long nStep);
            // Sends m_MySingleData to the other player.
            void WifiWait_SendSetStep();
            // The trophy (0 to 4) of Wi-Fi rating nSkill.
            long CheckSkillToTrophy(short nSkill);
            // The steps of the friend match's set-up (after
            // SelectMenu::CWiFiFrindVS), and what they call.
            void FriendVSWait_In();
            void FriendVSWait_Loop();
            void FriendVSWait_Out();
            long FriendVSWait_Agreement(long nStep);
            // Shows the guest what the host is choosing.
            void FriendVSWait_Message();
            // (no symbols; names guessed) Inline in game/menu/unit_4BB48.cpp:
            // the parts of m_MySingleData both match-making screens fill
            // the same way, and the copy of both players' data into the
            // game once they agree.
            void WiFi_SetMyName();
            void WiFi_SetMy2P();
            void WiFi_SetMyRecord();
            void WiFi_SetMyChara();
            void WiFi_SetUpMatchPlayers();
            // Defined in other MenuF files (return types and argument names
            // guessed).
            long GetTeamPlayerNum();
            void WifiCourt_SetUpDisplay();
            void Court_EndCamera();
            // A random stage: its index and its sub-index.
            void Court_RandomSelect(long &rStage, long &rStageSub);
            // The second player's Mii and name in a two-player Wi-Fi game.
            RFLStoreData WiFi_Get2PMiiStoreData();
            wchar_t *WiFi_Get2PMiiName();
            // The Mii of character slot nIndex: its create id (returned as
            // the 8 bytes of an RFLCreateID) and its data source.
            unsigned long long Mii_GetMiiCreateId(long nIndex);
            RFLDataSource Mii_GetMiiDataSource(long nIndex);

            // Added for MenuF game/menu/CFriendList.cpp. The steps of the
            // friend roster screen (SelectMenu::CFriendList), and the help
            // line it shows for the selected row (the argument is not
            // used).
            void FriendList_In();
            void FriendList_Loop();
            void FriendList_LoopStep();
            void FriendList_Out();
            void FriendList_LineMessage(long nBtnKindId);

            // Added for MenuF game/menu/CWiFiGame.cpp (return types
            // guessed). The steps of the Wi-Fi game selection screen
            // (SelectMenu::CWiFiGame), the game and play mode button
            // nBtnKindId selects, and the steps of the Wi-Fi top screen.
            void WifiGame_In();
            void WifiGame_Loop();
            void WifiGame_Out();
            void WifiGame_SetPlayStyle(long nBtnKindId);
            void WifiTop_In();
            void WifiTop_Loop();
            void WifiTop_Out();

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

            // Added for MenuF game/menu/CCharaStatus.cpp (names guessed).
            // Size 0x28. What the character select screen holds for one of
            // its six character slots; the array is at 0x488.
            struct CharaSelectParam {
                long m_Unk00;                // 0x00, 0xFF with m_nPlayerKind for an empty slot
                long m_nPlayerKind;          // 0x04, 0 to 3 a pad, 4 the computer, 0xFF none
                long m_Unk08;                // 0x08, passed to CCharSel's pad sign calls
                long m_Unk0C;                // 0x0C
                unsigned char m_nChara;      // 0x10, 0 to 19, 0xFF none
                unsigned char m_nCharaSub;   // 0x11, the character's variant, 0xFF none
                unsigned char m_nState;      // 0x12, 1 and up once chosen
                unsigned char m_Unk13[0x15]; // 0x13
            };

            // Slot nIndex (0 to 5), or null.
            CharaSelectParam *GetCharaSelectParam(long nIndex);

            // Added for MenuF game/menu/CFriendCode.cpp (return types and
            // argument names guessed). The steps of the friend code entry
            // screen (SelectMenu::CFriendCode) and of the friend's
            // information window (SelectMenu::CFriendInfo), and what they
            // call.
            void FriendCode_In();
            void FriendCode_Loop();
            void FriendCode_LoopStep();
            void FriendCode_Out();
            void FriendPlayerInfo_In();
            void FriendPlayerInfo_Loop();
            void FriendPlayerInfo_Out();
            void FriendInfo_In();
            // The Wi-Fi message windows: message pLabel of group pGroup
            // with an A button, with Yes and No, with no button, and
            // closing the window. The texts fill parts of the message
            // (null: none).
            void WindowA_Button(char *pGroup, char *pLabel, wchar_t *pText0, wchar_t *pText1);
            void WindowYesNo_Button(char *pGroup, char *pLabel, int nUnk0, wchar_t *pText0, wchar_t *pText1,
                                    int nUnk1);
            void Window_Message(char *pGroup, char *pLabel, int nUnk);
            void Window_Close();
            // Writes friend code nCode as text into m_MessageWork and
            // returns it.
            wchar_t *ExchangeFriendCode(unsigned long long nCode);

            // Added for MenuF game/menu/CFriendInfo.cpp (return types
            // guessed). The rest of the friend's information window's steps,
            // the window of a friend who has not registered the player back
            // (SelectMenu::CNoFriendInfo), and what they call.
            void FriendInfo_Loop();
            void FriendInfo_LoopStep();
            void FriendInfo_Out();
            // Follows the cancel window shown while a friend match is being
            // set up (m_UnkA0C is its step).
            void FriendMatchChancelCheck();
            void NoFriendInfo_In();
            void NoFriendInfo_Loop();
            void NoFriendInfo_Main();
            void NoFriendInfo_Out();

            // Added for MenuF game/menu/unit_61C00.cpp (return types and
            // names guessed). The steps of the cup select screen
            // (SelectMenu::CCupSelect with its difficulty tabs,
            // SelectMenu::CCupSelectTab), and what they call.
            void Cup_In();
            void Cup_Loop();
            void Cup_Out();
            // 1 when the leader's pad moved the difficulty tab, else 0.
            int Cup_CheckTabChange();
            void Cup_ChangeStateInit(long nMode);
            // The number of difficulties open in the current game kind,
            // less one.
            long _DifficultyBitCount();
            // Enables the cup buttons of difficulty nLevel from the save.
            void _SetCupBit(long nLevel);
            // nCup when cup nCup of difficulty nLevel is open, else the
            // first open cup (0 for none).
            long _CheckCupBit(long nLevel, long nCup);
            void _TabChange(long nLevel);
            // Shows cup model nCup (0: none) on pMenu's cup stand.
            void _SetCupCharaObj(CSelectMenu *pMenu, long nCup);
            void Cup_OkButtonDelay();
            void Cup_OkButtonInit();
            void Cup_OkButton();
            void Cup_OkButtonOut();
            // Updates the background and the cup model for the button
            // the cursor is on.
            void _BgChangeDisp(long nBtnKindId);
            // Defined in other MenuF files.
            void SetPreviousButtonKindIdWithoutNone(long nPad, unsigned short nBtnKindId,
                                                    CSelectMenuLayoutBase *pObj);
            void Court_CameraReverse();

            // Added for MenuF game/menu/CRuleInfo.cpp. The steps of the
            // rules screen (SelectMenu::CRuleInfo), and the party game
            // information step it returns to (argument meaning guessed).
            void RuleInfo_In();
            void RuleInfo_Loop();
            void RuleInfo_Out();
            void PartyInfo_ChangeStateInit(long nMode);
            // Added for MenuF game/menu/CPartyInfo.cpp. The other steps of
            // the party game information screen (SelectMenu::CPartyInfo).
            void PartyInfo_In();
            void PartyInfo_Loop();
            void PartyInfo_Loop0();
            void PartyInfo_Loop1();
            void PartyInfo_Out();

            // Added for MenuF game/menu/CTopMenu.cpp (names of the
            // arguments guessed). The steps of the top menu
            // (SelectMenu::CTopMenu with the character table of
            // SelectMenu::CBgChr), and what they call.
            // Plays the name tag animation of character table slot nSlot,
            // selected or not.
            void _SetTagNameAnim(long nSlot, int bSelect);
            void TopMenu_In();
            void TopMenu_Loop();
            void TopMenu_DemoStep();
            void TopMenu_In2();
            void TopMenu_LoopStep0();
            // Lights the name tag of sport nChara (a character table
            // place) and turns the characters to it.
            void TopMenu_SelectAnim(long nChara);
            void TopMenu_ChangeStateInit(long nMode, long nUnk);
            void TopMenu_LoopStep1();
            void TopMenu_LoopStep10();
            void TopMenu_LoopStep11();
            void TopMenu_Out();
            // Queues the camera turn to place nPos; bReverse turns the
            // other way, bMix with the fifth (mix) sport open.
            void Top_SetTurnCamera(long nPos, int bReverse, int bMix);
            void TopMenu_BackPlaterSelect();
            // Defined in another MenuF file. The game kind of character
            // table place nChara.
            unsigned char _GetGameModeByBgChrId(unsigned char nChara);

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

        public:
            // 0x236. (name guessed; added for game/menu/unit_61C00.cpp)
            // A button kind id per pad, written by Cup_OkButtonOut with the
            // cup chosen.
            unsigned short m_nCupBtnKindId[4];

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
            // 0x25C. (name guessed; added for game/menu/unit_4BB48.cpp) The
            // game kind the Wi-Fi match-making screen was entered with
            // (negative: none); it then builds the court of that game.
            long m_nWifiGameKind;
        private:
            unsigned char m_Unk260[0x4];     // 0x260
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
            unsigned char m_Unk272[0x2];     // 0x272
            // 0x274 to 0x279. (names guessed; added for
            // game/menu/unit_4BB48.cpp) m_nGameKind, m_nStageSub and
            // m_nStage as the Wi-Fi match-making screen found them; put
            // back when it is left.
            long m_nWifiSavedGameKind;       // 0x274
            unsigned char m_nWifiSavedStageSub; // 0x278
            unsigned char m_nWifiSavedStage; // 0x279
            unsigned char m_Unk27A;          // 0x27A
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
            unsigned char m_Unk27E;          // 0x27E
        public:
            // 0x27F. (added for game/menu/CTopMenu.cpp) When non-zero the
            // top menu clears the stage behind it.
            unsigned char m_Unk27F;
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
            unsigned char m_Unk6D0[0x8];     // 0x6D0
        public:
            // 0x6D8. (added for game/menu/CFriendInfo.cpp; name guessed)
            // A copy of the friend's name, for the message windows.
            wchar_t m_FriendNameWork[0xC];
        private:
            unsigned char m_Unk6F0[0x8];     // 0x6F0
        public:
            // 0x6F8. (added for game/menu/CFriendCode.cpp; name guessed)
            // The Wi-Fi friend roster, taken from the connection by
            // FriendCode_In.
            bcs::dwc::CDwcFriendList *m_pFriendList;
            // 0x6FC to 0x707. (names guessed; added for
            // game/menu/CWiFiFrindVS.cpp) The Wi-Fi connection, the friend
            // the friend match is with, and a copy of it kept by
            // FriendVS_In.
            dwc::CGameDwc *m_pGameDwc;       // 0x6FC
            dwc::CFriend *m_pVsFriend;       // 0x700
            dwc::CFriend *m_pVsFriendSaved;  // 0x704
            // 0x708. (added for game/menu/CFriendCode.cpp) Cleared by
            // FriendPlayerInfo_In (meaning not known).
            // (game/menu/CFriendInfo.cpp) The friend the information
            // window shows; null for the player's own.
            dwc::CFriend *m_Unk708;
        private:
            unsigned char m_Unk70C[0x4];     // 0x70C
        public:
            CSave::PlayerData *m_pPlayerData; // 0x710, CSave::GetPlayerData(-1)
        private:
            unsigned char m_Unk714[0x48];    // 0x714
        public:
            // Size 0x1EC. (added for game/menu/CWiFiGame.cpp; names
            // guessed) What each side of a friend match sends the other
            // (bcs::dwc::CDwc::SetMySingleDataA, GetSingleDataA).
            // (game/menu/unit_4BB48.cpp) The match-making screens fill
            // the rest.
            struct WiFiSingleData {
                // 0x00. A hash of a name (dwc::CGameDwc::Makehash) telling
                // which screen sent the data.
                long m_nHash;
                unsigned char m_nRole;       // 0x04, compared with m_bStarMii
                unsigned char m_Unk05;       // 0x05, cleared by FriendVSWait_Loop
                unsigned char m_Unk06[2];    // 0x06
                long m_nBtnKindId;           // 0x08, the button the host's cursor is on
                unsigned long m_nInput;      // 0x0C, the host's decide input
                long m_nStep;                // 0x10, the step the sender has reached
                unsigned long m_nSeed;       // 0x14, a random seed both players use
                wchar_t m_Name[0xC];         // 0x18, the player's name
                wchar_t m_Name2P[0xC];       // 0x30, the second player's
                RFLStoreData m_StoreData;    // 0x48, the player's Mii
                RFLStoreData m_StoreData2P;  // 0x94, the second player's
                long m_nPlayerNum;           // 0xE0, players on this side
                long m_nGameKind;            // 0xE4, -1 for any
                long m_nSkill;               // 0xE8, the Wi-Fi rating
                unsigned char m_nTrophy[4];  // 0xEC, per sport (CheckSkillToTrophy)
                long m_nStage;               // 0xF0, 0xFF for any
                long m_nStageSub;            // 0xF4, 0xFF for any
                // 0xF8 to 0x1E4. The three characters: CharaSelectParam's
                // m_nChara, m_nCharaSub and m_Unk13[0], and the Mii of
                // each that is one.
                unsigned char m_nChara[3];
                unsigned char m_nCharaSub[3];
                unsigned char m_nCharaUnk[3];
                RFLStoreData m_CharaMii[3];
                // 0x1E5. CMenuPad::GetPadStyle of pads 0 and 1.
                unsigned char m_nPadStyle[2];
                unsigned char m_Unk1E7[2];   // 0x1E7
                // 0x1E9. The character change options of the save data:
                // bit 0 basketball, bit 2 dodgeball, bit 3 hockey.
                unsigned char m_nChrChangeFlags;
                // 0x1EA. SAVE_DATA_PLAYER::m_Unk5D1F.
                unsigned char m_Unk1EA;
                unsigned char m_Unk1EB;      // 0x1EB
            };

            union {
                // 0x75C. (added for game/menu/CWiFiFrindVS.cpp) 0x1EC bytes
                // FriendVS_Loop clears before the friend match starts.
                unsigned char m_FriendVSWaitWork[0x1EC];
                // The same bytes as this player's WiFiSingleData
                // (game/menu/CWiFiGame.cpp).
                WiFiSingleData m_MySingleData;
            };
        public:
            // 0x948 to 0x95B. (names guessed; added for
            // game/menu/unit_4BB48.cpp) The Wi-Fi match-making state.
            // The number of players whose data is compared (2).
            long m_nWifiPlayerNum;           // 0x948
            // Frames spent waiting for an opponent.
            float m_fWifiMatchTime;          // 0x94C
            // Passed to bcs::dwc::CDwc::StartAnybodyMatching; cleared after a
            // minute of waiting when the match is limited to Australia.
            long m_bWifiMatchLimit;          // 0x950
        private:
            unsigned char m_Unk954[0x4];     // 0x954
        public:
            // Set when the matching is started again: the wait window is
            // still open.
            long m_bWifiRetry;               // 0x958
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
            // 0x968. (added for game/menu/CFriendCode.cpp; name and
            // length guessed) Text filled in by ExchangeFriendCode and
            // shown in the message windows.
            wchar_t m_MessageWork[0x10];
        public:
            // 0x988. (name guessed; added for game/menu/CPlusMinus.cpp)
            // Non-zero when _SetStarMii gives Miis.
            long m_bStarMii;
            // 0x98C. (name guessed; added for game/menu/unit_4BB48.cpp) Bit
            // 0 and bit 1: FriendVSWait_Message has shown its first and its
            // second message.
            unsigned char m_nFriendVSMessage;
        private:
            unsigned char m_Unk98D[0x73];    // 0x98D
        public:
            long m_UnkA00;                   // 0xA00
            // 0xA04, 0xA08. (names guessed; added for
            // game/menu/unit_61C00.cpp) The cup model the cup select
            // screen shows (0 none, 1 to 3 the cups), and the one shown
            // before it.
            long m_nCupModel;
            long m_nPrevCupModel;
        private:
        public:
            // 0xA0C. (added for game/menu/CFriendInfo.cpp; name guessed)
            // The step of FriendMatchChancelCheck.
            long m_nCancelStep;
        private:
            unsigned char m_UnkA10[0x4];     // 0xA10
        public:
            // 0xA14. (added for game/menu/CTopMenu.cpp; name guessed) The
            // sport TopMenu_SelectAnim last lit, -1 for none.
            long m_nTopSelectChara;
            // 0xA18. (name guessed; added for game/menu/unit_4BB48.cpp)
            // When 1, WifiWait_In sets up the court and the camera first.
            long m_bWifiWaitCourt;
        private:
            unsigned char m_UnkA1C[0x40];    // 0xA1C
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
            unsigned char m_UnkA7C[0x24];    // 0xA7C
        public:
            // 0xAA0. (added for game/menu/CFriendList.cpp; name guessed)
            // The name the friend roster's help line was last made for.
            wchar_t m_FriendListLineName[0xC];
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

    // Defined in another MenuF file (added for game/menu/unit_61C00.cpp;
    // the return type and argument names are guesses). Stores the number
    // of cups of game kind nGame in *pCount.
    long _GetCupCount(long nGame, unsigned char *pCount);

    // Defined in another MenuF file (added for game/menu/CTopMenu.cpp;
    // return type guessed). Whether the fifth (mix) sport is open.
    int _IsOpenMixSportsFlag();
}

#endif /* !CSELECTMENU_HPP_ */
