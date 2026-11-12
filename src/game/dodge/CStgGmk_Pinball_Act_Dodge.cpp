// msw::CStgGmk_Pinball_Act_Dodge: the actor of one ball of dodge stage 12
// (pinball). It supplies the DODGE_PRM_S12_BALL_* tuning to
// msw::CStgGmk_Pinball_Act (a DOL class) and bounces off what it hits. The
// file also holds the stage's pinball manager, msw::CPinballManDodge, and
// the weak copies of the inline functions of the classes whose vtables it
// emits (CPinballMan, CStgGmk_Pinball, CStgGmk_Pinball_Dodge,
// CStgGmk_Pinball_Act). CPinballMan's are defined out of line at the end
// instead, in the same order (as in VolleyF), so that the stage units that
// include its header do not change.
#include "game/msw/CStgGmk_Pinball_Dodge.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CGameDodge.hpp"
#include "game/msw/CPinballManDodge.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgSrt.hpp"
#include "game/msw/DodgePrm.hpp"

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

    // (name guessed) Mirrors rVel about the plane with normal rNormal (a
    // unit vector), unless rVel already has a part along rNormal.
    inline void Reflect(bcs::FVEC3 &rVel, const bcs::FVEC3 &rNormal)
    {
        float dot = 2.0f * bcs::FVEC3Dot(&rVel, &rNormal);
        if (dot > 0.0f) {
            return;
        }
        bcs::FVEC3 ofs;
        bcs::FVEC3Scale(&ofs, &rNormal, dot);
        bcs::FVEC3Sub(&rVel, &rVel, &ofs);
    }
}

msw::CStgGmk_Pinball_Act_Dodge::~CStgGmk_Pinball_Act_Dodge()
{
}

// A dodgeball (pBall) hit this pinball's body pBody. The pinball bounces
// off it: unless it is already moving away from the dodgeball (in the XZ
// plane), its velocity is mirrored about that direction; its speed grows by
// GetParam_BallKickPower. The dodgeball is knocked out of play away from
// the pinball at 0.3 times its speed.
void msw::CStgGmk_Pinball_Act_Dodge::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
{
    CStgSrtMove *pSrt = GetSrt();
    bcs::FVEC3 bodyPos = pBody->GetPos();
    bcs::FVEC3 ballPos = pBall->GetPos();
    bcs::FVEC3 vel = pSrt->GetVel();
    float speed = bcs::FVEC3Len(&vel) + GetParam_BallKickPower();

    bcs::FVEC3 dir;
    VecSub(dir, bodyPos, ballPos);
    dir.y = 0.0f;
    VecNormalize(dir, dir);

    Reflect(vel, dir);
    VecNormalize(vel, vel);
    bcs::FVEC3Scale(&vel, &vel, speed);
    pSrt->SetVel(vel);

    if (pBall != 0) {
        bcs::FVEC3 outDir;
        bcs::FVEC3 ballVel = pBall->GetVelocity();

        VecSub(outDir, pBall->GetPos(), GetSrt()->GetPos());
        outDir.y = 0.0f;
        VecNormalize(outDir, outDir);
        bcs::FVEC3Scale(&outDir, &outDir, 0.3f * bcs::FVEC3Len(&ballVel));
        pBall->SetOutBallMode(outDir, 0x80, -1);
        KickHitEffect(pBall->GetPos());
    }
}

float msw::CStgGmk_Pinball_Act_Dodge::GetShotAngle() const
{
    return m_fShotAngle;
}

