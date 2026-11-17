// msw::CNpcBossPackunBasketBase: the boss Piranha Plant (Petey) of the
// Hungry Boss Packun basketball game. Two of them stand behind the goals and
// carry them; each opens and shuts its mouth in time with a table (the lip
// sync) and swallows the shots that go in, bites at players dunking at it,
// throws mud bombs, falls asleep and melts. Also here: its mud bomb
// (CMudBombBasket), the normal game's boss (CNpcBossPackunBasketGame) and the
// task that makes the two of them sleep or throw mud
// (CNpcBossPackunBasketGameMan), the party stages' bosses
// (CNpcBossPackunBasketParty70 to 72) and CChrManBasket::CreateBossPackun_.
#include "game/msw/CNpcBossPackunBasketBase.hpp"

#include <math.h>
#include <stdlib.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CFgMdl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/g3d/ResNode.hpp"

// Set by the file's static initialiser and used by nothing in the file (see
// game/basket/CBallBasket.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

namespace msw {

    namespace {
        // The boss's sounds, one table each, by GetType (the game, then the
        // party stages 70 to 72); -1 for none.
        const short s_aSeSleep[4] = {0xD3A, 0xDC6, 0xDC6, 0xDC6};
        const short s_aSeWakeUp[4] = {0xD3B, 0xDC7, 0xDC7, 0xDC7};
        const short s_aSeAttack[4] = {0xD3C, 0xDC8, 0xDC8, 0xDC8};
        const short s_aSeEat[4] = {0xD3D, 0xDD2, 0xDD2, 0xDD2};
        const short s_aSeMouthOpen[4] = {0xD3F, 0xDCA, 0xDCA, 0xDCA};
        const short s_aSeMouthClose[4] = {0xD40, 0xDCB, 0xDCB, 0xDCB};
        const short s_aSeMudAttack[4] = {0xD41, -1, -1, -1};
        const short s_aSeMudExplode[4] = {0xD42, -1, -1, -1};
        const short s_aSeAttackHit[4] = {0xD43, 0xDCC, 0xDCC, 0xDCC};
        const short s_aSeEatBig[4] = {0xD44, 0xDCD, 0xDCD, 0xDCD};
        const short s_aSeAttackEnd[4] = {0xD45, -1, -1, -1};
        const short s_aSeEatBig2[4] = {-1, 0xDD1, 0xDD1, 0xDD1};

        // The angle brought into [0, 2 pi).
        inline float RoundRad(float fRad)
        {
            while (fRad > 6.2831855f) {
                fRad -= 6.2831855f;
            }
            while (fRad < 0.0f) {
                fRad += 6.2831855f;
            }
            return fRad;
        }

        // The angle brought into (-pi, pi].
        inline float RoundRadPI(float fRad)
        {
            fRad = RoundRad(fRad);
            if (fRad > 3.1415927f) {
                fRad -= 6.2831855f;
            }
            return fRad;
        }

        // rTo - rFrom and rA + rB. (the paired-single result goes to a
        // temporary that is then copied out)
        inline bcs::FVEC3 VecSub(const bcs::FVEC3 &rTo, const bcs::FVEC3 &rFrom)
        {
            bcs::FVEC3 vSub;
            return *bcs::FVEC3Sub(&vSub, &rTo, &rFrom);
        }

        inline bcs::FVEC3 VecAdd(const bcs::FVEC3 &rA, const bcs::FVEC3 &rB)
        {
            bcs::FVEC3 vAdd;
            return *bcs::FVEC3Add(&vAdd, &rA, &rB);
        }

        // rIn's direction in the XZ plane, length 1 (zero for zero).
        inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
        {
            bcs::BCS_Vec_Normalize(rOut, bcs::FVEC3(rIn.x, 0.0f, rIn.z));
        }

        inline float Clamp(float fValue, float fMin, float fMax)
        {
            return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
        }

        // The animation flags CMdlObj::Create takes for the animations of
        // the file.
        inline unsigned long GetAnmFlag(const nw4r::g3d::ResFile &file)
        {
            unsigned long nFlag = 0;
            file.HasResAnmChr(); // (tested, but adds no flag)
            if (file.HasResAnmClr()) {
                nFlag |= 0x108;
            }
            if (file.HasResAnmTexSrt()) {
                nFlag |= 0x204;
            }
            if (file.HasResAnmTexPat()) {
                nFlag |= 0x3;
            }
            if (file.HasResAnmShp()) {
                nFlag |= 0x7000;
            }
            return nFlag;
        }

        // Plays sound nSe at rPos; nothing for -1.
        inline void PlaySeAt(long nSe, const bcs::FVEC3 &rPos)
        {
            if (nSe >= 0) {
                CGameSnd::Play(nSe, rPos, 1, -1, 0.0f, 1.0f, 0.0f);
            }
        }
    }

    // (inline: the original's copy is weak, at the end of the file, but
    // its table of node names comes first in .rodata)
    inline void CNpcBossPackunBasketBase::CreateMdlObj_()
    {
        // The resources of the NPC slot of the boss (type 2).
        long nSlot = -1;
        for (long i = 0; i < 8; i++) {
            if (CGame::GetNpcType(i) == 2) {
                nSlot = i;
                break;
            }
        }
        if (nSlot < 0) {
            return;
        }
        nw4r::g3d::ResFile mdlFile = CLoadData::GetRes(nSlot + 0xB);
        nw4r::g3d::ResFile anmFile = CLoadData::GetRes(nSlot + 0x1B);
        nw4r::g3d::ResMdl resMdl = mdlFile.GetResMdl("body_h");

        m_pMdl = NewMdlObj_(resMdl, GetAnmFlag(mdlFile));
        m_pMdl->OnFlag(0x100);
        m_pMdl->OnFlag(2);
        m_pMdl->SetExBlendMode(1);
        m_pMdl->OnFlag(0x2000);
        m_pAnm = new (0) bcs::CAnmMotion;
        m_pAnm->Create(m_pMdl, anmFile, CC_DATA::GetNpcMdl_NodeExInfo(2), 3, 0);
        m_pMdl->AttachAnmObj(m_pAnm, bcs::CMdlObj::ANMOBJTYPE_CHR);
        bcs::CRenderMan::PushBack(m_pMdl, 2);

        // The jaws.
        const char *apNode[2] = {"B_kuchi_jnt", "U_kuchi_jnt"};
        for (long i = 0; i < 2; i++) {
            nw4r::g3d::ResNode node = resMdl.GetResNode(apNode[i]);
            if (node.IsValid()) {
                // (the original keeps the index as a signed byte)
                m_anMouthNode[i] = (signed char)node.GetID();
            } else {
                m_anMouthNode[i] = 0;
            }
        }

        nw4r::g3d::ResMdl shadowMdl = mdlFile.GetResMdl("body_s");
        if (!shadowMdl.IsValid()) {
            shadowMdl = mdlFile.GetResMdl("body_h");
        }
        if (shadowMdl.IsValid()) {
            m_pShadow = new bcs::CMdlShadow(m_pMdl, shadowMdl, 0x100, 0, 0);
            bcs::CRenderMan::PushBack(m_pShadow, 2);
        }

        // The model seen from the player's side.
        if (CGame::g_GameWork.GetStage() == 7) {
            nw4r::g3d::ResMdl subMdl = mdlFile.GetResMdl("p1_h");
            m_pSubMdl = static_cast<CFgMdl *>(NewMdlObj_(subMdl, GetAnmFlag(mdlFile)));
            m_pSubMdl->OnFlag(0x100);
            m_pSubMdl->OnFlag(2);
            m_pSubMdl->SetExBlendMode(1);
            m_pSubMdl->OnFlag(0x2000);
            m_pSubAnm = new (0) bcs::CAnmMotion;
            m_pSubAnm->Create(m_pSubMdl, anmFile, CC_DATA::GetNpcMdl_NodeExInfo(2), 3, 0);
            m_pSubMdl->AttachAnmObj(m_pSubAnm, bcs::CMdlObj::ANMOBJTYPE_CHR);
            bcs::CRenderMan::PushBack(m_pSubMdl, 2);
        }
        CalcRoot_();
    }

    CNpcBossPackunBasketGameMan *CNpcBossPackunBasketGameMan::g_pInstance;

    // ---------------------------------------------------------------------
    // CNpcBossPackunBasketBase: helpers
    // ---------------------------------------------------------------------

    inline void CNpcBossPackunBasketBase::MotSetRate_(float fRate)
    {
        if (m_pAnm) {
            m_pAnm->SetRate(fRate);
        }
        if (m_pSubAnm) {
            m_pSubAnm->SetRate(fRate);
        }
    }

    inline float CNpcBossPackunBasketBase::GetMotRateFit_(long nMotID, float fFrame)
    {
        return Clamp(AnimGetResFrameNum(nMotID) / fFrame, 1.0f, 3.0f);
    }

    inline void CNpcBossPackunBasketBase::PlaySe_(const short *pSe)
    {
        PlaySePos(pSe[GetType()], &m_vPos, 0, -1, 0.0f, 1.0f, 0.0f);
    }

    inline void CNpcBossPackunBasketBase::ResetState_(long nState)
    {
        FsmqClear();
        FsmqChangeState(nState);
    }

    inline void CNpcBossPackunBasketBase::ChangeStateIdle_()
    {
        ResetState_(STATE_IDLE);
    }

    inline void CNpcBossPackunBasketBase::NextLipIdx_()
    {
        if (++m_nLipIdx >= GetLipSecondIdxNum()) {
            m_nLipIdx = 0;
        }
    }

    inline float CNpcBossPackunBasketBase::LimitRotY_(float fRotY, long nMaxDeg) const
    {
        float fDiff = fRotY - m_fBaseRotY;
        float fMax = 0.017453292f * nMaxDeg;
        fDiff = RoundRadPI(fDiff);
        if (fabsf(fDiff) > fMax) {
            fDiff = fDiff > 0.0f ? fMax : -fMax;
        }
        return RoundRad(fDiff + m_fBaseRotY);
    }

    inline void CNpcBossPackunBasketBase::OnStart_()
    {
        m_nFlag |= 1;
        m_nPackunFlag &= ~2;
        if (CGame::g_GameWork.GetStage() == 7) {
            long nTeam = CGame::GetViewTargetTeam(0);
            int bFacing = CGame::GetTargetGoalIdx(nTeam) != GetGoalIdx();
            if (bFacing) {
                m_nPackunFlag |= 2;
            } else {
                m_nPackunFlag &= ~2;
            }
        }
    }

    inline void CNpcBossPackunBasketBase::SleepStart_()
    {
        STATE_WORK_SLEEP *pWork = FsmqGetWork<STATE_WORK_SLEEP>();
        pWork->m_nStep = 0;
        pWork->m_bEffect = 0;
        SE_PlayPos(0, s_aSeSleep[GetType()], &m_vPos, 0, -1, 0.0f, 1.0f, 0.0f);
    }

    inline void CNpcBossPackunBasketBase::SleepEnd_()
    {
        Eff_Stop(8);
        Eff_Stop(9);
        SE_Stop(0, 0.0f);
        if (CGameBasket::GetInstance()->IsNowMainLoopEnable_()) {
            PlaySe_(s_aSeWakeUp);
        }
        Eff_Stop(11);
        Eff_CreateAttachAuto(11, 0xD6, 3);
    }

    // ---------------------------------------------------------------------
    // CNpcBossPackunBasketBase
    // ---------------------------------------------------------------------

