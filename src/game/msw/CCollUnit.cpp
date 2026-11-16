#include "game/msw/CCollUnit.hpp"

#include <math.h>

#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC2.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollDefUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CGameSnd.hpp"
#include "nw4r/math/triangular.h"
#include "nw4r/ut/List.hpp"

namespace msw {

    nw4r::ut::List CCollMan::m_BodyUnitList;
    nw4r::ut::List CCollMan::m_AtkUnitList;
    nw4r::ut::List CCollMan::m_DefUnitList;

    CCollUnit::CCollUnit(long kind, long type, unsigned long handle)
    {
        m_nOwnerHandle = handle;
        m_nFlag10 = 2;
        m_nOwnerKind = kind;
        m_nCollType = type;
        m_nOwnerIdx = -1;
        m_Unk15 = kind;
        m_nTeamNo = -1;
        m_nChrNo = -1;
        m_nTargetFlag = 0;
        m_nMyNoHitFlag = 0;
        m_nNoHitFlag = 0;
        m_nTmpNoHitFlag = 0;
        m_Unk28 = 0;
        m_fRotY = 0.0f;
        m_nAtkType = 0;
        m_fVanishPower = 0.0f;
        m_nAtkHitFlag = 0;
        m_nHitCamVibID = -1;
        m_nUnk90 = -1;
        m_nHitSe = -1;
        m_fDownFrame = 0.0f;
        m_fUnk98 = 0.0f;
        m_nUnk9C = 0;
        m_nUnk9E = 0;
        m_Link.pPrev = 0;
        m_Link.pNext = 0;
        m_Pos.Set(0.0f, 0.0f, 0.0f);
        m_Pos2.Set(0.0f, 0.0f, 0.0f);
        m_Pos3.Set(0.0f, 0.0f, 0.0f);
        m_PrevPos.Set(0.0f, 0.0f, 0.0f);
        m_Size.Set(0.0f, 0.0f, 0.0f);
        m_AabbMin.Set(0.0f, 0.0f, 0.0f);
        m_AabbMax.Set(0.0f, 0.0f, 0.0f);
    }

    CCollUnit::~CCollUnit()
    {
    }

    void CCollUnit::SetCollKind(long kind)
    {
        m_nOwnerKind = kind;
    }

    void CCollUnit::SetCollType(long type)
    {
        m_nCollType = type;
    }

    void CCollUnit::SetPos(const bcs::FVEC3 &rPos)
    {
        // A moving sphere keeps its previous position, for the swept test.
        if ((m_nFlag10 & 4) && m_nCollType == 0) {
            if (m_nFlag10 & 2) {
                m_PrevPos = rPos;
                m_nFlag10 &= ~2;
            } else {
                m_PrevPos = m_Pos;
            }
        }
        m_Pos = rPos;
    }

    // The box of an upright cylinder of radius m_Size.x and height
    // m_Size.y standing on the unit's position.
    static inline void CalcCylinderAABB(CCollUnit &rUnit)
    {
        rUnit.m_AabbMin.x = rUnit.m_Pos.x - rUnit.m_Size.x;
        rUnit.m_AabbMin.y = rUnit.m_Pos.y;
        rUnit.m_AabbMin.z = rUnit.m_Pos.z - rUnit.m_Size.x;
        rUnit.m_AabbMax.x = rUnit.m_Pos.x + rUnit.m_Size.x;
        rUnit.m_AabbMax.y = rUnit.m_Pos.y + rUnit.m_Size.y;
        rUnit.m_AabbMax.z = rUnit.m_Pos.z + rUnit.m_Size.x;
    }

