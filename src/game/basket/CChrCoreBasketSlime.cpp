// msw::CChrCoreBasketSlime: the Slime in the basketball game.
//
// His jump to the upper floor and his smash cut leave after-images and keep
// a small body collision. His special shot first shows three slimes on him
// (CC16_BASKET::CcSquadFactoryBasket), then sends three slimes
// (CC16_BASKET::CcSlimeSquadBasket) out to circle in front of the goal his
// team attacks. The effects of the shot take the colour of his variant.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrOptTask.hpp"
#include "game/msw/CC_CMN/SLIME/CcSlimeSquad.hpp"
#include "game/msw/CC_CMN/SLIME/CcSquadFactory.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/math/triangular.h"

namespace {

    // The after-images of the smash cut and of the jump to the upper floor:
    // the frames between two, and how many frames one lasts.
    const msw::CChrCoreBasketSlime::ILLUSION_PARAM s_SmashCutIllusion = {1.0f, 8.0f};
    const msw::CChrCoreBasketSlime::ILLUSION_PARAM s_JumpUpperIllusion = {1.0f, 8.0f};
}

// The after-image helpers come before the other tables: the original has
// UpDateIllusionWork's WORK_FRM right after the two parameter sets.
inline void msw::CChrCoreBasketSlime::SetIllusionWork(float fEnd, const ILLUSION_PARAM &rParam)
{
    m_IllusionWork.m_fStart = 0.0f;
    m_IllusionWork.m_fEnd = fEnd;
    m_IllusionWork.m_fUnk08 = 0.0f;
    m_IllusionWork.m_fCount = 0.0f;
    m_IllusionWork.m_fInterval = rParam.m_fInterval;
    m_IllusionWork.m_fAlpha = 0.7f;
    m_IllusionWork.m_fNum = rParam.m_fNum;
}

// One after-image is due every m_fInterval frames of the state's window.
inline void msw::CChrCoreBasketSlime::UpDateIllusionWork(float fFrame)
{
    static const float WORK_FRM = 1.0f;

    if (fFrame >= m_IllusionWork.m_fStart && fFrame <= m_IllusionWork.m_fEnd) {
        float fStep = m_fFrameStep;
        while (fStep >= WORK_FRM) {
            fStep -= WORK_FRM;
            m_IllusionWork.m_fCount -= WORK_FRM;
            if (m_IllusionWork.m_fCount <= 0.0f) {
                m_IllusionWork.m_fCount += m_IllusionWork.m_fInterval;
                CreateIllusion(m_IllusionWork.m_fAlpha, WORK_FRM / m_IllusionWork.m_fNum);
            }
        }
        m_IllusionWork.m_fCount -= fStep;
    }
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
        {0, 0xB6},
        {5, 0xB7},
        {10, 0xB8},
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
        {0xB9, 0xBD, 0x5C9},
        {0xBA, 0xBE, 0x5CA},
        {0xBB, 0xBF, 0x5CB},
    };

    // (name guessed) 70 degrees; nothing in the unit reads it.
    float s_fUnkRange = bcs::DEG_TO_RAD(70.0f);

    // Stand-in for an unknown user of -1, which the original keeps in
    // .rodata after the slimes' chase distance but nothing reads (likely
    // stripped code).
    inline void UnusedReferences(float f)
    {
        if (f == -1.0f) {
        }
    }

    // (guessed helper) A colour variant kept within the three of
    // GetArtsEffectUnit.
    inline long ClampColor(long nColor)
    {
        return nColor > 2 ? 2 : (nColor < 0 ? 0 : nColor);
    }

    // (guessed helper) An angle brought into [0, 2 pi).
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
}

namespace msw {
    namespace CC16_BASKET {

        // The three slimes shown on the Slime while his special shot gets
        // ready. 0x40 bytes, as its base.
        class CcSquadFactoryBasket : public CC_CMN::SLIME::CcSquadFactory {
            public:
                CcSquadFactoryBasket(bcs::CTask *pParent, CChrCore *pChr, const float *pfSpeed)
                    : CcSquadFactory(pParent, pChr, pfSpeed)
                {
                }
                virtual ~CcSquadFactoryBasket() {}

                virtual void AnimPlay(long nEffSlot, int bLoop, float fInterFrame);
        };

