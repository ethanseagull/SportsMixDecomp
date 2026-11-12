// msw::CChrCoreHockeyKinopio (Toad) in the hockey game. His special shot
// puts two bouncing mushrooms (CC09_HOCKEY::CcKinopioBoundMushroom) in front
// of the goal his team attacks, to block its keeper.
#include "game/msw/CChrCoreHockey.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/New.hpp"
#include "bcs/FVEC4.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CMdlEffect.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CcBoundMushroom.hpp"
#include "nw4r/math/triangular.h"

#include <math.h>

namespace msw {
    namespace CC09_HOCKEY {

        // Toad's bouncing mushroom in the hockey game: smaller than the
        // volleyball one, swinging from side to side while it turns round
        // its point; it hits only some kinds of body (target flag 4). 0x128
        // bytes, as its base.
        class CcKinopioBoundMushroom : public CC_CMN::KINOPIO::CcBoundMushroom {
            public:
                CcKinopioBoundMushroom(bcs::CTask *pParent, nw4r::g3d::ResFile mdl, nw4r::g3d::ResFile mot,
                                       const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo, const bcs::FVEC3 &pos,
                                       float fScale, long nCourt, bcs::BCS_Color color, long nPlayer)
                    : CcBoundMushroom(pParent, mdl, mot, pInfo, pos, fScale, nCourt, color, nPlayer)
                {
                    m_BodyColl.ResetTargetFlagAll(4);
                    m_BodyColl.SetHitEffectFrameAll(30.0f);
                }
                virtual ~CcKinopioBoundMushroom();

                virtual float GetParamScale_() const;
                virtual void SetAppearEffect_();
                virtual void SetDisappearEffect_();
                virtual void MovePos_(float fFrame);
                virtual void CalcPos_();
        };
    }
}

namespace {

    // (guessed helpers) The larger of two values; an angle brought into
    // [0, 2 pi); degrees to radians.
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
}

msw::CChrCoreHockeyKinopio::CChrCoreHockeyKinopio()
{
}

msw::CChrCoreHockeyKinopio::~CChrCoreHockeyKinopio()
{
}

void msw::CChrCoreHockeyKinopio::SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    CChrCoreHockey::SubStateArtsShotSetTrickJumpParam_(pWork, nStep);
}

void msw::CChrCoreHockeyKinopio::SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    CChrCoreHockey::SubStateArtsShotSetTrickMoveParam_(pWork, nStep);
}

// The trick is three motions: at the end of the first the mushrooms come
// out (motion 0x9D), then the second one (0x9E), and the shot is ready when
// that is over. Each step goes on to the next when its motion has ended (a
// motion just started has not, so the fall-throughs only save a frame).
int msw::CChrCoreHockeyKinopio::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
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
            AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
            Eff_CreateAttachAuto(0x14, 0xA6, 3);
            Eff_CreateAttachAuto(0x17, 0xA8, 3);
            pWork->m_nTrickStep++;
            // fall through
        case 1:
            if (!IsAnimEnd(3)) {
                break;
            }
            Eff_Stop(0x16);
            AnimPlay(3, 0x9E, 0, 0.0f, 0.0f);
            Eff_CreateAttachAuto(0x14, 0xA7, 3);
            pWork->m_nTrickStep++;
            // fall through
        case 2:
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
    return bReady;
}

// The jump leaves a mushroom effect in Toad's colour.
void msw::CChrCoreHockeyKinopio::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0xA5, 3);
    SE_PlayArtsJumpSE(0);
    CC_CMN::CMdlEffect *pEffect = new (0x100) CC_CMN::CMdlEffect(CGame::GetTempParent(), 0x5E4, GetPos(), GetRot(), 0, 3);
    if (pEffect) {
        bcs::BCS_Color color = CC_DATA::GetChrOptColor_Kinopio(m_nColor);
        pEffect->SetMatTevKColorRGB(GX_KCOLOR0, color);
    }
}

