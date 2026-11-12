#ifndef SAVE_DATA_PLAYER_HPP_
#define SAVE_DATA_PLAYER_HPP_

#include "dwc.h"
#include "game/msw/WiFiSkill.hpp"
#include "revolution/rfl.h"

namespace msw {

    // One player's save data (DOL). Only the members used so far; the
    // per-sport settings below are read by TourF's TournamentRule::SetupGame.
    // (member names and meanings guessed)
    // Size 0x2C. (added for MenuF game/menu/CWiFiTopA.cpp; the class
    // name is from the DOL's GetMiiStoreData__Q23msw10PlayerInfoFv, which
    // reads the Mii at 0x20 and 0x24 of the save data: so the save data
    // starts with it. Member names guessed.) The player's name and Mii.
    struct PlayerInfo {
        // The player's own Mii (bcs::CMiiMan::GetStoreData of m_MiiSource
        // and m_MiiCreateId).
        RFLStoreData GetMiiStoreData();

        // 0x00. (added for MenuF game/menu/CFriendCode.cpp; name and
        // length guessed) The player's name, shown in the friend code
        // message.
        wchar_t m_Name[0xC];
        // 0x18 to 0x2B. (names guessed; added for MenuF
        // game/menu/CPlusMinus.cpp) The player's own Mii, passed to
        // CSelectMenu::Mii_SelectParam.
        unsigned char m_nMiiUnk;         // 0x18
        long m_nMiiIndex;                // 0x1C
        RFLDataSource m_MiiSource;       // 0x20
        RFLCreateID m_MiiCreateId;       // 0x24
    };

    struct SAVE_DATA_PLAYER : public PlayerInfo {
        // Size 0xC. (added for MenuF game/menu/unit_61C00.cpp; names
        // guessed) Bit n of m_nLevelBit: difficulty n is open (bit 3 in
        // game kind 0's record also gates game kind 5); bit n of
        // m_nCupBit[nLevel]: cup n of that difficulty is open, of
        // m_nCupNewBit[nLevel]: its button shows the new mark.
        struct CupRecord {
            unsigned char m_nLevelBit;      // 0x0
            unsigned char m_nCupBit[4];     // 0x1
            unsigned char m_nCupNewBit[4];  // 0x5
            unsigned char m_Unk9[3];        // 0x9
        };

        // Not zero while sport nGameMode has not been played in mode nMode.
        // (meaning and return type guessed)
        int IsFirstPlay(long nMode, long nGameMode);
        // (added for MenuF game/menu/CTrofy.cpp) The best Wi-Fi rating
        // reached in sport nSport (0 basketball, 1 volleyball, 2 dodgeball,
        // 3 hockey; meaning guessed from the trophy thresholds).
        short GetMaxSkill(long nSport);
        // (added for MenuF game/menu/CPlayMode.cpp) Forgets the Mii chosen
        // on the character select screen.
        void ClearSelectMii();
        // (added for MenuF game/menu/CEntry.cpp; return types and argument
        // names guessed) The index of the Mii with id createId, and
        // choosing a Mii by index.
        long SearchMiiIndex(RFLCreateID createId);
        // (added for MenuF game/menu/CSelectMenu.cpp) The same for Mii data.
        long SearchMiiIndex(RFLStoreData data);
        void SetSelectMii(long nIndex, int bUnk);
        // (added for MenuF game/menu/unit_6EE84.cpp; argument names
        // guessed) Records difficulty nLevel for game kind nGameKind.
        void SetDifficult(long nGameKind, unsigned char nLevel);

