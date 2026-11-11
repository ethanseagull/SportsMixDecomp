// msw::CStgGmk_Case_Act_Hockey: the actor of one case of hockey stage 15
// (the ship). It supplies the HOCKEY_PRM_S15_* tuning to
// msw::CStgGmk_Case_Act (a DOL class) and reflects the puck. Its inline
// getters and those of msw::CStgGmk_Case_Act are emitted here with the
// vtable.
#include "game/msw/CStgGmk_Case_Hockey.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CCollAtkUnit.hpp"
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
    inline float VecDot(const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        return bcs::FVEC3Dot(&r1, &r2);
    }
}

long msw::CStgGmk_Case_Act_Hockey::GetParam_Effect(long type) const
{
    switch (type) {
        case 0:
            return 0x70B;
        case 1:
            return 0x70C;
        case 2:
            return 0xC;
        default:
            return -1;
    }
}

long msw::CStgGmk_Case_Act_Hockey::GetParam_SE(long type) const
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

// Counts the reflect wait down to zero.
void msw::CStgGmk_Case_Act_Hockey::OnUpdate(float rate)
{
    CStgGmk_Case_Act::OnUpdate(rate);
    m_fReflectWait -= rate;
    float wait = 0.0f;
    if (m_fReflectWait > wait) {
        wait = m_fReflectWait;
    }
    m_fReflectWait = wait;
}

// A ball that hits the case is knocked out of play, away from the case in
// the XZ plane. The ball's speed is measured but not used.
void msw::CStgGmk_Case_Act_Hockey::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
{
    bcs::FVEC3 dir;
    bcs::FVEC3 vel = pBall->GetVelocity();

    VecSub(dir, pBall->GetPos(), GetSrt()->GetPos());
    dir.y = 0.0f;
    VecNormalize(dir, dir);
    bcs::FVEC3Len(&vel);
    pBall->SetOutBallMode(dir, 0x80, -1);
}

// Only attacks of owner kind 4 (meaning not known) damage the case.
unsigned long msw::CStgGmk_Case_Act_Hockey::OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                         const bcs::FVEC3 &rPos)
{
    if (pAtk != 0 && pAtk->GetOwnerKind() == 4) {
        return CStgGmk_Case_Act::OnBodyDamage(pBody, pAtk, rPos);
    }
    return 0;
}

// The puck leaves along the X or the Z axis, away from the case: along Z
// when it struck within 45 degrees of the front or back, along X
// otherwise. The new velocity has length 1 (the original measures the
// incoming speed and does not use it). Then the case ignores the puck for
// ten frames, and a live case not already breaking takes the hit.
unsigned long msw::CStgGmk_Case_Act_Hockey::OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall,
                                                        bcs::FVEC3 &rPos, bcs::FVEC3 &rVel)
{
    if (m_fReflectWait > 0.0f) {
        return 0;
    }

    float speed = bcs::FVEC3Len(&rVel);
    bcs::FVEC3 dir;
    VecSub(dir, pBall->GetPos(), GetSrt()->GetPos());
    dir.y = 0.0f;
    VecNormalize(dir, dir);

    bcs::FVEC3 front(0.0f, 0.0f, 1.0f);
    float cos = VecDot(dir, front);
    cos = cos > 1.0f ? 1.0f : (cos < -1.0f ? -1.0f : cos);
    float angle = bcs::BCS_ACosT(cos);
    if (angle < 0.7853982f || angle > 2.3561945f) {
        dir.x = 0.0f;
    } else {
        dir.z = 0.0f;
    }
    bcs::BCS_Vec_Normalize(rVel, dir);

    m_fReflectWait = 10.0f;
    if (IsAlive() && GetState() != 3) {
        DoDamage(pBall->GetPos(), 1, 1);
    }
    return 1 | 0x800;
}

msw::CStgGmk_Case_Act_Hockey::~CStgGmk_Case_Act_Hockey()
{
}
