// msw::CChrCoreBoss: the character core of the boss game. Its constructor
// and destructor, the model and collision set-up, the per-frame update, the
// ball catch and pass helpers and the boss-game virtuals. The vtable is
// emitted here, so the file ends with the weak copies of CChrCore's and
// CChrCoreBoss's inline virtuals. The characters' own classes are files of
// their own (game/boss/CChrCoreBoss<Name>.cpp).
#include "game/msw/CChrCoreBoss.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC2.hpp"
#include "bcs/CAnmBlendMotion.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CCollDefUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CDefCollision.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSpeedCtrl.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpitBoss.hpp"
#include "game/msw/CItem.hpp"
#include "game/msw/CNpcBase.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/math/triangular.h"

namespace {

    // (meaning not known) Three entries of { -1, n, 0 } for n = 58 to 60
    // (most likely state or motion numbers). The original's .rodata starts
    // with them, but nothing left in the file reads them: whatever used them
    // was stripped. Kept in place by UnusedReferences (see
    // notes/decisions.md, "Unreferenced data kept by dead references").
    const long s_aUnkTable[3][3] = {
        { -1, 58, 0 },
        { -1, 59, 0 },
        { -1, 60, 0 },
    };

    inline void UnusedReferences()
    {
        (void)s_aUnkTable;
    }

    // (guessed helpers) An angle brought into [0, 2 pi), and into
    // [-pi, pi].
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

