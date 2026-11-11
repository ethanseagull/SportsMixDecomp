// msw::CChrCoreDodgeSlime: the Slime in the dodgeball game.
//
// His special shot first shows three slimes on him
// (CC16_DODGE::CcSquadFactoryDodge), then sends three slimes
// (CC16_DODGE::CcSlimeSquadDodge) out to circle round the middle of the
// other team's court. The effects of the shot take the colour of his
// variant (SLIME_COL_SET).
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrOptTask.hpp"
#include "game/msw/CC_CMN/SLIME/CcSlimeSquad.hpp"
#include "game/msw/CC_CMN/SLIME/CcSquadFactory.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/math/triangular.h"

namespace CC16_DODGE {

    // The colours of the shot's effects for one Slime colour: the two
    // colours of the point light and the two konstant colours of the
    // effect model.
    struct SLIME_COL_SET {
        bcs::BCS_Color m_LightColor0;
        bcs::BCS_Color m_LightColor1;
        bcs::BCS_Color m_KColor0;
        bcs::BCS_Color m_KColor1;
    };
}

namespace {

    // The motion speeds of the three slimes shown on the Slime.
    const float s_afSquadSpeed[msw::CC_CMN::SLIME::CcSquadFactory::SQUAD_NUM] = {15.0f, 31.0f, 48.0f};

    // The motion and the effect each of those slimes plays when the trick
    // starts (negative for none).
    struct SQUAD_ANIM {
        short nMotId;
        short nEff;
    };

    const SQUAD_ANIM s_aSquadAnim[msw::CC_CMN::SLIME::CcSquadFactory::SQUAD_NUM] = {
        {0, 0xAB},
        {5, 0xAC},
        {10, 0xAD},
    };

    // The motions of the slimes sent out, by slime and kind.
    const short s_aSlimeMotId[3][4] = {
        {1, 2, 3, 4},
        {6, 7, 8, 9},
        {11, 12, 13, 14},
    };

    // The effects of the slimes sent out, by slime: while flying, on
    // landing and when they vanish (negative for none).
    const short s_aSlimeEff[3][3] = {
        {0xAE, 0xB2, 0x5DE},
        {0xAF, 0xB3, 0x5DF},
        {0xB0, 0xB4, 0x5E0},
    };

    // (name guessed) 60 degrees; nothing in the unit reads it.
    float s_fUnkRange = bcs::DEG_TO_RAD(60.0f);

    // By the Slime's colour (CChrCore::m_nColor, 0 to 2).
    CC16_DODGE::SLIME_COL_SET s_SlimeColSet[3] = {
        {
            bcs::BCS_Color(0x00, 0x46, 0xBE, 0xFF),
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

    // Stand-in for an unknown user of -1, which the original keeps in
    // .rodata after the slimes' chase distance but nothing reads (likely
    // stripped code).
    inline void UnusedReferences(float f)
    {
        if (f == -1.0f) {
        }
    }

    // (guessed helper) The colour set of colour nColor, clamped to 0 to 2.
    inline const CC16_DODGE::SLIME_COL_SET &GetColSet(long nColor)
    {
        long nIndex;
        if (nColor > 2) {
            nIndex = 2;
        } else {
            nIndex = nColor < 0 ? 0 : nColor;
        }
        return s_SlimeColSet[nIndex];
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

namespace CC16_DODGE {

    // The three slimes shown on the Slime while his special shot gets
    // ready. 0x40 bytes, as its base.
    class CcSquadFactoryDodge : public msw::CC_CMN::SLIME::CcSquadFactory {
        public:
            CcSquadFactoryDodge(bcs::CTask *pParent, msw::CChrCore *pChr, const float *pfSpeed)
                : CcSquadFactory(pParent, pChr, pfSpeed)
            {
            }
            virtual ~CcSquadFactoryDodge() {}

            virtual void AnimPlay(long nEffSlot, int bLoop, float fInterFrame);
    };

    // A slime the special shot sends out. 0x118 bytes, as its base.
    class CcSlimeSquadDodge : public msw::CC_CMN::SLIME::CcSlimeSquad {
        public:
            CcSlimeSquadDodge(bcs::CTask *pParent, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, long nIndex,
                              long nTeam, long nPlayer, nw4r::g3d::ResFile res, nw4r::g3d::ResFile mot)
                : CcSlimeSquad(pParent, rPos, rRot, nIndex, nTeam, nPlayer)
            {
                Create(res, mot);
                m_Body.SetHitEffectFrameAll(30.0f);
                long nEff = s_aSlimeEff[m_nIndex][0];
                if (nEff >= 0) {
                    m_hEffect = msw::CEffectCtrl::CreateAttachEffectAuto(nEff, m_pMdl, 3);
                }
            }
            virtual ~CcSlimeSquadDodge();

            virtual void CtorStateTouchDown();
            virtual void CtorStateDead();
            virtual short GetMotID_(long nKind) const;
            virtual float GetRotateRadius_() const;
            virtual float GetRotateSpeed_() const;
            virtual float GetChaseSpeed_() const;
            virtual float GetSearchTargetDist_() const;
            virtual float GetGiveupChaseDist_() const;
    };
}

// ---- CChrCoreDodgeSlime -------------------------------------------------

msw::CChrCoreDodgeSlime::CChrCoreDodgeSlime()
{
    m_hSquadFactory = 0;
}

msw::CChrCoreDodgeSlime::~CChrCoreDodgeSlime()
{
}

void msw::CChrCoreDodgeSlime::Reset_()
{
    if (m_hSquadFactory) {
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hSquadFactory);
        if (pTask && !pTask->IsDelete()) {
            pTask->Delete();
        }
    }
    CChrCoreDodge::Reset_();
}

// The three slimes appear on the Slime.
void msw::CChrCoreDodgeSlime::SubStateArtsShotStartTrick_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    if (m_hSquadFactory) {
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hSquadFactory);
        if (pTask && !pTask->IsDelete()) {
            pTask->Delete();
        }
    }
    CC16_DODGE::CcSquadFactoryDodge *pFactory =
        new (0x100) CC16_DODGE::CcSquadFactoryDodge(CGame::GetTempParent(), this, s_afSquadSpeed);
    if (pFactory) {
        m_hSquadFactory = pFactory->GetId();
    }
    CChrCoreDodge::SubStateArtsShotStartTrick_(pWork);
    SubStateArtsShotDoGetBallShotTrg_(pWork);
}

// Step 0 waits for the shot frame of the motion and sends the slimes out;
// step 1 waits for the end of the motion. The loop motion starts once,
// when the motion ends.
int msw::CChrCoreDodgeSlime::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                SubStateArtsShotFireBullet_(pWork, 0);
                pWork->m_nTrickStep++;
            } else {
                break;
            }
            // fall through: the end of the motion may come in the same frame
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

    // (0x124 serves here as the "loop motion started" flag)
    if (pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nArtsLevel = 1;
    }
    return bReady;
}

