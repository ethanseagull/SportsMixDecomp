// msw::CChrCoreVolleySlime: the Slime in the volleyball game. His air
// trick calls three slimes (the squad): their models first come out with
// him (CC_CMN::SLIME::CcSquadFactory), then he throws them as
// CC16_VOLLEY::CcSlimeSquadVolley, which circle a point in the middle of
// the other court before they chase the players of the other team. The
// effects of the trick are tinted with the Slime's colour (SLIME_COL_SET).
// Written from the hockey game's version (game/hockey/CChrCoreHockeySlime.cpp).
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CC_CMN/SLIME/CcSlimeSquad.hpp"
#include "game/msw/CC_CMN/SLIME/CcSquadFactory.hpp"
#include "game/msw/CChrOptTask.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/math/triangular.h"

namespace msw {
    namespace CC16_VOLLEY {

        // The colours of the trick's effects for one Slime colour: the two
        // colours of the point light and the two konstant colours of the
        // effect model.
        struct SLIME_COL_SET {
            bcs::BCS_Color m_LightColor0;
            bcs::BCS_Color m_LightColor1;
            bcs::BCS_Color m_KColor0;
            bcs::BCS_Color m_KColor1;
        };

        // The squad models' factory of the volleyball game: plays a motion
        // and an effect on the three models.
        class CcSquadFactoryVolley : public CC_CMN::SLIME::CcSquadFactory {
            public:
                CcSquadFactoryVolley(bcs::CTask *pParent, CChrCore *pChr, const float *pfSpeed)
                    : CcSquadFactory(pParent, pChr, pfSpeed)
                {
                }
                // (implicit destructor: the original emits it after the
                // base's inline Main and Disp)
                virtual void AnimPlay(long nEffSlot, int bLoop, float fSpeed);
        };

        // One slime of the shot in the volleyball game: its motions,
        // effects and speeds.
        class CcSlimeSquadVolley : public CC_CMN::SLIME::CcSlimeSquad {
            public:
                CcSlimeSquadVolley(bcs::CTask *pParent, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, long nIndex,
                                   long nTeam, long nPlayer, nw4r::g3d::ResFile res, nw4r::g3d::ResFile mot);
                virtual ~CcSlimeSquadVolley();
                virtual void CtorStateTouchDown();
                virtual void LoopStateTouchDown();
                virtual void DtorStateTouchDown();
                virtual void CtorStateDead();
                virtual void LoopStateDead();
                virtual void DtorStateDead();
                virtual short GetMotID_(long nMot) const;
                virtual float GetRotateRadius_() const;
                virtual float GetRotateSpeed_() const;
                virtual float GetChaseSpeed_() const;
                virtual float GetSearchTargetDist_() const;
                virtual float GetGiveupChaseDist_() const;
        };
    }
}

namespace {

    // Each squad model's motion and the effect on it (CcSquadFactoryVolley).
    struct SQUAD_ANIM {
        short nMot;
        short nEff;
    };
    const SQUAD_ANIM s_SquadAnim[3] = {
        { 0x0, 0xAA },
        { 0x5, 0xAB },
        { 0xA, 0xAC },
    };

    // The speed of each squad model's motion while they come out.
    const float s_SquadSpeed[3] = { 15.0f, 31.0f, 48.0f };

    // Each slime's motions (CcSlimeSquad::GetMotID_).
    const short s_SquadMotId[3][4] = {
        { 0x1, 0x2, 0x3, 0x4 },
        { 0x6, 0x7, 0x8, 0x9 },
        { 0xB, 0xC, 0xD, 0xE },
    };

    // Each slime's effects: while it flies, when it lands and when it is
    // gone; negative for none.
    struct SQUAD_EFFECT {
        short nFly;
        short nTouchDown;
        short nDead;
    };
    const SQUAD_EFFECT s_SquadEffect[3] = {
        { 0xAD, 0xB1, 0x61D },
        { 0xAE, 0xB2, 0x61E },
        { 0xAF, 0xB3, 0x61F },
    };