        // A slime the special shot sends out. 0x118 bytes, as its base.
        class CcSlimeSquadBasket : public CC_CMN::SLIME::CcSlimeSquad {
            public:
                CcSlimeSquadBasket(bcs::CTask *pParent, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, long nIndex,
                                   long nTeam, long nPlayer, nw4r::g3d::ResFile res, nw4r::g3d::ResFile mot)
                    : CcSlimeSquad(pParent, rPos, rRot, nIndex, nTeam, nPlayer)
                {
                    Create(res, mot);
                    m_BodyColl.SetHitEffectFrameAll(0.0f);
                    long nEff = s_aSlimeEff[m_nIndex][0];
                    if (nEff >= 0) {
                        m_hEffect = CEffectCtrl::CreateAttachEffectAuto(nEff, m_pMdl, 3);
                    }
                }
                virtual ~CcSlimeSquadBasket();

                virtual void CtorStateTouchDown();
                virtual void CtorStateDead();
                virtual long GetMotID_(long nKind) const;
                virtual float GetRotateRadius_() const;
                virtual float GetRotateSpeed_() const;
                virtual float GetChaseSpeed_() const;
                virtual float GetSearchTargetDist_() const;
                virtual float GetGiveupChaseDist_() const;
        };
    }
}

const msw::CChrCoreBasketSlime::SLIME_COL_SET *msw::CChrCoreBasketSlime::GetArtsEffectUnit(long nColor)
{
    static const SLIME_COL_SET s_SlimeArtsEffectUnit[3] = {
        { bcs::BCS_Color(0x00, 0x00, 0xB4, 0xFF), bcs::BCS_Color(0x32, 0xAF, 0xFF, 0xFF),
          bcs::BCS_Color(0x32, 0xFF, 0xFF, 0xFF), bcs::BCS_Color(0x00, 0x46, 0xFF, 0xFF) },
        { bcs::BCS_Color(0xFF, 0x00, 0x00, 0xFF), bcs::BCS_Color(0xFF, 0xA5, 0x3C, 0xFF),
          bcs::BCS_Color(0xFF, 0x78, 0x32, 0xFF), bcs::BCS_Color(0xE6, 0x5F, 0x00, 0xFF) },
        { bcs::BCS_Color(0x5F, 0x5F, 0x5F, 0xFF), bcs::BCS_Color(0x91, 0x91, 0x91, 0xFF),
          bcs::BCS_Color(0xC8, 0xC8, 0xC8, 0xFF), bcs::BCS_Color(0x69, 0x69, 0x69, 0xFF) },
    };

    return &s_SlimeArtsEffectUnit[nColor];
}

msw::CChrCoreBasketSlime::CChrCoreBasketSlime()
{
    m_hSquadFactory = 0;
}

msw::CChrCoreBasketSlime::~CChrCoreBasketSlime()
{
}

inline void msw::CChrCoreBasketSlime::DeleteSquadFactory_()
{
    if (m_hSquadFactory) {
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hSquadFactory);
        if (pTask && !pTask->IsDelete()) {
            pTask->Delete();
        }
    }
}

void msw::CChrCoreBasketSlime::Reset_()
{
    DeleteSquadFactory_();
    CChrCore::Reset_();
}

void msw::CChrCoreBasketSlime::CtorStateBlockCut()
{
    CChrCoreBasket::CtorStateBlockCut();
    SetCollBodyPushSize_(1.1f, 1.1f, 8.0f);
    SetCollBodyPushPos_(0.0f, 0.0f, 8.0f);
}

void msw::CChrCoreBasketSlime::CtorStateSmashCut()
{
    CChrCoreBasket::CtorStateSmashCut();
    SetIllusionWork(55.0f, s_SmashCutIllusion);
    SetCollBodyPushSize_(1.1f, 1.1f, 8.0f);
    SetCollBodyPushPos_(0.0f, 0.0f, 8.0f);
}