void msw::CChrCoreDodgeSlime::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0xAA, 3);

    const CC16_DODGE::SLIME_COL_SET &rColor = GetColSet(m_nColor);
    Eff_SetPointLightColor(0x13, rColor.m_LightColor0, rColor.m_LightColor1);
    Eff_SetMdlKColorReg(0x13, GX_KCOLOR0, rColor.m_KColor0);
    Eff_SetMdlKColorReg(0x13, GX_KCOLOR1, rColor.m_KColor1);

    if (m_hSquadFactory) {
        CC_CMN::SLIME::CcSquadFactory *pFactory =
            static_cast<CC_CMN::SLIME::CcSquadFactory *>(bcs::CTaskMan::GetTask(m_hSquadFactory));
        if (pFactory && !pFactory->IsDelete()) {
            pFactory->AnimPlay(0, 0, 0.0f);
        }
    }
    PlaySePos(0x487, 0, 0, -1, 0.0f, 1.0f, 0.0f);
}

void msw::CChrCoreDodgeSlime::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);
}

void msw::CChrCoreDodgeSlime::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x14, 0xB1, 3);

    const CC16_DODGE::SLIME_COL_SET &rColor = GetColSet(m_nColor);
    Eff_SetPointLightColor(0x14, rColor.m_LightColor0, rColor.m_LightColor1);
    Eff_SetMdlKColorReg(0x14, GX_KCOLOR0, rColor.m_KColor0);
    Eff_SetMdlKColorReg(0x14, GX_KCOLOR1, rColor.m_KColor1);
}