    void CCollUnit::CalcAABB()
    {
        switch (m_nCollType) {
            case 0: {
                // A sphere; a moving one covers its path from the previous
                // position.
                if ((m_nFlag10 & 4) && !(m_nFlag10 & 2)) {
                    COLL::GetMinMax(m_AabbMin, m_AabbMax, m_PrevPos, m_Pos);
                } else {
                    m_AabbMax = m_Pos;
                    m_AabbMin = m_Pos;
                }
                float r = m_Size.x;
                m_AabbMin.x -= r;
                m_AabbMin.y -= r;
                m_AabbMin.z -= r;
                m_AabbMax.x += r;
                m_AabbMax.y += r;
                m_AabbMax.z += r;
                break;
            }
            case 1:
                CalcCylinderAABB(*this);
                break;
            case 2: {
                // An upright capsule lying along the longer of x and z of
                // m_Size, turned by m_fRotY; a cylinder when both are equal.
                if (m_Size.x == m_Size.z) {
                    CalcCylinderAABB(*this);
                    break;
                }
                float s;
                float c;
                nw4r::math::SinCosRad(&s, &c, m_fRotY);
                float rx = m_Size.x;
                float rz = m_Size.z;
                if (rx < rz) {
                    float len = rz - rx;
                    bcs::FVEC2 start(-len * s, -len * c);
                    bcs::FVEC2 end(len * s, len * c);
                    if (start.x <= end.x) {
                        m_AabbMin.x = start.x - rx;
                        m_AabbMax.x = end.x + rx;
                    } else {
                        m_AabbMin.x = end.x - rx;
                        m_AabbMax.x = start.x + rx;
                    }
                    if (start.y <= end.y) {
                        m_AabbMin.z = start.y - rx;
                        m_AabbMax.z = end.y + rx;
                    } else {
                        m_AabbMin.z = end.y - rx;
                        m_AabbMax.z = start.y + rx;
                    }
                } else {
                    float len = rx - rz;
                    bcs::FVEC2 start(-len * c, len * s);
                    bcs::FVEC2 end(len * c, -len * s);
                    if (start.x <= end.x) {
                        m_AabbMin.x = start.x - rz;
                        m_AabbMax.x = end.x + rz;
                    } else {
                        m_AabbMin.x = end.x - rz;
                        m_AabbMax.x = start.x + rz;
                    }
                    if (start.y <= end.y) {
                        m_AabbMin.z = start.y - rz;
                        m_AabbMax.z = end.y + rz;
                    } else {
                        m_AabbMin.z = end.y - rz;
                        m_AabbMax.z = start.y + rz;
                    }
                }
                m_AabbMin.y = 0.0f;
                m_AabbMax.y = m_Size.y;
                bcs::FVEC3Add(&m_AabbMin, &m_AabbMin, &m_Pos);
                bcs::FVEC3Add(&m_AabbMax, &m_AabbMax, &m_Pos);
                break;
            }
            case 3:
                // A capsule of radius m_Size.x from m_Pos to m_Pos2.
                if (m_Pos.x <= m_Pos2.x) {
                    m_AabbMin.x = m_Pos.x - m_Size.x;
                    m_AabbMax.x = m_Pos2.x + m_Size.x;
                } else {
                    m_AabbMin.x = m_Pos2.x - m_Size.x;
                    m_AabbMax.x = m_Pos.x + m_Size.x;
                }
                if (m_Pos.y <= m_Pos2.y) {
                    m_AabbMin.y = m_Pos.y - m_Size.x;
                    m_AabbMax.y = m_Pos2.y + m_Size.x;
                } else {
                    m_AabbMin.y = m_Pos2.y - m_Size.x;
                    m_AabbMax.y = m_Pos.y + m_Size.x;
                }
                if (m_Pos.z <= m_Pos2.z) {
                    m_AabbMin.z = m_Pos.z - m_Size.x;
                    m_AabbMax.z = m_Pos2.z + m_Size.x;
                } else {
                    m_AabbMin.z = m_Pos2.z - m_Size.x;
                    m_AabbMax.z = m_Pos.z + m_Size.x;
                }
                break;
            case 4: {
                // A box of half-sizes m_Size turned by m_fRotY about Y: the
                // extent of its corners, centred on the unit.
                float s;
                float c;
                nw4r::math::SinCosRad(&s, &c, m_fRotY);
                float hx = fabsf(m_Size.x);
                float hz = fabsf(m_Size.z);
                float xc = hx * c;
                float zc = hz * c;
                float zs = hz * s;
                float xs = hx * s;
                float x0 = fabsf(-xc - zs);
                float x1 = fabsf(-xc + zs);
                float z0 = fabsf(-zc - xs);
                float z1 = fabsf(zc - xs);
                m_AabbMax.x = (x0 > x1) ? x0 : x1;
                m_AabbMax.y = fabsf(m_Size.y);
                m_AabbMax.z = (z0 > z1) ? z0 : z1;
                m_AabbMin.x = -m_AabbMax.x;
                m_AabbMin.y = -m_AabbMax.y;
                m_AabbMin.z = -m_AabbMax.z;
                bcs::FVEC3Add(&m_AabbMin, &m_AabbMin, &m_Pos);
                bcs::FVEC3Add(&m_AabbMax, &m_AabbMax, &m_Pos);
                break;
            }
            case 5:
                CalcCylinderAABB(*this);
                break;
            case 6:
                // A capsule from m_Pos to m_Pos2, grown to take in m_Pos3.
                COLL::GetMinMax(m_AabbMin, m_AabbMax, m_Pos, m_Pos2);
                m_AabbMin.x = (m_AabbMin.x > m_Pos3.x) ? m_Pos3.x : m_AabbMin.x;
                m_AabbMin.y = (m_AabbMin.y > m_Pos3.y) ? m_Pos3.y : m_AabbMin.y;
                m_AabbMin.z = (m_AabbMin.z > m_Pos3.z) ? m_Pos3.z : m_AabbMin.z;
                m_AabbMax.x = (m_AabbMax.x < m_Pos3.x) ? m_Pos3.x : m_AabbMax.x;
                m_AabbMax.y = (m_AabbMax.y < m_Pos3.y) ? m_Pos3.y : m_AabbMax.y;
                m_AabbMax.z = (m_AabbMax.z < m_Pos3.z) ? m_Pos3.z : m_AabbMax.z;
                break;
        }
    }

