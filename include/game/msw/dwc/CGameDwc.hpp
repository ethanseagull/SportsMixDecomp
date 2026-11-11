#ifndef MSW_DWC_CGAMEDWC_HPP_
#define MSW_DWC_CGAMEDWC_HPP_

#include "bcs/dwc/CDwc.hpp"
#include "revolution/rfl.h"
#include "wchar.h"

namespace msw {
    namespace dwc {
        class CFriend;

        // The game's Wi-Fi connection object; CSelectMenu keeps a pointer
        // to it at 0x6FC. Only the members used so far; the size is not
        // known. Argument names are guesses.
        class CGameDwc {
            public:
                // (added for BasketF game/basket/CCockpitBasket.cpp) The
                // instance; null when there is none.
                static CGameDwc *GetInstance();

                // Opens the message window with message pLabel of group
                // pGroup and an A button. The two texts fill parts of the
                // message (null: none); what nUnk1 and nUnk2 select is not
                // known.
                void WindowA_Button(char *pGroup, char *pLabel, wchar_t *pText0, wchar_t *pText1, int nUnk1,
                                    unsigned long nUnk2);
                // The same without a button.
                void Window_Message(char *pGroup, char *pLabel, int nUnk1, unsigned long nUnk2);
                void Window_Close();
                // Added for MenuF game/menu/CFriendInfo.cpp (argument names
                // guessed): the same with Yes and No, a window with a
                // picture, and closing that one.
                void WindowYesNo_Button(char *pGroup, char *pLabel, int nUnk0, wchar_t *pText0, wchar_t *pText1,
                                        unsigned long nUnk1);
                void Window_Picture1(char *pGroup, char *pLabel, int nUnk);
                void Window_Pict1Close();
                // Added for MenuF game/menu/unit_4BB48.cpp (return types and
                // argument names guessed): keeping the picture window open
                // (bLock) or not, closing the A button window, and a hash
                // of the string pName (used to tell the screens' data
                // apart).
                void Window_Pict1Lock(int bLock);
                void WindowA_Close();
                long Makehash(char *pName);

                // Added for MenuF game/menu/CWiFiGame.cpp (return types and
                // argument names guessed). A time limit of fTime (seconds,
                // guessed) for the screen; non-zero once it has run out;
                // stopping it.
                static void SetUpTimer(float fTime, int nUnk);
                static int CountTimer();
                static void CloseTimer();

                // (no symbol; name guessed) Forgets the answer of the last
                // message window. Inline in MenuF.
                void ClearWindowResult()
                {
                    m_UnkDBC = 0;
                    m_UnkDC0 = 0;
                }

            private:
                unsigned char m_Unk00[0x38]; // 0x00

            public:
                // 0x38. The connection itself. bcs::dwc::CDwc has no data
                // members in our header, so its size is not modelled: the
                // padding below keeps the offsets after it.
                bcs::dwc::CDwc m_Dwc;

                // Size 0xE0. (added for MenuF game/menu/unit_4BB48.cpp;
                // names guessed) What the match-making screens copy from
                // each side's CSelectMenu::WiFiSingleData for the game.
                struct PlayerInfo {
                    RFLStoreData m_StoreData;   // 0x00
                    RFLStoreData m_StoreData2P; // 0x4C
                    long m_nSkill;              // 0x98
                    unsigned char m_nTrophy[4]; // 0x9C
                    wchar_t m_Name[0x10];       // 0xA0
                    wchar_t m_Name2P[0x10];     // 0xC0
                };

                // (no symbol; name guessed) Sets player nPlayer's Miis and
                // rating. Inline in MenuF.
                void SetPlayerData(long nPlayer, const RFLStoreData &rData, const RFLStoreData &rData2P,
                                   long nSkill)
                {
                    m_PlayerInfo[nPlayer].m_StoreData = rData;
                    m_PlayerInfo[nPlayer].m_StoreData2P = rData2P;
                    m_PlayerInfo[nPlayer].m_nSkill = nSkill;
                }

                // (no symbol; names guessed) Copies a name into player
                // nPlayer's entry unless it is empty. Inline in MenuF.
                void SetPlayerName(long nPlayer, const wchar_t *pName)
                {
                    if (wcslen(pName) != 0) {
                        wcscpy(m_PlayerInfo[nPlayer].m_Name, pName);
                    }
                }
                void SetPlayerName2P(long nPlayer, const wchar_t *pName)
                {
                    if (wcslen(pName) != 0) {
                        wcscpy(m_PlayerInfo[nPlayer].m_Name2P, pName);
                    }
                }

            private:
                unsigned char m_Unk39[0xB30 - 0x39];

            public:
                // 0xB30. Player 0 is this side.
                PlayerInfo m_PlayerInfo[2];
                // 0xCF0. (added for MenuF game/menu/unit_4BB48.cpp) The
                // friend a friend match is with.
                CFriend *m_pMatchFriend;

            private:
                unsigned char m_UnkCF4[0xDBC - 0xCF4];

            public:
                // 0xDBC and 0xDC0. Cleared once the window's answer has been
                // read (names unknown).
                long m_UnkDBC;
                long m_UnkDC0;

                // 0xDC4. (added for MenuF game/menu/unit_4BB48.cpp) A copy
                // of CSelectMenu::m_bStarMii when the game starts.
                long m_nRole;

            private:
                unsigned char m_UnkDC8[0xDCC - 0xDC8];

            public:
                // 0xDCC: cleared by msw::CCockpitBasket::GameResult_In
                // before it sends empty data to the opponent (meaning not
                // known).
                long m_UnkDCC;
        };
    }
}

#endif /* !MSW_DWC_CGAMEDWC_HPP_ */
