#include "bcs/New.hpp"
#include "game/msw/CItemManDodge.hpp"
#include "game/msw/CItemDodge.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CItemDodgeDD.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/BCS_Phys.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/CMdlObj.hpp"
#include "nw4r/math/triangular.h"
#include <math.h>

namespace {
    // Brings an angle in radians into [0, 2 pi].
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

    inline float Clamp(float value, float min, float max)
    {
        return value > max ? max : (value < min ? min : value);
    }

    // rVel reflected on the plane with normal rNormal, unless it already
    // moves away from it.
    inline void Reflect(bcs::FVEC3 &rVel, const bcs::FVEC3 &rNormal)
    {
        float dot = 2.0f * bcs::FVEC3Dot(&rVel, &rNormal);
        if (dot > 0.0f) {
            // already moving away
        } else {
            bcs::FVEC3 work;
            bcs::FVEC3Scale(&work, &rNormal, dot);
            bcs::FVEC3Sub(&rVel, &rVel, &work);
        }
    }
}

// The original file goes on with the dodgeball item classes and the
// CItemManDodgeDD / CItemDodgeBombDD_* family (the split's unit
// game/dodge/CItemDodgeBombDD_Base, not written yet): this unit's .data holds
// their vtables.

namespace msw {

CItem *CItemManDodge::CreateItem(long kind, const bcs::FVEC3 &pos) {
    CItem *pItem;

    switch (kind) {
        case CItem::KIND_MUSHROOM:
            pItem = new (0x100) CItemDodgeMushroom(this);
            break;
        case CItem::KIND_TOADSTOOL:
            pItem = new (0x100) CItemDodgeToadstool(this);
            break;
        case CItem::KIND_BOMB:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            pItem = new (0x100) CItemDodgeBomb(this);
            break;
        case CItem::KIND_GSHELL:
            pItem = new (0x100) CItemDodgeGShell(this);
            break;
        case CItem::KIND_RSHELL:
            pItem = new (0x100) CItemDodgeRShell(this);
            break;
        case CItem::KIND_BANANA:
            pItem = new (0x100) CItemDodgeBanana(this);
            break;
        default:
            pItem = new (0x100) CItemDodgeGShell(this);
            break;
    }

    if (pItem != 0) {
        pItem->Create(pos);
    }
    return pItem;
}

void CItemManDodge::ExParamInitialize_() {
}

// --- mushroom ---

void CItemDodgeMushroom::ExParamUpdate_() {
    m_fParamMainLife = 900.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.4f;
    m_fParamB8 = 6.0f;
    m_fParamBC = 3.0f;
    m_fParamC0 = 30.0f;
}

void CItemDodgeMushroom::DoThrowInit(const bcs::FVEC3 &rDir, int arg) {
    SetMapDetectCode(0);
    CItemMushroom::DoThrowInit(rDir, arg);
}

// --- toadstool ---

void CItemDodgeToadstool::ExParamUpdate_() {
    m_fParamMainLife = 900.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.4f;
    m_fParamHitEffectFrame = 600.0f;
    m_fParamB8 = 6.0f;
    m_fParamBC = 3.0f;
    m_fParamC0 = 30.0f;
}

long CItemDodgeToadstool::GetParamThrowType() const {
    return 0;
}

float CItemDodgeToadstool::GetParamThrowSearchDirRangeMax() const {
    return 0.017453292f; // 1 degree
}

float CItemDodgeToadstool::GetParamThrowSearchDistRangeMax() const {
    return 0.1f;
}

void CItemDodgeToadstool::DoThrowInit(const bcs::FVEC3 &rDir, int arg) {
    SetMapDetectCode(0);
    CItemToadstool::DoThrowInit(rDir, arg);
}

// --- bomb ---

void CItemDodgeBomb::ExParamUpdate_() {
    m_fParamScale = 1.5f;
    m_fParamHitEffectFrame = 80.0f;
    m_fParamHitDropCoinRate = 0.0f;
    m_nParamHitDropCoinNumMin = 0;
    m_nParamHitDropCoinNumMax = 0;
    m_fParamB8 = 6.5f;
    m_fParamBC = 3.5f;
    m_fParamC0 = 70.0f;
    m_fExplosionCollSize = 3.3f;
}

long CItemDodgeBomb::GetParamThrowType() const {
    return 1;
}

float CItemDodgeBomb::GetParamThrowSearchDirRangeMax() const {
    return 0.017453292f; // 1 degree
}

float CItemDodgeBomb::GetParamThrowSearchDistRangeMax() const {
    return 0.1f;
}

void CItemDodgeBomb::DoThrowInit(const bcs::FVEC3 &rDir, int) {
    SetMapDetectCode(0);
    SetVelParabola_(rDir, m_fParamB8, m_fParamBC, m_fParamC0);
}

// The last parameter is the set of characters the blast hurts, one bit per
// character index: those in the field, and those outside it on the bomb's
// own side (m_nUnk1AA).
void CItemDodgeBomb::GetExplosionParam(float &a, float &size, float &c, long &d, long &e, unsigned long &rTarget) {
    a = m_fParamHitEffectFrame;
    size = m_fExplosionCollSize;
    c = m_fParamHitDropCoinRate;
    d = m_nParamHitDropCoinNumMin;
    e = m_nParamHitDropCoinNumMax;
    rTarget = 0;

    if (CChrMan::IsActive()) {
        long num = CChrMan::GetChrCoreNum();
        if (num > 8) {
            num = 8;
        }
        for (long i = 0; i < num; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable()) {
                if (!pChr->IsDodgeInfield() || pChr->GetTeamNo() == m_nUnk1AA) {
                    rTarget |= 1 << i;
                }
            }
        }
    }
}

