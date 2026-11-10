#ifndef BCS_DWC_CDWC_HPP_
#define BCS_DWC_CDWC_HPP_

namespace bcs {
    namespace dwc {

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
        };
    }
}

#endif /* !BCS_DWC_CDWC_HPP_ */
