// msw::CChrCoreBasketLuigi: Luigi in the basketball game. During his
// special shot his vacuum (CC_CMN::LUIGI::CcOvacuumForce) pulls the other
// team in while he turns towards the yaw his target marker sets (at most
// 45 degrees off the goal).
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcOvacuumForce.hpp"

namespace {

    // The node of Luigi's model the vacuum effect is attached to (a
    // null-terminated list).
    const char *s_apVacuumNode[] = {"hoos_top", 0};

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

    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    // Stand-in for an unknown user of 0.5 and -1, which the original keeps
    // in .rodata after the shot motion's constants but nothing reads
    // (likely stripped code).
    inline void UnusedReferences(float f)
    {
        if (f == 0.5f) {
        }
        if (f == -1.0f) {
        }
    }

    // (name guessed) 70 degrees; nothing in the unit reads it.
    float s_fUnkRange = DegToRad(70.0f);
}

msw::CChrCoreBasketLuigi::CChrCoreBasketLuigi()
{
    m_nUnk5E8 = 0;
    m_bVacuum = 0;
    m_hVacuum = 0;
}

msw::CChrCoreBasketLuigi::~CChrCoreBasketLuigi()
{
}

inline void msw::CChrCoreBasketLuigi::DeleteVacuum_()
{
    if (m_hVacuum) {
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hVacuum);
        if (pTask && !pTask->IsDelete()) {
            pTask->Delete();
        }
    }
    m_hVacuum = 0;
}

// A jump block from motion 3 or 0x67 raises dust where he stands.
void msw::CChrCoreBasketLuigi::CtorStateJumpBlock()
{
    CChrCoreBasket::CtorStateJumpBlock();

    long nMotId = AnimGetMotID(3, 0);
    if (nMotId == 3 || nMotId == 0x67) {
        bcs::FVEC3 pos;
        GetTransNodePos(pos);
        pos.y = 0.0f;
        Eff_CreateSimple(0x600, pos, 3, 0, 0);
        Eff_Stop(0xB);
        Eff_CreateAttach(0xB, 0x601, m_nEffNode, 0, 0, 3, 0);
    }
}

void msw::CChrCoreBasketLuigi::Reset_()
{
    DeleteVacuum_();
    m_bVacuum = 0;
    CChrCore::Reset_();
}

// While the vacuum pulls, the target marker sets the yaw the shot aims at.
int msw::CChrCoreBasketLuigi::MoveTargetMarkerPos(float fRotY)
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

void msw::CChrCoreBasketLuigi::CtorStateAirTrick()
{
    CChrCoreBasket::CtorStateAirTrick();

    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
    pWork->m_nUnkA8 = 40;
    m_bVacuum = 0;
}

// While the vacuum pulls, the character turns towards the aimed yaw (at most
// 45 degrees off the goal), and the force follows the character.
void msw::CChrCoreBasketLuigi::LoopStateAirTrick()
{
    static const float fRngPrm = 45.0f; // degrees

    CChrCoreBasket::LoopStateAirTrick();

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
            pForce->SetForceEnable(m_bVacuum, 1, 0.0f);
            pForce->SetForceBasePos(GetPos());
            pForce->SetForceYRot(GetRotY());
            pForce->SetForceLimit(30.0f);
            pForce->SetForceDmgEffectFrm(0.0f);
            pForce->SetForceParam(2.5f, 5.0f, 3.9f, 4.0f, 30.0f, 31.0f);
        }
    }
}

void msw::CChrCoreBasketLuigi::DtorStateAirTrick()
{
    CChrCoreBasket::DtorStateAirTrick();

    DeleteVacuum_();
    m_bVacuum = 0;
}

void msw::CChrCoreBasketLuigi::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x6B, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
    ArtsOptCreateAttachEffectAuto(0, 0x6D, 3);
    ArtsOptCreateAttachEffectAuto(1, 0x6C, 3);
}

// The loop motion switches the vacuum on.
void msw::CChrCoreBasketLuigi::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayLoopMotion_(pWork);

    ArtsOptCreateAttachEffectAuto(2, 0x6E, 3);
    ArtsOptCreateAttachEffect(3, 0x521, s_apVacuumNode[0], 0, 1, 3);
    m_bVacuum = 1;
    pWork->m_nFlag |= 0x200;

    DeleteVacuum_();
    CC_CMN::LUIGI::CcOvacuumForce *pForce =
        new (0x100) CC_CMN::LUIGI::CcOvacuumForce(CGame::GetTempParent(), GetPlayerNo(), GetTeamNo());
    if (pForce) {
        m_hVacuum = pForce->GetId();
        pForce->SetForceParam(2.5f, 5.0f, 3.9f, 4.0f, 30.0f, 31.0f);
    }
}

// The shot switches the vacuum off and tells every character.
void msw::CChrCoreBasketLuigi::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0x6F, 3);
    // (unused) The parameters are asked for twice and the results dropped,
    // perhaps a stripped choice of a value by pWork->m_nUnkA0.
    GetCcParam2_();
    if (pWork->m_nUnkA0 < 1) {
        GetCcParam2_();
    }
    ArtsOptStopEffect(0);
    ArtsOptStopEffect(1);
    ArtsOptStopEffect(2);
    ArtsOptStopEffect(3);
    ArtsOptFadeOut(5.0f, 11.0f, 1);

    DeleteVacuum_();
    m_bVacuum = 0;
    pWork->m_nFlag &= ~0x200;
    CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(0x33), 0, 0, 0, 0);
}

void msw::CChrCoreBasketLuigi::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayLandMotion_(pWork);

    ArtsOptFadeOut(5.0f, 0.0f, 0);
    m_bVacuum = 0;
}

void msw::CChrCoreBasketLuigi::SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK *pWork)
{
    Eff_CreateAttachAuto(0x15, 0x70, 3);
    SE_PlayArtsLandSE();
    UnusedReferences(pWork->m_fRotY);
}
