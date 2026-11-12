// msw::CChrCoreVolleyNinja: the ninja in the volleyball game.
//
// His air trick: he jumps to one of three places along his side of the net
// and two clones of himself (CC14_VOLLEY::CcNinjaCloneBody) slide out to
// the other two, 3 units apart. Each clone holds a copy of the ball
// (CC14_VOLLEY::CcNinjaCloneBall) and has a target marker of its own
// (CC14_VOLLEY::CcNinjaCloneMarker) that follows his, turned by 120 degrees
// per place. The clones follow his motions through their states and vanish
// when he spikes. His block jump hides his body for a moment.
//
// Written after DodgeF game/dodge/CcNinjaCloneBody.cpp and BasketF
// game/basket/CChrCoreBasketNinja.cpp (the clone models).
//
// .data (0x1288 in the original): pointer-to-member constants 0x360 (each
// clone class's 18 state functions twice: 0xD8 used by its constructor, 0xD8
// unreferenced, emitted here by the UnusedStateTable_ stand-ins), the small
// vtables 0x118, CChrCoreVolleyNinja's vtable 0xD8C, CState's weak vtable
// 0x18, then the four weak nw4r assert strings (ResNode::GetID's pair first,
// then FSqrt's, 4-aligned). Ours is 0x1289: the same objects, with the weak
// strings in the other order and 8-aligned (merged by the link).
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/CMdlClone.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CChrTargetMarkerVolley.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameSnd.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/g3d/ResNode.hpp"
#include "nw4r/math/triangular.h"

#include <math.h>

// Where the ninja (column 0) and his two clones (columns 1 and 2) stand, in
// steps of 3 units along the net, for the three arrangements
// SubStateAirTrickSetTrickMoveParam_ draws from. Mirrored for the other
// team. The clones' places are relative to his.
static const float s_ClonePosTbl[3][3] = {
    {-1.0f, 1.0f, 2.0f},
    {0.0f, -1.0f, 1.0f},
    {1.0f, -2.0f, -1.0f},
};

namespace {

    const float PI2 = 6.2831855f;

    // The state of the air trick.
    const long STATE_AIR_TRICK = 0x27;

    inline float WrapRad(float rad)
    {
        while (rad > PI2) {
            rad -= PI2;
        }
        while (rad < 0.0f) {
            rad += PI2;
        }
        return rad;
    }

    // The distance between two points in the XZ plane.
    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }

    inline float Clamp(float x, float min, float max)
    {
        return x > max ? max : (x < min ? min : x);
    }

    inline float Min(float a, float b)
    {
        return a > b ? b : a;
    }

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }
}

namespace msw {
    namespace CC14_VOLLEY {

        class CcNinjaCloneBody;

        // The target marker of one clone: shown while the ninja's own
        // marker is, at the point the clone spikes at.
        class CcNinjaCloneMarker : public CChrTargetMarkerVolley {
            public:
                CcNinjaCloneMarker(CcNinjaCloneBody *pBody, long nChr);
                virtual ~CcNinjaCloneMarker() {}

            protected:
                // (declared in this order: the weak copies come out in
                // reverse order of declaration, CheckAppear_ first in the
                // original)
                virtual void GetTargetPos_(bcs::FVEC3 &rPos);
                virtual int CheckAppear_();

                CcNinjaCloneBody *m_pBody; // 0xD8
        };

        // The copy of the ball a clone holds, 0xB8 bytes: a clone of the
        // ball's model that moves from where the ball was into the clone's
        // hand. bcs::CFsm at 0x34 (vptr at 0x78). Member names are guessed.
        class CcNinjaCloneBall : public bcs::CTask, public bcs::CFsm<CcNinjaCloneBall> {
                friend class CcNinjaCloneBody;
                friend class msw::CChrCoreVolleyNinja;

            public:
                // The same states as the clone's (the clone changes both).
                enum {
                    STATE_APPEAR,
                    STATE_LOOP,
                    STATE_ARTS_LOOP,
                    STATE_READY,
                    STATE_IMPACT,
                    STATE_DISAPPEAR,
                    STATE_NUM
                };

                // pMdl: the clone's model; rOfs: the ball's place in its
                // hand; pBall: the ball copied.
                CcNinjaCloneBall(bcs::CTask *pParent, bcs::CMdlObj *pMdl, const bcs::FVEC3 &rOfs, CBall *pBall);
                virtual ~CcNinjaCloneBall();
                virtual void Main();

            private:
                void CreateMdl_(CBall *pBall);
                // uncalled stand-in (see its definition)
                void UnusedStateTable_();
                // Moves the copy from where the ball was towards the
                // clone's hand by m_fRate.
                void UpdatePos_();

                void CtorStateAppear();
                void LoopStateAppear();
                void DtorStateAppear();
                void CtorStateLoop();
                void LoopStateLoop();
                void DtorStateLoop();
                void CtorStateArtsLoop();
                void LoopStateArtsLoop();
                void DtorStateArtsLoop();
                void CtorStateReady();
                void LoopStateReady();
                void DtorStateReady();
                void CtorStateImpact();
                void LoopStateImpact();
                void DtorStateImpact();
                void CtorStateDisappear();
                void LoopStateDisappear();
                void DtorStateDisappear();

                bcs::CMdlClone *m_pMdl;       // 0x7C
                bcs::CMdlShadow *m_pShadow;   // 0x80
                bcs::CMdlRef *m_pRef;         // 0x84
                bcs::CMdlObj *m_pParentMdl;   // 0x88, the clone's model
                unsigned long m_Unk8C;        // 0x8C (cleared, never read here)
                float m_fDisappear;           // 0x90, STATE_DISAPPEAR: 0 to 1
                float m_fRate;                // 0x94, 0 at the ball, 1 in the hand
                bcs::FVEC3 m_StartPos;        // 0x98, where the ball was
                bcs::FVEC3 m_Ofs;             // 0xA4, the place in the hand
                unsigned long m_hEffect;      // 0xB0
                unsigned long m_hEffect2;     // 0xB4
        };

