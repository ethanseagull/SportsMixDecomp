// msw::CChrCoreBasketDaisy (Daisy) in the basketball game, and her special
// shot's flower garden (CC03_BASKET::CcDaisyFlowerGarden).
//
// When the trick starts, a garden is planted in the middle of the court she
// attacks: a fence effect goes up around it and is a body collision that
// pushes the characters out, and every two frames a new ring of flowers grows
// a little further out, until the rings reach 5.4. Flowers are left out where
// the court's edge would move them by more than one unit. When the garden is
// told to go (messages 0xD and 0xE), the flowers and the fence vanish one
// ring after the other, in the order they grew.
// (The include order steers the order of the weak vtables in .data: the
// original's has bcs::CUnivTask, bcs::CState and msw::CGameTask defined
// before the character classes. The module link stripped the CGameTask and
// CUnivTask vtables and left their words as zeros after the Daisy and
// CState ones.)
#include "bcs/CUnivTask.hpp"
#include "bcs/CFsm.hpp"
#include "game/msw/CGameTask.hpp"
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnGroup.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/ut/List.hpp"

namespace {

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // (guessed helper) An angle brought into [0, 2 pi).
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

    // (guessed helper) The squared distance of two points seen from above.
    inline float DistSqXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3DistSq(&pos1, &pos2);
    }

    // (name guessed) 70 degrees; nothing in the unit reads it.
    float s_fUnkRange = bcs::DEG_TO_RAD(70.0f);
}

namespace msw {
    namespace CC03_BASKET {

        class CcDaisyFlowerGarden;

        // The fence around the garden: one effect. 0x44 bytes. (member
        // names guessed)
        class CcDaisyFenceUnit : public CGameTask {
            public:
                CcDaisyFenceUnit(bcs::CTask *pParent, const bcs::FVEC3 &rPos, float fRotY);
                virtual ~CcDaisyFenceUnit();
                virtual void Main();

                // The effect follows the garden.
                void SetPos(const bcs::FVEC3 &rPos)
                {
                    if (m_hEffect) {
                        CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
                        if (pCluster) {
                            bcs::FVEC3 pos(rPos);
                            pCluster->SetTranslate(&pos);
                        }
                    }
                }

                // Goes once its wait is over (m_fWait is never set, so at
                // once).
                void Vanish()
                {
                    if (m_nStep == 0) {
                        m_nStep++;
                    }
                }

            private:
                void CreateMdl_(const bcs::FVEC3 &rPos, float fRotY);

                unsigned long m_hEffect; // 0x34
                long m_nStep;            // 0x38: 0 standing, 1 vanishing
                float m_fScale;          // 0x3C, of the effect along X and Z
                float m_fWait;           // 0x40, frames before it goes
        };

        // One flower: an effect put in the garden's scene group. 0x4C bytes,
        // kept in its garden's list. (member names guessed)
        class CcDaisyFlowerUnit : public CGameTask {
            public:
                inline CcDaisyFlowerUnit(CcDaisyFlowerGarden *pGarden, bcs::CScnGroup *pGroup,
                                         const bcs::FVEC3 &rPos, float fWait);
                virtual ~CcDaisyFlowerUnit();
                virtual void Main();

                // Goes once its wait is over.
                void Vanish()
                {
                    if (m_nStep == 0) {
                        m_nStep++;
                    }
                }

            private:
                void CreateMdl_(bcs::CScnGroup *pGroup, const bcs::FVEC3 &rPos, float fWait);

                nw4r::ut::Link m_Link;          // 0x34
                CcDaisyFlowerGarden *m_pGarden; // 0x3C
                unsigned long m_hEffect;        // 0x40
                long m_nStep;                   // 0x44: 0 standing, 1 vanishing
                float m_fWait;                  // 0x48, frames before it goes (the frame its ring grew)
        };

        // The garden: an obstacle of type 3 with a body collision (the
        // fence), a scene group for the flowers and the list of them. 0x120
        // bytes. (member names guessed)
        class CcDaisyFlowerGarden : public CChrObstacle, public bcs::CFsm<CcDaisyFlowerGarden> {
            public:
                enum {
                    STATE_SLEEP,
                    STATE_APPEAR, // the rings grow
                    STATE_LOOP,
                    STATE_DISAPPEAR,
                    STATE_NUM
                };

                CcDaisyFlowerGarden(bcs::CTask *pParent, CChrCore *pChr, long nCourt, const bcs::FVEC3 &rPos);
                virtual ~CcDaisyFlowerGarden();
                virtual void Main();
                virtual long SendMessage(long nMsg, long, long, long);

