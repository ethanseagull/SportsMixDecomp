// msw::CChrCoreBasketKinopio (Toad) in the basketball game.
//
// His smash cut puts a mushroom (CC09_BASKET::CcKinopioReceiver) on the
// ground in front of the ball holder of the other team (or ahead of
// himself), and the mushroom is his attack collision while the cut lasts.
// His special shot jumps twice and puts two bouncing mushrooms
// (CC09_BASKET::CBoundMushroom) in front of the goal his team attacks.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGrdMdlEffect.hpp"
#include "game/msw/CMdlEffect.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CcBoundMushroom.hpp"
#include "nw4r/math/triangular.h"

#include <math.h>

namespace msw {
    namespace CC09_BASKET {

        // The mushroom of Toad's smash cut: a ground effect with a body
        // collision that grows from 0.01 to the cut's reach; its units
        // carry the owner's player number. 0x64 bytes. (member names guessed)
        class CcKinopioReceiver : public CC_CMN::CGrdMdlEffect {
            public:
                CcKinopioReceiver(bcs::CTask *pParent, long nEff, const bcs::FVEC3 &pos, const bcs::FVEC3 &rot,
                                  bcs::BCS_Color color, float fMaxRadius, const CChrCore *pOwner)
                    : CGrdMdlEffect(pParent, nEff, pos, rot), m_BodyColl(1, 5, 0, 0)
                {
                    m_fRadius = 0.01f;
                    m_fMaxRadius = fMaxRadius;
                    m_BodyColl.SetPushEnableAll();
                    m_BodyColl.SetTargetFlagAll(0x22);
                    m_BodyColl.SetPushRankAll(5);
                    m_BodyColl.SetPushPowerAll(6);
                    m_BodyColl.SetPosAll(pos);
                    m_BodyColl.SetRadiusAll(fMaxRadius);
                    m_BodyColl.SetBodyPushCallbackAll(CallbackBodyPush, this);
                    m_BodyColl.SetIndexAll(pOwner->GetPlayerNo());
                    m_BodyColl.SetNoHitFlagAll(0);
                    m_BodyColl.SetMyNoHitFlagAll(0);
                    m_BodyColl.SetEnableAll();

                    // The effect's models take the colour of Toad's variant.
                    if (m_hEffect) {
                        CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
                        if (pCluster) {
                            long nMdl = pCluster->GetMdlObjNum();
                            for (long i = 0; i < nMdl; i++) {
                                bcs::CMdlObj *pMdl = pCluster->GetMdlObj(i);
                                if (pMdl) {
                                    pMdl->SetMatTevKColorRGB(GX_KCOLOR0, color);
                                }
                            }
                        }
                    }
                }
                virtual ~CcKinopioReceiver();
                virtual void Main();

                static void CallbackBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                             bcs::FVEC3 *pPush);

            private:
                CBodyCollision m_BodyColl;  // 0x50
                float m_fRadius;            // 0x5C
                float m_fMaxRadius;         // 0x60
        };

        // Toad's bouncing mushroom in the basketball game: it swings back
        // and forth along Z while it turns round its point, and hits only
        // some kinds of body (target flag 4). 0x128 bytes, as its base.
        class CBoundMushroom : public CC_CMN::KINOPIO::CcBoundMushroom {
            public:
                CBoundMushroom(bcs::CTask *pParent, nw4r::g3d::ResFile mdl, nw4r::g3d::ResFile mot,
                               const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo, const bcs::FVEC3 &pos, float fScale,
                               long nCourt, bcs::BCS_Color color, long nPlayer)
                    : CcBoundMushroom(pParent, mdl, mot, pInfo, pos, fScale, nCourt, color, nPlayer)
                {
                    m_BodyColl.ResetTargetFlagAll(4);
                    m_BodyColl.SetHitEffectFrameAll(30.0f);
                }
                virtual ~CBoundMushroom();

                virtual float GetParamScale_() const;
                virtual void SetAppearEffect_();
                virtual void SetDisappearEffect_();
                virtual void MovePos_(float fFrame);
                virtual void CalcPos_();
        };
    }
}

namespace {

    // (guessed helpers) The larger of two values; fValue kept within [fMin,
    // fMax]; an angle brought into [0, 2 pi); degrees to radians; the
    // distance between two points in the XZ plane; r1 - r2 by way of a
    // temporary.
    inline float Max(float fValue, float fMin)
    {
        return fMin < fValue ? fValue : fMin;
    }

    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
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