    inline float WrapRadPi(float rad)
    {
        while (rad > 3.1415927f) {
            rad -= 6.2831855f;
        }
        while (rad < -3.1415927f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }

    inline float Min(float a, float b)
    {
        return a > b ? b : a;
    }

    // (guessed helper) The boss of the battle: the NPC of kind 0, or else
    // of kind 15.
    inline msw::CNpcBase *SearchBoss()
    {
        msw::CNpcBase *pBoss = msw::CChrMan::NpcSearch(0, 0);
        if (!pBoss) {
            pBoss = msw::CChrMan::NpcSearch(15, 0);
        }
        return pBoss;
    }

    // The dizzy state's work (names guessed; as in game/boss/unit_1C248.cpp,
    // which runs the state).
    struct STATE_WORK_DIZZY {
        long m_nStep;          // 0x00
        unsigned long m_nFlag; // 0x04
        long m_nType;          // 0x08: 1 can be shaken off, 2 a fixed time
        float m_fTime;         // 0x0C: frames left
    };

    // (name guessed) The yaw of a point seen from the origin, in [0, 2 pi).
    inline float GetRotYXZ(const bcs::FVEC3 &rPos)
    {
        return WrapRad(nw4r::math::Atan2Rad(rPos.x, rPos.z));
    }

}

msw::CChrCoreBoss::CChrCoreBoss()
    : m_pCcParam(0), m_nBossFlag(0), m_nBallCatchMode(0), m_nBallCatchShape(0), m_fBallCatchUnk7CC(0.3f),
      m_hSlowCtrl(0), m_nSlowCheck(0), m_hStickObj(0), m_fHPMax(100.0f), m_fHP(100.0f), m_fDownTime(0.0f)
{
    UnusedReferences();
    for (int i = 0; i < 8; i++) {
        m_aReceiveCatchPos[i].m_Low.Set(0.0f, 0.0f, 0.0f);
        m_aReceiveCatchPos[i].m_High.Set(0.0f, 0.0f, 0.0f);
        m_aTossCatchPos[i].m_Low.Set(0.0f, 0.0f, 0.0f);
        m_aTossCatchPos[i].m_High.Set(0.0f, 0.0f, 0.0f);
        m_afReceiveRot[i] = 0.0f;
        m_afReceiveHeight[i] = 0.0f;
        m_afReceiveDist[i] = 0.0f;
        m_afTossRot[i] = 0.0f;
    }
    m_fJumpTossRot = 0.0f;
    m_fJumpTossRot2 = 0.0f;
    m_SpikeOfs.Set(0.0f, 0.0f, 0.0f);
    m_SpikeOfsCharge.Set(0.0f, 0.0f, 0.0f);
    m_BallCatchPos0.Set(0.0f, 0.0f, 0.0f);
    m_BallCatchPos1.Set(0.0f, 0.0f, 0.0f);
    m_fBallCatchRange = 0.0f;
    m_fBallCatchHeight = 0.0f;
    m_fBallCatchRange2 = 0.0f;
    m_nBallCatchNode = 0;
}

msw::CChrCoreBoss::~CChrCoreBoss()
{
    if (m_pCcParam) {
        delete m_pCcParam;
    }
    m_pCcParam = 0;
}

void msw::CChrCoreBoss::CreateMdl_()
{
    if (m_pCcParam) {
        delete m_pCcParam;
    }
    m_pCcParam = 0;
    m_pCcParam = NewCcParam_(0);
    CChrCore::CreateMdl_();
    GetBallImpactOffsetPos_();
}

void msw::CChrCoreBoss::CreateColl_()
{
    m_pCollBody = new (0) CBodyCollision(7, 1, 2, m_pMdl->GetHandle());
    m_pCollBody->SetPushEnable(0);
    m_pCollBody->SetPushEnable(1);
    m_pCollBody->SetBodyEnable(2);
    m_pCollBody->SetCatchEnable(3);
    m_pCollBody->SetCatchEnable(4);
    m_pCollBody->SetCatchEnable(5);
    m_pCollBody->SetCatchEnable(6);
    m_pCollBody->SetCollType(1, 3);
    m_pCollBody->SetCollType(2, 3);
    m_pCollBody->SetCollType(3, 3);
    m_pCollBody->SetCollType(4, 0);
    m_pCollBody->SetCollType(5, 1);
    m_pCollBody->SetCollType(6, 3);
    m_pCollBody->ResetTargetFlag(0, 0x14);
    m_pCollBody->SetTargetFlag(1, 0x14);
    m_pCollBody->SetPushRank(1, 5);
    m_pCollBody->SetPushPower(1, 6);
    m_pCollBody->SetTargetFlag(3, 4);
    m_pCollBody->SetTargetFlag(4, 4);
    m_pCollBody->SetTargetFlag(5, 4);
    m_pCollBody->SetTargetFlag(6, 4);
    m_pCollBody->GetColl(4)->OnFlag10(4);
    m_pCollBody->SetIndexAll(GetPlayerNo());
    m_pCollBody->SetMyNoHitFlagAll(1 << GetPlayerNo());
    m_pCollBody->SetBodyDamageCallbackAll(CallbackBodyDamage, this);
    m_pCollBody->SetBodyPushCallbackAll(CallbackBodyPush, this);
    m_pCollBody->SetBallCatchCallback(3, CallbackBallCatch, this);
    m_pCollBody->SetBallCatchCallback(4, CallbackBallCatch2, this);
    m_pCollBody->SetBallCatchCallback(5, CallbackBallCatch, this);
    m_pCollBody->SetBallCatchCallback(6, CallbackBallCatch3, this);
    ResetCollBodyPush_(0.5f, 0.5f, 0.0f, 0.0f);

    m_pCollAtk = new (0) CAtkCollision(1, 1, 0, m_pMdl->GetHandle());
    m_pCollAtk->SetIndexAll(GetPlayerNo());
    m_pCollAtk->SetTeamNoAll(GetTeamNo());
    m_pCollAtk->SetAttackHitCallbackAll(CallbackAttackHit, this);
    m_pCollAtk->SetMyNoHitFlagAll(1 << GetPlayerNo());

    m_pCollDef = new (0) CDefCollision(2, 1, 2, m_pMdl->GetHandle());
    m_pCollDef->GetColl(0)->SetIndex(0);
    m_pCollDef->GetColl(1)->SetIndex(1);
    m_pCollDef->SetTeamNoAll(GetTeamNo());
    m_pCollDef->GetColl(0)->OnFlagA0(1);
    m_pCollDef->GetColl(1)->OnFlagA0(1);
    m_pCollDef->SetMyNoHitFlagAll(1 << GetPlayerNo());
    static_cast<CCollDefUnit *>(m_pCollDef->GetColl(0))->SetReflectHitCallback(CallbackDefReflectHit, this);
    static_cast<CCollDefUnit *>(m_pCollDef->GetColl(1))->SetReflectHitCallback(CallbackDefReflectHit, this);
    m_pCollBody->SetDisableAll();
}

// (inline in the original; name guessed) Poses the model at frame fFrame of
// motion nMot (on the spare motion slot 3) and reads where virtual node 4,
// the ball, is then.
inline void msw::CChrCoreBoss::GetMotVNodePos_(long nMot, float fFrame, bcs::FVEC3 &rPos)
{
    if (m_pMdl) {
        AnimPlay(3, nMot, 0, 0.0f, Max(fFrame, 0.0f));
        m_pMdl->UpdateFrame(0.0f);
        Util_NodePatchTransCopy();
        RecalcAllMatrix();
        AnimStop(3);
        GetVNodePos(4, rPos);
    }
}

// Measures, once the model is loaded, where the ball is held in each
// receive and toss pose and in the spikes, from the motions themselves.
void msw::CChrCoreBoss::GetBallImpactOffsetPos_()
{
    if (m_pMdl) {
        m_pMdl->m_vTrans.Set(0.0f, 0.0f, 0.0f);
        m_pMdl->SetRot(0.0f, 0.0f, 0.0f);
        if (m_pAnm) {
            m_pAnm->SetTrgCallback(CallbackMotTrg, 0);
        }

        float fLowFrame = 7.0f;
        float fHighFrame = 16.0f;
        float fTossFrame = 0.0f;
        float fTossHighFrame = 9.0f;
        for (long i = 0; i < 8; i++) {
            GetMotVNodePos_(GetReceivePoseMotID_(i, 2), fLowFrame, m_aReceiveCatchPos[i].m_Low);
            GetMotVNodePos_(GetReceivePoseMotID_(i, 2), fHighFrame, m_aReceiveCatchPos[i].m_High);
            GetMotVNodePos_(GetTossPoseMotID_(i, 2), fTossFrame, m_aTossCatchPos[i].m_Low);
            GetMotVNodePos_(GetTossPoseMotID_(i, 2), fTossHighFrame, m_aTossCatchPos[i].m_High);
        }

        GetMotVNodePos_(0x60, GetBallShotTrgFrame_(0x60, 6.0f), m_SpikeOfs);
        GetMotVNodePos_(0x67, GetBallShotTrgFrame_(0x67, 6.0f), m_SpikeOfsCharge);

        bcs::FVEC3 pos;
        for (long i = 0; i < 8; i++) {
            long nMot = GetReceivePoseMotID_(i, 1);
            GetMotVNodePos_(nMot, AnimGetResFrameNum(nMot) - 1.0f, pos);
            m_afReceiveHeight[i] = pos.y;
            m_afReceiveRot[i] = GetRotYXZ(pos);
            m_afReceiveDist[i] = bcs::BCS_Vec_GetLenXZ(pos);

            nMot = GetTossPoseMotID_(i, 0);
            GetMotVNodePos_(nMot, AnimGetResFrameNum(nMot) - 1.0f, pos);
            m_afTossRot[i] = GetRotYXZ(pos);
        }

        bcs::FVEC3 jumpPos;
        float fJumpFrame = 0.0f;
        GetMotVNodePos_(0x51, fJumpFrame, jumpPos);
        m_fJumpTossRot = GetRotYXZ(jumpPos);
        m_fJumpTossRot2 = GetRotYXZ(jumpPos);

        if (m_pAnm) {
            m_pAnm->SetTrgCallback(CallbackMotTrg, this);
        }
    }
}

// The frame of motion nMot the ball is hit at (its trigger 2), or fDefault
// when the motion has none.
float msw::CChrCoreBoss::GetBallShotTrgFrame_(long nMot, float fDefault) const
{
    float fFrame = AnimSearchTrgFrame(nMot, 2, -1.0f);
    if (!(fFrame >= 0.0f)) {
        fFrame = fDefault;
    }
    return fFrame;
}

// The order the characters are updated in: the players of team nTeam
// first, then the others, the ball holder last.
long msw::CChrCoreBoss::GetMainOrderValue(long nTeam)
{
    long nValue = GetPlayerNo();
    if (nTeam != GetTeamNo()) {
        nValue += 100;
    }
    if (m_nStatusFlag & 1) {
        nValue += 10000;
    }
    return nValue;
}

void msw::CChrCoreBoss::Reset_()
{
    CChrCore::Reset_();
    m_nBossFlag = 0;
    m_BallCatchPos0.Set(0.0f, 0.0f, 0.0f);
    m_BallCatchPos1.Set(0.0f, 0.0f, 0.0f);
    m_fBallCatchRange = 0.0f;
    m_fBallCatchHeight = 0.0f;
    m_fBallCatchRange2 = 0.0f;
    m_nBallCatchMode = 0;
    m_nBallCatchShape = 0;
    m_nBallCatchNode = 0;
    m_fBallCatchUnk7CC = 0.3f;
    if (m_hSlowCtrl) {
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hSlowCtrl);
        if (pTask && !pTask->IsDelete()) {
            pTask->Delete();
        }
        m_hSlowCtrl = 0;
    }
    m_nSlowCheck = 0;
    m_fSlowSpeed = 1.0f;
    m_fSlowInFrame = 1.0f;
    m_fSlowOutFrame = 1.0f;
    m_SlowOfs.Set(0.0f, 0.0f, 0.0f);
    m_SlowRange.Set(0.0f, 0.0f, 0.0f);
    m_SlowRange2.Set(0.0f, 0.0f, 0.0f);
    m_fHPMax = 100.0f;
    m_fHP = 100.0f;
    m_fDownTime = 0.0f;
    StickObjDestroy_(0.0f);
}

// Slows the game down while a ball comes within the range of a point near
// the character (nCheck 2: only balls that ... ).
void msw::CChrCoreBoss::EnableSlowCheck_(long nCheck, const bcs::FVEC3 &rOfs, const bcs::FVEC3 &rRange,
                                         const bcs::FVEC3 &rRange2, float fSpeed, float fInFrame, float fOutFrame)
{
    m_nSlowCheck = nCheck;
    m_fSlowSpeed = fSpeed;
    m_fSlowInFrame = fInFrame;
    m_fSlowOutFrame = fOutFrame;
    m_SlowOfs = rOfs;
    m_SlowRange = rRange;
    m_SlowRange2 = rRange2;
}