// Sends the three slimes out from the Slime to points 120 degrees apart
// round the middle of the court he aims at, starting at a random angle;
// the slimes shown on the Slime are taken away one by one.
void msw::CChrCoreDodgeSlime::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK_CMN *, long)
{
    nw4r::g3d::ResFile res = GetChrOptResFile();
    if (!res.IsValid()) {
        return;
    }
    nw4r::g3d::ResFile mot = GetChrOptMotResFile();

    bcs::FVEC3 pos(GetPos());
    bcs::FVEC3 rot(GetRot());

    float fBack, fFront, fRight, fLeft;
    CStageMan::GetInstance()->GetTeamCourtRect(GetTargetCourtIdx(), &fLeft, &fRight, &fFront, &fBack);
    bcs::FVEC3 center(0.5f * (fLeft + fRight), 0.0f, 0.5f * (fFront + fBack));

    float fOrbitRot = 60.0f;
    float fSide = 6.0f;
    // The distance from the centre of a triangle of side fSide to its
    // corners (the sine of 60 degrees).
    float fRadius = 0.5f * (fSide / nw4r::math::SinFIdx(42.666668f));
    float fRot = WrapRad(bcs::BCS_RandMax(6.2831855f));

    bcs::FVEC3 aTarget[3];
    long nDeg = 0;
    bcs::FVEC3 *pTarget = aTarget;
    for (long i = 0; i < 3; i++) {
        float fOffset = 0.017453292f * (float)nDeg;
        bcs::BCS_VecZ_RotY(*pTarget, fRadius, WrapRad(fRot + fOffset));
        pTarget->x += center.x;
        pTarget->z += center.z;
        pTarget->y = 0.0f;
        nDeg += 120;
        pTarget++;
    }

    pTarget = aTarget;
    for (long i = 0; i < 3; i++) {
        if (m_hSquadFactory) {
            CC_CMN::SLIME::CcSquadFactory *pFactory =
                static_cast<CC_CMN::SLIME::CcSquadFactory *>(bcs::CTaskMan::GetTask(m_hSquadFactory));
            if (pFactory && !pFactory->IsDelete()) {
                pFactory->KillChild(i);
            }
        }

        CC16_DODGE::CcSlimeSquadDodge *pSquad = new (0x100)
            CC16_DODGE::CcSlimeSquadDodge(CGame::GetTempParent(), pos, rot, i, m_nTeamNo, m_nPlayerNo, res, mot);
        if (pSquad) {
            pSquad->SetTargetOrbit(*pTarget, 2.0f + GetPosY(), fOrbitRot);
            fOrbitRot += 25.0f;
        }
        pTarget++;
    }
    PlaySePos(0x4EA, 0, 0, -1, 0.0f, 1.0f, 0.0f);
}

// ---- CC16_DODGE::CcSquadFactoryDodge ------------------------------------

// Every slime shown plays its motion from the start and its effect in
// slot nEffSlot.
void CC16_DODGE::CcSquadFactoryDodge::AnimPlay(long nEffSlot, int bLoop, float fInterFrame)
{
    for (long i = 0; i < SQUAD_NUM; i++) {
        if (m_aTaskId[i]) {
            msw::CChrOptTask *pTask = static_cast<msw::CChrOptTask *>(bcs::CTaskMan::GetTask(m_aTaskId[i]));
            if (pTask && !pTask->IsDelete()) {
                if (s_aSquadAnim[i].nMotId >= 0) {
                    pTask->AnimPlay(s_aSquadAnim[i].nMotId, bLoop, fInterFrame, 0.0f);
                }
                if (s_aSquadAnim[i].nEff >= 0) {
                    pTask->Eff_CreateAttachAuto(nEffSlot, s_aSquadAnim[i].nEff, 3);
                }
            }
        }
    }
}

// ---- CC16_DODGE::CcSlimeSquadDodge --------------------------------------

CC16_DODGE::CcSlimeSquadDodge::~CcSlimeSquadDodge()
{
    msw::CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
}

short CC16_DODGE::CcSlimeSquadDodge::GetMotID_(long nKind) const
{
    return s_aSlimeMotId[m_nIndex][nKind];
}

// On landing the flying effect gives way to the landing one.
void CC16_DODGE::CcSlimeSquadDodge::CtorStateTouchDown()
{
    CcSlimeSquad::CtorStateTouchDown();
    msw::CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
    if (m_pMdl) {
        long nEff = s_aSlimeEff[m_nIndex][1];
        if (nEff >= 0) {
            msw::CEffectCtrl::CreateAttachEffectAuto(nEff, m_pMdl, 3);
        }
    }
}

// The vanishing effect plays at the model's "SP_item_00" node.
void CC16_DODGE::CcSlimeSquadDodge::CtorStateDead()
{
    long nEff = s_aSlimeEff[m_nIndex][2];
    if (nEff >= 0 && m_pMdl) {
        long nNode = m_pMdl->GetNodeID("SP_item_00");
        bcs::FMTX34 mtx;
        m_pMdl->GetScnMtxPos(&mtx, nNode > 0 ? nNode : 0, bcs::CScnObj::SCNOBJMTXTYPE_1);
        bcs::FVEC3 pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
        msw::CEffectCtrl::SimpleEffect(nEff, pos, 3, 0, 0);
    }
    CcSlimeSquad::CtorStateDead();
}

float CC16_DODGE::CcSlimeSquadDodge::GetRotateRadius_() const
{
    return 3.0f;
}

float CC16_DODGE::CcSlimeSquadDodge::GetRotateSpeed_() const
{
    return 0.038333334f;
}

float CC16_DODGE::CcSlimeSquadDodge::GetChaseSpeed_() const
{
    return 3.0f * (1.0f / 60.0f);
}

float CC16_DODGE::CcSlimeSquadDodge::GetSearchTargetDist_() const
{
    return 5.0f;
}

float CC16_DODGE::CcSlimeSquadDodge::GetGiveupChaseDist_() const
{
    // (the local only keeps 7 ahead of -1 in .rodata, as in the original)
    float fDist = 7.0f;
    UnusedReferences(m_fRadius);
    return fDist;
}