    inline void FVEC3Diff(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 diff;

        bcs::FVEC3Sub(&diff, &r1, &r2);
        rOut = diff;
    }

    // Stand-in for an unknown user of -1, which the original keeps in
    // .rodata after the mushroom's scale but nothing reads (likely stripped
    // code).
    inline void UnusedReferences(float f)
    {
        if (f == -1.0f) {
        }
    }

    // (name guessed) 70 degrees; nothing in the unit reads it.
    float s_fUnkRange = DegToRad(70.0f);
}

// While the smash cut lasts and the mushroom is there, the attack collision
// is the mushroom: a sphere of the cut's reach at its middle.
void msw::CChrCoreBasketKinopio::UpdateCollAtk_()
{
    if (GetState() == 0x32) {
        CCollAtkUnit *pAtk = CollAtkGetColl_(0);
        CCollUnit *pUnit = pAtk->GetCollUnit();
        pUnit->OffFlag10(1);
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hReceiver);
        if (!pTask || pTask->IsDelete()) {
            return;
        }
        {
            CC09_BASKET::CcKinopioReceiver *pReceiver = static_cast<CC09_BASKET::CcKinopioReceiver *>(pTask);
            float fRadius = FsmqGetWork<STATE_WORK_SMASH_CUT>()->m_fRadius;
            pUnit->OnFlag10(1);
            bcs::FVEC3 pos(pReceiver->GetPos());
            pos.y += 0.5f * fRadius;
            pUnit->SetPos(pos);
            pUnit->SetSize(fRadius, 0.0f, 0.0f);
            pUnit->SetCollType(0);
            pUnit->SetShape(0xE);
            pAtk->m_fDownFrame = GetAtkDownFrame(5);
            pAtk->m_fVanishPower = GetAtkVanishPower(5);
            pAtk->m_nUnk90 = -1;
            pAtk->m_nHitSe = 0x4C2;
        }
    } else {
        CChrCoreBasket::UpdateCollAtk_();
    }
}

msw::CChrCoreBasketKinopio::CChrCoreBasketKinopio()
{
    m_hReceiver = 0;
}

msw::CChrCoreBasketKinopio::~CChrCoreBasketKinopio()
{
}

void msw::CChrCoreBasketKinopio::CtorStateJumpBlock()
{
    Eff_Stop(0xA);
    CChrCoreBasket::CtorStateJumpBlock();
}

void msw::CChrCoreBasketKinopio::CtorStateSmashCut()
{
    CChrCoreBasket::CtorStateSmashCut();
    m_hReceiver = 0;
}

// Once in the cut, the mushroom comes out: nine tenths of the way to the
// ball holder of the other team, or 3 ahead of Toad when there is none.
void msw::CChrCoreBasketKinopio::LoopStateSmashCut()
{
    static const float fDist = 3.0f;

    STATE_WORK_SMASH_CUT *pWork = FsmqGetWork<STATE_WORK_SMASH_CUT>();
    if (m_hReceiver == 0 && pWork->m_fUnk10 >= 0.0f) {
        bcs::FVEC3 pos;
        int bFound = 0;
        CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
        if (pHolder && pHolder->GetTeamNo() != GetTeamNo()) {
            bcs::FVEC3 diff;
            FVEC3Diff(diff, pHolder->GetPos(), GetPos());
            bcs::FVEC3 diffXZ(diff.x, 0.0f, diff.z);
            float fLen = 0.9f * bcs::FVEC3Len(&diffXZ);
            bcs::FVEC3 dir;
            bcs::BCS_VecZ_RotY(dir, 1.0f, GetRotY_ToPosXZ(pHolder->GetPos()));
            pos.x = GetPos().x + dir.x * fLen;
            pos.y = 0.0f;
            pos.z = GetPos().z + dir.z * fLen;
            bFound = 1;
        }
        if (!bFound) {
            bcs::FVEC3 dir;
            bcs::BCS_VecZ_RotY(dir, 1.0f, GetRotY());
            pos.x = GetPos().x + fDist * dir.x;
            pos.y = 0.0f;
            pos.z = GetPos().z + fDist * dir.z;
        }

        bcs::BCS_Color color = CC_DATA::GetChrOptColor_Kinopio(m_nColor);
        pos.y = 0.0f;
        bcs::FVEC3 rot(0.0f, GetRotY_ToPosXZ(pos), 0.0f);
        Eff_CreateSimple(0x649, pos, rot, 3, 0, 0);
        CC09_BASKET::CcKinopioReceiver *pReceiver = new CC09_BASKET::CcKinopioReceiver(
            CGame::GetTempParent(), 0x572, pos, rot, color, pWork->m_fRadius, this);
        if (pReceiver) {
            m_hReceiver = pReceiver->GetId();
        }
    }
    CChrCoreBasket::LoopStateSmashCut();
}