    // By CChrCoreVolleySlime::m_nColorType.
    msw::CC16_VOLLEY::SLIME_COL_SET s_SlimeColSet[3] = {
        {
            bcs::BCS_Color(0x00, 0x00, 0xB4, 0xFF),
            bcs::BCS_Color(0x32, 0xAF, 0xFF, 0xFF),
            bcs::BCS_Color(0x32, 0xFF, 0xFF, 0xFF),
            bcs::BCS_Color(0x00, 0x46, 0xFF, 0xFF),
        },
        {
            bcs::BCS_Color(0xFF, 0x00, 0x00, 0xFF),
            bcs::BCS_Color(0xFF, 0xA5, 0x3C, 0xFF),
            bcs::BCS_Color(0xFF, 0x78, 0x32, 0xFF),
            bcs::BCS_Color(0xE6, 0x5F, 0x00, 0xFF),
        },
        {
            bcs::BCS_Color(0x5F, 0x5F, 0x5F, 0xFF),
            bcs::BCS_Color(0x91, 0x91, 0x91, 0xFF),
            bcs::BCS_Color(0xC8, 0xC8, 0xC8, 0xFF),
            bcs::BCS_Color(0x69, 0x69, 0x69, 0xFF),
        },
    };

    // (guessed helper) Deletes the squad factory task hTask, if it is
    // still there.
    inline void DeleteSquadFactory(unsigned long hTask)
    {
        if (hTask != 0) {
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(hTask);
            if (pTask != 0 && !pTask->IsDelete()) {
                pTask->Delete();
            }
        }
    }

    // (guessed helper) An angle brought into [0, 2 pi].
    inline float WrapRad(float fRad)
    {
        while (fRad > 6.2831855f) {
            fRad -= 6.2831855f;
        }
        while (fRad < 0.0f) {
            fRad += 6.2831855f;
        }
        return fRad;
    }
}

// ---- CChrCoreVolleySlime ----------------------------------------------

msw::CChrCoreVolleySlime::CChrCoreVolleySlime()
{
    m_nColorType = 0;
    m_hSquadFactory = 0;
}

msw::CChrCoreVolleySlime::~CChrCoreVolleySlime()
{
}

// Removes the squad models, and takes the colour of the effects from his
// colour variant.
void msw::CChrCoreVolleySlime::Reset_()
{
    CChrCoreVolley::Reset_();
    DeleteSquadFactory(m_hSquadFactory);

    m_nColorType = 0;
    switch (m_nColor) {
        case 1:
            m_nColorType = 1;
            break;
        case 2:
            m_nColorType = 2;
            break;
    }
}

void msw::CChrCoreVolleySlime::DtorStateAirTrick()
{
    CChrCoreVolley::DtorStateAirTrick();
}

// The squad models come out with him when the trick starts.
void msw::CChrCoreVolleySlime::SubStateAirTrickStartTrick_(STATE_WORK_AIR_TRICK *pWork)
{
    DeleteSquadFactory(m_hSquadFactory);
    CC16_VOLLEY::CcSquadFactoryVolley *pFactory =
        new (0x100) CC16_VOLLEY::CcSquadFactoryVolley(CGame::GetTempParent(), this, s_SquadSpeed);
    if (pFactory != 0) {
        m_hSquadFactory = pFactory->GetId();
    }
    CChrCoreVolley::SubStateAirTrickStartTrick_(pWork);
    SubStateAirTrickDoGetBallShotTrg_(pWork);
}

// (guess) He turns until the first squad model is out.
float msw::CChrCoreVolleySlime::SubStateAirTrickGetTrickRotFrame_(STATE_WORK_AIR_TRICK *)
{
    return s_SquadSpeed[0];
}

// The ball is pulled straight onto him over 102 frames.
void msw::CChrCoreVolleySlime::SubStateAirTrickStartBallAdsorb_(STATE_WORK_AIR_TRICK *pWork, CBall *pBall)
{
    pWork->m_nAdsorbStep = 0;
    pWork->m_bAdsorbEnd = 0;
    pWork->m_fAdsorbFrameMax = SubStateAirTrickGetBallAdsorbFrame_(pWork);
    pWork->m_fAdsorbFrame = 0.0f;
    pWork->m_AdsorbOfs.x = 0.0f;
    pWork->m_AdsorbOfs.y = 0.0f;
    pWork->m_AdsorbOfs.z = 0.0f;
    AttachBall(pBall->GetBallNo(), 4, 102.0f, 0);
    m_nStatusFlag &= ~0x2C;
}

float msw::CChrCoreVolleySlime::SubStateAirTrickGetBallAdsorbFrame_(STATE_WORK_AIR_TRICK *)
{
    return 102.0f;
}

void msw::CChrCoreVolleySlime::SubStateAirTrickTrgBallAdsorbEnd_(STATE_WORK_AIR_TRICK *)
{
    CBall *pBall = GetAttachBall();
    if (pBall) {
        pBall->StopTailEffect();
    }
}

// The slimes are thrown at the shot frame of the trick motion; the shot is
// ready when the motion is over.
int msw::CChrCoreVolleySlime::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;
    SubStateAirTrickDoJumpMove_(pWork);
    SubStateAirTrickDoBallAdsorb_(pWork);

    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                SubStateAirTrickFireSlimeChild_(pWork);
                pWork->m_nTrickStep++;
            }
            break;
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
    return bReady;
}

