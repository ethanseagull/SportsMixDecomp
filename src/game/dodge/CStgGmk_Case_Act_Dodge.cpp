// msw::CStgGmk_Case_Act_Dodge: the actor of one case of dodge stage 15
// (the ship). It supplies the DODGE_PRM_S15_* tuning to
// msw::CStgGmk_Case_Act (a DOL class). Its inline getters and those of
// msw::CStgGmk_Case_Act are emitted here with the vtable.
#include "game/msw/CStgGmk_Case_Dodge.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CStageMan.hpp"
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

// The area the cases move in: the court's depth and the ground's width,
// widened on both sides by the case width plus a margin.
void msw::CStgGmk_Case_Act_Dodge::GetParam_OuterRect(float *pLeft, float *pRight, float *pFront, float *pBack) const
{
    CStageMan::GetInstance()->GetCourtRect(0, 0, pFront, pBack);
    CStageMan::GetInstance()->GetGroundRect(pLeft, pRight, 0, 0);
    float width = GetParam_Width();
    if (pLeft) {
        *pLeft -= 0.8f + width;
    }
    if (pRight) {
        *pRight += 0.8f + width;
    }
}

float msw::CStgGmk_Case_Act_Dodge::GetParam_Width() const
{
    return DODGE_PRM_S15_CASE_WIDTH;
}

long msw::CStgGmk_Case_Act_Dodge::GetParam_Effect(long type) const
{
    switch (type) {
        case 0:
            return 0x67D;
        case 1:
            return 0x67E;
        case 2:
            return 0xC;
        default:
            return -1;
    }
}

long msw::CStgGmk_Case_Act_Dodge::GetParam_SE(long type) const
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
// the XZ plane, at 0.3 times its speed. (Same body as
// msw::CStgGmk_Trolley_Act_Dodge::OnDamage.)
void msw::CStgGmk_Case_Act_Dodge::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
{
    bcs::FVEC3 dir;
    bcs::FVEC3 vel = pBall->GetVelocity();

    VecSub(dir, pBall->GetPos(), GetSrt()->GetPos());
    dir.y = 0.0f;
    VecNormalize(dir, dir);
    bcs::FVEC3Scale(&dir, &dir, 0.3f * bcs::FVEC3Len(&vel));
    pBall->SetOutBallMode(dir, 0x80, -1);
}

msw::CStgGmk_Case_Act_Dodge::~CStgGmk_Case_Act_Dodge()
{
}
