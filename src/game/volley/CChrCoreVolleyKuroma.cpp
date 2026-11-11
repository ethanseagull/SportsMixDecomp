// msw::CChrCoreVolleyKuroma (the Black Mage) in the volleyball game.
//
// The Black Mage does not dive for a receive: he throws a spell (the
// receiver, CC17_VOLLEY::CcKuromaReceiver) to where the ball comes down, and
// the ball is caught on it. His air trick puts a lock-on mark
// (CC17_VOLLEY::CcKuromaLockonUnit) on every character of the other team
// and strikes them all a while later (CC17_VOLLEY::CcKuromaMagicMgr). His
// block is a jump that puts a wall effect up over the net.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC4.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollDefUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/math/arithmetic.h"
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

    // (name guessed) Stands in for `r1 - r2` (see bcs::FVEC3Sum): the
    // difference goes to a temporary and is copied float by float.
    inline void FVEC3Diff(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 diff;

        bcs::FVEC3Sub(&diff, &r1, &r2);
        rOut = diff;
    }

    // (guessed) Where a ball is and where it goes. Only the position is
    // read; the original copies both.
    struct BALL_MOVE {
        BALL_MOVE(const msw::CBall *pBall) : pos(pBall->GetPos()), vel(pBall->GetVelocity()) {}

        bcs::FVEC3 pos;
        bcs::FVEC3 vel;
    };

    // The smaller of two values.
    inline float Min(float fValue, float fMax)
    {
        return fValue > fMax ? fMax : fValue;
    }

    // fValue kept within [fMin, fMax].
    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    // The square of the distance between two points seen from above.
    inline float DistXZSq(const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 p1(r1.x, 0.0f, r1.z);
        bcs::FVEC3 p2(r2.x, 0.0f, r2.z);
        return bcs::FVEC3DistSq(&p1, &p2);
    }

    // The length of a vector seen from above.
    inline float LenXZ(const bcs::FVEC3 &r)
    {
        bcs::FVEC3 v(r.x, 0.0f, r.z);
        return bcs::FVEC3Len(&v);
    }

    // The distance between two points seen from above.
    inline float DistXZ(const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 p1(r1.x, 0.0f, r1.z);
        bcs::FVEC3 p2(r2.x, 0.0f, r2.z);
        return bcs::FVEC3Dist(&p1, &p2);
    }
}

namespace msw {
    namespace CC17_VOLLEY {

        class CcKuromaMagicMgr;

        // The mark on one character of the other team during the air
        // trick: an effect that follows the character. 0x48 bytes, kept in
        // its CcKuromaMagicMgr's list. (member names guessed)
        class CcKuromaLockonUnit : public bcs::CTask {
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

        // The air trick's strike: marks every character of the other team
        // and, once started, strikes them all after a wait. 0x48 bytes.
        // (member names guessed)
        class CcKuromaMagicMgr : public bcs::CTask {
            public:
                CcKuromaMagicMgr(bcs::CTask *pParent, long nTeam) : CTask(pParent, -1)
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
        inline CcKuromaLockonUnit::CcKuromaLockonUnit(CcKuromaMagicMgr *pMgr, CChrCore *pChr) : CTask(pMgr, -1)
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
            m_hEffect = CEffectCtrl::CreateAttachEffectAuto(0xB8, pChr->GetMdl(), 3);
        }

        // The Black Mage's spell: it flies from the hand to the target point
        // (faster at the end), stops where it hits the ground or the court
        // edge and stays there for a while. 0x74 bytes. (member names
        // guessed)
        class CcKuromaReceiver : public bcs::CUnivTaskEx {
            public:
                CcKuromaReceiver(bcs::CTask *pParent) : CUnivTaskEx(pParent, 0x17)
                {
                    m_fTime = 0.0f;
                    m_fTimeMax = 1.0f;
                    m_hEffect = 0;
                    m_hSnd = 0;
                    m_nStep = 0;
                    m_bLanded = 0;
                    m_fWait = 180.0f;
                }
                virtual ~CcKuromaReceiver();
                virtual void Main();