    // The z of the base position: behind the goal, at the end of the court.
    inline float CNpcBossPackunBasketBase::GetBasePosZ_()
    {
        float fOfs = BASKET_PRM_S07_PACKUN_OFFSET_Z;
        if (GetGoalIdx() == 0) {
            return CStageMan::GetInstance()->GetTeamCourtFront(1) - fOfs;
        }
        return fOfs + CStageMan::GetInstance()->GetTeamCourtBack(0);
    }

    // (Expanded into CChrManBasket::CreateBossPackun_; the out-of-line copy
    // is stripped by the module link.)
    CNpcBossPackunBasketBase::CNpcBossPackunBasketBase(CChrMan *pMan, const bcs::FVEC3 &rPos, long nIdx)
        : CNpcBossPackun(pMan)
        , m_pSubAnm(0)
        , m_nGoalIdx(nIdx)
        , m_nLipIdx(0)
        , m_bEnd(0)
    {
        m_fBaseRotY = nIdx == 0 ? 0.0f : 3.1415927f;
        float fZ = GetBasePosZ_();
        m_vBasePos.x = rPos.x;
        m_vBasePos.y = rPos.y;
        m_vBasePos.z = fZ;
    }

    long CNpcBossPackunBasketBase::GetGoalIdx() const
    {
        return m_nGoalIdx;
    }

    CNpcBossPackunBasketBase::~CNpcBossPackunBasketBase()
    {
        m_pSubAnm = 0;
    }

    void CNpcBossPackunBasketBase::Create()
    {
        CNpcBossPackun::Create();

        m_pBodyColl = new (0) CBodyCollision(3, 3, 0, 0);

        // Unit 0: the head, pushes the characters away.
        m_pBodyColl->SetPushEnable(0);
        m_pBodyColl->SetCollType(0, 0);
        m_pBodyColl->SetPos(0, m_vBasePos);
        m_pBodyColl->SetPushRank(0, 5);
        m_pBodyColl->SetPushPower(0, 6);
        m_pBodyColl->SetTargetFlag(0, 6);
        m_pBodyColl->SetEnable(0);

        // Unit 1: the body, pushes the characters away.
        m_pBodyColl->SetPushEnable(1);
        m_pBodyColl->SetCollType(1, 2);
        m_pBodyColl->SetPushRank(1, 5);
        m_pBodyColl->SetPushPower(1, 6);
        m_pBodyColl->SetTargetFlag(1, 6);
        m_pBodyColl->SetEnable(1);
        m_pBodyColl->SetBodyPushCallbackAll(CallbackBodyPush, this);
        m_pBodyColl->SetPos(1, bcs::FVEC3(m_vBasePos.x, 0.0f, m_vBasePos.z));
        m_pBodyColl->SetSize(1, 2.5f, 0.5f, 1.5f);

        // Unit 2: the mouth over the goal, catches the shots.
        m_pBodyColl->SetCatchEnable(2);
        m_pBodyColl->SetCollType(2, 0);
        m_pBodyColl->SetPos(2, CStageMan::GetInstance()->GetGoalPos(GetGoalIdx()));
        m_pBodyColl->SetBallCatchCallbackAll(CallbackBallCatch, this);
        m_pBodyColl->SetEnable(2);

        // The bite.
        m_pAtkColl = new (0) CAtkCollision(1, 3, 0, 0);
        m_pAtkColl->SetAttackHitCallbackAll(CallbackAttackHit, this);
        m_pAtkColl->SetTargetFlag(0, 6);
        m_pAtkColl->SetDisableAll();
        m_pAtkColl->SetCollTypeAll(0);
    }

    void CNpcBossPackunBasketBase::Reset()
    {
        m_vPos.x = m_vBasePos.x;
        m_vPos.y = 0.0f;
        m_vPos.z = m_vBasePos.z;
        SetRotY(m_fBaseRotY);
        m_nPackunFlag &= ~1;
        ResetState_(STATE_IDLE);
        m_pAtkColl->SetDisableAll();
        m_nFlag = 0;
        m_nPackunFlag &= ~2;
        m_bEnd = 0;
        m_nLipIdx = 0;
        MotPlay_(MOT_IDLE, 1, 0.0f, 0.0f);
    }

    void CNpcBossPackunBasketBase::MainFunc()
    {
        // A function-local static, not a literal: the original reads it from
        // its own .rodata object, not from the float pool.
        static const float fDistAlpha = 0.0f;

        ActInterrup();

        // The models fade out when the view looks at them from behind.
        CFgMdl *pMdl = static_cast<CFgMdl *>(m_pMdl);
        if (pMdl) {
            pMdl->SetFadeDir(GetGoalIdx() == 1 ? 3.1415927f : 0.0f);
            pMdl->SetFadeAngle1(1.5707964f);
            pMdl->SetFadeAngle0(1.5707964f);
            pMdl->SetUnk218(fDistAlpha);
            pMdl->SetAlphaTestNearRange(-0.3f);
            pMdl->SetAlphaTestFarRange(0.3f);
            pMdl->SetUnk214(fDistAlpha);
        }
        CFgMdl *pSubMdl = m_pSubMdl;
        if (pSubMdl) {
            pSubMdl->SetFadeDir(GetGoalIdx() == 1 ? 0.0f : 3.1415927f);
            pSubMdl->SetFadeAngle1(1.5707964f);
            pSubMdl->SetFadeAngle0(1.5707964f);
            pSubMdl->SetUnk218(fDistAlpha);
        }

        CNpcBossPackun::MainFunc();
        UpDateCollBody_();
        UpDateCollAtk_();
    }

    void CNpcBossPackunBasketBase::ActInterrup()
    {
        if (CChrManBasket::GetInstance()->GetFlagAC() & 2) {
            MethdFears();
        }
    }

    bcs::CMdlObj *CNpcBossPackunBasketBase::NewMdlObj_(nw4r::g3d::ResMdl mdl, unsigned long heap)
    {
        CFgMdl *pMdl = new CFgMdl();
        if (pMdl) {
            pMdl->SetFadeDir(0.0f);
            pMdl->SetFadeAngle1(1.3962634f);
            pMdl->SetFadeAngle0(1.5707964f);
            pMdl->SetUnk214(0.0f);
            pMdl->SetAlphaTestNearRange(BASKET_PRM_S07_PACKUN_NEAR_RANG);
            pMdl->SetAlphaTestFarRange(BASKET_PRM_S07_PACKUN_FAR_RANG);
            pMdl->SetUnk218(0.0f);
            pMdl->SetExBlendMode(1);
            pMdl->CalcBoundingVolume();
            pMdl->Create(mdl, heap, 0);
            return pMdl;
        }
        return 0;
    }

    void CNpcBossPackunBasketBase::CheckDisp_()
    {
        // The second model shows (instead of the shadows) once the game
        // started, when the boss faces the player's view.
        int bSub = (m_nPackunFlag >> 1) & 1;
        if (!(m_nFlag & 1)) {
            bSub = 0;
        }
        if (m_pSubMdl) {
            if (!bSub) {
                m_pSubMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            } else {
                m_pSubMdl->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
            m_pSubMdl->SetAlphaRateF(m_fAlpha);
        }
        int bShadow = !bSub;
        if (m_pMdl) {
            m_pMdl->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            m_pMdl->SetAlphaRateF(m_fAlpha);
        }
        if (m_pShadow2) {
            if (!bShadow) {
                m_pShadow2->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            } else {
                m_pShadow2->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
            m_pShadow2->SetAlphaRateF(0.1f);
        }
        if (m_pShadow) {
            if (!bShadow) {
                m_pShadow->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            } else {
                m_pShadow->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
            m_pShadow->SetAlphaRateF(0.1f);
        }
    }

    float CNpcBossPackunBasketBase::CalcAttackDir_()
    {
        float fRotY = m_fBaseRotY;
        float fSum = 0.0f;
        long nNum = 0;

        // The mean direction of the shots in flight...
        for (long i = 0; i < CBallMan::GetBallNum(); i++) {
            CBall *pBall = CBallMan::GetBallPtr(i);
            if (pBall && pBall->IsActive() && pBall->IsShootMode()) {
                fSum += RoundRadPI(GetRotY_ToPosXZ(pBall->GetPos()) - fRotY);
                nNum++;
            }
        }
        // ...and of the characters dunking.
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            const CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && !pChr->IsDisable()) {
                long nState = pChr->GetState();
                if (nState == CChrCore::STATE_DUNK || nState == CChrCore::STATE_ALLEYOOP ||
                    nState == CChrCore::STATE_REBOUND_DUNK) {
                    fSum += RoundRadPI(GetRotY_ToPosXZ(pChr->GetPos()) - fRotY);
                    nNum++;
                }
            }
        }
        if (nNum != 0) {
            fRotY = RoundRad(fRotY + fSum / nNum);
        }
        return LimitRotY_(fRotY, BASKET_PRM_S07_PACKUN_MAX_DIR);
    }

    long CNpcBossPackunBasketBase::SendMessage(long nMsg, long n1, long n2, long n3, long n4)
    {
        CNpcBossPackunBasketGameMan *pMan = CNpcBossPackunBasketGameMan::GetInstance();
        if (pMan) {
            pMan->OnMessage(nMsg);
        }
        switch (nMsg) {
            case 0x3:
            case 0xF:
            case 0x13:
                Reset();
                break;
            case 0x12:
                if (FsmqGetState() != STATE_END_IDLE) {
                    Reset();
                }
                break;
            case 0x10:
                OnStart_();
                break;
            case 0x36:
                MethdEat(n1, n2, n3);
                break;
            case 0xB:
                SetRotY(m_fBaseRotY);
                m_nPackunFlag &= ~2;
                break;
            case 0x30:
                OnStart_();
                CheckDisp_();
                break;
            case 0xC:
                OnStart_();
                break;
            case 0x31:
                MethdEndIdle();
                break;
            case 0x11:
                m_bEnd = 1;
                break;
            case 0x24:
                switch (n1) {
                    case 0:
                        if (n2 == GetGoalIdx()) {
                            MethdSleep();
                        }
                        break;
                    case 1:
                        if (n2 == GetGoalIdx()) {
                            MethdMelt();
                        }
                        break;
                }
                break;
        }
        return CNpcBossPackun::SendMessage(nMsg, n1, n2, n3, n4);
    }

    void CNpcBossPackunBasketBase::UpDateCollAtk_()
    {
        if (CChrManBasket::GetInstance()->GetFlagAC() & 2) {
            return;
        }

        // The bite hits only while the mouth is shut.
        int bClose = !m_pBodyColl->IsEnable(2);
        if (m_pBodyColl->IsEnable(2)) {
            m_pAtkColl->SetTargetFlag(0, 2);
        } else {
            m_pAtkColl->SetTargetFlag(0, 6);
        }
        if (!bClose) {
            m_pAtkColl->SetDisable(0);
            return;
        }
        m_pAtkColl->SetEnable(0);

        unsigned long nNoHit = 0;
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && !pChr->IsDisable()) {
                CBall *pBall = GetAttachBall();
                if (pBall) {
                    nNoHit |= 1 << (pBall->GetBallNo() + 8);
                }
                // Only the characters dunking at the boss (or blocking while
                // it bites) can be bitten.
                switch (pChr->GetState()) {
                    case CChrCore::STATE_DUNK:
                        if (!pChr->IsHoldBall()) {
                            nNoHit |= 1 << pChr->GetPlayerNo();
                        }
                        break;
                    case CChrCore::STATE_JUMP_BLOCK:
                    case CChrCore::STATE_SWAT_IT:
                        if (FsmqGetState() == STATE_ATTACK) {
                            break;
                        }
                        // fall through
                    default:
                        nNoHit |= 1 << pChr->GetPlayerNo();
                        break;
                    case CChrCore::STATE_ALLEYOOP:
                    case CChrCore::STATE_REBOUND_DUNK:
                        break;
                }
            }
        }
        if (FsmqGetState() != STATE_ATTACK) {
            m_pAtkColl->SetNoHitFlag(0, nNoHit);
        } else {
            m_pAtkColl->SetNoHitFlag(0, 0);
        }

