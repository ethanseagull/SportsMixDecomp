// msw::CChrCoreHockeyDonkey: Donkey Kong in the hockey game. His special
// shot is a high jump; on the way he pounds the ice, which shakes the camera
// and sends out a shock wave (CC07_HOCKEY::CcDonkeyQuakeHockey).
//
// msw::CC07_HOCKEY::CcDonkeyQuakeHockey: the shock wave. The base class
// (msw::CC_CMN::DONKEY::CcEarthQuake, in main.dol) does the work; this class
// only gives its two tuning values.
#include "game/msw/CChrCoreHockey.hpp"

#include "bcs/New.hpp"
#include "game/msw/CC_PARAM_HOCKEY.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcEarthQuake.hpp"

namespace msw {
    namespace CC07_HOCKEY {

        class CcDonkeyQuakeHockey : public CC_CMN::DONKEY::CcEarthQuake {
            public:
                CcDonkeyQuakeHockey(bcs::CTask *pParent, CChrCore *pOwner, const bcs::FVEC3 &rPos, float fScale)
                    : CcEarthQuake(pParent, pOwner, rPos, fScale)
                {
                }
                virtual ~CcDonkeyQuakeHockey() {}

            protected:
                virtual float GetCollHitEffectFrm_() const;
                virtual float GetCollHitRadius_() const { return 5.2f; }
        };
    }
}

namespace {

    // (guessed helper) The larger of two values.
    inline float Max(float fValue, float fMin)
    {
        return fMin < fValue ? fValue : fMin;
    }
}

// The original .rodata ends with 0.5, -1, 90, 100 and 6 after the shock
// wave's 5, which nothing left in the file reads (the same five as in
// CChrCoreHockeyMoogle.cpp; most likely from a stripped header function).
// They are left out here: the dead comparisons that keep them (notes/
// decisions.md, "Unreferenced data kept by dead references") can only go in
// GetCollHitEffectFrm_, the last function to read a constant, and there they
// make the compiler load its 5 through a base register. So this unit's
// .rodata is 0x14 bytes short.
inline float msw::CC07_HOCKEY::CcDonkeyQuakeHockey::GetCollHitEffectFrm_() const
{
    return 5.0f;
}

msw::CChrCoreHockeyDonkey::CChrCoreHockeyDonkey()
{
}

msw::CChrCoreHockeyDonkey::~CChrCoreHockeyDonkey()
{
}

void msw::CChrCoreHockeyDonkey::Reset_()
{
    CChrCoreHockey::Reset_();
}

// The jump rises from where he stands to the height in the parameters (or
// stays level when he is higher already) over 60 frames. No jump at all
// (kind 0) when that height is not above the ice.
void msw::CChrCoreHockeyDonkey::SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    float fHeight = GetCcParam_()->m_Unk1BC;
    GetCcParam_(); // (the original reads the parameters a second time and uses nothing)
    float fPosY = GetPosY();
    if (fHeight < fPosY) {
        fHeight = fPosY;
    }

    float fFrame = 60.0f;
    fFrame = Max(fFrame, 1.0f);
    if (fHeight > 0.0f) {
        Util_SetJumpParam(fFrame, fPosY, fPosY, fHeight);
        m_fJumpUnk224 = 0.0f;
        m_fJumpUnk228 = 25.0f;
        pWork->m_nJumpType = 2;
    } else {
        pWork->m_nJumpType = 0;
    }
}

// The jump sound plays once he starts to fall; then come motion 0x9D, the
// shock wave at frame 65 of it (or its end) and motion 0x9E, and the shot
// is ready when that is over.
int msw::CChrCoreHockeyDonkey::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (m_fJumpUnk228 <= 0.0f) {
                SE_PlayArtsJumpSE(0);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x13, 0x9F, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 2:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 65.0f) {
                SubStateArtsShotFireBullet_(pWork, 0);
                pWork->m_nTrickStep++;
            }
            break;
        case 3:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9E, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xA0, 3);
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

void msw::CChrCoreHockeyDonkey::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);
}

void msw::CChrCoreHockeyDonkey::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x15, 0xA1, 3);
}

void msw::CChrCoreHockeyDonkey::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    Eff_Stop(0x15);
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x16, 0xA2, 3);
}

void msw::CChrCoreHockeyDonkey::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    CGame::StartCameraQuake(3, -1, -1);
    new (0x100) CC07_HOCKEY::CcDonkeyQuakeHockey(CGame::GetTempParent(), this, GetPos(), 16.0f);
}