        // One clone: a copy of the ninja's model, 0xB8 bytes. bcs::CFsm at
        // 0x34 (vptr at 0x78). Member names are guessed.
        class CcNinjaCloneBody : public bcs::CTask, public bcs::CFsm<CcNinjaCloneBody> {
                friend class CcNinjaCloneMarker;
                friend class msw::CChrCoreVolleyNinja;

            public:
                enum {
                    STATE_APPEAR,
                    STATE_LOOP,      // follows the ninja
                    STATE_ARTS_LOOP, // the loop motion of his air trick
                    STATE_READY,     // a puff of smoke on its chest
                    STATE_IMPACT,    // his spike
                    STATE_DISAPPEAR, // shrinks and fades away, then deletes the task
                    STATE_NUM
                };

                // fOfsX: its place beside the ninja in steps of 3 units;
                // pBall, rOfs: the ball it gets a copy of and where the
                // copy goes in its hand.
                CcNinjaCloneBody(bcs::CTask *pParent, CChrCoreVolley *pChr, CBall *pBall, float fOfsX,
                                 const bcs::FVEC3 &rOfs);
                virtual ~CcNinjaCloneBody();
                virtual void Main();

            private:
                void CreateMdl_(CBall *pBall, const bcs::FVEC3 &rOfs);
                // uncalled stand-in (see its definition)
                void UnusedStateTable_();

                void ChangeBallState_(long nState)
                {
                    if (m_pBall) {
                        m_pBall->ChangeState(nState);
                    }
                }

                void StopEffect_()
                {
                    if (m_hEffect) {
                        CEffectCtrl::StopAttachEffect(m_hEffect);
                        m_hEffect = 0;
                    }
                }

                // Turns the model like the ninja and puts it m_fPosRate of
                // its way out from him.
                void UpdatePos_();

                void CtorStateAppear();
                void LoopStateAppear();
                void DtorStateAppear();
                void CtorStateLoop();
                void LoopStateLoop();
                void DtorStateLoop();
                void CtorStateArtsLoop();
                void LoopStateArtsLoop();
                void DtorStateArtsLoop();
                void CtorStateReady();
                void LoopStateReady();
                void DtorStateReady();
                void CtorStateImpact();
                void LoopStateImpact();
                void DtorStateImpact();
                void CtorStateDisappear();
                void LoopStateDisappear();
                void DtorStateDisappear();

                CChrCoreVolley *m_pChr;          // 0x7C
                bcs::CMdlClone *m_pMdl;          // 0x80
                bcs::CMdlShadow *m_pShadow;      // 0x84
                bcs::CMdlRef *m_pRef;            // 0x88
                CcNinjaCloneBall *m_pBall;       // 0x8C
                CcNinjaCloneMarker *m_pMarker;   // 0x90
                float m_fOfsX;                   // 0x94
                float m_fPosRate;                // 0x98, 0 at the ninja, 1 at its place
                unsigned long m_Unk9C;           // 0x9C (cleared, never read here)
                float m_fDisappear;              // 0xA0, STATE_DISAPPEAR: 0 to 1
                unsigned long m_hEffect;         // 0xA4, the effect of the current state
                unsigned long m_hEffect2;        // 0xA8, the effect it appears with
                bcs::FVEC3 m_TargetPos;          // 0xAC, where its target marker is
        };
    }
}

namespace msw {
    namespace CC14_VOLLEY {

        inline CcNinjaCloneMarker::CcNinjaCloneMarker(CcNinjaCloneBody *pBody, long nChr)
            : CChrTargetMarkerVolley(pBody, nChr, 0), m_pBody(pBody)
        {
        }

        // Shown while the ninja's marker is.
        inline int CcNinjaCloneMarker::CheckAppear_()
        {
            if (m_pBody && m_pBody->m_pChr->IsVolleyTargetMarkerActive()) {
                return 2;
            }
            return 3;
        }

        // The clone's target (the marker's own place without a clone).
        inline void CcNinjaCloneMarker::GetTargetPos_(bcs::FVEC3 &rPos)
        {
            rPos.x = 0.0f;
            rPos.y = 0.0f;
            rPos.z = 0.0f;
            if (m_pMdlBase) {
                rPos = m_pMdlBase->m_vTrans;
            }
            if (m_pBody) {
                rPos = m_pBody->m_TargetPos;
            }
        }
    }
}

inline void msw::CChrCoreVolleyNinja::DeleteClone_()
{
    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i]) {
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_ahClone[i]);
            if (pTask) {
                delete pTask;
            }
            m_ahClone[i] = 0;
        }
    }
}

// The clones missing, at their places in the drawn arrangement, each with
// a copy of the ball he holds.
inline void msw::CChrCoreVolleyNinja::CreateClone_()
{
    const float *pPosTbl = s_ClonePosTbl[m_nPosType];
    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i] == 0) {
            float fOfsX = pPosTbl[1 + i];
            if (GetTargetGoalIdx() != 0) {
                fOfsX = -fOfsX;
            }
            CC14_VOLLEY::CcNinjaCloneBody *pClone = new CC14_VOLLEY::CcNinjaCloneBody(
                GetParentTask(), this, GetAttachBall(), fOfsX, m_AdsorbOfs);
            if (pClone) {
                m_ahClone[i] = pClone->GetId();
            }
        }
    }
}

// Changes the state of the clones still there and forgets the others.
inline void msw::CChrCoreVolleyNinja::ChangeStateClone_(long nState)
{
    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i]) {
            CC14_VOLLEY::CcNinjaCloneBody *pClone =
                static_cast<CC14_VOLLEY::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(m_ahClone[i]));
            if (pClone == 0) {
                m_ahClone[i] = 0;
            } else if (pClone->IsDelete()) {
                m_ahClone[i] = 0;
            } else {
                pClone->ChangeState(nState);
            }
        }
    }
}

// Puts the clones' targets at the default places seen from them.
inline void msw::CChrCoreVolleyNinja::ResetCloneTargetPos_()
{
    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i]) {
            CC14_VOLLEY::CcNinjaCloneBody *pClone =
                static_cast<CC14_VOLLEY::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(m_ahClone[i]));
            if (pClone == 0) {
                m_ahClone[i] = 0;
            } else if (pClone->IsDelete()) {
                m_ahClone[i] = 0;
            } else {
                pClone->m_pChr->GetVolleyTargetMarkerDefaultPos(6, pClone->m_pMdl->m_vTrans, pClone->m_TargetPos, 0);
            }
        }
    }
}

