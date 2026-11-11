// msw::CChrCoreBossKuroma (the Black Mage) in the boss game.
//
// The Black Mage does not dive for a receive: he throws a spell (the
// receiver, CC17_BOSS::CcKuromaReceiver) to where the ball comes down, and
// the ball is caught on it.
#include "game/msw/CChrCoreBoss.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CUnivTask.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"

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
    namespace CC17_BOSS {

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
                    m_hEffect = CEffectCtrl::CreateAttachEffect(0x525, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
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

msw::CChrCoreBossKuroma::CChrCoreBossKuroma()
{
    m_hReceiver = 0;
}

msw::CChrCoreBossKuroma::~CChrCoreBossKuroma()
{
}

inline int msw::CChrCoreBossKuroma::IsReceiverAlive_()
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

inline void msw::CChrCoreBossKuroma::ReleaseReceiver_()
{
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hReceiver);
    if (pTask && !pTask->IsDelete()) {
        pTask->Delete();
    }
    m_hReceiver = 0;
}

inline void msw::CChrCoreBossKuroma::VanishReceiver_()
{
    CC17_BOSS::CcKuromaReceiver *pReceiver =
        static_cast<CC17_BOSS::CcKuromaReceiver *>(bcs::CTaskMan::GetTask(m_hReceiver));
    if (pReceiver && !pReceiver->IsDelete() && !pReceiver->IsLanded()) {
        CEffectCtrl::SimpleEffect(0x526, pReceiver->GetPos(), 3, 0, 0);
        pReceiver->Delete();
    }
    m_hReceiver = 0;
}

void msw::CChrCoreBossKuroma::Reset_()
{
    CChrCoreBoss::Reset_();
    ReleaseReceiver_();
}

// The diving receive: 0 starts the motion, 1 throws the receiver at the
// motion's trigger frame, 3 waits for the motion and goes on to 4 while the
// receiver is there, 4 waits for the receiver to go. The ball can be caught
// on the receiver while it exists (further once it is on the ground).
// Returns not zero when the state is over.
int msw::CChrCoreBossKuroma::SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *pWork)
{
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0:
            PlayVoice(0x3D, 0);
            SetMoveMotType(0);
            AnimPlay(3, 0x58, 0, 4.0f, 0.0f);
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
                    AnimPlay(3, 0x5A, 1, 4.0f, 0.0f);
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
                CC17_BOSS::CcKuromaReceiver *pReceiver = static_cast<CC17_BOSS::CcKuromaReceiver *>(pTask);
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
            if (nReact & (0x10000 | 0x1000)) {
                nType = 2;
                if (nReact & 0x10000) {
                    nType = 4;
                }
            }
            pBall->TrgVolleyReceiveReaction(0, 1);
            ReceiveBall_(3, nCatch, -1.0f, nType, pWork->m_nReceiveType);
            ReleaseReceiver_();
        }
    }
    return bEnd;
}

void msw::CChrCoreBossKuroma::DtorStateReceive()
{
    CChrCoreBoss::DtorStateReceive();
    ReleaseReceiver_();
}

void msw::CChrCoreBossKuroma::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
}

// Throws the receiver 4 ahead, or to where the ball will be when the spell
// gets there (aiming a little early, and not faster than MAX_SPEED per
// frame), kept inside the court.
void msw::CChrCoreBossKuroma::FireReceiver_()
{
    static const float MAX_SPEED = 0.33333334f;

    VanishReceiver_();

    CC17_BOSS::CcKuromaReceiver *pReceiver = new CC17_BOSS::CcKuromaReceiver(CGame::GetTempParent());
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

        pReceiver->Fire(from, to, fFrame);
        m_hReceiver = pReceiver->GetId();
    }
}

msw::CC17_BOSS::CcKuromaReceiver::~CcKuromaReceiver()
{
    // Not used in this build. A guess at where the original had it: the
    // value is in .rodata between those of FireReceiver_ and Main (Toad's
    // unit has the same one in the same kind of place).
    float fUnused = 0.23f;

    StopSnd_();
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
}

// Flies along the path until the time is up or it hits something, then
// puts the landing effect down and waits before it goes.
void msw::CC17_BOSS::CcKuromaReceiver::Main()
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
            CEffectCtrl::SimpleEffect(0x527, pos, 3, 0, 0);
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