// The mushroom does not move when pushed.
void msw::CC09_BASKET::CcKinopioReceiver::CallbackBodyPush(CCollBodyUnit *, CCollBodyUnit *, void *, bcs::FVEC3 *)
{
}

// The trick is two jumps: a hop of 3 (step 0), then the trick jump (1) up
// to the height in the parameters.
void msw::CChrCoreBasketKinopio::SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    if (nStep == 0) {
        float fHeight = 3.0f;
        float fPosY = GetPosY();
        if (fHeight < fPosY) {
            fHeight = fPosY;
        }
        Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, fHeight, 1.0f + fHeight);
        m_fJumpUnk224 = 0.0f;
        m_fJumpUnk228 = 0.0f;
    } else {
        float fHeight = GetCcParam2_()->m_fArtsJumpHeight;
        float fPosY = GetPosY();
        if (fHeight < fPosY) {
            fHeight = fPosY;
        }
        Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, fHeight, fHeight);
        m_fJumpUnk224 = pWork->m_fJumpUnk;
        m_fJumpUnk228 = 0.0f;
    }
}

// The hop stays where Toad is; the trick jump moves as usual.
void msw::CChrCoreBasketKinopio::SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    if (nStep != 0) {
        CChrCoreBasket::SubStateArtsShotSetTrickMoveParam_(pWork, 0);
    } else {
        pWork->m_TargetPos = GetPos();
        pWork->m_MoveParam.m_nType = 0xB;
        pWork->m_MoveParam.m_fDist = DistXZ(pWork->m_TargetPos, GetPos());
        pWork->m_MoveParam.m_fFrame = 0.0f;
        pWork->m_MoveParam.m_fFrameMax = pWork->m_fTrickFrame;
        pWork->m_fUnk58 = 0.0f;
    }
}

// At the end of the hop the mushrooms come out and the trick jump starts;
// the shot is ready when its motion has ended (a motion just started has
// not, so the fall-through only saves a frame).
int msw::CChrCoreBasketKinopio::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (!IsAnimEnd(3)) {
                break;
            }
            SubStateArtsShotFireBullet_(pWork, 0);
            AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)1, (m_nStatusFlag >> 2) & 1), 0, 0.0f, 0.0f);
            Eff_Stop(0x16);
            Eff_CreateAttachAuto(0x14, 0x8F, 3);
            Eff_CreateAttachAuto(0x17, 0x91, 3);
            PlaySePos(0x440, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            pWork->m_fTrickFrame = AnimGetFrameNum(3);
            SubStateArtsShotSetTrickJumpParam_(pWork, 1);
            SubStateArtsShotSetTrickMoveParam_(pWork, 1);
            pWork->m_nTrickStep++;
            // fall through
        case 1:
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
void msw::CChrCoreBasketKinopio::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x8E, 3);
    Eff_CreateAttachAuto(0x16, 0x90, 3);
    CC_CMN::CMdlEffect *pEffect = new (0x100) CC_CMN::CMdlEffect(CGame::GetTempParent(), 0x572, GetPos(), GetRot(), 0, 3);
    if (pEffect) {
        bcs::BCS_Color color = CC_DATA::GetChrOptColor_Kinopio(m_nColor);
        pEffect->SetMatTevKColorRGB(GX_KCOLOR0, color);
    }
}

void msw::CChrCoreBasketKinopio::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x92, 3);
}

void msw::CChrCoreBasketKinopio::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_Stop(0x17);
    Eff_Stop(0x15);
    Eff_CreateAttachAuto(0x18, 0x93, 3);
}