// --- banana ---

void CItemDodgeBanana::ExParamUpdate_() {
    m_fParamMainLife = 900.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.4f;
    m_fParamHitEffectFrame = 12.0f;
    m_fParamHitDropCoinRate = 0.0f;
    m_nParamHitDropCoinNumMin = 0;
    m_nParamHitDropCoinNumMax = 0;
    m_fParamB8 = 5.0f;
    m_fParamBC = 2.8f;
    m_fParamC0 = 25.0f;
}

long CItemDodgeBanana::GetParamThrowType() const {
    return 0;
}

float CItemDodgeBanana::GetParamThrowSearchDirRangeMax() const {
    return 0.017453292f; // 1 degree
}

float CItemDodgeBanana::GetParamThrowSearchDistRangeMax() const {
    return 0.1f;
}

void CItemDodgeBanana::DoThrowInit(const bcs::FVEC3 &rDir, int arg) {
    SetMapDetectCode(0);
    CItemBanana::DoThrowInit(rDir, arg);
}

// --- DD game ---

// Items thrown in the DD game: the DD bomb of that kind (the normal one for
// any kind it has no class for).
CItem *CItemManDodgeDD::CreateThrowItem(long kind, const bcs::FVEC3 &pos) {
    CItem *pItem;

    switch (kind) {
        case 7:
            pItem = new (0x100) CItemDodgeBombDD_Small(this);
            break;
        case 9:
            pItem = new (0x100) CItemDodgeBombDD_Mega(this);
            break;
        case 10:
            pItem = new (0x100) CItemDodgeBombDD_Time(this);
            break;
        case 11:
            pItem = new (0x100) CItemDodgeBombDD_LockOn(this);
            break;
        default:
            pItem = new (0x100) CItemDodgeBombDD_Nml(this);
            break;
    }

    if (pItem != 0) {
        pItem->Create(pos);
    }
    return pItem;
}

void CItemDodgeBombDD_Small::ExParamUpdate_() {
    m_fParamScale = 1.0f;
    m_fParamHitEffectFrame = 30.0f;
    m_fParamHitDropCoinRate = 0.0f;
    m_nParamHitDropCoinNumMin = 0;
    m_nParamHitDropCoinNumMax = 0;
    m_fParamB8 = 6.5f;
    m_fParamBC = 3.5f;
    m_fParamC0 = 70.0f;
    m_fExplosionCollSize = 2.0f;
}

void CItemDodgeBombDD_Nml::ExParamUpdate_() {
    m_fParamScale = 1.6f;
    m_fParamHitEffectFrame = 60.0f;
    m_fParamHitDropCoinRate = 0.0f;
    m_nParamHitDropCoinNumMin = 0;
    m_nParamHitDropCoinNumMax = 0;
    m_fParamB8 = 6.5f;
    m_fParamBC = 3.5f;
    m_fParamC0 = 70.0f;
    m_fExplosionCollSize = 3.7f;
}

