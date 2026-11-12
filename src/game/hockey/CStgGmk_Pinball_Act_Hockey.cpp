// msw::CStgGmk_Pinball_Act_Hockey: the actor of one ball of hockey stage 12
// (pinball). It supplies the HOCKEY_PRM_S12_BALL_* tuning to
// msw::CStgGmk_Pinball_Act (a DOL class) and bounces off what it hits: the
// puck, a shot, and the goal posts. The
// file also holds the stage's pinball manager, msw::CPinballManHockey, and
// the weak copies of the inline functions of the classes whose vtables it
// emits (CPinballMan, CStgGmk_Pinball, CStgGmk_Pinball_Hockey,
// CStgGmk_Pinball_Act). CPinballMan's are defined out of line at the end
// instead, in the same order (as in VolleyF), so that the stage units that
// include its header do not change.
#include "game/msw/CStgGmk_Pinball_Hockey.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollDefUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CPinballManHockey.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgSrt.hpp"
#include "game/msw/HockeyPrm.hpp"

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

msw::CStgGmk_Pinball_Act_Hockey::~CStgGmk_Pinball_Act_Hockey()
{
}

// Counts the reflect wait down to zero.
void msw::CStgGmk_Pinball_Act_Hockey::OnUpdate(float rate)
{
    CStgGmk_Pinball_Act::OnUpdate(rate);
    m_fReflectWait -= rate;
    float wait = 0.0f;
    if (m_fReflectWait > wait) {
        wait = m_fReflectWait;
    }
    m_fReflectWait = wait;
}

// The ball's body unit 0 does not take target 4 (meaning not known).
void msw::CStgGmk_Pinball_Act_Hockey::UpdateColl()
{
    CStgGmk_Pinball_Act::UpdateColl();
    unsigned long flag = m_BodyColl.GetTargetFlag(0);
    m_BodyColl.SetTargetFlag(0, flag & ~4);
}

// An attack of kind 5 (a shot, meaning guessed) from another owner bounces
// the ball off it like the puck does in OnDamage, faster by
// GetParam_BallKickPower, and plays sound 0xD77 at rPos. Anything else
// goes to the base class.
unsigned long msw::CStgGmk_Pinball_Act_Hockey::OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                          const bcs::FVEC3 &rPos)
{
    if (pAtk != 0 && pAtk->GetOwnerKind() == 5 && pAtk->m_nOwnerNo != pBody->m_nOwnerIdx) {
        CStgSrtMove *pSrt = GetSrt();
        // Declared in this order for the original's stack layout.
        bcs::FVEC3 bodyPos = pBody->GetPos();
        bcs::FVEC3 atkPos;
        atkPos.x = pAtk->m_fPosX;
        atkPos.y = pAtk->m_fPosY;
        atkPos.z = pAtk->m_fPosZ;
        bcs::FVEC3 vel = pSrt->GetVel();
        float speed = bcs::FVEC3Len(&vel) + GetParam_BallKickPower();

        bcs::FVEC3 dir;
        VecSub(dir, bodyPos, atkPos);
        dir.y = 0.0f;
        VecNormalize(dir, dir);

        Reflect(vel, dir);
        VecNormalize(vel, vel);
        bcs::FVEC3Scale(&vel, &vel, speed);
        pSrt->SetVel(vel);

        CGameSnd::Play(0xD77, rPos, 1, -1, 0.0f, 1.0f, 0.0f);
        return 0;
    }
    return CStgGmk_Pinball_Act::OnBodyDamage(pBody, pAtk, rPos);
}