void msw::CChrCoreHockeyKinopio::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(pWork);
}

void msw::CChrCoreHockeyKinopio::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_Stop(0x17);
    Eff_Stop(0x15);
    Eff_CreateAttachAuto(0x18, 0xA9, 3);
}

// Two mushrooms on the middle line of the court the team attacks, 4.5 and
// 8 in front of the goal; the first is Toad's, and the second is larger.
void msw::CChrCoreHockeyKinopio::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    long nCourt;
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

    // Distances from the goal line, towards the middle of the rink.
    float fOfs = 4.5f;
    float fStep = 3.5f;
    bcs::FVEC3 goal(GetTargetGoalPos());
    if (GetTargetGoalIdx() != 0) {
        fOfs = -fOfs;
        fStep = -fStep;
    }
    bcs::FVEC3 pos(0.5f * (rect.x + rect.y), 0.0f, goal.z + fOfs);

    nCourt = GetTargetCourtIdx();
    bcs::BCS_Color color = CC_DATA::GetChrOptColor_Kinopio(m_nColor);
    for (long i = 0; i < 2; i++) {
        float fScale = 0.5f * (i & 1) + 0.25f;
        long nPlayer;
        if (i == 0) {
            nPlayer = GetPlayerNo();
        } else {
            nPlayer = -1;
        }
        new (0x100) CC09_HOCKEY::CcKinopioBoundMushroom(CGame::GetTempParent(), mdl, mot, pInfo, pos, fScale, nCourt,
                                                        color, nPlayer);
        pos.z += fStep;
    }
}

msw::CC09_HOCKEY::CcKinopioBoundMushroom::~CcKinopioBoundMushroom()
{
}

void msw::CC09_HOCKEY::CcKinopioBoundMushroom::SetAppearEffect_()
{
    CEffectCtrl::SimpleEffect(0x5E2, m_Pos, 3, 0, 0);
}

void msw::CC09_HOCKEY::CcKinopioBoundMushroom::SetDisappearEffect_()
{
    CEffectCtrl::SimpleEffect(0x5E3, m_Pos, 3, 0, 0);
}

// The swing takes 480 frames, the turn 300; the mushroom spins 4 degrees a
// frame.
void msw::CC09_HOCKEY::CcKinopioBoundMushroom::MovePos_(float fFrame)
{
    float fSwingFrame = 480.0f;
    m_fSwing += (1.0f / Max(fSwingFrame, 1.0f)) * fFrame;
    m_fSwing = fmod(m_fSwing, 1.0);
    float fTurnFrame = 300.0f;
    m_fTurn += (1.0f / Max(fTurnFrame, 1.0f)) * fFrame;
    m_fTurn = fmod(m_fTurn, 1.0);
    float fSpin = 4.0f;
    m_fRotY += DegToRad(fSpin) * fFrame;
    m_fRotY = WrapRad(m_fRotY);
}

// Swings 2.5 to each side of its point and turns round it at a distance of
// 3.5, kept inside the court.
void msw::CC09_HOCKEY::CcKinopioBoundMushroom::CalcPos_()
{
    bcs::FVEC3 pos(m_BasePos);
    if (m_nCourt >= 0) {
        bcs::FVEC3 ofs = CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
        bcs::FVEC3Add(&pos, &pos, &ofs);
    }

    float fSwing = 2.5f;
    pos.x += fSwing * nw4r::math::SinRad(WrapRad(6.2831855f * m_fSwing));

    float fTurn = WrapRad(6.2831855f * m_fTurn);
    float fTurnRadius = 3.5f;
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
        if (CCollMan::DetectMapHit(pos, hitNormal, fHitTime, from, from, 0.8f * m_fRadius, 5,
                                   (unsigned char)m_nCourt | 0x2000000)) {
            m_Pos = pos;
        }
    }
}

float msw::CC09_HOCKEY::CcKinopioBoundMushroom::GetParamScale_() const
{
    return 1.2f;
}