// Throws the three slimes. They circle the corners of a triangle (turned at
// random) round the middle of the court he aims at, each starting at
// another angle.
void msw::CChrCoreVolleySlime::SubStateAirTrickFireSlimeChild_(STATE_WORK_AIR_TRICK *)
{
    nw4r::g3d::ResFile res = GetChrOptResFile();
    if (!res.IsValid()) {
        return;
    }
    nw4r::g3d::ResFile mot = GetChrOptMotResFile();

    bcs::FVEC3 pos(GetPos());
    bcs::FVEC3 rot(GetRot());
    // The middle of the court he aims at.
    bcs::FVEC3 center;
    bcs::FVEC4 rect;
    Util_GetTargetCourtRect_(rect);
    center.z = 0.5f * (rect.z + rect.w);
    center.y = 0.0f;
    center.x = 0.5f * (rect.x + rect.y);

    float fStartRot = 60.0f;
    float fSide = 7.0f;
    // The distance from the centre of a triangle of side fSide to its
    // corners (the sine of 60 degrees).
    float fRadius = 0.5f * (fSide / nw4r::math::SinFIdx(42.666668f));
    float fPhase = WrapRad(bcs::BCS_RandMax(6.2831855f));

    bcs::FVEC3 aCorner[3];
    long nDeg = 0;
    bcs::FVEC3 *pCorner = aCorner;
    for (long i = 0; i < 3; i++) {
        float fOffset = 0.017453292f * (float)nDeg;
        bcs::BCS_VecZ_RotY(*pCorner, fRadius, WrapRad(fPhase + fOffset));
        pCorner->x += center.x;
        pCorner->z += center.z;
        pCorner->y = 0.0f;
        nDeg += 120;
        pCorner++;
    }

    pCorner = aCorner;
    for (long i = 0; i < 3; i++) {
        if (m_hSquadFactory != 0) {
            CC_CMN::SLIME::CcSquadFactory *pFactory =
                static_cast<CC_CMN::SLIME::CcSquadFactory *>(bcs::CTaskMan::GetTask(m_hSquadFactory));
            if (pFactory != 0 && !pFactory->IsDelete()) {
                pFactory->KillChild(i);
            }
        }
        CC16_VOLLEY::CcSlimeSquadVolley *pSquad = new (0x100) CC16_VOLLEY::CcSlimeSquadVolley(
            CGame::GetTempParent(), pos, rot, i, GetTeamNo(), GetPlayerNo(), res, mot);
        if (pSquad != 0) {
            pSquad->SetTargetOrbit(*pCorner, 2.0f + GetPosY(), fStartRot);
            fStartRot += 30.0f;
        }
        pCorner++;
    }
    PlaySePos(0x4EA, 0, 0, -1, 0.0f, 1.0f, 0.0f);
}

void msw::CChrCoreVolleySlime::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0xA9, 3);
    const CC16_VOLLEY::SLIME_COL_SET &rColor = s_SlimeColSet[m_nColorType];
    Eff_SetPointLightColor(0x13, rColor.m_LightColor0, rColor.m_LightColor1);
    Eff_SetMdlKColorReg(0x13, GX_KCOLOR0, rColor.m_KColor0);
    Eff_SetMdlKColorReg(0x13, GX_KCOLOR1, rColor.m_KColor1);

    if (m_hSquadFactory != 0) {
        CC_CMN::SLIME::CcSquadFactory *pFactory =
            static_cast<CC_CMN::SLIME::CcSquadFactory *>(bcs::CTaskMan::GetTask(m_hSquadFactory));
        if (pFactory != 0 && !pFactory->IsDelete()) {
            pFactory->AnimPlay(0, 0, 0.0f);
        }
    }
    PlaySePos(0x487, 0, 0, -1, 0.0f, 1.0f, 0.0f);
}

void msw::CChrCoreVolleySlime::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0xB0, 3);
    const CC16_VOLLEY::SLIME_COL_SET &rColor = s_SlimeColSet[m_nColorType];
    Eff_SetPointLightColor(0x14, rColor.m_LightColor0, rColor.m_LightColor1);
    Eff_SetMdlKColorReg(0x14, GX_KCOLOR0, rColor.m_KColor0);
    Eff_SetMdlKColorReg(0x14, GX_KCOLOR1, rColor.m_KColor1);
}

// ---- CC16_VOLLEY::CcSquadFactoryVolley ----------------------------------