                // Starts the flight from rFrom to rTo, fFrame frames long.
                void Fire(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fFrame)
                {
                    m_fTime = 0.0f;
                    m_fTimeMax = 1.0f < fFrame ? fFrame : 1.0f;
                    m_From = rFrom;
                    m_To = rTo;
                    m_Pos = rFrom;
                    m_hEffect = CEffectCtrl::CreateAttachEffect(0x6B3, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
                    UpdateEffect_();
                }

                const bcs::FVEC3 &GetPos() const { return m_Pos; }
                // Not zero once it is on the ground.
                int IsLanded() const { return m_bLanded; }

            private:
                // The effect at the spell's place, turned along its flight.
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
        };
    }
}

msw::CChrCoreVolleyKuroma::CChrCoreVolleyKuroma()
{
    m_hMagic = 0;
    m_hReceiver = 0;
}

msw::CChrCoreVolleyKuroma::~CChrCoreVolleyKuroma()
{
}

inline int msw::CChrCoreVolleyKuroma::IsReceiverAlive_()
{
    if (m_hReceiver == 0) {
        return 0;
    }
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hReceiver);
    if (pTask && !pTask->IsDelete()) {
        return 1;
    }
    m_hReceiver = 0;
    return 0;
}

inline void msw::CChrCoreVolleyKuroma::ReleaseMagic_()
{
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hMagic);
    if (pTask && !pTask->IsDelete()) {
        pTask->Delete();
    }
    m_hMagic = 0;
}

inline void msw::CChrCoreVolleyKuroma::ReleaseReceiver_()
{
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hReceiver);
    if (pTask && !pTask->IsDelete()) {
        pTask->Delete();
    }
    m_hReceiver = 0;
}

inline void msw::CChrCoreVolleyKuroma::VanishReceiver_()
{
    CC17_VOLLEY::CcKuromaReceiver *pReceiver =
        static_cast<CC17_VOLLEY::CcKuromaReceiver *>(bcs::CTaskMan::GetTask(m_hReceiver));
    if (pReceiver && !pReceiver->IsDelete() && !pReceiver->IsLanded()) {
        CEffectCtrl::SimpleEffect(0x6B4, pReceiver->GetPos(), 3, 0, 0);
        pReceiver->Delete();
    }
    m_hReceiver = 0;
}

void msw::CChrCoreVolleyKuroma::Reset_()
{
    CChrCoreVolley::Reset_();
    ReleaseMagic_();
    ReleaseReceiver_();
}

// The block: a jump with its own motion.
void msw::CChrCoreVolleyKuroma::SubStateBlockJumpActStart_(STATE_WORK_BLOCK_JUMP *pWork)
{
    pWork->m_fActStartFrame = 0.0f;
    pWork->m_fActTrgFrame = GetBallShotTrgFrame_(0x3E, 0.0f);
    SE_PlayWrapAct(0xB, 0, -1);
    AnimPlay(3, 0x3E, 0, 4.0f, 0.0f);
}