                long GetState() const { return m_nState; }

                void Append(CcDaisyFlowerUnit *pUnit) { nw4r::ut::List_Append(&m_List, pUnit); }
                void Remove(CcDaisyFlowerUnit *pUnit) { nw4r::ut::List_Remove(&m_List, pUnit); }

            private:
                void InitState();

                void CtorStateSleep();
                void LoopStateSleep();
                void DtorStateSleep();
                void CtorStateAppear();
                void LoopStateAppear();
                void DtorStateAppear();
                void CtorStateLoop();
                void LoopStateLoop();
                void DtorStateLoop();
                void CtorStateDisappear();
                void LoopStateDisappear();
                void DtorStateDisappear();

                CcDaisyFlowerUnit *GetFirst()
                {
                    return static_cast<CcDaisyFlowerUnit *>(nw4r::ut::List_GetNext(&m_List, 0));
                }
                CcDaisyFlowerUnit *GetNext(CcDaisyFlowerUnit *pUnit)
                {
                    if (pUnit) {
                        return static_cast<CcDaisyFlowerUnit *>(nw4r::ut::List_GetNext(&m_List, pUnit));
                    }
                    return 0;
                }

                // Stops the garden's sound, if it plays.
                void StopSnd_()
                {
                    if (m_hSnd) {
                        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hSnd);
                        if (pTask && !pTask->IsDelete()) {
                            static_cast<bcs::CSndTask *>(pTask)->Stop(0.0f);
                        }
                        m_hSnd = 0;
                    }
                }

                nw4r::ut::List m_List;       // 0xDC, of CcDaisyFlowerUnit
                bcs::CScnGroup *m_pGroup;    // 0xE8
                long m_nCourt;               // 0xEC, the court it is in
                bcs::FVEC3 m_LocalPos;       // 0xF0, without the court's offset
                float m_fTimer;              // 0xFC: frames to wait, then frames grown
                float m_fRadius;             // 0x100, of the next ring
                float m_fRingWait;           // 0x104, frames to the next ring
                float m_fStartRot;           // 0x108, where the next ring starts
                unsigned long m_hSnd;        // 0x10C, a sound task id
                CBodyCollision m_BodyColl;   // 0x110
                CcDaisyFenceUnit *m_pFence;  // 0x11C
        };

        // A flower at rPos in pGroup.
        inline CcDaisyFlowerUnit::CcDaisyFlowerUnit(CcDaisyFlowerGarden *pGarden, bcs::CScnGroup *pGroup,
                                                    const bcs::FVEC3 &rPos, float fWait)
            : CGameTask(pGarden, -1)
        {
            m_pGarden = pGarden;
            m_hEffect = 0;
            m_nStep = 0;
            m_fWait = fWait;
            m_Link.pPrev = 0;
            m_Link.pNext = 0;
            if (pGarden) {
                pGarden->Append(this);
            }
            CreateMdl_(pGroup, rPos, fWait);
        }
    }
}

msw::CChrCoreBasketDaisy::CChrCoreBasketDaisy()
{
}

msw::CChrCoreBasketDaisy::~CChrCoreBasketDaisy()
{
}

void msw::CChrCoreBasketDaisy::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x73, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
    SubStateArtsShotFireBullet_(pWork, 0);
}

void msw::CChrCoreBasketDaisy::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0x74, 3);
}

void msw::CChrCoreBasketDaisy::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0x75, 3);
}

void msw::CChrCoreBasketDaisy::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayLandMotion_(pWork);

    ArtsOptSetFadeOutSE(0x40A);
    ArtsOptFadeOut(5.0f, 0.0f, 0);
}

// The garden is planted under the goal her team attacks (the middle of that
// court is worked out first, then not used).
void msw::CChrCoreBasketDaisy::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    bcs::FVEC3 pos;
    bcs::FVEC4 rect;
    Util_GetTargetCourtRect_(rect);
    pos.x = 0.5f * (rect.x + rect.y);
    pos.y = 0.0f;
    pos.z = 0.5f * (rect.z + rect.w);
    pos = GetTargetGoalPos();
    pos.y = 0.0f;

    new (0) CC03_BASKET::CcDaisyFlowerGarden(m_pParentTask, this, GetTargetCourtIdx(), pos);
}

namespace msw {
    namespace CC03_BASKET {

