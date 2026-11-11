// msw::CChrCoreHockeyPeach: Peach in the hockey game. Her special shot
// throws a fan of five heart bombs (CC_CMN::PEACH::CcHeartBomb) towards the
// goal her team attacks.
#include "game/msw/CChrCoreHockey.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcHeartBomb.hpp"

namespace {

    // (values' meanings not known) The heart bombs' tuning; m_afParam[0] is
    // drawn anew for each bomb.
    msw::CC_CMN::PEACH::CcHeartBomb::FIRE_PARAM s_HeartBombParam = {
        {0x59B, 0x59C, 0x594, 0x595, 0x596},
        {0.0f, 1.4f, 1.2f, 0.8f},
    };

    // (guessed helpers) Degrees to radians; a random value between two
    // bounds given either way round.
    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    inline float RandBetween(float fA, float fB)
    {
        if (fA < fB) {
            return bcs::BCS_RandRange(fA, fB);
        }
        if (fA > fB) {
            return bcs::BCS_RandRange(fB, fA);
        }
        return fA;
    }

    // (guessed) The original .rodata ends with 0.5, -1, 90, 100 and 6, which
    // nothing left in the file reads (most likely used by code the link
    // stripped). These dead comparisons, made from the last function, keep
    // them in place. See notes/decisions.md, "Unreferenced data kept by dead
    // references".
    inline void UnusedReferences(float f)
    {
        if (f == 0.5f) {
        }
        if (f == -1.0f) {
        }
        if (f == 90.0f) {
        }
        if (f == 100.0f) {
        }
        if (f == 6.0f) {
        }
    }
}

msw::CChrCoreHockeyPeach::CChrCoreHockeyPeach()
{
}

msw::CChrCoreHockeyPeach::~CChrCoreHockeyPeach()
{
}

void msw::CChrCoreHockeyPeach::Reset_()
{
    CChrCoreHockey::Reset_();
}

void msw::CChrCoreHockeyPeach::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x8C, 3);
    SE_PlayArtsJumpSE(0);
}

// When the jump motion ends, the throw (motion 0x9D) starts; the bombs fly
// at its shot frame (or its end), and the shot is ready when it is over.
int msw::CChrCoreHockeyPeach::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0x8D, 3);
                SubStateArtsShotDoGetBallShotTrg_(pWork, 60.0f);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                SubStateArtsShotFireBullet_(pWork, 0);
                pWork->m_nTrickStep++;
            }
            break;
        default:
            if (IsAnimEnd(3)) {
                bReady = 1;
            }
            break;
    }
    return bReady;
}

void msw::CChrCoreHockeyPeach::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x16, 0x8E, 3);
}

// The first bomb flies from node 2 straight at the goal (or ahead, when the
// goal is right above her); the other four fan out in pairs, one to each
// side, each a random angle of 30 degrees up to 45 / 2 degrees further out
// than the last.
void msw::CChrCoreHockeyPeach::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    bcs::FVEC3 pos(GetPos());
    GetVNodePos(2, pos);

    bcs::FVEC3 target(0.0f, 0.0f, 0.0f);
    GetTargetGoalPos(target);

    bcs::FVEC3 dir;
    dir.x = target.x - pos.x;
    dir.z = target.z - pos.z;
    if (dir.x * dir.x + dir.z * dir.z > 0.0f) {
        bcs::FVEC3 flat(dir.x, 0.0f, dir.z);
        bcs::BCS_Vec_Normalize(dir, flat);
    } else {
        bcs::BCS_VecOneZ_RotY(dir, GetRotY());
    }

    CC_CMN::PEACH::CcHeartBomb::FIRE_PARAM param = s_HeartBombParam;
    float fParam = 8.0f;
    param.m_afParam[0] = RandBetween(fParam, fParam);
    new CC_CMN::PEACH::CcHeartBomb(CGame::GetTempParent(), this, pos, dir, param);

    long nNum = 5;
    long nRest = nNum - 1;
    if (nRest > 0) {
        long nPair = nNum / 2;
        float fRotL = 0.0f;
        float fRotR = 0.0f;
        float fFan = 45.0f;
        float fStep = DegToRad(fFan) / nPair;
        float fStepMin = 30.0f;
        float fMin = DegToRad(fStepMin);
        if (fMin > fStep) {
            fStep = fMin;
        }

        bcs::FVEC3 bombDir;
        for (long i = 0; i < nPair; i++) {
            param.m_afParam[0] = RandBetween(fParam, fParam);
            fRotL += fStep > fMin ? bcs::BCS_RandRange(fMin, fStep) : fStep;
            bcs::BCS_Vec_RotY(bombDir, dir, fRotL);
            new CC_CMN::PEACH::CcHeartBomb(CGame::GetTempParent(), this, pos, bombDir, param);
            if (--nRest <= 0) {
                break;
            }

            param.m_afParam[0] = RandBetween(fParam, fParam);
            fRotR -= fStep > fMin ? bcs::BCS_RandRange(fMin, fStep) : fStep;
            bcs::BCS_Vec_RotY(bombDir, dir, fRotR);
            new CC_CMN::PEACH::CcHeartBomb(CGame::GetTempParent(), this, pos, bombDir, param);
            if (--nRest <= 0) {
                break;
            }
        }
    }
    UnusedReferences(fParam);
}