void msw::CChrCoreBoss::DisableSlowCheck_(float fFrame)
{
    m_nSlowCheck = 0;
    if (m_hSlowCtrl) {
        CGameSpeedCtrl *pCtrl = static_cast<CGameSpeedCtrl *>(bcs::CTaskMan::GetTask(m_hSlowCtrl));
        if (pCtrl && !pCtrl->IsDelete()) {
            pCtrl->RestoreSpeed(fFrame, 1, 0);
        }
        m_hSlowCtrl = 0;
    }
}

void msw::CChrCoreBoss::CheckSlow_()
{
    if (m_nSlowCheck) {
        bcs::FVEC3 pos(GetPos());
        bcs::FVEC3 ofs;
        bcs::BCS_Vec_RotY(ofs, m_SlowOfs, GetRotY());
        bcs::FVEC3Add(&pos, &pos, &ofs);

        CGameSpeedCtrl *pCtrl = 0;
        bcs::FVEC3 range(m_SlowRange);
        if (m_hSlowCtrl) {
            pCtrl = static_cast<CGameSpeedCtrl *>(bcs::CTaskMan::GetTask(m_hSlowCtrl));
            if (pCtrl && !pCtrl->IsDelete()) {
                range = m_SlowRange2;
            } else {
                pCtrl = 0;
            }
        }

        float fRangeSq = range.x * range.x;
        int bHit = 0;
        if (IsOffenseTeam()) {
            long nBall = CBallMan::GetBallNum();
            for (long i = 0; i < nBall; i++) {
                CBall *pBall = CBallMan::GetBallPtr(i);
                if (pBall && pBall->IsPassMode() && (m_nSlowCheck != 2 || pBall->IsVolleyChargeBall())) {
                    bcs::FVEC3 ballPos(pBall->GetPos());
                    if (PSVECSquareDistance((const Vec *)&ballPos, (const Vec *)&pos) <= fRangeSq) {
                        bHit = 1;
                        break;
                    }
                }
            }
        }

        if (bHit) {
            if (!pCtrl) {
                CGameSpeedCtrl *pNew = CGame::CreateSpeedCtrl();
                if (pNew) {
                    pNew->StartSpeedAnim(1.0f, m_fSlowSpeed, m_fSlowInFrame, 0, 0);
                    m_hSlowCtrl = pNew->GetId();
                }
            }
        } else if (pCtrl) {
            pCtrl->RestoreSpeed(m_fSlowOutFrame, 1, 0);
            m_hSlowCtrl = 0;
        }
    }
}

// The effect of a change of condition nCondition (1 a star, 4 ..., 8 a
// damage of kind nType 8 or 12); the effects of the other slots stop.
void msw::CChrCoreBoss::SetConditionEffect_(long nCondition, unsigned long nType)
{
    long nSlot = 6;
    switch (nCondition) {
        case 1:
            Eff_CreateAttach(6, 0xB, m_nEffNodeSpike, 0, 0, 3, 0);
            nSlot = 7;
            break;
        case 4: {
            long nEff = 0x11;
            switch (GetChrIdS()) {
                case 7:
                case 10:
                    nEff = 0x10;
                    break;
                case 16:
                    nEff = 0x12;
                    break;
            }
            Eff_CreateAttach(6, nEff, m_nEffNodeSub2, 0, 0, 3, 0);
            nSlot = 7;
            break;
        }
        case 8:
            if (nType == 8 || nType == 12) {
                Eff_CreateAttach(6, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
                nSlot = 7;
            }
            break;
        case 9:
            switch (GetChrIdS()) {
                case 6:
                    break;
                case 7:
                    break;
                case 8:
                    break;
                case 10:
                    break;
                case 11:
                    break;
                case 12:
                    break;
            }
            break;
    }
    for (; nSlot < 9; nSlot++) {
        Eff_Stop(nSlot);
    }
}

float msw::CChrCoreBoss::CalcMoveSpeedRate_(float fSpeed, long)
{
    return CalcContiditionMoveSpeed_(fSpeed);
}

float msw::CChrCoreBoss::CalcContiditionMoveSpeed_(float fSpeed) const
{
    switch (m_nComLevel) {
        case 1:
            fSpeed *= 1.7f;
            break;
        case 4:
            fSpeed *= 1.3f;
            break;
        case 6:
            fSpeed *= 0.5f;
            break;
        case 7:
            fSpeed *= 0.6f;
            break;
        case 8:
            fSpeed *= 0.6f;
            break;
        case 10:
            fSpeed *= 0.7f;
            break;
    }
    return fSpeed;
}

// (names guessed) The sizes of the collisions of each body size class: the
// radius and height of the push, body, catch and damage collisions (pairs
// built by the static initialiser) and four more values.
struct msw::CChrCore::PRM_BODY_SIZE {
    bcs::FVEC2 m_PushSize;
    bcs::FVEC2 m_BodySize;
    bcs::FVEC2 m_CatchSize;
    bcs::FVEC2 m_CatchSize2;
    bcs::FVEC2 m_DamageSize;
    float m_afUnk28[4];
};

// (guessed name) The body size class of the character
// (m_pChrData->m_nBodySize): 0 small, 1 middle, 2 large.
namespace {
    msw::CChrCore::PRM_BODY_SIZE s_aBodySize[3] = {
        { bcs::FVEC2(0.5f, 0.5f), bcs::FVEC2(0.5f, 0.5f), bcs::FVEC2(0.5f, 0.5f), bcs::FVEC2(0.5f, 0.5f),
          bcs::FVEC2(0.5f, 0.8f), { 0.5f, 0.8f, 0.7f, 0.2f } },
        { bcs::FVEC2(0.5f, 0.5f), bcs::FVEC2(0.5f, 0.5f), bcs::FVEC2(0.5f, 0.5f), bcs::FVEC2(0.5f, 0.5f),
          bcs::FVEC2(0.5f, 0.8f), { 0.5f, 0.8f, 0.7f, 0.2f } },
        { bcs::FVEC2(1.0f, 1.0f), bcs::FVEC2(1.0f, 1.0f), bcs::FVEC2(1.0f, 1.0f), bcs::FVEC2(1.0f, 1.0f),
          bcs::FVEC2(1.2f, 1.2f), { 0.5f, 0.8f, 0.7f, 0.2f } },
    };
}

const msw::CChrCore::PRM_BODY_SIZE *msw::CChrCoreBoss::GetPrmBodySize() const
{
    return &s_aBodySize[m_pChrData->m_nBodySize];
}

float msw::CChrCoreBoss::GetPrmMoveSpeed() const
{
    return GetCcParam_()->m_Unk0C;
}

void msw::CChrCoreBoss::MainFunc_()
{
    m_nStatusFlag &= ~0x10;
    UpdateAreaCode_();
    if (m_UnkEC & 0x7F8000) {
        if (m_UnkEC & 0x8000) {
            if (!IsInvincible() && !(m_nStatusFlag & 0x80000)) {
                MethdDamage(m_nDamageFlag, m_fDamageRotY, m_fDamage, m_fDamageSizeTime, m_nDamageQuake,
                            m_fDamageUnk1F8, m_nDamageUnk1FC, m_nDamageUnk1FE);
            }
            m_UnkEC &= ~0x8000;
        }
        if (GetStatusFlag2() & 0x300000) {
            m_UnkEC &= ~0x300000;
        }
        if (m_UnkEC & 0x10000) {
            HookBallCatch_();
            m_UnkEC &= ~0x10000;
        }
        if (GetStatusFlag2() & 0x80000) {
            m_UnkEC &= ~0x80000;
        }
    }
    UpdateScaleState_();
    UpdateSpState_();
    if (!(m_UnkEC & 0x30000000)) {
        DriverSendMessage((CChrDrv::MSG_ID)0, 0, 0, 0, 0);
        m_nBossFlag &= ~8;
        if (DriverSendMessage((CChrDrv::MSG_ID)0x99, 0, 0, 0, 0)) {
            m_nBossFlag |= 8;
        }
    }
    FsmqExec();
    ClearOnBallChargePower();
    UpdateCountWork_();
    UpdateChargeBlink_();
    UpdatePos_();
    UpdateAnim_();
    UpdateScale_();
    RecalcAllMatrix();
    UpdateCollBody_();
    UpdateDisp_();
    CheckSlow_();
    m_nFlagF4 &= ~0x100000;
}

void msw::CChrCoreBoss::CheckCountWork__()
{
}

// Which part of its court the character stands in (m_nUnk17B: 0 the front
// zone, 1 the rest of the court, 2 outside), and the ground under it.
void msw::CChrCoreBoss::UpdateAreaCode_()
{
    long nArea = 2;
    bcs::FVEC3 pos;
    float fBack, fFront, fRight, fLeft;

    GetCompPos(pos);
    CStageMan::GetInstance()->GetTeamCourtRect(GetGuardCourtIdx(), &fLeft, &fRight, &fFront, &fBack);
    if (fLeft > fRight) {
        float f = fLeft;
        fLeft = fRight;
        fRight = f;
    }
    if (fFront > fBack) {
        float f = fFront;
        fFront = fBack;
        fBack = f;
    }
    if (pos.x >= fLeft && pos.x <= fRight && pos.z >= fFront && pos.z <= fBack) {
        CStageMan::GetInstance()->GetTeamCourtFrontZoneRect(GetGuardCourtIdx(), &fLeft, &fRight, &fFront, &fBack);
        if (fLeft > fRight) {
            float f = fLeft;
            fLeft = fRight;
            fRight = f;
        }
        if (fFront > fBack) {
            float f = fFront;
            fFront = fBack;
            fBack = f;
        }
        if (pos.x >= fLeft && pos.x <= fRight && pos.z >= fFront && pos.z <= fBack) {
            nArea = 0;
        } else {
            nArea = 1;
        }
    }
    m_nUnk17B = nArea;
    CCollMan::GetGrdAttr(pos, m_nGrdAttr, m_GrdFlow);
}

// The ball touched the catch collision: take it, unless the character
// already holds a ball or an arts event of another character runs.
void msw::CChrCoreBoss::HookBallCatch_()
{
    unsigned long hBall = m_hCatchBall;
    if (!hBall) {
        return;
    }
    m_hCatchBall = 0;
    if (m_nStatusFlag & 1) {
        return;
    }
    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(hBall));
    if (!pBall) {
        return;
    }
    if (!pBall->CanbeCatch()) {
        return;
    }
    if (CGame::IsReqArtsEvent() && GetPlayerNo() != CGame::g_nEventChrIdx) {
        return;
    }
    HookBallCatchCmn_(pBall);
}

