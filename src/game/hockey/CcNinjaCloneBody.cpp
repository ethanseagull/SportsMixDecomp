// msw::CChrCoreHockeyNinja: the ninja in the hockey game.
//
// His shot trick: he jumps up, vanishes and appears again in front of the
// goal he attacks, at one of three places across it, and two clones of
// himself (CC14_HOCKEY::CcNinjaCloneBody) slide out to the other two, 2.5
// units apart. Each clone holds a copy of the puck
// (CC14_HOCKEY::CcNinjaCloneBall) and wears his team shirt; a clone that
// someone runs into vanishes, and the rest vanish when he shoots.
//
// Written after VolleyF's game/volley/CcNinjaCloneBody.cpp (the clone
// models) and BasketF's game/basket/CChrCoreBasketNinja.cpp (the clones'
// body collision).
// (CGameTask's header comes first: the original's .data has the weak
// CGameTask and CUnivTask vtables after CChrCoreHockeyNinja's, so those
// classes were declared before it.)
#include "game/msw/CGameTask.hpp"
#include "game/msw/CChrCoreHockey.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/CMdlClone.hpp"
#include "bcs/CMdlCopy.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/g3d/ResMdl.hpp"

// Where the ninja (column 0) and his two clones (columns 1 and 2) stand, in
// steps of 2.5 units across the goal, for the three arrangements; mirrored
// for the other team. The clones' places are relative to his.
static const float s_ClonePosTbl[3][3] = {
    {0.0f, -2.0f, 2.0f},
    {-2.0f, 2.0f, 4.0f},
    {2.0f, -4.0f, -2.0f},
};

// Built by the file's static initialiser. The four team colours (of the
// clones' shirts), and two vectors nothing here reads (most likely from a
// shared header, as in other HockeyF files; names not known).
namespace {
    bcs::BCS_Color s_TeamColor[4] = {
        bcs::BCS_Color(0x003CF0FF),
        bcs::BCS_Color(0xC80000FF),
        bcs::BCS_Color(0x00C800FF),
        bcs::BCS_Color(0xFFC800FF),
    };

    bcs::FVEC3 s_vUnk[2] = {
        bcs::FVEC3(0.0f, 0.0f, -1.0f),
        bcs::FVEC3(0.0f, 0.0f, 0.0f),
    };

    const float PI = 3.1415927f;
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

    inline float Clamp(float x, float min, float max)
    {
        return x > max ? max : (x < min ? min : x);
    }
}

namespace msw {
    namespace CC14_HOCKEY {

        // The copy of the puck a clone holds, 0xAC bytes: a clone of the
        // puck's model attached to the clone's hand. bcs::CFsm at 0x34 (vptr
        // at 0x78). Member names are guessed.
        class CcNinjaCloneBall : public CGameTask, public bcs::CFsm<CcNinjaCloneBall> {
                friend class CcNinjaCloneBody;
                friend class msw::CChrCoreHockeyNinja;

            public:
                // The same states as the clone's (the clone changes both).
                enum {
                    STATE_APPEAR,
                    STATE_LOOP,
                    STATE_ARTS_LOOP,
                    STATE_IMPACT,
                    STATE_DISAPPEAR,
                    STATE_NUM
                };

                // pMdl, nNode: the clone's model and the node of its hand;
                // pBall: the puck copied.
                CcNinjaCloneBall(bcs::CTask *pParent, bcs::CMdlObj *pMdl, long nNode, CBall *pBall);
                virtual ~CcNinjaCloneBall();
                virtual void Main();

            private:
                void CreateMdl_(CBall *pBall);
                // (name guessed) The state table. The original .data holds
                // the pointers to members twice: this function is inlined
                // into the constructor and its own copy is dropped by the
                // link.
                void InitState_();

                void CtorStateAppear();
                void LoopStateAppear();
                void DtorStateAppear();
                void CtorStateLoop();
                void LoopStateLoop();
                void DtorStateLoop();
                void CtorStateArtsLoop();
                void LoopStateArtsLoop();
                void DtorStateArtsLoop();
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
                float m_fRate;                // 0x94 (set, not read here)
                bcs::FVEC3 m_StartPos;        // 0x98, where the puck was
                unsigned long m_hEffect;      // 0xA4
                unsigned long m_hEffect2;     // 0xA8
        };

