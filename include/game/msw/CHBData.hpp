#ifndef MSW_CHBDATA_HPP_
#define MSW_CHBDATA_HPP_

namespace msw {
namespace BA_PARTY {

    // Reader over the basketball party game's ("HB") binary data blob.
    // The blob is not copied; CHBData keeps a pointer to it.
    //
    // Layout of the blob (all values big-endian, offsets from its start).
    // The accessors return -1 for a time zone, turn, entry or group out of range
    // (combination and ball indices are not checked). Their byte-offset and
    // pointer-cursor arithmetic is written in the order the original computes it.
    //   0x00  8 bytes not read here
    //   0x08  short  number of time zones
    //   0x0A  short  number of turns
    //   0x0C  short  number of wait-ball entries
    //   0x0E  short  number of groups
    //   0x10  time zone records, one per time zone, each of m_nTimeZoneSize bytes:
    //           short  time rate
    //           short  turn rate      [turns]
    //           short  group entry    [turns]
    //           short  wait-ball entry[wait-ball entries]
    //   then  long   offset of each group from the start of the blob [groups]
    //   group: short  number of combinations
    //          short  number of balls
    //          combination records, each 8 + 2 * balls bytes:
    //            short  rate
    //            short  time
    //            short  minimum wait
    //            short  maximum wait
    //            short  ball kind [balls]
    class CHBData {
        public:
            struct HEADER {
                unsigned char unk0[8];
                short nTimeZoneNum;
                short nTurnNum;
                short nWaitEntryNum;
                short nGroupNum;
            };

            CHBData();
            ~CHBData();

            void SetData(const unsigned char *pData);

            long GetTimeZoneCount() const;
            long GetTurnCount() const;
            long GetWaitEntryCount() const;

            long GetTimeRate(long nTimeZone) const;
            long GetTurnRateData(long nTimeZone, long nTurn) const;
            long GetGroupEntryData(long nTimeZone, long nTurn) const;
            long GetWaitBallEntryData(long nTimeZone, long nEntry) const;

            long GetGroupCombiCount(long nGroup) const;
            long GetGroupBallCount(long nGroup) const;
            long GetGroupCombiRate(long nGroup, long nCombi) const;
            long GetGroupCombiTime(long nGroup, long nCombi) const;
            long GetGroupCombiWaitMin(long nGroup, long nCombi) const;
            long GetGroupCombiWaitMax(long nGroup, long nCombi) const;
            long GetGroupCombiBallKind(long nGroup, long nCombi, long nBall) const;

        private:
            const HEADER *GetHeader() const { return reinterpret_cast<const HEADER *>(m_pData); }

            // Start of the time zone records.
            const unsigned char *GetTimeZoneTop() const {
                const unsigned char *p = m_pData;
                p += sizeof(HEADER);
                return p;
            }

            // A short at byte offset `nOffset` of the blob.
            short ReadShort(long nOffset) const { return *reinterpret_cast<const short *>(&m_pData[nOffset]); }

            // Byte offset of group `nGroup` in the blob, 0 when out of range.
            long GetGroupOffset(long nGroup) const {
                if (nGroup < 0 || nGroup >= GetHeader()->nGroupNum) {
                    return 0;
                }
                const long *pGroupOffsets = reinterpret_cast<const long *>(&m_pData[sizeof(HEADER) + m_nTimeZoneSize * GetHeader()->nTimeZoneNum]);
                return pGroupOffsets[nGroup];
            }

            // Size in bytes of one combination record of group `nGroup`, 0 when out of range.
            long GetCombiSize(long nGroup) const {
                if (nGroup < 0 || nGroup >= GetHeader()->nGroupNum) {
                    return 0;
                }
                return GetGroupBallCount(nGroup) * 2 + 8;
            }

            // Byte offset of combination `nCombi` of group `nGroup` in the blob.
            long GetCombiOffset(long nGroup, long nCombi) const {
                return GetGroupOffset(nGroup) + 4 + nCombi * GetCombiSize(nGroup);
            }

            const unsigned char *m_pData; // 0x00
            long m_nTimeZoneSize;         // 0x04 size in bytes of one time zone record
    };

}
}

#endif /* !MSW_CHBDATA_HPP_ */
