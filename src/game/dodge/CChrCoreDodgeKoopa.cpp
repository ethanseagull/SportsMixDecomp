// msw::CChrCoreDodgeKoopa: Bowser in the dodgeball game. His special shot
// breathes a fireball (CC_CMN::KOOPA::CcFireBall) from his mouth at the
// point his target marker shows.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FMTX34.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcFireBall.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/g3d/ResNode.hpp"

namespace CC10_DODGE {
    // The node of Bowser's model the fireball leaves from.
    const char *_pszFireBallLauncherNodeName = "eff_kuchi";
}

// The fireball of the special shot (values as in the original; their
// meaning is not known).
static msw::CC_CMN::KOOPA::CcFireBall::FIRE_PARAM s_FireParam = {
    {0x58B, 0x58C, 0x591},
    {23.0f, 1.4f, 0.6f, 0.5f, 0.8f, 3.0f, 16.0f, 15.0f},
};

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

msw::CChrCoreDodgeKoopa::CChrCoreDodgeKoopa()
{
}

msw::CChrCoreDodgeKoopa::~CChrCoreDodgeKoopa()
{
}

void msw::CChrCoreDodgeKoopa::DtorStateAirTrick()
{
    CChrCoreDodge::DtorStateAirTrick();
}

// Step 0 jumps until the trick motion ends, then plays the breath motion
// at the trick's speed and fires; ready once that motion is over.
int msw::CChrCoreDodgeKoopa::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x7B, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0x91, 3);
                AnimSetSpeed(3, pWork->m_fAnimSpeed);
                SubStateArtsShotFireBullet_(pWork, 0);
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

// The fireball leaves from Bowser's mouth node (his position when he has
// no model) for the point of his target marker, on the ground.
void msw::CChrCoreDodgeKoopa::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK_CMN *, long)
{
    bcs::CMdlObj *pMdl = m_pMdl;
    unsigned long nNode = m_nEffNodeBlock;
    if (pMdl) {
        nw4r::g3d::ResNode node = pMdl->GetResMdl().GetResNode(CC10_DODGE::_pszFireBallLauncherNodeName);
        if (node.IsValid()) {
            nNode = node.GetID();
        }
    }

    bcs::FVEC3 pos(GetPos());
    if (pMdl) {
        bcs::FMTX34 mtx;
        pMdl->GetScnMtxPos(&mtx, nNode, (bcs::CScnObj::ScnObjMtxType)1);
        pos.x = mtx.m[0][3];
        pos.y = mtx.m[1][3];
        pos.z = mtx.m[2][3];
    }

    bcs::FVEC3 target;
    GetDodgeArtsTargetMarkerPos(target, 0);
    target.y = 0.0f;

    CC_CMN::KOOPA::CcFireBall *pFire = new CC_CMN::KOOPA::CcFireBall(CGame::GetTempParent());
    if (pFire) {
        pFire->Create(this, pos, target, s_FireParam);
    }
}

float msw::CChrCoreDodgeKoopa::SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK_CMN *, long)
{
    return AnimGetResFrameNum(0x7A) + AnimGetResFrameNum(0x7B);
}

void msw::CChrCoreDodgeKoopa::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x90, 3);
}

void msw::CChrCoreDodgeKoopa::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x92, 3);
}

void msw::CChrCoreDodgeKoopa::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_Stop(0x15);
    Eff_CreateAttachAuto(0x16, 0x93, 3);
}
