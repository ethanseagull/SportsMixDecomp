// msw::CChrCoreBoss's shoot stance, and the stick it raises
// (msw::CHR_CORE_BOSS::CStickObj): the stick fades in when the stance
// starts, swings up when the shot is made and fades out after it.
#include "game/msw/CChrCoreBoss.hpp"
#include "game/msw/CHR_CORE_BOSS/CStickObj.hpp"

#include "bcs/CRenderMan.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_PARAM_BOSS.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"

#include <math.h>

using msw::CHR_CORE_BOSS::CStickObj;

// The shoot stance's work in the state queue.
struct msw::CChrCoreBoss::STATE_WORK_SHOOT_STANCE {
    // 0x00: 0 raise the stick, 1 hold, 2 hold (looping motion), 3 shot,
    // 4 put down, 5 over
    long m_nStep;
    unsigned char m_Unk04[0x4];
    long m_nShot;      // 0x08: 2 once the driver asks for the shot
    unsigned char m_Unk0C[0x4];
    int m_bNoHit;      // 0x10: no hit window when set (never set here)
    int m_bHit;        // 0x14: set while the motion is in the hit window
    float m_fHitRate;  // 0x18: 0 to 1 across the hit window
    int m_bCanCancel;  // 0x1C: the stance may be left from now on
};

namespace {
    // The shoot stance's state number in the character's state queue.
    const long STATE_SHOOT_STANCE = 0x45;

    inline float Clamp01(float f)
    {
        return f > 1.0f ? 1.0f : (f < 0.0f ? 0.0f : f);
    }
}

void msw::CChrCoreBoss::StickObjDestroy_(float fFrame)
{
    // (guessed) Behaviour-free; see notes/decisions.md, "Dead float
    // comparison". The original's .rodata has 1.0f before 0.0f although
    // 0.0f is read first; this makes the 1.0f constant first.
    if (fFrame == 1.0f) {
    }
    if (m_hStickObj != 0) {
        CStickObj *pStick = static_cast<CStickObj *>(bcs::CTaskMan::GetTask(m_hStickObj));
        m_hStickObj = 0;
        if (pStick != 0 && !pStick->IsDelete()) {
            if (fFrame <= 0.0f) {
                pStick->Delete();
            } else {
                pStick->Disappear(fFrame);
            }
        }
    }
}

void msw::CChrCoreBoss::CtorStateShootStance()
{
    STATE_WORK_SHOOT_STANCE *pWork = FsmqGetWork<STATE_WORK_SHOOT_STANCE>();

    m_UnkEC |= 0x10;
    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    SetCollBodyPushRank_(1);
    SetMoveMotType(2);
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(2), 8.0f);
    m_fJumpUnk220 = 0.0f;
    pWork->m_nShot = 0;
    pWork->m_bNoHit = 0;
    pWork->m_bHit = 0;
    pWork->m_bCanCancel = 0;
    pWork->m_nStep = 0;

    long nEff = -1;
    switch (m_nChrId) {
        case 0x11:
            nEff = -1;
            break;
        case 0x10:
            nEff = 0x79;
            break;
    }
    if (nEff >= 0) {
        Eff_CreateAttachAuto(0x12, nEff, 3);
    }
}