        // One clone: a copy of the ninja's model, 0x114 bytes, an obstacle
        // the other characters run into. bcs::CFsm at 0x94 (vptr at 0xD8).
        // Member names are guessed.
        class CcNinjaCloneBody : public CChrObstacle, public bcs::CFsm<CcNinjaCloneBody> {
                friend class msw::CChrCoreHockeyNinja;

            public:
                enum {
                    STATE_APPEAR,    // slides out from the ninja
                    STATE_LOOP,      // at its place; others can run into it
                    STATE_ARTS_LOOP,
                    STATE_IMPACT,
                    STATE_DISAPPEAR, // shrinks and fades away, then deletes the task
                    STATE_NUM
                };

                // pChr: the ninja; pBall, nNode: the puck it gets a copy of
                // and the node of its hand; fOfsX: its place beside him in
                // steps of 2.5 units; nEffect: the effect it appears with;
                // pBody: his body unit, whose size its collision takes.
                CcNinjaCloneBody(bcs::CTask *pParent, CChrCore *pChr, CBall *pBall, float fOfsX, long nNode,
                                 long nEffect, const CCollBodyUnit *pBody);
                virtual ~CcNinjaCloneBody();
                virtual void Main();
                virtual long GetEffectRootNodeIdx() const { return m_nEffectRootNode; }

                // Anyone who runs into the clone makes it vanish, and is
                // pushed back.
                static void CallbackBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                             bcs::FVEC3 *pPush);

            private:
                void CreateMdl_(CBall *pBall, long nNode);
                // (name guessed) The state table, as CcNinjaCloneBall's.
                void InitState_();

                inline void ChangeBallState_(long nState);

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
                void CtorStateImpact();
                void LoopStateImpact();
                void DtorStateImpact();
                void CtorStateDisappear();
                void LoopStateDisappear();
                void DtorStateDisappear();

                CChrCore *m_pChr;                // 0xDC
                bcs::CMdlClone *m_pMdl;          // 0xE0
                bcs::CMdlShadow *m_pShadow;      // 0xE4
                bcs::CMdlRef *m_pRef;            // 0xE8
                CcNinjaCloneBall *m_pBall;       // 0xEC
                CBodyCollision *m_pBodyColl;     // 0xF0
                float m_fOfsX;                   // 0xF4
                float m_fPosRate;                // 0xF8, 0 at the ninja, 1 at its place
                unsigned long m_UnkFC;           // 0xFC (cleared, never read here)
                float m_fDisappear;              // 0x100, STATE_DISAPPEAR: 0 to 1
                unsigned long m_hEffect;         // 0x104, the effect of the current state
                unsigned long m_hEffect2;        // 0x108, the effect it appears with
                unsigned long m_hEffect3;        // 0x10C, the light of the shot
                long m_nEffectRootNode;          // 0x110, the ninja model's
        };
    }
}

// Deletes the clones and forgets them.
inline void msw::CChrCoreHockeyNinja::DeleteClone_()
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

// The clones missing, at their places in pPosTbl (a row of
// s_ClonePosTbl), each with a copy of the puck he holds; they appear with
// effect nEffect.
inline void msw::CChrCoreHockeyNinja::CreateClone_(const float *pPosTbl, long nEffect)
{
    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i] == 0) {
            float fOfsX = pPosTbl[1 + i];
            if (GetTargetGoalIdx() != 0) {
                fOfsX = -fOfsX;
            }
            CC14_HOCKEY::CcNinjaCloneBody *pClone = new (0x100) CC14_HOCKEY::CcNinjaCloneBody(
                CGame::GetTempParent(), this, GetAttachBall(), fOfsX, m_nEffNodeDunk, nEffect, CollBodyGetColl_(0));
            if (pClone) {
                m_ahClone[i] = pClone->GetId();
            }
        }
    }
}

// Changes the state of the clones still there and forgets the others.
inline void msw::CChrCoreHockeyNinja::ChangeStateClone_(long nState)
{
    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i]) {
            CC14_HOCKEY::CcNinjaCloneBody *pClone =
                static_cast<CC14_HOCKEY::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(m_ahClone[i]));
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