msw::CChrCoreVolleyNinja::CChrCoreVolleyNinja()
{
    m_nPosType = 0;
    for (long i = 0; i < CLONE_NUM; i++) {
        m_ahClone[i] = 0;
    }
}

msw::CChrCoreVolleyNinja::~CChrCoreVolleyNinja()
{
    DeleteClone_();
}

void msw::CChrCoreVolleyNinja::Reset_()
{
    CChrCoreVolley::Reset_();
    DeleteClone_();
}

// His own marker, and one per clone during the air trick.
long msw::CChrCoreVolleyNinja::GetVolleyTargetMarkerNum() const
{
    if (!IsVolleyTargetMarkerActive()) {
        return 0;
    }
    long nNum = 1;
    if (GetState() == STATE_AIR_TRICK) {
        if (m_ahClone[0]) {
            nNum++;
        }
        if (m_ahClone[1]) {
            nNum++;
        }
    }
    return nNum;
}

// Marker 0 is his own, 1 and 2 the clones'.
int msw::CChrCoreVolleyNinja::GetVolleyTargetMarkerPos(bcs::FVEC3 &rPos, long nIndex)
{
    if (!IsVolleyTargetMarkerActive()) {
        return 0;
    }
    if (nIndex >= 1 && nIndex <= CLONE_NUM && GetState() == STATE_AIR_TRICK) {
        // (an unsigned index: the original computes nIndex - 1 again here
        // instead of folding the -1 into the field offset)
        unsigned long hClone = m_ahClone[(unsigned long)(nIndex - 1)];
        if (hClone) {
            CC14_VOLLEY::CcNinjaCloneBody *pClone =
                static_cast<CC14_VOLLEY::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(hClone));
            if (pClone && !pClone->IsDelete()) {
                rPos = pClone->m_TargetPos;
                return 1;
            }
        }
    }
    rPos = m_TargetMarkerPos;
    return 1;
}

// When he stands at one end of the row, the middle one is a clone: the
// special shot's effects centre on it.
void msw::CChrCoreVolleyNinja::GetSpRootPos(bcs::FVEC3 &rPos) const
{
    rPos = GetPos();
    if (GetState() == STATE_AIR_TRICK) {
        long nIndex = -1;
        switch (m_nPosType) {
            case 0:
                nIndex = 0;
                break;
            case 2:
                nIndex = 1;
                break;
        }
        if (nIndex >= 0 && m_ahClone[nIndex]) {
            CC14_VOLLEY::CcNinjaCloneBody *pClone =
                static_cast<CC14_VOLLEY::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(m_ahClone[nIndex]));
            if (pClone && !pClone->IsDelete() && pClone->m_pMdl) {
                rPos = pClone->m_pMdl->m_vTrans;
            }
        }
    }
}

void msw::CChrCoreVolleyNinja::DtorStateAirTrick()
{
    CChrCoreVolley::DtorStateAirTrick();
    DeleteClone_();
}

int msw::CChrCoreVolleyNinja::SubStateAirTrickIsTMStartAuto_(STATE_WORK_AIR_TRICK *)
{
    return 0;
}

float msw::CChrCoreVolleyNinja::SubStateAirTrickGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return 0.0f;
}

void msw::CChrCoreVolleyNinja::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x9D, 3);
    Eff_CreateAttachAuto(0x14, 0x9E, 3);
}

void msw::CChrCoreVolleyNinja::SubStateAirTrickSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    Util_SetJumpParam(pWork->m_fTrickFrame, GetPosY(), 4.2f, 5.3f);
    m_fJumpUnk224 = 0.0f;
    m_fJumpUnk228 = 0.0f;
}

// He jumps to his place in the drawn arrangement, 1 unit from the net on
// his side.
void msw::CChrCoreVolleyNinja::SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    float fZ = fabsf(1.0f);
    pWork->m_TargetPos = GetPos();
    if (GetTargetGoalIdx() != 0) {
        fZ = -fZ;
    }
    pWork->m_TargetPos.z = fZ;

    long nPosType = bcs::BCS_RandMax(2L);
    float fX = s_ClonePosTbl[nPosType][0];
    if (GetTargetGoalIdx() != 0) {
        fX = -fX;
    }
    pWork->m_TargetPos.x = 3.0f * fX;

    // 25 frames at normal speed.
    float fFrame = 25.0f;
    if (pWork->m_fAnimSpeed > 0.0f) {
        fFrame = Max(25.0f / pWork->m_fAnimSpeed, 0.0f);
    }

    pWork->m_MoveParam.m_nType = 0xB;
    pWork->m_MoveParam.m_fDist = DistXZ(pWork->m_TargetPos, GetPos());
    pWork->m_MoveParam.m_fFrame = fFrame;
    pWork->m_MoveParam.m_fFrameMax = pWork->m_fTrickFrame;
    m_nPosType = nPosType;
}

float msw::CChrCoreVolleyNinja::SubStateAirTrickGetTrickRotFrame_(STATE_WORK_AIR_TRICK *pWork)
{
    float fFrame = Min(16.0f, AnimGetResFrameNum(0x75));
    if (pWork->m_fAnimSpeed > 0.0f) {
        fFrame /= pWork->m_fAnimSpeed;
    }
    return fFrame;
}

void msw::CChrCoreVolleyNinja::SubStateAirTrickDoBallAdsorb_(STATE_WORK_AIR_TRICK *)
{
}