        // Size 0x38. (added for MenuF game/menu/CMiiSel.cpp; names
        // guessed) One Mii of the player's list, or one of the guest Miis.
        struct MiiData {
            unsigned char m_Unk00[4];    // 0x00
            long m_nNameId;              // 0x04, a guest Mii's name in "List_Mii_Name"
            RFLDataSource m_Source;      // 0x08
            RFLCreateID m_CreateId;      // 0x0C, all zero for an empty entry
            // 0x14. (added for MenuF game/menu/CScoreBook.cpp; meaning
            // guessed) The Mii's record in each sport (0 basketball, 1
            // volleyball, 2 dodgeball, 3 hockey), ranked by the score book.
            long m_nScore[4];
            unsigned char m_Unk24[0x10]; // 0x24
            long m_bSelect;              // 0x34, read by GetSelectMii (meaning guessed)
        };
        // (added for MenuF game/menu/CMiiSel.cpp; return types guessed)
        // The number of Miis in the player's list, Mii nIndex of it (null
        // when empty) and guest Mii nIndex, and whether each is already
        // chosen.
        long GetMiiCount();
        MiiData *GetIndexMii(long nIndex);
        MiiData *GetGuestMii(long nIndex);
        int GetSelectMii(long nIndex);
        int GetSelectGuestMii(long nIndex);
        // (added for MenuF game/menu/CCharaLevelStar.cpp) For game kind
        // nGameKind (CSelectMenu::m_nGameKind): the computer level (0 to
        // 3) the level window starts from. Meaning guessed from that use.
        unsigned char IsDifficult(long nGameKind);
        // (added for MenuF game/menu/CFriendList.cpp; return type guessed)
        // Not zero when the data holds a player.
        int IsValid() const;
        // (added for MenuF game/menu/CSaveSelect.cpp) Clears the data: the
        // player is deleted.
        void Init();
        // (added for MenuF game/menu/CScoreBook.cpp; name from the symbol,
        // meaning guessed) The record of stage nStage in game kind
        // nGameKind, negative when it has none.
        long CheckStage(long nGameKind, unsigned short nStage);

        // 0x00 to 0x2B: PlayerInfo.
        // 0x2C. (added for MenuF game/menu/CWiFiTopA.cpp; name guessed)
        // The answer to the Wi-Fi top screen's question about showing
        // other players' Miis: 0 yes, 1 no.
        long m_nOtherMii;
        unsigned char m_Unk0030[0x38 - 0x30];

        // 0x38 to 0x56EF. (names guessed; added for MenuF
        // game/menu/CFriendCode.cpp) The Wi-Fi account and friend roster.
        // One saved friend, copied from bcs::dwc::CDwcFriend.
        struct Friend {
            unsigned char m_Unk00[0x18]; // 0x00
            union {
                struct {
                    long m_Unk18;        // 0x18, CDwcFriend 0x04
                    long m_Unk1C;        // 0x1C, CDwcFriend 0x08
                    long m_Unk20;        // 0x20, CDwcFriend 0x0C
                };
                // 0x18. (added for MenuF game/menu/CWiFiTopA.cpp) The
                // three words as the library's friend data, as
                // CDwcFriendList::WriteBack takes them.
                DWCstAccFriendData m_FriendData;
            };
            unsigned long long m_nFriendCode; // 0x28, CDwcFriend 0x10
            long m_Unk30;                // 0x30, CDwcFriend 0x18
            long m_Unk34;                // 0x34, CDwcFriend 0x1C
            unsigned char m_Data[0x260]; // 0x38, CDwcFriend 0xE0
            // 0x298. (added for MenuF game/menu/CFriendInfo.cpp; meaning
            // not known) Cleared when the friend is deleted.
            // Signed: game/menu/CFriendList.cpp shows them as numbers
            // (loaded with lha).
            short m_Unk298;
            short m_Unk29A;
            short m_Unk29C;
            unsigned char m_Unk29E[0x2];
        };

        enum {
            FRIEND_MAX = 32
        };