// The puck (pBall) hit the ball's defence volume pDef. Once every 10
// frames: the puck is sent away from the ball (rVel becomes the unit
// direction in the ground plane; its speed is measured but not used), and
// the ball bounces off the puck as in OnDamage. The result 0xA01 is passed
// back to the puck (meaning not known).
unsigned long msw::CStgGmk_Pinball_Act_Hockey::OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall,
                                                          bcs::FVEC3 &rPos, bcs::FVEC3 &rVel)
{
    if (m_fReflectWait > 0.0f) {
        return 0;
    }

    float ballSpeed = bcs::FVEC3Len(&rVel);
    bcs::FVEC3 out;
    VecSub(out, pBall->GetPos(), GetSrt()->GetPos());
    out.y = 0.0f;
    VecNormalize(rVel, out);
    m_fReflectWait = 10.0f;

    CStgSrtMove *pSrt = GetSrt();
    // Declared in this order for the original's stack layout.
    bcs::FVEC3 defPos = pDef->GetPos();
    bcs::FVEC3 ballPos = pBall->GetPos();
    bcs::FVEC3 vel = pSrt->GetVel();
    float speed = bcs::FVEC3Len(&vel) + GetParam_BallKickPower();

    bcs::FVEC3 dir;
    VecSub(dir, defPos, ballPos);
    dir.y = 0.0f;
    VecNormalize(dir, dir);

    Reflect(vel, dir);
    VecNormalize(vel, vel);
    bcs::FVEC3Scale(&vel, &vel, speed);
    pSrt->SetVel(vel);
    return 0xA01;
}

// The puck (pBall) hit this pinball's body pBody. The pinball bounces off
// it: unless it is already moving away from the puck (in the XZ plane), its
// velocity is mirrored about that direction; its speed grows by
// GetParam_BallKickPower. The puck is knocked out of play away from the
// pinball, keeping (1 - HOCKEY_PRM_S12_BALL_HIT_DECAY) of its speed.
void msw::CStgGmk_Pinball_Act_Hockey::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
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
        bcs::FVEC3Scale(&outDir, &outDir, bcs::FVEC3Len(&ballVel) * (1.0f - HOCKEY_PRM_S12_BALL_HIT_DECAY));
        pBall->SetOutBallMode(outDir, 0x80, -1);
        KickHitEffect(pBall->GetPos());
    }
}

float msw::CStgGmk_Pinball_Act_Hockey::GetShotAngle() const
{
    return m_fShotAngle;
}

// After the walls of the base class, the goals: a ball that hits a goal
// post (DetectBallGoalIn gives 0x20 or 0x40) is put just outside it and
// mirrored off it, and sound 0 plays. The goals never score.
int msw::CStgGmk_Pinball_Act_Hockey::WallHitCheck(bcs::FVEC3 *pPos, bcs::FVEC3 *pVel, const bcs::FVEC3 &rPrev,
                                                 const bcs::FVEC3 &rPos, const bcs::FVEC3 &rVel)
{
    bcs::FVEC3 prev = rPrev;
    bcs::FVEC3 pos = rPos;
    bcs::FVEC3 vel = rVel;

    int out = CStgGmk_Pinball_Act::WallHitCheck(&pos, &vel, rPrev, rPos, rVel);
    if (pPos != 0) {
        *pPos = pos;
    }
    if (pVel != 0) {
        *pVel = vel;
    }
    if (out) {
        return out;
    }

    long goal;
    bcs::FVEC3 hitPos;
    bcs::FVEC3 normal;
    int hit = false;
    unsigned long result = CCollMan::DetectBallGoalIn(goal, hitPos, normal, prev, pos, GetParam_CollRadius());
    if (result == 0x40 || result == 0x20) {
        hit = true;
        float dot = 2.0f * bcs::FVEC3Dot(&vel, &normal);
        bcs::FVEC3 ofs;
        bcs::FVEC3Scale(&ofs, &normal, 0.01f);
        bcs::FVEC3Add(&pos, &hitPos, &ofs);
        bcs::FVEC3 back;
        bcs::FVEC3Scale(&back, &normal, dot);
        bcs::FVEC3Sub(&vel, &vel, &back);
    }
    if (hit) {
        SE_PlayByParam(0);
    }

    if (pPos != 0) {
        *pPos = pos;
    }
    if (pVel != 0) {
        *pVel = vel;
    }
    return out;
}