        // Out of line, but auto-inlined into its only caller
        // (CtorStateAppear); the original's .rodata has its 1.0 ahead of
        // CreateMdl_'s 2 pi, so it was defined here, and its unused copy
        // left to the link to strip.
        CcDaisyFenceUnit::CcDaisyFenceUnit(bcs::CTask *pParent, const bcs::FVEC3 &rPos, float fRotY)
            : CGameTask(pParent, -1)
        {
            m_hEffect = 0;
            m_nStep = 0;
            m_fWait = 0.0f;
            m_fScale = 1.0f;
            CreateMdl_(rPos, fRotY);
        }

        CcDaisyFenceUnit::~CcDaisyFenceUnit()
        {
            if (m_hEffect) {
                CEffectCtrl::StopAttachEffect(m_hEffect);
            }
        }

        // The fence effect at rPos, turned by fRotY.
        void CcDaisyFenceUnit::CreateMdl_(const bcs::FVEC3 &rPos, float fRotY)
        {
            m_hEffect = CEffectCtrl::CreateAttachEffect(0x537, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
            if (m_hEffect) {
                CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
                if (pCluster) {
                    bcs::FVEC3 rot;
                    bcs::FVEC3 pos;
                    bcs::FVEC3 scale;

                    rot.Set(0.0f, WrapRad(fRotY), 0.0f);
                    pos = rPos;
                    scale.Set(m_fScale, 1.0f, m_fScale);
                    pCluster->SetTranslate(&pos);
                    pCluster->SetRotateRad(&rot);
                    pCluster->SetScale(&scale);

                    long nNum = pCluster->GetMdlObjNum();
                    for (long i = 0; i < nNum; i++) {
                        bcs::CMdlObj *pMdl = pCluster->GetMdlObj(i);
                        if (pMdl) {
                            pMdl->SetExBlendMode(0);
                        }
                    }
                }
            }
        }

        void CcDaisyFenceUnit::Main()
        {
            if (m_nStep) {
                m_fWait -= GetFrameRateF();
                if (m_fWait <= 0.0f) {
                    Delete();
                }
            }
        }

        CcDaisyFlowerUnit::~CcDaisyFlowerUnit()
        {
            if (m_pGarden) {
                m_pGarden->Remove(this);
            }
            if (m_hEffect) {
                CEffectCtrl::StopAttachEffect(m_hEffect);
            }
        }

        // One of the two flower effects at rPos, its models drawn over
        // everything and moved into pGroup.
        void CcDaisyFlowerUnit::CreateMdl_(bcs::CScnGroup *pGroup, const bcs::FVEC3 &rPos, float)
        {
            long nEffect = bcs::BCS_RandMax(1L) ? 0x535 : 0x536;
            if (nEffect >= 0) {
                m_hEffect = CEffectCtrl::CreateAttachEffect(nEffect, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0);
            }
            if (m_hEffect) {
                CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
                if (pCluster) {
                    bcs::FVEC3 pos(rPos);
                    pCluster->SetTranslate(&pos);

                    long nNum = pCluster->GetMdlObjNum();
                    for (long i = 0; i < nNum; i++) {
                        bcs::CMdlObj *pMdl = pCluster->GetMdlObj(i);
                        if (pMdl) {
                            pMdl->SetExBlendMode(0);
                            pMdl->MatSetZModeAll(1, GX_ALWAYS, 1);
                            pMdl->SetDrawPriority(2, 2);
                            pMdl->DetachFromParent();
                            pGroup->AppendChild(pMdl);
                        }
                    }
                }
            }
        }

        void CcDaisyFlowerUnit::Main()
        {
            if (m_nStep) {
                m_fWait -= GetFrameRateF();
                if (m_fWait <= 0.0f) {
                    Delete();
                }
            }
        }

        // The garden at rPos in court nCourt; the fence collision (off
        // until the garden appears) pushes everyone but pChr's player.
        CcDaisyFlowerGarden::CcDaisyFlowerGarden(bcs::CTask *pParent, CChrCore *pChr, long nCourt,
                                                 const bcs::FVEC3 &rPos)
            : CChrObstacle(pParent, 3, 0x13), m_nCourt(nCourt), m_fTimer(0.0f), m_fRadius(0.0f), m_fRingWait(0.0f),
              m_fStartRot(0.0f), m_hSnd(0), m_pFence(0)
        {
            static const float fFenceSize = 3.0f;

            m_LocalPos = rPos;
            bcs::FVEC3 ofs = CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
            m_LocalPos.x -= ofs.x;
            m_LocalPos.z -= ofs.z;

            m_pGroup = new (0x100) bcs::CScnGroup(0x10, 0);
            m_pGroup->m_vTrans = rPos;
            m_pGroup->CalcRootMtx();
            bcs::CRenderMan::PushBack(m_pGroup, 5);

            nw4r::ut::List_Init(&m_List, 0x34);

            m_BodyColl.Create(1, 5, 1, 0);
            m_BodyColl.SetTargetFlagAll(2);
            m_BodyColl.SetPushRankAll(5);
            m_BodyColl.SetPushPowerAll(6);
            m_BodyColl.SetNoHitFlagAll(1 << pChr->GetPlayerNo());
            m_BodyColl.SetPosAll(rPos);
            m_BodyColl.SetSizeAll(bcs::FVEC3(fFenceSize, 2.0f, fFenceSize));
            m_BodyColl.SetPushEnableAll();
            m_BodyColl.SetDisableAll();

            InitState();
        }

        CcDaisyFlowerGarden::~CcDaisyFlowerGarden()
        {
            StopSnd_();
            DeleteAllChild();
            nw4r::ut::List_Init(&m_List, 0x34);
            if (m_pGroup) {
                delete m_pGroup;
            }
            m_pGroup = 0;
        }

        // An ordinary function that the constructor inlines; the original
        // module has no copy of it, only its second copy of the state table
        // in .data.
        void CcDaisyFlowerGarden::InitState()
        {
            InitFsm(this, STATE_NUM);
            SetState(STATE_SLEEP, &CcDaisyFlowerGarden::CtorStateSleep, &CcDaisyFlowerGarden::LoopStateSleep,
                     &CcDaisyFlowerGarden::DtorStateSleep);
            SetState(STATE_APPEAR, &CcDaisyFlowerGarden::CtorStateAppear, &CcDaisyFlowerGarden::LoopStateAppear,
                     &CcDaisyFlowerGarden::DtorStateAppear);
            SetState(STATE_LOOP, &CcDaisyFlowerGarden::CtorStateLoop, &CcDaisyFlowerGarden::LoopStateLoop,
                     &CcDaisyFlowerGarden::DtorStateLoop);
            SetState(STATE_DISAPPEAR, &CcDaisyFlowerGarden::CtorStateDisappear,
                     &CcDaisyFlowerGarden::LoopStateDisappear, &CcDaisyFlowerGarden::DtorStateDisappear);
            ChangeState(STATE_SLEEP);
        }

        // The garden, its fence and its collision follow the court.
        void CcDaisyFlowerGarden::Main()
        {
            if (m_pGroup) {
                bcs::FVEC3 pos = CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
                bcs::FVEC3Add(&pos, &pos, &m_LocalPos);
                m_pGroup->m_vTrans = pos;
                m_pGroup->CalcRootMtx();
                if (m_pFence) {
                    m_pFence->SetPos(pos);
                }
                m_BodyColl.SetPosAll(m_pGroup->m_vTrans);
            }
            ExecState();
        }

        void CcDaisyFlowerGarden::CtorStateSleep()
        {
            m_fTimer = 0.0f;
        }

        void CcDaisyFlowerGarden::LoopStateSleep()
        {
            m_fTimer -= GetFrameRateF();
            if (m_fTimer <= 0.0f) {
                ChangeState(STATE_APPEAR);
            }
        }

        void CcDaisyFlowerGarden::DtorStateSleep()
        {
        }

        // The fence goes up (turned round on court 0) and its collision is
        // switched on; the first ring grows in the next frame.
        void CcDaisyFlowerGarden::CtorStateAppear()
        {
            CEffectCtrl::SimpleEffect(0x533, m_LocalPos, 3, 0, 0);
            m_hSnd = CGameSnd::Play(0x408, &m_LocalPos, 1, -1, 0.0f, 1.0f, 0.0f);
            m_fRingWait = 2.0f - GetFrameRateF();
            m_fTimer = 0.0f;
            m_BodyColl.SetEnableAll();
            m_pFence = new (0) CcDaisyFenceUnit(this, m_LocalPos, m_nCourt ? 0.0f : PI);
        }

        // Every two frames a ring of flowers, 0.84 further out than the last
        // and starting half a step further round, with a flower about every
        // 0.84 along it, a little scattered; until the rings pass 5.4.
        void CcDaisyFlowerGarden::LoopStateAppear()
        {
            // static-order swap (notes/decisions.md): the game numbers
            // FLOWER_UNIT_RADIUS first and stores it second.
            static const float FLOWER_COLL_RADIUS = 0.1f;
            static const float FLOWER_UNIT_RADIUS = 1.2f;

            m_fRingWait += GetFrameRateF();
            unsigned long nHitFlag = (unsigned char)m_nCourt | 0x2000000;
            bcs::FVEC3 center(m_pGroup->m_vTrans);

            while (m_fRingWait > 0.0f) {
                float fLength = PI * (m_fRadius * 2.0f);
                long nNum = (long)(fLength * (1.0f / (FLOWER_UNIT_RADIUS * 0.7f))) + 1;
                float fStep = PI2 / nNum;
                float fRot = WrapRad(m_fStartRot);
                // (as in the original, nothing sets it, so only the radius
                // ends the growth)
                int bNoRoom = 0;

                for (long i = 0; i < nNum; i++) {
                    float fDist = m_fRadius + bcs::BCS_RandRange(-FLOWER_UNIT_RADIUS * 0.1f, FLOWER_UNIT_RADIUS * 0.1f);
                    float fAngle = WrapRad(fRot + bcs::BCS_RandRange(-0.3f * fStep, 0.3f * fStep));
                    bcs::FVEC3 pos;
                    int bCreate = 1;

                    bcs::BCS_VecZ_RotY(pos, fDist, fAngle);

                    bcs::FVEC3 hitPos(pos);
                    bcs::FVEC3 hitNormal;
                    float fHitTime;
                    bcs::FVEC3Add(&hitPos, &hitPos, &center);
                    if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, hitPos, hitPos, FLOWER_COLL_RADIUS, 5,
                                               nHitFlag)) {
                        bcs::FVEC3Sub(&hitPos, &hitPos, &center);
                        if (DistSqXZ(pos, hitPos) <= 1.0f) {
                            pos = hitPos;
                        } else {
                            bCreate = 0;
                        }
                    }

                    if (bCreate) {
                        bNoRoom = 0;
                        new (0x100) CcDaisyFlowerUnit(this, m_pGroup, pos, m_fTimer);
                    }
                    fRot = WrapRad(fRot + fStep);
                }

                m_fTimer += 2.0f;
                m_fStartRot = WrapRad(m_fStartRot + 0.5f * fStep);
                m_fRadius += FLOWER_UNIT_RADIUS * 0.7f;
                if (m_fRadius > 5.4f || bNoRoom) {
                    ChangeState(STATE_LOOP);
                    return;
                }
                m_fRingWait -= 2.0f;
            }
        }

