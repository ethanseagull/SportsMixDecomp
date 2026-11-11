#include "bcs/CListNodeBase.hpp"

namespace bcs {

    void CListNodeBase::Link(CListNodeBase *pNode)
    {
        Unlink();
        PreLink();

        m_pNext = pNode;
        m_pPrev = pNode->m_pPrev;
        m_pList = pNode->m_pList;
        pNode->m_pPrev = this;
        if (m_pPrev != 0) {
            m_pPrev->m_pNext = this;
        }
        if (m_pList != 0) {
            m_pList->m_nCount++;
        }

        PostLink();
    }

    CListNodeBase *CListNodeBase::Unlink()
    {
        if (m_pNext == 0 && m_pPrev == 0) {
            return 0;
        }

        PreUnlink();

        CListNodeBase *pNext = m_pNext;
        if (pNext != 0) {
            pNext->m_pPrev = m_pPrev;
        }
        if (m_pPrev != 0) {
            m_pPrev->m_pNext = m_pNext;
        }
        if (m_pList != 0) {
            m_pList->m_nCount--;
        }
        m_pNext = 0;
        m_pPrev = 0;
        m_pList = 0;

        PostUnlink();
        return pNext;
    }

    CListNodeBase *CListNodeBase::GetBegin()
    {
        if (m_pList == 0) {
            return 0;
        }
        return m_pList->GetFirst();
    }

    const CListNodeBase *CListNodeBase::GetBegin() const
    {
        if (m_pList == 0) {
            return 0;
        }
        return m_pList->GetFirst();
    }
}