msw::CChrCoreHockeyNinja::CChrCoreHockeyNinja()
{
    m_pUnk780 = 0;
    m_nPosType = 0;
    m_BallOfs.x = 0.0f;
    m_BallOfs.y = 0.0f;
    m_BallOfs.z = 0.0f;
}

msw::CChrCoreHockeyNinja::~CChrCoreHockeyNinja()
{
    ArtsOptStopEffectAll();
    DeleteClone_();
    if (m_pUnk780) {
        delete m_pUnk780;
    }
    m_pUnk780 = 0;
}

void msw::CChrCoreHockeyNinja::SetCloneBodyPosRate_(float fRate, float)
{
    fRate = Clamp(fRate, 0.0f, 1.0f);

    bcs::FMTX34 mtx;
    m_pMdl->GetNodeLocalMtx(m_nEffNodeDunk, mtx);
    bcs::FVEC3 ofs(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
    bcs::BCS_Vec_RotY(ofs, ofs, GetRotY());
    m_BallOfs = ofs;

    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i]) {
            CC14_HOCKEY::CcNinjaCloneBody *pClone =
                static_cast<CC14_HOCKEY::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(m_ahClone[i]));
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

// When he stands at one end of the row, the middle one is a clone: the
// special shot's effects centre on it.
void msw::CChrCoreHockeyNinja::GetSpRootPos(bcs::FVEC3 &rPos) const
{
    rPos = GetPos();
    if (GetState() == STATE_AIR_TRICK) {
        long nIndex = -1;
        switch (m_nPosType) {
            case 1:
                nIndex = 0;
                break;
            case 2:
                nIndex = 1;
                break;
        }
        if (nIndex >= 0 && m_ahClone[nIndex]) {
            CC14_HOCKEY::CcNinjaCloneBody *pClone =
                static_cast<CC14_HOCKEY::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(m_ahClone[nIndex]));
            if (pClone && !pClone->IsDelete()) {
                rPos = pClone->GetPos();
            }
        }
    }
}

void msw::CChrCoreHockeyNinja::SubStateArtsShotSetSetbackFrame_(STATE_WORK_AIR_TRICK *pWork, long)
{
    pWork->m_fSetbackWait = AnimGetResFrameNum(0x9C);
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_fTrickMotFrameNum = 60.0f;
}

// He comes back 19 units in front of the goal he attacks.
void msw::CChrCoreHockeyNinja::SubStateArtsShotCalcSetbackPos_(bcs::FVEC3 &rPos)
{
    bcs::FVEC3 goal(GetTargetGoalPos());
    bcs::FVEC3 ofs;
    bcs::BCS_VecZ_RotY(ofs, 19.0f, GetTargetGoalYRot());
    rPos.x = goal.x + ofs.x;
    rPos.z = goal.z + ofs.z;
}

void msw::CChrCoreHockeyNinja::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x16, 0xBE, 3);
    SE_PlayArtsJumpSE(0);
}

