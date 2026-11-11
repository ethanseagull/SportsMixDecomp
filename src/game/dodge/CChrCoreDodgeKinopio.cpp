// msw::CChrCoreDodgeKinopio: Toad in the dodgeball game. His special shot
// is a hop, then a jump back, during which he throws two bouncing mushrooms
// (CC09_DODGE::CcKinopioBoundMushroom) into the other team's court.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC4.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CC_PARAM_DODGE.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CMdlEffect.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CcBoundMushroom.hpp"
#include "nw4r/math/triangular.h"

#include <math.h>

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace CC09_DODGE {

    // Toad's bouncing mushroom in the dodgeball game: bigger than the common
    // one, swinging from side to side while it turns round its point. 0x128
    // bytes, as its base.
    class CcKinopioBoundMushroom : public msw::CC_CMN::KINOPIO::CcBoundMushroom {
        public:
            CcKinopioBoundMushroom(bcs::CTask *pParent, nw4r::g3d::ResFile mdl, nw4r::g3d::ResFile mot,
                                   const msw::CC_DATA::ARTS_OPT_CREATE_INFO *pInfo, const bcs::FVEC3 &pos,
                                   float fScale, long nCourt, bcs::BCS_Color color, long nPlayer)
                : CcBoundMushroom(pParent, mdl, mot, pInfo, pos, fScale, nCourt, color, nPlayer)
            {
                m_BodyColl.ResetTargetFlagAll(4);
                m_BodyColl.SetHitEffectFrameAll(35.0f);
            }
            virtual ~CcKinopioBoundMushroom();

            virtual float GetParamScale_() const;
            virtual void SetAppearEffect_();
            virtual void SetDisappearEffect_();
            virtual void MovePos_(float fFrame);
            virtual void CalcPos_();
    };
}

namespace {

    // (guessed helpers) The larger of two values; an angle brought into
    // [0, 2 pi]; degrees to radians; the distance between two points in the
    // XZ plane.
    inline float Max(float fValue, float fMin)
    {
        return fMin < fValue ? fValue : fMin;
    }

    inline void Swap(float &rA, float &rB)
    {
        float fTemp = rA;
        rA = rB;
        rB = fTemp;
    }

    inline float WrapRad(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }

    // (guessed) Stand-in for code of the original that the link stripped.
    // Behaviour-free; see notes/decisions.md, "Dead float comparison". The
    // original's .rodata ends with a -1 that nothing left in the file reads.
    inline void UnusedReferences(float f)
    {
        if (f == -1.0f) {
        }
    }
}

msw::CChrCoreDodgeKinopio::CChrCoreDodgeKinopio()
{
}

msw::CChrCoreDodgeKinopio::~CChrCoreDodgeKinopio()
{
}

void msw::CChrCoreDodgeKinopio::DtorStateAirTrick()
{
    CChrCoreDodge::DtorStateAirTrick();
}

// The trick jumps twice: a hop (0) at least 2.5 high, then the trick jump
// (1) up to the height in the parameters.
void msw::CChrCoreDodgeKinopio::SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK_CMN *pWork, long nStep)
{
    if (nStep == 0) {
        float fHeight = 2.5f;
        float fPosY = GetPosY();
        if (fHeight < fPosY) {
            fHeight = fPosY;
        }
        Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, fHeight, 1.0f + fHeight);
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

// The hop moves as usual; the trick jump stays where it is.
void msw::CChrCoreDodgeKinopio::SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK_CMN *pWork, long nStep)
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

// Not zero once the trick jump is over. The end of the hop (step 0) throws
// the mushrooms and starts the trick jump, with its motion, effects and
// sound.
int msw::CChrCoreDodgeKinopio::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (!IsAnimEnd(3)) {
                break;
            }
            SubStateArtsShotFireBullet_(pWork, 0);
            Eff_Stop(0x16);
            AnimPlay(3, 0x7B, 0, 0.0f, 0.0f);
            Eff_CreateAttachAuto(0x14, 0x8B, 3);
            Eff_CreateAttachAuto(0x17, 0x8D, 3);
            PlaySePos(0x440, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            pWork->m_fTrickFrame = AnimGetFrameNum(3);
            SubStateArtsShotSetTrickJumpParam_(pWork, 1);
            SubStateArtsShotSetTrickMoveParam_(pWork, 1);
            pWork->m_nTrickStep++;
            // fall through
        case 1:
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

// The trick leaves a mushroom effect in Toad's colour where he stands.
void msw::CChrCoreDodgeKinopio::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x8A, 3);
    Eff_CreateAttachAuto(0x16, 0x8C, 3);
    CC_CMN::CMdlEffect *pEffect =
        new (0x100) CC_CMN::CMdlEffect(CGame::GetTempParent(), 0x584, GetPos(), GetRot(), 0, 3);
    if (pEffect) {
        bcs::BCS_Color color = CC_DATA::GetChrOptColor_Kinopio(m_nColor);
        pEffect->SetMatTevKColorRGB(GX_KCOLOR0, color);
    }
}

