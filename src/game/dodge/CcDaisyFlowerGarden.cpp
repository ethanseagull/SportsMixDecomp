// msw::CChrCoreDodgeDaisy (Daisy) in the dodgeball game, and her special
// shot's flower garden (CC03_DODGE::CcDaisyFlowerGarden).
//
// The same garden as BasketF's (game/basket/CcDaisyFlowerGarden.cpp), planted
// in the middle of the court she attacks. Here the fence is the court's extra
// collision (a circle that keeps the characters out) instead of a body
// collision, and it has states of its own: it stands at its full radius, can
// be told to shrink (message 0x44; in this build it keeps the same radius)
// and goes away with the garden. The rings of flowers stop at the fence.
// (The include order steers the order of the weak vtables in .data: the
// original's has bcs::CUnivTask, bcs::CState and msw::CGameTask defined
// before the character classes.)
#include "bcs/CUnivTask.hpp"
#include "bcs/CFsm.hpp"
#include "game/msw/CGameTask.hpp"
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
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
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGameTask.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/ut/List.hpp"

namespace {

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // (names guessed) The fence's radius, and the radius of its effect at
    // scale 1.
    const float FENCE_RADIUS = 4.3f;
    const float FENCE_EFFECT_RADIUS = 4.5f;

    // (guessed helpers) An angle brought into [0, 2 pi); the larger of two
    // values; fValue kept within [fMin, fMax]; from fFrom to fTo by fRate.
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

    inline float Max(float fValue, float fMin)
    {
        return fMin < fValue ? fValue : fMin;
    }

    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    inline float Lerp(float fFrom, float fTo, float fRate)
    {
        return fFrom + (fTo - fFrom) * fRate;
    }

    // (guessed helper) The squared distance of two points seen from above.
    inline float DistSqXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3DistSq(&pos1, &pos2);
    }

    // (name guessed) 60 degrees; nothing in the unit reads it.
    float s_fUnkRange = bcs::DEG_TO_RAD(60.0f);
}

// (Like the other DodgeF character classes, the namespace is not inside
// msw: the mangled names read CC03_DODGE::CcDaisyFlowerGarden.)
namespace CC03_DODGE {

    class CcDaisyFlowerGarden;

    // The fence around the garden: an effect and the extra collision of
    // its court. 0x90 bytes. (member names guessed)
    class CcDaisyFenceUnit : public msw::CGameTask, public bcs::CFsm<CcDaisyFenceUnit> {
        public:
            enum {
                STATE_APPEAR,
                STATE_REDUCE, // shrinks to its end radius
                STATE_DISAPPEAR,
                STATE_NUM
            };

            CcDaisyFenceUnit(bcs::CTask *pParent, long nColl, const bcs::FVEC3 &rPos, float fRotY);
            virtual ~CcDaisyFenceUnit();
            virtual void Main();

            long GetState() const { return m_nState; }

            // The effect and the collision follow the garden.
            void SetPos(const bcs::FVEC3 &rPos)
            {
                if (m_hEffect) {
                    msw::CEffectCluster *pCluster = msw::CEffectCtrl::GetObject(m_hEffect);
                    if (pCluster) {
                        bcs::FVEC3 pos(rPos);
                        pCluster->SetTranslate(&pos);
                    }
                }
                msw::CCollMan::SetExCollPos(m_nColl, rPos);
            }

        private:
            void InitState();
            void CreateMdl_(const bcs::FVEC3 &rPos, float fRotY);

            void CtorStateAppear();
            void LoopStateAppear();
            void DtorStateAppear();
            void CtorStateReduce();
            void LoopStateReduce();
            void DtorStateReduce();
            void CtorStateDisappear();
            void LoopStateDisappear();
            void DtorStateDisappear();

            unsigned long m_hEffect; // 0x7C
            long m_nColl;            // 0x80, the extra collision (the court's index)
            long m_nUnk84;           // 0x84 (not used)
            float m_fTimer;          // 0x88
            float m_fScale;          // 0x8C, of the effect along X and Z
    };

    // One flower: an effect put in the garden's scene group. 0x4C bytes,
    // kept in its garden's list. (member names guessed)
    class CcDaisyFlowerUnit : public msw::CGameTask {
        public:
            inline CcDaisyFlowerUnit(CcDaisyFlowerGarden *pGarden, bcs::CScnGroup *pGroup, const bcs::FVEC3 &rPos,
                                     float fWait);
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

