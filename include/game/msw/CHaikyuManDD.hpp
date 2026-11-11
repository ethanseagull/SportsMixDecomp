#ifndef GAME_MSW_CHAIKYUMANDD_HPP_
#define GAME_MSW_CHAIKYUMANDD_HPP_

#include "bcs/FVEC3.hpp"

// msw::CHaikyuManDD: the ball supply ("haikyu") of the Dodge or Die party
// game (DodgeF game/dodge/CHaikyuManDD.cpp). It plays a timetable read from
// a data file: each entry arms one cannon ("houdai") at its time, and when
// the cannon's random delay runs out a bomb kind is drawn by weight and
// handed to the callback. Field and type names are guesses; the layout is
// from the unit's functions.

namespace msw {

    // A position the unit hands out when no data is set (DodgeF .bss,
    // 0x10 bytes with alignment; defined in CHaikyuManDD.cpp).
    extern bcs::FVEC3 dummy;

    class CHaikyuManDD {
        public:
            // Kind of bomb a cannon fires; 0 means none.
            enum BOMB_ID {
                BOMB_NONE = 0
            };

            enum {
                BOMB_KIND_MAX = 6,
                HOUDAI_MAX = 16
            };

            // One timetable entry (0x28 bytes).
            struct ENTRY {
                int m_nHoudai;                    // 0x00 cannon number
                float m_fTime;                    // 0x04 frame it is armed at
                float m_fDelayMax;                // 0x08 random delay up to this
                float m_afWeight[BOMB_KIND_MAX];  // 0x0C chance of each bomb kind, in percent
                int m_nParam;                     // 0x24 passed to the callback
            };

            // The data file: a header, the cannon positions, then the
            // entries.
            struct DATA {
                int m_nUnk00;          // 0x00
                int m_nHoudaiCount;    // 0x04
                int m_nEntryCount;     // 0x08
                float m_fTotalFrame;   // 0x0C
                bcs::FVEC3 m_aHoudaiPos[1]; // 0x10 m_nHoudaiCount of them, then the entries

                const ENTRY *GetEntry(int i) const
                {
                    return reinterpret_cast<const ENTRY *>(&m_aHoudaiPos[m_nHoudaiCount]) + i;
                }
            };

            typedef void (*CALLBACK)(int nHoudai, BOMB_ID id, int nParam, void *pUser);

            // A cannon that has been armed and waits for its delay.
            struct SLOT {
                float m_fTimer;         // 0x00
                const ENTRY *m_pEntry;  // 0x04 0 when idle
            };

            CHaikyuManDD();
            ~CHaikyuManDD();

            void SetData(void *pData);
            void SetCallback(CALLBACK pCallback);
            void Run(float fStep, void *pUser);
            void Reset(int nFrame);
            bool IsOver() const;
            const bcs::FVEC3 *GetHoudaiPosition(int i) const;
            int GetHoudaiCount() const;
            float GetTotalFrame() const;

        private:
            // Skips the entries whose time is already past.
            void Seek()
            {
                while (m_pData->GetEntry(m_nIndex)->m_fTime < m_fFrame) {
                    m_nIndex++;
                    if (m_nIndex >= m_pData->m_nEntryCount) {
                        break;
                    }
                }
            }

            static BOMB_ID SelectBomb(const ENTRY *pEntry, float fRand)
            {
                for (int i = 0; i < BOMB_KIND_MAX; i++) {
                    if (fRand < pEntry->m_afWeight[i]) {
                        return static_cast<BOMB_ID>(i);
                    }
                    fRand -= pEntry->m_afWeight[i];
                }
                return BOMB_NONE;
            }

            DATA *m_pData;             // 0x00
            CALLBACK m_pCallback;      // 0x04
            float m_fFrame;            // 0x08
            int m_nIndex;              // 0x0C next entry
            SLOT m_aSlot[HOUDAI_MAX];  // 0x10
    };
}

#endif /* !GAME_MSW_CHAIKYUMANDD_HPP_ */
