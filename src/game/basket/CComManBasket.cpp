// msw::CComManBasket: the basketball computer players' shared helpers.
#include "game/msw/CComManBasket.hpp"

#include <math.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CQPanel.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/math/triangular.h"

// 70 degrees in radians, unused (see CComManBasketHB.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

namespace {
    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // (names guessed) An angle brought into [0, 2 pi), and into (-pi, pi].
    inline float RoundRad(float fRad)
    {
        while (fRad > PI2) {
            fRad -= PI2;
        }
        while (fRad < 0.0f) {
            fRad += PI2;
        }
        return fRad;
    }

    inline float RoundRadPI(float fRad)
    {
        fRad = RoundRad(fRad);
        if (fRad > PI) {
            fRad -= PI2;
        }
        return fRad;
    }

    // (guessed helper) The yaw from rPos1 towards rPos2 in the XZ plane.
    inline float RotY_XZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 dir = rPos1;
        dir.x = rPos2.x - dir.x;
        dir.z = rPos2.z - dir.z;
        return RoundRad(nw4r::math::Atan2Rad(dir.x, dir.z));
    }

    // (guessed helper) The length of rVec in the XZ plane.
    inline float LenXZ(const bcs::FVEC3 &rVec)
    {
        bcs::FVEC3 xz(rVec.x, 0.0f, rVec.z);
        return PSVECMag((const Vec *)&xz);
    }

    // (guessed helper) rPos1 - rPos2.
    inline bcs::FVEC3 Diff(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 diff;
        return *bcs::FVEC3Sub(&diff, &rPos1, &rPos2);
    }

    // (guessed helper) The length of a vector.
    inline float Len(bcs::FVEC3 v)
    {
        return PSVECMag((const Vec *)&v);
    }
}

msw::CChrCore *msw::CComManBasket::GetNearChrCore(const bcs::FVEC3 &rPos, long n1, long n2, long n3, int b)
{
    unsigned long nChr = GetNearChr(rPos, n1, n2, n3, b);

    if (nChr <= 5) {
        return CChrMan::GetChrCore(nChr);
    }
    return 0;
}

float msw::CComManBasket::GetRotY_P1ToP2XZ(const CChrCore *p1, const CChrCore *p2)
{
    return RotY_XZ(GetChrPos(p1), GetChrPos(p2));
}

float msw::CComManBasket::GetRotY_P1ToP2XZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
{
    return RotY_XZ(rPos1, rPos2);
}

int msw::CComManBasket::IsComDrv(const CChrCore *pChr)
{
    return pChr->DriverGetType() == 3;
}

long msw::CComManBasket::GetTeamCoin(long nTeam)
{
    return CGame::GetTeamCoin(nTeam);
}

float msw::CComManBasket::GetDistXZ(const bcs::FVEC3 &rVec)
{
    return LenXZ(rVec);
}

float msw::CComManBasket::GetDistXZ(const CChrCore *p1, const CChrCore *p2)
{
    return GetDistXZ(p1->GetPos(), p2->GetPos());
}

float msw::CComManBasket::GetDistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
{
    bcs::FVEC3 d = Diff(rPos1, rPos2);
    return GetDistXZ(d);
}