void msw::CChrCoreBoss::LoopStateShootStance()
{
    STATE_WORK_SHOOT_STANCE *pWork = FsmqGetWork<STATE_WORK_SHOOT_STANCE>();
    int bEnd = 0;
    int bTurn = 0;

    switch (pWork->m_nStep) {
        case 0: {
            AnimPlay(3, 0x6B, 0, 8.0f, 0.0f);
            PlaySePos(0xE0F, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            float fFrameNum = AnimGetFrameNum(3);
            float fFrame = fFrameNum > 16.0f ? 16.0f : fFrameNum;
            if (m_hStickObj == 0) {
                nw4r::g3d::ResMdl mdl = m_ResFile.GetResMdl("bstick");
                long nNode = m_pMdl->GetNodeID("stick");
                nNode = nNode > 0 ? nNode : 0;
                CStickObj *pStick = new (0x100) CStickObj(CGame::GetTempParent(), mdl, m_pMdl, nNode, GetChrIdS());
                if (pStick != 0) {
                    m_hStickObj = pStick->GetId();
                    pStick->Appear(fFrame);
                }
            }
            if (DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x1A), 0, 0, 0, 0) >= 2) {
                pWork->m_nShot = 2;
            }
            bTurn = 1;
            pWork->m_nStep++;
            break;
        }
        case 1:
            if (IsAnimEnd(3)) {
                if (pWork->m_nShot >= 2) {
                    // static-order swap: the original's .rodata holds the
                    // four statics of this function in reverse order
                    // (uFlg@2, nEffID@1, uFlg@0, nEffID); same names and
                    // values (notes/decisions.md).
                    static const long nEffID = 0x4E7;
                    static const unsigned long uFlg = 8;
                    Eff_CreateAttach(0xB, nEffID, m_nEffNode, 0, uFlg, 3, 0);
                    Eff_SetRotateY(0xB, GetRotY());
                    AnimPlay(3, 0x6D, 0, 2.0f, 0.0f);
                    PlaySePos(0xE10, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                    if (m_hStickObj != 0) {
                        CStickObj *pStick = static_cast<CStickObj *>(bcs::CTaskMan::GetTask(m_hStickObj));
                        if (pStick != 0 && !pStick->IsDelete()) {
                            pStick->Shoot(2.2f, 8.0f, 8.0f, 16.0f);
                        }
                    }
                    pWork->m_nStep = 3;
                    break;
                }
                AnimPlay(3, 0x6C, 1, 0.0f, 0.0f);
                pWork->m_nStep++;
            } else if (pWork->m_nShot == 0 &&
                       DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x1A), 0, 0, 0, 0) >= 2) {
                pWork->m_nShot = 2;
            }
            bTurn = 1;
            break;
        case 2: {
            long nMsg = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x1A), 0, 0, 0, 0);
            if (nMsg == 0) {
                AnimPlay(3, 0x6E, 0, 8.0f, 0.0f);
                StickObjDestroy_(8.0f);
                pWork->m_nStep = 4;
            }
            if (nMsg >= 2) {
                static const long nEffID = 0x4E7;
                static const unsigned long uFlg = 8;
                Eff_CreateAttach(0xB, nEffID, m_nEffNode, 0, uFlg, 3, 0);
                Eff_SetRotateY(0xB, GetRotY());
                AnimPlay(3, 0x6D, 0, 8.0f, 0.0f);
                PlaySePos(0xE10, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                if (m_hStickObj != 0) {
                    CStickObj *pStick = static_cast<CStickObj *>(bcs::CTaskMan::GetTask(m_hStickObj));
                    if (pStick != 0 && !pStick->IsDelete()) {
                        pStick->Shoot(2.2f, 8.0f, 8.0f, 16.0f);
                    }
                }
                pWork->m_nStep = 3;
            }
            bTurn = 1;
            break;
        }
        case 3: {
            pWork->m_bHit = 0;
            pWork->m_fHitRate = 0.0f;
            float fFrame = AnimGetFrame(3);
            float fStart = GetCcParam_()->m_Unk21C;
            float fEnd = GetCcParam_()->m_Unk220;
            if (fEnd < fStart) {
                float fTmp = fStart;
                fStart = fEnd;
                fEnd = fTmp;
            }
            if (fFrame >= fStart && fFrame <= fEnd) {
                if (pWork->m_bNoHit == 0) {
                    pWork->m_bHit = 1;
                }
                if (fEnd > fStart) {
                    pWork->m_fHitRate = Clamp01((fFrame - fStart) / (fEnd - fStart));
                } else {
                    pWork->m_fHitRate = 1.0f;
                }
            }
            float fCancel = GetCcParam_()->m_Unk22C;
            if (fCancel > 0.0f && fFrame >= fCancel) {
                pWork->m_bCanCancel = 1;
            }
            if (IsAnimEnd(3)) {
                pWork->m_bHit = 0;
                pWork->m_bCanCancel = 1;
                bEnd = 1;
                pWork->m_nStep = 5;
            }
            break;
        }
        case 4:
            if (IsAnimEnd(3)) {
                pWork->m_nStep = 5;
                bEnd = 1;
            }
            break;
        default:
            bEnd = 1;
            break;
    }

    if (bEnd) {
        FsmqNext();
        FsmqRetry();
    } else if (bTurn) {
        bcs::FVEC3 pos;
        GetSpikeTargetPosRot_(&pos);
        RotateYToPosRate_(pos, 0.125f);
    }
}

void msw::CChrCoreBoss::DtorStateShootStance()
{
    Eff_Stop(0x12);
    Eff_StopForce(0xB);
    ArtsOptDestroy();
    StickObjDestroy_(0.0f);
}

int msw::CChrCoreBoss::SubStateShootStanceCanbeCancel_() const
{
    if (FsmqGetState() != STATE_SHOOT_STANCE) {
        return 0;
    }
    return FsmqGetWork<STATE_WORK_SHOOT_STANCE>()->m_bCanCancel;
}

