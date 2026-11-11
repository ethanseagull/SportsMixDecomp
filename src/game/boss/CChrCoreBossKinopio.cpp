// msw::CChrCoreBossKinopio (Toad) in the boss game.
//
// Toad does not dive for a receive: he puts a mushroom (the receiver,
// CC09_BOSS::CcKinopioReceiver) on the ground where the ball comes down, and
// the ball is caught on it.
#include "game/msw/CChrCoreBoss.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGrdMdlEffect.hpp"

namespace msw {
    namespace CC09_BOSS {

        // The mushroom Toad receives the ball on: a ground effect with a
        // body collision that grows from 0.01 to 0.5 and keeps to the court.
        // 0x64 bytes. (member names guessed)
        class CcKinopioReceiver : public CC_CMN::CGrdMdlEffect {
            public:
                CcKinopioReceiver(bcs::CTask *pParent, long nEff, const bcs::FVEC3 &pos, const bcs::FVEC3 &rot,
                                  long nCourt, bcs::BCS_Color color);
                virtual ~CcKinopioReceiver();
                virtual void Main();

                static void CallbackBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                             bcs::FVEC3 *pPush);
                void CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);

            private:
                CBodyCollision m_BodyColl;  // 0x50
                float m_fRadius;            // 0x5C
                long m_nCourt;              // 0x60, the court it stays in (CCollMan::DetectMapHit)
        };
    }
}

namespace {

    // (guessed helper) fValue kept within [fMin, fMax].
    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }
}

msw::CChrCoreBossKinopio::CChrCoreBossKinopio()
{
    m_hReceiver = 0;
}

msw::CChrCoreBossKinopio::~CChrCoreBossKinopio()
{
}

inline int msw::CChrCoreBossKinopio::IsReceiverAlive_()
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

inline void msw::CChrCoreBossKinopio::ReleaseReceiver_()
{
    bcs::CTaskMan::GetTask(m_hReceiver);
    m_hReceiver = 0;
}

void msw::CChrCoreBossKinopio::Reset_()
{
    CChrCoreBoss::Reset_();
    ReleaseReceiver_();
}

// The diving receive: 0 starts the motion, 1 puts the receiver down at the
// motion's trigger frame, 3 waits for the motion and goes on to 4 while the
// receiver is there, 4 waits for the receiver to go. The ball can be caught
// on the receiver while it exists. Returns not zero when the state is over.
int msw::CChrCoreBossKinopio::SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *pWork)
{
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0:
            PlayVoice(0x3D, 0);
            SetMoveMotType(0);
            AnimPlay(3, 0x58, 0, 4.0f, 0.0f);
            pWork->m_fTrgFrame = GetBallShotTrgFrame_(0x58, 8.0f);
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
                CC09_BOSS::CcKinopioReceiver *pReceiver = static_cast<CC09_BOSS::CcKinopioReceiver *>(pTask);
                SetBallCatchEnableWorld_(1.0f, 0.5f, pReceiver->GetPos());
            }
        } else {
            SetBallCatchDisable_();
        }
    }

    if (m_nStatusFlag & 1) {
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

void msw::CChrCoreBossKinopio::DtorStateReceive()
{
    // Not used in this build. A guess at where the original had it: the
    // value is in .rodata between those of the functions before and after.
    float fUnused = 0.23f;

    CChrCoreBoss::DtorStateReceive();
    ReleaseReceiver_();
}

// Puts the receiver where the ball will land, or as far as the parameter
// block allows towards it, kept inside the court.
void msw::CChrCoreBossKinopio::FireReceiver_()
{
    ReleaseReceiver_();

    bcs::FVEC3 pos(GetPos());
    CBall *pBall = GetTargetBall_();
    if (pBall) {
        pBall->GetPosAfterFrame(pos, pBall->GetTouchDownFrame(pBall->GetBgCollRadius()));
        pos.y = 0.0f;
    }

    bcs::FVEC3 dir(0.0f, 0.0f, 0.0f);
    const bcs::FVEC3 &rChrPos = GetPos();
    dir.x = pos.x - rChrPos.x;
    dir.z = pos.z - rChrPos.z;
    float fMaxDist = GetCcParam_()->m_Unk68;
    if (dir.x * dir.x + dir.z * dir.z > fMaxDist * fMaxDist) {
        bcs::BCS_Vec_Normalize(dir, bcs::FVEC3(dir.x, 0.0f, dir.z));
        dir.x *= fMaxDist;
        dir.z *= fMaxDist;
        bcs::FVEC3Add(&pos, &GetPos(), &dir);
        pos.y = 0.0f;
    }

    long nCourt = GetGuardCourtIdx();
    bcs::FVEC3 hitPos(pos);
    bcs::FVEC3 hitNormal;
    float fHitTime;
    bcs::FVEC3 from(pos);
    if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, from, from, 0.2f, 1,
                               (unsigned char)nCourt | 0x2000000)) {
        pos.x = hitPos.x;
        pos.z = hitPos.z;
        pos.y = 0.0f;
    }

    bcs::BCS_Color color = CC_DATA::GetChrOptColor_Kinopio(m_nColor);
    Eff_CreateSimple(0x508, pos, GetRot(), 3, 0, 0);
    CC09_BOSS::CcKinopioReceiver *pReceiver =
        new CC09_BOSS::CcKinopioReceiver(CGame::GetTempParent(), 0x509, pos, GetRot(), GetGuardCourtIdx(), color);
    if (pReceiver) {
        m_hReceiver = pReceiver->GetId();
    }
}