        DWCstAccUserData m_AccUserData;  // 0x38
        unsigned long long m_nFriendCode; // 0x78, the player's own code
        long m_nFriendNum;               // 0x80
        unsigned char m_Unk0084[0x88 - 0x84];
        // 0x88 to 0x2E7 (0x260 bytes, through m_TotalSkill): the public
        // data other players can read (bcs::dwc::CDwc::
        // SaveEntryOwnPublicData). (added for MenuF game/menu/CWiFiTopA.cpp;
        // names guessed) A fixed tag, the name, the Mii.
        enum {
            PUBLIC_DATA_SIZE = 0x260,
            PUBLIC_DATA_TAG = 0x75C9E14A
        };
        long m_nPublicTag;               // 0x88
        wchar_t m_PublicName[0xC];       // 0x8C
        RFLStoreData m_PublicMii;        // 0xA4
        unsigned char m_Unk00F0[0x108 - 0xF0];
        // 0x108. (added for MenuF game/menu/CFriendInfo.cpp) The player's
        // ratings in the four sports, then the overall one.
        WiFiSkill m_Skill[4];
        WiFiSkill m_TotalSkill;          // 0x288
        unsigned char m_Unk02E8[0x2EA - 0x2E8];
        // 0x2EA to 0x2EF. (added for MenuF game/menu/CFriendList.cpp;
        // meaning not known) Three counts shown on the player's row of the
        // friend roster, like Friend::m_Unk298 to m_Unk29C for a friend.
        short m_Unk2EA;
        short m_Unk2EC;
        short m_Unk2EE;
        Friend m_Friend[FRIEND_MAX];     // 0x2F0
        // Size 0x10. (added for MenuF game/menu/CScoreBook.cpp; names
        // guessed) One character's record in one sport.
        struct CharaScore {
            unsigned short m_nPlayNum;   // 0x0, not zero once played
                                         // (game/menu/CCharSel.cpp reads it as
                                         // the bits of the colours open)
            // 0x2. (added for game/menu/CCharSel.cpp; name guessed) Not zero
            // while the character is new: the select screen marks it.
            unsigned short m_nNew;
            long m_nScore;               // 0x4, ranked by the score book
            unsigned char m_Unk8[0x8];   // 0x8
        };
        enum {
            CHARA_SCORE_NUM = 20
        };
        // 0x56F0. One row per character for each sport (0 basketball, 1
        // volleyball, 2 dodgeball, 3 hockey).
        CharaScore m_CharaScore[4][CHARA_SCORE_NUM];
        unsigned char m_Unk5BF0[0x5C50 - 0x5BF0];
        union {
            unsigned char m_nFlags5C50; // 0x5C50; bit 0x08 picks the second camera table in CSelectMenu::HowTo_In (MenuF)
            // 0x5C50. (added for MenuF game/menu/unit_61C00.cpp; names
            // guessed) The cup record of each game kind (0 to 3); kind 5
            // uses the last difficulty of kind 0.
            CupRecord m_Cup[4];
        };
        unsigned char m_Unk5C80[0x5C95 - 0x5C80];
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
        unsigned char m_Unk5D08[0x5D1F - 0x5D08];
        // 0x5D1F. (added for MenuF game/menu/unit_4BB48.cpp; meaning not
        // known) Sent to the Wi-Fi opponent next to the options above.
        unsigned char m_Unk5D1F;
        unsigned char m_Unk5D20[0x5D29 - 0x5D20];
        unsigned char bDodgeChrChange;
        unsigned char m_Unk5D2A[0x5D36 - 0x5D2A];
        unsigned char bHockeyChrChange;
        unsigned char m_Unk5D37[0x8864 - 0x5D37];

        // (added for MenuF game/menu/CScoreBook.cpp; names guessed) The
        // totals the score book shows for one sport: games played, three
        // counts shown by difficulty, then values shown as an average per
        // game or as a percentage of another value. Which value is which
        // differs between the sports, so they are kept as an array.
        // Basketball, volleyball and dodgeball use 0x44 bytes; hockey's
        // record is longer (the score book reads it up to 0x4C).
        struct SportScore {
            long m_nPlayNum;             // 0x00
            short m_nLevelNum[3];        // 0x04
            long m_nValue[14];           // 0x0C
        };
        struct HockeyScore {
            long m_nPlayNum;             // 0x00
            short m_nLevelNum[3];        // 0x04
            long m_nValue[16];           // 0x0C
        };
        SportScore m_BasketScore;        // 0x8864
        SportScore m_VolleyScore;        // 0x88A8
        SportScore m_DodgeScore;         // 0x88EC
        HockeyScore m_HockeyScore;       // 0x8930
    };
}

#endif /* !SAVE_DATA_PLAYER_HPP_ */
