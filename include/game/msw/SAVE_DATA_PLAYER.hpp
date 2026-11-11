#ifndef SAVE_DATA_PLAYER_HPP_
#define SAVE_DATA_PLAYER_HPP_

#include "revolution/rfl.h"

namespace msw {

    // One player's save data (DOL). Only the members used so far; the
    // per-sport settings below are read by TourF's TournamentRule::SetupGame.
    // (member names and meanings guessed)
    struct SAVE_DATA_PLAYER {
        // Not zero while sport nGameMode has not been played in mode nMode.
        // (meaning and return type guessed)
        int IsFirstPlay(long nMode, long nGameMode);
        // (added for MenuF game/menu/CPlayMode.cpp) Forgets the Mii chosen
        // on the character select screen.
        void ClearSelectMii();
        // (added for MenuF game/menu/CEntry.cpp; return types and argument
        // names guessed) The index of the Mii with id createId, and
        // choosing a Mii by index.
        long SearchMiiIndex(RFLCreateID createId);
        void SetSelectMii(long nIndex, int bUnk);
        // (added for MenuF game/menu/CCharaLevelStar.cpp) For game kind
        // nGameKind (CSelectMenu::m_nGameKind): the computer level (0 to
        // 3) the level window starts from. Meaning guessed from that use.
        unsigned char IsDifficult(long nGameKind);

        unsigned char m_Unk0000[0x18];
        // 0x18 to 0x2B. (names guessed; added for MenuF
        // game/menu/CPlusMinus.cpp) The player's own Mii, passed to
        // CSelectMenu::Mii_SelectParam.
        unsigned char m_nMiiUnk;         // 0x18
        long m_nMiiIndex;                // 0x1C
        RFLDataSource m_MiiSource;       // 0x20
        RFLCreateID m_MiiCreateId;       // 0x24
        unsigned char m_Unk002C[0x5C50 - 0x2C];
        unsigned char m_nFlags5C50; // 0x5C50; bit 0x08 picks the second camera table in CSelectMenu::HowTo_In (MenuF)
        unsigned char m_Unk5C51[0x5C95 - 0x5C51];
        // 0x5C95, 0x5CB5, 0x5CD5, 0x5CF5: one byte per sport (basketball,
        // volleyball, dodgeball, hockey); while it is 0 the tournament sets
        // rule option 0x200.
        signed char nBasketOpt;
        unsigned char m_Unk5C96[0x5CB5 - 0x5C96];
        signed char nVolleyOpt;
        unsigned char m_Unk5CB6[0x5CD5 - 0x5CB6];
        signed char nDodgeOpt;
        unsigned char m_Unk5CD6[0x5CF5 - 0x5CD6];
        signed char nHockeyOpt;
        unsigned char m_Unk5CF6[0x5D07 - 0x5CF6];
        // 0x5D07, 0x5D29, 0x5D36: not zero when the user switches characters
        // by hand in basketball, dodgeball and hockey (no such setting for
        // volleyball).
        unsigned char bBasketChrChange;
        unsigned char m_Unk5D08[0x5D29 - 0x5D08];
        unsigned char bDodgeChrChange;
        unsigned char m_Unk5D2A[0x5D36 - 0x5D2A];
        unsigned char bHockeyChrChange;
    };
}

#endif /* !SAVE_DATA_PLAYER_HPP_ */