void CItemDodgeBombDD_Mega::ExParamUpdate_() {
    m_fParamScale = 5.0f;
    m_fParamHitEffectFrame = 90.0f;
    m_fParamHitDropCoinRate = 0.0f;
    m_nParamHitDropCoinNumMin = 0;
    m_nParamHitDropCoinNumMax = 0;
    m_fParamB8 = 6.5f;
    m_fParamBC = 3.5f;
    m_fParamC0 = 70.0f;
    m_fExplosionCollSize = 6.0f;
}

void CItemDodgeBombDD_Time::ExParamUpdate_() {
    m_fParamMainLife = 300.0f;
    m_fParamDropLife = 300.0f;
    m_fParamScale = 1.6f;
    m_fParamHitEffectFrame = 60.0f;
    m_fParamHitDropCoinRate = 0.0f;
    m_nParamHitDropCoinNumMin = 0;
    m_nParamHitDropCoinNumMax = 0;
    m_fParamB8 = 6.5f;
    m_fParamBC = 3.5f;
    m_fParamC0 = 70.0f;
    m_fExplosionCollSize = 3.7f;
}

void CItemDodgeBombDD_LockOn::ExParamUpdate_() {
    m_fParamScale = 1.6f;
    m_fParamHitEffectFrame = 60.0f;
    m_fParamHitDropCoinRate = 0.0f;
    m_nParamHitDropCoinNumMin = 0;
    m_nParamHitDropCoinNumMax = 0;
    m_fParamB8 = 6.5f;
    m_fParamBC = 3.5f;
    m_fParamC0 = 70.0f;
    m_fExplosionCollSize = 3.7f;
}

CItemDodgeBombDD_Base::~CItemDodgeBombDD_Base() {
    StopSe_();
}

void CItemDodgeBombDD_Base::StopEffect_() {
    CItemBomb::StopEffect_();
    StopSe_();
}

void CItemDodgeBombDD_Base::Create(const bcs::FVEC3 &pos) {
    CItem::Create(pos);
    CreateMdlObj(3, 0, 1, 0x20);

    // The tint of the model: black, green for the lock-on bomb.
    bcs::BCS_Color color(0);
    switch (GetItemType()) {
        case 7:
            color.r = 0;
            color.g = 0;
            color.b = 0;
            m_fExplodeWait = 0.0f;
            break;
        case 8:
            color.r = 0;
            color.g = 0;
            color.b = 0;
            m_fExplodeWait = 0.0f;
            break;
        case 9:
            color.r = 0;
            color.g = 0;
            color.b = 0;
            m_fExplodeWait = 60.0f;
            break;
        case 10:
            color.r = 0;
            color.g = 0;
            color.b = 0;
            m_fExplodeWait = 0.0f;
            break;
        case 11:
            color.r = 0;
            color.g = 0xFF;
            color.b = 0;
            m_fExplodeWait = 0.0f;
            break;
    }
    if (m_pMdl != 0) {
        m_pMdl->SetMatTevColorRGB(GX_TEVREG0, color);
    }
    if (m_pMdlSub != 0) {
        m_pMdlSub->SetMatTevColorRGB(GX_TEVREG0, color);
    }

    CollBodyCreate(2);
    if (GetItemType() == 9) {
        m_BodyColl.SetPushRank(0, 2);
        m_BodyColl.SetPushPower(0, 5);
    }
    CollBodySetPushEnable(0, 1);
    m_BodyColl.SetBodyEnable(1);
    m_BodyColl.SetTargetFlag(1, 2);
    m_BodyColl.SetRadius(1, 2.0f * GetBodyCollRadius());
}

void CItemDodgeBombDD_Base::DoThrowInit(const bcs::FVEC3 &pos, int) {
    m_nFlag |= 0x20;
    SetVelParabola_(pos, 6.5f, 3.5f, 70.0f);
}

int CItemDodgeBombDD_Base::DoParabolaThrow(const bcs::FVEC3 &pos, float f1, float f2) {
    m_nFlag |= 0x20;
    int bThrown = CItem::DoParabolaThrow(pos, f1, f2);
    if (bThrown) {
        AnimPlay(0, 1, 0.0f, 0.0f);
    }
    return bThrown;
}

