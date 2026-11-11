// msw::CChrCoreVolleyKoopa: Bowser in the volleyball game. His air trick
// breathes a fireball (CC_CMN::KOOPA::CcFireBall) at the target marker; the
// volley game asks through IsVolleyArtsKoopaShotFire and
// IsVolleyArtsKoopaFireEnd whether it is out and whether it is over.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcFireBall.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/g3d/ResNode.hpp"

namespace msw {
    namespace CC10_VOLLEY {
        // The model node the fireball comes out of (his mouth).
        const char *_pszFireBallLauncherNodeName = "eff_kuchi";
    }
}

msw::CChrCoreVolleyKoopa::CChrCoreVolleyKoopa()
{
    m_hFireBall = 0;
}

msw::CChrCoreVolleyKoopa::~CChrCoreVolleyKoopa()
{
}

void msw::CChrCoreVolleyKoopa::Reset_()
{
    CChrCoreVolley::Reset_();
    m_hFireBall = 0;
}

void msw::CChrCoreVolleyKoopa::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
    Eff_CreateAttach(0x13, 0x688, m_nEffNode, 0, 1, 3, 0);
}

// Not zero once the air trick (state 0x27) has breathed its fireball.
int msw::CChrCoreVolleyKoopa::IsVolleyArtsKoopaShotFire() const
{
    if (GetState() == 0x27 && FsmqGetWork<STATE_WORK_AIR_TRICK>()->m_nAfterStep > 0) {
        return 1;
    }
    return 0;
}

// Not zero unless the air trick has breathed a fireball that is still
// flying.
int msw::CChrCoreVolleyKoopa::IsVolleyArtsKoopaFireEnd() const
{
    if (GetState() == 0x27) {
        if (FsmqGetWork<STATE_WORK_AIR_TRICK>()->m_nAfterStep <= 0) {
            return 0;
        }
        if (m_hFireBall == 0) {
            return 1;
        }
        CC_CMN::KOOPA::CcFireBall *pFire =
            static_cast<CC_CMN::KOOPA::CcFireBall *>(bcs::CTaskMan::GetTask(m_hFireBall));
        if (pFire && !pFire->IsDelete() && !pFire->IsEnd()) {
            return 0;
        }
    }
    return 1;
}

// Step 0 waits for the trick motion, then breathes the fireball and plays
// the shot motion 0x76; step 1 waits for that to end.
int msw::CChrCoreVolleyKoopa::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateAirTrickDoJumpMove_(pWork);
    SubStateAirTrickDoBallAdsorb_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                SubStateAirTrickFireKoopaFireBall_(pWork);
                pWork->m_nAfterStep = 1;
                AnimPlay(3, 0x76, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0x8F, 3);
                AnimSetSpeed(3, pWork->m_fAnimSpeed);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                pWork->m_nTrickStep++;
                bReady = 1;
            }
            break;
        default:
            bReady = 1;
            break;
    }
    return bReady;
}

// The fireball leaves his mouth (or his feet, without a model) for the
// target marker (or its default place), kept within the other court.
void msw::CChrCoreVolleyKoopa::SubStateAirTrickFireKoopaFireBall_(STATE_WORK_AIR_TRICK *)
{
    // (values' meanings not known)
    static CC_CMN::KOOPA::CcFireBall::FIRE_PARAM s_Param = {
        {0x5CD, 0x5CE, 0x5D2},
        {23.0f, 1.4f, 0.6f, 0.6f, 0.8f, 3.5f, 16.0f, 15.0f},
    };

    bcs::CMdlObj *pMdl = m_pMdl;
    unsigned long nNode = m_nEffNodeBlock;
    if (pMdl) {
        nw4r::g3d::ResNode node = pMdl->GetResMdl().GetResNode(CC10_VOLLEY::_pszFireBallLauncherNodeName);
        if (node.IsValid()) {
            nNode = node.GetID();
        }
    }

    bcs::FVEC3 pos(GetPos());
    if (pMdl) {
        bcs::FMTX34 mtx;
        pMdl->GetScnMtxPos(&mtx, nNode, bcs::CScnObj::SCNOBJMTXTYPE_1);
        pos.x = mtx.m[0][3];
        pos.y = mtx.m[1][3];
        pos.z = mtx.m[2][3];
    }

    bcs::FVEC3 target(GetPos());
    if (!GetVolleyTargetMarkerPos(target, 0)) {
        GetTargetMarkerDefaultPos_(6, GetPos(), target, 1);
    }
    CheckBallTargetRangeSpike_(target, GetTargetCourtIdx());

    CC_CMN::KOOPA::CcFireBall *pFire = new CC_CMN::KOOPA::CcFireBall(CGame::GetTempParent());
    if (pFire) {
        pFire->Create(this, pos, target, s_Param);
        m_hFireBall = pFire->GetId();
    }
}

float msw::CChrCoreVolleyKoopa::SubStateAirTrickGetBallAdsorbFrame_(STATE_WORK_AIR_TRICK *)
{
    return AnimGetResFrameNum(0x75) + AnimGetResFrameNum(0x76);
}

float msw::CChrCoreVolleyKoopa::SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return AnimGetResFrameNum(0x75) + AnimGetResFrameNum(0x76);
}

// The trick motion's length at the trick's speed.
float msw::CChrCoreVolleyKoopa::SubStateAirTrickGetTrickRotFrame_(STATE_WORK_AIR_TRICK *pWork)
{
    float fFrame = AnimGetResFrameNum(0x75);
    if (pWork->m_fAnimSpeed > 0.0f) {
        fFrame /= pWork->m_fAnimSpeed;
    }
    return fFrame;
}

void msw::CChrCoreVolleyKoopa::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x8E, 3);
}

void msw::CChrCoreVolleyKoopa::SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x90, 3);
}

void msw::CChrCoreVolleyKoopa::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);
    Eff_Stop(0x15);
    Eff_CreateAttachAuto(0x16, 0x91, 3);
}
