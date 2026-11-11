// msw::CChrCoreHockeyLuigi: Luigi in the hockey game. His special shot pulls
// the other team in with his vacuum (CC_CMN::LUIGI::CcOvacuumForce) while he
// turns towards a target; a pad player aims it with the target marker, the
// computer picks an opponent in front of it.
#include "game/msw/CChrCoreHockey.hpp"

#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcOvacuumForce.hpp"

#include <math.h>

namespace {

    // (guessed helper) An angle brought into [0, 2 pi).
    inline float WrapRad(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    // fValue kept within [fMin, fMax].
    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }
}

msw::CChrCoreHockeyLuigi::CChrCoreHockeyLuigi()
{
    m_hVacuum = 0;
    m_bVacuum = 0;
    m_fComAimWait = 0.0f;
    m_fComAimKeep = 0.0f;
    m_bComAimTimeUp = 0;
    m_fComAimDelay = 0.0f;
}

msw::CChrCoreHockeyLuigi::~CChrCoreHockeyLuigi()
{
}

inline void msw::CChrCoreHockeyLuigi::DeleteVacuum_()
{
    if (m_hVacuum) {
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hVacuum);
        if (pTask && !pTask->IsDelete()) {
            pTask->Delete();
        }
    }
    m_hVacuum = 0;
}

// The computer's level: its own, or with pad players in the game the level
// set for the team they play against (or a fixed one, see GameWork 0x151).
inline float msw::CChrCoreHockeyLuigi::GetComDatLevel_()
{
    float fLevel = GetComLevel();
    if (GetExistPTPadDrvNum_() > 0) {
        if (CGame::g_GameWork.m_Unk151) {
            fLevel = CComMan::GetDatF(0xF53, 0L);
        } else {
            fLevel = CComMan::GetDatF(0xF4B, CGame::GetTeamComLevel(1 - GetTeamNo()));
        }
    }
    return fLevel;
}

void msw::CChrCoreHockeyLuigi::Reset_()
{
    DeleteVacuum_();
    m_bVacuum = 0;
    CChrCoreHockey::Reset_();
}

// While the vacuum pulls, the target marker sets the yaw the shot aims at.
int msw::CChrCoreHockeyLuigi::MoveTargetMarkerPos(float fRotY)
{
    if (GetState() != 0x27) {
        return 0;
    }
    if (m_bVacuum) {
        STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
        pWork->m_fTargetRotY = WrapRad(fRotY);
        return 1;
    }
    return 0;
}

// The computer's aim: after a delay it looks for the opponent in the oval in
// front of it that is closest in direction to the goal, and aims at that
// one (straight at the goal when within 10 degrees). When nobody is in
// front for long enough, it stops aiming after a while.
int msw::CChrCoreHockeyLuigi::AutoMoveTargetMarkerPos()
{
    if (GetState() != 0x27) {
        return 0;
    }
    if (m_bVacuum) {
        m_fComAimWait -= m_fFrameStep * m_fTimeScale;
        int bGiveUp = 0;
        CChrCore *pTarget = 0;
        float fMinDiff = 999.0f;
        if (m_fComAimDelay > 0.0f) {
            m_fComAimDelay -= m_fFrameStep * m_fTimeScale;
            if (m_fComAimDelay <= 0.0f) {
                m_fComAimDelay = 0.0f;
            } else {
                return 0;
            }
        }

        float fWidth = 0.0f;
        if (!IsPadCtrl()) {
            fWidth = CComMan::GetDatF(0x6D8, GetComDatLevel_());
        }

        STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
        float fDist = GetDist_ToPosXZ(GetTargetGoalPos());
        float fRotY = GetRotY_ToPosXZ(GetTargetGoalPos());
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable() || pChr->IsHockeyDamage() || pChr->IsKeeper() ||
                pChr->GetTeamNo() == GetTeamNo()) {
                continue;
            }
            bcs::FVEC3 pos;
            pChr->GetCompPos(pos);
            if (HitCheckOval_(GetPos(), pos, fRotY, fDist, fWidth, 0.0f)) {
                float fGoalRot = GetRotY_FromPosXZ(GetTargetGoalPos());
                float fDiff = fabsf(CalcRotDiff_(GetRotY_FromPosXZ(pChr->GetPos()), fGoalRot));
                if (fMinDiff > fDiff) {
                    fMinDiff = fDiff;
                    pTarget = pChr;
                }
            } else if (m_fComAimWait <= 0.0f) {
                m_bComAimTimeUp = 1;
            }
        }

        if (m_bComAimTimeUp) {
            m_fComAimKeep -= m_fFrameStep * m_fTimeScale;
            if (m_fComAimKeep <= 0.0f) {
                pTarget = 0;
                bGiveUp = 1;
            }
        }

        if (pTarget) {
            if (fMinDiff < DegToRad(10.0f)) {
                pWork->m_fTargetRotY = -1.0f;
            } else {
                pWork->m_fTargetRotY = WrapRad(pTarget->GetRotY_FromPosXZ(GetPos()));
            }
            return 1;
        }
        if (bGiveUp) {
            pWork->m_fUnk0C = 0.0f;
            return 1;
        }
    }
    return 0;
}