    // The garden: an obstacle of type 3 with its fence, a scene group for
    // the flowers and the list of them. 0x114 bytes. (member names guessed)
    class CcDaisyFlowerGarden : public msw::CChrObstacle, public bcs::CFsm<CcDaisyFlowerGarden> {
        public:
            enum {
                STATE_SLEEP,
                STATE_APPEAR, // the rings grow
                STATE_LOOP,
                STATE_DISAPPEAR,
                STATE_NUM
            };

            CcDaisyFlowerGarden(bcs::CTask *pParent, msw::CChrCore *pChr, long nCourt, const bcs::FVEC3 &rPos);
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
            CcDaisyFenceUnit *m_pFence;  // 0x110
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

msw::CChrCoreDodgeDaisy::CChrCoreDodgeDaisy()
{
}

msw::CChrCoreDodgeDaisy::~CChrCoreDodgeDaisy()
{
}

// The trick plants the garden and starts the 360-frame timer that ends with
// bit 1 of the work's flags cleared (see SubStateArtsShotIsEndArtsShotAfter_).
void msw::CChrCoreDodgeDaisy::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    pWork->m_nFlag04 |= 4;
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x6D, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
    SubStateArtsShotFireBullet_(pWork, 0);
    pWork->m_fUnkD8 = 360.0f;
}

void msw::CChrCoreDodgeDaisy::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0x6E, 3);
}

void msw::CChrCoreDodgeDaisy::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0x6F, 3);
}

void msw::CChrCoreDodgeDaisy::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLandMotion_(pWork);

    ArtsOptSetFadeOutSE(0x40A);
    ArtsOptFadeOut(5.0f, 0.0f, 0);
}

// Step 0 waits for the shot motion and starts motion 0x80; step 1 starts
// motion 0x81 once bit 3 of the work's flags is set, the game allows it (bit
// 0 or 6 of the game flags) or bit 1 is clear, clearing bits 1 and 2. As
// the Moogle's: meanwhile she falls, turns and moves along the trick's path
// until the move is over, and counts down the timer started with the trick.
// Later steps are the common ones.
int msw::CChrCoreDodgeDaisy::SubStateArtsShotIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    switch (pWork->m_nAfterStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x80, 1, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0x70, 3);
                pWork->m_nAfterStep++;
            }
            break;
        case 1:
            if ((pWork->m_nFlag04 & 8) || (CGame::g_GameWork.m_nFlag18 & 1) ||
                (CGame::g_GameWork.m_nFlag18 & 0x40) || !(pWork->m_nFlag04 & 2)) {
                pWork->m_nFlag04 &= ~6;
                Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
                Eff_Stop(0x14);
                AnimPlay(3, 0x81, 0, 8.0f, 0.0f);
                pWork->m_nAfterStep++;
            }
            break;
        default:
            return CChrCoreDodge::SubStateArtsShotIsEndArtsShotAfter_(pWork);
    }

    int nJump = Util_DoJumpMoveEX(0);
    if (m_fJumpVelY <= 0.0f) {
        m_fJumpVelY = 0.0f;
        m_fJumpUnk224 += m_fFrameStep * m_fTimeScale;
    }
    if (nJump == 1) {
        RotateYParam_(pWork->m_RotLerp);
        if (pWork->m_fUnkD4 < pWork->m_MoveParam.m_fFrameMax) {
            pWork->m_fUnkD4 += m_fFrameStep * m_fTimeScale;
            pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_MovePos);
            Util_DoDirMove(pWork->m_MoveParam, pWork->m_fUnkD4, pWork->m_DirMove, 0);
        }
    }
    if (pWork->m_fUnkD8 > 0.0f) {
        pWork->m_fUnkD8 -= m_fFrameStep * m_fTimeScale;
        if (pWork->m_fUnkD8 <= 0.0f) {
            pWork->m_fUnkD8 = 0.0f;
            pWork->m_nFlag04 &= ~2;
        }
    }
    return 0;
}