    long CCollUnit::GetCollOwnerChrIdx() const
    {
        if (GetOwnerKind() == 1 || GetUnk15() == 1) {
            return m_nChrNo;
        }
        return -1;
    }

    void CCollUnit::SetAtkType(unsigned long type)
    {
        m_nAtkType = type;
    }

    void CCollUnit::PlayHitSe(const bcs::FVEC3 &rPos, unsigned long arg) const
    {
        if (m_nHitSe >= 0) {
            CGameSnd::Play(m_nHitSe, rPos, arg, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    CCollBodyUnit::CCollBodyUnit() : CCollUnit(0, 2, 0)
    {
        m_nUnkA0 = 1;
        m_nPushPower = 0;
        m_nBodyFlag = 0;
        m_nUnkA4 = 0;
        m_pBodyDamageCallback = 0;
        m_pBodyDamageArg = 0;
        m_pBodyPushCallback = 0;
        m_pBodyPushArg = 0;
        m_pBallCatchCallback = 0;
        m_pBallCatchArg = 0;
        m_nTargetFlag = 0x7F;
        nw4r::ut::List_Append(&CCollMan::m_BodyUnitList, this);
    }

    CCollBodyUnit::~CCollBodyUnit()
    {
        nw4r::ut::List_Remove(&CCollMan::m_BodyUnitList, this);
    }

    void CCollBodyUnit::DoBodyPushCallback(CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        if (m_pBodyPushCallback != 0) {
            // Pushed again by the unit already pushing it: no push.
            if (m_pPushingUnit == pOther) {
                pPush = 0;
            }
            m_pPushingUnit = pOther;
            m_pBodyPushCallback(this, pOther, m_pBodyPushArg, pPush);
            m_pPushingUnit = 0;
        }
    }

    CCollAtkUnit::CCollAtkUnit() : CCollUnit(0, 0, 0)
    {
        m_fAtkRotY = 0.0f;
        m_pAttackHitCallback = 0;
        m_pAttackHitArg = 0;
        OnFlag10(4);
        nw4r::ut::List_Append(&CCollMan::m_AtkUnitList, this);
    }

    CCollAtkUnit::CCollAtkUnit(long kind, long type, unsigned long handle) : CCollUnit(kind, type, handle)
    {
        m_fAtkRotY = 0.0f;
        m_pAttackHitCallback = 0;
        m_pAttackHitArg = 0;
        OnFlag10(4);
        nw4r::ut::List_Append(&CCollMan::m_AtkUnitList, this);
    }

    CCollAtkUnit::~CCollAtkUnit()
    {
        nw4r::ut::List_Remove(&CCollMan::m_AtkUnitList, this);
    }

    void CCollAtkUnit::SetAttackHitCallback(AttackHitCallback pFunc, void *pArg)
    {
        m_pAttackHitCallback = pFunc;
        m_pAttackHitArg = pArg;
    }

    void CCollAtkUnit::DoAttackHitCallback(const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos, unsigned long flag)
    {
        if (m_pAttackHitCallback != 0) {
            m_pAttackHitCallback(this, pBody, rPos, flag, m_pAttackHitArg);
        }
    }

    CCollDefUnit::CCollDefUnit() : CCollUnit(0, 2, 0)
    {
        m_nFlagA0 = 0;
        m_fUnkA4 = 0.0f;
        m_fUnkA8 = 0.0f;
        m_pReflectHitCallback = 0;
        m_pReflectHitArg = 0;
        m_UnkB4[0] = 0;
        m_UnkB4[1] = 0;
        nw4r::ut::List_Append(&CCollMan::m_DefUnitList, this);
    }

    CCollDefUnit::~CCollDefUnit()
    {
        nw4r::ut::List_Remove(&CCollMan::m_DefUnitList, this);
    }

    void CCollDefUnit::SetReflectHitCallback(ReflectHitCallback pCallback, void *pArg)
    {
        m_pReflectHitCallback = pCallback;
        m_pReflectHitArg = pArg;
    }

    unsigned long CCollDefUnit::DoReflectHitCallback(const CBall *pBall, bcs::FVEC3 &rPos, bcs::FVEC3 &rVel) const
    {
        if (m_pReflectHitCallback != 0) {
            return m_pReflectHitCallback(this, pBall, rPos, rVel, m_pReflectHitArg);
        }
        return 1;
    }
}
