#ifndef BCS_DWC_CDWC_HPP_
#define BCS_DWC_CDWC_HPP_

#include "dwc.h"

namespace bcs {
    namespace dwc {
        class CDwcFriendList;

        // Nintendo Wi-Fi Connection wrapper (game/bcs/dwc/CDwc.cpp). Only the
        // members used so far. The functions marked "unnamed" have no export
        // name in the binary (fn_800B....); their names are guesses from the
        // CWifiPadBase functions that wrap them.
        class CDwc {
            public:
                static CDwc *GetInstance();

                long GetMyAID() const;

                // fn_800B1A28: reads the word at 0x724. Unnamed; non-zero
                // while a match is running (guess).
                long IsInMatch() const;
                // fn_800B1D84: signed halfword at 0x802. Unnamed.
                long GetOnlinePadTotal() const;
                // fn_800B1D8C: signed byte table at 0x7FA. Unnamed.
                long GetAIDFromOnlinePadID(long id) const;
                // fn_800B1D9C: signed byte table at 0x7FE. Unnamed.
                long GetLocalPadIDFromOnlinePadID(long id) const;

                // Added for MenuF game/menu/CWiFiFrindVS.cpp (return types
                // guessed where the result is not used; argument names are
                // guesses). The connection state (4 and 10 are tested).
                long GetStat() const;
                // Accept or refuse the match friend nIndex asked for.
                int EntryFriendMatch(long nIndex, long nPlayerNum);
                int RefuseFriendMatch(long nIndex);
                void SetReadyFromMatchEnd();
                int IsMatchPlay() const;
                void CloseMatching();
                // Added for BasketF game/basket/CCockpitBasket.cpp (return
                // types guessed). Whether a connection is up; sends nSize
                // bytes of pData as this player's data.
                int IsConnect() const;
                void SetMySingleDataA(const void *pData, long nSize);
                // Added for MenuF game/menu/CFriendCode.cpp (return types
                // guessed). The friend roster, and asking for the friends'
                // servers to be updated in the background.
                CDwcFriendList *GetFriendList();
                void SetFriendServersAsyncFlag(int bFlag);
                // Added for MenuF game/menu/CFriendInfo.cpp (return types
                // guessed). Asks friend nIndex for a match; whether it has
                // been accepted.
                int CreateFriendMatch(long nIndex, long nPlayerNum);
                int IsMatched() const;
                // Added for MenuF game/menu/CWiFiGame.cpp (argument name
                // guessed). The data player nAid last sent with
                // SetMySingleDataA.
                const void *GetSingleDataA(long nAid) const;
                // fn_800B1ED0 (unnamed; name and return type guessed): asks
                // the storage server for the public data of friend nIndex.
                // Returns 0 when there is no storage connection.
                int LoadFriendStorage(long nIndex);
                // Added for MenuF game/menu/unit_4BB48.cpp (return types and
                // argument names guessed). The opponent's AID; the level the
                // match was made at; whether matches are limited to
                // Australia; starting a match with anybody for nNum players
                // (nPlayerNum of them here) in game kind nGameKind at rating
                // nSkill (bLimit: see CSelectMenu::m_bWifiMatchLimit).
                long GetEmAID() const;
                long GetMatchingLevel() const;
                int IsShouldMatchAustraliaOnly() const;
                void StartAnybodyMatching(long nNum, long nPlayerNum, long nGameKind, long nSkill, int bLimit);
                // Added for MenuF game/menu/CWiFiTopA.cpp (return types and
                // argument names guessed). Logging in with the player's
                // account and the names of its nNum players; the account
                // data back; whether it changed since it was last saved,
                // and forgetting that; the player's friend code; starting
                // and finishing the connection; the data other players see
                // (status and public storage data); whether the storage
                // has been read; how many friends' servers have answered.
                void Login(const DWCstAccUserData *pUserData, const wchar_t **ppNames, long nNum);
                void GetUserData(DWCstAccUserData *pUserData) const;
                int IsDirtyUserData() const;
                void ResetDirtyUserData();
                unsigned long long GetFriendCode() const;
                int Start();
                int Finish();
                int IsFinishComplete() const;
                void SetOwnStatusAppData(const void *pData, long nSize);
                void SaveEntryOwnPublicData(const void *pData, long nSize);
                int IsStorageComplete() const;
                long GetFriendServersAsyncCount() const;
                // Added for game/bcs/CWifiPad.cpp (all unnamed, fn_800B20B0
                // to fn_800B2488; names from the CWifiPad functions that
                // wrap them). The pad state the player of online pad nPad
                // sent (a 0x7C-byte record per pad at 0x820).
                int IsNunchukStyle(long nPad) const;
                float GetCameraAngle(long nPad) const;
                int IsDown(unsigned long nButton, long nPad) const;
                int IsTrg(unsigned long nButton, long nPad) const;
                int IsRelease(unsigned long nButton, long nPad) const;
                unsigned long GetDown(long nPad) const;
                unsigned long GetTrg(long nPad) const;
                unsigned long GetRelease(long nPad) const;
                float GetStickX(long nPad) const;
                float GetStickY(long nPad) const;
                long GetStrokeRaw0(long nPad) const;
                long GetStrokeRaw1(long nPad) const;
                long GetStrokeRaw2(long nPad) const;
                long GetStrokeRaw3(long nPad) const;
                long GetStrokeRaw4(long nPad) const;
                long GetStroke0(long nPad) const;
                long GetStroke1(long nPad) const;
                long GetStroke2(long nPad) const;
                long GetStroke3(long nPad) const;
                long GetStroke4(long nPad) const;
                void ResetAcc(long nPad);
                float GetScratchPower(long nPad) const;
                float GetNunchukPower(long nPad) const;
        };
    }
}

#endif /* !BCS_DWC_CDWC_HPP_ */
