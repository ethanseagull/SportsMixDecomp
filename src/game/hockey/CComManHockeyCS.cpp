// msw::CComManHockeyCS: where the computer players go on the hockey crush
// skate party stages. The court is round about the origin; the targets
// are worked out as an angle about the centre and a distance from it.
#include "game/msw/CComManHockeyCS.hpp"

#include "bcs/BCS_Vec.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"

// (name guessed) The character state in which another character is no
// target.
static const long STATE_NO_TARGET = 0x4E;

namespace {

    // Angle a brought into [0, 2 pi].
    inline float WrapRad2Pi(float a)
    {
        while (a > 6.2831855f) {
            a -= 6.2831855f;
        }
        while (a < 0.0f) {
            a += 6.2831855f;
        }
        return a;
    }

    // Angle a brought into [-pi, pi].
    inline float WrapRadPi(float a)
    {
        while (a > 3.1415927f) {
            a -= 6.2831855f;
        }
        while (a < -3.1415927f) {
            a += 6.2831855f;
        }
        return a;
    }
}

bcs::FVEC3 msw::CComManHockeyCS::GetTargetPos_StageCommon(bcs::FVEC3 pos, float fAdd)
{
    bcs::FVEC3 result(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 center(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 vec = pos;
    bcs::FVEC3 dir;

    bcs::FVEC3 posXZ(vec.x, 0.0f, vec.z);
    bcs::FVEC3 origin(0.0f, 0.0f, 0.0f);
    float dist = bcs::FVEC3Dist(&origin, &posXZ);
    dist += fAdd;
    if (dist < 0.0f) {
        dist = 0.0f;
    }

    vec.x -= center.x;
    vec.z -= center.z;
    float angle = WrapRad2Pi(nw4r::math::Atan2Rad(vec.x, vec.z));

    bcs::BCS_VecZ_RotY(dir, 1.0f, angle);
    result.x = dir.x * dist;
    result.z = dir.z * dist;
    return result;
}

bcs::FVEC3 msw::CComManHockeyCS::GetTargetPos_Stage86(bcs::FVEC3 dir, bcs::FVEC3 pos, float fMin, float fAdd,
                                                     float fMin2)
{
    bcs::FVEC3 result(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 unit;
    bcs::FVEC3 vec = pos;

    float posAngle = WrapRad2Pi(nw4r::math::Atan2Rad(vec.x, vec.z));
    float dirAngle = WrapRad2Pi(nw4r::math::Atan2Rad(dir.x, dir.z));
    float diff = WrapRadPi(dirAngle - posAngle);

    bcs::FVEC3 center(0.0f, 0.0f, 0.0f); // (not used; the court centre as in StageCommon)
    bcs::FVEC3 origin(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 posXZ(vec.x, 0.0f, vec.z);
    float dist = bcs::FVEC3Dist(&origin, &posXZ);
    if (fAdd > 0.0f) {
        dist += fAdd;
        if (dist > 8.0f) {
            dist = 8.0f;
        }
    }
    if (dist <= fMin + fMin2) {
        dist = fMin + fMin2;
    }

    // More than 15 degrees apart: go 30 degrees short of the direction.
    if (diff >= 0.2617994f) {
        bcs::BCS_VecZ_RotY(unit, 1.0f, dirAngle - 0.5235988f);
        result.x = unit.x * dist;
        result.z = unit.z * dist;
        return result;
    }
    if (diff <= -0.2617994f) {
        bcs::BCS_VecZ_RotY(unit, 1.0f, dirAngle + 0.5235988f);
        result.x = unit.x * dist;
        result.z = unit.z * dist;
        return result;
    }
    bcs::BCS_VecZ_RotY(unit, 1.0f, posAngle);
    result.x = unit.x * dist;
    result.z = unit.z * dist;
    return result;
}

bcs::FVEC3 msw::CComManHockeyCS::GetTargetPos_Stage87(bcs::FVEC3 dir, bcs::FVEC3 pos, float fMin, float fMax)
{
    bcs::FVEC3 result(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 unit;
    bcs::FVEC3 vec = pos;

    float posAngle = WrapRad2Pi(nw4r::math::Atan2Rad(vec.x, vec.z));
    float dirAngle = WrapRad2Pi(nw4r::math::Atan2Rad(dir.x, dir.z));
    float diff = WrapRadPi(dirAngle - posAngle);

    bcs::FVEC3 center(0.0f, 0.0f, 0.0f); // (not used; the court centre as in StageCommon)
    bcs::FVEC3 origin(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 posXZ(vec.x, 0.0f, vec.z);
    float dist = bcs::FVEC3Dist(&origin, &posXZ);
    if (dist < fMin) {
        dist = fMin;
    }
    if (dist > fMax) {
        dist = fMax;
    }

    if (diff >= 0.2617994f) {
        bcs::BCS_VecZ_RotY(unit, 1.0f, dirAngle - 0.5235988f);
        result.x = unit.x * dist;
        result.z = unit.z * dist;
        return result;
    }
    if (diff <= -0.2617994f) {
        bcs::BCS_VecZ_RotY(unit, 1.0f, dirAngle + 0.5235988f);
        result.x = unit.x * dist;
        result.z = unit.z * dist;
        return result;
    }
    bcs::BCS_VecZ_RotY(unit, 1.0f, posAngle);
    result.x = unit.x * dist;
    result.z = unit.z * dist;
    return result;
}

long msw::CComManHockeyCS::IsExistTargetInArea(const CChrCore *pChr, float f1, float f2, float f3)
{
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        const CChrCore *pTarget = CChrMan::GetChrCore(i);
        if (pTarget == 0 || pTarget->IsDisable() || pTarget == pChr) {
            continue;
        }
        if (pTarget->GetState() == STATE_NO_TARGET) {
            continue;
        }
        if (IsExistTargetCore(pChr, pTarget, f1, f2, f3)) {
            return pTarget->GetPlayerNo();
        }
    }
    return -1;
}
