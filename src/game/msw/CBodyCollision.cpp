#include "game/msw/CBodyCollision.hpp"

#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollUnit.hpp"

// The body collision set: an array of msw::CCollBodyUnit. Every Set*All
// applies one value to all units; the per-unit functions ignore an index out
// of range. Names without a symbol are guesses. The collision classes are one
// file in the original (see notes on CDefCollision); this unit holds the
// CBodyCollision slice.

namespace msw {

    namespace {
        // (name guessed) Brings an angle into [0, 2 pi).
        inline float WrapRad(float rot)
        {
            while (rot > 6.2831855f) {
                rot -= 6.2831855f;
            }
            while (rot < 0.0f) {
                rot += 6.2831855f;
            }
            return rot;
        }

        // CCollUnit::SetPos, which the original inlines here: a moving unit
        // keeps its previous position before taking the new one.
        inline void SetUnitPos(CCollBodyUnit &rUnit, const bcs::FVEC3 &rPos)
        {
            if ((rUnit.m_nFlag10 & 4) && rUnit.m_nCollType == 0) {
                if (rUnit.m_nFlag10 & 2) {
                    rUnit.m_PrevPos.x = rPos.x;
                    rUnit.m_PrevPos.y = rPos.y;
                    rUnit.m_PrevPos.z = rPos.z;
                    rUnit.m_nFlag10 &= ~2;
                } else {
                    rUnit.m_PrevPos.x = rUnit.m_Pos.x;
                    rUnit.m_PrevPos.y = rUnit.m_Pos.y;
                    rUnit.m_PrevPos.z = rUnit.m_Pos.z;
                }
            }
            rUnit.m_Pos.x = rPos.x;
            rUnit.m_Pos.y = rPos.y;
            rUnit.m_Pos.z = rPos.z;
        }

        // Both ends of a capsule.
        inline void SetUnitPos(CCollBodyUnit &rUnit, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rPos2)
        {
            SetUnitPos(rUnit, rPos);
            rUnit.m_Pos2.x = rPos2.x;
            rUnit.m_Pos2.y = rPos2.y;
            rUnit.m_Pos2.z = rPos2.z;
        }

        // Moves a unit of shape 3 in XZ, its previous position with it.
        inline void AddUnitPosXZ(CCollBodyUnit &rUnit, const bcs::FVEC3 &rMove)
        {
            if ((rUnit.m_nFlag10 & 4) && rUnit.GetCollType() == 3) {
                if (rUnit.m_nFlag10 & 2) {
                    rUnit.m_PrevPos.x += rMove.x;
                    rUnit.m_PrevPos.z += rMove.z;
                    rUnit.m_nFlag10 &= ~2;
                } else {
                    rUnit.m_PrevPos.x = rUnit.m_Pos.x;
                    rUnit.m_PrevPos.z = rUnit.m_Pos.z;
                }
            }
            rUnit.m_Pos.x += rMove.x;
            rUnit.m_Pos.z += rMove.z;
        }

        // The same, moving the other end of the capsule by rMove2.
        inline void AddUnitPosXZ(CCollBodyUnit &rUnit, const bcs::FVEC3 &rMove, const bcs::FVEC3 &rMove2)
        {
            AddUnitPosXZ(rUnit, rMove);
            rUnit.m_Pos2.x += rMove2.x;
            rUnit.m_Pos2.z += rMove2.z;
        }

        // The unit's setters of its callbacks and direction, inlined here.
        inline void SetUnitBodyDamageCallback(CCollBodyUnit &rUnit, CBodyCollision::BodyDamageCallback pFunc,
                                              void *pArg)
        {
            rUnit.m_pBodyDamageCallback = pFunc;
            rUnit.m_pBodyDamageArg = pArg;
        }

        inline void SetUnitBodyPushCallback(CCollBodyUnit &rUnit, CBodyCollision::BodyPushCallback pFunc, void *pArg)
        {
            rUnit.m_pBodyPushCallback = pFunc;
            rUnit.m_pBodyPushArg = pArg;
        }

        inline void SetUnitBallCatchCallback(CCollBodyUnit &rUnit, CBodyCollision::BallCatchCallback pFunc, void *pArg)
        {
            rUnit.m_pBallCatchCallback = pFunc;
            rUnit.m_pBallCatchArg = pArg;
        }

        inline void SetUnitRotY(CCollBodyUnit &rUnit, float fRotY)
        {
            rUnit.m_fRotY = WrapRad(fRotY);
        }
    }

    CBodyCollision::CBodyCollision() : m_pColl(0), m_nNum(0)
    {
    }

