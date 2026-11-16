// msw::CChrCoreBoss: the character core of the boss game. Its constructor
// and destructor, the model and collision set-up, the per-frame update, the
// ball catch and pass helpers and the boss-game virtuals. The vtable is
// emitted here, so the file ends with the weak copies of CChrCore's and
// CChrCoreBoss's inline virtuals. The characters' own classes are files of
// their own (game/boss/CChrCoreBoss<Name>.cpp).
#include "game/msw/CChrCoreBoss.hpp"
#include "game/msw/CChrCoreInline.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC2.hpp"
#include "bcs/CAnmBlendMotion.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CWorldEnv.hpp"
#include "game/msw/BallBossSpikeShotArg.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CCollBodyUnit.hpp"
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

#include <math.h>

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

    // (guessed helper) x kept in [lo, hi].
    inline float Clamp(float x, float lo, float hi)
    {
        return x > hi ? hi : (x < lo ? lo : x);
    }

    // (guessed helper) From a to b by t.
    inline float Interp(float a, float b, float t)
    {
        return t * (b - a) + a;
    }

    // (guessed helper) From a to b by t, t kept in [0, 1].
    inline float Lerp(float a, float b, float t)
    {
        return Interp(a, b, Clamp(t, 0.0f, 1.0f));
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
        case 9: {
            // (guess) The original picks a value per character here and
            // never uses it: its compare chain stays, the stores do not, and
            // 1.3, 1.4, 0.9 and 0.8 sit in .rodata ahead of the move speed
            // rates. Which of them the last two characters take is not
            // known. Behaviour-free (notes/decisions.md, dead comparisons).
            float fScale;
            switch (GetChrIdS()) {
                case 6:
                    fScale = 1.3f;
                    break;
                case 7:
                    fScale = 1.4f;
                    break;
                case 8:
                    fScale = 0.9f;
                    break;
                case 10:
                    fScale = 0.8f;
                    break;
                case 11:
                    fScale = 1.3f;
                    break;
                case 12:
                    fScale = 1.4f;
                    break;
                default:
                    fScale = 1.0f;
                    break;
            }
            (void)fScale;
            break;
        }
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
    // (The rate goes through a local: written straight into fSpeed the body is
    // small enough for CalcMoveSpeedRate_ to inline it, which the original
    // does not.)
    float fRate;
    switch (m_nComLevel) {
        case 1:
            fRate = 1.7f;
            fSpeed *= fRate;
            break;
        case 4:
            fRate = 1.3f;
            fSpeed *= fRate;
            break;
        case 6:
            fRate = 0.5f;
            fSpeed *= fRate;
            break;
        case 7:
            fRate = 0.6f;
            fSpeed *= fRate;
            break;
        case 8:
            fRate = 0.6f;
            fSpeed *= fRate;
            break;
        case 10:
            fRate = 0.7f;
            fSpeed *= fRate;
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

inline void msw::CChrCoreBoss::TakeLooseBall_(CBall *pBall)
{
    Eff_CreateAttach(10, 0x567, m_nEffNodeArts, 0, 0, 3, 0);
    AttachBall(pBall->GetBallNo(), 5, 0.0f, 0);
    PlaySePos(0xE0A, pBall->GetPos(), 0, -1, 0.0f, 1.0f, 0.0f);
    SetServeMode_(1);
    m_nStatusFlag |= 1;
    FsmqChangeState(STATE_NEUTRAL);
    CGame::SetLooseBallModeFlag(0);
    DriverSendMessage((CChrDrv::MSG_ID)0xD, pBall->GetBallNo(), GetPlayerNo(), 0, 0);
    CChrMan::AllSendMsg((CChrDrv::MSG_ID)0xE, pBall->GetBallNo(), GetPlayerNo(), 0, 0);
    CChrMan::AllSendMsg((CChrDrv::MSG_ID)0x14, 0, 0, 0, 0);
}

inline void msw::CChrCoreBoss::ReceiveAtPose_(CBall *pBall, long nPose)
{
    STATE_WORK_RECEIVE *pWork = SetStateReceive_(pBall, 0, nPose);
    float fFrame = pBall->GetTouchDownFrame(m_afReceiveHeight[nPose]);
    pBall->GetPosAfterFrame(pWork->m_BallPos, fFrame);
    pWork->m_nFlag |= 0x200;
    pWork->m_fUnk38 = fFrame;
    if (fFrame < 5.0f) {
        fFrame = 5.0f;
    }
    BallCatch_(pBall, fFrame, -1, 0);
}

inline void msw::CChrCoreBoss::ReceiveNow_(CBall *pBall)
{
    long nPose = GetReceivePoseDir_(GetReceiveBaseDir_());
    STATE_WORK_RECEIVE *pWork = SetStateReceive_(pBall, 0, nPose);
    pBall->GetPosAfterFrame(pWork->m_BallPos, pBall->GetTouchDownFrame(m_afReceiveHeight[nPose] + GetPosY()));
    pWork->m_nFlag |= 0x600;
    BallCatch_(pBall, 5.0f, -1, 0);
}

// What the character does with a ball that reached its catch collision, by
// its state: in the neutral stance, running, side-stepping or turning it
// picks a loose ball up or receives it (in the pose it stands in, or the
// one the ball's direction asks for); in a receive, the receive goes on
// with the ball; a spike or an air trick takes it. A ball it may not touch
// is missed, and knocks the character down when it was not its to take.
void msw::CChrCoreBoss::HookBallCatchCmn_(CBall *pBall)
{
    long nState = FsmqGetState();
    int bTouchable = pBall->CanebVolleyTouch(GetPlayerNo());
    int bTouch = 0;
    if (IsBossRightBallTouch(pBall->GetBallNo()) && bTouchable) {
        bTouch = 1;
    }

    switch (nState) {
        case STATE_NEUTRAL:
            if (bTouch && CanbeReceiveCondition()) {
                STATE_WORK_NEUTRAL *pWork = FsmqGetWork<STATE_WORK_NEUTRAL>();
                long nType = pWork->m_nType;
                long nPose = pWork->m_nPose;
                if (pBall->CanbeVolleyPickup(GetPlayerNo()) && !pBall->IsPassMode()) {
                    TakeLooseBall_(pBall);
                } else {
                    switch (nType) {
                        case 2:
                        case 3:
                            ReceiveAtPose_(pBall, nPose);
                            break;
                        case 1: {
                            STATE_WORK_RECEIVE *pReceive = SetStateReceive_(pBall, 2, nPose);
                            bcs::FVEC3 pos;
                            GetVNodePos(4, pos);
                            float fFrame = pBall->GetTouchDownFrame(pos.y);
                            pReceive->m_fUnk38 = fFrame;
                            if (fFrame < 5.0f) {
                                fFrame = 5.0f;
                            }
                            BallCatch_(pBall, fFrame, -1, 0);
                            break;
                        }
                        default:
                            ReceiveAtPose_(pBall, GetReceivePoseDir_(GetReceiveBaseDir_()));
                            break;
                    }
                }
            } else {
                Util_MissBall_(pBall, 3.0f, 8.0f);
                if (!bTouchable) {
                    MethdDamage(0, WrapRad(GetRotY_FromPosXZ(pBall->GetPos())));
                }
                if (!CanbeReceiveCondition()) {
                    MethdDamage(1, WrapRad(3.1415927f + GetRotY()));
                }
            }
            break;
        case STATE_RUN:
        case STATE_SIDE_STEP:
        case STATE_TURN:
            if (bTouch && CanbeReceiveCondition()) {
                if (pBall->CanbeVolleyPickup(GetPlayerNo()) && !pBall->IsPassMode()) {
                    TakeLooseBall_(pBall);
                } else {
                    ReceiveNow_(pBall);
                }
            } else {
                Util_MissBall_(pBall, 3.0f, 8.0f);
                if (!bTouchable) {
                    MethdDamage(0, WrapRad(GetRotY_FromPosXZ(pBall->GetPos())));
                }
                if (!CanbeReceiveCondition()) {
                    MethdDamage(1, WrapRad(3.1415927f + GetRotY()));
                }
            }
            break;
        case STATE_RECEIVE:
            if (bTouch) {
                STATE_WORK_RECEIVE *pWork = FsmqGetWork<STATE_WORK_RECEIVE>();
                pWork->m_Dir = pBall->GetVelocity();
                switch (pWork->m_nKind) {
                    case 1:
                        if (!(pWork->m_nFlag & 1)) {
                            pWork->m_nFlag |= 1;
                            if (pBall->IsArtsShootMode()) {
                                pWork->m_nFlag |= 0x100;
                            }
                            BallCatch_(pBall, 5.0f, -1, 0);
                        }
                        break;
                    case 2:
                        if (pWork->m_nFlag & 0x10000) {
                            ReceiveNow_(pBall);
                        } else if (!(pWork->m_nFlag & 1)) {
                            pWork->m_nFlag |= 1;
                            BallCatch_(pBall, 5.0f, -1, 0);
                        }
                        break;
                    case 3: {
                        int bAgain = 0;
                        if (pWork->m_nStep <= 4 && !(m_nStatusFlag & 0x400)) {
                            bAgain = 1;
                        }
                        if (bAgain) {
                            ReceiveNow_(pBall);
                        } else if (!(pWork->m_nFlag & 1)) {
                            pWork->m_nFlag |= 1;
                            BallCatch_(pBall, 5.0f, -1, 0);
                        }
                        break;
                    }
                    case 4:
                        ReceiveNow_(pBall);
                        break;
                    default:
                        if (pWork->m_nFlag & 0x4000) {
                            ReceiveNow_(pBall);
                        } else {
                            SetReceiveEndureWork_(pBall, pWork);
                            if (!(pWork->m_nFlag & 1)) {
                                pWork->m_nFlag |= 1;
                                BallCatch_(pBall, 5.0f, -1, 0);
                            }
                        }
                        break;
                }
            } else {
                Util_MissBall_(pBall, 3.0f, 8.0f);
                if (!bTouchable) {
                    MethdDamage(0, WrapRad(GetRotY_FromPosXZ(pBall->GetPos())));
                }
            }
            break;
        case STATE_JUMP:
            break;
        case STATE_DAMAGE:
            Util_MissBall_(pBall, 3.0f, 8.0f);
            break;
        case STATE_SPIKE:
            BallCatch_(pBall, 10.0f, -1, 0);
            break;
        case STATE_AIR_TRICK:
            BallCatch_(pBall, 2.0f, -1, 0);
            break;
        default:
            Util_MissBall_(pBall, 3.0f, 8.0f);
            MethdDamage(4, GetRotY());
            break;
    }
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

namespace {

    // The square of the distance between two points seen from above.
    inline float DistXZSq(const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 p1(r1.x, 0.0f, r1.z);
        bcs::FVEC3 p2(r2.x, 0.0f, r2.z);
        return bcs::FVEC3DistSq(&p1, &p2);
    }


}

namespace {

    // (inline in the original; name guessed) Of two candidates on the same
    // side of pSelf (in x) the farther one, of two on either side the
    // nearer one.
    inline msw::CChrCore *SelectPassToChr(msw::CChrCore *pSelf, msw::CChrCore *pBest, msw::CChrCore *pChr)
    {
        float fDx = pSelf->GetPos().x - pChr->GetPos().x;
        float fBestDx = pSelf->GetPos().x - pBest->GetPos().x;
        if ((fDx <= 0.0f && fBestDx <= 0.0f) || (fDx >= 0.0f && fBestDx >= 0.0f)) {
            float fBest = DistXZSq(pSelf->GetPos(), pBest->GetPos());
            if (DistXZSq(pSelf->GetPos(), pChr->GetPos()) > fBest) {
                pBest = pChr;
            }
        } else {
            float fBest = DistXZSq(pSelf->GetPos(), pBest->GetPos());
            if (DistXZSq(pSelf->GetPos(), pChr->GetPos()) < fBest) {
                pBest = pChr;
            }
        }
        return pBest;
    }

}

// The player number the ball is passed to. With up to four characters, the
// one two numbers on. Otherwise, with bReceive: a team mate in the spike or
// air trick before its swing (when nobody of the team stands in the front
// zone), else anybody of the team, by SelectPassToChr_ while somebody stands
// in the front zone ... by the smaller |z| when not; without bReceive the
// farthest team mate who can take a pass.
long msw::CChrCoreBoss::SearchPassToChrIdx_(int bReceive)
{
    long nTo = GetPlayerNo();
    long nNum = CChrMan::GetChrCoreNum();
    if (nNum <= 4) {
        nTo += 2;
        nTo %= nNum;
    } else if (bReceive) {
        CChrCore *pBest = 0;
        int bFront = 1;
        for (long i = GetPlayerNo() & 1; i < nNum; i += 2) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && !pChr->IsDisable() && pChr->GetPlayerNo() != GetPlayerNo() && pChr->GetUnk17B()) {
                bFront = 0;
                break;
            }
        }
        if (bFront) {
            for (long i = GetPlayerNo() & 1; i < nNum; i += 2) {
                // (a team mate in a spike or an air trick before the swing)
                int bOK = 0;
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr && !pChr->IsDisable() && pChr->GetPlayerNo() != GetPlayerNo()) {
                    switch (pChr->FsmqGetState()) {
                        case STATE_SPIKE:
                            if (!pChr->IsVolleySpikeSwingEnd()) {
                                bOK = 1;
                            }
                            break;
                        case STATE_AIR_TRICK:
                            if (!pChr->IsVolleyAirTrickSwingEnd()) {
                                bOK = 1;
                            }
                            break;
                    }
                }
                if (bOK) {
                    if (!pBest) {
                        pBest = pChr;
                    } else {
                        pBest = SelectPassToChr(this, pBest, pChr);
                    }
                }
            }
            if (!pBest) {
                for (long i = GetPlayerNo() & 1; i < nNum; i += 2) {
                    CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr && !pChr->IsDisable() && pChr->GetPlayerNo() != GetPlayerNo()) {
                        if (!pBest) {
                            pBest = pChr;
                        } else {
                            pBest = SelectPassToChr(this, pBest, pChr);
                        }
                    }
                }
            }
        } else {
            for (long i = GetPlayerNo() & 1; i < nNum; i += 2) {
                // (a team mate in a spike or an air trick before the swing)
                int bOK = 0;
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr && !pChr->IsDisable() && pChr->GetPlayerNo() != GetPlayerNo()) {
                    switch (pChr->FsmqGetState()) {
                        case STATE_SPIKE:
                            if (!pChr->IsVolleySpikeSwingEnd()) {
                                bOK = 1;
                            }
                            break;
                        case STATE_AIR_TRICK:
                            if (!pChr->IsVolleyAirTrickSwingEnd()) {
                                bOK = 1;
                            }
                            break;
                    }
                }
                if (bOK) {
                    if (!pBest) {
                        pBest = pChr;
                    } else if (fabsf(pChr->GetPos().z) < fabsf(pBest->GetPos().z)) {
                        pBest = pChr;
                    }
                }
            }
            if (!pBest) {
                for (long i = GetPlayerNo() & 1; i < nNum; i += 2) {
                    CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr && !pChr->IsDisable() && pChr->GetPlayerNo() != GetPlayerNo()) {
                        if (!pBest) {
                            pBest = pChr;
                        } else if (fabsf(pChr->GetPos().z) < fabsf(pBest->GetPos().z)) {
                            pBest = pChr;
                        }
                    }
                }
            }
        }
        if (pBest) {
            nTo = pBest->GetPlayerNo();
        }
    } else {
        float fMax = 0.0f;
        for (long i = GetPlayerNo() & 1; i < nNum; i += 2) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && !pChr->IsDisable() && pChr->GetPlayerNo() != GetPlayerNo() &&
                !(pChr->GetStatusFlag() & 0x80000)) {
                float fDist = GetDist_ToChrXZSq(pChr);
                if (fDist > fMax) {
                    fMax = fDist;
                    nTo = pChr->GetPlayerNo();
                }
            }
        }
    }
    return nTo;
}