// Step 0: at the end of his jump he vanishes. Step 1: he waits in front of
// the goal; when the move is over he appears at his place in a drawn
// arrangement (the one his jump's direction points at, if any) with the
// clones beside him. Step 2: ready at the end of his motion.
int msw::CChrCoreHockeyNinja::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);

    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
                m_nStatusFlag |= 0x0C000000;
                m_UnkEC |= 4;
                CBall *pBall = GetAttachBall();
                if (pBall) {
                    pBall->OnFlag(4);
                }
                pWork->m_nTrickStep++;
            }
            break;
        case 1: {
            bcs::FVEC3 goal(GetTargetGoalPos());
            bcs::FVEC3 pos;
            bcs::BCS_VecZ_RotY(pos, 19.0f, GetTargetGoalYRot());
            pos.x = goal.x + pos.x;
            pos.z = goal.z + pos.z;
            SetPos(pos);

            if (pWork->m_fMoveFrame >= pWork->m_Move.m_fFrameMax) {
                CBall *pBall = GetAttachBall();
                if (pBall) {
                    pBall->OffFlag(4);
                }

                long nGoal = GetTargetGoalIdx();
                // (the original computes this yaw and does not use it)
                GetRotY_ToPosXZ(CGame::GetGoalPos(nGoal));
                long nPosType = bcs::BCS_RandMax(2L);
                if (m_fJumpRotY >= 0.0f) {
                    float fRot = WrapRad(m_fJumpRotY - GetGoalYRot(nGoal));
                    if (fRot < 3.4906585f && fRot > 2.7925267f) {
                        nPosType = 0;
                    } else if (fRot > 4.363323f && fRot < 5.061455f) {
                        nPosType = 1;
                    } else if (fRot < 1.9198622f && fRot > 1.2217305f) {
                        nPosType = 2;
                    }
                }
                m_fJumpRotY = -1.0f;

                SetRotY(WrapRad(PI + GetGoalYRot(nGoal)));
                float fZ = 19.0f;
                float fGoalZ = CGame::GetGoalPos(nGoal).z;
                if (fGoalZ > 0.0f) {
                    fZ *= -1.0f;
                }
                fZ += fGoalZ;
                float fX = s_ClonePosTbl[nPosType][0];
                if (GetTargetGoalIdx() != 0) {
                    fX = -fX;
                }
                SetPosXZ(2.5f * fX, fZ);

                m_nStatusFlag &= ~0x0C000000;
                m_UnkEC &= ~4;
                AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
                Util_AttachEffect((ATTACH_EFFECT_TYPE)1, 0);
                Eff_CreateAttachAuto(0x17, 0xBF, 3);

                DeleteClone_();
                m_nPosType = nPosType;
                CreateClone_(s_ClonePosTbl[nPosType], m_pChrData->m_nUnk44);
                ChangeStateClone_(CC14_HOCKEY::CcNinjaCloneBody::STATE_APPEAR);
                SE_PlayMove(8, 0x476, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_nTrickStep++;
            }
            break;
        }
        case 2:
            if (IsAnimEnd(3)) {
                bReady = 1;
                pWork->m_fUnk0C = 120.0f;
                pWork->m_nTrickStep++;
            }
            break;
        default:
            bReady = 1;
            break;
    }
    return bReady;
}

// He skates towards the goal; the shot starts when the wait is over or
// once he has passed the goal's front.
int msw::CChrCoreHockeyNinja::SubStateArtsShotIsStartArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    CCollAtkUnit *pAtk = CollAtkGetColl_(1);
    if (pAtk) {
        pAtk->SetPos(GetPos());
    }
    MoveRunXZ_(WrapRad(PI + GetGoalYRot(GetTargetGoalIdx())), 13.0f, 1.0f / 60.0f);

    int bPassed = 0;
    bcs::FVEC3 goal(GetTargetGoalPos());
    bcs::FVEC3 point(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 normal(0.0f, 0.0f, 0.0f);
    bcs::BCS_VecOneZ_RotY(normal, GetTargetGoalYRot());
    // 5 units in front of the goal
    float fOfsX = normal.x * 5.0f;
    float fOfsZ = normal.z * 5.0f;
    point.x = fOfsX + goal.x;
    point.z = fOfsZ + goal.z;
    bcs::FVEC3 pos(GetPos());
    bcs::FVEC3 dir;
    dir.x = -normal.x;
    dir.y = normal.y;
    dir.z = -normal.z;
    bcs::FVEC3 cross;
    if (CCollMan::GetIntersectionTimeOfFrontPlaneAndRay(cross, normal, point, pos, dir) < 0.0f) {
        bPassed = 1;
    }
    if (bPassed) {
        pWork->m_fUnk0C = 0.0f;
    }
    if (pWork->m_fUnk0C > 0.0f) {
        pWork->m_fUnk0C -= m_fFrameStep;
    }

    SetCloneBodyPosRate_(1.0f, 1.0f);

    if (pWork->m_fUnk0C <= 0.0f) {
        m_nStatusFlag |= 0x40000000;
        CCollAtkUnit *pAtk2 = CollAtkGetColl_(1);
        if (pAtk2) {
            pAtk2->OffFlag10(1);
        }
        return 1;
    }
    return 0;
}

