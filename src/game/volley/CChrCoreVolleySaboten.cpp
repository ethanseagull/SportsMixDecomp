// msw::CChrCoreVolleySaboten: the cactus (Saboten) in the volleyball game.
// His air trick hops to the back line of his court, then shoots needles
// (CC13_VOLLEY::CcSabotenNeedleVolley) for 53 frames.
//
// msw::CC13_VOLLEY::CcSabotenNeedleVolley: the needles. The base class
// (msw::CC_CMN::SABOTEN::CcNeedle, in main.dol) does the work; this class
// only gives its three tuning values.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC4.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcNeedle.hpp"

namespace msw {
    namespace CC13_VOLLEY {

        class CcSabotenNeedleVolley : public CC_CMN::SABOTEN::CcNeedle {
            public:
                CcSabotenNeedleVolley(bcs::CTask *pParent, CChrCore *pOwner, const bcs::FVEC3 &rPos, float fFrame)
                    : CcNeedle(pParent, pOwner, rPos, fFrame)
                {
                }
                virtual ~CcSabotenNeedleVolley() {}

            protected:
                virtual float GetCollHitEffectFrm_() const { return 300.0f; }
                virtual float GetCollHitRadius_() const { return 15.0f; }
                virtual int GetCororinDmgEffectID_() const { return 0x5F3; }
        };
    }
}

namespace {

    // (guessed helpers) The smaller of two values; the distance of two
    // points in the XZ plane.
    inline float Min(float fValue, float fMax)
    {
        return fValue > fMax ? fMax : fValue;
    }

    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }
}

msw::CChrCoreVolleySaboten::CChrCoreVolleySaboten()
{
}

msw::CChrCoreVolleySaboten::~CChrCoreVolleySaboten()
{
}

void msw::CChrCoreVolleySaboten::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
    Eff_CreateAttach(0x13, 0x698, m_nEffNodeSpike, 0, 1, 3, 0);
}

// Step 0 is a hop 3 high; the second jump rises to the height in the
// parameters (or stays level when he is higher already).
void msw::CChrCoreVolleySaboten::SubStateAirTrickSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    if (nStep == 0) {
        float fHeight = 3.0f;
        Util_SetJumpParam(pWork->m_fTrickFrame, GetPosY(), fHeight, 0.5f + fHeight);
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

// The hop goes to the middle of the back line of his court; the second
// jump moves 2 back towards his own goal.
void msw::CChrCoreVolleySaboten::SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    static const float fDist = 2.0f;

    if (nStep == 0) {
        ClearMoveParamWork_(pWork->m_MoveParam);
        Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
        pWork->m_fUnk20 = 0.0f;
        pWork->m_TargetPos = GetPos();

        bcs::FVEC4 rect;
        Util_GetGuardCourtRect_(rect);
        pWork->m_TargetPos.x = 0.5f * (rect.x + rect.y);
        if (GetGuardGoalIdx()) {
            pWork->m_TargetPos.z = rect.z;
        } else {
            pWork->m_TargetPos.z = rect.w;
        }
        pWork->m_MoveParam.m_nType = 0xB;
        pWork->m_MoveParam.m_fDist = DistXZ(pWork->m_TargetPos, GetPos());
        pWork->m_MoveParam.m_fFrame = 0.0f;
        pWork->m_MoveParam.m_fFrameMax = pWork->m_fTrickFrame;
    } else {
        ClearMoveParamWork_(pWork->m_MoveParam);
        Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
        pWork->m_fUnk20 = 0.0f;
        pWork->m_TargetPos = GetPos();

        pWork->m_TargetPos.z += GetGuardGoalIdx() ? -fDist : fDist;
        pWork->m_MoveParam.m_nType = 0xB;
        float fMove = DistXZ(pWork->m_TargetPos, GetPos());
        pWork->m_MoveParam.m_fDist = -1.0f * fMove;
        pWork->m_MoveParam.m_fFrame = 0.0f;
        pWork->m_MoveParam.m_fFrameMax = pWork->m_fTrickFrame;
    }
}

float msw::CChrCoreVolleySaboten::SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long nStep)
{
    if (nStep == 0) {
        return AnimGetResFrameNum(0x75);
    }
    return AnimGetResFrameNum(0x77);
}

float msw::CChrCoreVolleySaboten::SubStateAirTrickGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long nStep)
{
    if (nStep == 0) {
        return 60.0f;
    }
    return GetCcParam_()->m_Unk380;
}

int msw::CChrCoreVolleySaboten::SubStateAirTrickIsTMStartAuto_(STATE_WORK_AIR_TRICK *)
{
    return 0;
}

// Step 0 is the hop; at its end he starts the needles, which fly for 53
// frames (step 1); then the target marker shows, and the shot is ready when
// the motion is over.
int msw::CChrCoreVolleySaboten::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    switch (pWork->m_nTrickStep) {
        case 0:
            pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
            pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);
            SubStateAirTrickDoJumpMove_(pWork);
            SubStateAirTrickDoBallAdsorb_(pWork);
            if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
                AnimPlay(3, 0x76, 0, 4.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0x9B, 3);
                PlaySePos(0x469, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                new (0x100) CC13_VOLLEY::CcSabotenNeedleVolley(CGame::GetTempParent(), this, GetPos(), 53.0f);
                pWork->m_fTrickCount = 0.0f;
                pWork->m_fTrickFrame = 53.0f;
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
            if ((pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame)) >= pWork->m_fTrickFrame) {
                SubStateAirTrickStartTMDisp_(pWork);
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

void msw::CChrCoreVolleySaboten::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x99, 3);
}

void msw::CChrCoreVolleySaboten::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x9C, 3);
}

void msw::CChrCoreVolleySaboten::SubStateAirTrickPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLandMotion_(pWork);
    Eff_CreateAttachAuto(0x16, 0x9A, 3);
}
