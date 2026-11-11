// msw::CStgGmk_Case_Act_Basket: the actor of one case of basket stage 15
// (the ship). It supplies the BASKET_PRM_S15_* tuning to
// msw::CStgGmk_Case_Act (a DOL class). Its inline getters and those of
// msw::CStgGmk_Case_Act are emitted here with the vtable.
#include "game/msw/CStgGmk_Case_Basket.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CStgSrt.hpp"

namespace {
    // (stand-ins for operators) Reached through this one
    // more inline level, the float registers come out numbered as in the
    // original (see notes/toolchain.md on asm register numbering by inline
    // depth).
    inline void VecSub(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3Sub(&rOut, &r1, &r2);
    }
    inline void VecNormalize(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::BCS_Vec_Normalize(rOut, rIn);
    }
}

long msw::CStgGmk_Case_Act_Basket::GetParam_Effect(long type) const
{
    switch (type) {
        case 0:
            return 0x6BD;
        case 1:
            return 0x6BE;
        case 2:
            return 0xC;
        default:
            return -1;
    }
}

long msw::CStgGmk_Case_Act_Basket::GetParam_SE(long type) const
{
    switch (type) {
        case 0:
            return 0xD7E;
        case 1:
            return 0xD7C;
        case 3:
            return 0xD7D;
        default:
            return CStgGmk_Case_Act::GetParam_SE(type);
    }
}

// A ball that hits the case is knocked out of play, away from the case in
// the XZ plane. The ball's speed is measured but not used.
void msw::CStgGmk_Case_Act_Basket::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
{
    bcs::FVEC3 dir;
    bcs::FVEC3 vel = pBall->GetVelocity();

    VecSub(dir, pBall->GetPos(), GetSrt()->GetPos());
    dir.y = 0.0f;
    VecNormalize(dir, dir);
    bcs::FVEC3Len(&vel);
    pBall->SetOutBallMode(dir, 0x80, -1);
}

msw::CStgGmk_Case_Act_Basket::~CStgGmk_Case_Act_Basket()
{
}
