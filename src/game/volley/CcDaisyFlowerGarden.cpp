// msw::CChrCoreVolleyDaisy (Daisy) in the volleyball game.
//
// Her air trick plants a flower garden (CC03_VOLLEY::CcDaisyFlowerGarden) in
// the middle of the court the ball goes to: rings of flowers
// (CC03_VOLLEY::CcDaisyFlowerUnit) grow outwards from that point until they
// are 6 across or a wall is in the way, and wilt ring by ring, the inner ones
// first, when the garden is told to go (messages 0xD and 0xE). The garden is
// an obstacle task, so the computer players can see it.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnGroup.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC4.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/ut/List.hpp"

namespace {

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

    // The square of the distance between two points in the XZ plane.
    inline float DistSqXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3DistSq(&pos1, &pos2);
    }
}

namespace msw {
    namespace CC03_VOLLEY {

        class CcDaisyFlowerGarden;

        // One flower of the garden: an effect whose models are moved into the
        // garden's scene group. 0x4C bytes, kept in the garden's list.
        // (member names guessed)
        class CcDaisyFlowerUnit : public bcs::CUnivTaskEx {
            public:
                inline CcDaisyFlowerUnit(CcDaisyFlowerGarden *pGarden, bcs::CScnGroup *pGroup,
                                         const bcs::FVEC3 &rPos, float fWait);
                virtual ~CcDaisyFlowerUnit();
                virtual void Main();

                // Wilts once the wait given at the start is over.
                void StartDisappear()
                {
                    if (m_bDisappear == 0) {
                        m_bDisappear++;
                    }
                }

            private:
                void CreateMdl_(bcs::CScnGroup *pGroup, const bcs::FVEC3 &rPos, float fWait);

                nw4r::ut::Link m_Link;          // 0x34
                CcDaisyFlowerGarden *m_pGarden; // 0x3C
                unsigned long m_hEffect;        // 0x40
                long m_bDisappear;              // 0x44
                float m_fWait;                  // 0x48, frames left before it goes once wilting
        };

        // The garden. 0x110 bytes. (member names guessed)
        class CcDaisyFlowerGarden : public CChrObstacle, public bcs::CFsm<CcDaisyFlowerGarden> {
            public:
                enum {
                    STATE_SLEEP,     // a short wait
                    STATE_APPEAR,    // the rings grow
                    STATE_LOOP,      // in full bloom
                    STATE_DISAPPEAR, // the rings wilt
                    STATE_NUM
                };

                CcDaisyFlowerGarden(bcs::CTask *pParent, CChrCore *pChr, long nCourt, const bcs::FVEC3 &rPos);
                virtual ~CcDaisyFlowerGarden();
                virtual void Main();
                virtual long SendMessage(long nMsg, long, long, long);

                void Append(CcDaisyFlowerUnit *pUnit) { nw4r::ut::List_Append(&m_List, pUnit); }
                void Remove(CcDaisyFlowerUnit *pUnit) { nw4r::ut::List_Remove(&m_List, pUnit); }

            private:
                CcDaisyFlowerUnit *GetNext(CcDaisyFlowerUnit *pUnit)
                {
                    return static_cast<CcDaisyFlowerUnit *>(nw4r::ut::List_GetNext(&m_List, pUnit));
                }

                // Stops the sound of the growing garden.
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

                nw4r::ut::List m_List;      // 0xDC, of CcDaisyFlowerUnit
                bcs::CScnGroup *m_pGroup;   // 0xE8, holds the flowers' models
                long m_nCourt;              // 0xEC, the team court it is planted on
                bcs::FVEC3 m_Pos;           // 0xF0, the centre, relative to that court
                float m_fWait;              // 0xFC: the sleep's wait, then the next ring's wilting delay
                float m_fRadius;            // 0x100, of the next ring
                float m_fTime;              // 0x104, time owed to the growth
                float m_fAngle;             // 0x108, where the next ring starts
                unsigned long m_hSnd;       // 0x10C, a sound task id
        };