// Bounces the bomb off what pushes it (the mega bomb is not turned by
// owners of kind 1), and makes it blow up when an owner of kind 1 pushes it
// (the time bomb only once it has been hit).
void CItemDodgeBombDD_Base::CallbackBodyPush_(CCollBodyUnit *pSelf, CCollBodyUnit *pOther, bcs::FVEC3 *pPush) {
    CItem::CallbackBodyPush_(pSelf, pOther, pPush);

    if (pPush != 0) {
        int bReflect = 1;
        if (GetItemType() == 9 && pOther->GetOwnerKind() == 1) {
            bReflect = 0;
        }
        if (bReflect) {
            bcs::FVEC3 normal(*pPush);
            if (normal.x * normal.x + normal.z * normal.z > 0.0f) {
                bcs::FVEC3 flat(normal.x, 0.0f, normal.z);
                bcs::BCS_Vec_Normalize(normal, flat);
                Reflect(m_vVelocity, normal);
            }
        }
    }

    if (pOther->GetOwnerKind() == 1) {
        switch (GetItemType()) {
            case 11:
                m_bExplode = 1;
                m_fExplodeRestFrame = 0.0f;
                break;
            case 9:
                m_bExplode = 1;
                m_fExplodeRestFrame = 0.0f;
                break;
            case 10:
                if (m_bHit) {
                    m_bExplode = 1;
                    m_fExplodeRestFrame = 0.0f;
                }
                break;
            default:
                m_bExplode = 1;
                m_fExplodeRestFrame = 0.0f;
                break;
        }
    }
}

// The time bomb, hit by an attack of kind 0x15, is knocked away from it in
// a random hop and blows up within a second.
bool CItemDodgeBombDD_Base::CallbackBodyDamage_(CCollBodyUnit *, const CCollAtkUnit *pAtk, const bcs::FVEC3 &) {
    if (pAtk->GetAtkKind() == 0x15 && GetItemType() == 10) {
        m_bHit = 1;
        m_vVelocity.x = 0.0f;
        m_vVelocity.y = 0.0f;
        m_vVelocity.z = 0.0f;

        bcs::FVEC3 dir(0.0f, 0.0f, 0.0f);
        dir.x = m_vPos.x - pAtk->m_fPosX;
        dir.z = m_vPos.z - pAtk->m_fPosZ;
        bcs::FVEC3 away(dir.x, 0.0f, dir.z);
        bcs::BCS_Vec_Normalize(dir, away);

        float frame = 60.0f;
        float rand = bcs::BCS_RandMax(1.0f);
        float height = -1.5f * rand + 2.0f;
        float dist = 1.5f * rand + 3.5f;

        SetRotY(WrapRad(nw4r::math::Atan2Rad(dir.x, dir.z)));

        float speed = dist / frame;
        m_vVelocity.x = dir.x * speed;
        m_vVelocity.z = dir.z * speed;
        bcs::BCS_Phys_CalcYVG(frame, m_vPos.y, 0.0f, m_vPos.y + height, m_vVelocity.y, m_fGravity);

        m_fExplodeRestFrame = m_fExplodeRestFrame > frame ? frame : m_fExplodeRestFrame;
    }
    return false;
}

void CItemDodgeBombDD_Base::DoExplode_() {
    float fRadius;
    float f2;
    float f3;
    long l1;
    long l2;
    unsigned long ul;

    GetExplosionParam(fRadius, f2, f3, l1, l2, ul);
    new (0x100) CBombExplosionDD(CGame::GetTempParent(), GetItemType(), m_vPos, 0, fRadius, f2, f3, l1, l2, ul);
    CollBodySetEnableAll(0);
    m_BodyColl.SetBodyDisableAll();
}

void CItemDodgeBombDD_Base::CtorStateThrow() {
    m_fStateTimer = GetParamMainLife();
    m_nFlag &= ~0x2000;
    m_nFlag &= ~0x4;
    m_nFlag |= 0x400;
    CollBodySetEnableAll(1);
    m_fExplodeRestFrame = GetParamMainLife();
    m_fFlashPhase = 0.0f;
    if (!m_bTimer) {
        StartEffect_();
    }
    m_fUnk160 = -0.5f;
    m_nSubStep = 0;
}

