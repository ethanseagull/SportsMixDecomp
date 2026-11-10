#include "game/msw/CHBData.hpp"

namespace msw {
namespace BA_PARTY {

    CHBData::CHBData()
        : m_pData(0)
        , m_nTimeZoneSize(0)
    {
    }

    CHBData::~CHBData()
    {
    }

    void CHBData::SetData(const unsigned char *pData)
    {
        m_pData = pData;
        const HEADER *pHeader = GetHeader();
        // time rate, turn rates, group entries, wait-ball entries
        m_nTimeZoneSize = 2 + pHeader->nTurnNum * 2 + pHeader->nTurnNum * 2 + pHeader->nWaitEntryNum * 2;
    }

    long CHBData::GetTimeZoneCount() const
    {
        return GetHeader()->nTimeZoneNum;
    }

    long CHBData::GetTurnCount() const
    {
        return GetHeader()->nTurnNum;
    }

    long CHBData::GetWaitEntryCount() const
    {
        return GetHeader()->nWaitEntryNum;
    }

    long CHBData::GetTimeRate(long nTimeZone) const
    {
        if (nTimeZone < 0 || nTimeZone >= GetHeader()->nTimeZoneNum) {
            return -1;
        }
        const unsigned char *p = GetTimeZoneTop();
        p += nTimeZone * m_nTimeZoneSize;
        return *reinterpret_cast<const short *>(p);
    }

    long CHBData::GetTurnRateData(long nTimeZone, long nTurn) const
    {
        if (nTimeZone < 0 || nTimeZone >= GetHeader()->nTimeZoneNum) {
            return -1;
        }
        if (nTurn < 0 || nTurn >= GetHeader()->nTurnNum) {
            return -1;
        }
        // skip to the time zone, its time rate, then the earlier turns
        const unsigned char *p = GetTimeZoneTop();
        p += nTimeZone * m_nTimeZoneSize + 2 + nTurn * 2;
        return *reinterpret_cast<const short *>(p);
    }

    long CHBData::GetGroupEntryData(long nTimeZone, long nTurn) const
    {
        if (nTimeZone < 0 || nTimeZone >= GetHeader()->nTimeZoneNum) {
            return -1;
        }
        if (nTurn < 0 || nTurn >= GetHeader()->nTurnNum) {
            return -1;
        }
        const unsigned char *p = GetTimeZoneTop();
        p += m_nTimeZoneSize * nTimeZone;
        // skip the time rate and the turn rates
        p += GetHeader()->nTurnNum * 2 + 2;
        return reinterpret_cast<const short *>(p)[nTurn];
    }

    long CHBData::GetWaitBallEntryData(long nTimeZone, long nEntry) const
    {
        if (nTimeZone < 0 || nTimeZone >= GetHeader()->nTimeZoneNum) {
            return -1;
        }
        if (nEntry < 0 || nEntry >= GetHeader()->nWaitEntryNum) {
            return -1;
        }
        const unsigned char *p = GetTimeZoneTop();
        p += m_nTimeZoneSize * nTimeZone;
        // skip the time rate, the turn rates and the group entries
        p += GetHeader()->nTurnNum * 4 + 2;
        return reinterpret_cast<const short *>(p)[nEntry];
    }

    long CHBData::GetGroupCombiCount(long nGroup) const
    {
        if (nGroup < 0 || nGroup >= GetHeader()->nGroupNum) {
            return -1;
        }
        return ReadShort(GetGroupOffset(nGroup));
    }

    long CHBData::GetGroupBallCount(long nGroup) const
    {
        if (nGroup < 0 || nGroup >= GetHeader()->nGroupNum) {
            return -1;
        }
        return ReadShort(GetGroupOffset(nGroup) + 2);
    }

    long CHBData::GetGroupCombiRate(long nGroup, long nCombi) const
    {
        if (nGroup < 0 || nGroup >= GetHeader()->nGroupNum) {
            return -1;
        }
        return ReadShort(GetCombiOffset(nGroup, nCombi));
    }

    long CHBData::GetGroupCombiTime(long nGroup, long nCombi) const
    {
        if (nGroup < 0 || nGroup >= GetHeader()->nGroupNum) {
            return -1;
        }
        return ReadShort(GetCombiOffset(nGroup, nCombi) + 2);
    }

    long CHBData::GetGroupCombiWaitMin(long nGroup, long nCombi) const
    {
        if (nGroup < 0 || nGroup >= GetHeader()->nGroupNum) {
            return -1;
        }
        return ReadShort(GetCombiOffset(nGroup, nCombi) + 4);
    }

    long CHBData::GetGroupCombiWaitMax(long nGroup, long nCombi) const
    {
        if (nGroup < 0 || nGroup >= GetHeader()->nGroupNum) {
            return -1;
        }
        return ReadShort(GetCombiOffset(nGroup, nCombi) + 6);
    }

    long CHBData::GetGroupCombiBallKind(long nGroup, long nCombi, long nBall) const
    {
        if (nGroup < 0 || nGroup >= GetHeader()->nGroupNum) {
            return -1;
        }
        long nOffset = GetCombiOffset(nGroup, nCombi) + 8; // the ball kinds follow the four shorts
        nOffset += nBall * 2;
        return ReadShort(nOffset);
    }

}
}
