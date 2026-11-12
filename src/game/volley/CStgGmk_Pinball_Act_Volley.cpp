// msw::CStgGmk_Pinball_Act_Volley: the actor of a ball of volley stage 12,
// with the VOLLEY_PRM_S12_BALL_* tuning for msw::CStgGmk_Pinball_Act (a DOL
// class). A volleyball that touches the pinball ball knocks it away. The
// file also has the volley pinball gimmick (CStgGmk_Pinball_Volley) and its
// manager (CPinballManVolley, made by CStageVolley12).
//
// In the original, CPinballManVolley's functions and the inline functions
// of the DOL classes CPinballMan and CStgGmk_Pinball_Act are weak copies
// emitted here. CPinballManVolley's and CPinballMan's are defined out of
// line below instead, in the same order, so that game/volley/CStageVolley12.cpp
// (which includes their headers) does not change.
#include "game/msw/CPinballManVolley.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStgGmk_Pinball.hpp"
#include "game/msw/CStgGmk_Pinball_Act.hpp"
#include "game/msw/CStgSrt.hpp"
#include "game/msw/VolleyPrm.hpp"

namespace msw {

    class CStgGmk_Pinball_Act_Volley : public CStgGmk_Pinball_Act {
        public:
            CStgGmk_Pinball_Act_Volley(CStgGmk *pParent) : CStgGmk_Pinball_Act(pParent) {}
            virtual ~CStgGmk_Pinball_Act_Volley();

            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);

            virtual float GetParam_AtkHitEffectFrame() const;
            virtual long GetParam_Effect(long type) const;
            virtual float GetParam_BallKickPower() const;
            virtual float GetParam_KickPower() const;
            virtual float GetParam_GuideDecay() const;
            virtual float GetParam_GuideDepth() const;
            virtual float GetParam_GuideWidth() const;
            virtual long GetParam_EndLineEnabled() const;
            virtual float GetParam_ForceAcc() const;
            virtual float GetParam_Dec() const;
            virtual float GetParam_VelLimit() const;
            virtual float GetParam_Speed() const;
            virtual float GetParam_Interval() const;
            virtual float GetParam_AtkCollRadius() const;
            virtual float GetParam_CollHeight() const;
            virtual float GetParam_CollRadius() const;
            virtual float GetParam_Height() const;
            virtual float GetParam_Radius() const;
            virtual float GetShotAngle() const;
    };

    // The pinball gimmick of volley stage 12 (CPinballManVolley::NewGmk).
    class CStgGmk_Pinball_Volley : public CStgGmk_Pinball {
        public:
            CStgGmk_Pinball_Volley(bcs::CTask *pParent, int arg) : CStgGmk_Pinball(pParent, arg) {}
            virtual ~CStgGmk_Pinball_Volley() {}

            virtual CStgDrv *NewDrv(long heap) { return new (heap) CStgGmk_Pinball_Drv(this); }
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Pinball_Act_Volley(this); }
    };
}

msw::CStgGmk_Pinball_Act_Volley::~CStgGmk_Pinball_Act_Volley()
{
}

// A volleyball in play bounces the pinball ball off it: the ball's
// velocity is mirrored on the line between the two (in the ground plane)
// and keeps its speed. Then the volleyball is knocked away, the kick effect
// plays, and the ball ignores further touches for 15 frames.
void msw::CStgGmk_Pinball_Act_Volley::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
{
    if (IsAlive()) {
        if (IsInvincible()) {
            return;
        }

        if (pBall->IsShootMode()) {
            CStgSrtMove *pSrt = GetSrt();
            // Declared in this order for the original's stack layout.
            bcs::FVEC3 bodyPos = pBody->GetPos();
            bcs::FVEC3 ballPos = pBall->GetPos();
            bcs::FVEC3 vel = pSrt->GetVel();
            float speed = bcs::FVEC3Len(&vel);

            bcs::FVEC3 normal;
            bcs::FVEC3Sub(&normal, &bodyPos, &ballPos);
            normal.y = 0.0f;
            bcs::BCS_Vec_Normalize(normal, normal);

            float dot = 2.0f * bcs::FVEC3Dot(&vel, &normal);
            if (!(dot > 0.0f)) {
                bcs::FVEC3 ofs;
                bcs::FVEC3Scale(&ofs, &normal, dot);
                bcs::FVEC3Sub(&vel, &vel, &ofs);
            }
            bcs::BCS_Vec_Normalize(vel, vel);
            bcs::FVEC3Scale(&vel, &vel, speed);
            pSrt->SetVel(vel);
        }

        if (pBall != 0) {
            bcs::FVEC3 pos = GetSrt()->GetPos();
            pBall->SetVolleyAnyHitMode(&pos, 0);
            KickHitEffect(pBall->GetPos());
        }
        SetInvincible(15.0f);
    }
}