// While motion 0x71 plays the body collision is the small one.
void msw::CChrCoreBasketSlime::LoopStateSmashCut()
{
    UpDateIllusionWork(FsmqGetWork<STATE_WORK_SMASH_CUT>()->m_fUnk10);
    CChrCoreBasket::LoopStateSmashCut();
    if (AnimGetMotID(3, 0) == 0x71) {
        SetCollBodyPushType_(static_cast<CChrCore::BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    }
}

void msw::CChrCoreBasketSlime::DtorStateSmashCut()
{
    CChrCoreBasket::DtorStateSmashCut();
    SetCollBodyPushType_(static_cast<CChrCore::BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
}

void msw::CChrCoreBasketSlime::CtorStateJumpUpper()
{
    CChrCoreBasket::CtorStateJumpUpper();
    SetIllusionWork(77.0f, s_JumpUpperIllusion);
    SetCollBodyPushSize_(1.1f, 1.1f, 8.0f);
    SetCollBodyPushPos_(0.0f, 0.0f, 8.0f);
}

// While motion 0x6C plays the body collision is the small one.
void msw::CChrCoreBasketSlime::LoopStateJumpUpper()
{
    UpDateIllusionWork(FsmqGetWork<STATE_WORK_JUMP_UPPER>()->m_fFrame);
    CChrCoreBasket::LoopStateJumpUpper();
    if (AnimGetMotID(3, 0) == 0x6C) {
        SetCollBodyPushType_(static_cast<CChrCore::BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    }
}

void msw::CChrCoreBasketSlime::DtorStateJumpUpper()
{
    SetCollBodyPushType_(static_cast<CChrCore::BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    CChrCoreBasket::DtorStateJumpUpper();
}

// The three slimes appear on the Slime.
void msw::CChrCoreBasketSlime::SubStateArtsShotStartTrick_(STATE_WORK_AIR_TRICK *pWork)
{
    DeleteSquadFactory_();
    CC16_BASKET::CcSquadFactoryBasket *pFactory =
        new (0x100) CC16_BASKET::CcSquadFactoryBasket(CGame::GetTempParent(), this, s_afSquadSpeed);
    if (pFactory) {
        m_hSquadFactory = pFactory->GetId();
    }
    CChrCoreBasket::SubStateArtsShotStartTrick_(pWork);
    SubStateArtsShotDoGetBallShotTrg_(pWork);
}

// Step 0 waits for the shot frame of the motion and sends the slimes out;
// step 1 waits for the end of the motion. The loop motion starts once,
// when the motion ends.
int msw::CChrCoreBasketSlime::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
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

    if (pWork->m_nUnkA0 < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nUnkA0 = 1;
    }
    return bReady;
}

void msw::CChrCoreBasketSlime::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0xB5, 3);

    const SLIME_COL_SET *pCol = GetArtsEffectUnit(ClampColor(m_nColor));
    Eff_SetPointLightColor(0x13, pCol->m_LightColor0, pCol->m_LightColor1);
    Eff_SetMdlKColorReg(0x13, GX_KCOLOR0, pCol->m_KColor0);
    Eff_SetMdlKColorReg(0x13, GX_KCOLOR1, pCol->m_KColor1);

    if (m_hSquadFactory) {
        CC_CMN::SLIME::CcSquadFactory *pFactory =
            static_cast<CC_CMN::SLIME::CcSquadFactory *>(bcs::CTaskMan::GetTask(m_hSquadFactory));
        if (pFactory && !pFactory->IsDelete()) {
            pFactory->AnimPlay(0, 0, 0.0f);
        }
    }
    PlaySePos(0x487, 0, 0, -1, 0.0f, 1.0f, 0.0f);
}

void msw::CChrCoreBasketSlime::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayLoopMotion_(pWork);
}

void msw::CChrCoreBasketSlime::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x14, 0xBC, 3);

    const SLIME_COL_SET *pCol = GetArtsEffectUnit(ClampColor(m_nColor));
    Eff_SetPointLightColor(0x14, pCol->m_LightColor0, pCol->m_LightColor1);
    Eff_SetMdlKColorReg(0x14, GX_KCOLOR0, pCol->m_KColor0);
    Eff_SetMdlKColorReg(0x14, GX_KCOLOR1, pCol->m_KColor1);
}