// The garden is planted in the middle of the court she attacks.
void msw::CChrCoreDodgeDaisy::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK_CMN *, long)
{
    bcs::FVEC3 pos;
    bcs::FVEC4 rect;
    CStageMan::GetInstance()->GetTeamCourtRect(GetTargetCourtIdx(), &rect.x, &rect.y, &rect.z, &rect.w);
    pos.x = 0.5f * (rect.x + rect.y);
    pos.y = 0.0f;
    pos.z = 0.5f * (rect.z + rect.w);

    new (0x100) CC03_DODGE::CcDaisyFlowerGarden(m_pParentTask, this, GetTargetCourtIdx(), pos);
}

namespace CC03_DODGE {

    // Out of line, but auto-inlined into its only caller (the garden's
    // CtorStateAppear); the original's .rodata has its constants ahead of
    // CreateMdl_'s, so it was defined here, and its unused copy left to the
    // link to strip.
    CcDaisyFenceUnit::CcDaisyFenceUnit(bcs::CTask *pParent, long nColl, const bcs::FVEC3 &rPos, float fRotY)
        : CGameTask(pParent, -1), m_hEffect(0), m_nColl(nColl), m_nUnk84(0), m_fTimer(0.0f), m_fScale(1.0f)
    {
        InitState();
        m_fScale = FENCE_RADIUS / FENCE_EFFECT_RADIUS;
        CreateMdl_(rPos, fRotY);
        ChangeState(STATE_APPEAR);
    }

    CcDaisyFenceUnit::~CcDaisyFenceUnit()
    {
        if (m_hEffect) {
            msw::CEffectCtrl::StopAttachEffect(m_hEffect);
        }
        msw::CCollMan::EnableExColl(m_nColl, 0);
    }

    // An ordinary function that the constructor inlines (see the garden's).
    void CcDaisyFenceUnit::InitState()
    {
        InitFsm(this, STATE_NUM);
        SetState(STATE_APPEAR, &CcDaisyFenceUnit::CtorStateAppear, &CcDaisyFenceUnit::LoopStateAppear,
                 &CcDaisyFenceUnit::DtorStateAppear);
        SetState(STATE_REDUCE, &CcDaisyFenceUnit::CtorStateReduce, &CcDaisyFenceUnit::LoopStateReduce,
                 &CcDaisyFenceUnit::DtorStateReduce);
        SetState(STATE_DISAPPEAR, &CcDaisyFenceUnit::CtorStateDisappear, &CcDaisyFenceUnit::LoopStateDisappear,
                 &CcDaisyFenceUnit::DtorStateDisappear);
    }