        void CcDaisyFlowerGarden::DtorStateAppear()
        {
        }

        void CcDaisyFlowerGarden::CtorStateLoop()
        {
        }

        void CcDaisyFlowerGarden::LoopStateLoop()
        {
        }

        void CcDaisyFlowerGarden::DtorStateLoop()
        {
        }

        // Every flower and the fence start their wait to go.
        void CcDaisyFlowerGarden::CtorStateDisappear()
        {
            if (m_pGroup) {
                CEffectCtrl::SimpleEffect(0x534, m_LocalPos, 3, 0, 0);
            }
            StopSnd_();
            CGameSnd::Play(0x409, m_LocalPos, 1, -1, 0.0f, 1.0f, 0.0f);

            for (CcDaisyFlowerUnit *pUnit = GetFirst(); pUnit; pUnit = GetNext(pUnit)) {
                pUnit->Vanish();
            }
            if (m_pFence) {
                m_pFence->Vanish();
            }
        }

        // Ends once the last flower and model are gone.
        void CcDaisyFlowerGarden::LoopStateDisappear()
        {
            long nNum = nw4r::ut::List_GetSize(&m_List);
            if (nNum <= 0) {
                if (m_pGroup) {
                    if (m_pGroup->GetChildNum() == 0) {
                        Delete();
                    }
                } else {
                    Delete();
                }
            }
        }

        void CcDaisyFlowerGarden::DtorStateDisappear()
        {
            // (guessed) Dead comparison: the original .rodata has -1.0f after
            // the last state's constants and nothing reads it (likely stripped
            // code); this keeps it in place. See notes/decisions.md.
            if (m_fTimer == -1.0f) {
            }
        }

        // Messages 0xD and 0xE make the garden go.
        long CcDaisyFlowerGarden::SendMessage(long nMsg, long, long, long)
        {
            switch (nMsg) {
                case 0xD:
                case 0xE:
                    if (GetState() != STATE_DISAPPEAR) {
                        ChangeState(STATE_DISAPPEAR);
                    }
                    break;
            }
            return 0;
        }
    }
}