void CItemDodgeBombDD_Base::LoopStateThrow() {
    if (m_fExplodeWait > 0.0f) {
        m_fExplodeWait -= GetFrameRateF();
    }

    switch (m_nSubStep) {
        case 0:
            DoMove();
            if (m_nFlag & 0x80) {
                DoShakeInit(0.1f, 0.7853982f);
                m_vVelocity.x *= 0.5f;
                m_vVelocity.z *= 0.5f;
                if (m_bTimer) {
                    if (m_fExplodeRestFrame <= 0.0f) {
                        m_bExplode = 1;
                    }
                    if (m_bHit) {
                        m_bExplode = 1;
                    }
                } else {
                    m_bExplode = 1;
                }
            }

            if (m_bTimer) {
                // The time bomb counts down once it has touched the ground
                // or been hit, and starts flashing for its last two seconds.
                if (((m_nFlag & 0x100) || m_bHit) && m_fExplodeRestFrame > 0.0f) {
                    m_fExplodeRestFrame -= GetFrameRateF();
                    if (m_fExplodeRestFrame <= 0.0f) {
                        m_fExplodeRestFrame = 0.0f;
                        m_bExplode = 1;
                    }
                }
                if (m_fExplodeRestFrame <= 120.0f && !m_bEffect) {
                    StartEffect_();
                }
                if (m_bHit && !m_bEffect) {
                    StartEffect_();
                }
            } else if (m_fExplodeRestFrame > 0.0f) {
                m_fExplodeRestFrame -= GetFrameRateF();
                if (m_fExplodeRestFrame < 0.0f) {
                    m_fExplodeRestFrame = 0.0f;
                }
            }

            if (m_bEffect) {
                // Flash red, faster as the end comes near.
                float step = 0.4f * GetFrameRateF();
                m_fFlashPhase += (1.0f - (1.0f / 60.0f) * m_fExplodeRestFrame) * step;
                m_fFlashPhase = fmod(m_fFlashPhase, 1.0);

                float flash = Clamp(fabs(nw4r::math::SinRad(WrapRad(3.1415927f * m_fFlashPhase))), 0.0f, 1.0f);

                unsigned char level = (unsigned char)(255.0f - 200.0f * flash);
                bcs::BCS_Color base;
                base = bcs::BCS_Color(level, level, level, 0xFF);
                if (m_pMdl != 0) {
                    m_pMdl->SetSilhouetteColor(bcs::BCS_Color(0xFF2020FF));
                    m_pMdl->SetBaseColor(base);
                }
            }

            if (m_bExplode && m_fExplodeWait <= 0.0f) {
                m_vPos.y = 0.0f;
                StopEffect_();
                m_vVelocity.x = 0.0f;
                m_vVelocity.y = 0.0f;
                m_vVelocity.z = 0.0f;
                m_nFlag &= ~0x4;
                m_nFlag |= 0x8;
                DoExplode_();
                m_fStateTimer = 8.0f;
                m_nSubStep++;
            }
            break;

        case 1:
            m_fStateTimer -= GetFrameRateF();
            if (m_fStateTimer <= 0.0f) {
                DoDelete();
            }
            break;
    }
}

} // namespace msw

// The blast: the small bomb's is smaller and the mega bomb's bigger, with
// a camera quake; the others shake the camera a little.
void CBombExplosionDD::CreateEffect_(const bcs::FVEC3 &rPos) {
    bcs::FVEC3 pos(rPos);
    pos.y = 0.0f;

    long nEff = 0x483;
    long nSe = 0xDE5;
    long nQuake = -1;
    m_fUnk68 = 25.0f;
    m_fUnk70 = 6.0f;

    switch (m_nItemType) {
        case 7:
            m_fUnk68 = 15.0f;
            m_fUnk70 = 4.0f;
            nEff = 0x482;
            nSe = 0xDE4;
            break;
        case 9:
            m_fUnk68 = 35.0f;
            m_fUnk70 = 10.0f;
            nEff = 0x484;
            nSe = 0xDE6;
            nQuake = 0x3C;
            break;
        case 10:
            m_fUnk68 = 25.0f;
            m_fUnk70 = 6.0f;
            nQuake = 0x3D;
            break;
        case 11:
            m_fUnk68 = 25.0f;
            m_fUnk70 = 6.0f;
            nQuake = 0x3D;
            break;
        default:
            nQuake = 0x3D;
            break;
    }

    unsigned long hEffect = msw::CEffectCtrl::CreateEffect(nEff, 3, 0, 0, 0);
    msw::CEffectCluster *pCluster = msw::CEffectCtrl::GetInstance()->FindList(hEffect);
    if (pCluster != 0) {
        pCluster->SetTranslate(&pos);
    }
    if (nQuake >= 0) {
        msw::CGame::StartCameraQuake(nQuake, -1, -1);
    }
    if (nSe >= 0) {
        msw::CGameSnd::Play(nSe, rPos, 1, -1, 0.0f, 1.0f, 0.0f);
    }
}
