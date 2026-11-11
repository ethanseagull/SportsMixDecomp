// msw::CChrCoreVolleyLuigi: Luigi in the volleyball game. His air trick
// pulls the ball in with his vacuum (CC_CMN::LUIGI::CcOvacuumForce), which
// swings from side to side while it pulls.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcOvacuumForce.hpp"
#include "nw4r/math/triangular.h"

namespace {

    // The node of Luigi's model the vacuum effect is attached to.
    const char *s_pVacuumNode = "hoos_top";

    // (guessed helpers) An angle brought into [0, 2 pi); degrees to radians;
    // fValue kept within [fMin, fMax].
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

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    // (guessed helper) The distance between two points in the XZ plane.
    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }

    inline float Min(float fValue, float fMax)
    {
        return fValue > fMax ? fMax : fValue;
    }

    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }
}

msw::CChrCoreVolleyLuigi::CChrCoreVolleyLuigi()
{
    m_bVacuum = 0;
    m_fVacuumRot = 0.0f;
    m_hVacuum = 0;
}

msw::CChrCoreVolleyLuigi::~CChrCoreVolleyLuigi()
{
}

inline void msw::CChrCoreVolleyLuigi::DeleteVacuum_()
{
    if (m_hVacuum) {
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hVacuum);
        if (pTask && !pTask->IsDelete()) {
            pTask->Delete();
        }
    }
    m_hVacuum = 0;
}

void msw::CChrCoreVolleyLuigi::Reset_()
{
    DeleteVacuum_();
    CChrCoreVolley::Reset_();
    m_bVacuum = 0;
    m_fVacuumRot = 0.0f;
}

// A dust effect where the block jump starts, and one on the character.
void msw::CChrCoreVolleyLuigi::SubStateBlockJumpJumpStart_(STATE_WORK_BLOCK_JUMP *pWork)
{
    CChrCoreVolley::SubStateBlockJumpJumpStart_(pWork);

    bcs::FVEC3 pos;
    GetTransNodePos(pos);
    pos.y = 0.0f;
    Eff_CreateSimple(0x656, pos, 3, 0, 0);
    Eff_CreateAttach(0x13, 0x657, m_nEffNode, 0, 0, 3, 0);
}

void msw::CChrCoreVolleyLuigi::DtorStateJump()
{
    CChrCoreVolley::DtorStateJump();
    Eff_Stop(0x13);
}

// The start of a diving receive: two effects on the character's root node.
void msw::CChrCoreVolleyLuigi::SubStateReceiveDoDiveReceiveDiveStart_(STATE_WORK_RECEIVE *)
{
    bcs::FVEC3 ofs(0.0f, 0.4f, -0.7f);

    Eff_CreateAttach(0x13, 0x658, 0, &ofs, 1, 3, 0);
    Eff_CreateAttach(0x14, 0x659, 0, &ofs, 1, 3, 0);
}

// Landing after a spike: a dust effect on the ground under the character.
void msw::CChrCoreVolleyLuigi::SubStateSpikeDoLandStart_(STATE_WORK_SPIKE *pWork)
{
    CChrCoreVolley::SubStateSpikeDoLandStart_(pWork);

    if (!(pWork->m_nFlag & 2) && (pWork->m_nFlag & 0x40) && (pWork->m_nFlag & 0x100)) {
        bcs::FVEC3 pos;
        GetTransNodePos(pos);
        pos.y = 0.0f;
        Eff_CreateSimple(0x655, pos, 3, 0, 0);
    }
}

void msw::CChrCoreVolleyLuigi::CtorStateAirTrick()
{
    CChrCoreVolley::CtorStateAirTrick();
    m_bVacuum = 0;
    m_fVacuumRot = 0.0f;
}

// While the vacuum pulls it swings round (100 degrees a second) and the
// force follows the character.
void msw::CChrCoreVolleyLuigi::LoopStateAirTrick()
{
    CChrCoreVolley::LoopStateAirTrick();

    if (m_bVacuum) {
        float fSpeed = 100.0f; // degrees a second
        float fStep = (1.0f / 60.0f) * DegToRad(fSpeed);
        m_fVacuumRot += m_fTimeScale * (fStep * m_fFrameStep);
        m_fVacuumRot = WrapRad(m_fVacuumRot);
    }

    if (m_hVacuum) {
        CC_CMN::LUIGI::CcOvacuumForce *pForce =
            static_cast<CC_CMN::LUIGI::CcOvacuumForce *>(bcs::CTaskMan::GetTask(m_hVacuum));
        if (pForce) {
            pForce->SetForceEnable(m_bVacuum, 0, 0.0f);
            pForce->SetForceBasePos(GetPos());
            pForce->SetForceYRot(GetRotY());
            pForce->SetForceLimit(80.0f);
            pForce->SetForceParam(4.0f, 8.0f, 0.0f, 30.0f, 30.0f, 31.0f);
        }
    }
}

void msw::CChrCoreVolleyLuigi::DtorStateAirTrick()
{
    CChrCoreVolley::DtorStateAirTrick();
    DeleteVacuum_();
    m_bVacuum = 0;
    m_fVacuumRot = 0.0f;
}

// The trick goes to the middle of the attack zone of the court the
// character defends.
void msw::CChrCoreVolleyLuigi::SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    bcs::FVEC4 rect;
    Util_GetGuardCourtAttackZoneRect_(rect);
    pWork->m_TargetPos.Set(0.5f * (rect.x + rect.y), 0.0f, 0.5f * (rect.z + rect.w));
    pWork->m_MoveParam.m_nType = 0xB;

    pWork->m_MoveParam.m_fDist = DistXZ(pWork->m_TargetPos, GetPos());
    pWork->m_MoveParam.m_fFrame = 0.0f;
    pWork->m_MoveParam.m_fFrameMax = pWork->m_fTrickFrame;
}