    CBodyCollision::CBodyCollision(long num, long kind, long type, unsigned long arg) : m_pColl(0), m_nNum(0)
    {
        m_pColl = new (0x100) CCollBodyUnit[num];
        m_nNum = num;
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nOwnerKind = kind;
            m_pColl[i].m_nCollType = type;
            m_pColl[i].m_nOwnerHandle = arg;
        }
    }

    CBodyCollision::~CBodyCollision()
    {
        if (m_pColl != 0) {
            delete[] m_pColl;
        }
        m_pColl = 0;
        m_nNum = 0;
    }

    void CBodyCollision::Create(long num, long kind, long type, unsigned long arg)
    {
        m_pColl = new (0x100) CCollBodyUnit[num];
        m_nNum = num;
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nOwnerKind = kind;
            m_pColl[i].m_nCollType = type;
            m_pColl[i].m_nOwnerHandle = arg;
        }
    }

    CCollUnit *CBodyCollision::GetColl(long index) const
    {
        return m_pColl[index].GetCollUnit();
    }

    void CBodyCollision::SetOwnerHandleAll(unsigned long handle)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nOwnerHandle = handle;
        }
    }

    void CBodyCollision::SetCollType(long index, long type)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nCollType = type;
    }

    void CBodyCollision::SetEnable(long index)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nFlag10 |= 1;
    }

    void CBodyCollision::SetEnableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nFlag10 |= 1;
        }
    }

    int CBodyCollision::IsEnable(long index) const
    {
        if (index < 0 || index >= m_nNum) {
            return false;
        }
        return m_pColl[index].m_nFlag10 & 1;
    }

    void CBodyCollision::SetDisable(long index)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nFlag10 &= ~1;
    }

    void CBodyCollision::SetDisableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nFlag10 &= ~1;
        }
    }

    void CBodyCollision::SetPos(long index, const bcs::FVEC3 &rPos)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        SetUnitPos(m_pColl[index], rPos);
    }

    void CBodyCollision::SetPosAll(const bcs::FVEC3 &rPos)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            SetUnitPos(m_pColl[i], rPos);
        }
    }

    // Both ends of a capsule.
    void CBodyCollision::SetPos(long index, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rPos2)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        SetUnitPos(m_pColl[index], rPos, rPos2);
    }

    void CBodyCollision::AddPosXZAll(const bcs::FVEC3 &rMove)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            AddUnitPosXZ(m_pColl[i], rMove);
        }
    }

    // The same, moving the other end of each capsule by rMove2.
    void CBodyCollision::AddPosXZAll(const bcs::FVEC3 &rMove, const bcs::FVEC3 &rMove2)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            AddUnitPosXZ(m_pColl[i], rMove, rMove2);
        }
    }

    void CBodyCollision::SetSize(long index, const bcs::FVEC3 &rSize)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_Size = rSize;
    }

    void CBodyCollision::SetSizeAll(const bcs::FVEC3 &rSize)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_Size = rSize;
        }
    }

    void CBodyCollision::SetSize(long index, float x, float y, float z)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_Size.Set(x, y, z);
    }

    void CBodyCollision::SetSizeAll(float x, float y, float z)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_Size.Set(x, y, z);
        }
    }

    // A sphere: the radius is the size's x.
    void CBodyCollision::SetRadius(long index, float fRadius)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_Size.Set(fRadius, 0.0f, 0.0f);
    }

    void CBodyCollision::SetRadiusAll(float fRadius)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_Size.Set(fRadius, 0.0f, 0.0f);
        }
    }

    float CBodyCollision::GetRadius(long index) const
    {
        if (index < 0 || index >= m_nNum) {
            return 0.0f;
        }
        return m_pColl[index].m_Size.x;
    }

    void CBodyCollision::SetTargetFlag(long index, unsigned long flag)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nTargetFlag = flag;
    }

    void CBodyCollision::SetTargetFlagAll(unsigned long flag)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nTargetFlag = flag;
        }
    }

    void CBodyCollision::AddTargetFlag(long index, unsigned long flag)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nTargetFlag |= flag;
    }

    void CBodyCollision::AddTargetFlagAll(unsigned long flag)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nTargetFlag |= flag;
        }
    }

    void CBodyCollision::ResetTargetFlag(long index, unsigned long flag)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nTargetFlag &= ~flag;
    }

    void CBodyCollision::ResetTargetFlagAll(unsigned long flag)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nTargetFlag &= ~flag;
        }
    }

    unsigned long CBodyCollision::GetTargetFlag(long index) const
    {
        if (index < 0 || index >= m_nNum) {
            return 0;
        }
        return m_pColl[index].m_nTargetFlag;
    }

    void CBodyCollision::SetPushEnable(long index)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nBodyFlag |= 2;
    }

    void CBodyCollision::SetPushEnableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nBodyFlag |= 2;
        }
    }

    void CBodyCollision::SetPushDisable(long index)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nBodyFlag &= ~2;
    }

    void CBodyCollision::SetPushDisableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nBodyFlag &= ~2;
        }
    }

    void CBodyCollision::SetBodyEnable(long index)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nBodyFlag |= 1;
    }

    int CBodyCollision::IsBodyEnable(long index) const
    {
        if (index < 0 || index >= m_nNum) {
            return false;
        }
        return m_pColl[index].m_nBodyFlag & 1;
    }

    void CBodyCollision::SetBodyDisable(long index)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nBodyFlag &= ~1;
    }

    void CBodyCollision::SetBodyDisableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nBodyFlag &= ~1;
        }
    }

    void CBodyCollision::SetCatchEnable(long index)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nBodyFlag |= 4;
    }

    void CBodyCollision::SetCatchDisable(long index)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nBodyFlag &= ~4;
    }

    void CBodyCollision::SetCatchDisableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nBodyFlag &= ~4;
        }
    }

    void CBodyCollision::SetAttackEnable(long index)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nBodyFlag |= 8;
    }

    void CBodyCollision::SetAttackEnableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nBodyFlag |= 8;
        }
    }

    void CBodyCollision::SetAttackDisableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nBodyFlag &= ~0x108;
        }
    }

    void CBodyCollision::SetPushRank(long index, long rank)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nUnkA0 = rank;
    }

    void CBodyCollision::SetPushRankAll(long rank)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nUnkA0 = rank;
        }
    }

    void CBodyCollision::SetPushPower(long index, long power)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nPushPower = power;
    }

    void CBodyCollision::SetPushPowerAll(long power)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nPushPower = power;
        }
    }

    void CBodyCollision::SetBodyDamageCallbackAll(BodyDamageCallback pFunc, void *pArg)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            SetUnitBodyDamageCallback(m_pColl[i], pFunc, pArg);
        }
    }

    void CBodyCollision::SetBodyPushCallback(long index, BodyPushCallback pFunc, void *pArg)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        SetUnitBodyPushCallback(m_pColl[index], pFunc, pArg);
    }

    void CBodyCollision::SetBodyPushCallbackAll(BodyPushCallback pFunc, void *pArg)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            SetUnitBodyPushCallback(m_pColl[i], pFunc, pArg);
        }
    }

    void CBodyCollision::SetBallCatchCallback(long index, BallCatchCallback pFunc, void *pArg)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        SetUnitBallCatchCallback(m_pColl[index], pFunc, pArg);
    }

    void CBodyCollision::SetBallCatchCallbackAll(BallCatchCallback pFunc, void *pArg)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            SetUnitBallCatchCallback(m_pColl[i], pFunc, pArg);
        }
    }

    void CBodyCollision::SetIndex(long index, long nIndex)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nOwnerIdx = nIndex;
    }

    void CBodyCollision::SetIndexAll(long nIndex)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nOwnerIdx = nIndex;
        }
    }

    void CBodyCollision::SetTeamNoAll(long nTeamNo)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nTeamNo = nTeamNo;
        }
    }

    void CBodyCollision::SetChrIdxAll(long nChr)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nUnk17 = nChr;
        }
    }

    void CBodyCollision::SetMyNoHitFlagAll(unsigned long flag)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nMyNoHitFlag = flag;
        }
    }

    void CBodyCollision::SetNoHitFlag(long index, unsigned long flag)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nNoHitFlag = flag;
    }

    void CBodyCollision::SetNoHitFlagAll(unsigned long flag)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nNoHitFlag = flag;
        }
    }

    void CBodyCollision::SetTmpNoHitFlag(long index, unsigned long flag)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nTmpNoHitFlag = flag;
    }

    void CBodyCollision::SetTmpNoHitFlagAll(unsigned long flag)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nTmpNoHitFlag = flag;
        }
    }

    void CBodyCollision::SetYRot(long index, float fRotY)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        SetUnitRotY(m_pColl[index], fRotY);
    }

    // The angle is wrapped once here and again by each unit's setter.
    void CBodyCollision::SetYRotAll(float fRotY)
    {
        fRotY = WrapRad(fRotY);
        for (unsigned long i = 0; i < m_nNum; i++) {
            SetUnitRotY(m_pColl[i], fRotY);
        }
    }

    void CBodyCollision::CalcAABBAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].GetCollUnit()->CalcAABB();
        }
    }

    void CBodyCollision::SetAtkTypeAll(unsigned long nType)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nAtkType = nType;
        }
    }

    void CBodyCollision::SetHitSeIDAll(long nSe)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nHitSe = nSe;
        }
    }
}
