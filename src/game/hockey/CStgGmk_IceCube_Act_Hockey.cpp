// The ice cubes of hockey party stage 85: their model
// (CStgGmk_IceCube_Mdl_Hockey), actor (CStgGmk_IceCube_Act_Hockey), gimmick
// (CStgGmk_IceCube_Hockey) and cluster (CStgGmk_IceCubeCluster_Hockey), all
// built on the pylons (msw::CStgGmk_Pylon and the hockey pylon's actor).
// The floor of the stage tilts; the cubes slide with it (SetAccel), are
// knocked along X or Z by attacks, and while fast they hurt what they hit.
// A cube pushed off the field falls over into the water and splashes.
#include "game/msw/CStgGmk_IceCube_Hockey.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/FMTX34.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgSrt.hpp"

namespace {

    // (stand-ins for operators) Reached through this one more inline level,
    // the float registers come out numbered as in the original (see
    // game/dodge/CStgGmk_Pylon_Act_Dodge.cpp).
    inline void VecSub(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3Sub(&rOut, &r1, &r2);
    }
    inline void VecNormalize(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::BCS_Vec_Normalize(rOut, rIn);
    }

    // (names guessed) The work memory of the actor's states, as in
    // game/msw/CStgGmk_Pylon.cpp: FALL takes the direction to fall in.
    struct STATE_WORK {
        long m_Unk00;
        float m_fArg; // 0x04
    };

    // Shows (false) or hides (true) a part of the model.
    inline void SetHidden(bcs::CScnObj *pObj, bool bHide)
    {
        if (bHide) {
            pObj->OnFlag(1);
        } else {
            pObj->OffFlag(1);
        }
    }
}

// Camera quakes (CGame::StartCameraQuake).
enum {
    QUAKE_HIT = 0x2F,
    QUAKE_LANDING = 0x38
};

CStgGmk_IceCube_Mdl_Hockey::~CStgGmk_IceCube_Mdl_Hockey()
{
}

void CStgGmk_IceCube_Mdl_Hockey::Create()
{
    static const MDL_PARAM mdl_param = { "s85_box_00", "s85_box_00", "s85_box_00", 0, 0, 0, 0, 0 };

    msw::CStgMdl::Create(CGenericGimmickCreator(msw::CLoadData::GetRes(0), &mdl_param));
}

// The shadow shows only above the floor, the reflection only above the
// water.
void CStgGmk_IceCube_Mdl_Hockey::Main(float rate)
{
    msw::CStgGmk_Pylon_Mdl::Main(rate);
    GetParent()->GetAct();
    bcs::CMdlObj *pMdl = GetMdlObj();
    bcs::CMdlShadow *pShadow = GetMdlShadow();
    bcs::CMdlRef *pRef = GetMdlRef();
    if (pMdl != 0) {
        bcs::FMTX34 mtx;
        pMdl->GetMtx(bcs::CScnObj::SCNOBJMTXTYPE_ROOT, mtx);
        if (pShadow != 0) {
            SetHidden(pShadow, mtx.m[1][3] < 0.0f);
        }
        if (pRef != 0) {
            SetHidden(pRef, mtx.m[1][3] < -1.0f);
        }
    }
}

float CStgGmk_IceCube_Mdl_Hockey::GetMdlWidth() const
{
    return 3.5f;
}

float CStgGmk_IceCube_Mdl_Hockey::GetMdlDepth() const
{
    return 3.5f;
}

float CStgGmk_IceCube_Mdl_Hockey::GetMdlHeight() const
{
    return 1.5f;
}

// The attack collision is one sphere, off until UpdateColl turns it on.
CStgGmk_IceCube_Act_Hockey::CStgGmk_IceCube_Act_Hockey(msw::CStgGmk *pParent)
    : msw::CStgGmk_Pylon_Act_Hockey(pParent)
{
    m_AtkColl.Create(1, 5, 0, 0);
    float radius = 0.9f * GetParam_Radius();
    m_AtkColl.SetCollType(0, 0);
    m_AtkColl.SetTargetFlag(0, 2);
    m_AtkColl.SetAtkType(0, 0x1101);
    m_AtkColl.SetRadius(0, radius);
    RegistAttackHitCallback(m_AtkColl);
    m_AtkColl.SetEnable(0);
}

float CStgGmk_IceCube_Act_Hockey::GetParam_Radius() const
{
    return msw::HOCKEY_PRM_S85_ICECUBE_RADIUS;
}