// Takes pBall into the hand: sound nSe (when not negative), the item the
// character holds goes onto the ball (or turns into a coin when the ball
// takes none), then the drivers and the other characters are told.
void msw::CChrCoreBoss::BallCatch_(CBall *pBall, float fFrame, long nSe, unsigned long nSeFlag)
{
    if (nSe >= 0) {
        bcs::FVEC3 pos(pBall->GetPos());
        PlaySePos(nSe, &pos, nSeFlag, -1, 0.0f, 1.0f, 0.0f);
    }
    if (GetTeamNo() == pBall->GetShotTeamIdx()) {
        pBall->GetVolleyTouchNum(GetTeamNo());
    }
    CItem *pItem = GetAttachItem();
    if (pItem) {
        if (!pBall->AttachItemEffect(pItem->GetItemType())) {
            CGimmickMan::EffectCoinGetCreate(pItem->GetPos(), (DROP_COIN::CoinType)0);
            CGame::AddTeamCoin(GetTeamNo(), 1);
        }
        DeleteAttachItem();
    }
    pBall->SetSquashParam(pBall->GetVelocity(), 1.0f, fFrame, 2.0f, 4.0f);
    unsigned long nFlag = 2;
    if (pBall->IsArtsShootMode()) {
        nFlag |= 0x20;
    }
    AttachBall(pBall->GetBallNo(), 5, fFrame, nFlag);
    m_nStatusFlag &= ~0x2C;
    CGame::SetLooseBallMode(0);
    DriverSendMessage((CChrDrv::MSG_ID)0xD, pBall->GetBallNo(), GetPlayerNo(), 0, 0);
    CChrMan::AllSendMsg((CChrDrv::MSG_ID)0xE, pBall->GetBallNo(), GetPlayerNo(), 0, 0);
}

// The yaw towards the court the team attacks.
float msw::CChrCoreBoss::GetTargetCourtDir_()
{
    if (GetTargetCourtIdx()) {
        return 3.1415927f;
    }
    return 0.0f;
}

// Messages 13 and 14 (a ball caught or passed) end a dizziness of the
// fixed-time kind.
// (The original returns 0 in r3; CChrCore declares the slot void, as the
// other modules' callers use it.)
void msw::CChrCoreBoss::SendMessage_(long nMsg, long, long, long)
{
    if (nMsg == 13 || nMsg == 14) {
        if (FsmqGetState() == STATE_DIZZY) {
            STATE_WORK_DIZZY *pWork = FsmqGetWork<STATE_WORK_DIZZY>();
            if (pWork->m_nType == 2) {
                pWork->m_fTime = 0.0f;
            }
        }
    }
}