// Step 0: at frame 25 of his trick motion the clones appear beside him.
// Step 1: the loop motion follows the trick motion; then 60 frames of
// waiting. Until the loop motion the ball rises in his hand and the clones
// slide out to their places.
int msw::CChrCoreVolleyNinja::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
    pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);

    switch (pWork->m_nTrickStep) {
        case 0:
            SubStateAirTrickDoJumpMove_(pWork);
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 25.0f) {
                CreateClone_();
                ChangeStateClone_(CC14_VOLLEY::CcNinjaCloneBody::STATE_LOOP);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            SubStateAirTrickDoJumpMove_(pWork);
            if (pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
                SubStateAirTrickPlayLoopMotion_(pWork);
                pWork->m_nArtsLevel = 1;
                pWork->m_nTrickStep++;
                pWork->m_fTrickFrame = 60.0f;
                pWork->m_fTrickCount = 0.0f;
            }
            break;
        default:
            if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
                bReady = 1;
            }
            break;
    }

    if (pWork->m_nArtsLevel <= 0) {
        CBall *pBall = GetAttachBall();
        if (pBall) {
            bcs::FVEC3 ofs;
            bcs::BCS_Vec_RotY(ofs, m_AdsorbOfs, pWork->m_fRotY);
            float fFrameNum = AnimGetFrameNum(3);
            if (fFrameNum > 0.0f) {
                static const float fHeightMax = 10.0f;
                float fRate = Clamp((1.0f + AnimGetFrame(3)) / fFrameNum, 0.0f, 1.0f);
                float fFrameNum2 = fFrameNum - 25.0f;
                bcs::FVEC3Scale(&ofs, &ofs, fRate);
                float fCloneRate = Clamp((1.0f + (AnimGetFrame(3) - 25.0f)) / fFrameNum2, 0.0f, 1.0f);
                SetCloneBodyPosRate_(fCloneRate, fRate);
                float fHeight = fHeightMax * nw4r::math::CosRad(1.5707964f * fRate);
                ofs.y += Clamp(fHeight, 0.0f, fHeightMax);
            }
            pBall->SetAttachPos(ofs, 0);
        }
    } else {
        CBall *pBall = GetAttachBall();
        if (pBall) {
            SetCloneBodyPosRate_(1.0f, 1.0f);
            bcs::FVEC3 ofs;
            bcs::BCS_Vec_RotY(ofs, m_AdsorbOfs, pWork->m_fRotY);
            pBall->SetAttachPos(ofs, 0);
        }
    }
    return bReady;
}

// The clones take up the loop motion and their markers start at their
// default places.
void msw::CChrCoreVolleyNinja::SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLoopMotion_(pWork);

    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0x9F, 3);
    SE_PlayMove(8, 0x476, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);

    ChangeStateClone_(CC14_VOLLEY::CcNinjaCloneBody::STATE_ARTS_LOOP);
    ResetCloneTargetPos_();
    SubStateAirTrickStartTMDisp_(pWork);
}

void msw::CChrCoreVolleyNinja::SubStateAirTrickReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickReadyArtsShot_(pWork);

    ChangeStateClone_(CC14_VOLLEY::CcNinjaCloneBody::STATE_READY);
}

void msw::CChrCoreVolleyNinja::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);

    Eff_Stop(0x15);
    Eff_CreateAttachAuto(0x16, 0xA0, 3);

    ChangeStateClone_(CC14_VOLLEY::CcNinjaCloneBody::STATE_IMPACT);
}

void msw::CChrCoreVolleyNinja::SubStateAirTrickShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickShootBall_(pWork);

    ChangeStateClone_(CC14_VOLLEY::CcNinjaCloneBody::STATE_DISAPPEAR);
}

// Called by the motion trigger that shows (bVisible != 0) or hides the
// body: a puff of smoke in the block (state 5) and in state 0x2C (meaning
// not known). Bit 0x10000000 of the status flags is set while the body is
// hidden.
void msw::CChrCoreVolleyNinja::CallbackMotTrg_BodyVisible_(long bVisible, long)
{
    if (bVisible != 0) {
        if (!(m_nStatusFlag & 0x10000000)) {
            bcs::FVEC3 pos;
            GetTransNodePos(pos);
            switch (GetState()) {
                case 0x2C:
                    Eff_CreateSimple(0x69F, pos, 3, 0, 0);
                    break;
                case 5:
                    Eff_CreateSimple(0x69D, pos, 3, 0, 0);
                    break;
            }
            m_nStatusFlag |= 0x0C000000;
            m_UnkEC |= 4;
        }
    } else if (m_nStatusFlag & 0x10000000) {
        bcs::FVEC3 pos;
        GetTransNodePos(pos);
        switch (GetState()) {
            case 0x2C:
                Eff_CreateSimple(0x6A0, pos, 3, 0, 0);
                break;
            case 5:
                Eff_CreateSimple(0x69E, pos, 3, 0, 0);
                break;
        }
        m_nStatusFlag &= ~0x0C000000;
        m_UnkEC &= ~4;
    }
}

void msw::CChrCoreVolleyNinja::SetCloneBodyPosRate_(float fRate, float)
{
    fRate = Clamp(fRate, 0.0f, 1.0f);
    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i]) {
            CC14_VOLLEY::CcNinjaCloneBody *pClone =
                static_cast<CC14_VOLLEY::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(m_ahClone[i]));
            if (pClone == 0) {
                m_ahClone[i] = 0;
            } else if (pClone->IsDelete()) {
                m_ahClone[i] = 0;
            } else {
                pClone->m_fPosRate = fRate;
                if (pClone->m_pBall) {
                    pClone->m_pBall->m_fRate = fRate;
                }
            }
        }
    }
}

// The body hides from frame 2 of the block jump to frame 16.
void msw::CChrCoreVolleyNinja::SubStateBlockJumpJumpStart_(STATE_WORK_BLOCK_JUMP *pWork)
{
    CChrCoreVolley::SubStateBlockJumpJumpStart_(pWork);
    pWork->m_fActStartFrame = 2.0f;
    pWork->m_fActTrgFrame = 16.0f;
}