void msw::CChrCoreHockeyLuigi::CtorStateAirTrick()
{
    CChrCoreHockey::CtorStateAirTrick();

    m_bVacuum = 0;
    m_fComAimWait = 20.0f;
    m_fComAimKeep = 0.0f;
    m_fComAimDelay = 0.0f;
    m_bComAimTimeUp = 0;
    if (!IsPadCtrl()) {
        float fLevel = GetComDatLevel_();
        m_fComAimWait = CComMan::GetDatF(0x6D0, fLevel);
        m_fComAimKeep = CComMan::GetDatF(0x6E0, fLevel);
        m_fComAimDelay = 10.0f;
    }
}

// While the vacuum pulls, the character turns towards the aimed yaw (at most
// 25 degrees off the goal), and the force follows the character.
void msw::CChrCoreHockeyLuigi::LoopStateAirTrick()
{
    static const float fRngPrm = 25.0f; // degrees

    CChrCoreHockey::LoopStateAirTrick();

    if (m_bVacuum) {
        STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
        if (pWork->m_fTargetRotY >= 0.0f) {
            float fGoalRot = GetRotY_ToGoal();
            float fDiff = WrapRad(pWork->m_fTargetRotY - fGoalRot);
            if (fDiff > 3.1415927f) {
                fDiff -= 6.2831855f;
            }
            float fRng = DegToRad(fRngPrm);
            pWork->m_fRotY = WrapRad(fGoalRot + Clamp(fDiff, -fRng, fRng));
            SubStateArtsShotCalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, 16.0f);
        }
    }

    if (m_hVacuum) {
        CC_CMN::LUIGI::CcOvacuumForce *pForce =
            static_cast<CC_CMN::LUIGI::CcOvacuumForce *>(bcs::CTaskMan::GetTask(m_hVacuum));
        if (pForce) {
            pForce->SetForceEnable(m_bVacuum, 0, 0.0f);
            pForce->SetForceBasePos(GetPos());
            pForce->SetForceYRot(GetRotY());
            pForce->SetForceLimit(90.0f);
            pForce->SetForceParam(8.0f, 9.0f, 7.0f, 10.0f, 12.0f, 80.0f);
        }
    }

    // (guess) Unused: only puts the damage effect frame of
    // SubStateArtsShotPlayLoopMotion_ after these constants in .rodata, as
    // in the original (likely a stripped use of it here).
    float fDmgEffectFrame = 30.0f;
    (void)fDmgEffectFrame;
}

void msw::CChrCoreHockeyLuigi::DtorStateAirTrick()
{
    CChrCoreHockey::DtorStateAirTrick();

    DeleteVacuum_();
    m_bVacuum = 0;
    m_fComAimWait = 0.0f;
    m_fComAimDelay = 0.0f;
}

void msw::CChrCoreHockeyLuigi::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x86, 3);
    SE_PlayArtsJumpSE(0);
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
    ArtsOptCreateAttachEffectAuto(0, 0x87, 3);
}

// Ready once the trick motion and then the pull motion (0x9D) are over.
int msw::CChrCoreHockeyLuigi::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    if (pWork->m_nTrickStep == 0) {
        if (IsAnimEnd(3)) {
            AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
            pWork->m_nTrickStep++;
        }
    } else if (IsAnimEnd(3)) {
        bReady = 1;
    }
    return bReady;
}

// The loop motion switches the vacuum on.
void msw::CChrCoreHockeyLuigi::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(pWork);

    ArtsOptCreateAttachEffectAuto(2, 0x88, 3);
    ArtsOptCreateAttachEffectAuto(3, 0x89, 3);
    m_bVacuum = 1;
    pWork->m_nFlag |= 1;

    DeleteVacuum_();
    CC_CMN::LUIGI::CcOvacuumForce *pForce =
        new (0x100) CC_CMN::LUIGI::CcOvacuumForce(CGame::GetTempParent(), GetPlayerNo(), GetTeamNo());
    if (pForce) {
        m_hVacuum = pForce->GetId();
        pForce->SetForceDmgEffectFrm(30.0f);
        pForce->SetForceParam(8.0f, 9.0f, 7.0f, 10.0f, 12.0f, 80.0f);
    }
}

// The shot switches the vacuum off.
void msw::CChrCoreHockeyLuigi::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0x8B, 3);
    ArtsOptStopEffect(0);
    ArtsOptStopEffect(1);
    ArtsOptStopEffect(2);
    ArtsOptStopEffect(3);
    ArtsOptFadeOut(5.0f, 11.0f, 1);

    DeleteVacuum_();
    m_bVacuum = 0;
    pWork->m_nFlag &= ~1;
}

void msw::CChrCoreHockeyLuigi::SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotShootBall_(pWork);

    CGame::StartCameraQuake(0x28, -1, -1);
}

void msw::CChrCoreHockeyLuigi::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLandMotion_(pWork);

    ArtsOptFadeOut(5.0f, 0.0f, 0);
    m_bVacuum = 0;
}

void msw::CChrCoreHockeyLuigi::SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK *)
{
    SE_PlayArtsLandSE();
}