bool CStgGmk_IceCube_Act_Hockey::IsOffensive() const
{
    bool bOffensive = false;
    bcs::FVEC3 vel = GetSrt()->GetVel();
    float speed = bcs::FVEC3Len(&vel);
    float atkVel = GetParam_AtkVel();
    if (atkVel >= 0.0f && speed >= atkVel) {
        bOffensive = true;
    }
    return bOffensive;
}

float CStgGmk_IceCube_Act_Hockey::GetParam_AtkVel() const
{
    return 1.0f;
}

// While the cube is alive the attack sphere has the cube's radius: ahead of
// it by ATK_OFS and raised by the radius while it is offensive, below its
// position by ATK_OFS2 radii otherwise.
void CStgGmk_IceCube_Act_Hockey::UpdateColl()
{
    msw::CStgGmk_Pylon_Act_Hockey::UpdateColl();
    msw::CStgSrtMove *pSrt = GetSrt();
    msw::CStgGmk *pGmk = static_cast<msw::CStgGmk *>(GetParent());
    long idx;
    if (pGmk != 0) {
        idx = pGmk->GetClusterIndex();
    } else {
        idx = -1;
    }
    m_AtkColl.SetIndexAll(idx);
    if (IsAlive()) {
        bcs::FVEC3 pos = pSrt->GetPos();
        float radius = GetParam_Radius();
        bcs::FVEC3 atkPos;
        if (IsOffensive()) {
            bcs::FVEC3 vel = pSrt->GetVel();
            bcs::BCS_Vec_Normalize(atkPos, vel);
            bcs::FVEC3Scale(&atkPos, &atkPos, msw::HOCKEY_PRM_S85_ICECUBE_ATK_OFS);
            bcs::FVEC3Add(&atkPos, &atkPos, &pos);
            atkPos.y += radius;
        } else {
            atkPos = pos;
            atkPos.y -= radius * msw::HOCKEY_PRM_S85_ICECUBE_ATK_OFS2;
        }
        m_AtkColl.SetPos(0, atkPos);
        m_AtkColl.SetRadius(0, radius);
        m_AtkColl.SetEnable(0);
    } else {
        m_AtkColl.SetDisableAll();
    }
}

// The cube does not stop at the map: it goes where it is pushed, and once
// it is more than half a unit outside the court it falls over, away from
// the court (the direction for the FALL state).
int CStgGmk_IceCube_Act_Hockey::CheckCollBg(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, const bcs::FVEC3 &rFrom,
                                            const bcs::FVEC3 &rTo)
{
    float margin;
    rPos = rTo;
    if (IsAlive() && FsmqGetState() != STATE_FALL) {
        float rot = -1.0f;
        int bOut = 0;
        GetParam_MdlRadius();
        margin = 0.5f;
        float left;
        float right;
        float front;
        float back;
        msw::CStageMan::GetInstance()->GetCourtRect(&left, &right, &front, &back);
        if (rTo.x < left - margin) {
            rot = 4.712389f;
            bOut = 1;
        } else if (rTo.x > right + margin) {
            rot = 1.5707964f;
            bOut = 1;
        } else if (rTo.z < front - margin) {
            rot = 3.1415927f;
            bOut = 1;
        } else if (rTo.z > back + margin) {
            bOut = 1;
            rot = 0.0f;
        }
        if (bOut) {
            static_cast<STATE_WORK *>(FsmqSetState(STATE_FALL))->m_fArg = rot;
        }
    }
    return 0;
}

float CStgGmk_IceCube_Act_Hockey::GetParam_MdlRadius() const
{
    return msw::HOCKEY_PRM_S85_ICECUBE_MDL_RADIUS;
}