// Not zero once the block motion is over. At its trigger frame the wall
// comes up over the net where the ball will cross it (inside the court),
// for the time in the parameters.
int msw::CChrCoreVolleyKuroma::SubStateBlockJumpIsActEnd_(STATE_WORK_BLOCK_JUMP *pWork)
{
    if (pWork->m_fRotY >= 0.0f) {
        RotateYRate_(pWork->m_fRotY, 0.2f);
    }

    float fFrame = AnimGetFrame(3);
    if (fFrame >= pWork->m_fActTrgFrame + GetCcParam_()->m_Unk78) {
        if (pWork->m_nFlag & 2) {
            pWork->m_nFlag &= ~2;
        }
    } else if (fFrame >= pWork->m_fActTrgFrame && !(pWork->m_nFlag & 8) && !(pWork->m_nFlag & 2)) {
        pWork->m_nFlag |= 2;
        GetTransNodePos(pWork->m_EffOfs);
        pWork->m_EffOfs.y = 0.0f;
        bcs::FVEC3 netPos(CStageMan::GetInstance()->GetNetPos());
        pWork->m_EffOfs.z = netPos.z;

        CBall *pBall = GetTargetBall_();
        if (pBall) {
            pWork->m_EffOfs.x = pBall->GetPos().x;
            pWork->m_EffOfs.y = pBall->GetPos().y;
            if (pBall->IsShootMode() && !pBall->IsFlag1()) {
                BALL_MOVE move(pBall);
                float fLandFrame = pBall->GetTouchDownFrame(pBall->GetBgCollRadius());
                bcs::FVEC3 landPos;
                pBall->GetPosAfterFrame(landPos, fLandFrame);
                bcs::FVEC3 point(pWork->m_EffOfs);
                bcs::FVEC3 normal(0.0f, 0.0f, 1.0f);
                if (GetTargetCourtIdx()) {
                    normal.z = -1.0f;
                } else {
                    normal.z = 1.0f;
                }
                bcs::FVEC3 hitPos;
                float fTime =
                    CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(hitPos, normal, point, move.pos, landPos);
                if (fTime > 0.0f && fTime < 1.0f) {
                    pWork->m_EffOfs.x = hitPos.x;
                    pBall->GetPosAfterFrame(landPos, fLandFrame * fTime);
                    pWork->m_EffOfs.y = landPos.y;
                }
            }
        }

        bcs::FVEC4 rect;
        Util_GetGuardCourtRect_(rect);
        pWork->m_EffOfs.x = Clamp(pWork->m_EffOfs.x, rect.x, rect.y);

        bcs::FVEC3 pos;
        bcs::BCS_VecZ_RotY(pos, GetCcParam_()->m_Unk84, GetBlockTargetRot_());
        bcs::FVEC3Add(&pos, &pos, &pWork->m_EffOfs);
        pos.y = 0.0f;
        CEffectCtrl::SimpleEffect(0x6B2, pos, 3, 0, 0);
    }
    return IsAnimEnd(3) != 0;
}

int msw::CChrCoreVolleyKuroma::SubStateBlockJumpActEnd_(STATE_WORK_BLOCK_JUMP *pWork)
{
    pWork->m_nFlag &= ~2;
    return 1;
}

// A ball hit on the block ends it with its own motion.
void msw::CChrCoreVolleyKuroma::SubStateBlockJumpBlockHit_(const CCollDefUnit *)
{
    if (GetState() == 5) {
        STATE_WORK_BLOCK_JUMP *pWork = static_cast<STATE_WORK_BLOCK_JUMP *>(FsmqGetWork_());
        pWork->m_nFlag = (pWork->m_nFlag & ~6) | 8;
    }
    m_fBlockUnk888 = 0.0f;
    m_fBlockUnk88C = 0.0f;
    AnimPlay(3, 0x3F, 0, 4.0f, 0.0f);
    DriverSendMessage((CChrDrv::MSG_ID)0x53, 5, 0, 0, 0);
}

void msw::CChrCoreVolleyKuroma::SubStateBlockJumpCollSet_(STATE_WORK_BLOCK_JUMP *pWork)
{
    // Not used in this build. A guess at where the original had it: the
    // value is in .rodata between those of the block and receive functions
    // (the other characters' units have the same one).
    float fUnused = 0.23f;

    if (pWork->m_nFlag & 2) {
        UpdateCollDefSetParam_(pWork->m_EffOfs, pWork->m_fCollSize, GetBlockTargetRot_());
    }
}