int msw::CChrCoreVolleyNinja::SubStateBlockJumpIsJumpEnd_(STATE_WORK_BLOCK_JUMP *pWork)
{
    Util_DoJumpMoveEX(0);
    if (m_nJumpStep >= 2) {
        return 1;
    }

    if (pWork->m_fRotY >= 0.0f) {
        RotateYRate_(pWork->m_fRotY, 0.2f);
    }
    if (pWork->m_nFlag & 2) {
        float fHeight = GetCcParam_()->m_Unk74;
        if (GetPosY() <= fHeight && m_fJumpVelY < 0.0f) {
            pWork->m_nFlag &= ~6;
        }
    }

    float fFrame = AnimGetFrame(3);
    if (pWork->m_fActStartFrame >= 0.0f) {
        if (fFrame >= pWork->m_fActStartFrame) {
            CallbackMotTrg_BodyVisible_(1, 0);
            m_nStatusFlag |= 0x1C000000;
            m_UnkEC |= 4;
            pWork->m_fActStartFrame = -1.0f;
        }
    } else if (pWork->m_fActTrgFrame >= 0.0f && fFrame >= pWork->m_fActTrgFrame) {
        CallbackMotTrg_BodyVisible_(0, 0);
        m_nStatusFlag &= ~0x1C000000;
        m_UnkEC &= ~4;
        pWork->m_fActTrgFrame = -1.0f;
    }
    return 0;
}

void msw::CChrCoreVolleyNinja::SubStateBlockJumpLandStart_(STATE_WORK_BLOCK_JUMP *pWork)
{
    CChrCoreVolley::SubStateBlockJumpLandStart_(pWork);
    CallbackMotTrg_BodyVisible_(0, 0);
    m_nStatusFlag &= ~0x1C000000;
    m_UnkEC &= ~4;
}

void msw::CChrCoreVolleyNinja::SubStateBlockJumpBlockHit_(const CCollDefUnit *pDef)
{
    CChrCoreVolley::SubStateBlockJumpBlockHit_(pDef);
    CallbackMotTrg_BodyVisible_(0, 0);
    m_nStatusFlag &= ~0x1C000000;
    m_UnkEC &= ~4;
}

namespace msw {
    namespace CC14_VOLLEY {

        CcNinjaCloneBody::CcNinjaCloneBody(bcs::CTask *pParent, CChrCoreVolley *pChr, CBall *pBall, float fOfsX,
                                           const bcs::FVEC3 &rOfs)
            : bcs::CTask(pParent, 0x15)
            , m_pChr(pChr)
            , m_pMdl(0)
            , m_pShadow(0)
            , m_pRef(0)
            , m_pBall(0)
            , m_pMarker(0)
            , m_fOfsX(fOfsX)
            , m_fPosRate(0.0f)
            , m_Unk9C(0)
            , m_fDisappear(0.0f)
            , m_hEffect(0)
            , m_hEffect2(0)
        {
            InitFsm(this, STATE_NUM);
            SetState(STATE_APPEAR, &CcNinjaCloneBody::CtorStateAppear, &CcNinjaCloneBody::LoopStateAppear,
                     &CcNinjaCloneBody::DtorStateAppear);
            SetState(STATE_LOOP, &CcNinjaCloneBody::CtorStateLoop, &CcNinjaCloneBody::LoopStateLoop,
                     &CcNinjaCloneBody::DtorStateLoop);
            SetState(STATE_ARTS_LOOP, &CcNinjaCloneBody::CtorStateArtsLoop, &CcNinjaCloneBody::LoopStateArtsLoop,
                     &CcNinjaCloneBody::DtorStateArtsLoop);
            SetState(STATE_READY, &CcNinjaCloneBody::CtorStateReady, &CcNinjaCloneBody::LoopStateReady,
                     &CcNinjaCloneBody::DtorStateReady);
            SetState(STATE_IMPACT, &CcNinjaCloneBody::CtorStateImpact, &CcNinjaCloneBody::LoopStateImpact,
                     &CcNinjaCloneBody::DtorStateImpact);
            SetState(STATE_DISAPPEAR, &CcNinjaCloneBody::CtorStateDisappear, &CcNinjaCloneBody::LoopStateDisappear,
                     &CcNinjaCloneBody::DtorStateDisappear);

            CreateMdl_(pBall, rOfs);
            m_TargetPos.x = 0.0f;
            m_TargetPos.y = 0.0f;
            m_TargetPos.z = 0.0f;
            m_hEffect = CEffectCtrl::CreateAttachEffectAuto(0x9E, m_pMdl, 3);
            m_hEffect2 = CEffectCtrl::CreateAttachEffectAuto(0x22, m_pMdl, 3);
        }

        // uncalled stand-in: the original .data holds a second, unreferenced
        // copy of this class's state-function pointer constants right after
        // the ones the constructor uses (most likely from code the link
        // removed); this function makes the compiler emit them there. The
        // module link strips it.
        void CcNinjaCloneBody::UnusedStateTable_()
        {
            InitFsm(this, STATE_NUM);
            SetState(STATE_APPEAR, &CcNinjaCloneBody::CtorStateAppear, &CcNinjaCloneBody::LoopStateAppear,
                     &CcNinjaCloneBody::DtorStateAppear);
            SetState(STATE_LOOP, &CcNinjaCloneBody::CtorStateLoop, &CcNinjaCloneBody::LoopStateLoop,
                     &CcNinjaCloneBody::DtorStateLoop);
            SetState(STATE_ARTS_LOOP, &CcNinjaCloneBody::CtorStateArtsLoop, &CcNinjaCloneBody::LoopStateArtsLoop,
                     &CcNinjaCloneBody::DtorStateArtsLoop);
            SetState(STATE_READY, &CcNinjaCloneBody::CtorStateReady, &CcNinjaCloneBody::LoopStateReady,
                     &CcNinjaCloneBody::DtorStateReady);
            SetState(STATE_IMPACT, &CcNinjaCloneBody::CtorStateImpact, &CcNinjaCloneBody::LoopStateImpact,
                     &CcNinjaCloneBody::DtorStateImpact);
            SetState(STATE_DISAPPEAR, &CcNinjaCloneBody::CtorStateDisappear, &CcNinjaCloneBody::LoopStateDisappear,
                     &CcNinjaCloneBody::DtorStateDisappear);
        }

        CcNinjaCloneBody::~CcNinjaCloneBody()
        {
            if (m_hEffect2) {
                CEffectCtrl::StopAttachEffect(m_hEffect2);
                m_hEffect2 = 0;
            }
            CEffectCtrl::StopAttachEffect(m_hEffect);
            m_hEffect = 0;
            if (m_pRef) {
                bcs::CScnObj::AppendDelayDeleteObject(m_pRef);
            }
            m_pRef = 0;
            if (m_pShadow) {
                bcs::CScnObj::AppendDelayDeleteObject(m_pShadow);
            }
            m_pShadow = 0;
            if (m_pMdl) {
                bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
            }
            m_pMdl = 0;
        }

