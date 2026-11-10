// msw::CItemManBoss: the item manager of the boss game, and the items it
// creates (msw::CItemBoss*, msw::CBombExplosionBoss).
#include "game/msw/CItemManBoss.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBombExplosion.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CItemBoss.hpp"
#include "nw4r/math/triangular.h"

msw::CItem *msw::CItemManBoss::CreateItem(long type, const bcs::FVEC3 &rPos)
{
    CItem *pItem;

    switch (type) {
    case CItem::KIND_MUSHROOM:
        pItem = new (0x100) CItemBossMushroom(this);
        break;
    case CItem::KIND_TOADSTOOL:
        pItem = new (0x100) CItemBossToadstool(this);
        break;
    case CItem::KIND_BOMB:
        pItem = new (0x100) CItemBossBomb(this);
        break;
    case CItem::KIND_GSHELL:
        pItem = new (0x100) CItemBossGShell(this);
        break;
    case CItem::KIND_RSHELL:
        pItem = new (0x100) CItemBossRShell(this);
        break;
    case CItem::KIND_BANANA:
        pItem = new (0x100) CItemBossBanana(this);
        break;
    default:
        pItem = new (0x100) CItemBossGShell(this);
        break;
    }

    if (pItem != 0) {
        pItem->Create(rPos);
    }
    return pItem;
}

void msw::CItemManBoss::ExParamInitialize_()
{
}

// --- mushroom ---

void msw::CItemBossMushroom::ExParamUpdate_()
{
    m_fParamMainLife = 600.0f;
    m_fParamScale = 1.4f;
    m_fParamB8 = 9.0f;
    m_fParamBC = 4.0f;
    m_fParamC0 = 35.0f;
}

// --- toadstool ---

void msw::CItemBossToadstool::ExParamUpdate_()
{
    m_fParamMainLife = 600.0f;
    m_fParamScale = 1.4f;
    m_fParamHitEffectFrame = 180.0f;
    m_fParamB8 = 9.0f;
    m_fParamBC = 4.0f;
    m_fParamC0 = 35.0f;
}

long msw::CItemBossToadstool::GetParamThrowType() const
{
    return 0;
}

float msw::CItemBossToadstool::GetParamThrowSearchDirRangeMax() const
{
    return 0.0f;
}

float msw::CItemBossToadstool::GetParamThrowSearchDistRangeMax() const
{
    return 0.0f;
}

void msw::CItemBossToadstool::DoThrowInit(const bcs::FVEC3 &rDir, int arg)
{
    CItemToadstool::DoThrowInit(rDir, arg);
    CollAtkSetAtkType(0, 0x11E0D);
}

// --- green shell ---

void msw::CItemBossGShell::ExParamUpdate_()
{
    m_fParamMainLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamThrowSpeed = 15.5f;
    m_fParamHitEffectFrame = 30.0f;
    m_fParamHitDropCoinRate = 0.5f;
    m_nParamHitDropCoinNumMin = 2;
    m_nParamHitDropCoinNumMax = 5;
    m_nParamReboundNum = 1;
}

float msw::CItemBossGShell::GetBodyCollRadius() const
{
    return 0.4f * GetParamScale();
}

long msw::CItemBossGShell::GetParamThrowType() const
{
    return 1;
}

float msw::CItemBossGShell::GetParamThrowSearchDirRangeMax() const
{
    return 0.7853982f; // 45 degrees
}

float msw::CItemBossGShell::GetParamThrowSearchDistRangeMax() const
{
    return 8.0f;
}

// Throws the shell along rDir without hurting the thrower's side. With
// `arg` set it flies straight for a while; otherwise it falls.
void msw::CItemBossGShell::DoThrowInit(const bcs::FVEC3 &rDir, int arg)
{
    float fSpeed;

    m_BodyColl.ResetTargetFlagAll(0x28);
    CollAtkSetTargetFlagAll(0x7F);
    CollAtkSetHitEffectFrameAll(m_fParamHitEffectFrame);
    CollAtkSetHitDropCoinRateAll(m_fParamHitDropCoinRate);
    CollAtkSetHitDropCoinNumAll(m_nParamHitDropCoinNumMin, m_nParamHitDropCoinNumMax);
    m_AtkColl.SetAtkPowerAll(0.0f);

    fSpeed = (1.0f / 60.0f) * m_fParamThrowSpeed;
    bcs::FVEC3Scale(&m_vVelocity, &rDir, fSpeed);
    m_vMove = m_vVelocity;
    if (arg != 0) {
        m_fGravity = 0.0f;
        m_fThrowTime = 7.0f / fSpeed;
    } else {
        m_fGravity = -0.0125f;
        m_fThrowTime = 0.0f;
    }
}

// --- red shell ---

void msw::CItemBossRShell::ExParamUpdate_()
{
    m_fParamMainLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamThrowSpeed = 15.0f;
    m_fParamHitEffectFrame = 30.0f;
    m_fParamHitDropCoinRate = 0.5f;
    m_nParamHitDropCoinNumMin = 2;
    m_nParamHitDropCoinNumMax = 5;
    m_nParamReboundNum = 1;
}

float msw::CItemBossRShell::GetBodyCollRadius() const
{
    return 0.4f * GetParamScale();
}

long msw::CItemBossRShell::GetParamThrowType() const
{
    return 1;
}

float msw::CItemBossRShell::GetParamThrowSearchDirRangeMax() const
{
    return 0.0f;
}

