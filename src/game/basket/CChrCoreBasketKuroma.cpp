// msw::CChrCoreBasketKuroma (the Black Mage) in the basketball game.
//
// His smash cut is a hop backwards while a spell (CC17_BASKET::CcKuromaSmash)
// flies from him to where the ball holder of the other team will be; the
// spell is an attack collision while it flies. His special shot puts a
// lock-on mark (CC17_BASKET::CcKuromaLockonUnit) on every character of the
// other team and strikes them all a while later
// (CC17_BASKET::CcKuromaMagicMgr).
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/New.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/math/triangular.h"
#include "nw4r/ut/List.hpp"

#include <math.h>

namespace {

    // (guessed helpers) An angle brought into [0, 2 pi).
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

    // fValue kept within [fMin, fMax].
    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    // (name guessed) Stands in for `r1 - r2` (see bcs::FVEC3Sum): the
    // difference goes to a temporary and is copied float by float.
    inline void FVEC3Diff(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 diff;

        bcs::FVEC3Sub(&diff, &r1, &r2);
        rOut = diff;
    }

    // The length of a vector seen from above.
    inline float LenXZ(const bcs::FVEC3 &r)
    {
        bcs::FVEC3 v(r.x, 0.0f, r.z);
        return bcs::FVEC3Len(&v);
    }

    // (name guessed) 70 degrees; nothing in the unit reads it.
    float s_fUnkRange = bcs::DEG_TO_RAD(70.0f);
}

namespace msw {
    namespace CC17_BASKET {

        class CcKuromaMagicMgr;

        // The mark on one character of the other team during the special
        // shot: an effect that follows the character. 0x48 bytes, kept in
        // its CcKuromaMagicMgr's list. (member names guessed)
        class CcKuromaLockonUnit : public bcs::CUnivTaskEx {
            public:
                inline CcKuromaLockonUnit(CcKuromaMagicMgr *pMgr, CChrCore *pChr);
                virtual ~CcKuromaLockonUnit();
                virtual void Main();

                // The index of the character marked.
                long GetChrIdx() const { return m_nChr; }
                // The mark stays without its effect.
                void ClearEffect() { m_hEffect = 0; }

            private:
                nw4r::ut::Link m_Link;      // 0x34
                CcKuromaMagicMgr *m_pMgr;   // 0x3C
                unsigned long m_hEffect;    // 0x40
                long m_nChr;                // 0x44, the character index
        };

        // The special shot's strike: marks every character of the other
        // team and, once started, strikes them all after a wait. 0x48
        // bytes. (member names guessed)
        class CcKuromaMagicMgr : public bcs::CUnivTaskEx {
            public:
                CcKuromaMagicMgr(bcs::CTask *pParent, long nTeam) : CUnivTaskEx(pParent, -1)
                {
                    m_nStep = 0;
                    m_fWait = 0.0f;
                    nw4r::ut::List_Init(&m_List, 0x34);

                    long nChr = CChrMan::GetChrCoreNum();
                    for (long i = 0; i < nChr; i++) {
                        CChrCore *pChr = CChrMan::GetChrCore(i);
                        if (!pChr->IsDisable() && nTeam != pChr->GetTeamNo()) {
                            new CcKuromaLockonUnit(this, pChr);
                        }
                    }
                }
                virtual ~CcKuromaMagicMgr();
                virtual void Main();

                // Starts the wait before the strike (once).
                void Start(float fWait)
                {
                    if (m_nStep == 0) {
                        m_fWait = fWait;
                        m_nStep++;
                    }
                }

                void Append(CcKuromaLockonUnit *pUnit) { nw4r::ut::List_Append(&m_List, pUnit); }
                void Remove(CcKuromaLockonUnit *pUnit) { nw4r::ut::List_Remove(&m_List, pUnit); }

            private:
                CcKuromaLockonUnit *GetNext(CcKuromaLockonUnit *pUnit)
                {
                    return static_cast<CcKuromaLockonUnit *>(nw4r::ut::List_GetNext(&m_List, pUnit));
                }

                nw4r::ut::List m_List;      // 0x34, of CcKuromaLockonUnit
                long m_nStep;               // 0x40: 0 marked, 1 waiting, 2 done
                float m_fWait;              // 0x44
        };

