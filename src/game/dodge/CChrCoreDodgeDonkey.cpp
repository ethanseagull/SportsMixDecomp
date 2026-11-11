// msw::CChrCoreDodgeDonkey: Donkey Kong in the dodgeball game. His special
// shot is a jump in two parts: at the end of the first motion he pounds the
// ground, which shakes the camera and sends out a shock wave
// (CC07_DODGE::CcDonkeyQuakeDodge), then he jumps to the height of the shot.
//
// CC07_DODGE::CcDonkeyQuakeDodge: the shock wave. The base class
// (msw::CC_CMN::DONKEY::CcEarthQuake, in main.dol) does the work; this class
// only gives its two tuning values. (Top-level namespace, unlike the
// msw::CC07_HOCKEY class.)
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CC_PARAM_DODGE.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcEarthQuake.hpp"

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace CC07_DODGE {

    class CcDonkeyQuakeDodge : public msw::CC_CMN::DONKEY::CcEarthQuake {
        public:
            CcDonkeyQuakeDodge(bcs::CTask *pParent, msw::CChrCore *pOwner, const bcs::FVEC3 &rPos, float fScale)
                : CcEarthQuake(pParent, pOwner, rPos, fScale)
            {
            }
            virtual ~CcDonkeyQuakeDodge() {}

        protected:
            virtual float GetCollHitEffectFrm_() const;
            virtual float GetCollHitRadius_() const { return 50.0f; }
    };
}

namespace {

    // (guessed helper) The distance between two points in the XZ plane.
    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }
}

inline float CC07_DODGE::CcDonkeyQuakeDodge::GetCollHitEffectFrm_() const
{
    return 16.0f;
}

msw::CChrCoreDodgeDonkey::CChrCoreDodgeDonkey()
{
}

msw::CChrCoreDodgeDonkey::~CChrCoreDodgeDonkey()
{
}

void msw::CChrCoreDodgeDonkey::DtorStateAirTrick()
{
    CChrCoreDodge::DtorStateAirTrick();
}

// The trick jumps twice: a hop (0) that ends at least 5 high, then the shot
// jump (1) up to the height in the parameters.
void msw::CChrCoreDodgeDonkey::SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK_CMN *pWork, long nStep)
{
    if (nStep == 0) {
        float fHeight = 5.0f;
        float fPosY = GetPosY();
        if (fHeight < fPosY) {
            fHeight = fPosY;
        }
        Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, 0.0f, fHeight);
        m_fJumpUnk224 = 0.0f;
        m_fJumpUnk228 = 0.0f;
    } else {
        float fHeight = GetCcParam_()->m_Unk560;
        float fPosY = GetPosY();
        if (fHeight < fPosY) {
            fHeight = fPosY;
        }
        Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, fHeight, fHeight);
        m_fJumpUnk224 = pWork->m_fJumpUnk;
        m_fJumpUnk228 = 0.0f;
    }
}

// The hop stays where it is; the shot jump moves as usual.
void msw::CChrCoreDodgeDonkey::SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK_CMN *pWork, long nStep)
{
    if (nStep != 0) {
        CChrCoreDodge::SubStateArtsShotSetTrickMoveParam_(pWork, 0);
        return;
    }

    pWork->m_MovePos = GetPos();
    pWork->m_MoveParam.m_nType = 0xB;
    pWork->m_MoveParam.m_fDist = DistXZ(pWork->m_MovePos, GetPos());
    pWork->m_MoveParam.m_fFrame = 0.0f;
    pWork->m_MoveParam.m_fFrameMax = pWork->m_fTrickFrame;
    pWork->m_fUnkD4 = 0.0f;
}

// Not zero once the shot jump is over. The end of the hop (step 0) sends
// the shock wave and plays motion 0x7B; the end of that (step 1) starts the
// shot jump with motion 0x7C.
int msw::CChrCoreDodgeDonkey::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                SubStateArtsShotFireBullet_(pWork, 0);
                AnimPlay(3, 0x7B, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0x81, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x7C, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x15, 0x82, 3);
                pWork->m_fTrickFrame = AnimGetFrameNum(3);
                SubStateArtsShotSetTrickJumpParam_(pWork, 1);
                SubStateArtsShotSetTrickMoveParam_(pWork, 1);
                pWork->m_nTrickStep++;
            }
            break;
        case 2:
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

void msw::CChrCoreDodgeDonkey::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x80, 3);
}

void msw::CChrCoreDodgeDonkey::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x16, 0x83, 3);
}

void msw::CChrCoreDodgeDonkey::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_Stop(0x16);
    Eff_CreateAttachAuto(0x17, 0x84, 3);
}

void msw::CChrCoreDodgeDonkey::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK_CMN *, long)
{
    CGame::StartCameraQuake(3, -1, -1);
    new (0x100) CC07_DODGE::CcDonkeyQuakeDodge(CGame::GetTempParent(), this, GetPos(), 16.0f);
}