// The parameters. They are inline in the original (weak copies, in this
// order); defined here so that they come before CPinballManVolley's.
float msw::CStgGmk_Pinball_Act_Volley::GetShotAngle() const
{
    return m_fShotAngle;
}

// The sizes are diameters.
float msw::CStgGmk_Pinball_Act_Volley::GetParam_Radius() const
{
    return 0.5f * VOLLEY_PRM_S12_BALL_SIZE;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_Height() const
{
    return VOLLEY_PRM_S12_BALL_SIZE;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_CollRadius() const
{
    return 0.5f * VOLLEY_PRM_S12_BALL_COLLSIZE;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_CollHeight() const
{
    return VOLLEY_PRM_S12_BALL_COLLSIZE;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_AtkCollRadius() const
{
    return GetParam_CollRadius() + 0.3f;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_Interval() const
{
    return -1.0f;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_Speed() const
{
    return VOLLEY_PRM_S12_BALL_SPEED;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_VelLimit() const
{
    return VOLLEY_PRM_S12_BALL_SPEED_LIMIT;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_Dec() const
{
    return VOLLEY_PRM_S12_BALL_DECEL;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_ForceAcc() const
{
    return VOLLEY_PRM_S12_BALL_FORCE_ACC;
}

long msw::CStgGmk_Pinball_Act_Volley::GetParam_EndLineEnabled() const
{
    return VOLLEY_PRM_S12_BALL_END_LINE;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_GuideWidth() const
{
    return 7.2f;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_GuideDepth() const
{
    return 2.4f;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_GuideDecay() const
{
    return VOLLEY_PRM_S12_BALL_GUIDE_DECAY;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_KickPower() const
{
    return VOLLEY_PRM_S12_BALL_KICK_POW;
}

float msw::CStgGmk_Pinball_Act_Volley::GetParam_BallKickPower() const
{
    return VOLLEY_PRM_S12_BALL_BALL_KICK_POW;
}

long msw::CStgGmk_Pinball_Act_Volley::GetParam_Effect(long type) const
{
    if (type == 2) {
        return 12;
    }
    return -1;
}

// Frames of the hit effect of an attack.
float msw::CStgGmk_Pinball_Act_Volley::GetParam_AtkHitEffectFrame() const
{
    return VOLLEY_PRM_S12_BALL_DOWN_TIME;
}

// The team ahead in the current period wins (0 or 1), -1 on a draw.
long msw::CPinballManVolley::GetWinTeam() const
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

long msw::CPinballManVolley::GetParam_BallNum() const
{
    return VOLLEY_PRM_S12_BALL_NUM;
}

// The times are in seconds, at 60 frames a second.
float msw::CPinballManVolley::GetParam_InitialInterval() const
{
    return 60.0f * VOLLEY_PRM_S12_BALL_INIT_SLEEP;
}

float msw::CPinballManVolley::GetParam_Interval() const
{
    return 60.0f * VOLLEY_PRM_S12_BALL_INTERVAL;
}

// The angle is in degrees.
float msw::CPinballManVolley::GetParam_ShotAngle() const
{
    return 0.017453292f * VOLLEY_PRM_S12_BALL_ANGLE;
}

long msw::CPinballManVolley::GetParam_ShotSideRate() const
{
    return VOLLEY_PRM_S12_BALL_SIDE_RATE;
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

msw::CStgGmk_Pinball *msw::CPinballManVolley::NewGmk(bcs::CTask *pParent, int arg)
{
    return new (1) CStgGmk_Pinball_Volley(pParent, arg);
}

long msw::CPinballManVolley::GetMaxNum() const
{
    return 0x20;
}

msw::CPinballManVolley::~CPinballManVolley()
{
}