// Sends pBall on after a receive of kind nType (1, 2: a toss, 4 to 6: a
// jump toss, others a receive) to the player the pass goes to: its sound,
// effect (none when bNoEffect) and voice (nVoice when not negative).
void msw::CChrCoreBoss::Util_ReceiveBall_(CBall *pBall, long nType, long nCatch, int bNoEffect, long nVoice)
{
    m_nBossFlag &= ~7;

    long nPass;
    switch (nType) {
        case 0:
            nPass = 3;
            break;
        case 1:
        case 2:
            nPass = 2;
            break;
        case 4:
            nPass = 3;
            break;
        case 5:
        case 6:
            nPass = 2;
            break;
        default:
            nPass = 3;
            break;
    }

    long nEff;
    long nSe;
    long nVoiceId;
    switch (nType) {
        case 0:
        case 1:
        case 2:
            nEff = 0x56E;
            nSe = 0xC8E;
            nVoiceId = 0x3C;
            break;
        case 4:
        case 5:
        case 6:
            nEff = 0x56E;
            nSe = 0xC8B;
            nVoiceId = 0x39;
            break;
        default:
            nEff = 0x56C;
            nSe = 0xC8C;
            nVoiceId = 0x3E;
            break;
    }
    if (nVoice >= 0) {
        nVoiceId = nVoice;
    }

    long nTo = SearchPassToChrIdx_(0);
    bcs::FVEC3 target;
    long nKind = Util_SetPassBallTargetPos_(target, nTo, 0, IsChargeBall_(pBall));
    if (bNoEffect) {
        nEff = -1;
    }
    bcs::FVEC3 pos(pBall->GetPos());
    if (nSe >= 0) {
        PlaySePos(nSe, &pos, 1, -1, 0.0f, 1.0f, 0.0f);
    }
    Eff_CreateSimple(nEff, pos, GetRotY(), 3, 0, 0);
    if (nVoiceId >= 0) {
        PlayVoice(nVoiceId, 0);
    }
    pBall->SetVsBossPassMode(target, nKind, GetPlayerNo(), GetTeamNo(), nTo, nCatch, PlayGetOutline_(1));
    CChrMan::AllSendMsg((CChrDrv::MSG_ID)0x9C, pBall->GetBallNo(), GetPlayerNo(), GetTeamNo(), GetTeamNo());
    DriverSendMessage((CChrDrv::MSG_ID)0x53, nPass, 1, 0, 0);
}

// Lets the held ball fall towards fRotY. The original works out a speed and
// does nothing with it (whatever used it is not in this build).
void msw::CChrCoreBoss::DropBall_(float fRotY)
{
    CBall *pBall = GetAttachBall();
    DetachBall();
    if (pBall) {
        fRotY = WrapRad(fRotY);
        bcs::FVEC3 vel;
        bcs::BCS_VecZ_RotY(vel, 0.016666668f * bcs::BCS_RandRange(6.0f, 7.0f), fRotY);
        vel.y = 0.016666668f * bcs::BCS_RandRange(1.0f, 1.5f);
    }
}

// Not zero when the character may touch ball nBall now: a pass may only be
// taken by its receiver, any other ball only by the other team's
// characters... (by the shooter's number).
int msw::CChrCoreBoss::IsBossRightBallTouch(long nBall) const
{
    CBall *pBall = CBallMan::GetBallPtr(nBall);
    if (!pBall) {
        return 0;
    }
    if (pBall->IsScored()) {
        return 0;
    }
    if (pBall->IsAlleyoopPassMode()) {
        if (pBall->GetShotChrIdx() == GetPlayerNo()) {
            return 1;
        } else {
            return 0;
        }
    }
    return pBall->GetShotChrIdx() != GetPlayerNo();
}

int msw::CChrCoreBoss::IsBossRightBallTouch_(const CBall *pBall) const
{
    if (!pBall) {
        return 0;
    }
    if (pBall->IsScored()) {
        return 0;
    }
    if (pBall->IsAlleyoopPassMode()) {
        return pBall->GetShotChrIdx() == GetPlayerNo();
    }
    return pBall->GetShotChrIdx() != GetPlayerNo();
}

// Not zero while the character holds the ball in a state where it counts
// as held (neutral, run, motion play while serving ...).
int msw::CChrCoreBoss::IsHoldBall() const
{
    switch (FsmqGetState()) {
        case 1:
        case 4:
        case 60:
            if ((m_nStatusFlag & 1) && IsBossServeMode()) {
                return 1;
            }
            break;
        case 43:
            if ((m_nStatusFlag & 1) && IsBossServeMode()) {
                return 1;
            }
            break;
    }
    return 0;
}

int msw::CChrCoreBoss::IsBossServeMode() const
{
    return m_nBossFlag & 1;
}

void *msw::CChrCoreBoss::GetHoldBall() const
{
    if (IsHoldBall()) {
        return GetAttachBall();
    }
    return 0;
}

float msw::CChrCoreBoss::GetCollSize_BodyDamage() const
{
    if (m_pCollBody == 0) {
        return 0.0f;
    } else {
        return m_pCollBody->GetRadius(2);
    }
}

int msw::CChrCoreBoss::IsBossSuperSpikeCurveType() const
{
    return GetCcParam_()->m_Unk00 == 1;
}

float msw::CChrCoreBoss::GetBossSuperSpikeCurveRate() const
{
    return 0.0f;
}

float msw::CChrCoreBoss::GetBossSuperSpikeCurveMaxDist() const
{
    return 0.0f;
}

float msw::CChrCoreBoss::GetBossSuperSpikeCurveMinDist() const
{
    return 0.0f;
}

float msw::CChrCoreBoss::GetBossBlockJumpDelayFrame() const
{
    return GetCcParam_()->m_Unk34;
}

int msw::CChrCoreBoss::IsBossEnableDiveReceive() const
{
    return GetCcParam_()->m_Unk88 == 0;
}

// The spike's progress, read from its work (STATE_WORK_SPIKE).
int msw::CChrCoreBoss::IsBossSpikeJumpStart() const
{
    if (FsmqGetState() != STATE_SPIKE) {
        return 0;
    }
    return FsmqGetWork<STATE_WORK_SPIKE>()->m_nStep >= 1;
}

int msw::CChrCoreBoss::IsBossSuperSpikeJump() const
{
    if (FsmqGetState() != STATE_SPIKE) {
        return 0;
    }
    return (FsmqGetWork<STATE_WORK_SPIKE>()->m_nFlag >> 6) & 1;
}

int msw::CChrCoreBoss::IsBossSpikeSwingEnd() const
{
    if (FsmqGetState() != STATE_SPIKE) {
        return 0;
    }
    STATE_WORK_SPIKE *pWork = FsmqGetWork<STATE_WORK_SPIKE>();
    if (pWork->m_nStep >= 4) {
        return 1;
    }
    return (pWork->m_nFlag >> 9) & 1;
}

int msw::CChrCoreBoss::IsBossSpikeFall() const
{
    if (FsmqGetState() != STATE_SPIKE) {
        return 0;
    }
    STATE_WORK_SPIKE *pWork = FsmqGetWork<STATE_WORK_SPIKE>();
    if (pWork->m_nStep > 1) {
        if (pWork->m_nStep >= 5) {
            return 1;
        }
        if (m_fJumpVelY < 0.0f && m_fJumpUnk224 <= 0.0f) {
            return 1;
        }
    }
    return 0;
}

int msw::CChrCoreBoss::IsAirTrickSwingEnd() const
{
    return 0;
}

int msw::CChrCoreBoss::IsAirTrickFall() const
{
    return 0;
}

int msw::CChrCoreBoss::IsBossBlockJumpStart() const
{
    return 0;
}

int msw::CChrCoreBoss::IsBossBlockFall() const
{
    return 0;
}

