// msw::CChrCoreDodgeLuigi: Luigi in the dodgeball game. His special shot
// pulls the other team in with his vacuum (CC_CMN::LUIGI::CcOvacuumForce)
// while he turns towards the target of the shot.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcOvacuumForce.hpp"

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace {

    // The node of Luigi's model the vacuum effect is attached to.
    const char *s_pVacuumNode = "hoos_top";

    // (guessed) Stand-ins for code of the original that the link stripped.
    // Behaviour-free; see notes/decisions.md, "Dead float comparison". The
    // original's .rodata holds constants nothing left in the file reads, in
    // three runs: 0, 1, 2 pi and 256 / 2 pi (a sine table lookup) before
    // the vacuum's constants; 0.05, 60, 1/6, 0.2 and an int to float
    // conversion after the arts shot's; -1 after the diving catch offset.
    // They most likely belonged to CIntakeAir, whose other functions are
    // gone.
    inline void UnusedReferences(float f)
    {
        if (f == 0.0f) {
        }
        if (f == 1.0f) {
        }
        if (f == 6.2831855f) {
        }
        if (f == 40.743664f) {
        }
    }

    // (the unnamed int only tells this group from the first)
    inline void UnusedReferences(float f, int)
    {
        if (f == 0.05f) {
        }
        if (f == 60.0f) {
        }
        if (f == 1.0f / 6.0f) {
        }
        if (f == 0.2f) {
        }
        // the int to float conversion constant
        if (f == 4503601774854144.0) {
        }
    }

    inline void UnusedReferences(const bcs::FVEC3 &v)
    {
        if (v.x == -1.0f) {
        }
    }
}

msw::CChrCoreDodgeLuigi::CChrCoreDodgeLuigi()
{
    m_hVacuum = 0;
    m_bVacuum = 0;
}

msw::CChrCoreDodgeLuigi::~CChrCoreDodgeLuigi()
{
}

inline void msw::CChrCoreDodgeLuigi::DeleteVacuum_()
{
    if (m_hVacuum) {
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hVacuum);
        if (pTask && !pTask->IsDelete()) {
            pTask->Delete();
        }
    }
    m_hVacuum = 0;
}

void msw::CChrCoreDodgeLuigi::Reset_()
{
    DeleteVacuum_();
    m_bVacuum = 0;
    CChrCoreDodge::Reset_();
}

void msw::CChrCoreDodgeLuigi::CtorStateAirTrick()
{
    CChrCoreDodge::CtorStateAirTrick();
    m_bVacuum = 0;
}

// While the vacuum pulls, the character turns towards the target, and the
// force follows the character.
void msw::CChrCoreDodgeLuigi::LoopStateAirTrick()
{
    CChrCoreDodge::LoopStateAirTrick();
    UnusedReferences(m_fFrameStep);

    if (m_bVacuum) {
        STATE_WORK_AIR_TRICK_CMN *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK_CMN>();
        pWork->m_fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
        SubStateArtsShotCalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, 16.0f);
    }

    if (m_hVacuum) {
        CC_CMN::LUIGI::CcOvacuumForce *pForce =
            static_cast<CC_CMN::LUIGI::CcOvacuumForce *>(bcs::CTaskMan::GetTask(m_hVacuum));
        if (pForce) {
            pForce->SetForceEnable(m_bVacuum, 0, 30.0f);
            if (m_bVacuum) {
                pForce->SetForceBasePos(GetPos());
                pForce->SetForceYRot(GetRotY());
                pForce->SetForceLimit(120.0f);
                pForce->SetForceParam(3.0f, 4.0f, 20.0f, 20.1f, 15.0f, 16.0f);
            }
        }
    }
}

void msw::CChrCoreDodgeLuigi::DtorStateAirTrick()
{
    CChrCoreDodge::DtorStateAirTrick();
    DeleteVacuum_();
    m_bVacuum = 0;
}

void msw::CChrCoreDodgeLuigi::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x65, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
    ArtsOptCreateAttachEffectAuto(0, 0x67, 3);
    ArtsOptCreateAttachEffectAuto(1, 0x66, 3);
}

// The loop motion switches the vacuum on.
void msw::CChrCoreDodgeLuigi::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);

    ArtsOptCreateAttachEffectAuto(2, 0x68, 3);
    ArtsOptCreateAttachEffect(3, 0x51E, s_pVacuumNode, 0, 1, 3);
    m_bVacuum = 1;

    DeleteVacuum_();
    CC_CMN::LUIGI::CcOvacuumForce *pForce =
        new (0x100) CC_CMN::LUIGI::CcOvacuumForce(CGame::GetTempParent(), GetPlayerNo(), GetTeamNo());
    if (pForce) {
        m_hVacuum = pForce->GetId();
        pForce->SetForceDmgEffectFrm(0.0f);
        pForce->SetForceParam(3.0f, 4.0f, 20.0f, 20.1f, 15.0f, 16.0f);
    }
}

// The shot switches the vacuum off (the task itself goes when the state
// ends). The original reads the character's parameters twice here and uses
// neither (whatever used them is not in this build).
void msw::CChrCoreDodgeLuigi::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0x69, 3);
    const CHR_CORE_DODGE::CC_PARAM_DODGE *pParam = GetCcParam_();
    if (pWork->m_nArtsLevel < 1) {
        pParam = GetCcParam_();
    }
    (void)pParam;

    ArtsOptStopEffect(0);
    ArtsOptStopEffect(1);
    ArtsOptStopEffect(2);
    ArtsOptStopEffect(3);
    ArtsOptFadeOut(5.0f, 11.0f, 1);
    m_bVacuum = 0;
}

void msw::CChrCoreDodgeLuigi::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLandMotion_(pWork);

    ArtsOptFadeOut(5.0f, 0.0f, 0);
    m_bVacuum = 0;
}

void msw::CChrCoreDodgeLuigi::SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK_CMN *)
{
    Eff_CreateAttachAuto(0x15, 0x6A, 3);
    SE_PlayArtsLandSE();
    UnusedReferences(m_fFrameStep, 0);
}

void msw::CChrCoreDodgeLuigi::SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotShootBall_(pWork);

    CGame::StartCameraQuake(0x28, -1, -1);
}

int msw::CChrCoreDodgeLuigi::CanbeSwitchLockTarget() const
{
    return CanbeLockTarget();
}

msw::CChrCoreDodgeLuigi::CIntakeAir::~CIntakeAir()
{
    if (m_pMdl) {
        m_pMdl->DetachFromParent();
        if (m_pMdl) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
        }
        m_pMdl = 0;
    }
    m_nUnk08 = 0;
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
}

// The start of a diving catch: two effects on the character.
void msw::CChrCoreDodgeLuigi::OnDivingCatchStart_(STATE_WORK_DIVING_CATCH *)
{
    unsigned long nNode = m_nEffNode;
    bcs::FVEC3 ofs(0.0f, 0.4f, -0.7f);
    UnusedReferences(ofs);

    Eff_CreateAttach(0xD, 0x61A, nNode, &ofs, 0, 3, 0);
    Eff_CreateAttach(0xF, 0x619, nNode, 0, 0, 3, 0);
}
