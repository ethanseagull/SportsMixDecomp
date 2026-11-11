#ifndef BCS_CMUSICDATA_HPP_
#define BCS_CMUSICDATA_HPP_

namespace bcs {

    // One melody a player can trigger: an entry of the music data's melody
    // table, 0x24 bytes. The entries of all players follow each other,
    // player 0's first (CMusicData::GetPlayerMelodyDataTop).
    struct MusicMelodyData {
        unsigned short m_Unk00;  // 0x00
        unsigned short m_Type;   // 0x02
        float m_DisMin;          // 0x04
        float m_DisMax;          // 0x08
        unsigned short m_Flag;   // 0x0C (bit 0x80 read by GetPlayerMelodyID)
        unsigned short m_Unk0E;  // 0x0E
        float m_MeetTime;        // 0x10
        char m_Name[0x10];       // 0x14 (first letter read by GetPlayerMelodyID)
    };

    // The binary music data CMusicData reads (loaded elsewhere; header up to
    // 0x78, then the melody table).
    struct MusicDataHeader {
        unsigned char m_Unk00[6];        // 0x00
        short m_BgmID;                   // 0x06
        unsigned char m_Unk08[4];        // 0x08
        float m_ShotMinFrame[8];         // 0x0C
        float m_ShotMaxFrame[8];         // 0x2C
        short m_BallBaseType;            // 0x4C
        short m_ItemBaseType;            // 0x4E
        float m_BallBaseDis;             // 0x50
        float m_ItemBaseDis;             // 0x54
        short m_Unk58;                   // 0x58
        short m_RainTrackNum;            // 0x5A
        short m_PlayerMelodyNum[14];     // 0x5C
        MusicMelodyData m_Melody[1];     // 0x78 (sum of m_PlayerMelodyNum entries)
    };

    // Read-only view of a music data blob (the rhythm of a stage's music).
    // A DOL class, size 8; created with the blob it reads, which it does not
    // own.
    class CMusicData {
        public:
            CMusicData(const void *pData);
            virtual ~CMusicData();

            long GetBgmID() const;
            float GetShotMinFrame(long idx) const;
            float GetShotMaxFrame(long idx) const;
            long GetBallBaseType() const;
            long GetItemBaseType() const;
            float GetBallBaseDis() const;
            float GetItemBaseDis() const;
            long GetRainTrackNum() const;
            long GetPlayerMelodyNum(long player) const;
            const MusicMelodyData *GetPlayerMelodyDataTop(long player) const;
            long GetPlayerMelodyType(long player, long idx) const;
            float GetPlayerMelodyDisMin(long player, long idx) const;
            float GetPlayerMelodyDisMax(long player, long idx) const;
            long GetPlayerMelodyFlag(long player, long idx) const;
            float GetPlayerMelodyMeetTime(long player, long idx) const;
            long GetPlayerMelodyID(long player, long idx) const;

        private:
            const MusicDataHeader *m_pData; // 0x04
    };
}

#endif /* !BCS_CMUSICDATA_HPP_ */
