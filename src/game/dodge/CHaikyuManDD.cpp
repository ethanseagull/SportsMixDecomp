// msw::CHaikyuManDD: the ball supply of the Dodge or Die party game. See
// the header for what it does.
#include "game/msw/CHaikyuManDD.hpp"

#include "bcs/BCS_Rand.hpp"

namespace msw {

    bcs::FVEC3 dummy;

    CHaikyuManDD::CHaikyuManDD()
    {
        for (int i = 0; i < HOUDAI_MAX; i++) {
            m_aSlot[i].m_fTimer = 0.0f;
            m_aSlot[i].m_pEntry = 0;
        }
        m_pCallback = 0;
        m_pData = 0;
        m_fFrame = 0.0f;
        m_nIndex = 0;
    }

    CHaikyuManDD::~CHaikyuManDD()
    {
    }

    void CHaikyuManDD::SetData(void *pData)
    {
        m_pData = static_cast<DATA *>(pData);
        m_fFrame = 0.0f;
        m_nIndex = 0;
        Seek();
    }

    void CHaikyuManDD::SetCallback(CALLBACK pCallback)
    {
        m_pCallback = pCallback;
    }

    void CHaikyuManDD::Run(float fStep, void *pUser)
    {
        m_fFrame += fStep;

        // Arm the cannons whose entries are due.
        if (m_nIndex < m_pData->m_nEntryCount) {
            do {
                const ENTRY *pEntry = m_pData->GetEntry(m_nIndex);
                if (!(pEntry->m_fTime < m_fFrame)) {
                    break;
                }
                m_aSlot[pEntry->m_nHoudai].m_fTimer = bcs::BCS_RandRange(0.0f, pEntry->m_fDelayMax);
                m_aSlot[pEntry->m_nHoudai].m_pEntry = pEntry;
            } while (++m_nIndex < m_pData->m_nEntryCount);
        }

        // Fire the cannons whose delay has run out.
        for (int i = 0; i < HOUDAI_MAX; i++) {
            if (m_aSlot[i].m_pEntry == 0) {
                continue;
            }
            m_aSlot[i].m_fTimer -= fStep;
            if (m_aSlot[i].m_fTimer <= 0.0f) {
                if (m_pCallback != 0) {
                    const ENTRY *pEntry = m_aSlot[i].m_pEntry;
                    BOMB_ID id = SelectBomb(pEntry, bcs::BCS_RandRange(0.0f, 99.99f));
                    if (id != BOMB_NONE) {
                        m_pCallback(pEntry->m_nHoudai, id, pEntry->m_nParam, pUser);
                    }
                }
                m_aSlot[i].m_pEntry = 0;
            }
        }
    }

    void CHaikyuManDD::Reset(int nFrame)
    {
        m_fFrame = nFrame;
        m_nIndex = 0;
        Seek();
    }

    bool CHaikyuManDD::IsOver() const
    {
        if (m_pData == 0) {
            return true;
        }
        return m_fFrame > m_pData->m_fTotalFrame;
    }

    const bcs::FVEC3 *CHaikyuManDD::GetHoudaiPosition(int i) const
    {
        if (m_pData == 0) {
            return &dummy;
        }
        return &m_pData->m_aHoudaiPos[i];
    }

    int CHaikyuManDD::GetHoudaiCount() const
    {
        return m_pData->m_nHoudaiCount;
    }

    float CHaikyuManDD::GetTotalFrame() const
    {
        if (m_pData == 0) {
            return 0.0f;
        }
        return m_pData->m_fTotalFrame;
    }
}