// Plays each squad model's motion (looped or not, at fSpeed) and puts its
// effect in effect slot nEffSlot.
void msw::CC16_VOLLEY::CcSquadFactoryVolley::AnimPlay(long nEffSlot, int bLoop, float fSpeed)
{
    for (long i = 0; i < SQUAD_NUM; i++) {
        if (m_aTaskId[i] != 0) {
            CChrOptTask *pTask = static_cast<CChrOptTask *>(bcs::CTaskMan::GetTask(m_aTaskId[i]));
            if (pTask != 0 && !pTask->IsDelete()) {
                if (s_SquadAnim[i].nMot >= 0) {
                    pTask->AnimPlay(s_SquadAnim[i].nMot, bLoop, fSpeed, 0.0f);
                }
                if (s_SquadAnim[i].nEff >= 0) {
                    pTask->Eff_CreateAttachAuto(nEffSlot, s_SquadAnim[i].nEff, 3);
                }
            }
        }
    }
}

// ---- CC16_VOLLEY::CcSlimeSquadVolley ------------------------------------

inline msw::CC16_VOLLEY::CcSlimeSquadVolley::CcSlimeSquadVolley(bcs::CTask *pParent, const bcs::FVEC3 &rPos,
                                                               const bcs::FVEC3 &rRot, long nIndex, long nTeam,
                                                               long nPlayer, nw4r::g3d::ResFile res,
                                                               nw4r::g3d::ResFile mot)
    : CcSlimeSquad(pParent, rPos, rRot, nIndex, nTeam, nPlayer)
{
    Create(res, mot);
    m_Body.SetHitEffectFrameAll(0.0f);
    short nEff = s_SquadEffect[m_nIndex].nFly;
    if (nEff >= 0) {
        m_hEffect = CEffectCtrl::CreateAttachEffectAuto(nEff, m_pMdl, 3);
    }
}

msw::CC16_VOLLEY::CcSlimeSquadVolley::~CcSlimeSquadVolley()
{
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
}

short msw::CC16_VOLLEY::CcSlimeSquadVolley::GetMotID_(long nMot) const
{
    return s_SquadMotId[m_nIndex][nMot];
}

void msw::CC16_VOLLEY::CcSlimeSquadVolley::CtorStateTouchDown()
{
    CcSlimeSquad::CtorStateTouchDown();
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
    if (m_pMdl != 0) {
        short nEff = s_SquadEffect[m_nIndex].nTouchDown;
        if (nEff >= 0) {
            CEffectCtrl::CreateAttachEffectAuto(nEff, m_pMdl, 3);
        }
    }
}

// A last effect where the slime's item node was.
void msw::CC16_VOLLEY::CcSlimeSquadVolley::LoopStateTouchDown()
{
    CcSlimeSquad::LoopStateTouchDown();
}

void msw::CC16_VOLLEY::CcSlimeSquadVolley::DtorStateTouchDown()
{
    CcSlimeSquad::DtorStateTouchDown();
}

void msw::CC16_VOLLEY::CcSlimeSquadVolley::CtorStateDead()
{
    short nEff = s_SquadEffect[m_nIndex].nDead;
    if (nEff >= 0 && m_pMdl != 0) {
        long nNode = m_pMdl->GetNodeID("SP_item_00");
        bcs::FMTX34 mtx;
        m_pMdl->GetScnMtxPos(&mtx, nNode > 0 ? nNode : 0, static_cast<bcs::CScnObj::ScnObjMtxType>(1));
        bcs::FVEC3 pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
        CEffectCtrl::SimpleEffect(nEff, pos, 3, 0, 0);
    }
    CcSlimeSquad::CtorStateDead();
}

void msw::CC16_VOLLEY::CcSlimeSquadVolley::LoopStateDead()
{
    CcSlimeSquad::LoopStateDead();
}

void msw::CC16_VOLLEY::CcSlimeSquadVolley::DtorStateDead()
{
    CcSlimeSquad::DtorStateDead();
}

float msw::CC16_VOLLEY::CcSlimeSquadVolley::GetRotateRadius_() const
{
    return 3.0f;
}

float msw::CC16_VOLLEY::CcSlimeSquadVolley::GetRotateSpeed_() const
{
    return 2.3f / 60.0f;
}

float msw::CC16_VOLLEY::CcSlimeSquadVolley::GetChaseSpeed_() const
{
    return 2.3f / 60.0f;
}

float msw::CC16_VOLLEY::CcSlimeSquadVolley::GetSearchTargetDist_() const
{
    return 5.0f;
}

float msw::CC16_VOLLEY::CcSlimeSquadVolley::GetGiveupChaseDist_() const
{
    return 7.0f;
}
