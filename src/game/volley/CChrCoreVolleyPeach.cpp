// msw::CChrCoreVolleyPeach: Peach in the volleyball game. Her air trick
// throws a fan of five heart bombs (CC_CMN::PEACH::CcHeartBomb) towards the
// target marker.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcHeartBomb.hpp"

namespace {

    // (values' meanings not known) The heart bombs' tuning; m_afParam[0] is
    // drawn anew for each bomb.
    msw::CC_CMN::PEACH::CcHeartBomb::FIRE_PARAM s_HeartBombParam = {
        {0x575, 0x576, 0x574, 0x56E, 0x56F},
        {0.0f, 1.0f, 1.2f, 0.5f},
    };

    // (guessed helpers) The distance between two points in the XZ plane;
    // degrees to radians; a random value between two bounds given either
    // way round.
    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }

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

    // (guessed) The original .rodata ends with a -1.0f that nothing left in
    // the file reads, most likely used by code the link stripped. This dead
    // comparison keeps it in place. See notes/decisions.md, "Unreferenced
    // data kept by dead references".
    inline void UnusedReferences(float f)
    {
        if (f == -1.0f) {
        }
    }
}

msw::CChrCoreVolleyPeach::CChrCoreVolleyPeach()
{
}

msw::CChrCoreVolleyPeach::~CChrCoreVolleyPeach()
{
}

void msw::CChrCoreVolleyPeach::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
    Eff_CreateAttach(0x13, 0x65F, m_nEffNode, 0, 1, 3, 0);
}

// The trick goes to the middle of the attack zone of the court the
// character defends.
void msw::CChrCoreVolleyPeach::SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    bcs::FVEC4 rect;
    Util_GetGuardCourtAttackZoneRect_(rect);
    pWork->m_TargetPos.Set(0.5f * (rect.x + rect.y), 0.0f, 0.5f * (rect.z + rect.w));
    pWork->m_MoveParam.m_nType = 0xB;

    pWork->m_MoveParam.m_fDist = DistXZ(pWork->m_TargetPos, GetPos());
    pWork->m_MoveParam.m_fFrame = 0.0f;
    pWork->m_MoveParam.m_fFrameMax = pWork->m_fTrickFrame;
}

// The turn takes the trick's length, at most 63 frames.
float msw::CChrCoreVolleyPeach::SubStateAirTrickGetTrickRotFrame_(STATE_WORK_AIR_TRICK *pWork)
{
    return 63.0f > pWork->m_fTrickFrame ? pWork->m_fTrickFrame : 63.0f;
}

// Step 0 throws the bombs at frame 63 of the trick motion (or when it
// ends), step 1 waits for the motion to end; then the shot is ready. The
// loop motion starts (once, m_nArtsLevel set to 1) when a motion ends.
int msw::CChrCoreVolleyPeach::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateAirTrickDoJumpMove_(pWork);
    SubStateAirTrickDoBallAdsorb_(pWork);

    switch (pWork->m_nTrickStep) {
        case 0:
            if (!IsAnimEnd(3) && !(AnimGetFrame(3) >= 63.0f)) {
                break;
            }
            SubStateAirTrickFireHeartBomb_();
            pWork->m_nTrickStep++;
            // fall through
        case 1:
            if (IsAnimEnd(3)) {
                bReady = 1;
                pWork->m_nTrickStep++;
            }
            break;
        default:
            bReady = 1;
            break;
    }

    if (pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
        SubStateAirTrickPlayLoopMotion_(pWork);
        pWork->m_nArtsLevel = 1;
    }
    return bReady;
}

void msw::CChrCoreVolleyPeach::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x6B, 3);
}

void msw::CChrCoreVolleyPeach::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x14, 0x6C, 3);
}

// The first bomb flies from node 2 straight at the target marker (or ahead,
// when the marker is right above her); the other four fan out in pairs,
// one to each side, each a random angle of 13 degrees up to 45 / 2 degrees
// further out than the last.
void msw::CChrCoreVolleyPeach::SubStateAirTrickFireHeartBomb_()
{
    bcs::FVEC3 pos(GetPos());
    GetVNodePos(2, pos);

    bcs::FVEC3 target(0.0f, 0.0f, 0.0f);
    GetVolleyTargetMarkerPos(target, 0);

    bcs::FVEC3 dir;
    bcs::FVEC3 bombDir;
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
    float fParamMax = 8.0f;
    param.m_afParam[0] = RandBetween(fParamMin, fParamMax);
    new CC_CMN::PEACH::CcHeartBomb(CGame::GetTempParent(), this, pos, dir, param);

    long nNum = 5;
    long nRest = nNum - 1;
    if (nRest > 0) {
        long nPair = nNum / 2;
        float fRotL = 0.0f;
        float fRotR = 0.0f;
        float fBombParamMin = 3.0f;
        float fBombParamMax = 8.0f;
        float fFan = 45.0f;
        float fStep = DegToRad(fFan) / nPair;
        float fStepMin = 13.0f;
        float fMin = DegToRad(fStepMin);
        if (fMin > fStep) {
            fStep = fMin;
        }

        for (long i = 0; i < nPair; i++) {
            param.m_afParam[0] = RandBetween(fBombParamMin, fBombParamMax);
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

            param.m_afParam[0] = RandBetween(fBombParamMin, fBombParamMax);
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

        PlaySePos(-1, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    }
    UnusedReferences(param.m_afParam[0]);
}