namespace CChrCoreBoss_cpp {

    // uncalled stand-in: the original file holds the weak copy of
    // CStageMan::GetTeamGroundRect here, after SearchPassToChrIdx_, although
    // none of its functions calls it; whatever did was stripped. A virtual
    // call makes this compiler emit the copy. Nothing calls this function
    // and the module link strips it (notes/decisions.md, "Uncalled stand-in
    // functions").
    void UnusedTeamGroundRect(const msw::CStageMan *pStage)
    {
        pStage->GetTeamGroundRect(0, 0, 0, 0, 0);
    }

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

// The direction pBall is knocked away in: between the direction from the
// character to the ball and the ball's own course, mostly the latter.
// Without a ball, the reverse of the character's yaw.
float msw::CChrCoreBoss::GetFlickDir_(const CBall *pBall)
{
    if (!pBall) {
        return WrapRad(-1.0f * GetRotY());
    }
    float fRot = GetRotY_FromPosXZ(pBall->GetPos());
    float fDir = fRot;
    bcs::FVEC3 vel = pBall->GetVelocity();
    if (fabsf(vel.x) > 0.0f || fabsf(vel.z) > 0.0f) {
        fDir = GetRotYXZ(vel);
    }
    float fDiff = WrapRadPi(fDir - fRot);
    float fRate = 0.85f;
    return WrapRad(fRot + Lerp(0.0f, fDiff, fRate));
}

// The character misses pBall. What is left of it here: the receiver is
// looked up and a point fMin to fMax (at random) from the ball worked out,
// but neither is used. (The original's code that sent the ball on is
// gone.)
void msw::CChrCoreBoss::Util_MissBall_(CBall *pBall, float fMin, float fMax)
{
    if (!pBall) {
        return;
    }
    if (fMin > fMax) {
        float f = fMin;
        fMin = fMax;
        fMax = f;
    }
    if (fMin < fMax) {
        fMin = bcs::BCS_RandRange(fMin, fMax);
    }
    bcs::FVEC3 pos;
    GetBossTargetMarkerPos(pos, 0);
    m_nBossFlag &= ~7;
    CChrMan::GetChrCore(SearchPassToChrIdx_(0));
    pos = pBall->GetPos();
    int bSent = 1;
    bcs::FVEC3 ofs;
    bcs::BCS_VecZ_RotY(ofs, fMin, WrapRad(bcs::BCS_RandMax(6.2831855f)));
    bcs::FVEC3Add(&pos, &pos, &ofs);
    // (The original keeps this test of a flag it has just set, so the
    // effect is never made.)
    if (!bSent) {
        Eff_CreateSimple(-1, pBall->GetPos(), GetRotY(), 3, 0, 0);
    }
}

// Spikes pBall (nType 2: a super spike, 5: ... with fFrame; nCourse 1 or
// 2 a course the driver chose; bItem: the stocked item goes with it) at
// the boss target marker, or else at the boss, then hands the attack to
// the other team.
void msw::CChrCoreBoss::Util_SpikeBall_(CBall *pBall, long nCourse, long nType, float fFrame, int bItem)
{
    if (!pBall) {
        return;
    }
    const CHR_CORE_BOSS::CC_PARAM_BOSS *pParam = GetCcParam_();
    BALL::BOSS::SPIKE_SHOT_ARG arg;
    arg.m_nUnk0C = nType;
    arg.m_nUnk2C = m_nPlayerNo;
    arg.m_nUnk30 = m_nTeamNo;
    arg.m_nUnk34 = GetTargetCourtIdx();
    arg.m_fUnk40 = GetRotY();
    arg.m_nUnk54 = PlayGetOutline_(1);
    if (arg.m_nUnk14) {
        arg.m_fUnk58 = pParam->m_UnkEC;
        arg.m_fUnk5C = pParam->m_UnkF0;
        arg.m_fUnk60 = pParam->m_UnkF4;
        arg.m_fUnk64 = pParam->m_UnkF8;
        arg.m_fUnk6C = pParam->m_UnkFC;
        arg.m_fUnk70 = pParam->m_Unk100;
        arg.m_fUnk74 = pParam->m_Unk104;
        arg.m_fUnk78 = pParam->m_Unk108;
    } else {
        arg.m_fUnk58 = pParam->m_UnkB8;
        arg.m_fUnk5C = pParam->m_UnkBC;
        arg.m_fUnk60 = pParam->m_UnkC0;
        arg.m_fUnk64 = pParam->m_UnkC4;
        arg.m_fUnk6C = pParam->m_UnkC8;
        arg.m_fUnk70 = pParam->m_UnkCC;
        arg.m_fUnk74 = pParam->m_UnkD0;
        arg.m_fUnk78 = pParam->m_UnkD4;
    }
    bcs::FVEC3 pos;
    if (!GetBossTargetMarkerPos(pos, 0)) {
        pos = GetPos();
        // (the original reads the position once more without using it)
        GetPos();
        pos.Set(0.0f, 0.0f, 0.0f);
        const CNpcBase *pBoss = SearchBoss();
        if (pBoss) {
            pos = pBoss->GetPos();
            pos.y = 0.0f;
        }
    }
    m_nBossFlag &= ~7;
    GetTargetCourtIdx();
    arg.m_vUnk00 = pos;
    if (nType == 2) {
        arg.m_nUnk20 = 1;
        arg.m_nUnk44 = 0xC85;
        arg.m_fUnk58 = pParam->m_UnkD8;
        arg.m_fUnk5C = pParam->m_UnkDC;
        arg.m_fUnk60 = pParam->m_UnkE0;
        arg.m_fUnk64 = pParam->m_UnkE4;
        arg.m_fUnk6C = pParam->m_UnkE8;
        arg.m_fUnk70 = 0.0f;
        arg.m_fUnk74 = 0.0f;
        arg.m_fUnk78 = 0.0f;
    } else {
        arg.m_nUnk44 = 0xC85;
    }
    if (nType == 5) {
        arg.m_nUnk4C = 0x56E;
        PlayVoice(0x35, 0);
    } else {
        arg.m_nUnk4C = 0x575;
        PlayVoice(0x33, 0);
    }
    if (GetCcParam_()->m_Unk00 == 2) {
        arg.m_nUnk10 = 1;
    }
    if (nType == 5) {
        arg.m_fUnk38 = fFrame;
        DriverSendMessage((CChrDrv::MSG_ID)0x53, 4, 1, 0, 0);
    } else {
        switch (nCourse) {
            case 1:
                arg.m_nUnk18 = 1;
                break;
            case 2:
                arg.m_nUnk18 = 2;
                break;
        }
        if (bItem) {
            arg.m_nUnk3C = GetStockItemType();
        }
        if (m_nStatusFlag & 0x20000) {
            arg.m_nUnk3C = 4;
        }
        int nResult = pBall->SetVsBossSpikeShotMode(arg);
        if (bItem && (nResult & 8)) {
            SetStockItem(0);
        }
        long nPower = 1;
        if (nResult & 1) {
            nPower = 7;
        }
        if (nResult & 0x10) {
            nPower = 7;
        } else if (nResult & 2) {
            if (bcs::CWorldEnv::GetViewSplitNum() <= 1 && IsPadCtrl()) {
                float fLineFrame = 30.0f;
                CGame::StartSaturatedLinework(Max(1.0f, fLineFrame));
            }
            CGame::StartCameraQuake(8, m_nPlayerNo, -1);
        }
        DriverSendMessage((CChrDrv::MSG_ID)0x53, nPower, 1, 0, 0);
    }
    long nTeam = m_nTeamNo;
    nTeam++;
    nTeam %= CGame::g_GameWork.m_nTeamNum;
    CGame::SetOffenseTeam(nTeam);
    CChrMan::AllSendMsg((CChrDrv::MSG_ID)0x17, nTeam, 0, 0, 0);
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

namespace {

    // (guessed helper) The yaw from pChr towards the boss's position on the
    // ground (the origin when there is no boss).
    inline float GetRotY_ToBossXZ(const msw::CChrCore *pChr)
    {
        bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
        const msw::CNpcBase *pBoss = SearchBoss();
        if (pBoss) {
            pos = pBoss->GetPos();
            pos.y = 0.0f;
        }
        return pChr->GetRotY_ToPosXZ(pos);
    }

}

// Where a pass to player nPlayerNo goes (rPos), and its kind: 2 into the
// spike of a character that is jumping (at the height its spike takes the
// ball, the charged one when bCharge and the spike is a super spike), 1 to
// the hands of a character (nHeight >= 1; moved towards the middle of the
// court, or ahead of a running character), 0 to its feet.
long msw::CChrCoreBoss::Util_SetPassBallTargetPos_(bcs::FVEC3 &rPos, long nPlayerNo, long nHeight, int bCharge)
{
    CChrCore *pTo = CChrMan::GetChrCore(nPlayerNo);
    rPos = pTo->GetPos();
    long nState = pTo->FsmqGetState();
    long nKind = 0;
    if (nState == STATE_SPIKE) {
        nKind = 2;
        if (!(pTo->FsmqGetWork<STATE_WORK_SPIKE>()->m_nFlag & 0x40)) {
            bCharge = 0;
        }
    }

    bcs::FVEC3 ofs;
    float fHeight = 3.0f;
    pTo->GetVolleySpikeOffsetPos(0, ofs);
    if (bCharge) {
        fHeight = 3.2f;
        pTo->GetVolleySpikeOffsetPos(1, ofs);
    }

    if (nKind == 2) {
        rPos.y = 0.25f * pTo->GetVolleySpikeAdsorbRangeY(bCharge) + fHeight;
        bcs::FVEC3 vec;
        bcs::BCS_Vec_RotY(vec, ofs, GetRotY_ToBossXZ(this));
        bcs::FVEC3Add(&rPos, &rPos, &vec);
    } else if (nHeight >= 1) {
        rPos.y = fHeight;
        bcs::FVEC3 vec;
        bcs::BCS_Vec_RotY(vec, ofs, GetRotY_ToBossXZ(this));
        bcs::FVEC3Add(&rPos, &rPos, &vec);
        if (nState != STATE_SPIKE && nState != STATE_AIR_TRICK) {
            int bRun = 0;
            if (nState == STATE_RUN) {
                bRun = 1;
            }
            if (!bRun) {
                // Pulled towards the middle of the court by a quarter of
                // how much farther than 4 from it the receiver stands.
                bcs::FVEC3 pos(pTo->GetPos());
                float fZ = fabsf(pos.z);
                bcs::FVEC3 court = CStageMan::GetInstance()->GetTeamCourtOffset(GetGuardCourtIdx());
                float fOver = fZ - fabsf(court.z);
                if (fOver >= 4.0f) {
                    fOver -= 4.0f;
                    fOver *= 0.25f;
                    if (pos.z < 0.0f) {
                        rPos.z += fOver;
                    } else {
                        rPos.z -= fOver;
                    }
                }
            } else {
                // Ahead of the runner, short of the walls.
                bcs::BCS_VecZ_RotY(vec, 2.5f, pTo->GetRotY());
                rPos.x += vec.x;
                rPos.z += vec.z;
                bcs::FVEC3 from(pTo->GetPos());
                bcs::FVEC3 to(rPos);
                bcs::FVEC3 hit;
                bcs::FVEC3 normal;
                float fTime;
                if (CCollMan::DetectMapHit(hit, normal, fTime, from, to, 0.8f, 1,
                                           (GetGuardCourtIdx() & 0xFF) | 0x82000000)) {
                    rPos.x = hit.x;
                    rPos.z = hit.z;
                }
            }
        }
        nKind = 1;
    } else {
        nKind = 0;
        rPos.y = 0.0f;
    }
    return nKind;
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

// The move of the frame: the body radius the map is checked with grows
// back slowly (0.05 a frame at most) to the one of the moment (the scale
// while bit 0x8000 of the status is set, none in the air trick, smaller in
// the serve mode) and shrinks at once; then the floor flow and forced
// moves, and the model is kept out of the walls.
void msw::CChrCoreBoss::UpdatePos_()
{
    float fRadius;
    if (m_nStatusFlag & 0x8000) {
        fRadius = Clamp(m_fScaleBase - 1.0f, 0.0f, 2.0f);
    } else if (FsmqGetState() == STATE_AIR_TRICK) {
        fRadius = 0.0f;
    } else if (IsBossServeMode()) {
        fRadius = 0.4f;
    } else {
        fRadius = 1.1f;
    }
    if (fRadius < m_fUnk26C) {
        m_fUnk26C = fRadius;
    } else if (fRadius > m_fUnk26C) {
        float fStep = Min(fRadius - m_fUnk26C, 0.05f);
        fStep *= m_fTimeScale * m_fFrameStep;
        m_fUnk26C += fStep;
        m_fUnk26C = Min(m_fUnk26C, fRadius);
    }
    FloorFlowMove_();
    ForceMove_();
    bcs::FVEC3 hit;
    bcs::FVEC3 normal;
    bcs::FVEC3 pos = m_pMdl->m_vTrans;
    float fTime;
    m_nMapHitFlag = (unsigned char)GetGuardCourtIdx() | 0x02000000;
    if (CCollMan::DetectMapHit(hit, normal, fTime, pos, pos, m_fUnk26C, 1, m_nMapHitFlag)) {
        m_pMdl->m_vTrans = hit;
    }
}

// Places the body collisions on the character: the push collision (0) at
// its offset from the model's root, turned by the yaw; the body, catch and
// damage capsules (1, 2, 3, 6) above it; the catch of the boss receive (6,
// m_fBallCatchUnk7CC wide); the catch of a ball the character may take (5,
// in the neutral stance, running, turning or some receives; widened by the
// ball's catchable rate); and the catch set by SetBallCatchEnable* (4).
void msw::CChrCoreBoss::UpdateCollBody_()
{
    if (m_pCollBody) {
        float fRotY = GetRotY();
        m_pCollBody->SetEnableAll();
        bcs::FVEC3 root;
        GetTransNodePos(root);
        CalcBodyCollInter_(m_fFrameStep * m_fTimeScale);

        bcs::FVEC3 pos(m_PushOfsInter.m_fNowX, 0.0f, m_PushOfsInter.m_fNowZ);
        if (0.0f != fRotY) {
            bcs::BCS_Vec_RotY(pos, pos, WrapRad(fRotY));
        }
        bcs::FVEC3Add(&pos, &pos, &root);
        m_pCollBody->SetPos(0, pos);
        m_pCollBody->SetSize(0, m_PushSizeInter.m_fNowX * m_fScaleBase, 1.8f * m_fScaleBase,
                             m_PushSizeInter.m_fNowZ * m_fScaleBase);
        m_pCollBody->SetYRot(0, fRotY);
        m_pCollBody->SetPushRank(0, m_nPushRank);
        m_pCollBody->SetPushPower(0, m_nPushPower);

        bcs::FVEC3 low(pos);
        bcs::FVEC3 high(pos);
        low.y += 0.25f * m_fScaleBase;
        high.y += 0.75f * m_fScaleBase;
        m_pCollBody->SetPos(2, low, high);
        m_pCollBody->SetPos(3, low, high);
        m_pCollBody->SetPos(6, low, high);
        m_pCollBody->SetPos(1, low, high);

        // Above the normal size the radii grow by 0.7 of the scale only.
        float fScale = m_fScaleBase;
        if (fScale > 1.0f) {
            fScale = 0.7f * (fScale - 1.0f) + 1.0f;
        }
        m_pCollBody->SetRadius(2, 0.5f * fScale);
        m_pCollBody->SetRadius(3, 0.5f * fScale);
        m_pCollBody->SetRadius(1, 0.4f * fScale);

        CCollUnit *pColl = m_pCollBody->GetColl(6);
        if (m_fBallCatchUnk7CC <= 0.0f) {
            pColl->OffFlag10(1);
        } else {
            pColl->SetSize(m_fBallCatchUnk7CC * fScale, 0.0f, 0.0f);
        }

        CBall *pBall = GetTargetBall_();
        pColl = m_pCollBody->GetColl(5);
        if (pColl) {
            int bCatch = 0;
            bcs::FVEC3 size(2.0f, 1.1f, 1.0f);
            float fRate = 0.0f;
            if (pBall && IsBossRightBallTouch(pBall->GetBallNo())) {
                fRate = pBall->GetVolleyCatchableRate();
                switch (FsmqGetState()) {
                    case STATE_NEUTRAL:
                        bCatch = 1;
                        size.y = 0.9f;
                        break;
                    case STATE_RUN:
                    case STATE_TURN:
                        bCatch = 1;
                        break;
                    case STATE_RECEIVE: {
                        size.y = 0.9f;
                        STATE_WORK_RECEIVE *pWork = FsmqGetWork<STATE_WORK_RECEIVE>();
                        switch (pWork->m_nKind) {
                            case 0:
                                bCatch = 1;
                                if (m_nBallCatchMode == 0 || m_fBallCatchRange <= 0.0f) {
                                    bCatch = 0;
                                }
                                break;
                            case 2:
                                bCatch = 0;
                                if (pWork->m_nFlag & 0x10000) {
                                    bCatch = 1;
                                }
                                break;
                        }
                        break;
                    }
                }
            }
            if (bCatch && fRate > 0.0f) {
                float fScaleXZ = m_fScaleBase * fRate;
                size.x *= fScaleXZ;
                size.y *= m_fScaleBase;
                size.z *= fScaleXZ;
                pColl->SetSize(size);
                pColl->SetPos(pos);
            } else {
                pColl->OffFlag10(1);
            }
        }

        pColl = m_pCollBody->GetColl(4);
        if (pColl) {
            float fRate = 1.0f;
            if (pBall && IsBossRightBallTouch(pBall->GetBallNo())) {
                fRate = pBall->GetVolleyCatchableRate();
            }
            switch (m_nBallCatchMode) {
                case 1: {
                    // Placed from the character (or one of its nodes),
                    // turned by its yaw.
                    bcs::FVEC3 base;
                    if (m_nBallCatchNode > 0) {
                        GetNodePos(m_nBallCatchNode, base);
                    } else {
                        base = GetPos();
                    }
                    bcs::FVEC3 catchPos;
                    bcs::BCS_Vec_RotY(catchPos, m_BallCatchPos0, GetRotY());
                    bcs::FVEC3Add(&catchPos, &catchPos, &base);
                    pColl->SetCollType(m_nBallCatchShape);
                    pColl->SetPos(catchPos);
                    if (m_nBallCatchShape == 3) {
                        bcs::BCS_Vec_RotY(catchPos, m_BallCatchPos1, GetRotY());
                        bcs::FVEC3Add(&catchPos, &catchPos, &base);
                        pColl->SetPos2(catchPos);
                    }
                    bcs::FVEC3 size(m_fBallCatchRange * fRate, m_fBallCatchHeight, m_fBallCatchRange2 * fRate);
                    pColl->SetSize(size);
                    break;
                }
                case 2: {
                    // Placed in the world.
                    pColl->SetCollType(m_nBallCatchShape);
                    pColl->SetPos(m_BallCatchPos0);
                    if (m_nBallCatchShape == 3) {
                        pColl->SetPos2(m_BallCatchPos1);
                    }
                    bcs::FVEC3 size(m_fBallCatchRange * fRate, m_fBallCatchHeight, m_fBallCatchRange2 * fRate);
                    pColl->SetSize(size);
                    break;
                }
                default:
                    pColl->SetCollType(0);
                    pColl->OffFlag10(1);
                    break;
            }
        }

        if (m_nStatusFlag & 0x40000000) {
            m_pCollBody->SetDisable(2);
        } else {
            m_pCollBody->SetEnable(2);
        }
        if (m_UnkEC & 4) {
            m_pCollBody->SetPushDisable(0);
            m_pCollBody->SetPushDisable(1);
        } else {
            m_pCollBody->SetPushEnable(0);
            m_pCollBody->SetPushEnable(1);
        }
    }
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

// Where the character looks: at the target ball while it waits or runs
// (at the boss, the spike's target, when the serve is its own or the ball
// is out of play or held), while it spikes (not in a super spike) and while
// it receives (some kinds only); otherwise straight ahead.
void msw::CChrCoreBoss::UpdateLook_()
{
    if (CBallMan::GetActiveBallNum() <= 0) {
        return;
    }
    CBall *pBall = GetTargetBall_();
    if (!pBall) {
        return;
    }
    switch (FsmqGetState()) {
        case STATE_RUN:
        case STATE_TURN:
        case STATE_NEUTRAL:
            if (!CanbeReceiveCondition()) {
                ClearLook();
                return;
            }
            if ((m_nStatusFlag & 1) || !pBall->IsActive() || pBall->IsAttach()) {
                bcs::FVEC3 pos;
                GetSpikeTargetPosRot_(&pos);
                UpdateLookAt_(pos, 0.125f, 0);
                return;
            }
            UpdateLookAt_(pBall->GetPos(), 0.125f, 0);
            return;
        case STATE_SPIKE:
            if (FsmqGetWork<STATE_WORK_SPIKE>()->m_nFlag & 0x40) {
                ClearLook();
                return;
            }
            UpdateLookAt_(pBall->GetPos(), 0.125f, 0);
            return;
        case STATE_RECEIVE: {
            int bLook = 0;
            STATE_WORK_RECEIVE *pWork = FsmqGetWork<STATE_WORK_RECEIVE>();
            if ((pWork->m_nKind >= 3 && pWork->m_nKind <= 4) ||
                ((pWork->m_nKind == 0 || pWork->m_nKind == 2) && !(pWork->m_nFlag & 0x800))) {
                bLook = 1;
            }
            if (bLook) {
                UpdateLookAt_(pBall->GetPos(), 0.125f, 0);
                return;
            }
        }
        default:
            ClearLook();
            break;
    }
}

// The attack collision of the frame: in the shoot stance, while its motion
// is in the hit window, unit 0 is a capsule at the character growing from
// the parameters' smallest to largest radius across the window. The body
// units 0 and 1 lose their attack (bits 3 and 8) every frame.
void msw::CChrCoreBoss::UpdateCollAtk_()
{
    if (m_pCollAtk) {
        m_pCollAtk->SetDisableAll();
    }
    if (FsmqGetState() == STATE_SHOOT_STANCE) {
        STATE_WORK_SHOOT_STANCE *pWork = FsmqGetWork<STATE_WORK_SHOOT_STANCE>();
        CCollUnit *pUnit = m_pCollAtk->GetColl(0);
        if (pUnit && pWork->m_bHit) {
            pUnit->SetCollType(1);
            pUnit->SetAtkType(0xC);
            pUnit->SetShape(0x20);
            pUnit->SetCapsule(GetPos(), GetPos());
            float fRadius = Interp(GetCcParam_()->m_Unk224, GetCcParam_()->m_Unk228, pWork->m_fHitRate);
            pUnit->SetSize(bcs::FVEC3(fRadius, 1.0f, fRadius));
            pUnit->SetVanishPower(20.0f);
            pUnit->OnFlag10(1);
        }
    }
    CCollBodyUnit *pBody0 = CollBodyGetColl_(0);
    CCollBodyUnit *pBody1 = CollBodyGetColl_(1);
    pBody0->m_nBodyFlag &= ~0x108;
    pBody1->m_nBodyFlag &= ~0x108;
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
            bcs::FVEC3 ofs(0.0f, 1.8f, 0.0f);
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

// The leader of the team that serves first: takes ball nBall and stands in
// the serve pose (motion 1, first frame).
void msw::CChrCoreBoss::Util_SetOffenseLeaderFirstState(long nBall)
{
    CBallMan::SetBallPos(nBall, GetPosPtr());
    AttachBall(nBall, 5, 0.0f, 0);
    m_nStatusFlag |= 1;
    FsmqClear();
    FsmqChangeState(STATE_NEUTRAL);
    if (m_pMdl) {
        AnimPlay(3, 1, 0, 0.0f, 0.0f);
        m_pMdl->UpdateFrame(0.0f);
        Util_NodePatchTransCopy();
        RecalcAllMatrix();
        AnimStop(3);
    }
    SetServeMode_(1);
    ResetCollBodyPush_(0.5f, 0.5f, 0.0f, 0.0f);
    CBall *pBall = GetAttachBall();
    if (pBall) {
        pBall->SetShotChrIdx(m_nPlayerNo);
        pBall->SetPassChrIdx(m_nPlayerNo);
        pBall->SetShotTeamIdx(m_nTeamNo);
    }
}

// The leader of the other team: waits in the idle pose.
void msw::CChrCoreBoss::Util_SetDefenseLeaderFirstState()
{
    FsmqClear();
    FsmqChangeState(STATE_NEUTRAL);
    if (m_pMdl) {
        AnimPlay(3, Util_GetIdleMotID(), 0, 0.0f, 0.0f);
        m_pMdl->UpdateFrame(0.0f);
        Util_NodePatchTransCopy();
        RecalcAllMatrix();
        AnimStop(3);
    }
    SetServeMode_(0);
    ResetCollBodyPush_(0.5f, 0.5f, 0.0f, 0.0f);
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

// Catch collision 4 touched pBall: when the character may catch it, the
// ball bounces off it (more softly while bit 0x8000 of the status is set)
// and no longer hits it.
void msw::CChrCoreBoss::CallbackBallCatch2_(CBall *pBall)
{
    if (!pBall) {
        return;
    }
    int nCatch = CallbackBallCatch2__(pBall);
    if (!(nCatch & 1)) {
        return;
    }
    if (!pBall->IsAttach() && !pBall->IsInvincible() && (pBall->GetNoCatchCount() <= 0.0f || (nCatch & 2)) &&
        !pBall->IsFlag4()) {
        float fRate = 0.0f;
        pBall->SetNoHitChr(m_nPlayerNo);
        if (m_nStatusFlag & 0x8000) {
            fRate = 0.25f;
        }
        pBall->SetChrReflectMode(m_nPlayerNo, fRate);
    }
}

// Whether catch collision 4 may catch pBall: bit 0 it bounces off, bit 1
// even while it may not be caught yet. In the catching states the ball is
// taken in instead (0).
int msw::CChrCoreBoss::CallbackBallCatch2__(CBall *pBall)
{
    if (m_nStatusFlag & 0x02000000) {
        return 0;
    }
    if (m_UnkEC & 0x20000000) {
        return 1;
    }
    if (!IsBossRightBallTouch(pBall->GetBallNo())) {
        return 0;
    }
    if (m_nStatusFlag & 0x8000) {
        return 3;
    }
    switch (FsmqGetState()) {
        case STATE_JUMP_UPPER:
            return 1;
        case STATE_PASS:
        case STATE_AERIAL_PASS:
        case STATE_JUMP_SHOOT:
        case STATE_AERIAL_SHOOT:
            return 0;
        case STATE_DAMAGE:
            return 0;
        default:
            if (pBall->GetNoCatchCount() > 0.0f) {
                return 0;
            }
        case STATE_PASS_CATCH:
        case STATE_BALL_CATCH:
            SetHookBallCatch(pBall);
            return 0;
    }
}

void msw::CChrCoreBoss::CallbackBallCatch3(CCollBodyUnit *, CBall *pBall, void *pArg)
{
    if (pArg) {
        static_cast<CChrCoreBoss *>(pArg)->CallbackBallCatch3_(pBall);
    }
}

// Catch collision 6 touched pBall: when the character may catch it, the
// ball bounces off it if it is already marked (bit 0) or the team has
// touched it three times (and is then marked); otherwise it is missed.
void msw::CChrCoreBoss::CallbackBallCatch3_(CBall *pBall)
{
    if (!pBall) {
        return;
    }
    int nCatch = CallbackBallCatch3__(pBall);
    if ((nCatch & 1) && !pBall->IsAttach() && !pBall->IsInvincible() &&
        (pBall->GetNoCatchCount() <= 0.0f || (nCatch & 2)) && !pBall->IsFlag4() &&
        !pBall->IsChrBit178(m_nPlayerNo)) {
        float fRate = 0.0f;
        if (m_nStatusFlag & 0x8000) {
            fRate = 0.25f;
        }
        if (pBall->IsFlag1()) {
            pBall->SetChrReflectMode(m_nPlayerNo, fRate);
            return;
        }
        if (pBall->GetVolleyTouchNum(m_nTeamNo) >= 3) {
            pBall->SetChrReflectMode(m_nPlayerNo, fRate);
            pBall->OnFlag(1);
            return;
        }
        Util_MissBall_(pBall, 3.0f, 5.0f);
    }
}

// Whether catch collision 6 may catch pBall (as CallbackBallCatch2__). A
// spike that has hit the ball is hurt by it instead (1).
int msw::CChrCoreBoss::CallbackBallCatch3__(CBall *pBall)
{
    if (m_nStatusFlag & 0x02000000) {
        return 0;
    }
    if (m_UnkEC & 0x20000000) {
        return 1;
    }
    if (!IsBossRightBallTouch(pBall->GetBallNo())) {
        return 0;
    }
    if (m_nStatusFlag & 0x8000) {
        return 3;
    }
    switch (FsmqGetState()) {
        case STATE_SERVE:
        case STATE_JUMP:
            return 0;
        case STATE_DAMAGE:
            return 0;
        case STATE_SPIKE: {
            STATE_WORK_SPIKE *pWork = FsmqGetWork<STATE_WORK_SPIKE>();
            if (pWork->m_nFlag & 1) {
                if (!pBall->IsChrBit178(m_nPlayerNo)) {
                    if (!(m_UnkEC & 0x8000) && !IsInvincible()) {
                        float fRotY = GetRotY_FromPosXZ(pBall->GetPos());
                        unsigned long nFlag = 0x20000;
                        long nState = FsmqGetState();
                        if (nState == STATE_SPIKE || nState == STATE_AIR_TRICK) {
                            nFlag = 0x12;
                        }
                        SetHookDamage(nFlag, fRotY, 0.0f, 0.0f, -1, 0.0f, 0, 0, -1, -1);
                    }
                    return 1;
                }
                return 0;
            }
            break;
        }
        default:
            if (pBall->GetNoCatchCount() > 0.0f) {
                return 0;
            }
            break;
    }
    SetHookBallCatch(pBall);
    return 0;
}

// An attack hit a body owned by something of kind 3 or 5 during the shoot stance: a stance
// about to shoot misses its shot; the hit effect and message 0x53.
void msw::CChrCoreBoss::CallbackAttackHit_(const CCollAtkUnit *, const CCollBodyUnit *pBody,
                                           const bcs::FVEC3 &rPos, unsigned long)
{
    if (pBody) {
        long nKind = pBody->m_nOwnerKind;
        if (nKind == 5 || nKind == 3) {
            if (FsmqGetState() == STATE_SHOOT_STANCE) {
                STATE_WORK_SHOOT_STANCE *pWork = FsmqGetWork<STATE_WORK_SHOOT_STANCE>();
                if (pWork->m_nStep == 3) {
                    pWork->m_bNoHit = 1;
                    pWork->m_bHit = 0;
                }
                Eff_CreateSimple(0xC, rPos, 3, 0, 0);
                DriverSendMessage((CChrDrv::MSG_ID)0x53, 6, 1, 0, 0);
            }
        }
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
    }
    pos.y = 3.0f;
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
    rLerp.m_afUnk[1] = fDiff / rLerp.m_afUnk[2];
    rLerp.m_afUnk[3] = 0.0f;
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