int msw::CChrCoreBoss::IsBossBlockSuccess() const
{
    return 0;
}

// Not zero while the spike jump marker is to be shown: during the spike
// until its swing ends, or while a spike could start.
int msw::CChrCoreBoss::IsBossDispSpikeJumpMarker() const
{
    if (FsmqGetState() == STATE_SPIKE && !IsBossSpikeSwingEnd() && !IsBossSpikeFall()) {
        return 1;
    }
    if (m_nBossFlag & 8) {
        return CanbeSpike(1);
    }
    int bBall = 0;
    if (GetTargetBall_()) {
        bBall = 1;
    }
    if (bBall && CanbeSpike(0)) {
        return 1;
    }
    return 0;
}

int msw::CChrCoreBoss::IsVsBossDead() const
{
    return m_fHP <= 0.0f;
}

long msw::CChrCoreBoss::GetVsBossMaxHP() const
{
    return m_fHPMax;
}

long msw::CChrCoreBoss::GetVsBossNowHP() const
{
    return m_fHP;
}

float msw::CChrCoreBoss::GetVsBossHPRate() const
{
    if (m_fHPMax > 0.0f && m_fHP > 0.0f) {
        return m_fHP / m_fHPMax;
    }
    return 0.0f;
}

long msw::CChrCoreBoss::GetBossTargetMarkerNum() const
{
    return IsBossTargetMarkerActive() != 0;
}

int msw::CChrCoreBoss::IsBossTargetMarkerActive() const
{
    return (m_nBossFlag >> 2) & 1;
}

// The target marker's place when nothing moved it: under the boss.
void msw::CChrCoreBoss::GetBossTargetMarkerDefaultPos(long, bcs::FVEC3 &rPos, int)
{
    if (!IsBossTargetMarkerActive()) {
        GetPos();
        rPos.Set(0.0f, 0.0f, 0.0f);
        const CNpcBase *pBoss = SearchBoss();
        if (pBoss) {
            rPos = pBoss->GetPos();
            rPos.y = 0.0f;
        }
    }
}

void msw::CChrCoreBoss::GetBossTargetMarkerDefaultPos(long, const bcs::FVEC3 &, bcs::FVEC3 &rPos, int)
{
    rPos.Set(0.0f, 0.0f, 0.0f);
    const CNpcBase *pBoss = SearchBoss();
    if (pBoss) {
        rPos = pBoss->GetPos();
        rPos.y = 0.0f;
    }
}

long msw::CChrCoreBoss::GetBossTargetMarkerPos(bcs::FVEC3 &, long)
{
    return 0;
}

long msw::CChrCoreBoss::GetBossTargetMarkerMoved(bcs::FVEC3 &)
{
    return 0;
}

// Where the ball is hit in a spike of kind nType (0 normal, 1 charged), the
// character facing the boss, and the frame it is hit at; 0 for other kinds.
int msw::CChrCoreBoss::GetBossBallShotCollPos(long nType, bcs::FVEC3 &rPos, float &rFrame) const
{
    bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
    long nMot;
    float fDefault;
    switch (nType) {
        case 0:
            ofs = m_SpikeOfs;
            nMot = 0x60;
            fDefault = 7.0f;
            break;
        case 1:
            ofs = m_SpikeOfsCharge;
            nMot = 0x67;
            fDefault = 7.0f;
            break;
        default:
            return 0;
    }
    bcs::FVEC3 rot;
    bcs::FVEC3 target(0.0f, 0.0f, 0.0f);
    const CNpcBase *pBoss = SearchBoss();
    if (pBoss) {
        target = pBoss->GetPos();
        target.y = 0.0f;
    }
    bcs::BCS_Vec_RotY(rot, ofs, GetRotY_ToPosXZ(target));
    bcs::FVEC3Add(&rPos, &GetPos(), &rot);
    rFrame = GetBallShotTrgFrame_(nMot, fDefault);
    return 1;
}

void msw::CChrCoreBoss::GetVolleySpikeOffsetPos(long nType, bcs::FVEC3 &rPos) const
{
    rPos.Set(0.0f, 0.0f, 0.0f);
    if (nType == 1) {
        rPos = m_SpikeOfsCharge;
    } else {
        rPos = m_SpikeOfs;
    }
}

float msw::CChrCoreBoss::GetVolleySpikeAdsorbRangeXZ(int bCharge) const
{
    if (bCharge) {
        return GetCcParam_()->m_Unk114;
    }
    return GetCcParam_()->m_Unk98;
}

float msw::CChrCoreBoss::GetVolleySpikeAdsorbRangeY(int bCharge) const
{
    if (bCharge) {
        return GetCcParam_()->m_Unk118;
    }
    return GetCcParam_()->m_Unk9C;
}

int msw::CChrCoreBoss::MoveBossTargetMarkerPos(float)
{
    return 0;
}

int msw::CChrCoreBoss::CanbeMoveBossTargetMarkerPos()
{
    return 0;
}

void msw::CChrCoreBoss::SetBossTargetMarkerPosClip(long, bcs::FVEC3 &)
{
}

void msw::CChrCoreBoss::SetTargetMarkerEnable_(long, const bcs::FVEC3 &, long)
{
}

void msw::CChrCoreBoss::SetTargetMarkerDisable_()
{
    m_nBossFlag &= ~4;
}

// The second half of the update: the look, the hit counts, the attack and
// defence collisions, and picking up coins and treading on ? panels.
void msw::CChrCoreBoss::UpdateFunc_()
{
    UpdateLook_();
    UpdateBodyHitCont_();
    RecalcAllMatrix();
    UpdateCollAtk_();
    if (m_pCollDef) {
        m_pCollDef->SetDisableAll();
    }
    if (!IsDisable() && !(m_UnkEC & 0x20) && m_pCollBody && CGimmickMan::IsActive()) {
        bcs::FVEC3 high;
        bcs::FVEC3 low;
        low = m_pCollBody->GetColl(3)->GetPos();
        high = m_pCollBody->GetColl(3)->GetPos2();
        CGimmickMan::DropCoinPickup(low, m_pCollBody->GetRadius(3), high.y - low.y, CallbackDropCoinPickup, this);
        CheckQPanel_();
    }
}

// Treading on a ? panel gives the team an item (when it has none) or shows
// that the panel was empty.
void msw::CChrCoreBoss::CheckQPanel_()
{
    if (IsDisable()) {
        return;
    }
    if (m_nStatusFlag & 0x10000) {
        return;
    }
    if (m_UnkEC & 0x20) {
        return;
    }
    if (m_nStatusFlag & 0x400) {
        return;
    }
    bcs::FVEC3 pos;
    GetTransNodePos(pos);
    if (pos.y >= 0.4f) {
        return;
    }
    pos.y = 0.0f;
    long nItem;
    if (CGimmickMan::QPanelCheck(pos, GetTeamNo(), m_nStatusFlag & 1, IsPadCtrl(), (m_nStatusFlag >> 17) & 1, &nItem,
                                 0)) {
        DriverSendMessage((CChrDrv::MSG_ID)0x10, nItem, 0, 0, 0);
        if (nItem >= 0) {
            if (CGame::GetTeamItem(GetTeamNo(), 0) < 0) {
                CreateStockItem(nItem);
            }
        } else if (nItem == -1) {
            bcs::FVEC3 ofs(0.0f, 0.75f, 0.0f);
            CCockpitBoss::CreateMiniTelop(9, GetPlayerNo(), &ofs);
        }
    }
}


