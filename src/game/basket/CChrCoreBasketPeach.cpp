// msw::CChrCoreBasketPeach: Peach in the basketball game. Her special shot
// throws a fan of five heart bombs (CC_CMN::PEACH::CcHeartBomb) towards the
// goal her team attacks. Written from HockeyF's Peach.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/New.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcHeartBomb.hpp"

namespace {

    // (values' meanings not known) The heart bombs' tuning; m_afParam[0] is
    // drawn anew for each bomb.
    msw::CC_CMN::PEACH::CcHeartBomb::FIRE_PARAM s_HeartBombParam = {
        {0x52B, 0x52C, 0x523, 0x524, 0x525},
        {0.0f, 1.3f, 1.2f, 0.8f},
    };

    // (guessed helpers) Degrees to radians; a random value between two
    // bounds given either way round.
    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    inline float RandBetween(float fA, float fB)
    {
        float f = fA;
        if (fA < fB) {
            f = bcs::BCS_RandRange(fA, fB);
        } else if (fA > fB) {
            f = bcs::BCS_RandRange(fB, fA);
        }
        return f;
    }

    // Stand-in for an unknown user of 0.5 and -1, which the original keeps
    // at the end of .rodata but nothing reads (likely stripped code). See
    // notes/decisions.md, "Unreferenced data kept by dead references". (In
    // the original they come after FireBullet's int-to-float constant, so
    // their user was compiled after it; here they come before it.)
    inline void UnusedReferences(float f)
    {
        if (f == 0.5f) {
        }
        if (f == -1.0f) {
        }
    }
}

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

msw::CChrCoreBasketPeach::CChrCoreBasketPeach()
{
}

msw::CChrCoreBasketPeach::~CChrCoreBasketPeach()
{
}

// Step 0 jumps until frame 50 of the trick motion (or its end), then
// throws the bombs; ready once the motion is over. The loop motion starts
// the first time a motion ends.
int msw::CChrCoreBasketPeach::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (!IsAnimEnd(3) && !(AnimGetFrame(3) >= 50.0f)) {
                break;
            }
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

void msw::CChrCoreBasketPeach::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x71, 3);
}

void msw::CChrCoreBasketPeach::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x14, 0x72, 3);
}

// The first bomb flies from node 2 straight at the goal (or ahead, when the
// goal is right above her); the other four fan out in pairs, one to each
// side, each a step further out than the last: a random angle between 30
// degrees and 45 degrees over the pairs, or 30 degrees when that is less (as
// it is with two pairs). A sound (id -1) ends the throw.
void msw::CChrCoreBasketPeach::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
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
    new (0x100) CC_CMN::PEACH::CcHeartBomb(CGame::GetTempParent(), this, pos, dir, param);

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
            if (fStep > fMin) {
                fRotL += bcs::BCS_RandRange(fMin, fStep);
            } else {
                fRotL += fStep;
            }
            bcs::BCS_Vec_RotY(bombDir, dir, fRotL);
            new (0x100) CC_CMN::PEACH::CcHeartBomb(CGame::GetTempParent(), this, pos, bombDir, param);
            if (--nRest <= 0) {
                break;
            }

            param.m_afParam[0] = RandBetween(fParam, fParam);
            if (fStep > fMin) {
                fRotR -= bcs::BCS_RandRange(fMin, fStep);
            } else {
                fRotR -= fStep;
            }
            bcs::BCS_Vec_RotY(bombDir, dir, fRotR);
            new (0x100) CC_CMN::PEACH::CcHeartBomb(CGame::GetTempParent(), this, pos, bombDir, param);
            if (--nRest <= 0) {
                break;
            }
        }
        PlaySePos(-1, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    }
    UnusedReferences(fParam);
}

