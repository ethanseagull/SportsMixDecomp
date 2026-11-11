// msw::CChrCoreVolleyDonkey: Donkey Kong in the volleyball game. His air
// trick jumps up and pounds the ground at the top, which shakes the camera
// and sends out a shock wave (CC07_VOLLEY::CcDonkeyQuake), then a second
// jump carries the shot.
//
// msw::CC07_VOLLEY::CcDonkeyQuake: the shock wave. The base class
// (msw::CC_CMN::DONKEY::CcEarthQuake, in main.dol) does the work; this class
// only gives its two tuning values.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/New.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcEarthQuake.hpp"

namespace msw {
    namespace CC07_VOLLEY {

        class CcDonkeyQuake : public CC_CMN::DONKEY::CcEarthQuake {
            public:
                CcDonkeyQuake(bcs::CTask *pParent, CChrCore *pOwner, const bcs::FVEC3 &rPos, float fScale)
                    : CcEarthQuake(pParent, pOwner, rPos, fScale)
                {
                }
                virtual ~CcDonkeyQuake() {}

            protected:
                virtual float GetCollHitEffectFrm_() const { return 16.0f; }
                virtual float GetCollHitRadius_() const { return 50.0f; }
        };
    }
}

namespace {

    // (guessed helper) The smaller of two values.
    inline float Min(float fValue, float fMax)
    {
        return fValue > fMax ? fMax : fValue;
    }

}

msw::CChrCoreVolleyDonkey::CChrCoreVolleyDonkey()
{
}

msw::CChrCoreVolleyDonkey::~CChrCoreVolleyDonkey()
{
}

// Step 0 is a small hop at least 3 high; the second jump rises to the
// height in the parameters (or stays level when he is higher already).
void msw::CChrCoreVolleyDonkey::SubStateAirTrickSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    if (nStep == 0) {
        float fPosY = GetPosY();
        float fTop = 1.5f + fPosY;
        Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, 0.0f, fTop < 3.0f ? 3.0f : fTop);
        m_fJumpUnk224 = 0.0f;
        m_fJumpUnk228 = 0.0f;
    } else {
        float fHeight = GetCcParam_()->m_Unk37C;
        float fPosY = GetPosY();
        if (fHeight < fPosY) {
            fHeight = fPosY;
        }
        Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, fHeight, fHeight);
        m_fJumpUnk224 = pWork->m_fJumpUnk;
        m_fJumpUnk228 = 0.0f;
    }
}

// He jumps straight up both times.
void msw::CChrCoreVolleyDonkey::SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    if (nStep == 0) {
        ClearMoveParamWork_(pWork->m_MoveParam);
        Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
        pWork->m_fUnk20 = 0.0f;
    } else {
        ClearMoveParamWork_(pWork->m_MoveParam);
        Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
        pWork->m_fUnk20 = 0.0f;
    }
}

float msw::CChrCoreVolleyDonkey::SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long nStep)
{
    if (nStep == 0) {
        return AnimGetResFrameNum(0x75);
    }
    return AnimGetResFrameNum(0x77);
}

float msw::CChrCoreVolleyDonkey::SubStateAirTrickGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long nStep)
{
    if (nStep == 0) {
        return AnimGetResFrameNum(0x75);
    }
    return GetCcParam_()->m_Unk380;
}

int msw::CChrCoreVolleyDonkey::SubStateAirTrickIsTMStartAuto_(STATE_WORK_AIR_TRICK *)
{
    return 0;
}

// Steps 0 and 1 are the hop (hand effects from frame 50 of the motion); at
// its end he pounds the ground. Step 2 waits for frame 5 of the pound to
// stop the hand effects, step 3 for its end to start the second jump.
// After that the shot is ready when the jump's frames are done.
int msw::CChrCoreVolleyDonkey::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    switch (pWork->m_nTrickStep) {
        case 0:
        case 1:
            pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
            pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);
            SubStateAirTrickDoJumpMove_(pWork);
            if (pWork->m_nTrickStep == 0 && (IsAnimEnd(3) || AnimGetFrame(3) >= 50.0f)) {
                Eff_CreateAttach(0x15, 0x5A6, m_nEffNodeHand0, 0, 0, 3, 0);
                Eff_CreateAttach(0x16, 0x5A6, m_nEffNodeHand1, 0, 0, 3, 0);
                pWork->m_nTrickStep++;
            }
            if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
                CGame::StartCameraQuake(3, -1, -1);
                new (0x100) CC07_VOLLEY::CcDonkeyQuake(CGame::GetTempParent(), this, GetPos(), 16.0f);
                Eff_Stop(0x16);
                AnimPlay(3, 0x76, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x17, 0x80, 3);
                AnimSetSpeed(3, pWork->m_fAnimSpeed);
                pWork->m_nTrickStep = 2;
            }
            break;
        case 2:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 5.0f) {
                Eff_Stop(0x15);
                Eff_Stop(0x16);
                pWork->m_nTrickStep++;
            }
            // fall through
        case 3:
            if (IsAnimEnd(3)) {
                SubStateAirTrickSetTrickFrameParam_(pWork, 1);
                AnimPlay(3, 0x77, 0, 6.0f, 0.0f);
                AnimSetSpeed(3, pWork->m_fAnimSpeed);
                Eff_CreateAttachAuto(0x18, 0x81, 3);
                SubStateAirTrickSetTrickJumpParam_(pWork, 1);
                SubStateAirTrickSetTrickMoveParam_(pWork, 1);
                SubStateAirTrickStartTMDisp_(pWork);
                pWork->m_nTrickStep = 4;
            }
            break;
        default:
            pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
            pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);
            SubStateAirTrickDoJumpMove_(pWork);
            if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
                bReady = 1;
            }
            break;
    }
    SubStateAirTrickDoBallAdsorb_(pWork);
    return bReady;
}

void msw::CChrCoreVolleyDonkey::SubStateAirTrickShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickShootBall_(pWork);
}

void msw::CChrCoreVolleyDonkey::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x7F, 3);
}

void msw::CChrCoreVolleyDonkey::SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x19, 0x82, 3);
}

void msw::CChrCoreVolleyDonkey::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    Eff_Stop(0x19);
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x1A, 0x83, 3);
}

void msw::CChrCoreVolleyDonkey::SubStateAirTrickPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLandMotion_(pWork);
}