// The size parameter is a diameter.
float msw::CStgGmk_Pinball_Act_Dodge::GetParam_Radius() const
{
    return 0.5f * DODGE_PRM_S12_BALL_SIZE;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_Height() const
{
    return DODGE_PRM_S12_BALL_SIZE;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_CollRadius() const
{
    return 0.5f * DODGE_PRM_S12_BALL_COLLSIZE;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_CollHeight() const
{
    return DODGE_PRM_S12_BALL_COLLSIZE;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_AtkCollRadius() const
{
    return 0.3f + GetParam_CollRadius();
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_Interval() const
{
    return -1.0f;
}

void msw::CStgGmk_Pinball_Act_Dodge::GetParam_OuterRect(float *pLeft, float *pRight, float *pFront,
                                                        float *pBack) const
{
    CStageMan::GetInstance()->GetOuterFieldRect(pLeft, pRight, pFront, pBack);
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_Speed() const
{
    return DODGE_PRM_S12_BALL_SPEED;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_VelLimit() const
{
    return DODGE_PRM_S12_BALL_SPEED_LIMIT;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_Dec() const
{
    return DODGE_PRM_S12_BALL_DECEL;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_ForceAcc() const
{
    return DODGE_PRM_S12_BALL_FORCE_ACC;
}

long msw::CStgGmk_Pinball_Act_Dodge::GetParam_EndLineEnabled() const
{
    return false;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_GuideWidth() const
{
    return 4.75f;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_GuideDepth() const
{
    return 2.4f;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_GuideDecay() const
{
    return DODGE_PRM_S12_BALL_GUIDE_DECAY;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_KickPower() const
{
    return DODGE_PRM_S12_BALL_KICK_POW;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_BallKickPower() const
{
    return DODGE_PRM_S12_BALL_BALL_KICK_POW;
}

long msw::CStgGmk_Pinball_Act_Dodge::GetParam_Effect(long type) const
{
    if (type == 2) {
        return 0xC;
    }
    return -1;
}

float msw::CStgGmk_Pinball_Act_Dodge::GetParam_AtkHitEffectFrame() const
{
    return DODGE_PRM_S12_BALL_DOWN_TIME;
}

// The team with more lives left over all its characters wins; -1 on a tie.
long msw::CPinballManDodge::GetWinTeam() const
{
    long chr;
    long life[2] = { 0, 0 };

    for (long team = 0; team < 2; team++) {
        long num = CGameDodge::GetVariableChrNum(team);
        chr = team;
        for (long i = 0; i < num; i++) {
            life[team] += CGameDodge::GetChrRestLife(chr);
            chr += 2;
        }
    }

    if (life[0] > life[1]) {
        return 0;
    }
    if (life[0] < life[1]) {
        return 1;
    }
    return -1;
}

long msw::CPinballManDodge::GetParam_BallNum() const
{
    return DODGE_PRM_S12_BALL_NUM;
}

// The intervals are in seconds, returned in frames.
float msw::CPinballManDodge::GetParam_InitialInterval() const
{
    return 60.0f * DODGE_PRM_S12_BALL_INIT_SLEEP;
}

float msw::CPinballManDodge::GetParam_Interval() const
{
    return 60.0f * DODGE_PRM_S12_BALL_INTERVAL;
}

// The angle is in degrees.
float msw::CPinballManDodge::GetParam_ShotAngle() const
{
    return bcs::DEG_TO_RAD(DODGE_PRM_S12_BALL_ANGLE);
}

long msw::CPinballManDodge::GetParam_ShotSideRate() const
{
    return DODGE_PRM_S12_BALL_SIDE_RATE;
}

void msw::CPinballMan::MechaCancel()
{
    GetMecha()->Cancel();
}

void msw::CPinballMan::MechaReset()
{
    GetMecha()->Reset();
}

void msw::CPinballMan::SetMechaMdl(CStageMdlListNode *pNode)
{
    GetMecha()->SetMdl(pNode);
}

msw::CStgGmk_Pinball *msw::CPinballMan::GetGmk(long idx)
{
    if (m_ppGmk != 0) {
        return m_ppGmk[idx];
    }
    return 0;
}

const msw::CStgGmk_Pinball *msw::CPinballMan::GetGmk(long idx) const
{
    if (m_ppGmk != 0) {
        return m_ppGmk[idx];
    }
    return 0;
}

const bcs::FVEC3 &msw::CPinballMan::GetDefaultPos() const
{
    return m_vDefaultPos;
}
