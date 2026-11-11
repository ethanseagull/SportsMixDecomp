// msw::CChrCoreHockeySaboten: the cactus (Saboten) in the hockey game. His
// special shot is a run of motions; in the middle one he fires a needle
// (CC13_HOCKEY::CcSabotenNeedleHockey).
//
// msw::CC13_HOCKEY::CcSabotenNeedleHockey: the needle. The base class
// (msw::CC_CMN::SABOTEN::CcNeedle, in main.dol) does the work; this class
// only gives its three tuning values.
#include "game/msw/CChrCoreHockey.hpp"

#include "bcs/New.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcNeedle.hpp"

namespace msw {
    namespace CC13_HOCKEY {

        class CcSabotenNeedleHockey : public CC_CMN::SABOTEN::CcNeedle {
            public:
                CcSabotenNeedleHockey(bcs::CTask *pParent, CChrCore *pOwner, const bcs::FVEC3 &rPos, float fSpeed)
                    : CcNeedle(pParent, pOwner, rPos, fSpeed)
                {
                }
                virtual ~CcSabotenNeedleHockey() {}

            protected:
                virtual float GetCollHitEffectFrm_() const { return 300.0f; }
                virtual float GetCollHitRadius_() const { return 15.0f; }
                virtual int GetCororinDmgEffectID_() const { return 0x610; }
        };
    }
}

msw::CChrCoreHockeySaboten::CChrCoreHockeySaboten()
{
}

msw::CChrCoreHockeySaboten::~CChrCoreHockeySaboten()
{
}

void msw::CChrCoreHockeySaboten::Reset_()
{
    CChrCoreHockey::Reset_();
}

void msw::CChrCoreHockeySaboten::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0xB9, 3);
    SE_PlayArtsJumpSE(0);
}

// The trick is a run of motions, each started when the one before has
// ended: 0x9D, 0x9E, then the needle at frame 40 of 0x9E (or its end), 0x9F
// and 0xA0; the shot is ready when the last is over.
int msw::CChrCoreHockeySaboten::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xBA, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9E, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xBB, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 2:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 40.0f) {
                SubStateArtsShotFireBullet_(pWork, 0);
                pWork->m_nTrickStep++;
            }
            break;
        case 3:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9F, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xBC, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 4:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0xA0, 0, 0.0f, 0.0f);
                pWork->m_nTrickStep++;
            }
            break;
        case 5:
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

void msw::CChrCoreHockeySaboten::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x15, 0xBD, 3);
}

// The needle flies at 53 from where he stands.
void msw::CChrCoreHockeySaboten::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    new (0x100) CC13_HOCKEY::CcSabotenNeedleHockey(CGame::GetTempParent(), this, GetPos(), 53.0f);
}
