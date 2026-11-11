// msw::CChrCoreDodgeSaboten: the cactus (Saboten) in the dodgeball game. His
// special shot is a jump; at its top he fires a needle
// (CC13_DODGE::CcSabotenNeedleDodge), then falls and lands.
//
// CC13_DODGE::CcSabotenNeedleDodge: the needle. The base class
// (msw::CC_CMN::SABOTEN::CcNeedle, in main.dol) does the work; this class only
// gives its three tuning values. (Top-level namespace, unlike the
// msw::CC13_BASKET class.)
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_PARAM_DODGE.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcNeedle.hpp"

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace CC13_DODGE {

    class CcSabotenNeedleDodge : public msw::CC_CMN::SABOTEN::CcNeedle {
        public:
            CcSabotenNeedleDodge(bcs::CTask *pParent, msw::CChrCore *pOwner, const bcs::FVEC3 &rPos, float fSpeed)
                : CcNeedle(pParent, pOwner, rPos, fSpeed)
            {
            }
            virtual ~CcSabotenNeedleDodge() {}

        protected:
            virtual float GetCollHitEffectFrm_() const;
            virtual float GetCollHitRadius_() const { return 15.0f; }
            virtual int GetCororinDmgEffectID_() const { return 0x5B4; }
    };
}

namespace {

    // (guessed) Stand-in for code of the original that the link stripped.
    // Behaviour-free; see notes/decisions.md, "Dead float comparison". The
    // original's .rodata ends with a -1 that nothing left in the file reads.
    inline void UnusedReferences(float f)
    {
        if (f == -1.0f) {
        }
    }
}

inline float CC13_DODGE::CcSabotenNeedleDodge::GetCollHitEffectFrm_() const
{
    float fFrame = 300.0f;
    UnusedReferences(fFrame);
    return fFrame;
}

msw::CChrCoreDodgeSaboten::CChrCoreDodgeSaboten()
{
}

msw::CChrCoreDodgeSaboten::~CChrCoreDodgeSaboten()
{
}

// The trick has two jumps: a short one (0) 2.5 high, then the jump the shot
// is fired from (1), up to the height in the parameters.
void msw::CChrCoreDodgeSaboten::SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK_CMN *pWork, long nStep)
{
    if (nStep == 0) {
        float fHeight = 2.5f;
        Util_SetJumpParam(pWork->m_fTrickFrame, GetPosY(), fHeight, 0.5f + fHeight);
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

void msw::CChrCoreDodgeSaboten::SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK_CMN *pWork, long nStep)
{
    CChrCoreDodge::SubStateArtsShotSetTrickMoveParam_(pWork, nStep);
}

float msw::CChrCoreDodgeSaboten::SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK_CMN *, long nStep)
{
    if (nStep == 0) {
        return AnimGetResFrameNum(0x7A);
    }
    return AnimGetResFrameNum(0x7C);
}

float msw::CChrCoreDodgeSaboten::SubStateArtsShotGetTrickDoFrame_(STATE_WORK_AIR_TRICK_CMN *, long nStep)
{
    if (nStep == 0) {
        return 0.0f;
    }
    return GetCcParam_()->m_Unk570;
}

// Not zero once the shot's motion is over. The end of the first jump (step
// 0) fires the needle; step 1 waits for the end of the second.
int msw::CChrCoreDodgeSaboten::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    int bReady = 0;

    switch (pWork->m_nTrickStep) {
        case 0:
            pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
            pWork->m_fTrickCount =
                pWork->m_fTrickCount > pWork->m_fTrickFrame ? pWork->m_fTrickFrame : pWork->m_fTrickCount;
            SubStateArtsShotDoJumpMove_(pWork);
            if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
                AnimPlay(3, 0x7B, 0, 4.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0x9D, 3);
                PlaySePos(0x469, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                new (0x100) CC13_DODGE::CcSabotenNeedleDodge(CGame::GetTempParent(), this, GetPos(), 53.0f);
                pWork->m_fTrickCount = 0.0f;
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
            pWork->m_fTrickCount =
                pWork->m_fTrickCount > pWork->m_fTrickFrame ? pWork->m_fTrickFrame : pWork->m_fTrickCount;
            if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
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

void msw::CChrCoreDodgeSaboten::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x9B, 3);
}

void msw::CChrCoreDodgeSaboten::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x9E, 3);
}

void msw::CChrCoreDodgeSaboten::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLandMotion_(pWork);
    Eff_CreateAttachAuto(0x16, 0x9C, 3);
}
