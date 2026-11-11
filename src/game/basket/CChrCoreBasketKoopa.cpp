// msw::CChrCoreBasketKoopa: Bowser in the basketball game. His lay-up has
// an attack collision; his special shot breathes a fireball
// (CC_CMN::KOOPA::CcFireBall) from his mouth at the goal his team attacks.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FMTX34.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcFireBall.hpp"

namespace msw {
    namespace CC10_BASKET {
        // The node of Bowser's model the fireball leaves from.
        const char *_pszFireBallLauncherNodeName = "eff_kuchi";
    }
}

// The fireball of the special shot (values as in the original; their
// meaning is not known).
static msw::CC_CMN::KOOPA::CcFireBall::FIRE_PARAM s_FireParam = {
    {0x579, 0x57A, 0x57D},
    {22.0f, 1.0f, 0.6f, 0.7f, 0.8f, 3.8f, 30.0f, 15.0f},
};

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

msw::CChrCoreBasketKoopa::CChrCoreBasketKoopa()
{
    m_nUnk5EC = 0;
}

msw::CChrCoreBasketKoopa::~CChrCoreBasketKoopa()
{
}

// The lay-up (state 0x25) turns its attack collision on at the trigger.
void msw::CChrCoreBasketKoopa::CallbackMotTrg_AtkOn_(long, long)
{
    if (GetState() == 0x25) {
        SetCCLayUpAtkCol();
    }
}

void msw::CChrCoreBasketKoopa::CallbackMotTrg_AtkOff_(long, long)
{
    CollAtkGetColl_(0)->GetCollUnit()->OffFlag10(1);
}

float msw::CChrCoreBasketKoopa::SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return AnimGetResFrameNum(0x7E) + AnimGetResFrameNum(0x7F);
}

// Step 0 jumps until the trick motion ends, then plays the breath motion
// and fires; ready once that motion is over. The loop motion starts the
// first time a motion ends.
int msw::CChrCoreBasketKoopa::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (!IsAnimEnd(3)) {
                break;
            }
            AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)1, (m_nStatusFlag >> 2) & 1), 0, 0.0f, 0.0f);
            Eff_CreateAttachAuto(0x14, 0x95, 3);
            SubStateArtsShotFireBullet_(pWork, 0);
            pWork->m_nTrickStep++;
            // fall through
        case 1:
            if (!IsAnimEnd(3)) {
                break;
            }
            pWork->m_nTrickStep++;
            bReady = 1;
            break;
        default:
            bReady = 1;
            break;
    }

    if (pWork->m_nUnkA0 < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nUnkA0 = 1;
    }
    return bReady;
}

void msw::CChrCoreBasketKoopa::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x94, 3);
}

void msw::CChrCoreBasketKoopa::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x96, 3);
}

void msw::CChrCoreBasketKoopa::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_Stop(0x15);
    Eff_CreateAttachAuto(0x16, 0x97, 3);
}

// The fireball leaves from Bowser's mouth node (his position when he has
// no model) for the goal his team attacks, aimed one unit short of it on
// the ground.
void msw::CChrCoreBasketKoopa::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    bcs::CMdlObj *pMdl = m_pMdl;
    // (The original reads the node signed and cuts it back to 16 bits, so
    // the field was most likely signed in its source; HockeyF's Bowser
    // does the same.)
    unsigned long nNode = (unsigned short)(short)m_nEffNodeBlock;
    if (pMdl) {
        nw4r::g3d::ResNode node = pMdl->GetResMdl().GetResNode(CC10_BASKET::_pszFireBallLauncherNodeName);
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

    bcs::FVEC3 target(GetTargetGoalPos());
    bcs::FVEC3 dir;
    dir.x = target.x - pos.x;
    dir.z = target.z - pos.z;
    bcs::BCS_Vec_Normalize(dir, bcs::FVEC3(dir.x, 0.0f, dir.z));
    bcs::FVEC3Scale(&dir, &dir, -1.0f);
    target.x += dir.x;
    target.y = 0.0f;
    target.z += dir.z;

    CC_CMN::KOOPA::CcFireBall *pFire = new CC_CMN::KOOPA::CcFireBall(CGame::GetTempParent());
    if (pFire) {
        pFire->Create(this, pos, target, s_FireParam);
    }
}