        // Puts the flower at rPos (relative to pGroup) in pGarden.
        inline CcDaisyFlowerUnit::CcDaisyFlowerUnit(CcDaisyFlowerGarden *pGarden, bcs::CScnGroup *pGroup,
                                                    const bcs::FVEC3 &rPos, float fWait)
            : CUnivTaskEx(pGarden, -1)
        {
            m_pGarden = pGarden;
            m_hEffect = 0;
            m_bDisappear = 0;
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

msw::CChrCoreVolleyDaisy::CChrCoreVolleyDaisy()
{
}

msw::CChrCoreVolleyDaisy::~CChrCoreVolleyDaisy()
{
}

void msw::CChrCoreVolleyDaisy::Reset_()
{
    CChrCoreVolley::Reset_();
}

void msw::CChrCoreVolleyDaisy::SubStateBlockJumpLandStart_(STATE_WORK_BLOCK_JUMP *pWork)
{
    ArtsOptAnimStop();
    ArtsOptFadeOut(5.0f, 0.0f, 0);
    CChrCoreVolley::SubStateBlockJumpLandStart_(pWork);
}

// The garden is planted in the middle of the court the shot goes to.
void msw::CChrCoreVolleyDaisy::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x6D, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 0.0f, 0);

    bcs::FVEC4 rect;
    Util_GetTargetCourtRect_(rect);
    bcs::FVEC3 pos;
    pos.x = 0.5f * (rect.x + rect.y);
    pos.y = 0.0f;
    pos.z = 0.5f * (rect.z + rect.w);
    new (0x100) CC03_VOLLEY::CcDaisyFlowerGarden(m_pTaskParent, this, GetTargetCourtIdx(), pos);
}

void msw::CChrCoreVolleyDaisy::SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0x6F, 3);
}

void msw::CChrCoreVolleyDaisy::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);

    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0x6E, 3);
}

void msw::CChrCoreVolleyDaisy::SubStateAirTrickPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLandMotion_(pWork);

    ArtsOptSetFadeOutSE(0x40A);
    ArtsOptFadeOut(5.0f, 0.0f, 0);
}

namespace msw {
    namespace CC03_VOLLEY {

        CcDaisyFlowerUnit::~CcDaisyFlowerUnit()
        {
            if (m_pGarden) {
                m_pGarden->Remove(this);
            }
            if (m_hEffect) {
                CEffectCtrl::StopAttachEffect(m_hEffect);
            }
        }