        // A copy of the ninja's model with its shadow and reflection, the
        // copy of the ball and the clone's target marker.
        void CcNinjaCloneBody::CreateMdl_(CBall *pBall, const bcs::FVEC3 &rOfs)
        {
            bcs::CMdlObj *pSrc = m_pChr->GetMdl();
            nw4r::g3d::ResFile file = pSrc->GetResMdl().GetParent();

            nw4r::g3d::ResMdl mdl = file.GetResMdl(0);
            if (mdl.IsValid()) {
                m_pMdl = new (0) bcs::CMdlClone(pSrc, mdl, 0, 0, 0);
                m_pMdl->OnFlag(0x100);
                m_pMdl->OnFlag(2);
                m_pMdl->SetExBlendMode(1);
                m_pMdl->m_vTrans = pSrc->m_vTrans;
                m_pMdl->SetRotXYZ(pSrc->m_fRotX, pSrc->m_fRotY, pSrc->m_fRotZ);
                m_pMdl->CalcRootMtx();
                m_pMdl->CalcWorldMatrixAll(0);
                m_pMdl->OnFlag(0x4000);
                bcs::CRenderMan::PushBack(m_pMdl, 2);
            }

            nw4r::g3d::ResMdl shadow = file.GetResMdl(2);
            if (shadow.IsValid()) {
                m_pShadow = new bcs::CMdlShadow(m_pMdl, shadow, 0x40, 0, 0);
                bcs::CRenderMan::PushBack(m_pShadow, 2);
            }

            nw4r::g3d::ResMdl ref = file.GetResMdl(1);
            if (ref.IsValid()) {
                m_pRef = new bcs::CMdlRef(m_pMdl, ref, 0, 0, 0);
                bcs::CRenderMan::PushBack(m_pRef, 4);
            }

            m_pBall = new (0x100) CcNinjaCloneBall(this, m_pMdl, rOfs, pBall);
            m_pMarker = new CcNinjaCloneMarker(this, m_pChr->GetPlayerNo());
        }

        // Its target follows the ninja's marker, turned by 120 degrees per
        // place.
        void CcNinjaCloneBody::Main()
        {
            ExecState();

            if (m_pMdl) {
                m_pMdl->OffFlag(2);
                m_pMdl->ObjProc(1, 0, 0);
                m_pMdl->OnFlag(2);
            }

            if (m_pChr && m_pChr->IsVolleyTargetMarkerActive()) {
                bcs::FVEC3 move;
                m_pChr->GetVolleyTargetMarkerMoved(move);
                float fRot = WrapRad(m_fOfsX * (PI2 / 3.0f));
                bcs::BCS_Vec_RotY(move, move, fRot);
                m_TargetPos.x += move.x;
                m_TargetPos.z += move.z;
                m_pChr->SetVolleyTargetMarkerPosClip(6, m_TargetPos);
            }
        }

        inline void CcNinjaCloneBody::UpdatePos_()
        {
            if (m_pMdl) {
                const bcs::FVEC3 &rRot = m_pChr->GetRot();
                m_pMdl->SetRotXYZ(rRot.x, rRot.y, rRot.z);
                bcs::FVEC3 pos(m_pChr->GetPos());
                pos.x += m_fPosRate * (3.0f * m_fOfsX);
                m_pMdl->m_vTrans = pos;
            }
        }

        void CcNinjaCloneBody::CtorStateAppear()
        {
            ChangeBallState_(CcNinjaCloneBall::STATE_APPEAR);
        }

        void CcNinjaCloneBody::LoopStateAppear()
        {
            UpdatePos_();
        }

        void CcNinjaCloneBody::DtorStateAppear()
        {
        }

        void CcNinjaCloneBody::CtorStateLoop()
        {
            ChangeBallState_(CcNinjaCloneBall::STATE_LOOP);
        }

        void CcNinjaCloneBody::LoopStateLoop()
        {
            UpdatePos_();
        }

        void CcNinjaCloneBody::DtorStateLoop()
        {
        }

        void CcNinjaCloneBody::CtorStateArtsLoop()
        {
            ChangeBallState_(CcNinjaCloneBall::STATE_ARTS_LOOP);
            StopEffect_();
            m_hEffect = CEffectCtrl::CreateAttachEffectAuto(0x9F, m_pMdl, 3);
        }

        void CcNinjaCloneBody::LoopStateArtsLoop()
        {
            UpdatePos_();
        }

        void CcNinjaCloneBody::DtorStateArtsLoop()
        {
        }

        // A puff of smoke on the clone's chest.
        void CcNinjaCloneBody::CtorStateReady()
        {
            ChangeBallState_(CcNinjaCloneBall::STATE_READY);
            if (m_pMdl) {
                nw4r::g3d::ResMdl mdl = m_pMdl->GetResMdl();
                if (mdl.IsValid()) {
                    nw4r::g3d::ResNode node = mdl.GetResNode("mune_jnt");
                    if (node.IsValid()) {
                        bcs::FMTX34 mtx;
                        m_pMdl->GetScnMtxPos(&mtx, node.GetID(), (bcs::CScnObj::ScnObjMtxType)1);
                        bcs::FVEC3 pos;
                        pos.x = mtx.m[0][3];
                        pos.y = mtx.m[1][3];
                        pos.z = mtx.m[2][3];
                        CEffectCtrl::SimpleEffect(0x4EF, pos, 3, 0, 0);
                    }
                }
            }
        }

        void CcNinjaCloneBody::LoopStateReady()
        {
            UpdatePos_();
        }

        void CcNinjaCloneBody::DtorStateReady()
        {
        }

        void CcNinjaCloneBody::CtorStateImpact()
        {
            ChangeBallState_(CcNinjaCloneBall::STATE_IMPACT);
            StopEffect_();
        }

        void CcNinjaCloneBody::LoopStateImpact()
        {
            UpdatePos_();
        }

        void CcNinjaCloneBody::DtorStateImpact()
        {
        }