// The light of the shot on each clone.
void msw::CChrCoreHockeyNinja::SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotReadyArtsShot_(pWork);
    pWork->m_fUnk0C = 120.0f;

    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i]) {
            CC14_HOCKEY::CcNinjaCloneBody *pClone =
                static_cast<CC14_HOCKEY::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(m_ahClone[i]));
            if (pClone && !pClone->IsDelete() && pClone->m_pMdl) {
                pClone->m_hEffect3 = CEffectCtrl::CreateAttachEffectAuto(0xC0, pClone->m_pMdl, 3);
            }
        }
    }
}

void msw::CChrCoreHockeyNinja::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    m_nStatusFlag &= ~0x40000000;
    CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x16, 0xC0, 3);
    ChangeStateClone_(CC14_HOCKEY::CcNinjaCloneBody::STATE_LOOP);
    PlayFsSe_(1, 0);
}

// The clones vanish with his shot; he forgets them.
void msw::CChrCoreHockeyNinja::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    SE_Stop(8, 0.0f);
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    ChangeStateClone_(CC14_HOCKEY::CcNinjaCloneBody::STATE_DISAPPEAR);
    m_ahClone[0] = 0;
    m_ahClone[1] = 0;
}

void msw::CChrCoreHockeyNinja::DtorStateAirTrick()
{
    CChrCoreHockey::DtorStateAirTrick();
    ArtsOptStopEffectAll();
    DeleteClone_();
    if (m_pUnk780) {
        delete m_pUnk780;
    }
    m_pUnk780 = 0;
}

namespace msw {
    namespace CC14_HOCKEY {

        inline void CcNinjaCloneBody::ChangeBallState_(long nState)
        {
            if (m_pBall) {
                m_pBall->ChangeState(nState);
            }
        }

        inline void CcNinjaCloneBody::UpdatePos_()
        {
            if (m_pMdl) {
                const bcs::FVEC3 &rRot = m_pChr->GetRot();
                m_pMdl->SetRotXYZ(rRot.x, rRot.y, rRot.z);
                bcs::FVEC3 pos(m_pChr->GetPos());
                pos.x += m_fPosRate * (2.5f * m_fOfsX);
                m_Pos = pos;
                m_pMdl->m_vTrans = pos;
            }
        }

        CcNinjaCloneBody::CcNinjaCloneBody(bcs::CTask *pParent, CChrCore *pChr, CBall *pBall, float fOfsX,
                                           long nNode, long nEffect, const CCollBodyUnit *pBody)
            : CChrObstacle(pParent, 0xE, 0x17)
            , m_pChr(pChr)
            , m_pMdl(0)
            , m_pShadow(0)
            , m_pRef(0)
            , m_pBall(0)
            , m_fOfsX(fOfsX)
            , m_fPosRate(1.0f)
            , m_UnkFC(0)
            , m_fDisappear(0.0f)
            , m_hEffect(0)
            , m_hEffect2(0)
            , m_hEffect3(0)
            , m_nEffectRootNode(0)
        {
            InitState_();
            CreateMdl_(pBall, nNode);
            UpdatePos_();

            m_pBodyColl = new (0) CBodyCollision(1, 1, 2, m_pMdl->GetHandle());
            m_pBodyColl->SetSize(0, pBody->m_Size.x, pBody->m_Size.y, pBody->m_Size.z);
            m_pBodyColl->SetPushRank(0, 5);
            m_pBodyColl->SetPushPower(0, 6);
            m_pBodyColl->ResetTargetFlag(0, 0x14);
            m_pBodyColl->SetNoHitFlagAll(1 << m_pChr->GetPlayerNo());
            m_pBodyColl->SetPushEnable(0);
            m_pBodyColl->SetDisable(0);
            m_pBodyColl->SetBodyPushCallbackAll(CallbackBodyPush, this);

            m_hEffect = CEffectCtrl::CreateAttachEffectAuto(0xBF, m_pMdl, 3);
            m_hEffect2 = CEffectCtrl::CreateAttachEffectAuto(nEffect, m_pMdl, 3);

            m_AtkColl.Create(1, 4, 0, m_pMdl->GetHandle());
            m_AtkColl.SetAtkTypeAll(0x11C00);
            m_AtkColl.SetTargetFlagAll(2);
            m_AtkColl.SetRadiusAll(1.5f);
            m_AtkColl.SetPosAll(m_Pos);
            m_AtkColl.SetEnableAll();
        }