// The size parameter is a diameter.
float msw::CStgGmk_Pinball_Act_Hockey::GetParam_Radius() const
{
    return 0.5f * HOCKEY_PRM_S12_BALL_SIZE;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_Height() const
{
    return HOCKEY_PRM_S12_BALL_SIZE;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_CollRadius() const
{
    return 0.5f * HOCKEY_PRM_S12_BALL_COLLSIZE;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_CollHeight() const
{
    return HOCKEY_PRM_S12_BALL_COLLSIZE;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_AtkCollRadius() const
{
    return GetParam_CollRadius();
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_Interval() const
{
    return -1.0f;
}

// The court, with the front and back edges moved inwards by the guide
// depth less 2.5 (the court radius is read and not used).
void msw::CStgGmk_Pinball_Act_Hockey::GetParam_OuterRect(float *pLeft, float *pRight, float *pFront,
                                                        float *pBack) const
{
    CStageMan *pStage = CStageMan::GetInstance();
    pStage->GetCourtRect(pLeft, pRight, pFront, pBack);
    pStage->GetCourtRadius();
    float ofs = GetParam_GuideDepth() - 2.5f;
    if (pFront != 0) {
        *pFront += ofs;
    }
    if (pBack != 0) {
        *pBack -= ofs;
    }
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_Speed() const
{
    return HOCKEY_PRM_S12_BALL_SPEED;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_VelLimit() const
{
    return HOCKEY_PRM_S12_BALL_SPEED_LIMIT;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_Dec() const
{
    return HOCKEY_PRM_S12_BALL_DECEL;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_ForceAcc() const
{
    return HOCKEY_PRM_S12_BALL_FORCE_ACC;
}

long msw::CStgGmk_Pinball_Act_Hockey::GetParam_EndLineEnabled() const
{
    return HOCKEY_PRM_S12_BALL_END_LINE;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_GuideWidth() const
{
    return 9.9f;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_GuideDepth() const
{
    return 5.0f;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_GuideDecay() const
{
    return HOCKEY_PRM_S12_BALL_GUIDE_DECAY;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_KickPower() const
{
    return HOCKEY_PRM_S12_BALL_KICK_POW;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_BallKickPower() const
{
    return HOCKEY_PRM_S12_BALL_BALL_KICK_POW;
}

long msw::CStgGmk_Pinball_Act_Hockey::GetParam_Effect(long type) const
{
    if (type == 2) {
        return 0xC;
    }
    return -1;
}

float msw::CStgGmk_Pinball_Act_Hockey::GetParam_AtkHitEffectFrame() const
{
    return HOCKEY_PRM_S12_BALL_DOWN_TIME;
}

// The team with the higher score in the current period wins; -1 on a tie.
long msw::CPinballManHockey::GetWinTeam() const
{
    long period = CGame::g_GameWork.GetPeriod();
    long score[2] = { 0, 0 };
    for (long i = 0; i < 2; i++) {
        score[i] = CGame::GetTeamPeriodScore(i, period);
    }
    if (score[0] > score[1]) {
        return 0;
    }
    if (score[0] < score[1]) {
        return 1;
    }
    return -1;
}

long msw::CPinballManHockey::GetParam_BallNum() const
{
    return HOCKEY_PRM_S12_BALL_NUM;
}

// The intervals are in seconds, returned in frames.
float msw::CPinballManHockey::GetParam_InitialInterval() const
{
    return 60.0f * HOCKEY_PRM_S12_BALL_INIT_SLEEP;
}

float msw::CPinballManHockey::GetParam_Interval() const
{
    return 60.0f * HOCKEY_PRM_S12_BALL_INTERVAL;
}

// The angle is in degrees.
float msw::CPinballManHockey::GetParam_ShotAngle() const
{
    return bcs::DEG_TO_RAD(HOCKEY_PRM_S12_BALL_ANGLE);
}

long msw::CPinballManHockey::GetParam_ShotSideRate() const
{
    return HOCKEY_PRM_S12_BALL_SIDE_RATE;
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