    // The fence effect at rPos, turned by fRotY, and the collision there.
    void CcDaisyFenceUnit::CreateMdl_(const bcs::FVEC3 &rPos, float fRotY)
    {
        m_hEffect = msw::CEffectCtrl::CreateAttachEffect(0x539, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
        if (m_hEffect) {
            msw::CEffectCluster *pCluster = msw::CEffectCtrl::GetObject(m_hEffect);
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
        msw::CCollMan::SetExCollPos(m_nColl, rPos);
    }

    void CcDaisyFenceUnit::Main()
    {
        ExecState();
    }

    // The collision is switched on, one unit inside the effect.
    void CcDaisyFenceUnit::CtorStateAppear()
    {
        float fRadius = FENCE_RADIUS - 1.0f;
        fRadius = Max(fRadius, 1.0f);
        msw::CCollMan::EnableExColl(m_nColl, 1);
        msw::CCollMan::SetExCollRadius(m_nColl, fRadius);
    }

    void CcDaisyFenceUnit::LoopStateAppear()
    {
    }

    void CcDaisyFenceUnit::DtorStateAppear()
    {
    }

    void CcDaisyFenceUnit::CtorStateReduce()
    {
        m_fTimer = 0.0f;
    }

    // Over 100 frames from the full radius to the end radius (the same in
    // this build), the effect scaled to it.
    void CcDaisyFenceUnit::LoopStateReduce()
    {
        m_fTimer += GetFrameRateF();
        float fFrame = 100.0f;
        fFrame = Max(fFrame, 1.0f);
        if (m_fTimer > fFrame) {
            m_fTimer = fFrame;
        }
        float fRate = Clamp(m_fTimer / fFrame, 0.0f, 1.0f);
        float fRadius = Lerp(FENCE_RADIUS, FENCE_RADIUS, fRate);

        m_fScale = fRadius / FENCE_EFFECT_RADIUS;
        if (m_hEffect) {
            msw::CEffectCluster *pCluster = msw::CEffectCtrl::GetObject(m_hEffect);
            if (pCluster) {
                bcs::FVEC3 scale(m_fScale, 1.0f, m_fScale);
                pCluster->SetScale(&scale);
            }
        }
        msw::CCollMan::SetExCollRadius(m_nColl, Max(fRadius - 1.0f, 1.0f));
    }

    void CcDaisyFenceUnit::DtorStateReduce()
    {
    }

    // The collision goes at once, the effect is stopped and the fence ends
    // 5 frames later.
    void CcDaisyFenceUnit::CtorStateDisappear()
    {
        msw::CCollMan::EnableExColl(m_nColl, 0);
        m_fTimer = 0.0f;
        if (m_hEffect) {
            msw::CEffectCtrl::StopAttachEffect(m_hEffect);
            m_hEffect = 0;
            m_fTimer = 5.0f;
        }
    }

    void CcDaisyFenceUnit::LoopStateDisappear()
    {
        m_fTimer -= GetFrameRateF();
        if (m_fTimer <= 0.0f) {
            Delete();
        }
    }

    void CcDaisyFenceUnit::DtorStateDisappear()
    {
    }

    CcDaisyFlowerUnit::~CcDaisyFlowerUnit()
    {
        if (m_pGarden) {
            m_pGarden->Remove(this);
        }
        if (m_hEffect) {
            msw::CEffectCtrl::StopAttachEffect(m_hEffect);
        }
    }

    // One of the two flower effects at rPos, its models drawn over
    // everything and moved into pGroup.
    void CcDaisyFlowerUnit::CreateMdl_(bcs::CScnGroup *pGroup, const bcs::FVEC3 &rPos, float)
    {
        long nEffect = bcs::BCS_RandMax(1L) ? 0x537 : 0x538;
        if (nEffect >= 0) {
            m_hEffect = msw::CEffectCtrl::CreateAttachEffect(nEffect, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0);
        }
        if (m_hEffect) {
            msw::CEffectCluster *pCluster = msw::CEffectCtrl::GetObject(m_hEffect);
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

    // The garden at rPos in court nCourt.
    CcDaisyFlowerGarden::CcDaisyFlowerGarden(bcs::CTask *pParent, msw::CChrCore *, long nCourt,
                                             const bcs::FVEC3 &rPos)
        : CChrObstacle(pParent, 3, 0x17), m_nCourt(nCourt), m_fTimer(0.0f), m_fRadius(0.0f), m_fRingWait(0.0f),
          m_fStartRot(0.0f), m_hSnd(0), m_pFence(0)
    {
        m_LocalPos = rPos;
        bcs::FVEC3 ofs = msw::CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
        m_LocalPos.x -= ofs.x;
        m_LocalPos.z -= ofs.z;

        m_pGroup = new (0x100) bcs::CScnGroup(0x10, 0);
        m_pGroup->m_vTrans = rPos;
        m_pGroup->CalcRootMtx();
        bcs::CRenderMan::PushBack(m_pGroup, 5);

        nw4r::ut::List_Init(&m_List, 0x34);

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
    // module has no copy of it, only its second copy of the state table in
    // .data.
    void CcDaisyFlowerGarden::InitState()
    {
        InitFsm(this, STATE_NUM);
        SetState(STATE_SLEEP, &CcDaisyFlowerGarden::CtorStateSleep, &CcDaisyFlowerGarden::LoopStateSleep,
                 &CcDaisyFlowerGarden::DtorStateSleep);
        SetState(STATE_APPEAR, &CcDaisyFlowerGarden::CtorStateAppear, &CcDaisyFlowerGarden::LoopStateAppear,
                 &CcDaisyFlowerGarden::DtorStateAppear);
        SetState(STATE_LOOP, &CcDaisyFlowerGarden::CtorStateLoop, &CcDaisyFlowerGarden::LoopStateLoop,
                 &CcDaisyFlowerGarden::DtorStateLoop);
        SetState(STATE_DISAPPEAR, &CcDaisyFlowerGarden::CtorStateDisappear, &CcDaisyFlowerGarden::LoopStateDisappear,
                 &CcDaisyFlowerGarden::DtorStateDisappear);
        ChangeState(STATE_SLEEP);
    }

    // The garden and its fence follow the court.
    void CcDaisyFlowerGarden::Main()
    {
        if (m_pGroup) {
            bcs::FVEC3 pos = msw::CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
            bcs::FVEC3Add(&pos, &pos, &m_LocalPos);
            m_pGroup->m_vTrans = pos;
            m_pGroup->CalcRootMtx();
            if (m_pFence) {
                m_pFence->SetPos(pos);
            }
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

    // The fence goes up (turned round on court 0); the first ring grows in
    // the next frame.
    void CcDaisyFlowerGarden::CtorStateAppear()
    {
        msw::CEffectCtrl::SimpleEffect(0x535, m_LocalPos, 3, 0, 0);
        m_hSnd = msw::CGameSnd::Play(0x408, &m_LocalPos, 1, -1, 0.0f, 1.0f, 0.0f);
        m_fRingWait = 2.0f - GetFrameRateF();
        m_fTimer = 0.0f;
        m_pFence = new (0) CcDaisyFenceUnit(this, m_nCourt, m_LocalPos, m_nCourt ? 0.0f : PI);
    }

    // Every two frames a ring of flowers, 1.2 further out than the last and
    // starting half a step further round, with a flower about every 1.4
    // along it, a little scattered; until the rings pass the fence.
    void CcDaisyFlowerGarden::LoopStateAppear()
    {
        // static-order swap (notes/decisions.md): the game numbers
        // FLOWER_UNIT_RADIUS first and stores it second.
        static const float FLOWER_COLL_RADIUS = 0.1f;
        static const float FLOWER_UNIT_RADIUS = 2.0f;

        m_fRingWait += GetFrameRateF();
        unsigned long nHitFlag = (unsigned char)m_nCourt | 0x2000000;
        bcs::FVEC3 center(m_pGroup->m_vTrans);

        while (m_fRingWait > 0.0f) {
            float fLength = PI * (m_fRadius * 2.0f);
            long nNum = (long)(fLength * (1.0f / (FLOWER_UNIT_RADIUS * 0.7f))) + 1;
            float fStep = PI2 / nNum;
            float fRot = WrapRad(m_fStartRot);
            // (as in the original, nothing sets it, so only the radius ends
            // the growth)
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
                if (msw::CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, hitPos, hitPos, FLOWER_COLL_RADIUS, 5,
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
            m_fRadius += FLOWER_UNIT_RADIUS * 0.6f;
            if (m_fRadius > FENCE_RADIUS || bNoRoom) {
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

    // Every flower starts its wait to go, and the fence goes.
    void CcDaisyFlowerGarden::CtorStateDisappear()
    {
        if (m_pGroup) {
            msw::CEffectCtrl::SimpleEffect(0x536, m_LocalPos, 3, 0, 0);
        }
        StopSnd_();
        msw::CGameSnd::Play(0x409, m_LocalPos, 1, -1, 0.0f, 1.0f, 0.0f);

        for (CcDaisyFlowerUnit *pUnit = GetFirst(); pUnit; pUnit = GetNext(pUnit)) {
            pUnit->Vanish();
        }
        if (m_pFence && m_pFence->GetState() != CcDaisyFenceUnit::STATE_DISAPPEAR) {
            m_pFence->ChangeState(CcDaisyFenceUnit::STATE_DISAPPEAR);
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

    // Messages 0xD and 0xE make the garden go; 0x44 makes the fence shrink.
    long CcDaisyFlowerGarden::SendMessage(long nMsg, long, long, long)
    {
        switch (nMsg) {
            case 0xD:
            case 0xE:
                if (GetState() != STATE_DISAPPEAR) {
                    ChangeState(STATE_DISAPPEAR);
                }
                break;
            case 0x44:
                if (m_pFence && m_pFence->GetState() == CcDaisyFenceUnit::STATE_APPEAR) {
                    m_pFence->ChangeState(CcDaisyFenceUnit::STATE_REDUCE);
                }
                break;
        }
        return 0;
    }
}