float msw::CItemBossRShell::GetParamThrowSearchDistRangeMax() const
{
    return 0.0f;
}

// Throws the shell along rDir and picks the target it homes in on.
void msw::CItemBossRShell::DoThrowInit(const bcs::FVEC3 &rDir, int arg)
{
    float fDir;
    float fSpeed;

    CollAtkSetTargetFlagAll(0x7F);
    CollAtkSetHitEffectFrameAll(m_fParamHitEffectFrame);
    CollAtkSetHitDropCoinRateAll(m_fParamHitDropCoinRate);
    CollAtkSetHitDropCoinNumAll(m_nParamHitDropCoinNumMin, m_nParamHitDropCoinNumMax);
    m_AtkColl.SetAtkPowerAll(30.0f);

    // The direction of the throw as an angle in [0, 2 pi].
    fDir = nw4r::math::Atan2Rad(rDir.x, rDir.z);
    while (fDir > 6.2831855f) {
        fDir -= 6.2831855f;
    }
    while (fDir < 0.0f) {
        fDir += 6.2831855f;
    }
    m_nTarget = SearchTarget_(m_vPos, fDir, arg, m_nUnk1AA);

    fSpeed = (1.0f / 60.0f) * m_fParamThrowSpeed;
    bcs::FVEC3Scale(&m_vVelocity, &rDir, fSpeed);
    m_vMove = m_vVelocity;
    if (arg != 0 && m_nUnk1C8 != 0) {
        m_fGravity = 0.0f;
        m_fThrowTime = 7.0f / fSpeed;
    } else {
        m_fGravity = -0.0125f;
        m_fThrowTime = 0.0f;
    }
}

// --- bomb blast ---

namespace msw {

    // The blast of the boss game's bomb: a bigger effect, and the attack
    // collision gets two more flags.
    class CBombExplosionBoss : public CBombExplosion {
        public:
            CBombExplosionBoss(bcs::CTask *pParent, const bcs::FVEC3 &rPos, unsigned long arg, float fRadius,
                               float f2, float f3, long l1, long l2, unsigned long ul)
                : CBombExplosion(pParent, rPos, arg, fRadius, f2, f3, l1, l2, ul)
            {
            }
            // Empty; explicit so its weak copy lands at its original place.
            virtual ~CBombExplosionBoss() {}

            virtual void CreateEffect_(const bcs::FVEC3 &rPos)
            {
                // The radius the effect is made for (name guessed). A
                // function-local static in the original: the constant sits
                // at its own .rodata slot (0x50), not in the float pool.
                static const float BOMB_EXP_BASE_SIZE = 3.0f;

                CGimmickMan::EffectBombExplosionCreate(rPos, 0, m_fRadius / BOMB_EXP_BASE_SIZE, 0x48);
                CGameSnd::Play(0xC51, rPos, 1, -1, 0.0f, 1.0f, 0.0f);
            }

            virtual void Create_(const bcs::FVEC3 &rPos, unsigned long arg)
            {
                CBombExplosion::Create_(rPos, arg);
                if (m_pAtkUnit != 0) {
                    m_pAtkUnit->m_nFlag |= 0x28;
                }
            }
    };
}

// --- bomb ---

void msw::CItemBossBomb::ExParamUpdate_()
{
    m_fParamScale = 1.5f;
    m_fParamHitEffectFrame = 80.0f;
    m_fParamHitDropCoinRate = 1.0f;
    m_nParamHitDropCoinNumMin = 1;
    m_nParamHitDropCoinNumMax = 10;
    m_fParamB8 = 8.5f;
    m_fParamBC = 4.5f;
    m_fParamC0 = 80.0f;
    m_fExplosionCollSize = 3.5f;
}

long msw::CItemBossBomb::GetParamThrowType() const
{
    return 1;
}

float msw::CItemBossBomb::GetParamThrowSearchDirRangeMax() const
{
    return 0.0f;
}

float msw::CItemBossBomb::GetParamThrowSearchDistRangeMax() const
{
    return 0.0f;
}

void msw::CItemBossBomb::DoThrowInit(const bcs::FVEC3 &rDir, int arg)
{
    m_BodyColl.AddTargetFlagAll(0x28);
    CItemBomb::DoThrowInit(rDir, arg);
}

// Replaces the bomb by its blast.
void msw::CItemBossBomb::DoExplode_()
{
    float fRadius;
    float f2;
    float f3;
    long l1;
    long l2;
    unsigned long ul;

    GetExplosionParam(fRadius, f2, f3, l1, l2, ul);
    new (0x100) CBombExplosionBoss(CGame::GetTempParent(), m_vPos, 0, fRadius, f2, f3, l1, l2, ul);
    CollBodySetEnableAll(0);
}

// --- banana ---

void msw::CItemBossBanana::ExParamUpdate_()
{
    m_fParamMainLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamHitEffectFrame = 12.0f;
    m_fParamHitDropCoinRate = 0.5f;
    m_nParamHitDropCoinNumMin = 2;
    m_nParamHitDropCoinNumMax = 10;
    m_fParamB8 = 7.0f;
    m_fParamBC = 3.0f;
    m_fParamC0 = 25.0f;
}

long msw::CItemBossBanana::GetParamThrowType() const
{
    return 0;
}

float msw::CItemBossBanana::GetParamThrowSearchDirRangeMax() const
{
    return 0.0f;
}

float msw::CItemBossBanana::GetParamThrowSearchDistRangeMax() const
{
    return 0.0f;
}