        // Puts the mark on pChr.
        inline CcKuromaLockonUnit::CcKuromaLockonUnit(CcKuromaMagicMgr *pMgr, CChrCore *pChr)
            : CUnivTaskEx(pMgr, -1)
        {
            m_pMgr = pMgr;
            m_hEffect = 0;
            m_nChr = -1;
            m_Link.pPrev = 0;
            m_Link.pNext = 0;
            if (pMgr) {
                pMgr->Append(this);
            }
            m_nChr = pChr->GetPlayerNo();
            m_hEffect = CEffectCtrl::CreateAttachEffectAuto(0xC4, pChr->GetMdl(), 3);
        }

        // The spell of the smash cut: it flies from the Black Mage to the
        // target point (faster at the end) as an attack collision, stops
        // where it hits the map or the ground and goes a while later. A hit
        // on a body ends it at once. 0x80 bytes. (member names guessed)
        class CcKuromaSmash : public bcs::CUnivTaskEx {
            public:
                CcKuromaSmash(bcs::CTask *pParent);
                virtual ~CcKuromaSmash();
                virtual void Main();

                // Starts the flight from rFrom to rTo, fFrame frames long;
                // the collision hits no one of pOwner's player.
                void Create(CChrCore *pOwner, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fFrame);

                static void CallbackAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                              const bcs::FVEC3 &rPos, unsigned long flag, void *pArg);

                // Not zero once it is on the ground.
                int IsLanded() const { return m_bLanded; }

            private:
                // The effect at the spell's place, turned along its flight,
                // and the collision with it.
                void UpdateEffect_()
                {
                    if (m_hEffect) {
                        CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
                        if (pCluster) {
                            pCluster->SetTranslate(&m_Pos);
                            bcs::FVEC3 dir;
                            bcs::FVEC3 rot;
                            FVEC3Diff(dir, m_To, m_From);
                            rot.x = WrapRad(nw4r::math::Atan2Rad(LenXZ(dir), dir.y));
                            rot.x = WrapRad(rot.x - 1.5707964f);
                            rot.y = WrapRad(nw4r::math::Atan2Rad(dir.x, dir.z));
                            rot.z = 0.0f;
                            pCluster->SetRotateRad(&rot);
                        }
                        m_AtkColl.SetPosAll(m_Pos);
                    }
                }

                // Stops the sound (none is started in this build).
                void StopSnd_()
                {
                    if (m_hSnd) {
                        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hSnd);
                        m_hSnd = 0;
                        if (pTask && !pTask->IsDelete()) {
                            static_cast<bcs::CSndTask *>(pTask)->Stop(4.0f);
                        }
                    }
                }

                bcs::FVEC3 m_Pos;       // 0x34
                bcs::FVEC3 m_From;      // 0x40
                bcs::FVEC3 m_To;        // 0x4C
                float m_fTime;          // 0x58, frames flown
                float m_fTimeMax;       // 0x5C, frames of the flight
                unsigned long m_hEffect; // 0x60
                unsigned long m_hSnd;   // 0x64, a sound task id
                long m_nStep;           // 0x68: 0 flying, 1 on the ground
                long m_bLanded;         // 0x6C
                float m_fWait;          // 0x70, frames left on the ground
                CAtkCollision m_AtkColl; // 0x74
        };
    }
}

// Out of line, but auto-inlined into its only caller (LoopStateSmashCut);
// the original's .rodata starts with its constants (0, 1, 180), so it was
// defined here, first in the file, and its unused copy left to the link to
// strip.
msw::CC17_BASKET::CcKuromaSmash::CcKuromaSmash(bcs::CTask *pParent) : CUnivTaskEx(pParent, 0x17)
{
    m_fTime = 0.0f;
    m_fTimeMax = 1.0f;
    m_hEffect = 0;
    m_hSnd = 0;
    m_nStep = 0;
    m_bLanded = 0;
    m_fWait = 180.0f;
}

msw::CC17_BASKET::CcKuromaSmash::~CcKuromaSmash()
{
    StopSnd_();
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
}