// Adds to the team's arts gauge.
void msw::CChrCoreBoss::AddArtsGauge_(float fRate, float f)
{
    CGame::AddTeamArtsGauge(GetTeamNo(), fRate, f);
}

void msw::CChrCoreBoss::Util_AddCoin(long nCoin)
{
    CChrCore::Util_AddCoin(nCoin);
}

// The motions of standing, running and walking, with or without the ball
// and by condition; negative for the mirrored motion.
int msw::CChrCoreBoss::Util_GetIdleMotID()
{
    int nMot = 1;
    if (m_nStatusFlag & 1) {
        nMot = 0x13;
    }
    switch (m_nComLevel) {
        case 4:
            nMot = 0x81;
            break;
        case 6:
            nMot = 0x85;
            break;
        case 7:
            nMot = 0x86;
            break;
        case 8:
            nMot = 0x83;
            break;
    }
    if (m_nStatusFlag & 0x20) {
        nMot = -nMot;
    }
    return nMot;
}

int msw::CChrCoreBoss::Util_GetRunMotID()
{
    int nMot = 3;
    if (m_nStatusFlag & 1) {
        nMot = 0x15;
    }
    IsBossServeMode();
    switch (m_nComLevel) {
        case 4:
            nMot = 0x81;
            break;
        case 5:
            nMot = 0x84;
            break;
        case 6:
            nMot = 0x85;
            break;
        case 7:
            nMot = 0x86;
            break;
        case 8:
            nMot = 0x83;
            break;
        case 9:
            nMot = 0x87;
            break;
    }
    return nMot;
}

int msw::CChrCoreBoss::Util_GetWalkMotID()
{
    int nMot = 2;
    if (m_nStatusFlag & 1) {
        nMot = 0x14;
    }
    IsBossServeMode();
    switch (m_nComLevel) {
        case 4:
            nMot = 0x81;
            break;
        case 5:
            nMot = 0x84;
            break;
        case 6:
            nMot = 0x85;
            break;
        case 7:
            nMot = 0x86;
            break;
        case 8:
            nMot = 0x83;
            break;
        case 9:
            nMot = 0x87;
            break;
    }
    return nMot;
}

int msw::CChrCoreBoss::Util_GetLandMotID(long nMot)
{
    if (nMot < 0) {
        return 0x28;
    }
    return nMot;
}

int msw::CChrCoreBoss::Util_GetJumpUpMotID(int)
{
    return 0x25;
}

int msw::CChrCoreBoss::Util_GetJumpLandMotID(int bLand)
{
    return Util_GetLandMotID(bLand ? 0x28 : -1);
}

// A tap of the dribble: the held ball bounces.
int msw::CChrCoreBoss::CallbackDribbleTap_()
{
    CBall *pBall = GetAttachBall();
    if (!pBall) {
        return 0;
    }
    pBall->PlayBoundSe();
    return 1;
}

// The catch collision touched pBall: in the states that hold the ball
// (neutral, run, ..., and the receive when no catch point is set, which is
// then marked as touched) the ball is taken in by the next update, unless
// it may not be caught yet.
void msw::CChrCoreBoss::CallbackBallCatch_(CBall *pBall)
{
    if (!pBall) {
        return;
    }
    if (m_UnkEC & 0x24000000) {
        return;
    }
    if (m_UnkEC & 0x0A000000) {
        return;
    }
    if (!IsBossRightBallTouch(pBall->GetBallNo())) {
        return;
    }
    if (m_nStatusFlag & 0x8000) {
        return;
    }
    switch (FsmqGetState()) {
        case STATE_RECEIVE:
            if (m_nBallCatchMode == 0 || m_fBallCatchRange <= 0.0f) {
                STATE_WORK_RECEIVE *pWork = FsmqGetWork<STATE_WORK_RECEIVE>();
                if (!(pWork->m_nFlag |= 0x10000)) {
                    break;
                }
            }
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
            if (pBall->GetNoCatchCount() > 0.0f) {
                break;
            }
            SetHookBallCatch(pBall);
            break;
    }
}

void msw::CChrCoreBoss::CallbackBallCatch2(CCollBodyUnit *, CBall *pBall, void *pArg)
{
    if (pArg) {
        static_cast<CChrCoreBoss *>(pArg)->CallbackBallCatch2_(pBall);
    }
}

void msw::CChrCoreBoss::CallbackBallCatch3(CCollBodyUnit *, CBall *pBall, void *pArg)
{
    if (pArg) {
        static_cast<CChrCoreBoss *>(pArg)->CallbackBallCatch3_(pBall);
    }
}

int msw::CChrCoreBoss::CallbackDefReflectHit_(const CCollDefUnit *, const CBall *, bcs::FVEC3 &, bcs::FVEC3 &)
{
    return 0;
}

void msw::CChrCoreBoss::CallbackMotTrg_BodyVisible_(long, long)
{
}

// The ball the character goes for: the only one, or of several the one
// coming down nearest to it that it may touch.
msw::CBall *msw::CChrCoreBoss::GetTargetBall_() const
{
    long nBall = CBallMan::GetBallNum();
    if (nBall <= 0) {
        return 0;
    }
    CBall *pTarget = CBallMan::GetBallPtr(0);
    if (nBall <= 1) {
        return pTarget;
    }
    float fMin = 9999980000.0f;
    for (long i = 0; i < nBall; i++) {
        CBall *pBall = CBallMan::GetBallPtr(i);
        if (pBall && pBall->IsShootMode() && pBall->CanebVolleyTouch(GetPlayerNo())) {
            bcs::FVEC3 pos(pBall->GetTargetPos());
            float fDist = GetDist_ToPosXZ(pos);
            if (fDist < fMin) {
                fMin = fDist;
                pTarget = pBall;
            }
        }
    }
    return pTarget;
}

const msw::CNpcBase *msw::CChrCoreBoss::Util_GetBoss_() const
{
    return SearchBoss();
}

// The yaw to the boss (on the ground), whose position goes to pPos.
float msw::CChrCoreBoss::GetSpikeTargetPosRot_(bcs::FVEC3 *pPos) const
{
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
    const CNpcBase *pBoss = SearchBoss();
    if (pBoss) {
        pos = pBoss->GetPos();
        pos.y = 0.0f;
    }
    float fRot = GetRotY_ToPosXZ(pos);
    if (pPos) {
        *pPos = pos;
    }
    return fRot;
}

// The yaw to the boss's face (at a height of 3), whose position goes to
// pPos.
float msw::CChrCoreBoss::GetDunkTargetPosRot_(bcs::FVEC3 *pPos) const
{
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
    const CNpcBase *pBoss = SearchBoss();
    if (pBoss) {
        long nNode = pBoss->GetNodeID("eff_face");
        pBoss->GetNodePos(nNode > 0 ? nNode : 0, pos);
        pos.y = 3.0f;
    }
    float fRot = GetRotY_ToPosXZ(pos);
    if (pPos) {
        *pPos = pos;
    }
    return fRot;
}

