// msw::CStgGmk_Pylon_Act_Hockey: the actor of one pylon of hockey stage 04
// (written from DodgeF's game/dodge/CStgGmk_Pylon_Act_Dodge.cpp). It
// supplies the HOCKEY_PRM_S04_PYLON_* tuning to msw::CStgGmk_Pylon_Act (a
// DOL class). The puck bounces off a pylon; a special shot
// (IsArtsShootMode) breaks a breakable one, a fast shot costs it a life.
// Characters' attacks of type 6 push it. The file also holds the stage's
// pylon cluster, msw::CStgGmk_PylonCluster_Hockey, and the weak copies of
// the inline functions of the classes whose vtables it emits
// (CStgGmk_PylonCluster, CStgGmk_Pylon, CStgGmk_Pylon_Hockey,
// CStgGmk_Pylon_Act).
#include "game/msw/CStgGmk_Pylon_Hockey.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameSnd.hpp"
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

long msw::CStgGmk_Pylon_Act_Hockey::GetParam_Effect(long type) const
{
    switch (type) {
        case 0:
            return 0x701;
        case 2:
            return 0xC;
        default:
            return -1;
    }
}

void msw::CStgGmk_Pylon_Act_Hockey::OnUpdate(float rate)
{
    CStgGmk_Pylon_Act::OnUpdate(rate);
    m_fReflectWait -= rate;
    float wait = 0.0f;
    if (m_fReflectWait > wait) {
        wait = m_fReflectWait;
    }
    m_fReflectWait = wait;
    m_fHitSeWait -= rate;
    wait = 0.0f;
    if (m_fHitSeWait > wait) {
        wait = m_fHitSeWait;
    }
    m_fHitSeWait = wait;
}

// Catching the puck does nothing to a hockey pylon.
void msw::CStgGmk_Pylon_Act_Hockey::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
{
}

// The ball (pBall) hit the pylon. A special shot breaks a breakable pylon.
// Any other ball is knocked out of play, away from the pylon in the XZ
// plane, and pushes the pylon along its velocity; a breakable pylon out of
// lives then breaks.
void msw::CStgGmk_Pylon_Act_Hockey::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
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
    // (the speed is computed and not used)
    float speed = bcs::FVEC3Len(&vel);
    pBall->SetOutBallMode(dir, 0x80, -1);
    SetPushVel(vel);
    if (GetMaxLife() > 0.0f && GetLife() <= 0.0f) {
        FsmqChangeState(STATE_BREAK);
    }
}

// An attack of type 6 by a character pushes the pylon away from the
// attack, with a sound at most every 20 frames. A ball's attack (an item
// ball in play) damages it.
unsigned long msw::CStgGmk_Pylon_Act_Hockey::OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                          const bcs::FVEC3 &rPos)
{
    if (pAtk->GetOwnerKind() == 1 && pAtk->GetAtkType() == 6) {
        bcs::FVEC3 dir;
        VecSub(dir, pBody->GetPos(), pAtk->GetPos());
        VecNormalize(dir, dir);
        SetPushVel(dir);
        if (m_fHitSeWait <= 0.0f) {
            CGameSnd::Play(0xD1D, rPos, 1, -1, 0.0f, 1.0f, 0.0f);
            m_fHitSeWait = 20.0f;
        }
    } else if (pAtk->GetOwnerKind() == 2) {
        CBall *pBall = pAtk->m_nOwnerNo >= 0 ? CBallMan::GetBallPtr(pAtk->m_nOwnerNo) : 0;
        if (!IsInvincible() && pBall != 0 && pBall->IsActive() && pBall->IsItemBall()) {
            DoDamage(0, pBall);
        }
        return 0;
    }
    return 2;
}