float msw::CComManBasket::GetDist(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
{
    bcs::FVEC3 d = Diff(rPos1, rPos2);
    return PSVECMag((const Vec *)&d);
}

float msw::CComManBasket::GetNearQPanelDistXZ(const CChrCore *pChr)
{
    float fDist = QPanel_GetNearDisXZ(GetChrPos(pChr), -1, 1);

    if (fDist < 0.0f) {
        fDist = 9999.0f;
    }
    return fDist;
}

float msw::CComManBasket::GetDistOnGoalTo3PointAreaCenter(long nGoal)
{
    bcs::FVEC3 center = CStageMan::GetInstance()->Get3PointAreaPos(nGoal);
    const bcs::FVEC3 &rGoal = CGame::GetGoalPos(nGoal);
    float fDist = center.z - rGoal.z;

    if (rGoal.z > 0.0f) {
        fDist = -fDist;
    }
    return fDist;
}

bool msw::CComManBasket::Is3PointArea(long nGoal, const bcs::FVEC3 &rPos)
{
    float fSize = CGameBasket::GetCourt3PointSize();
    return CStageMan::GetInstance()->Is3PointArea(nGoal, Local2World(rPos, -1), fSize);
}

bcs::FVEC3 msw::CComManBasket::Get3PointAreaRundPos(long nGoal)
{
    bcs::FVEC3 pos = CStageMan::GetInstance()->Get3PointAreaPos(nGoal);
    pos.y = 0.0f;

    float fDeg = bcs::BCS_RandRange(0L, 180L);
    if (bcs::BCS_RandMax(1L)) {
        fDeg = -fDeg;
    }

    // vec is the direction from the area's centre, then the normal of
    // the wall hit.
    bcs::FVEC3 vec;
    float fRot = bcs::DEG_TO_RAD(fDeg);
    bcs::BCS_VecZ_RotY(vec, pos.z > 0.0f ? -1.0f : 1.0f, fRot);
    float fLen = bcs::BCS_RandRange(1.5f, CGameBasket::GetCourt3PointSize());
    pos.x += vec.x * fLen;
    pos.z += vec.z * fLen;

    bcs::FVEC3 hit;
    float fTime;
    vec.Set(0.0f, 0.0f, 0.0f);
    if (CCollMan::DetectMapHit(hit, vec, fTime, pos, pos, 1.0f, 1, 0x1000000)) {
        pos = hit;
    }
    return World2Local(pos, -1);
}

void msw::CComManBasket::NormalizeCanMovePosToBossPackun(bcs::FVEC3 &rPos, const CChrCore *pChr)
{
    if (!CChrManBasket::GetInstance()->IsCreateBossPackun_()) {
        return;
    }

    rPos = Local2World(rPos, -1);

    float fLeft;
    float fRight;
    float fFront;
    float fBack;
    CStageMan::GetInstance()->GetCourtRect(&fLeft, &fRight, &fFront, &fBack);

    float aEdge[2] = {fFront, fBack};
    float aOffset[2] = {0.75f, -0.75f};

    for (int i = 0; i < 2; i++) {
        // Half the size of the boss Packun's area, around the centre of
        // each half of the court. static-order swap: the bytes are right
        // but the original names these @LOCAL@...@fX@0 and @fZ, ours
        // @fX and @fZ@0.
        static const float fX = 2.8f;
        static const float fZ = 0.75f;

        // The area's corners (the original stores all four; only some are
        // read).
        float fCenter = aEdge[i] + aOffset[i];
        bcs::FVEC3 aRect[4] = {
            bcs::FVEC3(-fX, 0.0f, fCenter + -fZ),
            bcs::FVEC3(-fX, 0.0f, fCenter + fZ),
            bcs::FVEC3(fX, 0.0f, fCenter + fZ),
            bcs::FVEC3(fX, 0.0f, fCenter + -fZ),
        };

        if (aRect[0].x <= rPos.x && aRect[2].x >= rPos.x && aRect[0].z <= rPos.z && aRect[1].z >= rPos.z) {
            bcs::FVEC3 center(0.0f, 0.0f, aEdge[i]);
            bcs::BCS_VecZ_RotY(rPos, 1.0f, CChrManBasket::GetRotY_Pos1ToPos2XZ(center, pChr->GetPos()));

            bcs::FVEC3 scale(fX * rPos.x, 0.0f, 1.5f * rPos.z);
            float fLen = Len(scale);
            rPos.x *= fLen;
            rPos.y = 0.0f;
            rPos.z = rPos.z * fLen + center.z;
            break;
        }
    }

    rPos = World2Local(rPos, -1);
}

int msw::CComManBasket::IsExistTarget(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float f1, float f2,
                                      unsigned long nChrBits)
{
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && IsEnableBit(nChrBits, i)) {
            if (IsPointInFaceXZ(GetChrPos(pChr), rFrom, rTo, f1, f2)) {
                return 1;
            }
        }
    }
    return 0;
}

int msw::CComManBasket::IsExistTarget(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float f1, float f2,
                                      const CChrCore *pChr)
{
    unsigned long nChrBits = 0;
    SetEnableBit(nChrBits, pChr->GetPlayerNo());
    return IsExistTarget(rFrom, rTo, f1, f2, nChrBits);
}

void msw::CComManBasket::NormalVecXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
{
    bcs::FVEC3 xz(rIn.x, 0.0f, rIn.z);
    bcs::BCS_Vec_Normalize(rOut, xz);
}

int msw::CComManBasket::IsAllComTeam(long nTeam)
{
    return CChrManBasket::GetInstance()->IsAllComTeam_(nTeam);
}

int msw::CComManBasket::IsQPanelDisable(unsigned long nId)
{
    CQPanel *pPanel = static_cast<CQPanel *>(bcs::CTaskMan::GetTask(nId));

    if (pPanel != 0) {
        if (static_cast<bcs::CTask *>(pPanel)->IsDelete()) {
            return 1;
        }
        return !pPanel->CanAccess();
    }
    return 1;
}

long msw::CComManBasket::GetRandValueI(long n1, long n2)
{
    if (n1 > n2) {
        long n = n1;
        n1 = n2;
        n2 = n;
    }
    return bcs::BCS_RandRange(n1, n2);
}

float msw::CComManBasket::GetRandValueF(float f1, float f2)
{
    if (f1 > f2) {
        float f = f1;
        f1 = f2;
        f2 = f;
    }
    return bcs::BCS_RandRange(f1, f2);
}

int msw::CComManBasket::IsExistChrToTarget(const CChrCore *pChr, float fAngle, const bcs::FVEC3 &rTarget, float f1,
                                           float f2, unsigned long nChrBits)
{
    return IsExistChrToTargetWorld(pChr, fAngle, Local2World(rTarget, -1), f1, f2, nChrBits);
}

int msw::CComManBasket::IsExistChrToTargetWorld(const CChrCore *pChr, float fAngle, const bcs::FVEC3 &rTarget,
                                                float f1, float f2, unsigned long nChrBits)
{
    bcs::FVEC3 pos = GetChrWorldPos(pChr);
    long nSelf = pChr->GetPlayerNo();
    float fRotY = pChr->GetRotY_ToPosXZ(rTarget);

    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        if (i == nSelf) {
            continue;
        }
        CChrCore *pOther = CChrMan::GetChrCore(i);
        if (pOther != 0 && !pOther->IsDisable() && IsEnableBit(nChrBits, i)) {
            bcs::FVEC3 otherPos = GetChrWorldPos(pOther);
            if (IsPointInFaceXZ(otherPos, pos, rTarget, f1, f2)) {
                float fDiff = RoundRadPI(pChr->GetRotY_ToPosXZ(otherPos) - fRotY);
                if (fabsf(fDiff) > fAngle) {
                    continue;
                }
                return 1;
            }
        }
    }
    return 0;
}