void msw::CC17_BASKET::CcKuromaSmash::Create(CChrCore *pOwner, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo,
                                             float fFrame)
{
    m_fTime = 0.0f;
    m_fTimeMax = 1.0f < fFrame ? fFrame : 1.0f;
    m_From = rFrom;
    m_To = rTo;
    m_Pos = rFrom;
    m_hEffect = CEffectCtrl::CreateAttachEffect(0x695, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
    UpdateEffect_();

    m_AtkColl.Create(1, 1, 0, 0);
    m_AtkColl.SetAtkTypeAll(0);
    m_AtkColl.SetTargetFlagAll(2);
    m_AtkColl.SetNoHitFlagAll(1 << pOwner->GetPlayerNo());
    m_AtkColl.SetRadiusAll(0.55f);
    m_AtkColl.SetEnableAll();
    m_AtkColl.SetAttackHitCallbackAll(CallbackAttackHit, this);
    m_AtkColl.SetHitEffectIDAll(0x696);
    m_AtkColl.SetIndexAll(pOwner->GetPlayerNo());
}

// A hit on a body while the spell flies ends it.
void msw::CC17_BASKET::CcKuromaSmash::CallbackAttackHit(const CCollAtkUnit *, const CCollBodyUnit *,
                                                        const bcs::FVEC3 &, unsigned long, void *pArg)
{
    // (The original tests the pointer twice: once as the spell, once as
    // the task.)
    CcKuromaSmash *pSmash = static_cast<CcKuromaSmash *>(pArg);
    if (pSmash) {
        bcs::CTask *pTask = pSmash;
        if (pTask && !pTask->IsDelete() && !pSmash->IsLanded()) {
            pTask->Delete();
        }
    }
}

// Flies along the path until the time is up or it hits something, then
// turns its collision off and waits before it goes.
void msw::CC17_BASKET::CcKuromaSmash::Main()
{
    if (m_nStep == 0) {
        int bEnd = 0;
        float fRate;
        m_fTime += GetFrameRateF();
        if (m_fTime >= m_fTimeMax) {
            m_fTime = m_fTimeMax;
            bEnd = 1;
            fRate = 1.0f;
        } else {
            fRate = pow(Clamp(m_fTime / m_fTimeMax, 0.0f, 1.0f), 1.5);
        }

        bcs::FVEC3 from(m_Pos);
        bcs::FVEC3 hitPos;
        bcs::FVEC3 hitNormal;
        float fHitTime;
        bcs::BCS_Vec_Lerp(m_Pos, m_From, m_To, fRate);
        if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, from, m_Pos, 0.0f, 2, 0)) {
            bEnd = 1;
            m_Pos = hitPos;
        }
        if (m_Pos.y <= 0.0f) {
            m_Pos.y = 0.0f;
            bEnd = 1;
        }

        UpdateEffect_();

        if (bEnd) {
            CEffectCtrl::StopAttachEffect(m_hEffect);
            StopSnd_();
            // The landing point; nothing uses it in this build (the
            // receivers of the other modes put an effect there).
            bcs::FVEC3 pos(m_Pos);
            pos.y = 0.0f;
            m_fWait = 30.0f;
            m_bLanded = 1;
            m_AtkColl.SetDisableAll();
            m_nStep++;
        }
    } else {
        m_fWait -= GetFrameRateF();
        if (m_fWait > 0.0f) {
            return;
        }
        Delete();
    }
}

msw::CChrCoreBasketKuroma::CChrCoreBasketKuroma()
{
    m_hMagic = 0;
    m_hSmash = 0;
    m_hEffect = 0;
}

msw::CChrCoreBasketKuroma::~CChrCoreBasketKuroma()
{
}

inline void msw::CChrCoreBasketKuroma::ReleaseMagic_()
{
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hMagic);
    if (pTask && !pTask->IsDelete()) {
        pTask->Delete();
    }
    m_hMagic = 0;
}

void msw::CChrCoreBasketKuroma::Reset_()
{
    CChrCoreBasket::Reset_();
    ReleaseMagic_();
}

// Trigger 0x21 puts the item effect on the option model's "SP_item_02"
// node (in states 0x25 and 0xC, each its own effect); trigger 0x22 takes
// it off again in state 0xC.
void msw::CChrCoreBasketKuroma::CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *pTrg)
{
    CChrCoreBasket::CallbackMotTrg_(pTrg);
    switch (pTrg->m_nType) {
        case 0x21:
            switch (GetState()) {
                case 0x25:
                    if (m_hEffect == 0) {
                        bcs::CMdlObj *pMdl = GetArtsOptMdl();
                        if (pMdl) {
                            nw4r::g3d::ResNode node = pMdl->GetResMdl().GetResNode("SP_item_02");
                            m_hEffect = CEffectCtrl::CreateAttachEffect(0x697, pMdl, 0, node.GetID(), 0, 0, 0, 3,
                                                                        0, 0, 0);
                        }
                    }
                    break;
                case 0xC:
                    if (m_hEffect == 0) {
                        bcs::CMdlObj *pMdl = GetArtsOptMdl();
                        if (pMdl) {
                            nw4r::g3d::ResNode node = pMdl->GetResMdl().GetResNode("SP_item_02");
                            m_hEffect = CEffectCtrl::CreateAttachEffect(0x694, pMdl, 0, node.GetID(), 0, 0, 0, 3,
                                                                        0, 0, 0);
                        }
                    }
                    break;
            }
            break;
        case 0x22:
            if (GetState() == 0xC) {
                CEffectCtrl::StopAttachEffect(m_hEffect);
                m_hEffect = 0;
            }
            break;
    }
}

