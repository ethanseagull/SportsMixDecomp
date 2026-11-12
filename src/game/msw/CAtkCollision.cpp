#include "game/msw/CAtkCollision.hpp"

#include "bcs/New.hpp"

namespace msw {

    // (name guessed) Brings an angle into [0, 2 pi), as CDefCollision does
    // for its units.
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

    CAtkCollision::CAtkCollision() : m_pColl(0), m_nNum(0)
    {
    }

    CAtkCollision::CAtkCollision(long num, long kind, long type, unsigned long arg) : m_pColl(0), m_nNum(0)
    {
        m_pColl = new (0x100) CCollAtkUnit[num];
        m_nNum = num;
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nOwnerKind = kind;
            m_pColl[i].m_nCollType = type;
            m_pColl[i].m_nUnk0C = arg;
        }
    }

    CAtkCollision::~CAtkCollision()
    {
        if (m_pColl != 0) {
            delete[] m_pColl;
        }
        m_pColl = 0;
        m_nNum = 0;
    }

    void CAtkCollision::Create(long num, long kind, long type, unsigned long arg)
    {
        m_pColl = new (0x100) CCollAtkUnit[num];
        m_nNum = num;
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nOwnerKind = kind;
            m_pColl[i].m_nCollType = type;
            m_pColl[i].m_nUnk0C = arg;
        }
    }

    void CAtkCollision::Destroy()
    {
        if (m_pColl != 0) {
            delete[] m_pColl;
        }
        m_pColl = 0;
        m_nNum = 0;
    }

    CCollUnit *CAtkCollision::GetColl(long index) const
    {
        return m_pColl[index].GetCollUnit();
    }

    void CAtkCollision::SetCollKind(long index, long kind)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nOwnerKind = kind;
    }

    void CAtkCollision::SetCollType(long index, long type)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nCollType = type;
    }

    void CAtkCollision::SetCollTypeAll(long type)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nCollType = type;
        }
    }

    void CAtkCollision::SetEnable(long index)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nFlag10 |= 1;
    }

    void CAtkCollision::SetEnableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nFlag10 |= 1;
        }
    }

    bool CAtkCollision::IsEnable(long index) const
    {
        if (index < 0 || index >= m_nNum) {
            return false;
        }
        return m_pColl[index].m_nFlag10 & 1;
    }

    void CAtkCollision::SetDisable(long index)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nFlag10 &= ~1;
    }

    void CAtkCollision::SetDisableAll()
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nFlag10 &= ~1;
        }
    }

    inline void CAtkCollision::SetUnitPos_(CCollAtkUnit &rUnit, const bcs::FVEC3 &rPos)
    {
        // Keep the previous position for a moving unit before taking the
        // new one.
        if ((rUnit.m_nFlag10 & 4) && rUnit.m_nCollType == 0) {
            if (rUnit.m_nFlag10 & 2) {
                rUnit.m_PrevPos = rPos;
                rUnit.m_nFlag10 &= ~2;
            } else {
                rUnit.m_PrevPos.x = rUnit.m_fPosX;
                rUnit.m_PrevPos.y = rUnit.m_fPosY;
                rUnit.m_PrevPos.z = rUnit.m_fPosZ;
            }
        }
        rUnit.m_fPosX = rPos.x;
        rUnit.m_fPosY = rPos.y;
        rUnit.m_fPosZ = rPos.z;
    }

    void CAtkCollision::SetPos(long index, const bcs::FVEC3 &rPos)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        SetUnitPos_(m_pColl[index], rPos);
    }

    void CAtkCollision::SetPosAll(const bcs::FVEC3 &rPos)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            SetUnitPos_(m_pColl[i], rPos);
        }
    }

    // Both ends of a capsule.
    inline void CAtkCollision::SetUnitPos_(CCollAtkUnit &rUnit, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rPos2)
    {
        SetUnitPos_(rUnit, rPos);
        rUnit.m_Pos2.x = rPos2.x;
        rUnit.m_Pos2.y = rPos2.y;
        rUnit.m_Pos2.z = rPos2.z;
    }

    void CAtkCollision::SetPos(long index, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rPos2)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        SetUnitPos_(m_pColl[index], rPos, rPos2);
    }

    void CAtkCollision::SetPosAll(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rPos2)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            SetUnitPos_(m_pColl[i], rPos, rPos2);
        }
    }

    void CAtkCollision::SetPos2(long index, const bcs::FVEC3 &rPos)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_Pos2 = rPos;
    }

    void CAtkCollision::SetSize(long index, const bcs::FVEC3 &rSize)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_Size = rSize;
    }

    void CAtkCollision::SetSizeAll(const bcs::FVEC3 &rSize)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_Size = rSize;
        }
    }

    void CAtkCollision::SetSize(long index, float x, float y, float z)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_Size.Set(x, y, z);
    }

    void CAtkCollision::SetSizeAll(float x, float y, float z)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_Size.Set(x, y, z);
        }
    }

    // A sphere: the radius is the size's x.
    void CAtkCollision::SetRadius(long index, float fRadius)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_Size.Set(fRadius, 0.0f, 0.0f);
    }

    void CAtkCollision::SetRadiusAll(float fRadius)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_Size.Set(fRadius, 0.0f, 0.0f);
        }
    }

    float CAtkCollision::GetRadius(long index) const
    {
        if (index < 0 || index >= m_nNum) {
            return 0.0f;
        }
        return m_pColl[index].m_Size.x;
    }

    void CAtkCollision::SetTargetFlag(long index, unsigned long nFlag)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nFlag = nFlag;
    }

    void CAtkCollision::SetTargetFlagAll(unsigned long nFlag)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nFlag = nFlag;
        }
    }

    void CAtkCollision::ResetTargetFlag(long index, unsigned long nFlag)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nFlag &= ~nFlag;
    }

    void CAtkCollision::ResetTargetFlagAll(unsigned long nFlag)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nFlag &= ~nFlag;
        }
    }

    inline void CAtkCollision::SetUnitAttackHitCallback_(CCollAtkUnit &rUnit, AttackHitCallback pFunc, void *pArg)
    {
        rUnit.m_pAttackHitCallback = pFunc;
        rUnit.m_pAttackHitArg = pArg;
    }

    void CAtkCollision::SetAttackHitCallback(long index, AttackHitCallback pFunc, void *pArg)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        SetUnitAttackHitCallback_(m_pColl[index], pFunc, pArg);
    }

    void CAtkCollision::SetAttackHitCallbackAll(AttackHitCallback pFunc, void *pArg)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            SetUnitAttackHitCallback_(m_pColl[i], pFunc, pArg);
        }
    }

    void CAtkCollision::SetIndexAll(long nIndex)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nOwnerNo = nIndex;
        }
    }

    void CAtkCollision::SetUnk15All(long n)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_Unk15 = n;
        }
    }

    void CAtkCollision::SetTeamNoAll(long nTeamNo)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nTeamNo = nTeamNo;
        }
    }

    void CAtkCollision::SetTeamIdx(long index, long nTeam)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nTeamNo = nTeam;
    }

    void CAtkCollision::SetTeamIdxAll(long nTeam)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nTeamNo = nTeam;
        }
    }

    void CAtkCollision::SetChrIdx(long index, long nChr)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nChrNo = nChr;
    }

    void CAtkCollision::SetChrIdxAll(long nChr)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nChrNo = nChr;
        }
    }

    void CAtkCollision::SetMyNoHitFlagAll(unsigned long nFlag)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nMyNoHitFlag = nFlag;
        }
    }

    void CAtkCollision::SetNoHitFlag(long index, unsigned long nFlag)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nNoHitFlag = nFlag;
    }

    void CAtkCollision::SetNoHitFlagAll(unsigned long nFlag)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nNoHitFlag = nFlag;
        }
    }

    void CAtkCollision::SetTmpNoHitFlag(long index, unsigned long nFlag)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nTmpNoHitFlag = nFlag;
    }

    void CAtkCollision::SetTmpNoHitFlagAll(unsigned long nFlag)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nTmpNoHitFlag = nFlag;
        }
    }

    void CAtkCollision::SetAtkRotY(long index, float rot)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        CCollAtkUnit &rUnit = m_pColl[index];
        rUnit.m_nAtkHitFlag |= 1;
        rUnit.m_fAtkRotY = WrapRad(rot);
    }

    float CAtkCollision::GetAtkRotY(long index) const
    {
        if (index < 0 || index >= m_nNum) {
            return 0.0f;
        }
        return m_pColl[index].m_fAtkRotY;
    }

    void CAtkCollision::SetAtkType(long index, unsigned long nType)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nAtkType = nType;
    }

    void CAtkCollision::SetAtkTypeAll(unsigned long nType)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nAtkType = nType;
        }
    }

    void CAtkCollision::ResetAtkHit(long index)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nAtkHitFlag &= ~1;
    }

    bool CAtkCollision::IsAtkHit(long index) const
    {
        if (index < 0 || index >= m_nNum) {
            return false;
        }
        return m_pColl[index].m_nAtkHitFlag & 1;
    }

    void CAtkCollision::SetHitEffectID(long index, long nEffect)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nUnk90 = nEffect;
    }

    void CAtkCollision::SetHitEffectIDAll(long nEffect)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nUnk90 = nEffect;
        }
    }

    void CAtkCollision::SetHitSeID(long index, long nSe)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_nHitSe = nSe;
    }

    void CAtkCollision::SetHitSeIDAll(long nSe)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nHitSe = nSe;
        }
    }

    void CAtkCollision::SetAtkPowerAll(float power)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_fVanishPower = power;
        }
    }

    void CAtkCollision::SetHitEffectFrame(long index, float fFrame)
    {
        if (index < 0 || index >= m_nNum) {
            return;
        }
        m_pColl[index].m_fDownFrame = fFrame;
    }

    void CAtkCollision::SetHitEffectFrameAll(float fFrame)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_fDownFrame = fFrame;
        }
    }

    void CAtkCollision::SetHitCamVibIDAll(long nId)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_nHitCamVibID = nId;
        }
    }

    void CAtkCollision::SetUnk98All(float f)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pColl[i].m_fUnk98 = f;
        }
    }

    inline void CAtkCollision::SetUnitUnk9C_(CCollAtkUnit &rUnit, long n9C, long n9E)
    {
        rUnit.m_nUnk9C = n9C;
        rUnit.m_nUnk9E = n9E;
    }

    void CAtkCollision::SetUnk9CAll(long n9C, long n9E)
    {
        for (unsigned long i = 0; i < m_nNum; i++) {
            SetUnitUnk9C_(m_pColl[i], n9C, n9E);
        }
    }
}