// A character's attack knocks the cube away from the character, along X or
// Z (whichever is nearer), at a speed that depends on the kind of attack
// and its vanish power.
unsigned long CStgGmk_IceCube_Act_Hockey::OnBodyDamage(msw::CCollBodyUnit *pBody, const msw::CCollAtkUnit *pAtk,
                                                       const bcs::FVEC3 &rPos)
{
    long chrNo = pAtk->m_nOwnerNo;
    bool bChr = false;
    if (pAtk->GetOwnerKind() == 1 && chrNo >= 0) {
        bChr = true;
    }
    const msw::CChrCore *pChr = bChr ? msw::CChrMan::GetChrCore(chrNo) : 0;
    if (pChr != 0) {
        const bcs::FVEC3 &rChrPos = pChr->GetPos();
        bcs::FVEC3 dir;
        VecSub(dir, GetSrt()->GetPos(), rChrPos);
        dir.y = 0.0f;
        VecNormalize(dir, dir);
        if (__fabsf(dir.x) < __fabsf(dir.z)) {
            dir.x = 0.0f;
        } else {
            dir.z = 0.0f;
        }
        VecNormalize(dir, dir);

        unsigned long kind = pAtk->GetAtkKind();
        float power = pAtk->m_fVanishPower;
        float acc = msw::HOCKEY_PRM_S85_ICECUBE_ATK_ACC;
        switch (kind) {
            case 0x13:
                acc = msw::HOCKEY_PRM_S85_ICECUBE_ATK_ACC2;
                break;
            case 0x14:
                acc = msw::HOCKEY_PRM_S85_ICECUBE_ATK_ACC3;
                break;
            case 0x12:
                acc = msw::HOCKEY_PRM_S85_ICECUBE_ATK_ACC4;
                break;
        }
        bcs::FVEC3Scale(&dir, &dir, acc * power);
        GetSrt()->SetVel(dir);
    }
    return 2;
}

// A character hit by the cube shakes its own camera.
void CStgGmk_IceCube_Act_Hockey::OnAttackHit(const msw::CCollAtkUnit *pAtk, const msw::CCollBodyUnit *pBody,
                                             const bcs::FVEC3 &rPos, unsigned long flag)
{
    if (pBody == 0) {
        return;
    }
    if (pBody->GetOwnerKind() != 1 || flag != 1) {
        return;
    }
    msw::CGame::StartCameraQuake(QUAKE_HIT, pBody->m_nOwnerIdx, -1);
}

// As the hockey pylon, with 0x0D in bits 8 to 15 of the result (meaning
// not known).
unsigned long CStgGmk_IceCube_Act_Hockey::OnReflectHit(const msw::CCollDefUnit *pDef, const msw::CBall *pBall,
                                                       bcs::FVEC3 &rPos, bcs::FVEC3 &rVel)
{
    unsigned long ret = msw::CStgGmk_Pylon_Act_Hockey::OnReflectHit(pDef, pBall, rPos, rVel);
    return ((unsigned char)ret | 0xD00) | ((ret >> 16) << 16);
}

void CStgGmk_IceCube_Act_Hockey::OnLanding()
{
    msw::CGame::StartCameraQuake(QUAKE_LANDING, -1, -1);
}