        // One of the two flower effects at random, its models moved into
        // pGroup and drawn on top.
        void CcDaisyFlowerUnit::CreateMdl_(bcs::CScnGroup *pGroup, const bcs::FVEC3 &rPos, float)
        {
            long nEffect = bcs::BCS_RandMax(1L) != 0 ? 0x580 : 0x581;
            if (nEffect >= 0) {
                m_hEffect = CEffectCtrl::CreateAttachEffect(nEffect, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0);
            }

            if (m_hEffect) {
                CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
                if (pCluster) {
                    bcs::FVEC3 pos;
                    pos = rPos;
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
            if (m_bDisappear) {
                m_fWait -= GetFrameRateF();
                if (m_fWait <= 0.0f) {
                    Delete();
                }
            }
        }

        // pChr (Daisy) is not kept. rPos is in the world; the garden keeps it
        // relative to its court, which may move.
        CcDaisyFlowerGarden::CcDaisyFlowerGarden(bcs::CTask *pParent, CChrCore *, long nCourt,
                                                 const bcs::FVEC3 &rPos)
            : CChrObstacle(pParent, 3, 0x17)
        {
            m_nCourt = nCourt;
            m_fWait = 0.0f;
            m_fRadius = 0.0f;
            m_fTime = 0.0f;
            m_fAngle = 0.0f;
            m_hSnd = 0;
            m_Pos = rPos;

            bcs::FVEC3 ofs = CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
            m_Pos.x -= ofs.x;
            m_Pos.z -= ofs.z;

            m_pGroup = new (0x100) bcs::CScnGroup(0x10, 0);
            m_pGroup->m_vTrans = rPos;
            m_pGroup->CalcRootMtx();
            bcs::CRenderMan::PushBack(m_pGroup, 5);

            nw4r::ut::List_Init(&m_List, 0x34);

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

        // The flowers follow the court.
        void CcDaisyFlowerGarden::Main()
        {
            if (m_pGroup) {
                bcs::FVEC3 pos = CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
                bcs::FVEC3Add(&pos, &m_Pos, &pos);
                m_pGroup->m_vTrans = pos;
                m_pGroup->CalcRootMtx();
            }
            ExecState();
        }

        void CcDaisyFlowerGarden::CtorStateSleep()
        {
            m_fWait = 15.0f;
        }

        void CcDaisyFlowerGarden::LoopStateSleep()
        {
            m_fWait -= GetFrameRateF();
            if (m_fWait <= 0.0f) {
                ChangeState(STATE_APPEAR);
            }
        }

        void CcDaisyFlowerGarden::DtorStateSleep()
        {
        }

        void CcDaisyFlowerGarden::CtorStateAppear()
        {
            CEffectCtrl::SimpleEffect(0x57E, m_Pos, 3, 0, 0);
            m_hSnd = CGameSnd::Play(0x408, &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
            // The first ring grows at once.
            float fRate = GetFrameRateF();
            m_fWait = 0.0f;
            m_fTime = 2.0f - fRate;
        }

        // One ring every 2 frames: flowers 1.4 apart around the circle, each
        // a little off its place at random. A flower whose place is behind a
        // wall moves to the wall, or is left out when that is more than 1
        // away. Each ring wilts 2 frames after the one inside it.
        void CcDaisyFlowerGarden::LoopStateAppear()
        {
            // Named in the original (the @LOCAL@ objects are in its .rodata),
            // but its code loads separate literals of the same values.
            static const float FLOWER_COLL_RADIUS = 0.1f;
            static const float FLOWER_UNIT_RADIUS = 2.0f;

            m_fTime += GetFrameRateF();
            unsigned long nHitFlag = (m_nCourt & 0xFF) | 0x2000000;
            bcs::FVEC3 center(m_pGroup->m_vTrans);

            while (m_fTime > 0.0f) {
                // As many flowers as fit 1.4 apart on the circle (the
                // original multiplies by 1 / 1.4).
                long nNum = (long)(2.0f * m_fRadius * 3.1415927f * (1.0f / 1.4f)) + 1;
                float fStep = 6.2831855f / nNum;
                float fAngle = WrapRad(m_fAngle);
                // (guessed) Never set in this build: it is cleared again for
                // each flower put, so only the radius ends the growth.
                int bEnd = 0;

                for (long i = 0; i < nNum; i++) {
                    float fRadius = m_fRadius + bcs::BCS_RandRange(-0.2f, 0.2f);
                    float fRot = WrapRad(fAngle + bcs::BCS_RandRange(-0.3f * fStep, 0.3f * fStep));
                    int bPut = 1;
                    bcs::FVEC3 ofs;
                    bcs::BCS_VecZ_RotY(ofs, fRadius, fRot);

                    bcs::FVEC3 pos(ofs);
                    bcs::FVEC3Add(&pos, &pos, &center);
                    bcs::FVEC3 normal;
                    float fHitTime;
                    if (CCollMan::DetectMapHit(pos, normal, fHitTime, pos, pos, 0.1f, 1, nHitFlag)) {
                        bcs::FVEC3Sub(&pos, &pos, &center);
                        if (DistSqXZ(ofs, pos) <= 1.0f) {
                            ofs = pos;
                        } else {
                            bPut = 0;
                        }
                    }

                    if (bPut) {
                        bEnd = 0;
                        new (0x100) CcDaisyFlowerUnit(this, m_pGroup, ofs, m_fWait);
                    }

                    fAngle = WrapRad(fAngle + fStep);
                }

                m_fWait += 2.0f;
                m_fAngle = WrapRad(m_fAngle + fStep * 0.5f);
                m_fRadius += 1.4f;
                if (m_fRadius > 6.0f || bEnd) {
                    ChangeState(STATE_LOOP);
                    break;
                }
                m_fTime -= 2.0f;
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

        // Every flower starts to wilt (after its ring's delay).
        void CcDaisyFlowerGarden::CtorStateDisappear()
        {
            if (m_pGroup) {
                CEffectCtrl::SimpleEffect(0x57F, m_Pos, 3, 0, 0);
            }
            StopSnd_();
            CGameSnd::Play(0x409, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);

            CcDaisyFlowerUnit *pUnit = GetNext(0);
            while (pUnit) {
                pUnit->StartDisappear();
                pUnit = pUnit ? GetNext(pUnit) : 0;
            }
        }

        // Ends once every flower and its models are gone.
        void CcDaisyFlowerGarden::LoopStateDisappear()
        {
            if (nw4r::ut::List_GetSize(&m_List) <= 0) {
                if (m_pGroup) {
                    if (m_pGroup->GetCount() == 0) {
                        Delete();
                    }
                } else {
                    Delete();
                }
            }
        }

        void CcDaisyFlowerGarden::DtorStateDisappear()
        {
        }

        // Messages 0xD and 0xE make the garden go.
        long CcDaisyFlowerGarden::SendMessage(long nMsg, long, long, long)
        {
            if ((nMsg == 0xD || nMsg == 0xE) && m_nState != STATE_DISAPPEAR) {
                ChangeState(STATE_DISAPPEAR);
            }
            return 0;
        }
    }
}