void msw::CChrCoreBasketKuroma::CtorStateSmashCut()
{
    CChrCoreBasket::CtorStateSmashCut();
    m_hSmash = 0;
}

namespace {

    // The work of the Black Mage's smash cut (the state's work memory, as
    // he uses it; names guessed).
    struct KUROMA_SMASH_CUT_WORK {
        long m_nStep;                  // 0x00, 0 to 3
        unsigned char m_Unk04[0xC - 0x4];
        float m_fRotY;                 // 0x0C, the yaw he faces (he hops the other way)
        float m_fFrame;                // 0x10, frames in the state
        unsigned char m_Unk14[0x18 - 0x14];
        float m_fHoldFrame;            // 0x18, frames of the slide left
        float m_fSpeed;                // 0x1C, speed of the hop per frame
    };
}

// After 25 frames the spell flies (once) to where the ball holder of the
// other team will be when it gets there, at most 7 away, or 3 ahead when
// there is none. Meanwhile he hops backwards (step 0 sets up the hop, 1
// waits for its motion, 2 slides to a stop for 20 frames and lands) and the
// state ends when the landing motion does.
void msw::CChrCoreBasketKuroma::LoopStateSmashCut()
{
    static const float fHeight = 1.0f;
    static const float fOffsetDist = 0.5f;

    KUROMA_SMASH_CUT_WORK *pWork = FsmqGetWork<KUROMA_SMASH_CUT_WORK>();
    if (m_hSmash == 0 && pWork->m_fFrame >= 25.0f) {
        bcs::FVEC3 target;
        float fDist;
        int bFound = 0;
        CChrCoreBasket *pHolder = static_cast<CChrCoreBasket *>(CChrMan::Util_SearchHoldBallChr(0));
        if (pHolder && pHolder->GetTeamNo() != GetTeamNo()) {
            bcs::FVEC3 diff;
            FVEC3Diff(diff, GetPos(), pHolder->GetPos());
            float fFrame = (fOffsetDist + LenXZ(diff)) / 0.3f;
            bcs::FVEC3 future;
            pHolder->GetFuturePos(future, fFrame);
            bcs::FVEC3 dir;
            FVEC3Diff(dir, future, GetPos());
            fDist = fOffsetDist + LenXZ(dir);
            bcs::BCS_Vec_Normalize(dir, bcs::FVEC3(dir.x, 0.0f, dir.z));
            if (fDist > 7.0f) {
                fDist = 7.0f;
            }
            target.x = GetPos().x + dir.x * fDist;
            target.y = 0.0f;
            target.z = GetPos().z + dir.z * fDist;
            bFound = 1;
        }
        if (!bFound) {
            fDist = 3.0f;
            bcs::FVEC3 dir;
            bcs::BCS_VecZ_RotY(dir, 1.0f, GetRotY());
            target.x = GetPos().x + dir.x * fDist;
            target.y = 0.0f;
            target.z = GetPos().z + dir.z * fDist;
        }

        PlaySePos(0x4EC, GetPos(), 0, -1, 0.0f, 1.0f, 0.0f);
        CC17_BASKET::CcKuromaSmash *pSmash = new CC17_BASKET::CcKuromaSmash(CGame::GetTempParent());
        if (pSmash) {
            bcs::FVEC3 from;
            bcs::FVEC3Sum(from, GetPos(), bcs::FVEC3(0.0f, fHeight, 0.0f));
            pSmash->Create(this, from, target, fDist / 0.3f);
            m_hSmash = pSmash->GetId();
        }
    }

    float fStep = m_fFrameStep;
    pWork->m_fFrame += fStep;
    switch (pWork->m_nStep) {
        case 0: {
            float fPosY = GetPos().y;
            float fTop = fPosY + (1.0f - fPosY) * 0.5f;
            float fFrame = AnimGetResFrameNum(0x6E) / 3.0f;
            Util_SetJumpParam(fFrame, fPosY, 1.0f, fTop);
            pWork->m_fRotY = GetRotY();
            // Back 0.5, up to 3 when the ball holder is right in front.
            float fDist = 0.5f;
            const CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
            if (pHolder && pHolder->GetTeamNo() != GetTeamNo()) {
                pWork->m_fRotY = GetRotY_ToPosXZ(pHolder->GetPos());
                bcs::FVEC3 diff;
                FVEC3Diff(diff, pHolder->GetPos(), GetPos());
                float fRate = Clamp(LenXZ(diff) / 3.0f, 0.0f, 1.0f);
                fDist = 2.5f * (1.0f - fRate) + 0.5f;
            }
            pWork->m_fSpeed = fDist / fFrame;
            AnimPlay(3, 0x6E, 0, 0.0f, 0.0f);
            AnimSetSpeed(3, 3.0f);
            pWork->m_nStep++;
        }
            // fall through
        case 1: {
            bcs::FVEC3 move;
            bcs::BCS_VecZ_RotY(move, 1.0f, 3.1415927f + pWork->m_fRotY);
            float fMove = pWork->m_fSpeed * m_fFrameStep;
            move.x *= fMove;
            move.z *= fMove;
            AddPos(move);
            int bLanded = Util_DoJumpMove(m_fFrameStep);
            if (IsAnimEnd(3) && !bLanded) {
                AnimPlay(3, 0x6F, 0, 0.0f, 0.0f);
                pWork->m_fHoldFrame = 20.0f;
                pWork->m_nStep++;
            }
            break;
        }
        case 2: {
            bcs::FVEC3 move;
            bcs::BCS_VecZ_RotY(move, 1.0f, 3.1415927f + pWork->m_fRotY);
            pWork->m_fSpeed *= 0.93f;
            float fMove = pWork->m_fSpeed * m_fFrameStep;
            move.x *= fMove;
            move.z *= fMove;
            AddPos(move);
            pWork->m_fHoldFrame -= fStep;
            if (pWork->m_fHoldFrame > 0.0f) {
                break;
            }
            pWork->m_fHoldFrame = 0.0f;
            if (!Util_DoJumpMove(m_fFrameStep)) {
                break;
            }
            ArtsOptDestroy();
            AnimPlay(3, 0x71, 0, 4.0f, 0.0f);
            PlayLandSe(0);
            Util_SetEffect_((SET_EFFECT_TYPE_BASKET)0);
            pWork->m_nStep++;
        }
            // fall through
        default:
            Util_DoJumpMove(m_fFrameStep);
            if (IsAnimEnd(3)) {
                FsmqNext();
                FsmqRetry();
            }
            break;
    }
}