// The puck bounces off the pylon: rVel becomes the direction from the
// pylon to the puck, along X or Z (whichever side was hit). A special shot
// breaks a breakable pylon instead; a shot faster than 10 costs it a life
// (with the hit effect and sound). Further hits are ignored for 8 frames,
// and the pylon is pushed along the puck's velocity.
unsigned long msw::CStgGmk_Pylon_Act_Hockey::OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall,
                                                          bcs::FVEC3 &rPos, bcs::FVEC3 &rVel)
{
    if (m_fReflectWait > 0.0f) {
        return 0;
    }
    if (pBall->IsArtsShootMode() && GetMaxLife() > 0.0f) {
        FsmqChangeState(STATE_BREAK);
        return 0;
    }

    // (the speed is computed and not used)
    float inSpeed = bcs::FVEC3Len(&rVel);
    bcs::FVEC3 dir;
    VecSub(dir, pBall->GetPos(), GetSrt()->GetPos());
    dir.y = 0.0f;
    VecNormalize(dir, dir);

    // Within 45 degrees of the Z axis the puck hit a front or back side.
    bcs::FVEC3 axisZ(0.0f, 0.0f, 1.0f);
    float dot = bcs::FVEC3Dot(&dir, &axisZ);
    float angle = bcs::BCS_ACosT(dot > 1.0f ? 1.0f : (dot < -1.0f ? -1.0f : dot));
    if (angle < 0.7853982f || angle > 2.3561945f) {
        dir.x = 0.0f;
    } else {
        dir.z = 0.0f;
    }
    VecNormalize(rVel, dir);

    bcs::FVEC3 vel = pBall->GetVelocity();
    float speed = bcs::FVEC3Len(&vel);
    bool bShot = false;
    if (pBall->IsShootMode() && speed > 10.0f) {
        bShot = true;
    }
    if (FsmqGetState() != STATE_BREAK && bShot && !IsInvincible()) {
        bcs::FVEC3 pos = pBall->GetPos();
        long eff = GetParam_Effect(2);
        if (eff >= 0) {
            CEffectCtrl::SimpleEffect(eff, 0, &pos, 0, 3, 0, 0, 0);
        }
        long se = GetParam_SE(2);
        if (se >= 0) {
            CGameSnd::Play(se, pos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
        DecLife();
        if (GetMaxLife() > 0.0f && GetLife() <= 0.0f) {
            FsmqChangeState(STATE_BREAK);
        }
    }

    m_fReflectWait = 0.13333334f; // eight frames
    bcs::FVEC3 ballVel = pBall->GetVelocity();
    SetPushVel(ballVel);
    // Bit 0: reflected; bits 8 to 15: 9 (meaning not known; the ice cube
    // puts 0x0D there).
    // (The original builds 0x901 as 1 | 0x900 in two instructions; the
    // source form of that was not found.)
    return 0x901;
}

// The pylon slides along rVel at GetParam_Acc and slows by GetParam_Dec.
void msw::CStgGmk_Pylon_Act_Hockey::SetPushVel(const bcs::FVEC3 &rVel)
{
    CStgSrtMove *pSrt = GetSrt();
    bcs::FVEC3 dir;
    VecNormalize(dir, rVel);
    bcs::FVEC3 vel;
    bcs::FVEC3Scale(&vel, &dir, GetParam_Acc());
    pSrt->SetVel(vel);
    pSrt->SetDecel(GetParam_Dec());
}

float msw::CStgGmk_Pylon_Act_Hockey::GetParam_Acc() const
{
    return HOCKEY_PRM_S04_PYLON_ACC;
}

float msw::CStgGmk_Pylon_Act_Hockey::GetParam_Dec() const
{
    return HOCKEY_PRM_S04_PYLON_DEC;
}

long msw::CStgGmk_PylonCluster_Hockey::GetLocValidNumIdx() const
{
    return 0;
}

// The index in the raw location table of position idx of pattern: each
// pattern's block starts at its table entry and holds three values per
// position.
long msw::CStgGmk_PylonCluster_Hockey::GetLocRawTblIdx(long pattern, long idx) const
{
    const long tbl[] = { 57, 90, 123, 156, 189, 222, 255, 288, 321, 354 };
    static const long patnum = 40;
    static const long colnum = 3;
    long posNum = (tbl[1] - tbl[0]) / colnum;

    // The original checks the pattern against patnum (40), not 10, so
    // patterns 10 to 39 read past the table. (The "@0" on patnum's symbol
    // says another patnum was declared before it in the original file.)
    if (pattern < 0 || pattern >= patnum || idx < 0 || idx >= posNum) {
        return -1;
    }
    return tbl[pattern] + idx * colnum;
}