        if (m_pAtkColl->IsEnable(0)) {
            bcs::FVEC3 vHead;
            GetVNodePos(0, vHead);
            m_pAtkColl->SetRadius(0, BASKET_PRM_S07_PACKUN_CHR_ATK_HEAD_COL_R);
            m_pAtkColl->SetPos(0, vHead);
        }
    }

    void CNpcBossPackunBasketBase::UpDateCollBody_()
    {
        long nMot = GetMotIdx();
        int bShotOnly = 1;
        if (nMot == MOT_EAT_BIG || nMot == MOT_EAT) {
            bShotOnly = 0;
        }

        // The characters dunking and the balls pass through the head.
        unsigned long nNoHit = 0;
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && !pChr->IsDisable()) {
                switch (pChr->GetState()) {
                    case CChrCore::STATE_AIR_TRICK:
                        if (pChr->IsAirTrickDunkMode()) {
                            nNoHit |= 1 << pChr->GetPlayerNo();
                        }
                        break;
                    case CChrCore::STATE_DUNK:
                    case CChrCore::STATE_ALLEYOOP:
                    case CChrCore::STATE_REBOUND_DUNK:
                        if (!static_cast<CChrCoreBasket *>(pChr)->IsStateDunkStepStart()) {
                            nNoHit |= 1 << pChr->GetPlayerNo();
                        }
                        break;
                }
            }
        }
        for (long i = 0; i < CBallMan::GetBallNum(); i++) {
            CBall *pBall = CBallMan::GetBallPtr(i);
            if (pBall && pBall->IsActive()) {
                if (!bShotOnly || ((pBall->IsShootMode() || pBall->IsArtsShootMode()) && !pBall->IsAirBall())) {
                    nNoHit |= 1 << (pBall->GetBallNo() + 8);
                }
            }
        }
        m_pBodyColl->SetNoHitFlag(0, nNoHit);
        m_pBodyColl->SetRadius(0, BASKET_PRM_S07_PACKUN_PUSH_COL_R);
        float fOfsZ;
        if (GetGoalIdx() == 0) {
            fOfsZ = BASKET_PRM_S07_PACKUN_PUSH_COL_OFFSET_Z;
        } else {
            fOfsZ = -BASKET_PRM_S07_PACKUN_PUSH_COL_OFFSET_Z;
        }
        bcs::FVEC3 vOfs(0.0f, 0.0f, fOfsZ);
        bcs::FVEC3 vPos = VecAdd(m_vBasePos, vOfs);
        m_pBodyColl->SetPos(0, vPos);

        // The mouth catches while it is open.
        switch (nMot) {
            case MOT_MOUTH_OPEN:
                if (AnimGetFrame() < 0.9f * AnimGetFrameNum()) {
                    m_pBodyColl->SetDisable(2);
                    break;
                }
                // fall through
            case MOT_MOUTH_OPEN_LOOP:
                m_pBodyColl->SetEnable(2);
                break;
            case MOT_MOUTH_CLOSE:
                if (AnimGetFrame() < 0.35f * AnimGetFrameNum()) {
                    m_pBodyColl->SetEnable(2);
                } else {
                    m_pBodyColl->SetDisable(2);
                }
                break;
            case MOT_SLEEP_START:
                if (AnimGetFrame() < 0.7f * AnimGetFrameNum()) {
                    m_pBodyColl->SetDisable(2);
                    break;
                }
                // fall through
            case MOT_SLEEP:
            case MOT_PARTY_EAT:
                m_pBodyColl->SetEnable(2);
                break;
            case MOT_MELT:
                if (AnimGetFrame() <= 0.8f * AnimGetFrameNum()) {
                    m_pBodyColl->SetEnable(2);
                } else {
                    m_pBodyColl->SetDisable(2);
                }
                break;
            case MOT_FEARS:
                m_pBodyColl->SetEnable(2);
                break;
            default:
                m_pBodyColl->SetDisable(2);
                break;
        }
        m_pBodyColl->SetRadius(2, BASKET_PRM_S07_PACKUN_CHR_CATCH_COL_R);
    }

    long CNpcBossPackunBasketBase::GetMotIdx()
    {
        switch (abs(AnimGetMotID())) {
            case 0x11:
            case 0x22:
                return MOT_IDLE_START;
            case 0x12:
            case 0x23:
                return MOT_IDLE;
            case 0x13:
            case 0x24:
                return MOT_MOUTH_OPEN;
            case 0x14:
            case 0x25:
                return MOT_MOUTH_OPEN_LOOP;
            case 0x15:
            case 0x26:
                return MOT_MOUTH_CLOSE;
            case 0x16:
            case 0x27:
                return MOT_EAT;
            case 0x17:
            case 0x28:
                return MOT_SLEEP_START;
            case 0x18:
            case 0x29:
                return MOT_SLEEP;
            case 0x19:
            case 0x2A:
                return MOT_EAT_BIG;
            case 0x1A:
            case 0x2B:
                return MOT_ATTACK_START;
            case 0x1B:
            case 0x2C:
                return MOT_ATTACK_WAIT;
            case 0x1C:
            case 0x2D:
                return MOT_ATTACK;
            case 0x1D:
            case 0x2E:
                return MOT_MUD_START;
            case 0x1E:
            case 0x2F:
                return MOT_MUD_THROW;
            case 0x1F:
            case 0x30:
                return MOT_MUD_END;
            case 0x20:
            case 0x31:
                return MOT_MELT;
            case 0x21:
            case 0x32:
                return MOT_FEARS;
            case 0x33:
                return MOT_PARTY_EAT;
            case 0x34:
                return MOT_END_IDLE_START;
            case 0x35:
                return MOT_END_IDLE;
            default:
                return MOT_IDLE_START;
        }
    }

    inline long CNpcBossPackunBasketBase::GetSubMotID_(long nMot)
    {
        const long anMot[MOT_NUM] = {
            0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B,
            0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
        };
        return anMot[nMot];
    }

    inline long CNpcBossPackunBasketBase::MotPlay_(long nMot, int bLoop, float fBlendArg, float fFrameArg)
    {
        const long anMot[MOT_NUM] = {
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A,
            0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x33, 0x34, 0x35,
        };
        long nMotID = anMot[nMot];
        float fFrame = fFrameArg;
        float fBlend = fBlendArg;
        if (m_pAnm || m_pSubAnm) {
            unsigned long nFlag = 0;
            if (fBlend < 0.0f) {
                fBlend = fabsf(fBlend);
                nFlag |= 8;
            }
            if (bLoop) {
                nFlag |= 2;
            }
            if (m_pAnm) {
                m_pAnm->Play(nMotID, nFlag, 1.0f, fBlend, fFrame);
            }
            if (m_pSubAnm) {
                m_pSubAnm->Play(GetSubMotID_(GetMotIdx()), nFlag, 1.0f, fBlend, fFrame);
            }
        }
        return nMotID;
    }

    void CNpcBossPackunBasketBase::CtorStateSleep()
    {
        SleepStart_();
    }

    long CNpcBossPackunBasketBase::GetType() const
    {
        return 0;
    }

    void CNpcBossPackunBasketBase::LoopStateSleep()
    {
        STATE_WORK_SLEEP *pWork = FsmqGetWork<STATE_WORK_SLEEP>();
        switch (pWork->m_nStep) {
            case 0:
                MotPlay_(MOT_SLEEP_START, 0, 8.0f, 0.0f);
                pWork->m_fTimer = 60.0f * bcs::BCS_RandRange((float)BASKET_PRM_S07_PACKUN_SLEEP_TIME_MIN,
                                                             (float)BASKET_PRM_S07_PACKUN_SLEEP_TIME_MAX);
                pWork->m_nStep++;
                // fall through
            case 1:
                if (!IsAnimEnd()) {
                    break;
                }
                if (!(m_nPackunFlag & 2)) {
                    Eff_Stop(8);
                    Eff_CreateAttachAuto(8, 0xD4, 3);
                }
                Eff_Stop(9);
                Eff_CreateAttachAuto(9, 0xD5, 3);
                MotPlay_(MOT_SLEEP, 1, 8.0f, 0.0f);
                pWork->m_nStep++;
                // fall through
            case 2:
                pWork->m_fTimer -= GetFrameRateF();
                if (pWork->m_fTimer <= 0.0f) {
                    pWork->m_fTimer = 0.0f;
                    pWork->m_nStep++;
                    MethdMelt();
                }
                break;
        }
    }

    void CNpcBossPackunBasketBase::DtorStateSleep()
    {
        SleepEnd_();
    }

    void CNpcBossPackunBasketBase::CtorStateIdle()
    {
        STATE_WORK_IDLE *pWork = FsmqGetWork<STATE_WORK_IDLE>();
        pWork->m_nStep = 0;
        long nMot = MOT_IDLE_START;
        pWork->m_nLipTbl = 0;
        int bLoop = 0;
        pWork->m_nLipKind = 0;
        pWork->m_fUnk0C = 0.0f;
        pWork->m_fUnk10 = 0.0f;
        pWork->m_fTimer = 0.0f;
        m_nLipIdx = 0;
        switch (GetMotIdx()) {
            case MOT_EAT:
            case MOT_SLEEP_START:
            case MOT_SLEEP:
            case MOT_EAT_BIG:
            case MOT_ATTACK_START:
            case MOT_ATTACK_WAIT:
            case MOT_MUD_START:
            case MOT_MUD_THROW:
            case MOT_MELT:
            case MOT_FEARS:
            case MOT_PARTY_EAT:
                nMot = MOT_IDLE;
                bLoop = 1;
                break;
            case MOT_IDLE_START:
            case MOT_IDLE:
                nMot = -1;
                break;
        }
        if (nMot >= 0) {
            MotPlay_(nMot, bLoop, 8.0f, 0.0f);
        }
    }

    inline void CNpcBossPackunBasketBase::UpdateLipSync_(STATE_WORK_IDLE *pWork)
    {
        if (!(m_nFlag & 1)) {
            return;
        }
        float fRate = GetFrameRateF();
        switch (pWork->m_nStep) {
            case 0:
                pWork->m_nStep++;
                // fall through
            case 1: {
                // The next entry of the table; empty ones are skipped.
                long nFrame = GetLipSyncFrame(pWork->m_nLipTbl, m_nLipIdx, pWork->m_nLipKind);
                if (nFrame < 0) {
                    m_nLipIdx = 0;
                    return;
                }
                while (nFrame == 0 && pWork->m_nLipKind != 2) {
                    NextLipIdx_();
                    nFrame = GetLipSyncFrame(pWork->m_nLipTbl, m_nLipIdx, pWork->m_nLipKind);
                }
                pWork->m_fTimer = nFrame;
                pWork->m_nStep++;
            }
                // fall through
            case 2:
                switch (pWork->m_nLipKind) {
                    case 0: // open the mouth
                        switch (GetMotIdx()) {
                            case MOT_PARTY_EAT:
                            case MOT_MOUTH_OPEN:
                                if (!IsAnimEnd()) {
                                    break;
                                }
                                MotPlay_(MOT_MOUTH_OPEN_LOOP, 1, 8.0f, 0.0f);
                                MotSetRate_(1.0f);
                                // fall through
                            case MOT_MOUTH_OPEN_LOOP:
                                pWork->m_nStep++;
                                break;
                            default:
                                long nMotID = MotPlay_(MOT_MOUTH_OPEN, 0, 8.0f, 0.0f);
                                MotSetRate_(GetMotRateFit_(nMotID, pWork->m_fTimer));
                                PlaySe_(s_aSeMouthOpen);
                                break;
                        }
                        break;
                    case 1: { // shut it
                        long nMot = GetMotIdx();
                        if (nMot != MOT_IDLE && nMot != MOT_MOUTH_CLOSE) {
                            long nMotID = MotPlay_(MOT_MOUTH_CLOSE, 0, 8.0f, 0.0f);
                            MotSetRate_(GetMotRateFit_(nMotID, pWork->m_fTimer));
                            PlaySe_(s_aSeMouthClose);
                        }
                        pWork->m_nStep++;
                        break;
                    }
                    case 2: // throw mud
                        MethdMudAttack();
                        NextLipIdx_();
                        return;
                }
                // fall through
            case 3:
                // Shut the mouth faster just before the time is up.
                if (GetMotIdx() == MOT_MOUTH_OPEN_LOOP && BASKET_PRM_COMMON_HUNGRY_LIP_PREV_CLOSE_FRAME != 0 &&
                    pWork->m_fTimer <= (float)BASKET_PRM_COMMON_HUNGRY_LIP_PREV_CLOSE_FRAME) {
                    MotSetRate_(BASKET_PRM_COMMON_HUNGRY_LIP_OPEN_TO_CLOSE_SPEED);
                }
                pWork->m_fTimer -= fRate;
                if (pWork->m_fTimer <= 0.0f) {
                    pWork->m_fTimer = 0.0f;
                    NextLipIdx_();
                    pWork->m_nStep = 1;
                }
                break;
        }
    }

    void CNpcBossPackunBasketBase::LoopStateIdle()
    {
        STATE_WORK_IDLE *pWork = FsmqGetWork<STATE_WORK_IDLE>();
        GetFrameRateF(); // (result unused)
        RotateYRate(m_fBaseRotY, 0.1f);
        long nMot = GetMotIdx();
        if (nMot == MOT_IDLE_START || nMot == MOT_MOUTH_CLOSE) {
            if (IsAnimEnd()) {
                MotPlay_(MOT_IDLE, 1, 8.0f, 0.0f);
                MotSetRate_(1.0f);
            }
            return;
        }
        MethdFears();
        UpdateLipSync_(pWork);
    }

    long CNpcBossPackunBasketBase::GetLipSyncFrame(long nFirst, long nSecond, long &rKind)
    {
        return -1;
    }

    long CNpcBossPackunBasketBase::GetLipSecondIdxNum() const
    {
        return 0;
    }

    void CNpcBossPackunBasketBase::DtorStateIdle()
    {
        MotSetRate_(1.0f);
    }

    void CNpcBossPackunBasketBase::CtorStateAttack()
    {
        STATE_WORK_ATTACK *pWork = FsmqGetWork<STATE_WORK_ATTACK>();
        pWork->m_nStep = 0;
        pWork->m_fRotY = CalcAttackDir_();
        MotPlay_(MOT_ATTACK_START, 0, 8.0f, 0.0f);
    }

    void CNpcBossPackunBasketBase::LoopStateAttack()
    {
        STATE_WORK_ATTACK *pWork = FsmqGetWork<STATE_WORK_ATTACK>();
        RotateYRate(pWork->m_fRotY, 0.2f);
        switch (pWork->m_nStep) {
            case 0: {
                int bReady = 1;
                if (GetMotIdx() == MOT_ATTACK_START) {
                    if (IsAnimEnd()) {
                        MotPlay_(MOT_ATTACK_WAIT, 1, 8.0f, 0.0f);
                    } else {
                        bReady = 0;
                    }
                }
                // Bite when a shot or a dunk comes near the goal, or when
                // there is none any more.
                float fMinDist = 9999.0f;
                int bShot = 0;
                for (long i = 0; i < CBallMan::GetBallNum(); i++) {
                    CBall *pBall = CBallMan::GetBallPtr(i);
                    if (pBall && pBall->IsActive() && pBall->IsShootMode()) {
                        bShot = 1;
                        bcs::FVEC3 vDiff =
                            VecSub(pBall->GetPos(), CStageMan::GetInstance()->GetGoalPos(GetGoalIdx()));
                        float fDist = bcs::BCS_Vec_GetLenXZ(vDiff);
                        if (fMinDist > fDist) {
                            fMinDist = fDist;
                        }
                    }
                }
                for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                    const CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr && !pChr->IsDisable()) {
                        long nState = pChr->GetState();
                        if (nState == CChrCore::STATE_DUNK || nState == CChrCore::STATE_ALLEYOOP ||
                            nState == CChrCore::STATE_REBOUND_DUNK) {
                            bShot = 1;
                            bcs::FVEC3 vDiff =
                                VecSub(pChr->GetPos(), CStageMan::GetInstance()->GetGoalPos(GetGoalIdx()));
                            float fDist = bcs::BCS_Vec_GetLenXZ(vDiff);
                            if (fMinDist > fDist) {
                                fMinDist = fDist;
                            }
                        }
                    }
                }
                if ((fMinDist < 1.5f && bReady) || (!bShot && bReady)) {
                    pWork->m_nStep++;
                }
                break;
            }
            case 1:
                PlaySe_(s_aSeAttack);
                MotPlay_(MOT_ATTACK, 0, 8.0f, 0.0f);
                pWork->m_nStep++;
                // fall through
            case 2:
                if (IsAnimEnd()) {
                    PlaySe_(s_aSeAttackEnd);
                    ChangeStateIdle_();
                }
                break;
        }
    }

    void CNpcBossPackunBasketBase::DtorStateAttack() {}

    void CNpcBossPackunBasketBase::CtorStateMudAttack()
    {
        STATE_WORK_MUD_ATTACK *pWork = FsmqGetWork<STATE_WORK_MUD_ATTACK>();
        pWork->m_nStep = 0;
        pWork->m_nNum = BASKET_PRM_S07_PACKUN_BLT_NUM;
        pWork->m_vTarget.x = 0.0f;
        pWork->m_vTarget.y = 0.0f;
        pWork->m_vTarget.z = 0.0f;
        pWork->m_fRotY = m_vRot.y;
        pWork->m_fThrowRotY = m_vRot.y;

        // Aim at the ball, within BLT_APPEND_DIR degrees of the court's
        // direction (at the same distance when it is outside).
        CBall *pBall = CBallMan::GetBallPtr(0);
        if (pBall && pBall->IsActive()) {
            float fRotY = GetRotY_ToPosXZ(pBall->GetPos());
            pWork->m_vTarget = pBall->GetPos();
            pWork->m_vTarget.y = 0.0f;
            float fThrowRotY = fRotY - m_fBaseRotY;
            float fMax = 0.017453292f * BASKET_PRM_S07_PACKUN_BLT_APPEND_DIR;
            fThrowRotY = RoundRadPI(fThrowRotY);
            if (fabsf(fThrowRotY) > fMax) {
                fThrowRotY = RoundRad((fThrowRotY > 0.0 ? fMax : -fMax) + m_fBaseRotY);
                bcs::FVEC3 vDiff = VecSub(m_vPos, pWork->m_vTarget);
                float fDist = bcs::BCS_Vec_GetLenXZ(vDiff);
                bcs::FVEC3 vDir;
                bcs::BCS_VecZ_RotY(vDir, 1.0f, fThrowRotY);
                pWork->m_vTarget.x = vDir.x * fDist + m_vPos.x;
                pWork->m_vTarget.y = 0.0f;
                pWork->m_vTarget.z = vDir.z * fDist + m_vPos.z;
            }
            // (when the ball is in range this is the angle from the court's
            // direction, not a yaw; it is what the original stores)
            pWork->m_fThrowRotY = fThrowRotY;
            pWork->m_fRotY = LimitRotY_(GetRotY_ToPosXZ(pBall->GetPos()), BASKET_PRM_S07_PACKUN_MAX_DIR);
        }
        MotPlay_(MOT_MUD_START, 0, 8.0f, 0.0f);
    }

    void CNpcBossPackunBasketBase::LoopStateMudAttack()
    {
        STATE_WORK_MUD_ATTACK *pWork = FsmqGetWork<STATE_WORK_MUD_ATTACK>();
        switch (pWork->m_nStep) {
            case 0:
                if (!IsAnimEnd()) {
                    break;
                }
                MotPlay_(MOT_MUD_THROW, 0, 8.0f, 0.0f);
                pWork->m_nStep++;
                // fall through
            case 1:
                // Throw a third of the way into the motion.
                if (AnimGetFrame() >= AnimGetFrameNum() / 3.0f) {
                    long nTbl;
                    if (BASKET_PRM_S07_MUD_TBL_IDX == 0) {
                        nTbl = bcs::BCS_RandRange(0L, 2L);
                    } else {
                        nTbl = BASKET_PRM_S07_MUD_TBL_IDX - 1;
                    }
                    float fRot;
                    if (BASKET_PRM_S07_MUD_TBL_ROTATE_ENABLE == 0) {
                        fRot = 0.0f;
                    } else {
                        fRot = 0.017453292f * (bcs::BCS_RandRange(0L, 36L) * 10);
                    }
                    const bcs::FVEC3 &rFrom = m_vBasePos;
                    for (long i = 0; i < pWork->m_nNum; i++) {
                        CMudBombBasket *pBomb = new CMudBombBasket(CGame::GetTempParent());
                        if (pBomb) {
                            pBomb->Create();
                            float fFrame = BASKET_PRM_S07_PACKUN_BLT_TIME +
                                           bcs::BCS_RandMax(BASKET_PRM_S07_PACKUN_BLT_TIME_RND);
                            pBomb->SetAppendParam(rFrom, pWork->m_vTarget, nTbl, i, 1 - GetGoalIdx(), fFrame,
                                                  pWork->m_fThrowRotY + fRot);
                        }
                    }
                    PlaySe_(s_aSeMudAttack);
                    pWork->m_nStep++;
                }
                // fall through
            case 2:
                if (!IsAnimEnd()) {
                    break;
                }
                MotPlay_(MOT_MUD_END, 0, 8.0f, 0.0f);
                pWork->m_nStep++;
                // fall through
            default:
                if (IsAnimEnd()) {
                    ChangeStateIdle_();
                    return;
                }
                break;
        }
        RotateYRate(pWork->m_fRotY, 0.2f);
    }

    void CNpcBossPackunBasketBase::DtorStateMudAttack() {}

    void CNpcBossPackunBasketBase::CtorStateEatWait()
    {
        FsmqGetWork<STATE_WORK_EAT_WAIT>()->m_nStep = 0;
    }

    void CNpcBossPackunBasketBase::LoopStateEatWait()
    {
        STATE_WORK_EAT_WAIT *pWork = FsmqGetWork<STATE_WORK_EAT_WAIT>();

        // Wait with the mouth open while a shot flies. (the original looks at
        // ball 0 each time round)
        int bNoShot = 1;
        for (long i = 0; i < CBallMan::GetBallNum(); i++) {
            CBall *pBall = CBallMan::GetBallPtr(0);
            if (pBall && pBall->IsActive() && pBall->IsShootMode()) {
                bNoShot = 0;
                break;
            }
        }
        if (bNoShot) {
            ChangeStateIdle_();
            return;
        }
        switch (pWork->m_nStep) {
            case 0:
                MotPlay_(MOT_MOUTH_OPEN, 0, 8.0f, 0.0f);
                pWork->m_nStep++;
                // fall through
            case 1:
                if (IsAnimEnd()) {
                    MotPlay_(MOT_MOUTH_OPEN_LOOP, 1, 8.0f, 0.0f);
                    pWork->m_nStep++;
                }
                break;
        }
    }

    void CNpcBossPackunBasketBase::DtorStateEatWait() {}

    void CNpcBossPackunBasketBase::CtorStateEat()
    {
        STATE_WORK_EAT *pWork = FsmqGetWork<STATE_WORK_EAT>();
        long nKind = pWork->m_nKind;
        long nMot = MOT_EAT;
        if ((nKind >= 9 && nKind <= 11) || nKind == 3) {
            nMot = MOT_EAT_BIG;
        } else {
            PlaySe_(s_aSeEat);
        }
        MotPlay_(nMot, 0, 8.0f, 0.0f);
        if (pWork->m_nKind == 3) {
            PlaySe_(s_aSeEatBig);
            PlaySe_(s_aSeEatBig2);
        }
    }

    void CNpcBossPackunBasketBase::LoopStateEat()
    {
        AnimGetMotID(); // (result unused)
        if (IsAnimEnd()) {
            ChangeStateIdle_();
        }
    }

    void CNpcBossPackunBasketBase::DtorStateEat() {}

    void CNpcBossPackunBasketBase::CtorStateMelt()
    {
        MotPlay_(MOT_MELT, 0, 8.0f, 0.0f);
    }

    void CNpcBossPackunBasketBase::LoopStateMelt()
    {
        if (IsAnimEnd()) {
            ChangeStateIdle_();
        }
    }

    void CNpcBossPackunBasketBase::DtorStateMelt() {}

    void CNpcBossPackunBasketBase::CtorStateFears()
    {
        MotPlay_(MOT_FEARS, 1, 8.0f, 0.0f);
    }

    void CNpcBossPackunBasketBase::LoopStateFears()
    {
        if (!CanbeFears()) {
            ChangeStateIdle_();
        }
    }

    void CNpcBossPackunBasketBase::DtorStateFears() {}

    void CNpcBossPackunBasketBase::CtorStateEndIdle() {}

    void CNpcBossPackunBasketBase::LoopStateEndIdle() {}

    void CNpcBossPackunBasketBase::DtorStateEndIdle() {}

    int CNpcBossPackunBasketBase::CanbeSleep()
    {
        if (CChrManBasket::GetInstance()->GetFlagAC() & 2) {
            return 0;
        }
        if (m_bEnd) {
            return 0;
        }
        long nState = FsmqGetState();
        if ((nState >= STATE_ATTACK && nState <= STATE_MUD_ATTACK) || nState == STATE_SLEEP) {
            return 0;
        }
        return 1;
    }

    int CNpcBossPackunBasketBase::MethdSleep()
    {
        if (!CanbeSleep()) {
            return 0;
        }
        FsmqChangeState(STATE_SLEEP);
        return 1;
    }

    int CNpcBossPackunBasketBase::CanbeIdle()
    {
        long nState = FsmqGetState();
        if (nState == STATE_IDLE || nState == STATE_EAT) {
            return 0;
        }
        return 1;
    }

    int CNpcBossPackunBasketBase::MethdIdle()
    {
        if (!CanbeIdle()) {
            return 0;
        }
        FsmqChangeState(STATE_IDLE);
        return 1;
    }

    int CNpcBossPackunBasketBase::CanbeAttack()
    {
        long nState = FsmqGetState();
        if ((nState >= STATE_EAT && nState <= STATE_FEARS) || (nState >= STATE_ATTACK && nState <= STATE_MUD_ATTACK) ||
            nState == STATE_SLEEP) {
            return 0;
        }
        return 1;
    }

    int CNpcBossPackunBasketBase::MethdAttack()
    {
        if (!CanbeAttack()) {
            return 0;
        }
        FsmqChangeState(STATE_ATTACK);
        return 1;
    }

    int CNpcBossPackunBasketBase::CanbeMudAttack()
    {
        if (m_bEnd) {
            return 0;
        }
        long nState = FsmqGetState();
        if ((nState >= STATE_EAT_WAIT && nState <= STATE_EAT) || nState == STATE_MUD_ATTACK ||
            nState == STATE_SLEEP || nState == STATE_FEARS) {
            return 0;
        }
        return 1;
    }

    int CNpcBossPackunBasketBase::MethdMudAttack()
    {
        if (!CanbeMudAttack()) {
            return 0;
        }
        FsmqChangeState(STATE_MUD_ATTACK);
        return 1;
    }

    int CNpcBossPackunBasketBase::CanbeEatWait()
    {
        long nState = FsmqGetState();
        if (nState == STATE_EAT_WAIT || nState == STATE_SLEEP) {
            return 0;
        }
        return 1;
    }

    int CNpcBossPackunBasketBase::MethdEatWait()
    {
        if (!CanbeEatWait()) {
            return 0;
        }
        FsmqChangeState(STATE_EAT_WAIT);
        return 1;
    }

    int CNpcBossPackunBasketBase::CanbeEat()
    {
        if (FsmqGetState() == STATE_EAT) {
            return 0;
        }
        return 1;
    }

    int CNpcBossPackunBasketBase::MethdEat(long nChr, long nKind, long)
    {
        if (!CanbeEat()) {
            return 0;
        }
        // Only the shots at the boss's own goal.
        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        if (pChr && pChr->GetTargetGoalIdx() != GetGoalIdx()) {
            return 0;
        }
        long nEff = -1;
        if (nKind < 9 || nKind > 11) {
            nEff = nKind == 3 ? 0xD7 : 0xD8;
        }
        if (CGame::g_GameWork.GetGameType() == CGame::GAME_TYPE_PARTY && FsmqGetState() != STATE_SLEEP) {
            Eff_Stop(9);
            Eff_CreateAttachAuto(9, 0xD9, 3);
        }
        if (nEff > 0) {
            Eff_Stop(10);
            Eff_CreateAttachAuto(10, nEff, 3);
        }
        STATE_WORK_EAT *pWork = static_cast<STATE_WORK_EAT *>(FsmqSetState(STATE_EAT));
        pWork->m_nChr = nChr;
        pWork->m_nKind = nKind;
        return 1;
    }

    int CNpcBossPackunBasketBase::CanbeMelt()
    {
        if (m_bEnd) {
            return 0;
        }
        if (FsmqGetState() != STATE_SLEEP) {
            return 0;
        }
        return 1;
    }

    int CNpcBossPackunBasketBase::MethdMelt()
    {
        if (!CanbeMelt()) {
            return 0;
        }
        FsmqChangeState(STATE_MELT);
        return 1;
    }

    int CNpcBossPackunBasketBase::CanbeFears()
    {
        // While the arts event plays, or when a character in the star
        // condition attacks the boss's goal.
        if (!(CChrManBasket::GetInstance()->GetFlagAC() & 2)) {
            int bFound = 0;
            long nGoal = GetGoalIdx();
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr && !pChr->IsDisable() && nGoal == pChr->GetTargetGoalIdx() &&
                    pChr->GetComLevelWork() == 1) {
                    bFound = 1;
                    break;
                }
            }
            if (!bFound) {
                return 0;
            }
        }
        long nState = FsmqGetState();
        if ((nState >= STATE_EAT_WAIT && nState <= STATE_EAT) || nState == STATE_SLEEP) {
            return 0;
        }
        return 1;
    }

    int CNpcBossPackunBasketBase::MethdFears()
    {
        if (!CanbeFears()) {
            return 0;
        }
        if (FsmqGetState() == STATE_FEARS) {
            return 0;
        }
        FsmqChangeState(STATE_FEARS);
        return 1;
    }

    int CNpcBossPackunBasketBase::CanbeEndIdle()
    {
        if (FsmqGetState() != STATE_IDLE) {
            return 0;
        }
        return GetMotIdx() == MOT_IDLE;
    }

    int CNpcBossPackunBasketBase::MethdEndIdle()
    {
        if (!CanbeEndIdle()) {
            return 0;
        }
        if (FsmqGetState() == STATE_END_IDLE) {
            return 0;
        }
        FsmqChangeState(STATE_END_IDLE);
        return 1;
    }

    // ---------------------------------------------------------------------
    // CNpcBossPackunBasketBase::CMudBombBasket
    // ---------------------------------------------------------------------

    void CNpcBossPackunBasketBase::CMudBombBasket::CreateAtkColl()
    {
        m_AtkColl.Create(1, 4, 0, 0);
        SetAttackHitCallbackAll_(m_AtkColl);
        m_AtkColl.SetDisable(0);
    }

    void CNpcBossPackunBasketBase::CMudBombBasket::UpdateAtkColl()
    {
        if (GetCurStep() == 1) {
            m_AtkColl.SetPos(0, GetPos());
            m_AtkColl.SetCollType(0, 1);
            m_AtkColl.SetCollKind(0, 5);
            m_AtkColl.SetTargetFlag(0, 2);
            m_AtkColl.SetAtkType(0, 0xD04);
            m_AtkColl.SetSize(0, GetParam_ExplodeAtkCollRadius(), GetParam_ExplodeAtkCollHeight(),
                              GetParam_ExplodeAtkCollRadius());
            m_AtkColl.SetEnable(0);
        } else {
            m_AtkColl.SetDisable(0);
        }
    }

    void CNpcBossPackunBasketBase::CMudBombBasket::OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                                               const bcs::FVEC3 &rPos, unsigned long)
    {
    }

    const CStgMdl::MDL_PARAM *CNpcBossPackunBasketBase::CMudBombBasket::MdlParam_Bullet() const
    {
        static const char *anm_name_tbl[] = {"", 0};
        static const CStgMdl::MDL_PARAM mdl_param = {
            "s07_gm_mudspray00", "s07_gm_mudspray00", "s07_gm_mudspray00", 0x204,
            anm_name_tbl,        "s07_gm_mudspray00", 0,                   0,
        };
        return &mdl_param;
    }

    const CStgMdl::MDL_PARAM *CNpcBossPackunBasketBase::CMudBombBasket::MdlParam_Explode() const
    {
        static const char *anm_name_tbl[] = {"", 0};
        static const CStgMdl::MDL_PARAM mdl_param = {
            "s07_gm_mud00", 0, 0, 0x204, anm_name_tbl, "s07_gm_mud00", 0, 0,
        };
        return &mdl_param;
    }

    long CNpcBossPackunBasketBase::CMudBombBasket::GetParam_EffectID(long nEff) const
    {
        switch (nEff) {
            case 0:
                return 0x6BB;
            case 1:
                return 0x6BC;
            default:
                return -1;
        }
    }

    float CNpcBossPackunBasketBase::CMudBombBasket::GetParam_FadeOutTime() const
    {
        return 10.0f;
    }

    float CNpcBossPackunBasketBase::CMudBombBasket::GetParam_LifeLimit() const
    {
        return BASKET_PRM_S07_PACKUN_BLT_LIFE;
    }

    float CNpcBossPackunBasketBase::CMudBombBasket::GetParam_ExplodeTime() const
    {
        return BASKET_PRM_S07_PACKUN_EXPLODE_TIME;
    }

    float CNpcBossPackunBasketBase::CMudBombBasket::GetParam_ExplodeAtkCollRadius() const
    {
        return (BASKET_PRM_S07_PACKUN_EXPLODE_SIZE * GetParam_ExplodeMdlRadius()) / 100.0f;
    }

    float CNpcBossPackunBasketBase::CMudBombBasket::GetParam_ExplodeMdlRadius() const
    {
        return m_fExplodeMdlRadius;
    }

    float CNpcBossPackunBasketBase::CMudBombBasket::GetParam_ExplodeAtkCollHeight() const
    {
        return BASKET_PRM_S07_PACKUN_EXPLODE_SIZE_Y;
    }

    float CNpcBossPackunBasketBase::CMudBombBasket::GetParam_BulletJumpHeight() const
    {
        return BASKET_PRM_S07_PACKUN_BLT_HEIGHT;
    }

    void CNpcBossPackunBasketBase::CMudBombBasket::SetAppendParam(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo,
                                                                  long nTbl, long nIdx, long nGoal, float fFrame,
                                                                  float fRotY)
    {
        // The three mud tables: the size and the place (around the target)
        // of each of the three bombs.
        struct MUD {
            MUD(float fScaleIn, const bcs::FVEC3 &rPosIn)
                : fScale(fScaleIn)
                , vPos(rPosIn)
            {
            }

            float fScale;
            bcs::FVEC3 vPos;
        };
        MUD aMud[3][3] = {
            {
                MUD(BASKET_PRM_S07_MUD_TBL0_SCALE0,
                     bcs::FVEC3(BASKET_PRM_S07_MUD_TBL0_POS_X0, 0.0f, BASKET_PRM_S07_MUD_TBL0_POS_Z0)),
                MUD(BASKET_PRM_S07_MUD_TBL0_SCALE1,
                     bcs::FVEC3(BASKET_PRM_S07_MUD_TBL0_POS_X1, 0.0f, BASKET_PRM_S07_MUD_TBL0_POS_Z1)),
                MUD(BASKET_PRM_S07_MUD_TBL0_SCALE2,
                     bcs::FVEC3(BASKET_PRM_S07_MUD_TBL0_POS_X2, 0.0f, BASKET_PRM_S07_MUD_TBL0_POS_Z2)),
            },
            {
                MUD(BASKET_PRM_S07_MUD_TBL1_SCALE0,
                     bcs::FVEC3(BASKET_PRM_S07_MUD_TBL1_POS_X0, 0.0f, BASKET_PRM_S07_MUD_TBL1_POS_Z0)),
                MUD(BASKET_PRM_S07_MUD_TBL1_SCALE1,
                     bcs::FVEC3(BASKET_PRM_S07_MUD_TBL1_POS_X1, 0.0f, BASKET_PRM_S07_MUD_TBL1_POS_Z1)),
                MUD(BASKET_PRM_S07_MUD_TBL1_SCALE2,
                     bcs::FVEC3(BASKET_PRM_S07_MUD_TBL1_POS_X2, 0.0f, BASKET_PRM_S07_MUD_TBL1_POS_Z2)),
            },
            {
                MUD(BASKET_PRM_S07_MUD_TBL2_SCALE0,
                     bcs::FVEC3(BASKET_PRM_S07_MUD_TBL2_POS_X0, 0.0f, BASKET_PRM_S07_MUD_TBL2_POS_Z0)),
                MUD(BASKET_PRM_S07_MUD_TBL2_SCALE1,
                     bcs::FVEC3(BASKET_PRM_S07_MUD_TBL2_POS_X1, 0.0f, BASKET_PRM_S07_MUD_TBL2_POS_Z1)),
                MUD(BASKET_PRM_S07_MUD_TBL2_SCALE2,
                     bcs::FVEC3(BASKET_PRM_S07_MUD_TBL2_POS_X2, 0.0f, BASKET_PRM_S07_MUD_TBL2_POS_Z2)),
            },
        };
        MUD mud = aMud[nTbl][nIdx];
        m_fExplodeMdlRadius = mud.fScale;

        // The bomb's place around the target, turned by fRotY.
        float fLen = bcs::BCS_Vec_GetLenXZ(mud.vPos);
        NormalizeXZ(mud.vPos, mud.vPos);
        bcs::BCS_Vec_RotY(mud.vPos, mud.vPos, fRotY);
        mud.vPos.x *= fLen;
        mud.vPos.z *= fLen;

        // The target, within the bomb's range.
        bcs::FVEC3 vDiff = VecSub(rTo, rFrom);
        float fDist = bcs::BCS_Vec_GetLenXZ(vDiff);
        fDist = Clamp(fDist, BASKET_PRM_S07_PACKUN_BLT_DIST_MIN, BASKET_PRM_S07_PACKUN_BLT_DIST_MAX);
        bcs::FVEC3 vDir;
        NormalizeXZ(vDir, VecSub(rTo, rFrom));
        bcs::FVEC3 vTarget(vDir.x * fDist + rFrom.x, 0.0f, vDir.z * fDist + rFrom.z);

        CMudBomb::SetGoalPosAuto(rFrom, nGoal, fFrame);
        bcs::FVEC3 vGoal = VecAdd(mud.vPos, vTarget);
        SetGoalPos(rFrom, vGoal, fFrame);
    }

    // ---------------------------------------------------------------------
    // CNpcBossPackunBasketGame
    // ---------------------------------------------------------------------

    long CNpcBossPackunBasketGame::GetLipSyncFrame(long nFirst, long nSecond, long &rKind)
    {
        static const long nSecondNum = 10;
        // The frames of each entry; the entries alternate between opening
        // and shutting the mouth.
        unsigned short anFrame[nSecondNum] = {
            BASKET_PRM_S07_LIP_TBL0_STATE0, BASKET_PRM_S07_LIP_TBL0_STATE1, BASKET_PRM_S07_LIP_TBL0_STATE2,
            BASKET_PRM_S07_LIP_TBL0_STATE3, BASKET_PRM_S07_LIP_TBL0_STATE4, BASKET_PRM_S07_LIP_TBL0_STATE5,
            BASKET_PRM_S07_LIP_TBL0_STATE6, BASKET_PRM_S07_LIP_TBL0_STATE7, BASKET_PRM_S07_LIP_TBL0_STATE8,
            BASKET_PRM_S07_LIP_TBL0_STATE9,
        };
        rKind = nSecond % 2;
        return anFrame[nSecond];
    }

    void CNpcBossPackunBasketGame::LoopStateSleep()
    {
        // Asleep until CNpcBossPackunBasketGameMan wakes the boss.
        STATE_WORK_SLEEP *pWork = FsmqGetWork<STATE_WORK_SLEEP>();
        switch (pWork->m_nStep) {
            case 0:
                MotPlay_(MOT_SLEEP_START, 0, 8.0f, 0.0f);
                pWork->m_fTimer = 60.0f * bcs::BCS_RandRange((float)BASKET_PRM_S07_PACKUN_SLEEP_TIME_MIN,
                                                             (float)BASKET_PRM_S07_PACKUN_SLEEP_TIME_MAX);
                pWork->m_nStep++;
                // fall through
            case 1:
                if (IsAnimEnd()) {
                    if (!(m_nPackunFlag & 2)) {
                        Eff_Stop(8);
                        Eff_CreateAttachAuto(8, 0xD4, 3);
                    }
                    Eff_Stop(9);
                    Eff_CreateAttachAuto(9, 0xD5, 3);
                    MotPlay_(MOT_SLEEP, 1, 8.0f, 0.0f);
                    pWork->m_nStep++;
                }
                break;
        }
    }

    // ---------------------------------------------------------------------
    // CNpcBossPackunBasketGameMan
    // ---------------------------------------------------------------------

    CNpcBossPackunBasketGameMan::CNpcBossPackunBasketGameMan(bcs::CTask *pParent)
        : CGameTask(pParent, -1)
    {
        m_nStep = 0;
        m_nMudNum = 0;
        m_nSleepFrame = 0;
        m_fTimer = 0.0f;
        m_apBoss[0] = 0;
        m_apBoss[1] = 0;

        InitFsm(this, STATE_NUM);
        SetState(STATE_INIT, &CNpcBossPackunBasketGameMan::CtorStateInit,
                 &CNpcBossPackunBasketGameMan::LoopStateInit, &CNpcBossPackunBasketGameMan::DtorStateInit);
        SetState(STATE_SET_UP, &CNpcBossPackunBasketGameMan::CtorStateSetUp,
                 &CNpcBossPackunBasketGameMan::LoopStateSetUp, &CNpcBossPackunBasketGameMan::DtorStateSetUp);
        SetState(STATE_MAIN, &CNpcBossPackunBasketGameMan::CtorStateMain,
                 &CNpcBossPackunBasketGameMan::LoopStateMain, &CNpcBossPackunBasketGameMan::DtorStateMain);
        SetState(STATE_SLEEP, &CNpcBossPackunBasketGameMan::CtorStateSleep,
                 &CNpcBossPackunBasketGameMan::LoopStateSleep, &CNpcBossPackunBasketGameMan::DtorStateSleep);
        SetState(STATE_MUD_ATTACK_CHECK, &CNpcBossPackunBasketGameMan::CtorStateMudAttackCheck,
                 &CNpcBossPackunBasketGameMan::LoopStateMudAttackCheck,
                 &CNpcBossPackunBasketGameMan::DtorStateMudAttackCheck);
        SetState(STATE_END, &CNpcBossPackunBasketGameMan::CtorStateEnd, &CNpcBossPackunBasketGameMan::LoopStateEnd,
                 &CNpcBossPackunBasketGameMan::DtorStateEnd);
        ChangeState(STATE_INIT);
    }

    CNpcBossPackunBasketGameMan::~CNpcBossPackunBasketGameMan()
    {
        g_pInstance = 0;
    }

    void CNpcBossPackunBasketGameMan::Main()
    {
        ExecState();
    }

    inline void CNpcBossPackunBasketGameMan::OnMessage(long nMsg)
    {
        switch (nMsg) {
            case 0xF:
                m_nSleepFrame = 0;
                m_nMudNum = 0;
                if (m_nState != STATE_SET_UP) {
                    ChangeState(STATE_SET_UP);
                }
                break;
            case 0x3:
            case 0x5:
            case 0x12:
            case 0x13:
                if (m_nState != STATE_SET_UP) {
                    ChangeState(STATE_SET_UP);
                }
                break;
            case 0x16:
                if (m_nState != STATE_MAIN) {
                    ChangeState(STATE_MAIN);
                }
                break;
            case 0x17:
                if (m_nState != STATE_END) {
                    ChangeState(STATE_END);
                }
                break;
        }
    }

    void CNpcBossPackunBasketGameMan::CtorStateInit()
    {
        m_nMudNum = 0;
        m_nSleepFrame = 0;
    }

    void CNpcBossPackunBasketGameMan::LoopStateInit() {}

    void CNpcBossPackunBasketGameMan::DtorStateInit() {}

    void CNpcBossPackunBasketGameMan::CtorStateSetUp() {}

    void CNpcBossPackunBasketGameMan::LoopStateSetUp() {}

    void CNpcBossPackunBasketGameMan::DtorStateSetUp() {}

    void CNpcBossPackunBasketGameMan::CtorStateMain()
    {
        m_nStep = 0;
    }

    inline int CNpcBossPackunBasketGameMan::IsAllBossCanbeSleep() const
    {
        if (CChrManBasket::GetInstance()->GetFlagAC() & 2) {
            return 0;
        }
        for (long i = 0; i < 2; i++) {
            if (!m_apBoss[i]->CanbeSleep()) {
                return 0;
            }
        }
        return 1;
    }

    void CNpcBossPackunBasketGameMan::LoopStateMain()
    {
        switch (m_nStep) {
            case 0:
                if (!IsAllBossCanbeSleep()) {
                    break;
                }
                if (BASKET_PRM_S07_PACKUN_MELT_TIME_MIN < 0 || BASKET_PRM_S07_PACKUN_MELT_TIME_MAX < 0) {
                    break;
                }
                m_fTimer = 60.0f *
                           bcs::BCS_RandRange(BASKET_PRM_S07_PACKUN_MELT_TIME_MIN, BASKET_PRM_S07_PACKUN_MELT_TIME_MAX);
                if (BASKET_PRM_S07_PACKUN_STATE_MUD_NUM == 0) {
                    m_nSleepFrame = 0;
                }
                if (m_nMudNum >= BASKET_PRM_S07_PACKUN_STATE_MUD_NUM) {
                    m_nSleepFrame = 0;
                    m_nMudNum = 0;
                }
                m_nStep++;
                // fall through
            case 1:
                // While both bosses are idle, count down to a sleep or a mud
                // attack.
                if (m_apBoss[0]->GetState() != CNpcBossPackun::STATE_IDLE) {
                    break;
                }
                if (m_apBoss[1]->GetState() != CNpcBossPackun::STATE_IDLE) {
                    break;
                }
                if (CGame::g_GameWork.m_nFlag18 & 0x4000) {
                    break;
                }
                if ((m_fTimer -= GetFrameRateF()) <= 0.0f) {
                    if (m_nSleepFrame < BASKET_PRM_S07_PACKUN_STATE_SLEEP_NUM) {
                        ChangeState(STATE_SLEEP);
                    } else if (BASKET_PRM_S07_PACKUN_STATE_MUD_NUM > 0) {
                        ChangeState(STATE_MUD_ATTACK_CHECK);
                    }
                }
                break;
        }
    }

    void CNpcBossPackunBasketGameMan::DtorStateMain() {}

    void CNpcBossPackunBasketGameMan::CtorStateSleep()
    {
        m_fTimer = 60.0f *
                   bcs::BCS_RandRange(BASKET_PRM_S07_PACKUN_SLEEP_TIME_MIN, BASKET_PRM_S07_PACKUN_SLEEP_TIME_MAX);
        for (long i = 0; i < 2; i++) {
            m_apBoss[i]->MethdSleep();
        }
        CGame::SendMessage(0x38, 0, 0, 0);
    }

    void CNpcBossPackunBasketGameMan::LoopStateSleep()
    {
        m_fTimer -= GetFrameRateF();
        if (m_fTimer < 0.0f) {
            m_fTimer = 0.0f;
        }
        // Wake both when the time is up or when one woke up.
        int bWake = 0;
        if (m_fTimer <= 0.0f && !(CChrManBasket::GetInstance()->GetFlagAC() & 2)) {
            bWake = 1;
        }
        if (!bWake) {
            if (m_apBoss[0]->GetState() != CNpcBossPackun::STATE_SLEEP) {
                bWake = 1;
            } else if (m_apBoss[1]->GetState() != CNpcBossPackun::STATE_SLEEP) {
                bWake = 1;
            }
        }
        if (bWake) {
            for (long i = 0; i < 2; i++) {
                m_apBoss[i]->MethdMelt();
            }
            ChangeState(STATE_MAIN);
        }
        m_nSleepFrame++;
    }

    void CNpcBossPackunBasketGameMan::DtorStateSleep()
    {
        CGame::SendMessage(0x39, 0, 0, 0);
    }

    void CNpcBossPackunBasketGameMan::CtorStateMudAttackCheck()
    {
        // The boss nearer to the ball throws.
        CBall *pBall = CBallMan::GetBallPtr(0);
        if (pBall) {
            float fMinDist = 9999.0f;
            long nNear = -1;
            for (long i = 0; i < 2; i++) {
                bcs::FVEC3 vDiff = VecSub(pBall->GetPos(), m_apBoss[i]->GetPos());
                float fDist = bcs::BCS_Vec_GetLenXZ(vDiff);
                if (fDist < fMinDist) {
                    fMinDist = fDist;
                    nNear = i;
                }
            }
            if (nNear != -1) {
                m_apBoss[nNear]->MethdMudAttack();
            }
        }
        m_nMudNum++;
    }

    void CNpcBossPackunBasketGameMan::LoopStateMudAttackCheck()
    {
        if (m_apBoss[0]->GetState() == CNpcBossPackun::STATE_MUD_ATTACK) {
            return;
        }
        if (m_apBoss[1]->GetState() == CNpcBossPackun::STATE_MUD_ATTACK) {
            return;
        }
        ChangeState(STATE_MAIN);
    }

    void CNpcBossPackunBasketGameMan::DtorStateMudAttackCheck() {}

    void CNpcBossPackunBasketGameMan::CtorStateEnd() {}

    void CNpcBossPackunBasketGameMan::LoopStateEnd() {}

    void CNpcBossPackunBasketGameMan::DtorStateEnd() {}

    // ---------------------------------------------------------------------
    // CChrManBasket
    // ---------------------------------------------------------------------

    CNpcBossPackun *CChrManBasket::CreateBossPackun_(const bcs::FVEC3 &rPos, long nIdx)
    {
        CNpcBossPackunBasketBase *pBoss = 0;
        if (CGame::g_GameWork.GetGameType() == CGame::GAME_TYPE_PARTY) {
            switch (CGame::g_GameWork.GetStage()) {
                case 0x46:
                    pBoss = new (0) CNpcBossPackunBasketParty70(this, rPos, nIdx);
                    break;
                case 0x47:
                    pBoss = new (0) CNpcBossPackunBasketParty71(this, rPos, nIdx);
                    break;
                case 0x48:
                    pBoss = new (0) CNpcBossPackunBasketParty72(this, rPos, nIdx);
                    break;
            }
        } else {
            CNpcBossPackunBasketGameMan::Create(this);
            pBoss = new (0) CNpcBossPackunBasketGame(this, rPos, nIdx);
            CNpcBossPackunBasketGameMan::GetInstance()->SetBoss(pBoss->GetGoalIdx(), pBoss);
        }
        if (pBoss) {
            pBoss->Create();
        }
        return pBoss;
    }

    // ---------------------------------------------------------------------
    // The party stages' lip sync tables (their statics come before the
    // weak copies' literals in the original)
    // ---------------------------------------------------------------------

    // The lip sync tables of the party stages: for each party level (the
    // first index) ten entries of a kind (STATE_WORK_IDLE::m_nLipKind,
    // brought into 0 or 1) and a time in frames.