void msw::CChrCoreDodgeKinopio::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x8E, 3);
}

void msw::CChrCoreDodgeKinopio::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_Stop(0x17);
    Eff_Stop(0x15);
    Eff_CreateAttachAuto(0x18, 0x8F, 3);
}

// Two mushrooms on the middle line of the court the team attacks, a quarter
// and three quarters of the way back; the first is Toad's, and the second
// is larger.
void msw::CChrCoreDodgeKinopio::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK_CMN *, long)
{
    const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo =
        CC_DATA::GetArtsOptionMdl_CreateInfo(GetChrIdS(), CGame::g_GameWork.GetGameMode());
    if (!pInfo) {
        return;
    }
    nw4r::g3d::ResFile mdl = GetChrOptResFile();
    if (!mdl.IsValid()) {
        return;
    }
    nw4r::g3d::ResFile mot = GetChrOptMotResFile();

    bcs::FVEC4 rect;
    CStageMan::GetInstance()->GetTeamCourtRect(GetTargetCourtIdx(), &rect.x, &rect.y, &rect.z, &rect.w);
    if (rect.x > rect.y) {
        Swap(rect.x, rect.y);
    }
    if (rect.z > rect.w) {
        Swap(rect.z, rect.w);
    }
    float fParts = 4.0f;
    float fStep = (rect.w - rect.z) / fParts;
    bcs::FVEC3 pos(0.5f * (rect.x + rect.y), 0.0f, rect.z + fStep);

    long nCourt = GetTargetCourtIdx();
    bcs::BCS_Color color = CC_DATA::GetChrOptColor_Kinopio(m_nColor);
    for (long i = 0; i < 2; i++) {
        float fScale = 0.5f * (i & 1) + 0.25f;
        long nPlayer;
        if (i == 0) {
            nPlayer = GetPlayerNo();
        } else {
            nPlayer = -1;
        }
        new (0x100) CC09_DODGE::CcKinopioBoundMushroom(CGame::GetTempParent(), mdl, mot, pInfo, pos, fScale, nCourt,
                                                       color, nPlayer);
        pos.z += 2.0f * fStep;
    }
}

CC09_DODGE::CcKinopioBoundMushroom::~CcKinopioBoundMushroom()
{
}

void CC09_DODGE::CcKinopioBoundMushroom::SetAppearEffect_()
{
    msw::CEffectCtrl::SimpleEffect(0x582, m_Pos, 3, 0, 0);
}

void CC09_DODGE::CcKinopioBoundMushroom::SetDisappearEffect_()
{
    msw::CEffectCtrl::SimpleEffect(0x583, m_Pos, 3, 0, 0);
}

// The swing takes 300 frames, the turn 150; the mushroom spins 4 degrees a
// frame.
void CC09_DODGE::CcKinopioBoundMushroom::MovePos_(float fFrame)
{
    float fSwingFrame = 300.0f;
    m_fSwing += (1.0f / Max(fSwingFrame, 1.0f)) * fFrame;
    m_fSwing = fmod(m_fSwing, 1.0);
    float fTurnFrame = 150.0f;
    m_fTurn += fFrame * (1.0f / Max(fTurnFrame, 1.0f));
    m_fTurn = fmod(m_fTurn, 1.0);
    float fSpin = 4.0f;
    m_fRotY += fFrame * DegToRad(fSpin);
    m_fRotY = WrapRad(m_fRotY);
}

// Swings 2 to each side of its point and turns round it at a distance of 2,
// kept inside the court.
void CC09_DODGE::CcKinopioBoundMushroom::CalcPos_()
{
    bcs::FVEC3 pos(m_BasePos);
    if (m_nCourt >= 0) {
        bcs::FVEC3 ofs = msw::CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
        bcs::FVEC3Add(&pos, &pos, &ofs);
    }

    float fSwing = 2.0f;
    pos.x += fSwing * nw4r::math::SinRad(WrapRad(6.2831855f * m_fSwing));

    float fTurn = WrapRad(6.2831855f * m_fTurn);
    float fTurnRadius = 2.0f;
    if (fTurnRadius > 0.0f) {
        bcs::FVEC3 turn;
        bcs::BCS_VecZ_RotY(turn, fTurnRadius, fTurn);
        pos.x += turn.x;
        pos.z += turn.z;
    }

    m_Pos = pos;
    if (m_nCourt >= 0) {
        bcs::FVEC3 from(pos);
        bcs::FVEC3 hitNormal;
        float fHitTime;
        if (msw::CCollMan::DetectMapHit(pos, hitNormal, fHitTime, from, from, 0.8f * m_fRadius, 1,
                                        (unsigned char)m_nCourt | 0x2000000)) {
            m_Pos = pos;
        }
    }
}

float CC09_DODGE::CcKinopioBoundMushroom::GetParamScale_() const
{
    float fScale = 1.3f;
    UnusedReferences(fScale);
    return fScale;
}