// The diving receive: 0 starts the motion, 1 throws the receiver at the
// motion's trigger frame, 3 waits for the motion and goes on to 4 while the
// receiver is there, 4 waits for the receiver to go. The ball can be caught
// on the receiver while it exists (further once it is on the ground).
// Returns not zero when the state is over.
int msw::CChrCoreVolleyKuroma::SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *pWork)
{
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0:
            SE_PlayWrapAct(0xD, 0, -1);
            PlayVoice(0x3D, 0);
            SetMoveMotType(0);
            AnimPlay(3, 0x1E, 0, 4.0f, 0.0f);
            pWork->m_fTrgFrame = 3.0f;
            RotateYRate_(pWork->m_fRotY, 0.2f);
            pWork->m_nStep = 1;
            break;
        case 1:
            RotateYRate_(pWork->m_fRotY, 0.2f);
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fTrgFrame) {
                FireReceiver_();
                pWork->m_nStep = 3;
            }
            break;
        case 3:
            RotateYRate_(pWork->m_fRotY, 0.2f);
            if (IsAnimEnd(3)) {
                if (IsReceiverAlive_()) {
                    AnimPlay(3, 0x20, 1, 4.0f, 0.0f);
                    pWork->m_nStep = 4;
                } else {
                    pWork->m_nStep = 5;
                    bEnd = 1;
                }
            }
            break;
        case 4:
            RotateYRate_(pWork->m_fRotY, 0.2f);
            if (!IsReceiverAlive_()) {
                bEnd = 1;
            }
            break;
        default:
            bEnd = 1;
            break;
    }

    if (!(m_nStatusFlag & 1)) {
        if (IsReceiverAlive_()) {
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hReceiver);
            if (pTask && !pTask->IsDelete()) {
                CC17_VOLLEY::CcKuromaReceiver *pReceiver = static_cast<CC17_VOLLEY::CcKuromaReceiver *>(pTask);
                if (pReceiver->IsLanded()) {
                    SetBallCatchEnableWorld_(1.0f, 0.5f, pReceiver->GetPos());
                } else {
                    SetBallCatchEnableWorld_(0.5f, pReceiver->GetPos());
                }
            }
        } else {
            SetBallCatchDisable_();
        }
    }

    if (m_nStatusFlag & 1) {
        VanishReceiver_();
        long nType = 0;
        if (pWork->m_nFlag & 2) {
            nType = 1;
        }
        CBall *pBall = GetAttachBall();
        if (pBall) {
            unsigned long nReact = pBall->GetVolleyReceiveReaction(3, GetCcParam_()->m_Unk04);
            pBall->GetVolleyFlickDownPower(3);
            long nCatch = 0;
            nReact = SubStateReceiveDoCheckDiveReactFlag_(pWork, pBall, nReact);
            if (nReact & 0x100) {
                nCatch = (unsigned char)nReact;
            }
            if (nReact & (0x10000 | 0x8000 | 0x1000)) {
                if (nReact & 0x8000) {
                    nType = 3;
                } else {
                    nType = 2;
                    if (nReact & 0x10000) {
                        nType = 4;
                    }
                }
            }
            pBall->TrgVolleyReceiveReaction(0, 1);
            ReceiveBall_(3, nCatch, -1.0f, nType, pWork->m_nReceiveType, (pWork->m_nFlag >> 8) & 1);
            ReleaseReceiver_();
        }
    }

    m_afCountWork[0] = pWork->m_afCountWork[0];
    m_afCountWork[1] = pWork->m_afCountWork[1];
    return bEnd;
}

void msw::CChrCoreVolleyKuroma::DtorStateReceive()
{
    // Not used here. A guess: in .rodata the value comes between those of
    // the receive state and FireReceiver_ (it is the wait CcKuromaMagicMgr
    // gets in SubStateAirTrickIsReadyArtsShot_).
    float fUnused = 10.0f;

    CChrCoreVolley::DtorStateReceive();
    ReleaseReceiver_();
}

void msw::CChrCoreVolleyKuroma::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
    ArtsOptCreateAttachEffect(0, 0x6B6, "SP_item_02", 0, 1, 3);
}