        void CcNinjaCloneBody::InitState_()
        {
            InitFsm(this, STATE_NUM);
            SetState(STATE_APPEAR, &CcNinjaCloneBody::CtorStateAppear, &CcNinjaCloneBody::LoopStateAppear,
                     &CcNinjaCloneBody::DtorStateAppear);
            SetState(STATE_LOOP, &CcNinjaCloneBody::CtorStateLoop, &CcNinjaCloneBody::LoopStateLoop,
                     &CcNinjaCloneBody::DtorStateLoop);
            SetState(STATE_ARTS_LOOP, &CcNinjaCloneBody::CtorStateArtsLoop, &CcNinjaCloneBody::LoopStateArtsLoop,
                     &CcNinjaCloneBody::DtorStateArtsLoop);
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
            if (m_hEffect3) {
                CEffectCtrl::StopAttachEffect(m_hEffect3);
                m_hEffect3 = 0;
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
                m_pMdl->DeleteJointObj();
            }
            if (m_pMdl) {
                bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
            }
            m_pMdl = 0;
            if (m_pBodyColl) {
                delete m_pBodyColl;
            }
            m_pBodyColl = 0;
            m_AtkColl.Destroy();
        }

        // A copy of the ninja's model with his team shirt, shadow and
        // reflection, and the copy of the puck in its hand.
        void CcNinjaCloneBody::CreateMdl_(CBall *pBall, long nNode)
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
                m_nEffectRootNode = pSrc->GetEffectRootNodeIdx();
                bcs::CRenderMan::PushBack(m_pMdl, 2);
            }

            // The shirt (blue on courts of kind 1, else red) in his team's
            // colour.
            if (pSrc->GetJointObj()) {
                int bBlue = 0;
                if (CStageMan::GetCourtType(CGame::g_GameWork.GetStage()) == 1) {
                    bBlue = 1;
                }
                nw4r::g3d::ResMdl shirt = file.GetResMdl(bBlue ? "jb_h" : "jr_h");
                if (shirt.IsValid()) {
                    bcs::CMdlCopy *pShirt = new (0) bcs::CMdlCopy(m_pMdl, shirt, 0, 0, 0);
                    long nColor = CGame::GetTeamColorIdx(m_pChr->GetTeamNo());
                    pShirt->MatSetDstAlphaAll(1, 0xFF);
                    pShirt->SetMatTevColor((_GXTevRegID)1, s_TeamColor[nColor]);
                    m_pMdl->SetJointObj(pShirt);
                }
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

            m_pBall = new (0x100) CcNinjaCloneBall(this, m_pMdl, nNode, pBall);
        }