CStickObj::CStickObj(bcs::CTask *pParent, nw4r::g3d::ResMdl mdl, bcs::CMdlObj *pAttach, long nNode, long nChrId)
    : CGameTask(pParent, -1)
    , m_pMdl(0)
    , m_nChrId(nChrId)
    , m_nScaleNode(-1)
    , m_fAlpha(0.0f)
    , m_fAppearSpeed(0.0625f)
    , m_fDisappearSpeed(0.0625f)
    , m_nShootStep(0)
    , m_fShootRate(0.0f)
    , m_fShootSpeed(0.0625f)
    , m_fShootScale(1.0f)
    , m_fShootWait(0.0f)
    , m_hEffect(0)
{
    InitFsm(this, STATE_NUM);
    SetState(STATE_SLEEP, &CStickObj::CtorStateSleep, &CStickObj::LoopStateSleep, &CStickObj::DtorStateSleep);
    SetState(STATE_APPEAR, &CStickObj::CtorStateAppear, &CStickObj::LoopStateAppear, &CStickObj::DtorStateAppear);
    SetState(STATE_SHOOT, &CStickObj::CtorStateShoot, &CStickObj::LoopStateShoot, &CStickObj::DtorStateShoot);
    SetState(STATE_DISAPPEAR, &CStickObj::CtorStateDisappear, &CStickObj::LoopStateDisappear,
             &CStickObj::DtorStateDisappear);
    ChangeState(STATE_SLEEP);

    m_Scale.x = 1.0f;
    m_Scale.y = 1.0f;
    m_Scale.z = 1.0f;

    if (mdl.IsValid()) {
        m_pMdl = new (0x100) bcs::CMdlObj;
        m_pMdl->Create(mdl, 0, 0);
        m_pMdl->OnFlag(0x100);
        m_pMdl->OnFlag(0x2);
        m_pMdl->AttachToObject(pAttach, nNode, bcs::CScnObj::ATTACHMODE_0);
        m_pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
        m_nScaleNode = m_pMdl->GetNodeID("stick_scal");
        bcs::CRenderMan::PushBack(m_pMdl, 2);
    }
}

CStickObj::~CStickObj()
{
    if (m_hEffect != 0) {
        CEffectCtrl::StopAttachEffect(m_hEffect);
        m_hEffect = 0;
    }
    if (m_pMdl != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
    }
    m_pMdl = 0;
}

void CStickObj::Main()
{
    ExecState();
    if (m_pMdl != 0) {
        m_pMdl->SetAlphaRateF(m_fAlpha);
        m_pMdl->UpdateFrame(GetFrameRateF());
        m_pMdl->CalcRootMtx();
        m_pMdl->CalcWorldMatrixAll(0);
    }
}

void CStickObj::CtorStateSleep()
{
    if (m_pMdl != 0) {
        m_pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
    }
    SetScale(1.0f);
    m_fAlpha = 0.0f;
}

void CStickObj::LoopStateSleep() {}

void CStickObj::DtorStateSleep() {}

void CStickObj::CtorStateAppear()
{
    if (m_pMdl != 0) {
        m_pMdl->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
        if (m_hEffect != 0) {
            CEffectCtrl::StopAttachEffect(m_hEffect);
            m_hEffect = 0;
        }
        m_hEffect = CEffectCtrl::CreateAttachEffectAuto(m_nChrId == 0x11 ? 0x77 : 0x75, m_pMdl, 3);
    }
    SetScale(1.0f);
}

void CStickObj::LoopStateAppear()
{
    m_fAlpha += m_fAppearSpeed;
    m_fAlpha = Clamp01(m_fAlpha);
}

void CStickObj::DtorStateAppear() {}

void CStickObj::CtorStateShoot()
{
    if (m_pMdl != 0) {
        m_pMdl->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
        if (m_hEffect != 0) {
            CEffectCtrl::StopAttachEffect(m_hEffect);
            m_hEffect = 0;
        }
        m_hEffect = CEffectCtrl::CreateAttachEffectAuto(m_nChrId == 0x11 ? 0x78 : 0x76, m_pMdl, 3);
    }
    m_Scale.x = 1.0f;
    m_Scale.y = 1.0f;
    m_Scale.z = 1.0f;
    m_fAlpha = 1.0f;
    m_nShootStep = 0;
}

void CStickObj::LoopStateShoot()
{
    if (m_nShootStep == 0) {
        m_fShootRate += m_fShootSpeed;
        m_fShootRate = Clamp01(m_fShootRate);
        float fRate = pow(m_fShootRate, 2.0);
        SetScale(fRate * (m_fShootScale - 1.0f) + 1.0f);
        if (m_fShootRate >= 1.0f) {
            m_nShootStep++;
        }
    } else {
        m_fShootWait -= GetFrameRateF();
        if (m_fShootWait <= 0.0f) {
            ChangeState(STATE_DISAPPEAR);
        }
    }
}

void CStickObj::DtorStateShoot() {}

void CStickObj::CtorStateDisappear()
{
    if (m_hEffect != 0) {
        CEffectCtrl::StopAttachEffect(m_hEffect);
        m_hEffect = 0;
    }
}

void CStickObj::LoopStateDisappear()
{
    m_fAlpha -= m_fDisappearSpeed;
    m_fAlpha = Clamp01(m_fAlpha);
    if (m_fAlpha <= 0.0f) {
        Delete();
    }
}

void CStickObj::DtorStateDisappear() {}