void msw::CChrCoreVolleyLuigi::SubStateAirTrickStartBallAdsorb_(STATE_WORK_AIR_TRICK *pWork, CBall *pBall)
{
    pWork->m_fAdsorbFrameMax = SubStateAirTrickGetBallAdsorbFrame_(pWork);
    pWork->m_fAdsorbFrame = 0.0f;
    pWork->m_AdsorbOfs.x = 0.0f;
    pWork->m_AdsorbOfs.y = 0.0f;
    pWork->m_AdsorbOfs.z = 0.0f;
    AttachBall(pBall->GetBallNo(), 5, pWork->m_fAdsorbFrameMax, 0);
    m_nStatusFlag &= ~0x2C;
}

// The ball comes in over the length of the pull motion, at most 100 frames.
float msw::CChrCoreVolleyLuigi::SubStateAirTrickGetBallAdsorbFrame_(STATE_WORK_AIR_TRICK *)
{
    float fFrame = AnimGetResFrameNum(0x75);
    return 100.0f > fFrame ? fFrame : 100.0f;
}

// The ball sinks from fHeightMax above the hand to the hand.
void msw::CChrCoreVolleyLuigi::SubStateAirTrickDoBallAdsorb_(STATE_WORK_AIR_TRICK *pWork)
{
    static const float fHeightMax = 10.0f;

    CBall *pBall = GetAttachBall();
    if (pBall && pWork->m_nAdsorbStep == 0) {
        bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
        float fFrameMax = pWork->m_fAdsorbFrameMax;
        if (fFrameMax > 0.0f) {
            pWork->m_fAdsorbFrame = AnimGetFrame(3);
            pWork->m_fAdsorbFrame = Min(pWork->m_fAdsorbFrame, fFrameMax);
            float fRate = Clamp(pWork->m_fAdsorbFrame / fFrameMax, 0.0f, 1.0f);
            bcs::FVEC3Scale(&pos, &pos, fRate);
            pos.y += Clamp(fHeightMax * nw4r::math::CosRad(1.5707964f * fRate), 0.0f, fHeightMax);
            if (fRate >= 1.0f) {
                pWork->m_nAdsorbStep++;
            }
        }
        pBall->SetAttachPos(pos, 0);
    }
}

void msw::CChrCoreVolleyLuigi::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x65, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
    ArtsOptCreateAttachEffectAuto(0, 0x67, 3);
    ArtsOptCreateAttachEffectAuto(1, 0x66, 3);
}

// The loop motion switches the vacuum on.
void msw::CChrCoreVolleyLuigi::SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLoopMotion_(pWork);

    ArtsOptCreateAttachEffectAuto(2, 0x68, 3);
    ArtsOptCreateAttachEffect(3, 0x56A, s_pVacuumNode, 0, 1, 3);
    m_bVacuum = 1;

    DeleteVacuum_();
    CC_CMN::LUIGI::CcOvacuumForce *pForce =
        new (0x100) CC_CMN::LUIGI::CcOvacuumForce(CGame::GetTempParent(), GetPlayerNo(), GetTeamNo());
    if (pForce) {
        m_hVacuum = pForce->GetId();
        pForce->SetForceDmgEffectFrm(0.0f);
        pForce->SetForceParam(4.0f, 8.0f, 0.0f, 30.0f, 30.0f, 31.0f);
    }
}

// The shot switches the vacuum off. The original reads the character's
// parameters twice here and uses neither (whatever used them is not in this
// build).
void msw::CChrCoreVolleyLuigi::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0x69, 3);
    const CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *pParam = GetCcParam_();
    if (pWork->m_nArtsLevel < 1) {
        pParam = GetCcParam_();
    }
    (void)pParam;

    ArtsOptStopEffect(0);
    ArtsOptStopEffect(1);
    ArtsOptStopEffect(2);
    ArtsOptStopEffect(3);
    ArtsOptFadeOut(5.0f, 11.0f, 1);

    DeleteVacuum_();
    m_bVacuum = 0;
}

void msw::CChrCoreVolleyLuigi::SubStateAirTrickPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLandMotion_(pWork);

    ArtsOptFadeOut(5.0f, 0.0f, 0);
    m_bVacuum = 0;
}

void msw::CChrCoreVolleyLuigi::SubStateAirTrickPlayLandEffect_(STATE_WORK_AIR_TRICK *)
{
    Eff_CreateAttachAuto(0x15, 0x6A, 3);
    SE_PlayArtsLandSE();
}

void msw::CChrCoreVolleyLuigi::SubStateAirTrickShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickShootBall_(pWork);

    CGame::StartCameraQuake(0x28, -1, -1);
}

// While the vacuum pulls, the character turns with it (45 degrees each way).
int msw::CChrCoreVolleyLuigi::SubStateAirTrickIsStartArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bStart = CChrCoreVolley::SubStateAirTrickIsStartArtsShot_(pWork);
    if (!bStart && m_bVacuum) {
        float fSwing = 45.0f;
        float fSwingMax = DegToRad(fSwing);
        float fRot = fSwingMax * nw4r::math::SinRad(m_fVacuumRot);
        SetRotY(pWork->m_fRotY + fRot);
    }
    return bStart;
}