// Sends the three slimes out from the Slime to points 120 degrees apart
// round a centre 5.5 in front of the goal, starting at a random angle; the
// slimes shown on the Slime are taken away one by one.
void msw::CChrCoreBasketSlime::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *pWork, long)
{
    nw4r::g3d::ResFile res = GetChrOptResFile();
    if (!res.IsValid()) {
        return;
    }
    nw4r::g3d::ResFile mot = GetChrOptMotResFile();

    bcs::FVEC3 pos;
    pos = GetPos();
    bcs::FVEC3 rot;
    rot = GetRot();
    bcs::FVEC3 goal = GetTargetGoalPos();
    float fGoalRotY = GetTargetGoalYRot();

    float fOrbitRot = 40.0f;
    float fSide = 4.5f;
    float fRadius = 0.5f * (fSide / nw4r::math::SinRad(bcs::DEG_TO_RAD(60.0f)));
    float fRot = WrapRad(bcs::BCS_RandMax(6.2831855f));

    bcs::FVEC3 center;
    bcs::BCS_VecZ_RotY(center, 1.0f + fSide, fGoalRotY);
    center.x += goal.x;
    center.z += goal.z;

    bcs::FVEC3 aTarget[3];
    for (long i = 0; i < 3; i++) {
        bcs::BCS_VecZ_RotY(aTarget[i], fRadius, WrapRad(fRot + bcs::DEG_TO_RAD(i * 120)));
        aTarget[i].x += center.x;
        aTarget[i].z += center.z;
        aTarget[i].y = 0.0f;
    }

    for (long i = 0; i < 3; i++) {
        if (m_hSquadFactory) {
            CC_CMN::SLIME::CcSquadFactory *pFactory =
                static_cast<CC_CMN::SLIME::CcSquadFactory *>(bcs::CTaskMan::GetTask(m_hSquadFactory));
            if (pFactory && !pFactory->IsDelete()) {
                pFactory->KillChild(i);
            }
        }

        CC16_BASKET::CcSlimeSquadBasket *pSquad = new (0x100)
            CC16_BASKET::CcSlimeSquadBasket(CGame::GetTempParent(), pos, rot, i, m_nTeamNo, m_nPlayerNo, res, mot);
        if (pSquad) {
            pSquad->SetTargetOrbit(aTarget[i], GetPosY() + 2.0f, fOrbitRot);
            fOrbitRot += 30.0f;
        }
    }
    PlaySePos(0x4EA, 0, 0, -1, 0.0f, 1.0f, 0.0f);
}

// Every slime shown plays its motion from the start and its effect in
// slot nEffSlot.
void msw::CC16_BASKET::CcSquadFactoryBasket::AnimPlay(long nEffSlot, int bLoop, float fInterFrame)
{
    for (long i = 0; i < SQUAD_NUM; i++) {
        if (m_aTaskId[i]) {
            CChrOptTask *pTask = static_cast<CChrOptTask *>(bcs::CTaskMan::GetTask(m_aTaskId[i]));
            if (pTask && !pTask->IsDelete()) {
                if (s_aSquadAnim[i].nMotId >= 0) {
                    pTask->PlayMotion(s_aSquadAnim[i].nMotId, bLoop, fInterFrame, 0.0f);
                }
                if (s_aSquadAnim[i].nEff >= 0) {
                    pTask->Eff_CreateAttachAuto(nEffSlot, s_aSquadAnim[i].nEff, 3);
                }
            }
        }
    }
}

msw::CC16_BASKET::CcSlimeSquadBasket::~CcSlimeSquadBasket()
{
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
}

long msw::CC16_BASKET::CcSlimeSquadBasket::GetMotID_(long nKind) const
{
    return s_aSlimeMotId[m_nIndex][nKind];
}

// On landing the flying effect gives way to the landing one.
void msw::CC16_BASKET::CcSlimeSquadBasket::CtorStateTouchDown()
{
    CcSlimeSquad::CtorStateTouchDown();
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
    if (m_pMdl) {
        long nEff = s_aSlimeEff[m_nIndex][1];
        if (nEff >= 0) {
            CEffectCtrl::CreateAttachEffectAuto(nEff, m_pMdl, 3);
        }
    }
}

// The vanishing effect plays at the model's "SP_item_00" node.
void msw::CC16_BASKET::CcSlimeSquadBasket::CtorStateDead()
{
    long nEff = s_aSlimeEff[m_nIndex][2];
    if (nEff >= 0 && m_pMdl) {
        long nNode = m_pMdl->GetNodeID("SP_item_00");
        bcs::FMTX34 mtx;
        m_pMdl->GetScnMtxPos(&mtx, nNode > 0 ? nNode : 0, bcs::CScnObj::SCNOBJMTXTYPE_1);
        bcs::FVEC3 pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
        CEffectCtrl::SimpleEffect(nEff, pos, 3, 0, 0);
    }
    CcSlimeSquad::CtorStateDead();
}

float msw::CC16_BASKET::CcSlimeSquadBasket::GetRotateRadius_() const
{
    return 3.0f;
}

float msw::CC16_BASKET::CcSlimeSquadBasket::GetRotateSpeed_() const
{
    return 0.038333334f;
}

float msw::CC16_BASKET::CcSlimeSquadBasket::GetChaseSpeed_() const
{
    return 0.038333334f;
}

float msw::CC16_BASKET::CcSlimeSquadBasket::GetSearchTargetDist_() const
{
    return 5.0f;
}

float msw::CC16_BASKET::CcSlimeSquadBasket::GetGiveupChaseDist_() const
{
    // (the local only keeps 7 ahead of -1 in .rodata, as in the original)
    float fDist = 7.0f;
    UnusedReferences(m_fRadius);
    return fDist;
}
