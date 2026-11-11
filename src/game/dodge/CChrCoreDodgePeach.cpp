// msw::CChrCoreDodgePeach: Peach in the dodgeball game. Her special shot
// throws a fan of seven heart bombs (CC_CMN::PEACH::CcHeartBomb) towards
// the point her trick aims at. Written from BasketF's Peach.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcHeartBomb.hpp"

namespace {

    // (values' meanings not known) The heart bombs' tuning; m_afParam[0] is
    // drawn anew for each bomb.
    msw::CC_CMN::PEACH::CcHeartBomb::FIRE_PARAM s_HeartBombParam = {
        {0x52A, 0x52B, 0x529, 0x522, 0x523},
        {0.0f, 1.2f, 1.2f, 0.6f},
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

    // Stand-in for an unknown user of -1 and 1, which the original keeps at
    // the end of .rodata but nothing reads (likely stripped code). See
    // notes/decisions.md, "Unreferenced data kept by dead references".
    inline void UnusedReferences(float f)
    {
        if (f == -1.0f) {
        }
        if (f == 1.0f) {
        }
    }
}

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

msw::CChrCoreDodgePeach::CChrCoreDodgePeach()
{
}

msw::CChrCoreDodgePeach::~CChrCoreDodgePeach()
{
}

// Step 0 jumps until frame 63 of the trick motion (or its end), then
// throws the bombs; ready once the motion is over. The loop motion starts
// the first time a motion ends (m_nArtsLevel serves as its flag here).
int msw::CChrCoreDodgePeach::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (!IsAnimEnd(3) && !(AnimGetFrame(3) >= 63.0f)) {
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

    if (pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nArtsLevel = 1;
    }
    return bReady;
}

void msw::CChrCoreDodgePeach::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x6B, 3);
}

void msw::CChrCoreDodgePeach::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);
}

void msw::CChrCoreDodgePeach::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x6C, 3);
}

// The first bomb flies from node 2 straight at the target marker's point
// (or ahead, when it is right above her); the other six fan out in pairs,
// one to each side, each a step further out than the last: a random angle
// between 15 degrees and 45 degrees over the pairs, or 15 degrees when that
// is less.
void msw::CChrCoreDodgePeach::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK_CMN *, long)
{
    bcs::FVEC3 pos(GetPos());
    GetVNodePos(2, pos);

    bcs::FVEC3 target(0.0f, 0.0f, 0.0f);
    GetDodgeArtsTargetMarkerPos(target, 0);

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
    float fParamMin = 3.0f;
    float fParamMax = 7.0f;
    param.m_afParam[0] = RandBetween(fParamMin, fParamMax);
    new CC_CMN::PEACH::CcHeartBomb(CGame::GetTempParent(), this, pos, dir, param);

    long nNum = 7;
    long nRest = nNum - 1;
    if (nRest > 0) {
        long nPair = nNum / 2;
        float fRotL = 0.0f;
        float fRotR = 0.0f;
        float fFan = 45.0f;
        float fStep = DegToRad(fFan) / nPair;
        float fStepMin = 15.0f;
        float fMin = DegToRad(fStepMin);
        if (fMin > fStep) {
            fStep = fMin;
        }

        bcs::FVEC3 bombDir;
        for (long i = 0; i < nPair; i++) {
            param.m_afParam[0] = RandBetween(fParamMin, fParamMax);
            if (fStep > fMin) {
                fRotL += bcs::BCS_RandRange(fMin, fStep);
            } else {
                fRotL += fStep;
            }
            bcs::BCS_Vec_RotY(bombDir, dir, fRotL);
            new CC_CMN::PEACH::CcHeartBomb(CGame::GetTempParent(), this, pos, bombDir, param);
            if (--nRest <= 0) {
                break;
            }

            param.m_afParam[0] = RandBetween(fParamMin, fParamMax);
            if (fStep > fMin) {
                fRotR -= bcs::BCS_RandRange(fMin, fStep);
            } else {
                fRotR -= fStep;
            }
            bcs::BCS_Vec_RotY(bombDir, dir, fRotR);
            new CC_CMN::PEACH::CcHeartBomb(CGame::GetTempParent(), this, pos, bombDir, param);
            if (--nRest <= 0) {
                break;
            }
        }
    }
    UnusedReferences(fParamMin);
}