        void CcNinjaCloneBody::CallbackBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                                bcs::FVEC3 *pPush)
        {
            CcNinjaCloneBody *pClone = static_cast<CcNinjaCloneBody *>(pArg);
            if (pClone) {
                if (pClone->m_nState != STATE_DISAPPEAR) {
                    pClone->ChangeState(STATE_DISAPPEAR);
                }
                if (pPush) {
                    pOther->DoBodyPushCallback(pBody, 0);
                }
            }
        }

        void CcNinjaCloneBody::Main()
        {
            ExecState();

            if (m_pBodyColl->IsEnable(0) == 1) {
                m_pBodyColl->SetPos(0, m_pMdl->m_vTrans, m_pMdl->m_vTrans);
            }

            if (m_pMdl) {
                m_pMdl->OffFlag(2);
                m_pMdl->ObjProc(1, 0, 0);
                m_pMdl->OnFlag(2);
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

        // At its place: others can run into it now.
        void CcNinjaCloneBody::CtorStateLoop()
        {
            ChangeBallState_(CcNinjaCloneBall::STATE_LOOP);
            m_fPosRate = 1.0f;
            m_AtkColl.SetDisableAll();
            m_pBodyColl->SetEnable(0);
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
        }

        void CcNinjaCloneBody::LoopStateArtsLoop()
        {
            UpdatePos_();
        }

        void CcNinjaCloneBody::DtorStateArtsLoop()
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
            if (m_pBodyColl) {
                m_pBodyColl->SetDisableAll();
            }
            ChangeBallState_(CcNinjaCloneBall::STATE_DISAPPEAR);
            if (m_hEffect2) {
                CEffectCtrl::StopAttachEffect(m_hEffect2);
                m_hEffect2 = 0;
            }
            StopEffect_();
            m_hEffect = CEffectCtrl::CreateAttachEffectAuto(0xC2, m_pMdl, 3);
            CGameSnd::Play(0x4DE, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
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

        CcNinjaCloneBall::CcNinjaCloneBall(bcs::CTask *pParent, bcs::CMdlObj *pMdl, long nNode, CBall *pBall)
            : CGameTask(pParent, -1)
            , m_pMdl(0)
            , m_pShadow(0)
            , m_pRef(0)
            , m_pParentMdl(pMdl)
            , m_Unk8C(0)
            , m_fDisappear(0.0f)
            , m_fRate(0.0f)
            , m_hEffect(0)
            , m_hEffect2(0)
        {
            InitState_();
            CreateMdl_(pBall);
            if (m_pMdl && pMdl) {
                m_pMdl->AttachToObject(pMdl, nNode, (bcs::CScnObj::AttachMode)0);
            }
        }

        void CcNinjaCloneBall::InitState_()
        {
            InitFsm(this, STATE_NUM);
            SetState(STATE_APPEAR, &CcNinjaCloneBall::CtorStateAppear, &CcNinjaCloneBall::LoopStateAppear,
                     &CcNinjaCloneBall::DtorStateAppear);
            SetState(STATE_LOOP, &CcNinjaCloneBall::CtorStateLoop, &CcNinjaCloneBall::LoopStateLoop,
                     &CcNinjaCloneBall::DtorStateLoop);
            SetState(STATE_ARTS_LOOP, &CcNinjaCloneBall::CtorStateArtsLoop, &CcNinjaCloneBall::LoopStateArtsLoop,
                     &CcNinjaCloneBall::DtorStateArtsLoop);
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

        // A copy of the puck's model with its shadow and reflection, in the
        // stage's puck colour.
        void CcNinjaCloneBall::CreateMdl_(CBall *pBall)
        {
            bcs::CMdlObj *pSrc = pBall->GetMdlObj();
            nw4r::g3d::ResFile file = pSrc->GetResMdl().GetParent();

            nw4r::g3d::ResMdl mdl = file.GetResMdl(0);
            if (mdl.IsValid()) {
                m_pMdl = new (0x100) bcs::CMdlClone(pSrc, mdl, 0, 0, 0);
                m_pMdl->SetExBlendMode(1);
                m_pMdl->CalcRootMtx();
                m_pMdl->CalcWorldMatrixAll(0);
                m_pMdl->OnFlag(0x4000);
                bcs::CRenderMan::PushBack(m_pMdl, 2);
            }

            nw4r::g3d::ResMdl shadow = file.GetResMdl(2);
            if (shadow.IsValid()) {
                m_pShadow = new (0x100) bcs::CMdlShadow(m_pMdl, shadow, 0x40, 0, 0);
                bcs::CRenderMan::PushBack(m_pShadow, 2);
            }

            nw4r::g3d::ResMdl ref = file.GetResMdl(1);
            if (ref.IsValid()) {
                m_pRef = new (0x100) bcs::CMdlRef(m_pMdl, ref, 0, 0, 0);
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
        }

        void CcNinjaCloneBall::CtorStateAppear()
        {
        }

        void CcNinjaCloneBall::LoopStateAppear()
        {
        }

        void CcNinjaCloneBall::DtorStateAppear()
        {
        }

        void CcNinjaCloneBall::CtorStateLoop()
        {
        }

        void CcNinjaCloneBall::LoopStateLoop()
        {
        }

        void CcNinjaCloneBall::DtorStateLoop()
        {
        }

        void CcNinjaCloneBall::CtorStateArtsLoop()
        {
        }

        void CcNinjaCloneBall::LoopStateArtsLoop()
        {
        }

        void CcNinjaCloneBall::DtorStateArtsLoop()
        {
        }

        void CcNinjaCloneBall::CtorStateImpact()
        {
        }

        void CcNinjaCloneBall::LoopStateImpact()
        {
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