// The splash where the cube fell in: where the line down from the cube
// meets the water surface, which tilts with the stage (GetWorldRotZ),
// 1.8 below the origin.
void CStgGmk_IceCube_Act_Hockey::KickSplashEffect()
{
    msw::CStgSrt *pSrt = GetSrt();
    long eff = GetParam_Effect(4);
    bcs::FVEC3 pos = pSrt->GetPos();
    float rot = msw::CStageMan::GetInstance()->GetWorldRotZ();
    float waterY = -1.8f;

    bcs::FVEC3 water0(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 water1(1.0f, 0.0f, 0.0f);
    bcs::BCS_Vec_RotZ(water1, water1, rot);
    water0.y += waterY;
    water0.z = pos.z;
    water1.y += waterY;
    water1.z = pos.z;
    bcs::FVEC3 down0 = pos;
    bcs::FVEC3 down1(down0.x, 100.0f, down0.z);
    bcs::BCS_GetLineToLineCross(&pos.x, &pos.y, water0.x, water0.y, water1.x, water1.y, down0.x, down0.y, down1.x,
                                down1.y);

    if (eff >= 0) {
        unsigned long hEff = msw::CEffectCtrl::CreateEffect(eff, 3, 0, 0, 0);
        msw::CEffectCluster *pEff = hEff != 0 ? msw::CEffectCtrl::GetInstance()->FindList(hEff) : 0;
        if (pEff != 0) {
            pEff->SetTranslate(&pos);
            bcs::FVEC3 effRot(0.0f, 0.0f, rot);
            pEff->SetRotateRad(&effRot);
        }
    }
    long se = GetParam_SE(5);
    if (se >= 0) {
        msw::CGameSnd::Play(se, pos, 1, -1, 0.0f, 1.0f, 0.0f);
    }
}

void CStgGmk_IceCube_Act_Hockey::CtorState_MOVE()
{
    msw::CStgGmk_Pylon_Act_Hockey::CtorState_MOVE();
}

// The trail effect (slot 1) runs while the cube is offensive.
void CStgGmk_IceCube_Act_Hockey::LoopState_MOVE()
{
    msw::CStgGmk_Pylon_Act_Hockey::LoopState_MOVE();
    if (IsOffensive()) {
        if (!Eff_IsAlive(1)) {
            Eff_CreateAttach(1, GetParam_Effect(5), 0, (unsigned long)0, 0, 0, 0, 0, 3);
        }
    } else {
        Eff_Stop(1);
    }
}

void CStgGmk_IceCube_Act_Hockey::DtorState_MOVE()
{
    msw::CStgGmk_Pylon_Act_Hockey::DtorState_MOVE();
}

// The cube comes back at the height LOC_POS_Y.
void CStgGmk_IceCube_Act_Hockey::CtorState_RESPAWN()
{
    msw::CStgGmk_Pylon_Act_Hockey::CtorState_RESPAWN();
    msw::CStgSrt *pSrt = GetSrt();
    bcs::FVEC3 pos = pSrt->GetPos();
    pos.y = msw::HOCKEY_PRM_S85_ICECUBE_LOC_POS_Y;
    pSrt->SetPos(pos);
}

void CStgGmk_IceCube_Act_Hockey::LoopState_RESPAWN()
{
}

void CStgGmk_IceCube_Act_Hockey::DtorState_RESPAWN()
{
}

long CStgGmk_IceCube_Act_Hockey::GetParam_Effect(long type) const
{
    switch (type) {
        case 1:
            return 0x4A6;
        case 2:
            return 0xC;
        case 3:
            return 0x4A5;
        case 4:
            return 0x4A4;
        default:
            return -1;
    }
}

long CStgGmk_IceCube_Act_Hockey::GetParam_SE(long type) const
{
    switch (type) {
        case 1:
            return 0xDDA;
        case 4:
            return 0xDD9;
        case 5:
            return 0xDDE;
        default:
            return -1;
    }
}

bool CStgGmk_IceCube_Hockey::IsOffensive() const
{
    const CStgGmk_IceCube_Act_Hockey *pAct = static_cast<const CStgGmk_IceCube_Act_Hockey *>(GetAct());
    if (pAct != 0) {
        return pAct->IsOffensive();
    }
    return false;
}

// The tilt of the floor accelerates the cube by GetParam_Acc.
void CStgGmk_IceCube_Hockey::SetAccel(const bcs::FVEC3 &rAccel)
{
    bcs::FVEC3 accel = rAccel;
    CStgGmk_IceCube_Act_Hockey *pAct = static_cast<CStgGmk_IceCube_Act_Hockey *>(GetAct());
    if (pAct != 0) {
        bcs::FVEC3Scale(&accel, &accel, pAct->GetParam_Acc());
    }
    msw::CStgGmk_Pylon_Hockey::SetAccel(accel);
}

float CStgGmk_IceCube_Act_Hockey::GetParam_Acc() const
{
    return SelectByPattern(msw::HOCKEY_PRM_S85_ICE0_ACC, msw::HOCKEY_PRM_S85_ICE1_ACC, msw::HOCKEY_PRM_S85_ICE2_ACC,
                           msw::HOCKEY_PRM_S85_ICE3_ACC);
}

long CStgGmk_IceCubeCluster_Hockey::GetLocValidNumIdx() const
{
    return 0;
}

// The index in the raw location table of position idx of pattern: each
// pattern's block starts at its table entry and holds three values per
// position. The original checks the pattern against patnum (40), not 10,
// so patterns 10 to 39 read past the table. (The "@0" on patnum's symbol
// says another patnum was declared before it in the original file.)
long CStgGmk_IceCubeCluster_Hockey::GetLocRawTblIdx(long pattern, long idx) const
{
    const long tbl[] = { 225, 258, 291, 324, 357, 390, 423, 456, 489, 522 };
    static const long patnum = 40;
    static const long colnum = 3;
    long posNum = (tbl[1] - tbl[0]) / colnum;

    if (pattern < 0 || pattern >= patnum || idx < 0 || idx >= posNum) {
        return -1;
    }
    return tbl[pattern] + idx * colnum;
}
