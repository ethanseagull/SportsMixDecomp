// msw::CStgGmk_Pylon_Act_Dodge: the actor of one pylon of dodge stage 04.
// It supplies the DODGE_PRM_S04_PYLON_* tuning to msw::CStgGmk_Pylon_Act (a
// DOL class): a ball shot in a special shot (IsArtsShootMode) breaks the
// pylon, any other ball is knocked away and pushes it. The file also holds
// the stage's pylon cluster, msw::CStgGmk_PylonCluster_Dodge, and the weak
// copies of the inline functions of the classes whose vtables it emits
// (CStgGmkClusterImpl, CStgGmk_PylonCluster, CStgGmk_Pylon,
// CStgGmk_Pylon_Dodge, CStgGmk_Pylon_Act).
#include "game/msw/CStgGmk_Pylon_Dodge.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CStgSrt.hpp"

namespace {
    // (stand-ins for operators) Reached through this one more inline level,
    // the float registers come out numbered as in the original (see
    // notes/toolchain.md on asm register numbering by inline depth).
    inline void VecSub(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3Sub(&rOut, &r1, &r2);
    }
    inline void VecNormalize(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::BCS_Vec_Normalize(rOut, rIn);
    }
}

long msw::CStgGmk_Pylon_Act_Dodge::GetParam_Effect(long type) const
{
    switch (type) {
        case 0:
            return 0x679;
        case 2:
            return 0xC;
        default:
            return -1;
    }
}

// A ball (pBall) hit the pylon. A special shot breaks a breakable pylon.
// Any other ball is knocked out of play, away from the pylon in the XZ
// plane at 0.3 times its speed, and pushes the pylon along its velocity.
void msw::CStgGmk_Pylon_Act_Dodge::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
{
    if (pBall->IsArtsShootMode() && GetMaxLife() > 0.0f) {
        FsmqChangeState(STATE_BREAK);
        return;
    }

    bcs::FVEC3 dir;
    bcs::FVEC3 vel = pBall->GetVelocity();

    VecSub(dir, pBall->GetPos(), GetSrt()->GetPos());
    dir.y = 0.0f;
    VecNormalize(dir, dir);
    bcs::FVEC3Scale(&dir, &dir, 0.3f * bcs::FVEC3Len(&vel));
    pBall->SetOutBallMode(dir, 0x80, -1);
    SetPushVel(vel);
}

// The pylon slides along rVel at GetParam_Acc and slows by GetParam_Dec.
void msw::CStgGmk_Pylon_Act_Dodge::SetPushVel(const bcs::FVEC3 &rVel)
{
    CStgSrt *pSrt = GetSrt();
    bcs::FVEC3 dir;
    VecNormalize(dir, rVel);
    bcs::FVEC3 vel;
    bcs::FVEC3Scale(&vel, &dir, GetParam_Acc());
    pSrt->SetVel(vel);
    pSrt->SetDecel(GetParam_Dec());
}

float msw::CStgGmk_Pylon_Act_Dodge::GetParam_Acc() const
{
    return DODGE_PRM_S04_PYLON_ACC;
}

float msw::CStgGmk_Pylon_Act_Dodge::GetParam_Dec() const
{
    return DODGE_PRM_S04_PYLON_DEC;
}

long msw::CStgGmk_PylonCluster_Dodge::GetLocValidNumIdx() const
{
    return 0;
}

// The index in the raw location table of position idx of pattern: each
// pattern's block starts at its table entry and holds three values per
// position.
long msw::CStgGmk_PylonCluster_Dodge::GetLocRawTblIdx(long pattern, long idx) const
{
    const long tbl[] = { 57, 90, 123, 156, 189, 222, 255, 288, 321, 354 };
    long posNum = (tbl[1] - tbl[0]) / 3;

    // The original compares the pattern with the size of the table in
    // bytes, not its number of entries.
    if (pattern < 0 || pattern >= (long)sizeof(tbl) || idx < 0 || idx >= posNum) {
        return -1;
    }
    return tbl[pattern] + idx * 3;
}