void msw::CChrCoreBoss::CalcRotParam_(ROT_LERP1 &rLerp, float fRotY, float fFrame)
{
    rLerp.m_afUnk[0] = WrapRad(fRotY);
    rLerp.m_afUnk[2] = fFrame;
    rLerp.m_afUnk[2] = Max(rLerp.m_afUnk[2], 1.0f);
    float fDiff = WrapRad(rLerp.m_afUnk[0] - GetRotY());
    if (fDiff > 3.1415927f) {
        fDiff -= 6.2831855f;
    }
    rLerp.m_afUnk[3] = 0.0f;
    rLerp.m_afUnk[1] = fDiff / rLerp.m_afUnk[2];
}

// Where the ball can be caught (see m_nBallCatchMode).
void msw::CChrCoreBoss::SetBallCatchDisable_()
{
    m_nBallCatchMode = 0;
    m_nBallCatchShape = 7;
    m_fBallCatchRange = 0.0f;
    m_fBallCatchHeight = 0.0f;
    m_fBallCatchRange2 = 0.0f;
    m_BallCatchPos0.Set(0.0f, 0.0f, 0.0f);
    m_BallCatchPos1.Set(0.0f, 0.0f, 0.0f);
    m_nBallCatchNode = m_nEffNodeDunk;
    m_fBallCatchUnk7CC = 0.3f;
}

void msw::CChrCoreBoss::SetBallCatchEnableVNode_(float fRange, long nNode)
{
    m_nBallCatchMode = 1;
    m_nBallCatchShape = 0;
    m_fBallCatchRange = fRange;
    m_fBallCatchHeight = 0.0f;
    m_fBallCatchRange2 = 0.0f;
    m_BallCatchPos0.Set(0.0f, 0.0f, 0.0f);
    m_BallCatchPos1.Set(0.0f, 0.0f, 0.0f);
    m_nBallCatchNode = GetVNode(nNode);
    m_fBallCatchUnk7CC = 0.2f;
}

void msw::CChrCoreBoss::SetBallCatchEnableVNode_(float fRange, const bcs::FVEC3 &rPos, long nNode)
{
    m_nBallCatchMode = 1;
    m_nBallCatchShape = 0;
    m_fBallCatchRange = fRange;
    m_fBallCatchHeight = 0.0f;
    m_fBallCatchRange2 = 0.0f;
    m_BallCatchPos0 = rPos;
    m_BallCatchPos1 = rPos;
    m_nBallCatchNode = GetVNode(nNode);
    m_fBallCatchUnk7CC = 0.2f;
}

void msw::CChrCoreBoss::SetBallCatchEnableVNode_(float fRange, float fHeight, const bcs::FVEC3 &rPos, long nNode)
{
    m_nBallCatchMode = 1;
    m_nBallCatchShape = 1;
    m_fBallCatchRange = fRange;
    m_fBallCatchHeight = fHeight;
    m_fBallCatchRange2 = fRange;
    m_BallCatchPos0 = rPos;
    m_BallCatchPos1 = rPos;
    m_nBallCatchNode = GetVNode(nNode);
    m_fBallCatchUnk7CC = 0.2f;
}

void msw::CChrCoreBoss::SetBallCatchEnableRoot_(float fRange, float fHeight, const bcs::FVEC3 &rPos)
{
    m_nBallCatchMode = 1;
    m_nBallCatchShape = 1;
    m_fBallCatchRange = fRange;
    m_fBallCatchHeight = fHeight;
    m_fBallCatchRange2 = fRange;
    m_BallCatchPos0 = rPos;
    m_BallCatchPos1 = rPos;
    m_nBallCatchNode = 0;
    m_fBallCatchUnk7CC = 0.2f;
}

void msw::CChrCoreBoss::SetBallCatchEnableWorld_(float fRange, const bcs::FVEC3 &rPos)
{
    m_nBallCatchMode = 2;
    m_nBallCatchShape = 0;
    m_fBallCatchRange = fRange;
    m_fBallCatchHeight = 0.0f;
    m_fBallCatchRange2 = 0.0f;
    m_BallCatchPos0 = rPos;
    m_BallCatchPos1 = rPos;
    m_nBallCatchNode = 0;
    m_fBallCatchUnk7CC = 0.2f;
}

void msw::CChrCoreBoss::SetBallCatchEnableWorld_(float fRange, const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
{
    m_nBallCatchMode = 2;
    m_nBallCatchShape = 3;
    m_fBallCatchRange = fRange;
    m_fBallCatchHeight = 0.0f;
    m_fBallCatchRange2 = 0.0f;
    m_BallCatchPos0 = rPos1;
    m_BallCatchPos1 = rPos2;
    m_nBallCatchNode = 0;
    m_fBallCatchUnk7CC = 0.2f;
}

void msw::CChrCoreBoss::SetBallCatchEnableWorld_(float fRange, float fHeight, const bcs::FVEC3 &rPos)
{
    m_nBallCatchMode = 2;
    m_nBallCatchShape = 1;
    m_fBallCatchRange = fRange;
    m_fBallCatchHeight = fHeight;
    m_fBallCatchRange2 = fRange;
    m_BallCatchPos0 = rPos;
    m_BallCatchPos1 = rPos;
    m_nBallCatchNode = 0;
    m_fBallCatchUnk7CC = 0.2f;
}

void msw::CChrCoreBoss::SetServeMode_(int bServe)
{
    if (bServe) {
        m_nBossFlag |= 1;
    } else {
        m_nBossFlag &= ~3;
    }
}

void msw::CChrCoreBoss::ClearMoveParamWork_(CC_DATA::CHR_PARAM::MOVE_PARAM &rParam)
{
    rParam.m_nType = 2;
    rParam.m_fFrame = 0.0f;
    rParam.m_fFrameMax = 0.0f;
    rParam.m_fDist = 0.0f;
}

// Effect 0 is the boss game's own (effect 0x56B at the root, on the
// ground); the others are CChrCore's.
void msw::CChrCoreBoss::Util_SetEffect(SET_EFFECT_TYPE nType)
{
    if (m_pMdl) {
        if (nType == 0) {
            bcs::FVEC3 pos;
            GetTransNodePos(pos);
            pos.y = 0.0f;
            CEffectCtrl::SimpleEffect(0x56B, pos, 3, 0, 0);
        } else {
            CChrCore::Util_SetEffect(nType);
        }
    }
}

// Effects 0, 3 and 4 are not attached in the boss game.
void msw::CChrCoreBoss::Util_AttachEffect(ATTACH_EFFECT_TYPE nType, long n)
{
    if (m_pMdl) {
        if (nType == 3 || nType == 4) {
            return;
        }
        if (nType == 0) {
            return;
        }
        CChrCore::Util_AttachEffect(nType, n);
    }
}

int msw::CChrCoreBoss::IsChargeBall_(const CBall *pBall) const
{
    if (pBall && pBall->GetVolleyChargePower() >= 1.0f) {
        return 1;
    }
    return 0;
}
