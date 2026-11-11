// bcs::CMusicData, read access to a binary music data blob: the stage music's
// ID, shot timing windows, base distances and the per-player melody table.
#include "bcs/CMusicData.hpp"

#include <stdio.h>

#include "bcs/CSnd.hpp"

namespace bcs {

    CMusicData::CMusicData(const void *pData)
        : m_pData(static_cast<const MusicDataHeader *>(pData))
    {
    }

    CMusicData::~CMusicData()
    {
    }

    long CMusicData::GetBgmID() const
    {
        if (m_pData != 0) {
            return m_pData->m_BgmID;
        }
        return -1;
    }

    float CMusicData::GetShotMinFrame(long idx) const
    {
        if (m_pData != 0) {
            return m_pData->m_ShotMinFrame[idx];
        }
        return 0.0f;
    }

    float CMusicData::GetShotMaxFrame(long idx) const
    {
        if (m_pData != 0) {
            return m_pData->m_ShotMaxFrame[idx];
        }
        return 0.0f;
    }

    long CMusicData::GetBallBaseType() const
    {
        if (m_pData != 0) {
            return m_pData->m_BallBaseType;
        }
        return 0;
    }

    long CMusicData::GetItemBaseType() const
    {
        if (m_pData != 0) {
            return m_pData->m_ItemBaseType;
        }
        return 0;
    }

    float CMusicData::GetBallBaseDis() const
    {
        if (m_pData != 0) {
            return m_pData->m_BallBaseDis;
        }
        return 0.0f;
    }

    float CMusicData::GetItemBaseDis() const
    {
        if (m_pData != 0) {
            return m_pData->m_ItemBaseDis;
        }
        return 0.0f;
    }

    long CMusicData::GetRainTrackNum() const
    {
        if (m_pData != 0) {
            return m_pData->m_RainTrackNum;
        }
        return 0;
    }

    long CMusicData::GetPlayerMelodyNum(long player) const
    {
        const MusicDataHeader *pData = m_pData;
        return pData != 0 ? pData->m_PlayerMelodyNum[player] : 0;
    }

    const MusicMelodyData *CMusicData::GetPlayerMelodyDataTop(long player) const
    {
        long top = 0;
        for (long i = 0; i < player; i++) {
            top += GetPlayerMelodyNum(i);
        }
        return &m_pData->m_Melody[top];
    }

    long CMusicData::GetPlayerMelodyType(long player, long idx) const
    {
        return GetPlayerMelodyDataTop(player)[idx].m_Type;
    }

    float CMusicData::GetPlayerMelodyDisMin(long player, long idx) const
    {
        return GetPlayerMelodyDataTop(player)[idx].m_DisMin;
    }

    float CMusicData::GetPlayerMelodyDisMax(long player, long idx) const
    {
        return GetPlayerMelodyDataTop(player)[idx].m_DisMax;
    }

    long CMusicData::GetPlayerMelodyFlag(long player, long idx) const
    {
        return GetPlayerMelodyDataTop(player)[idx].m_Flag;
    }

    float CMusicData::GetPlayerMelodyMeetTime(long player, long idx) const
    {
        return GetPlayerMelodyDataTop(player)[idx].m_MeetTime;
    }

    long CMusicData::GetPlayerMelodyID(long player, long idx) const
    {
        const MusicMelodyData *pTop = GetPlayerMelodyDataTop(player);

        // Flagged melodies are not sounds of their own: the first letter of
        // the name says which special one it is.
        if (GetPlayerMelodyFlag(player, idx) & 0x80) {
            switch (pTop[idx].m_Name[0]) {
                case 'R':
                    return -2;
                case 'C':
                    return -3;
                case 'T':
                    return -4;
                case 'O':
                    return -5;
                case 'B':
                    return -6;
                case 'M':
                    return -7;
                default:
                    return -1;
            }
        }

        long bgm = GetBgmID();
        if (bgm < 0) {
            return -1;
        }

        char label[0x100];
        sprintf(label, "OTOGAME_%.2d_%s", bgm, pTop[idx].m_Name);
        long id = CSnd::GetSoundID(label);
        if (id < 0) {
            id = -1;
        }
        return id;
    }
}