// Throws the receiver 4 ahead, or to where the ball will be when the spell
// gets there (aiming a little early, and not faster than MAX_SPEED per
// frame), kept inside the court.
void msw::CChrCoreVolleyKuroma::FireReceiver_()
{
    static const float MAX_SPEED = 0.33333334f;

    VanishReceiver_();

    CC17_VOLLEY::CcKuromaReceiver *pReceiver = new CC17_VOLLEY::CcKuromaReceiver(CGame::GetTempParent());
    if (pReceiver) {
        bcs::FVEC3 from(GetPos());
        from.y += 1.0f;
        bcs::FVEC3 to;
        bcs::BCS_VecZ_RotY(to, 4.0f, GetRotY());
        to.x += from.x;
        to.z += from.z;
        float fFrame = ceil(4.0f / MAX_SPEED);

        CBall *pBall = GetTargetBall_();
        if (pBall) {
            fFrame = pBall->GetTouchDownFrame(pBall->GetBgCollRadius());
            if (fFrame >= 0.0f) {
                pBall->GetPosAfterFrame(to, fFrame);
                if (fFrame >= 60.0f && DistXZSq(to, from) < 0.25f) {
                    fFrame = 30.0f;
                    pBall->GetPosAfterFrame(to, fFrame);
                    to.y = 0.0f;
                }
                if (fFrame >= 4.0f) {
                    fFrame -= 2.0f;
                }
            }
            // Kept inside the court.
            long nCourt = GetGuardCourtIdx();
            bcs::FVEC3 hitFrom(to);
            bcs::FVEC3 hitPos(to);
            bcs::FVEC3 hitNormal;
            float fHitTime;
            if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, hitFrom, hitFrom, 0.2f, 1,
                                       (unsigned char)nCourt | 0x2000000)) {
                to.x = hitPos.x;
                to.z = hitPos.z;
                to.y = 0.0f;
            }

            float fDist = DistXZ(to, from);
            if (fFrame > 0.0f) {
                if (fDist / fFrame > MAX_SPEED) {
                    fFrame = ceil(fDist / MAX_SPEED);
                }
            } else {
                fFrame = ceil(fDist / MAX_SPEED);
            }
        } else {
            long nCourt = GetGuardCourtIdx();
            bcs::FVEC3 hitFrom(to);
            bcs::FVEC3 hitPos(to);
            bcs::FVEC3 hitNormal;
            float fHitTime;
            if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, hitFrom, hitFrom, 0.2f, 1,
                                       (unsigned char)nCourt | 0x2000000)) {
                to.x = hitPos.x;
                to.z = hitPos.z;
                to.y = 0.0f;
            }
        }

        PlaySePos(0x4EC, 0, 0, -1, 0.0f, 1.0f, 0.0f);
        pReceiver->Fire(from, to, fFrame);
        m_hReceiver = pReceiver->GetId();
    }
}

void msw::CChrCoreVolleyKuroma::DtorStateAirTrick()
{
    CChrCoreVolley::DtorStateAirTrick();
    ReleaseMagic_();
}

// The trick jump goes up to the height in the parameters (or stays at the
// character's height when that is higher).
void msw::CChrCoreVolleyKuroma::SubStateAirTrickSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    float fHeight = GetCcParam_()->m_Unk37C;
    float fPosY = GetPosY();
    if (fHeight < fPosY) {
        fHeight = fPosY;
    }
    Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, fHeight, fHeight);
    m_fJumpUnk224 = pWork->m_fTrickFrame + pWork->m_fJumpUnk;
    m_fJumpUnk228 = 0.0f;
}

// The air trick does not move.
void msw::CChrCoreVolleyKuroma::SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    ClearMoveParamWork_(pWork->m_MoveParam);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fUnk20 = 0.0f;
}

float msw::CChrCoreVolleyKuroma::SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return AnimGetResFrameNum(0x75);
}

float msw::CChrCoreVolleyKuroma::SubStateAirTrickGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return GetCcParam_()->m_Unk380;
}

