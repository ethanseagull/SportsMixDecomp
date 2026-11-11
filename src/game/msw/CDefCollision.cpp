#include "game/msw/CDefCollision.hpp"

#include "bcs/New.hpp"

namespace msw {

    // (name guessed) Brings an angle into [0, 2 pi), as CCollUnit's inline
    // SetRotYWrapped does for the unit.
    static inline float WrapRad(float rot)
    {
        while (rot > 6.2831855f) {
            rot -= 6.2831855f;
        }
        while (rot < 0.0f) {
            rot += 6.2831855f;
        }
        return rot;
    }

    CDefCollision::CDefCollision() : m_pColl(0), m_nNum(0)
    {
    }

    CDefCollision::CDefCollision(long num, long kind, long type, unsigned long arg) : m_pColl(0), m_nNum(0)
    {
        m_pColl = new (0x100) CCollDefUnit[num];
        m_nNum = num;
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nCollKind = kind;
        }
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nCollType = type;
        }
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nUnk0C = arg;
        }
    }

    CDefCollision::~CDefCollision()
    {
        if (m_pColl != 0) {
            delete[] m_pColl;
        }
        m_pColl = 0;
        m_nNum = 0;
    }

    void CDefCollision::Create(long num, long kind, long type, unsigned long arg)
    {
        m_pColl = new (0x100) CCollDefUnit[num];
        m_nNum = num;
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nCollKind = kind;
        }
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nCollType = type;
        }
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nUnk0C = arg;
        }
    }

    CCollUnit *CDefCollision::GetColl(long index) const
    {
        return &m_pColl[index];
    }

    void CDefCollision::SetEnableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].OnFlag10(1);
        }
    }

    void CDefCollision::SetDisableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].OffFlag10(1);
        }
    }

    inline void CDefCollision::SetUnitPos_(CCollUnit &rUnit, const bcs::FVEC3 &rPos)
    {
        // Keep the previous position for a moving unit before taking the
        // new one.
        if ((rUnit.m_nFlag10 & 4) && rUnit.m_nCollType == 0) {
            if (rUnit.m_nFlag10 & 2) {
                rUnit.m_PrevPos = rPos;
                rUnit.m_nFlag10 &= ~2;
            } else {
                rUnit.m_PrevPos = rUnit.m_Pos;
            }
        }
        rUnit.m_Pos = rPos;
    }

    void CDefCollision::SetPosAll(const bcs::FVEC3 &rPos)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            SetUnitPos_(m_pColl[i], rPos);
        }
    }

    void CDefCollision::SetSizeAll(const bcs::FVEC3 &rSize)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].SetSize(rSize);
        }
    }

    void CDefCollision::SetYRotAll(float rot)
    {
        rot = WrapRad(rot);
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].SetRotYWrapped(rot);
        }
    }

    void CDefCollision::SetIndexAll(long nIndex)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nIndex = nIndex;
        }
    }

    void CDefCollision::SetTeamNoAll(long nTeamNo)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nTeamNo = nTeamNo;
        }
    }

    void CDefCollision::SetReflectDisableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].OffFlagA0(1);
        }
    }

    void CDefCollision::SetMyNoHitFlagAll(unsigned long flag)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nNoHitFlag = flag;
        }
    }

    inline void CDefCollision::SetUnitReflectHitCallback_(CCollDefUnit &rUnit,
                                                          CCollDefUnit::ReflectHitCallback pCallback, void *pArg)
    {
        rUnit.m_pReflectHitCallback = pCallback;
        rUnit.m_pReflectHitArg = pArg;
    }

    void CDefCollision::SetReflectHitCallbackAll(CCollDefUnit::ReflectHitCallback pCallback, void *pArg)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            SetUnitReflectHitCallback_(m_pColl[i], pCallback, pArg);
        }
    }
}