#define DEFINE_PARTY_LIP_SYNC_FRAME(CLASS, S)                                                                        \
    long CLASS::GetLipSyncFrame(long nFirst, long nSecond, long &rKind)                                               \
    {                                                                                                                 \
        static const long nSecondNum = 10;                                                                            \
        static const long nFirstNum = 3;                                                                              \
        unsigned short anFrame[nFirstNum][nSecondNum] = {                                                             \
            {                                                                                                         \
                BASKET_PRM_##S##_STATE0_TBL0, BASKET_PRM_##S##_STATE0_TBL1, BASKET_PRM_##S##_STATE0_TBL2,             \
                BASKET_PRM_##S##_STATE0_TBL3, BASKET_PRM_##S##_STATE0_TBL4, BASKET_PRM_##S##_STATE0_TBL5,             \
                BASKET_PRM_##S##_STATE0_TBL6, BASKET_PRM_##S##_STATE0_TBL7, BASKET_PRM_##S##_STATE0_TBL8,             \
                BASKET_PRM_##S##_STATE0_TBL9,                                                                         \
            },                                                                                                        \
            {                                                                                                         \
                BASKET_PRM_##S##_STATE1_TBL0, BASKET_PRM_##S##_STATE1_TBL1, BASKET_PRM_##S##_STATE1_TBL2,             \
                BASKET_PRM_##S##_STATE1_TBL3, BASKET_PRM_##S##_STATE1_TBL4, BASKET_PRM_##S##_STATE1_TBL5,             \
                BASKET_PRM_##S##_STATE1_TBL6, BASKET_PRM_##S##_STATE1_TBL7, BASKET_PRM_##S##_STATE1_TBL8,             \
                BASKET_PRM_##S##_STATE1_TBL9,                                                                         \
            },                                                                                                        \
            {                                                                                                         \
                BASKET_PRM_##S##_STATE2_TBL0, BASKET_PRM_##S##_STATE2_TBL1, BASKET_PRM_##S##_STATE2_TBL2,             \
                BASKET_PRM_##S##_STATE2_TBL3, BASKET_PRM_##S##_STATE2_TBL4, BASKET_PRM_##S##_STATE2_TBL5,             \
                BASKET_PRM_##S##_STATE2_TBL6, BASKET_PRM_##S##_STATE2_TBL7, BASKET_PRM_##S##_STATE2_TBL8,             \
                BASKET_PRM_##S##_STATE2_TBL9,                                                                         \
            },                                                                                                        \
        };                                                                                                            \
        unsigned char anKind[nFirstNum][nSecondNum] = {                                                               \
            {                                                                                                         \
                BASKET_PRM_##S##_STATE0_TBL0_IDX, BASKET_PRM_##S##_STATE0_TBL1_IDX, BASKET_PRM_##S##_STATE0_TBL2_IDX, \
                BASKET_PRM_##S##_STATE0_TBL3_IDX, BASKET_PRM_##S##_STATE0_TBL4_IDX, BASKET_PRM_##S##_STATE0_TBL5_IDX, \
                BASKET_PRM_##S##_STATE0_TBL6_IDX, BASKET_PRM_##S##_STATE0_TBL7_IDX, BASKET_PRM_##S##_STATE0_TBL8_IDX, \
                BASKET_PRM_##S##_STATE0_TBL9_IDX,                                                                     \
            },                                                                                                        \
            {                                                                                                         \
                BASKET_PRM_##S##_STATE1_TBL0_IDX, BASKET_PRM_##S##_STATE1_TBL1_IDX, BASKET_PRM_##S##_STATE1_TBL2_IDX, \
                BASKET_PRM_##S##_STATE1_TBL3_IDX, BASKET_PRM_##S##_STATE1_TBL4_IDX, BASKET_PRM_##S##_STATE1_TBL5_IDX, \
                BASKET_PRM_##S##_STATE1_TBL6_IDX, BASKET_PRM_##S##_STATE1_TBL7_IDX, BASKET_PRM_##S##_STATE1_TBL8_IDX, \
                BASKET_PRM_##S##_STATE1_TBL9_IDX,                                                                     \
            },                                                                                                        \
            {                                                                                                         \
                BASKET_PRM_##S##_STATE2_TBL0_IDX, BASKET_PRM_##S##_STATE2_TBL1_IDX, BASKET_PRM_##S##_STATE2_TBL2_IDX, \
                BASKET_PRM_##S##_STATE2_TBL3_IDX, BASKET_PRM_##S##_STATE2_TBL4_IDX, BASKET_PRM_##S##_STATE2_TBL5_IDX, \
                BASKET_PRM_##S##_STATE2_TBL6_IDX, BASKET_PRM_##S##_STATE2_TBL7_IDX, BASKET_PRM_##S##_STATE2_TBL8_IDX, \
                BASKET_PRM_##S##_STATE2_TBL9_IDX,                                                                     \
            },                                                                                                        \
        };                                                                                                            \
        rKind = anKind[nFirst][nSecond];                                                                              \
        rKind = rKind > 1 ? 1 : (rKind < 0 ? 0 : rKind);                                                              \
        return anFrame[nFirst][nSecond];                                                                              \
    }

    DEFINE_PARTY_LIP_SYNC_FRAME(CNpcBossPackunBasketParty70, S70)

    void CNpcBossPackunBasketParty70::CheckDisp_()
    {
        static const int bDisp = 1;
        if (m_pMdl) {
            if (bDisp) {
                m_pMdl->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            } else {
                m_pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
            m_pMdl->SetAlphaRateF(m_fAlpha);
        }
        if (m_pShadow2) {
            if (bDisp) {
                m_pShadow2->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            } else {
                m_pShadow2->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
            m_pShadow2->SetAlphaRateF(0.1f);
        }
        if (m_pShadow) {
            if (bDisp) {
                m_pShadow->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            } else {
                m_pShadow->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
            m_pShadow->SetAlphaRateF(0.1f);
        }
    }

    DEFINE_PARTY_LIP_SYNC_FRAME(CNpcBossPackunBasketParty71, S71)
    DEFINE_PARTY_LIP_SYNC_FRAME(CNpcBossPackunBasketParty72, S72)

#undef DEFINE_PARTY_LIP_SYNC_FRAME

    // ---------------------------------------------------------------------
    // CNpcBossPackunBasketBase (the functions the original has as weak
    // copies at the end of the file)
    // ---------------------------------------------------------------------

    void CNpcBossPackunBasketBase::CalcRoot_()
    {
        if (m_pMdl) {
            m_pMdl->m_vTrans = m_vPos;
            m_pMdl->SetRotXYZ(m_vRot.x, m_vRot.y, m_vRot.z);
            m_pMdl->m_vScale = m_vScale;
            m_pMdl->CalcRootMtx();
            m_pMdl->CalcWorldMatrixAll(0);
        }
        if (m_pSubMdl) {
            m_pSubMdl->m_vTrans = m_vPos;
            // The yaw from the court's direction, within MAX_DIR_LIP. (the
            // original works it out but turns the model by m_vRot all the
            // same)
            bcs::FVEC3 vRot = m_vRot;
            vRot.y = RoundRadPI(m_vRot.y - m_fBaseRotY);
            float fMax = BASKET_PRM_S07_PACKUN_MAX_DIR_LIP;
            if (fabsf(vRot.y) > fMax) {
                vRot.y = vRot.y > 0.0 ? fMax : -fMax;
            }
            m_pSubMdl->SetRotXYZ(m_vRot.x, m_vRot.y, m_vRot.z);
            float fScale = BASKET_PRM_S07_PACKUN_LIP_SCALE;
            m_pSubMdl->m_vScale = bcs::FVEC3(fScale, fScale, fScale);
            m_pSubMdl->CalcRootMtx();
            m_pSubMdl->CalcWorldMatrixAll(0);
        }
    }

    float CNpcBossPackunBasketBase::GetRemainderMouthCloseFrame()
    {
        if (IsMouthOpen()) {
            return 0.0f;
        }
        if (GetMotIdx() == MOT_IDLE && FsmqGetState() == STATE_IDLE) {
            return FsmqGetWork<STATE_WORK_IDLE>()->m_fTimer;
        }
        return 0.0f;
    }

    int CNpcBossPackunBasketBase::IsMouthOpen() const
    {
        return m_pBodyColl->IsEnable(2);
    }

    float CNpcBossPackunBasketBase::GetRemainderMouthOpenFrame()
    {
        if (!IsMouthOpen()) {
            return 0.0f;
        }
        switch (GetMotIdx()) {
            case MOT_MOUTH_OPEN_LOOP:
            case MOT_PARTY_EAT:
                if (FsmqGetState() == STATE_SLEEP) {
                    return 9999.9f;
                }
                if (FsmqGetState() == STATE_IDLE) {
                    return FsmqGetWork<STATE_WORK_IDLE>()->m_fTimer;
                }
                break;
            case MOT_SLEEP:
                return 9999.9f;
        }
        return 0.0f;
    }

    void CNpcBossPackunBasketBase::CallbackBallCatch_(CCollBodyUnit *pBody, CBall *pBall)
    {
        // A shot at the boss's goal goes in (in the party game any shot).
        if (pBall && pBall->IsShootMode() && !pBall->IsAirBall() &&
            (CGame::g_GameWork.GetGameType() == CGame::GAME_TYPE_PARTY ||
             pBall->GetTargetGoalIdx() == GetGoalIdx())) {
            long nTeam = CChrMan::GetChrCore(pBall->GetShotChrIdx())->GetTeamNo();
            pBall->TrgGoalIn(pBall->GetShotChrIdx(), nTeam, GetGoalIdx(), pBall->GetShotArea(),
                             pBall->GetShootType());
        }
    }

    void CNpcBossPackunBasketBase::CallbackAttackHit_(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                                     const bcs::FVEC3 &rPos, unsigned long)
    {
        switch (pBody->GetOwnerKind()) {
            case 2: // a ball
                PlaySe_(s_aSeAttackHit);
                break;
            case 1: { // a character
                CChrCore *pChr = CChrMan::GetChrCore(pBody->m_nOwnerIdx);
                if (pChr && pChr->IsPadCtrl()) {
                    CGameBasket::GetInstance()->CameraQuake_(0x13, pChr->GetPlayerNo());
                }
                break;
            }
        }
    }

    // ---------------------------------------------------------------------
    // CNpcBossPackunBasketParty70
    // ---------------------------------------------------------------------

    int CNpcBossPackunBasketParty70::MethdEat(long nChr, long nKind, long)
    {
        if (!CanbeEat()) {
            return 0;
        }
        long nEff = -1;
        if (nKind < 9 || nKind > 11) {
            nEff = nKind == 3 ? 0xD7 : 0xD8;
        }
        if (CGame::g_GameWork.GetGameType() == CGame::GAME_TYPE_PARTY && FsmqGetState() != STATE_SLEEP) {
            Eff_Stop(9);
            Eff_CreateAttachAuto(9, 0xD9, 3);
        }
        if (nEff > 0) {
            Eff_Stop(10);
            Eff_CreateAttachAuto(10, nEff, 3);
        }
        if (nKind == 3) {
            PlaySe_(s_aSeEatBig);
        } else {
            PlaySe_(s_aSeEat);
        }
        PlaySePos(0xC60, m_vPos, 0, -1, 0.0f, 1.0f, 0.0f);
        MotPlay_(MOT_PARTY_EAT, 0, 8.0f, 0.0f);
        return 1;
    }

    int CNpcBossPackunBasketParty70::CanbeEat()
    {
        // (asleep, idle or melting)
        long nState = FsmqGetState();
        if ((unsigned long)nState > STATE_IDLE && nState != STATE_MELT) {
            return 0;
        }
        return 1;
    }

    void CNpcBossPackunBasketParty70::LoopStateEndIdle()
    {
        if (GetMotIdx() == MOT_END_IDLE_START && IsAnimEnd()) {
            MotPlay_(MOT_END_IDLE, 1, 4.0f, 0.0f);
        }
    }

    void CNpcBossPackunBasketParty70::CtorStateEndIdle()
    {
        MotPlay_(MOT_END_IDLE_START, 0, 4.0f, 0.0f);
    }

    void CNpcBossPackunBasketParty70::LoopStateEat()
    {
        STATE_WORK_EAT *pWork = FsmqGetWork<STATE_WORK_EAT>();
        AnimGetMotID(); // (result unused)
        if (IsAnimEnd()) {
            ResetState_(pWork->m_nNextState);
        }
    }

    void CNpcBossPackunBasketParty70::CtorStateEat()
    {
        MotPlay_(MOT_PARTY_EAT, 0, 4.0f, 0.0f);
    }

    void CNpcBossPackunBasketParty70::DtorStateSleep()
    {
        SleepEnd_();
        CGame::SendMessage(0x39, 0, 0, 0);
    }

    void CNpcBossPackunBasketParty70::LoopStateSleep()
    {
        STATE_WORK_SLEEP *pWork = FsmqGetWork<STATE_WORK_SLEEP>();
        switch (GetMotIdx()) {
            case MOT_SLEEP_START:
                if (!IsAnimEnd()) {
                    break;
                }
                // fall through
            case MOT_PARTY_EAT:
                if (IsAnimEnd()) {
                    if (pWork->m_bEffect == 0) {
                        if (!(m_nPackunFlag & 2)) {
                            Eff_Stop(8);
                            Eff_CreateAttachAuto(8, 0xD4, 3);
                        }
                        Eff_Stop(9);
                        Eff_CreateAttachAuto(9, 0xD5, 3);
                        pWork->m_bEffect = 1;
                    }
                    MotPlay_(MOT_SLEEP, 1, 8.0f, 0.0f);
                }
                break;
            case MOT_SLEEP:
                break;
            default:
                MotPlay_(MOT_SLEEP_START, 0, 8.0f, 0.0f);
                break;
        }
    }

    void CNpcBossPackunBasketParty70::CtorStateSleep()
    {
        SleepStart_();
        CGame::SendMessage(0x38, 0, 0, 0);
    }

    void CNpcBossPackunBasketParty70::LoopStateIdle()
    {
        STATE_WORK_IDLE *pWork = FsmqGetWork<STATE_WORK_IDLE>();
        GetFrameRateF(); // (result unused)
        RotateYRate(m_fBaseRotY, 0.1f);
        switch (GetMotIdx()) {
            case MOT_IDLE_START:
            case MOT_MOUTH_CLOSE:
                if (IsAnimEnd()) {
                    MotPlay_(MOT_IDLE, 1, 8.0f, 0.0f);
                    MotSetRate_(1.0f);
                }
                return;
            case MOT_PARTY_EAT:
                // After eating, go on with the mouth open.
                if (!IsAnimEnd()) {
                    return;
                }
                if (pWork->m_nStep >= 3) {
                    pWork->m_nStep = 2;
                }
                break;
            case MOT_IDLE:
                if (m_bEnd) {
                    MethdEndIdle();
                    return;
                }
                break;
        }
        UpdateLipSync_(pWork);
    }

    void CNpcBossPackunBasketParty70::CtorStateIdle()
    {
        // The lip sync goes on where it was; its table follows the party
        // level.
        long nLipIdx = m_nLipIdx;
        CNpcBossPackunBasketBase::CtorStateIdle();
        STATE_WORK_IDLE *pWork = FsmqGetWork<STATE_WORK_IDLE>();
        m_nLipIdx = nLipIdx;
        pWork->m_nLipTbl = CGameBasket::GetPartyLv();
    }

    // ---------------------------------------------------------------------
    // CNpcBossPackunBasketGame
    // ---------------------------------------------------------------------

    void CNpcBossPackunBasketGame::CtorStateMelt()
    {
        MotPlay_(MOT_MELT, 0, 8.0f, 0.0f);
    }

    void CNpcBossPackunBasketGame::DtorStateSleep()
    {
        SleepEnd_();
    }

    void CNpcBossPackunBasketGame::CtorStateSleep()
    {
        SleepStart_();
    }

    // ---------------------------------------------------------------------
    // CNpcBossPackunBasketBase::CMudBombBasket (weak copies in the original)
    // ---------------------------------------------------------------------

    void CNpcBossPackunBasketBase::CMudBombBasket::Explode()
    {
        CMudBomb::Explode();
        long nType;
        switch (CGame::g_GameWork.GetStage()) {
            case 7:
                nType = 0;
                break;
            case 0x46:
                nType = 1;
                break;
            case 0x47:
                nType = 2;
                break;
            case 0x48:
                nType = 3;
                break;
            default:
                return;
        }
        PlaySeAt(s_aSeMudExplode[nType], GetPos());
    }

    float CNpcBossPackunBasketBase::CMudBombBasket::GetParam_GroundHeight() const
    {
        return 0.3f;
    }
}