void msw::CChrCoreBasketKuroma::CtorStateJumpUpper()
{
    CChrCoreBasket::CtorStateJumpUpper();
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
}

void msw::CChrCoreBasketKuroma::DtorStateJumpUpper()
{
    CChrCoreBasket::DtorStateJumpUpper();
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
}

void msw::CChrCoreBasketKuroma::CtorStateLayUp()
{
    CChrCoreBasket::CtorStateLayUp();
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
}

void msw::CChrCoreBasketKuroma::DtorStateLayUp()
{
    CChrCoreBasket::DtorStateLayUp();
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
}

void msw::CChrCoreBasketKuroma::DtorStateAirTrick()
{
    CChrCoreBasket::DtorStateAirTrick();
    ReleaseMagic_();
}

// Not zero once the trick is over. Step 0 is the jump: at its end the spell
// motion starts and the strike on the marked characters is set off; then
// the trick waits for that motion.
int msw::CChrCoreBasketKuroma::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    if (pWork->m_nTrickStep == 0) {
        pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
        pWork->m_fTrickCount = pWork->m_fTrickCount > pWork->m_fTrickFrame ? pWork->m_fTrickFrame
                                                                         : pWork->m_fTrickCount;
        SubStateArtsShotDoJumpMove_(pWork);
        if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
            AnimPlay(3, 0x7F, 0, 4.0f, 0.0f);
            PlaySePos(0x4F1, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            Eff_CreateAttachAuto(0x14, 0xC2, 3);
            ArtsOptCreateAttachEffectAuto(1, 0xC3, 3);
            CC17_BASKET::CcKuromaMagicMgr *pMagic =
                static_cast<CC17_BASKET::CcKuromaMagicMgr *>(bcs::CTaskMan::GetTask(m_hMagic));
            if (pMagic && !pMagic->IsDelete()) {
                pMagic->Start(10.0f);
            }
            pWork->m_fTrickCount = 0.0f;
            pWork->m_fTrickFrame = AnimGetResFrameNum(0x7F);
            pWork->m_nTrickStep++;
        }
    } else {
        pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
        pWork->m_fTrickCount = pWork->m_fTrickCount > pWork->m_fTrickFrame ? pWork->m_fTrickFrame
                                                                         : pWork->m_fTrickCount;
        if (IsAnimEnd(3)) {
            bReady = 1;
        }
    }
    return bReady;
}

