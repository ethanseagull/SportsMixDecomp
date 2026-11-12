#ifndef MENU_CSTATESAVE_HPP_
#define MENU_CSTATESAVE_HPP_

#include "revolution/rfl.h"

namespace msw {
    namespace menu {

        // Menu state kept across mode changes. Lives in the DOL: one static
        // instance of 0xBB0 bytes (GetInstance returns its address; the
        // static initialiser clears bytes 0 and 1, stores the vptr at 0xBAC
        // and registers the destructor). Only bytes 0 and 1 are used by any
        // code found so far; names are guesses.
        class CStateSave {
            public:
                CStateSave() : m_bActive(0), m_nStartMenuMode(0) {}

                static CStateSave *GetInstance();
                void Initialize();
                void Finalize();

                // MenuF emits weak copies of these two.
                unsigned char GetStartMenuMode() const { return m_nStartMenuMode; }
                void SetStartMenuMode(unsigned char mode) { m_nStartMenuMode = mode; }
                // (added for MenuF game/menu/CSelectMenu.cpp; name guessed)
                unsigned char IsActive() const { return m_bActive; }

            private:
                // The instance GetInstance returns (name guessed).
                static CStateSave s_Instance;

                unsigned char m_bActive;        // 0x000, 1 after Initialize, 0 after Finalize
                unsigned char m_nStartMenuMode; // 0x001
                unsigned char m_Unk002[2];

            public:
                // 0x004 to 0xBAB. (added for MenuF game/menu/CSelectMenu.cpp;
                // names follow msw::CSelectMenu's members) What
                // CSelectMenu::Backup keeps of the select menu while a game
                // runs, and Restore puts back: a copy of most of its
                // members, the arrays sized for 0x80 screens.
                // The button each pad was on, for one select menu screen
                // (CSelectMenu::PadBtnKindId). A struct: Backup and Restore
                // copy it whole (two words).
                struct PadBtnKindId {
                    unsigned short m_nPad[4];
                };
                struct SelectSlot {               // CSelectMenu::CharaSelectParam
                    long m_Unk00;
                    long m_nPlayerKind;
                    long m_Unk08;
                    long m_Unk0C;
                    unsigned char m_nChara;
                    unsigned char m_nCharaSub;
                    unsigned char m_nState;
                    unsigned char m_nColor;
                    unsigned short m_Unk14;
                    unsigned char m_Unk16;
                    unsigned char m_nMiiMode;
                    long m_nMiiIndex;
                    RFLDataSource m_MiiSource;
                    RFLCreateID m_MiiCreateId;
                };
                long m_bPadLeader[4];             // 0x004, flag 0x04 of each CMenuPadInfo pad
                long m_nTopChara;                 // 0x014, CBgChr::GetPosChara(0)
                short m_nStep;                    // 0x018
                float m_fWait;                    // 0x01C
                unsigned char m_bModeEnd;         // 0x020
                unsigned char m_nNextMode;        // 0x021
                unsigned char m_Unk22;            // 0x022
                unsigned char m_nMenuMode;        // 0x023
                unsigned char m_Unk24;            // 0x024
                unsigned char m_nPrevModeState;   // 0x025
                unsigned short m_nCurrentBtnKindId[4][0x80]; // 0x026
                unsigned short m_nPrevBtnKindId[4]; // 0x426
                unsigned char m_PadSelect[4][5];  // 0x42E
                unsigned char m_nCharaColor[4];   // 0x442
                unsigned char m_nGameKind;        // 0x446
                long m_nWifiGameKind;             // 0x448
                unsigned char m_nPlayerNum;       // 0x44C
                unsigned char m_Unk44D;           // 0x44D
                unsigned char m_nPlayMode;        // 0x44E
                unsigned char m_Unk44F;           // 0x44F
                unsigned char m_Unk450;           // 0x450
                unsigned char m_Unk451;           // 0x451
                unsigned char m_nStageSub;        // 0x452
                unsigned char m_nStage;           // 0x453
                long m_nCourtGameKind;            // 0x454
                long m_nWifiSavedGameKind;        // 0x458
                unsigned char m_nWifiSavedStageSub; // 0x45C
                unsigned char m_nWifiSavedStage;  // 0x45D
                unsigned char m_Unk45E[5];        // 0x45E, CSelectMenu 0x27A to 0x27E
                unsigned char m_Unk463;           // 0x463, not copied
                unsigned char m_nPadPlayer[4];    // 0x464
                unsigned char m_History[0x80][2]; // 0x468
                unsigned char m_Unk568[2];       // 0x568
                PadBtnKindId m_HistoryBtnKindId[0x80]; // 0x56A
                long m_nHistoryNum;               // 0x96C
                unsigned char m_Unk970[4];        // 0x970
                SelectSlot m_aCharaSelectParam[6]; // 0x974
                SelectSlot m_aCharaSaveParam[6];  // 0xA64
                float m_fRandomMoveTime[4];       // 0xB54
                long m_nRandomDecideTime[4];      // 0xB64
                long m_Unk688;                    // 0xB74
                SelectSlot m_Wifi2P;              // 0xB78, CSelectMenu 0x68C to 0x6B3
                long m_bStarMii;                  // 0xBA0
                long m_Unk708;                    // 0xBA4
                long m_pVsFriendSaved;            // 0xBA8

            public:
                // vptr at 0xBAC; the vtable has only the destructor. Empty,
                // but out of line: the static initialiser registers it.
                virtual ~CStateSave();
        };
    }
}

#endif /* !MENU_CSTATESAVE_HPP_ */