msw::CC09_BOSS::CcKinopioReceiver::CcKinopioReceiver(bcs::CTask *pParent, long nEff, const bcs::FVEC3 &pos,
                                                     const bcs::FVEC3 &rot, long nCourt, bcs::BCS_Color color)
    : CGrdMdlEffect(pParent, nEff, pos, rot), m_BodyColl(1, 5, 2, 0)
{
    m_nCourt = nCourt;
    m_fRadius = 0.01f;
    m_BodyColl.SetTargetFlagAll(0x22);
    m_BodyColl.SetPushRankAll(3);
    m_BodyColl.SetPushPowerAll(5);
    m_BodyColl.SetPosAll(pos);
    m_BodyColl.SetSizeAll(m_fRadius, 1.5f, m_fRadius);
    m_BodyColl.SetBodyPushCallbackAll(CallbackBodyPush, this);
    m_BodyColl.SetPushEnableAll();
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

msw::CC09_BOSS::CcKinopioReceiver::~CcKinopioReceiver()
{
}

// Grows the collision to its full size, and keeps it a little below the
// effect.
void msw::CC09_BOSS::CcKinopioReceiver::Main()
{
    CGrdMdlEffect::Main();

    if (!IsDelete()) {
        m_fRadius += 0.05f * GetFrameRateF();
        m_fRadius = Clamp(m_fRadius, 0.01f, 0.5f);
        m_BodyColl.SetSizeAll(m_fRadius, 1.5f, m_fRadius);
        bcs::FVEC3 pos(m_Pos);
        pos.y -= 0.5f;
        m_BodyColl.SetPosAll(pos);
    }
}

void msw::CC09_BOSS::CcKinopioReceiver::CallbackBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                                          bcs::FVEC3 *pPush)
{
    if (pArg) {
        static_cast<CcKinopioReceiver *>(pArg)->CallbackBodyPush_(pBody, pOther, pPush);
    }
}

// Pushed by a body: moves along the push as far as the court allows, and
// passes what is left of the push back to the other body.
void msw::CC09_BOSS::CcKinopioReceiver::CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther,
                                                           bcs::FVEC3 *pPush)
{
    if (pPush) {
        bcs::FVEC3 from(m_Pos);
        bcs::FVEC3 to;
        to.x = from.x + pPush->x;
        to.z = from.z + pPush->z;
        to.y = from.y;
        bcs::FVEC3 hitPos;
        bcs::FVEC3 hitNormal;
        float fHitTime;
        if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, from, to, m_fRadius, 5,
                                   (unsigned char)m_nCourt | 0x2000000)) {
            hitNormal.x = hitPos.x - to.x;
            hitNormal.z = hitPos.z - to.z;
            pOther->DoBodyPushCallback(pBody, &hitNormal);
        } else {
            pOther->DoBodyPushCallback(pBody, 0);
        }
        m_Pos = hitPos;
        to.x = m_Pos.x - from.x;
        to.z = m_Pos.z - from.z;
        m_BodyColl.AddPosXZAll(to, to);
        m_BodyColl.CalcAABBAll();
    }
}