// The trick motion: its effects and sound, the option model, and the marks
// on the other team.
void msw::CChrCoreBasketKuroma::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0xC0, 3);
    PlaySePos(0x48D, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    ArtsOptCreate(1, 0, 1);
    ArtsOptCreateAttachEffectAuto(0, 0xC1, 3);
    ArtsOptFadeIn(5.0f, 10.0f, 1);

    ReleaseMagic_();
    CC17_BASKET::CcKuromaMagicMgr *pMagic =
        new (0) CC17_BASKET::CcKuromaMagicMgr(CGame::GetTempParent(), GetTeamNo());
    m_hMagic = pMagic->GetId();
}

void msw::CChrCoreBasketKuroma::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayLoopMotion_(pWork);
    ArtsOptCreateAttachEffectAuto(2, 0xC5, 3);
}

void msw::CChrCoreBasketKuroma::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0xC7, 3);
    ArtsOptStopEffect(2);
    ArtsOptCreateAttachEffectAuto(3, 0xC6, 3);
    ArtsOptSetFadeOutSE(0x48F);
    ArtsOptFadeOut(5.0f, 59.0f, 1);
}

void msw::CChrCoreBasketKuroma::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayLandMotion_(pWork);
    ArtsOptFadeOut(5.0f, 0.0f, 0);
}

void msw::CChrCoreBasketKuroma::SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK *)
{
    Eff_CreateAttachAuto(0x16, 0xC8, 3);
    SE_PlayArtsLandSE();
}

msw::CC17_BASKET::CcKuromaMagicMgr::~CcKuromaMagicMgr()
{
    DeleteAllChild();
    nw4r::ut::List_Init(&m_List, 0x34);
}

// Strikes every marked character still in the game when the wait is over:
// an effect at its feet, a sound and a hit that knocks it down.
void msw::CC17_BASKET::CcKuromaMagicMgr::Main()
{
    if (m_nStep == 1 && m_fWait > 0.0f) {
        m_fWait -= GetFrameRateF();
        if (m_fWait <= 0.0f) {
            CcKuromaLockonUnit *pUnit = GetNext(0);
            while (pUnit) {
                long nChr = pUnit->GetChrIdx();
                CChrCore *pChr;
                if (nChr >= 0 && nChr < CChrMan::GetChrCoreNum() && (pChr = CChrMan::GetChrCore(nChr)) != 0 &&
                    pChr->IsEnable()) {
                    pUnit->ClearEffect();
                    bcs::FVEC3 pos;
                    pChr->GetTransNodePos(pos);
                    pos.y = 0.0f;
                    CEffectCtrl::SimpleEffect(0x5D4, pos, 3, 0, 0);
                    pChr->PlaySePos(0x48E, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                    pChr->SetHookDamage(0xB0D, pChr->GetRotY(), 0.0f, 220.0f, 0x27, 0.0f, 0, 0, -1, -1);
                }
                pUnit->Delete();
                pUnit = pUnit ? GetNext(pUnit) : 0;
            }
            m_nStep++;
        }
    }
}

msw::CC17_BASKET::CcKuromaLockonUnit::~CcKuromaLockonUnit()
{
    if (m_pMgr) {
        m_pMgr->Remove(this);
    }
    if (m_hEffect) {
        CEffectCtrl::StopAttachEffect(m_hEffect);
    }
}

// Keeps the mark on the character.
void msw::CC17_BASKET::CcKuromaLockonUnit::Main()
{
    CChrCore *pChr;
    if (m_hEffect && m_nChr >= 0 && m_nChr < CChrMan::GetChrCoreNum() &&
        (pChr = CChrMan::GetChrCore(m_nChr)) != 0 && pChr->IsEnable()) {
        CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
        if (pCluster) {
            bcs::FVEC3 pos;
            pChr->GetTransNodePos(pos);
            pCluster->SetTranslate(&pos);
        }
    }
}