        void CcNinjaCloneBody::CtorStateDisappear()
        {
            m_fPosRate = 1.0f;
            m_fDisappear = 0.0f;
            if (m_pMdl) {
                m_pMdl->OnCloneFlag(0x40000);
            }
            ChangeBallState_(CcNinjaCloneBall::STATE_DISAPPEAR);
            if (m_hEffect2) {
                CEffectCtrl::StopAttachEffect(m_hEffect2);
                m_hEffect2 = 0;
            }
            StopEffect_();
            m_hEffect = CEffectCtrl::CreateAttachEffectAuto(0xA1, m_pMdl, 3);
            CGameSnd::Play(0x4DE, m_pMdl->m_vTrans, 1, -1, 0.0f, 1.0f, 0.0f);
        }

        // Stretches up and fades out over 16 frames, then deletes the task.
        void CcNinjaCloneBody::LoopStateDisappear()
        {
            m_fDisappear += 0.0625f * GetFrameRateF();
            m_fDisappear = Clamp(m_fDisappear, 0.0f, 1.0f);
            if (m_pMdl) {
                m_pMdl->SetAlphaRateF(1.0f - m_fDisappear);
                m_pMdl->m_vScale.y = 2.0f * m_fDisappear + 1.0f;
                m_pMdl->m_vScale.x = 1.0f - 0.8f * m_fDisappear;
                m_pMdl->m_vScale.z = 1.0f - 0.8f * m_fDisappear;
            }
            if (m_fDisappear >= 1.0f) {
                Delete();
            }
        }

        void CcNinjaCloneBody::DtorStateDisappear()
        {
        }

        CcNinjaCloneBall::CcNinjaCloneBall(bcs::CTask *pParent, bcs::CMdlObj *pMdl, const bcs::FVEC3 &rOfs,
                                           CBall *pBall)
            : bcs::CTask(pParent, 0x16)
            , m_pMdl(0)
            , m_pShadow(0)
            , m_pRef(0)
            , m_pParentMdl(pMdl)
            , m_Unk8C(0)
            , m_fDisappear(0.0f)
            , m_fRate(0.0f)
            , m_Ofs(rOfs)
            , m_hEffect(0)
            , m_hEffect2(0)
        {
            InitFsm(this, STATE_NUM);
            SetState(STATE_APPEAR, &CcNinjaCloneBall::CtorStateAppear, &CcNinjaCloneBall::LoopStateAppear,
                     &CcNinjaCloneBall::DtorStateAppear);
            SetState(STATE_LOOP, &CcNinjaCloneBall::CtorStateLoop, &CcNinjaCloneBall::LoopStateLoop,
                     &CcNinjaCloneBall::DtorStateLoop);
            SetState(STATE_ARTS_LOOP, &CcNinjaCloneBall::CtorStateArtsLoop, &CcNinjaCloneBall::LoopStateArtsLoop,
                     &CcNinjaCloneBall::DtorStateArtsLoop);
            SetState(STATE_READY, &CcNinjaCloneBall::CtorStateReady, &CcNinjaCloneBall::LoopStateReady,
                     &CcNinjaCloneBall::DtorStateReady);
            SetState(STATE_IMPACT, &CcNinjaCloneBall::CtorStateImpact, &CcNinjaCloneBall::LoopStateImpact,
                     &CcNinjaCloneBall::DtorStateImpact);
            SetState(STATE_DISAPPEAR, &CcNinjaCloneBall::CtorStateDisappear, &CcNinjaCloneBall::LoopStateDisappear,
                     &CcNinjaCloneBall::DtorStateDisappear);

            CreateMdl_(pBall);
            m_hEffect = CEffectCtrl::CreateAttachEffect(0x4E6, m_pMdl, 0, 0, 0, 0, 0, 3, 0, 0, 0);
            m_hEffect2 = CEffectCtrl::CreateAttachEffect(0x4E7, m_pMdl, 0, 0, 0, 0, 0, 3, 0, 0, 0);
        }

        // uncalled stand-in: the original .data holds a second, unreferenced
        // copy of this class's state-function pointer constants right after
        // the ones the constructor uses (most likely from code the link
        // removed); this function makes the compiler emit them there. The
        // module link strips it.
        void CcNinjaCloneBall::UnusedStateTable_()
        {
            InitFsm(this, STATE_NUM);
            SetState(STATE_APPEAR, &CcNinjaCloneBall::CtorStateAppear, &CcNinjaCloneBall::LoopStateAppear,
                     &CcNinjaCloneBall::DtorStateAppear);
            SetState(STATE_LOOP, &CcNinjaCloneBall::CtorStateLoop, &CcNinjaCloneBall::LoopStateLoop,
                     &CcNinjaCloneBall::DtorStateLoop);
            SetState(STATE_ARTS_LOOP, &CcNinjaCloneBall::CtorStateArtsLoop, &CcNinjaCloneBall::LoopStateArtsLoop,
                     &CcNinjaCloneBall::DtorStateArtsLoop);
            SetState(STATE_READY, &CcNinjaCloneBall::CtorStateReady, &CcNinjaCloneBall::LoopStateReady,
                     &CcNinjaCloneBall::DtorStateReady);
            SetState(STATE_IMPACT, &CcNinjaCloneBall::CtorStateImpact, &CcNinjaCloneBall::LoopStateImpact,
                     &CcNinjaCloneBall::DtorStateImpact);
            SetState(STATE_DISAPPEAR, &CcNinjaCloneBall::CtorStateDisappear, &CcNinjaCloneBall::LoopStateDisappear,
                     &CcNinjaCloneBall::DtorStateDisappear);
        }

        CcNinjaCloneBall::~CcNinjaCloneBall()
        {
            CEffectCtrl::StopAttachEffect(m_hEffect);
            CEffectCtrl::StopAttachEffect(m_hEffect2);
            m_hEffect = 0;
            m_hEffect2 = 0;
            if (m_pRef) {
                bcs::CScnObj::AppendDelayDeleteObject(m_pRef);
            }
            m_pRef = 0;
            if (m_pShadow) {
                bcs::CScnObj::AppendDelayDeleteObject(m_pShadow);
            }
            m_pShadow = 0;
            if (m_pMdl) {
                bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
            }
            m_pMdl = 0;
        }

