// msw::CChrCoreHockeyKoopa: Bowser in the hockey game. His special shot is a
// run of motions; in the third he breathes a fireball
// (CC_CMN::KOOPA::CcFireBall) at the goal his team attacks.
#include "game/msw/CChrCoreHockey.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcFireBall.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/g3d/ResNode.hpp"

namespace msw {
    namespace CC10_HOCKEY {
        // The model node the fireball comes out of (his mouth).
        const char *_pszFireBallLauncherNodeName = "eff_kuchi";
    }
}

msw::CChrCoreHockeyKoopa::CChrCoreHockeyKoopa()
{
}

msw::CChrCoreHockeyKoopa::~CChrCoreHockeyKoopa()
{
}

void msw::CChrCoreHockeyKoopa::Reset_()
{
    CChrCoreHockey::Reset_();
}

// The trick is a run of motions, each started when the one before has
// ended: 0x9D, 0x9E, then the fireball (ending the breath sound) with 0x9F,
// then 0xA0; the shot is ready when the last is over.
int msw::CChrCoreHockeyKoopa::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xAB, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9E, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xAC, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 2:
            if (IsAnimEnd(3)) {
                SE_Stop(8, 8.0f);
                SubStateArtsShotFireBullet_(pWork, 0);
                AnimPlay(3, 0x9F, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xAD, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 3:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0xA0, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xAE, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 4:
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

void msw::CChrCoreHockeyKoopa::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0xAA, 3);
    SE_PlayMove(8, 0x44F, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
}

void msw::CChrCoreHockeyKoopa::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x15, 0xAF, 3);
}

void msw::CChrCoreHockeyKoopa::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_Stop(0x15);
    Eff_CreateAttachAuto(0x16, 0xB0, 3);
}

// The fireball leaves his mouth (or his feet, without a model) for a point
// 2.7 short of the goal his team attacks, on the ice.
void msw::CChrCoreHockeyKoopa::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    // (values' meanings not known)
    static CC_CMN::KOOPA::CcFireBall::FIRE_PARAM s_Param = {
        {0x5ED, 0x5EE, 0x5F2},
        {22.0f, 1.0f, 0.6f, 0.7f, 0.8f, 4.0f, 16.0f, 15.0f},
    };

    bcs::CMdlObj *pMdl = m_pMdl;
    unsigned long nNode = m_nEffNodeBlock;
    if (pMdl) {
        nw4r::g3d::ResNode node = pMdl->GetResMdl().GetResNode(CC10_HOCKEY::_pszFireBallLauncherNodeName);
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

    bcs::FVEC3 target(GetTargetGoalPos());
    bcs::FVEC3 ofs(target.x - pos.x, 0.0f, target.z - pos.z);
    bcs::FVEC3 dir(ofs);
    bcs::BCS_Vec_Normalize(ofs, dir);
    bcs::FVEC3Scale(&ofs, &ofs, -2.7f);
    target.y = 0.0f;
    target.x += ofs.x;
    target.z += ofs.z;

    CC_CMN::KOOPA::CcFireBall *pFire = new CC_CMN::KOOPA::CcFireBall(CGame::GetTempParent());
    if (pFire) {
        pFire->Create(this, pos, target, s_Param);
    }
}
