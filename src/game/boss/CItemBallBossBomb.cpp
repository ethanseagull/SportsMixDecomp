#include "game/msw/CItemBallBossKinds.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBombExplosion.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CItemBallShellWreck.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"

// The item behaviours of the boss game's balls (see CItemBallBossKinds.hpp).
// One file: the kinds share their float constants.

namespace {

    // Sound of a shell breaking; a shell that reaches the goal after it has
    // been shot plays the next one.
    const long SE_SHELL_BREAK = 0xC4C;
    const long SE_SHELL_BREAK_SHOT = 0xC4D;
    // The bomb's fuse and the change effect.
    const long SE_BOMB_FUSE = 0xC4F;
    const long SE_ITEM_CHANGE = 0xC41;
    const long EFFECT_BOMB_FUSE = 5;

    // Sizes the item model is drawn at (shells and banana, bomb, star).
    const float ITEM_SCALE = 1.38f;
    const float BOMB_SCALE = 1.8f;
    const float STAR_SCALE = 1.0f;

    inline void SetMdlScale(bcs::CMdlObj *pMdl, float scale)
    {
        if (pMdl != 0) {
            pMdl->m_vScale.x = scale;
            pMdl->m_vScale.y = scale;
            pMdl->m_vScale.z = scale;
        }
    }

    // The original .rodata holds 20.0f, 5.0f and 2.5f after the bomb's
    // 180.0f and before the mushroom's 0.55f, read by nothing left in the
    // file: most likely by bomb code the link stripped. These dead
    // comparisons, made from the last bomb function, keep them in place.
    inline void UnusedReferences(float f)
    {
        if (f == 20.0f) {
        }
        if (f == 5.0f) {
        }
        if (f == 2.5f) {
        }
    }

    // Breaks a shell: throws its wreck off the ball and plays nSe (none when
    // negative).
    inline void BreakShell(const msw::CItemBall *pItem, long nKind, long nSe)
    {
        msw::CBall *pBall = pItem->GetBall();
        if (pBall != 0) {
            new (0) msw::CItemBallShellWreck(pBall, nKind);
            if (nSe >= 0) {
                pBall->PlaySePos(nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            }
        }
    }
}

namespace msw {

    // Green shell ----------------------------------------------------------

    CItemBallBossGShell::CItemBallBossGShell(CBall *pBall)
        : CItemBallBoss(pBall, 0)
    {
        SetMdlScale(m_pMdl, ITEM_SCALE);
    }

    CItemBallBossGShell::~CItemBallBossGShell()
    {
    }

    void CItemBallBossGShell::MainFunc_()
    {
    }

    bool CItemBallBossGShell::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    bool CItemBallBossGShell::TrgChrHit_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    bool CItemBallBossGShell::TrgGoalIn_()
    {
        long nSe = SE_SHELL_BREAK;
        if (IsShot()) {
            nSe = SE_SHELL_BREAK_SHOT;
        }
        BreakShell(this, 0, nSe);
        return true;
    }

    bool CItemBallBossGShell::TrgFreeMode_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    // The original returns 4 when the ball has not been shot; with the bool
    // return type of CItemBall's triggers that cannot be written here.
    bool CItemBallBossGShell::TrgDamage_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return true; // 4 in the original
    }

    bool CItemBallBossGShell::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    bool CItemBallBossGShell::TrgReceive_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    bool CItemBallBossGShell::TrgBlock_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    long CItemBallBossGShell::GetVolleyPassTailEffect(long, long, long) const
    {
        return -1;
    }

    long CItemBallBossGShell::GetVolleySpikeTailEffect(long, long) const
    {
        return -1;
    }

    // Red shell ------------------------------------------------------------

    CItemBallBossRShell::CItemBallBossRShell(CBall *pBall)
        : CItemBallBoss(pBall, 1)
    {
        SetMdlScale(m_pMdl, ITEM_SCALE);
    }

    CItemBallBossRShell::~CItemBallBossRShell()
    {
    }

    void CItemBallBossRShell::MainFunc_()
    {
    }

    bool CItemBallBossRShell::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    bool CItemBallBossRShell::TrgChrHit_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    bool CItemBallBossRShell::TrgGoalIn_()
    {
        long nSe = SE_SHELL_BREAK;
        if (IsShot()) {
            nSe = SE_SHELL_BREAK_SHOT;
        }
        BreakShell(this, 1, nSe);
        return true;
    }

