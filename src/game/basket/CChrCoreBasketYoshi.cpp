// msw::CChrCoreBasketYoshi: Yoshi in the basketball game.
//
// His smash cut is his own: four motions in a row (0x6E to 0x71, mirrored
// when the model plays flipped), turning towards the ball holder, with the
// second motion held for a time from the parameters. His jump block shows
// an effect that turns with him, and his special shot only adds effects to
// the common one.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CEffectCluster.hpp"

namespace {

    // (guessed helper) Degrees to radians.
    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    // (name guessed) 70 degrees; nothing in the unit reads it.
    float s_fUnkRange = DegToRad(70.0f);
}

const msw::CChrCoreBasketYoshi::YOSHI_COL_SET *msw::CChrCoreBasketYoshi::GetEffectUnit(long nColor)
{
    static const YOSHI_COL_SET s_EffectUnit[4] = {
        { bcs::BCS_Color(0x73, 0xCD, 0x6E, 0xFF), bcs::BCS_Color(0x32, 0x73, 0x19, 0xFF) },
        { bcs::BCS_Color(0xFF, 0xB4, 0xB4, 0xFF), bcs::BCS_Color(0xFF, 0x96, 0xAA, 0xFF) },
        { bcs::BCS_Color(0x96, 0xE6, 0xFF, 0xFF), bcs::BCS_Color(0x46, 0xB9, 0xFF, 0xFF) },
        { bcs::BCS_Color(0xFF, 0xFF, 0x64, 0xFF), bcs::BCS_Color(0xFF, 0xFF, 0x00, 0xFF) },
    };

    return &s_EffectUnit[nColor];
}

msw::CChrCoreBasketYoshi::CChrCoreBasketYoshi()
{
}

msw::CChrCoreBasketYoshi::~CChrCoreBasketYoshi()
{
}

void msw::CChrCoreBasketYoshi::DtorStateJumpShoot()
{
    Eff_Stop(0xA);
    CChrCoreBasket::DtorStateJumpShoot();
}

void msw::CChrCoreBasketYoshi::DtorStateDunk()
{
    Eff_Stop(0xA);
    CChrCoreBasket::DtorStateDunk();
}

void msw::CChrCoreBasketYoshi::CtorStateJumpBlock()
{
    CChrCoreBasket::CtorStateJumpBlock();
    Eff_Stop(0xB);
    Eff_CreateAttach(0xB, 0x61D, m_nEffNodeBlock, 0, 0, 3, 0);
}

// The block effect turns with Yoshi.
void msw::CChrCoreBasketYoshi::LoopStateJumpBlock()
{
    CEffectCluster *pCluster = Eff_GetCluster(0xB);
    if (pCluster) {
        bcs::FVEC3 rot(0.0f, GetRotY(), 0.0f);
        pCluster->SetRotateRad(&rot);
    }
    CChrCoreBasket::LoopStateJumpBlock();
}

void msw::CChrCoreBasketYoshi::DtorStateJumpBlock()
{
    Eff_Stop(0xA);
    CChrCoreBasket::DtorStateJumpBlock();
}

void msw::CChrCoreBasketYoshi::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x76, 3);
    Eff_CreateAttachAuto(0x14, 0x77, 3);
}

void msw::CChrCoreBasketYoshi::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x78, 3);
}

void msw::CChrCoreBasketYoshi::SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotShootBall_(pWork);
    Eff_Stop(0x14);
}

// The attack collision is on for the whole cut and does not hit Yoshi's
// own player.
void msw::CChrCoreBasketYoshi::CtorStateSmashCut()
{
    CChrCoreBasket::CtorStateSmashCut();

    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    STATE_WORK_SMASH_CUT *pWork = FsmqGetWork<STATE_WORK_SMASH_CUT>();
    pWork->m_nStep = 0;
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_fHoldFrame = pParam->m_nSmashCutHoldFrame;

    CCollUnit *pUnit = CollAtkGetColl_(0)->GetCollUnit();
    pUnit->OnFlag10(1);
    unsigned long nBit = 1 << m_nPlayerNo;
    pUnit->SetMyNoHitFlag(nBit);
    pUnit->SetNoHitFlag(nBit);
}

// Step 0 starts the first motion and picks the yaw (towards the ball
// holder, if any). In steps 1 to 3, when the motion has ended (in step 2
// also when the hold time has run out), the step's motion is started, or
// the cut ends if that motion is already playing; step 4 ends the cut.
void msw::CChrCoreBasketYoshi::LoopStateSmashCut()
{
    STATE_WORK_SMASH_CUT *pWork = FsmqGetWork<STATE_WORK_SMASH_CUT>();
    pWork->m_fUnk10 += GetFrameStep();

    if (pWork->m_nStep < 1 || pWork->m_nStep > 4) {
        if (pWork->m_nStep != 0) {
            return;
        }
        AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x6E : 0x6E, 0, 4.0f, 0.0f);
        pWork->m_fRotY = GetRotY();
        const CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
        if (pHolder) {
            pWork->m_fRotY = GetRotY_ToPosXZ(pHolder->GetPos());
        }
        pWork->m_nStep++;
    }

    RotateYRate_(pWork->m_fRotY, 0.2f);
    if (pWork->m_nStep == 2) {
        pWork->m_fHoldFrame -= GetFrameStep();
        if (pWork->m_fHoldFrame < 0.0f) {
            pWork->m_fHoldFrame = 0.0f;
        }
    }

    if (IsAnimEnd(3) || (pWork->m_nStep == 2 && pWork->m_fHoldFrame <= 0.0f)) {
        if (pWork->m_nStep == 4) {
            FsmqNext();
            FsmqRetry();
        } else {
            long motTbl[] = { 0x6E, 0x6F, 0x70, 0x71 };
            long nMotId = motTbl[pWork->m_nStep];
            if (nMotId == AnimGetMotID(3, 0)) {
                pWork->m_nStep++;
                FsmqNext();
                FsmqRetry();
            } else {
                if (m_pMdl->IsAnmFlipPlay(3, 0)) {
                    nMotId = -nMotId;
                }
                AnimPlay(3, nMotId, pWork->m_nStep == 1, 4.0f, 0.0f);
                AnimSetSpeed(3, pWork->m_fAnimSpeed);
                pWork->m_nStep++;
            }
        }
    }
}

void msw::CChrCoreBasketYoshi::DtorStateSmashCut()
{
    CollAtkGetColl_(0)->GetCollUnit()->OffFlag10(1);
}
