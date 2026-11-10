// msw::CCollManBoss: the collision manager of the boss game.
#include "game/msw/CCollManBoss.hpp"

#include "bcs/New.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CStageMan.hpp"

msw::CCollMan *msw::CCollManBoss::CreateInstance(bcs::CTask *pParent)
{
    if (CCollMan::m_pInstance == 0) {
        CCollMan::m_pInstance = new (0x100) CCollManBoss(pParent);
    }
    return CCollMan::m_pInstance;
}

msw::CCollManBoss::~CCollManBoss()
{
}

void msw::CCollManBoss::Main()
{
    CCollMan::Main();
}

void msw::CCollManBoss::Disp()
{
    CCollMan::Disp();
}

// The court of the boss game is a circle around the origin: a position
// further out than the court's radius less `fRadius` is moved back onto
// that circle (y is left alone) and rNormal points back to the centre.
// Returns 2 for a hit. (what the 2 stands for is not known; the DOL's
// DetectMapHitOuterCircle_ returns 0x1E from the same place)
int msw::CCollManBoss::DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &, const bcs::FVEC3 &,
                                     const bcs::FVEC3 &rTo, float fRadius, long, unsigned long)
{
    bcs::FVEC3 center;
    bcs::FVEC3 diff;
    bcs::FVEC3 dir;
    float fLimit;
    float fDistSq;

    // Most likely `rPos = rTo` in the original, which copies float by
    // float; with bcs::FVEC3 as the plain struct it is now, the assignment
    // would copy words.
    rPos.x = rTo.x;
    rPos.y = rTo.y;
    rPos.z = rTo.z;
    rNormal.Set(0.0f, 0.0f, 0.0f);
    // The centre of the court is a vector of its own in the original too:
    // the subtractions and additions of its zeros are in the code.
    center.Set(0.0f, 0.0f, 0.0f);
    diff.x = rTo.x - center.x;
    diff.z = rTo.z - center.z;

    // How far from the centre the sphere's own centre may be; not negative.
    fLimit = CStageMan::GetInstance()->GetCourtRadius() - fRadius;
    fLimit = 0.0f < fLimit ? fLimit : 0.0f;
    fDistSq = diff.x * diff.x + diff.z * diff.z;
    if (fDistSq <= fLimit * fLimit) {
        return 0;
    }

    dir.Set(diff.x, 0.0f, diff.z);
    bcs::BCS_Vec_Normalize(rNormal, dir);
    diff.x = rNormal.x * fLimit;
    diff.z = rNormal.z * fLimit;
    rPos.x = center.x + diff.x;
    rPos.z = center.z + diff.z;
    rNormal.x *= -1.0f;
    rNormal.z *= -1.0f;
    return 2;
}