    bool CItemBallBossRShell::TrgFreeMode_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    // Returns 4 in the original when not shot; see the green shell.
    bool CItemBallBossRShell::TrgDamage_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return true; // 4 in the original
    }

    bool CItemBallBossRShell::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    bool CItemBallBossRShell::TrgReceive_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    bool CItemBallBossRShell::TrgBlock_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    long CItemBallBossRShell::GetVolleyPassTailEffect(long, long, long) const
    {
        return -1;
    }

    long CItemBallBossRShell::GetVolleySpikeTailEffect(long, long) const
    {
        return -1;
    }

    // Bomb -----------------------------------------------------------------

    inline void CItemBallBossBomb::StartFuse_()
    {
        if (m_pMdl == 0) {
            return;
        }
        if (m_hEffect == 0) {
            m_hEffect = CEffectCtrl::CreateAttachEffect(EFFECT_BOMB_FUSE, m_pMdl, 0, 0, 1, 1, 0, 3, 0, 0, 0);
        }
        if (m_hSe == 0) {
            m_hSe = CGameSnd::Play(SE_BOMB_FUSE, &m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    inline void CItemBallBossBomb::StopFuse_()
    {
        if (m_hEffect != 0) {
            CEffectCtrl::StopAttachEffect(m_hEffect);
            m_hEffect = 0;
        }
        if (m_hSe != 0) {
            bcs::CSndTask *pSe = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_hSe));
            if (pSe != 0) {
                pSe->Stop(0.0f);
            }
            m_hSe = 0;
        }
    }

    inline void CItemBallBossBomb::Explode_(int bAttach)
    {
        if (m_bExploded == 0) {
            StopFuse_();
            CBall *pBall = GetBall();
            if (pBall != 0) {
                unsigned long nHandle = bAttach ? pBall->GetMdlObj()->GetHandle() : 0;
                bcs::FVEC3 pos = pBall->GetPos();
                new (0) CBombExplosion(pBall, pos, nHandle, 180.0f, 4.5f, 0.0f, 0, 0, 0);
            }
            m_bExploded = 1;
        }
    }

    CItemBallBossBomb::CItemBallBossBomb(CBall *pBall)
        : CItemBallBoss(pBall, 3), m_bExploded(0), m_bShot(0), m_fTime(60.0f), m_fTimer(60.0f), m_fRate(1.0f),
          m_fUnk78(0.0f), m_fUnk7C(0.0f), m_hEffect(0), m_hSe(0)
    {
        SetMdlScale(m_pMdl, BOMB_SCALE);
        StartFuse_();
    }

    CItemBallBossBomb::~CItemBallBossBomb()
    {
        StopFuse_();
    }

    void CItemBallBossBomb::MainFunc_()
    {
    }

    // The first call only marks the bomb as shot; any later one sets it off.
    bool CItemBallBossBomb::TrgShootStart_()
    {
        if (m_bShot != 0) {
            Explode_(1);
            return true;
        }
        m_bShot = 1;
        return CItemBall::TrgShootStart_();
    }

    bool CItemBallBossBomb::TrgReceive_()
    {
        if (IsShot()) {
            Explode_(1);
            return true;
        }
        return false;
    }

    bool CItemBallBossBomb::TrgBlock_()
    {
        if (IsShot()) {
            Explode_(1);
            return true;
        }
        return false;
    }

    bool CItemBallBossBomb::TrgGoalIn_()
    {
        Explode_(1);
        return true;
    }

    // Returns 4 in the original when not shot; see the green shell.
    bool CItemBallBossBomb::TrgDamage_()
    {
        if (IsShot()) {
            Explode_(0);
            return true;
        }
        return true; // 4 in the original
    }

    long CItemBallBossBomb::GetVolleyPassTailEffect(long, long, long) const
    {
        return -1;
    }

    long CItemBallBossBomb::GetVolleySpikeTailEffect(long, long) const
    {
        return -1;
    }

    bool CItemBallBossBomb::TrgPreparationTouch_()
    {
        UnusedReferences(m_fRate);
        return false;
    }

    // Mushroom -------------------------------------------------------------

    CItemBallBossToadstool::CItemBallBossToadstool(CBall *pBall)
        : CItemBallBoss(pBall, 2)
    {
    }

    CItemBallBossToadstool::~CItemBallBossToadstool()
    {
    }

    float CItemBallBossToadstool::GetScale() const
    {
        return 0.55f;
    }

    void CItemBallBossToadstool::MainFunc_()
    {
    }

    bool CItemBallBossToadstool::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    bool CItemBallBossToadstool::TrgChrHit_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return true;
    }

    bool CItemBallBossToadstool::TrgGoalIn_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return true;
    }

    bool CItemBallBossToadstool::TrgFreeMode_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return true;
    }

    bool CItemBallBossToadstool::TrgDamage_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return true;
    }

    bool CItemBallBossToadstool::TrgDestroy_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return CItemBall::TrgDestroy_();
    }

    bool CItemBallBossToadstool::TrgReceive_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return true;
    }

    bool CItemBallBossToadstool::TrgBlock_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return true;
    }

    long CItemBallBossToadstool::GetVolleySpikeTailEffect(long, long) const
    {
        return -1;
    }

    long CItemBallBossToadstool::GetVolleyPassTailEffect(long, long, long) const
    {
        return -1;
    }

    // Star -----------------------------------------------------------------

    CItemBallBossStar::CItemBallBossStar(CBall *pBall)
        : CItemBallBoss(pBall, 4)
    {
        SetMdlScale(m_pMdl, STAR_SCALE);
    }

    CItemBallBossStar::~CItemBallBossStar()
    {
    }

    float CItemBallBossStar::GetScale() const
    {
        return STAR_SCALE;
    }

    void CItemBallBossStar::MainFunc_()
    {
    }

    bool CItemBallBossStar::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    bool CItemBallBossStar::TrgChrHit_()
    {
        return true;
    }

    bool CItemBallBossStar::TrgGoalIn_()
    {
        return true;
    }

    bool CItemBallBossStar::TrgFreeMode_()
    {
        return true;
    }

    bool CItemBallBossStar::TrgDamage_()
    {
        return true;
    }

    bool CItemBallBossStar::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    bool CItemBallBossStar::TrgReceive_()
    {
        return true;
    }

    bool CItemBallBossStar::TrgBlock_()
    {
        return true;
    }

    long CItemBallBossStar::GetVolleyPassTailEffect(long, long, long) const
    {
        return -1;
    }

    long CItemBallBossStar::GetVolleySpikeTailEffect(long, long) const
    {
        return -1;
    }

    float CItemBallBossStar::GetVolleyFlickDownPower(long, float) const
    {
        return 180.0f;
    }

    // Banana ---------------------------------------------------------------

    CItemBallBossBanana::CItemBallBossBanana(CBall *pBall)
        : CItemBallBoss(pBall, 5)
    {
        SetMdlScale(m_pMdl, ITEM_SCALE);
    }

    CItemBallBossBanana::~CItemBallBossBanana()
    {
    }

    void CItemBallBossBanana::MainFunc_()
    {
    }

    bool CItemBallBossBanana::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    bool CItemBallBossBanana::TrgChrHit_()
    {
        return IsShot();
    }

    bool CItemBallBossBanana::TrgGoalIn_()
    {
        return true;
    }

    bool CItemBallBossBanana::TrgFreeMode_()
    {
        return IsShot();
    }

    // Returns 4 in the original when not shot; see the green shell.
    bool CItemBallBossBanana::TrgDamage_()
    {
        if (IsShot()) {
            return true;
        }
        return true; // 4 in the original
    }

    bool CItemBallBossBanana::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    bool CItemBallBossBanana::TrgReceive_()
    {
        return IsShot();
    }

    bool CItemBallBossBanana::TrgBlock_()
    {
        return IsShot();
    }

    long CItemBallBossBanana::GetVolleyPassTailEffect(long, long, long) const
    {
        return -1;
    }

    long CItemBallBossBanana::GetVolleySpikeTailEffect(long, long) const
    {
        return -1;
    }

    // Change effect --------------------------------------------------------

    CItemBallChangeEffectBoss::CItemBallChangeEffectBoss(bcs::CTask *pParent, unsigned long nBallId)
        : CItemBallChangeEffect(pParent, nBallId)
    {
    }

    void CItemBallChangeEffectBoss::SetStartEffect()
    {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall != 0) {
            CGameSnd::Play(SE_ITEM_CHANGE, pBall->GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    // Looks the ball up but plays nothing. (The original calls GetTask
    // inside a stack frame rather than as a tail call: something empty in
    // this build followed it.)
    void CItemBallChangeEffectBoss::SetEndEffect()
    {
        bcs::CTaskMan::GetTask(m_nBallId);
    }

    CItemBallChangeEffectBoss::~CItemBallChangeEffectBoss()
    {
    }
}