// Two mushrooms in front of the goal the team attacks: 3.05 out from it and
// 1.25 to the side, then 2.5 further to the other side; the first is
// Toad's.
void msw::CChrCoreBasketKinopio::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    static const float fOffsetX = 1.25f;
    static const float fOffsetZ = 3.05f;

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
    if (!mot.IsValid()) {
        return;
    }

    bcs::BCS_Color color = CC_DATA::GetChrOptColor_Kinopio(m_nColor);
    bcs::FVEC3 pos(GetTargetGoalPos());
    pos.y = 0.0f;

    bcs::FVEC3 ofsZ;
    bcs::BCS_VecZ_RotY(ofsZ, fOffsetZ, GetTargetGoalYRot());
    pos.x += ofsZ.x;
    pos.z += ofsZ.z;

    bcs::FVEC3 ofsX;
    bcs::BCS_VecX_RotY(ofsX, fOffsetX, GetTargetGoalYRot());
    pos.x -= ofsX.x;
    pos.z -= ofsX.z;

    bcs::BCS_VecX_RotY(ofsX, 2.5f, GetTargetGoalYRot());
    for (long i = 0; i < 2; i++) {
        long nPlayer;
        if (i == 0) {
            nPlayer = GetPlayerNo();
        } else {
            nPlayer = -1;
        }
        new (0x100) CC09_BASKET::CBoundMushroom(CGame::GetTempParent(), mdl, mot, pInfo, pos, 0.0f, -1, color,
                                                nPlayer);
        pos.x += ofsX.x;
        pos.z += ofsX.z;
    }
}

msw::CC09_BASKET::CBoundMushroom::~CBoundMushroom()
{
}

void msw::CC09_BASKET::CBoundMushroom::SetAppearEffect_()
{
    CEffectCtrl::SimpleEffect(0x570, m_Pos, 3, 0, 0);
}

void msw::CC09_BASKET::CBoundMushroom::SetDisappearEffect_()
{
    CEffectCtrl::SimpleEffect(0x571, m_Pos, 3, 0, 0);
}

// The swing takes 360 frames, the turn 150; the mushroom spins 4 degrees a
// frame.
void msw::CC09_BASKET::CBoundMushroom::MovePos_(float fFrame)
{
    float fSwingFrame = 360.0f;
    m_fSwing += (1.0f / Max(fSwingFrame, 1.0f)) * fFrame;
    m_fSwing = fmod(m_fSwing, 1.0);
    float fTurnFrame = 150.0f;
    m_fTurn += (1.0f / Max(fTurnFrame, 1.0f)) * fFrame;
    m_fTurn = fmod(m_fTurn, 1.0);
    float fSpin = 4.0f;
    m_fRotY += DegToRad(fSpin) * fFrame;
    m_fRotY = WrapRad(m_fRotY);
}

// Swings 0.1 back and forth along Z and turns round its point at a
// distance of 2, kept inside the court.
void msw::CC09_BASKET::CBoundMushroom::CalcPos_()
{
    bcs::FVEC3 pos(m_BasePos);
    if (m_nCourt >= 0) {
        bcs::FVEC3 ofs = CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
        bcs::FVEC3Add(&pos, &pos, &ofs);
    }

    float fSwing = 0.1f;
    pos.z += fSwing * nw4r::math::SinRad(WrapRad(6.2831855f * m_fSwing));

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
        if (CCollMan::DetectMapHit(pos, hitNormal, fHitTime, from, from, 0.8f * m_fRadius, 5,
                                   (unsigned char)m_nCourt | 0x2000000)) {
            m_Pos = pos;
        }
    }
}

// Grows the collision to the cut's reach, and keeps it at the middle of
// the mushroom.
void msw::CC09_BASKET::CcKinopioReceiver::Main()
{
    CGrdMdlEffect::Main();

    if (!IsDelete()) {
        m_fRadius += 0.05f * GetFrameRateF();
        m_fRadius = Clamp(m_fRadius, 0.01f, m_fMaxRadius);
        m_BodyColl.SetRadiusAll(m_fRadius - 0.05f);
        bcs::FVEC3 pos(m_Pos);
        pos.y += m_fRadius * 0.5f;
        m_BodyColl.SetPosAll(pos);
    }
}

msw::CC09_BASKET::CcKinopioReceiver::~CcKinopioReceiver()
{
}

float msw::CC09_BASKET::CBoundMushroom::GetParamScale_() const
{
    UnusedReferences(m_fRadius);
    return 1.3f;
}