// Not zero once the trick is over. Step 0 is the jump: at its end the spell
// motion starts and the strike on the marked characters is set off; step 1
// waits for that motion and goes on to the loop motion.
int msw::CChrCoreVolleyKuroma::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
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
                PlaySePos(0x4F1, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xB6, 3);
                ArtsOptCreateAttachEffectAuto(1, 0xB7, 3);
                CC17_VOLLEY::CcKuromaMagicMgr *pMagic =
                    static_cast<CC17_VOLLEY::CcKuromaMagicMgr *>(bcs::CTaskMan::GetTask(m_hMagic));
                if (pMagic && !pMagic->IsDelete()) {
                    pMagic->Start(10.0f);
                }
                pWork->m_fTrickCount = 0.0f;
                pWork->m_fTrickFrame = 60.0f + AnimGetResFrameNum(0x76);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
            pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);
            if (IsAnimEnd(3)) {
                pWork->m_fAnimSpeed = 1.0f;
                SubStateAirTrickPlayLoopMotion_(pWork);
                pWork->m_nArtsLevel = 1;
                SubStateAirTrickSetTrickJumpParam_(pWork, 1);
                pWork->m_nTrickStep++;
            }
            break;
        default:
            pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
            pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);
            SubStateAirTrickDoJumpMove_(pWork);
            if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
                bReady = 1;
            }
            break;
    }
    return bReady;
}

// The trick motion: its effects and sound, the option model, and the marks
// on the other team.
void msw::CChrCoreVolleyKuroma::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0xB4, 3);
    PlaySePos(0x48D, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    ArtsOptCreate(1, 0, 1);
    ArtsOptCreateAttachEffectAuto(0, 0xB5, 3);
    ArtsOptFadeIn(5.0f, 10.0f, 1);

    ReleaseMagic_();
    CC17_VOLLEY::CcKuromaMagicMgr *pMagic =
        new (0) CC17_VOLLEY::CcKuromaMagicMgr(CGame::GetTempParent(), GetTeamNo());
    m_hMagic = pMagic->GetId();
}

void msw::CChrCoreVolleyKuroma::SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLoopMotion_(pWork);
    ArtsOptCreateAttachEffectAuto(2, 0xB9, 3);
}

void msw::CChrCoreVolleyKuroma::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0xBB, 3);
    ArtsOptStopEffect(2);
    ArtsOptCreateAttachEffectAuto(3, 0xBA, 3);
    ArtsOptSetFadeOutSE(0x48F);
    ArtsOptFadeOut(5.0f, 59.0f, 1);
}

void msw::CChrCoreVolleyKuroma::SubStateAirTrickPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLandMotion_(pWork);
    ArtsOptFadeOut(5.0f, 0.0f, 0);
}

void msw::CChrCoreVolleyKuroma::SubStateAirTrickPlayLandEffect_(STATE_WORK_AIR_TRICK *)
{
    Eff_CreateAttachAuto(0x16, 0xBC, 3);
    SE_PlayArtsLandSE();
}

msw::CC17_VOLLEY::CcKuromaMagicMgr::~CcKuromaMagicMgr()
{
    DeleteAllChild();
    nw4r::ut::List_Init(&m_List, 0x34);
}

// Strikes every marked character still in the game when the wait is over:
// an effect at its feet, a sound and a hit that knocks it down.
void msw::CC17_VOLLEY::CcKuromaMagicMgr::Main()
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
                    CEffectCtrl::SimpleEffect(0x62A, pos, 3, 0, 0);
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

msw::CC17_VOLLEY::CcKuromaLockonUnit::~CcKuromaLockonUnit()
{
    if (m_pMgr) {
        m_pMgr->Remove(this);
    }
    if (m_hEffect) {
        CEffectCtrl::StopAttachEffect(m_hEffect);
    }
}

// Keeps the mark on the character.
void msw::CC17_VOLLEY::CcKuromaLockonUnit::Main()
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

msw::CC17_VOLLEY::CcKuromaReceiver::~CcKuromaReceiver()
{
    StopSnd_();
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
}

// Flies along the path until the time is up or it hits something, then
// puts the landing effect down and waits before it goes.
void msw::CC17_VOLLEY::CcKuromaReceiver::Main()
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
            bcs::FVEC3 pos(m_Pos);
            pos.y = 0.0f;
            CEffectCtrl::SimpleEffect(0x6B5, pos, 3, 0, 0);
            m_fWait = 30.0f;
            m_bLanded = 1;
            m_nStep++;
        }
    } else if (m_fWait <= 0.0f) {
        Delete();
    } else {
        m_fWait -= GetFrameRateF();
    }
}