        // A copy of the ball's model with its shadow and reflection, in the
        // stage's ball colour, where the ball is.
        void CcNinjaCloneBall::CreateMdl_(CBall *pBall)
        {
            bcs::CMdlObj *pSrc = pBall->GetMdlObj();
            nw4r::g3d::ResFile file = pSrc->GetResMdl().GetParent();

            nw4r::g3d::ResMdl mdl = file.GetResMdl(0);
            if (mdl.IsValid()) {
                m_pMdl = new (0) bcs::CMdlClone(pSrc, mdl, 0, 0, 0);
                m_pMdl->OnFlag(0x100);
                m_pMdl->OnFlag(2);
                m_pMdl->SetExBlendMode(1);
                m_pMdl->m_vTrans = pSrc->m_vTrans;
                m_pMdl->SetRotXYZ(pSrc->m_fRotX, pSrc->m_fRotY, pSrc->m_fRotZ);
                m_pMdl->CalcRootMtx();
                m_pMdl->CalcWorldMatrixAll(0);
                m_pMdl->OnFlag(0x4000);
                bcs::CRenderMan::PushBack(m_pMdl, 2);
            }

            nw4r::g3d::ResMdl shadow = file.GetResMdl(2);
            if (shadow.IsValid()) {
                m_pShadow = new bcs::CMdlShadow(m_pMdl, shadow, 0x40, 0, 0);
                bcs::CRenderMan::PushBack(m_pShadow, 2);
            }

            nw4r::g3d::ResMdl ref = file.GetResMdl(1);
            if (ref.IsValid()) {
                m_pRef = new bcs::CMdlRef(m_pMdl, ref, 0, 0, 0);
                bcs::CRenderMan::PushBack(m_pRef, 4);
            }

            bcs::BCS_Color color(0xFFFFFFFF);
            if (CBallMan::GetStageBallColor(color)) {
                m_pMdl->SetMatTevColor((_GXTevRegID)1, color);
            }
            m_StartPos = pSrc->m_vTrans;
        }

        void CcNinjaCloneBall::Main()
        {
            ExecState();

            if (m_pMdl) {
                m_pMdl->OffFlag(2);
                m_pMdl->ObjProc(1, 0, 0);
                m_pMdl->OnFlag(2);
            }
        }

        inline void CcNinjaCloneBall::UpdatePos_()
        {
            if (m_pMdl) {
                bcs::FMTX34 mtx;
                m_pParentMdl->GetScnMtxPos(&mtx, 0, (bcs::CScnObj::ScnObjMtxType)1);
                bcs::FVEC3 ofs;
                ofs.x = mtx.m[0][3];
                ofs.y = mtx.m[1][3];
                ofs.z = mtx.m[2][3];
                bcs::FVEC3 pos;
                bcs::FVEC3Lerp(&pos, &m_StartPos, &ofs, m_fRate);
                ofs.x = 0.0f;
                ofs.y = 0.0f;
                ofs.z = 0.0f;
                bcs::FVEC3Lerp(&ofs, &ofs, &m_Ofs, m_fRate);
                bcs::BCS_Vec_RotY(ofs, ofs, m_pParentMdl->m_fRotY);
                bcs::FVEC3Add(&pos, &pos, &ofs);
                m_pMdl->m_vTrans = pos;
            }
        }

        void CcNinjaCloneBall::CtorStateAppear()
        {
        }

        void CcNinjaCloneBall::LoopStateAppear()
        {
            UpdatePos_();
        }

        void CcNinjaCloneBall::DtorStateAppear()
        {
        }

        void CcNinjaCloneBall::CtorStateLoop()
        {
        }

        void CcNinjaCloneBall::LoopStateLoop()
        {
            UpdatePos_();
        }

        void CcNinjaCloneBall::DtorStateLoop()
        {
        }

        void CcNinjaCloneBall::CtorStateArtsLoop()
        {
        }

        void CcNinjaCloneBall::LoopStateArtsLoop()
        {
            UpdatePos_();
        }

        void CcNinjaCloneBall::DtorStateArtsLoop()
        {
        }

        void CcNinjaCloneBall::CtorStateReady()
        {
            CEffectCtrl::StopAttachEffect(m_hEffect);
            CEffectCtrl::StopAttachEffect(m_hEffect2);
            m_hEffect = 0;
            m_hEffect2 = 0;
        }

        void CcNinjaCloneBall::LoopStateReady()
        {
            UpdatePos_();
        }

        void CcNinjaCloneBall::DtorStateReady()
        {
        }

        void CcNinjaCloneBall::CtorStateImpact()
        {
        }

        void CcNinjaCloneBall::LoopStateImpact()
        {
            UpdatePos_();
        }

        void CcNinjaCloneBall::DtorStateImpact()
        {
        }

        void CcNinjaCloneBall::CtorStateDisappear()
        {
            m_fRate = 1.0f;
            m_fDisappear = 0.0f;
            if (m_pMdl) {
                m_pMdl->OnCloneFlag(0x40000);
            }
            CEffectCtrl::StopAttachEffect(m_hEffect);
            CEffectCtrl::StopAttachEffect(m_hEffect2);
            m_hEffect = 0;
            m_hEffect2 = 0;
        }

        // Stretches up and fades out over 16 frames, then deletes the task.
        void CcNinjaCloneBall::LoopStateDisappear()
        {
            m_fDisappear += 0.0625f * GetFrameRateF();
            m_fDisappear = Clamp(m_fDisappear, 0.0f, 1.0f);
            if (m_pMdl) {
                m_pMdl->SetAlphaRateF(1.0f - m_fDisappear);
                m_pMdl->m_vScale.y = 2.0f * m_fDisappear + 1.0f;
                m_pMdl->m_vScale.x = 1.0f - 0.8f * m_fDisappear;
                m_pMdl->m_vScale.z = 1.0f - 0.8f * m_fDisappear;
            }
            if (m_fDisappear >= 1.0f) {
                Delete();
            }
        }

        void CcNinjaCloneBall::DtorStateDisappear()
        {
        }
    }
}
