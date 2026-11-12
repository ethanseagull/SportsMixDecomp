#include "game/msw/CItemBallHockeyKinds.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBombExplosion.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CItemBallShellWreck.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"

// The item behaviours of the hockey game's balls (see
// CItemBallHockeyKinds.hpp). One file: the kinds share their float constants.
// The hockey copy of game/boss/CItemBallBossBomb.cpp.

namespace {

    // Sound of a shell breaking; a shell that reaches the goal after it has
    // been shot plays the next one.
    const long SE_SHELL_BREAK = 0xC4C;
    const long SE_SHELL_BREAK_SHOT = 0xC4D;
    // The bomb's fuse and the change effect.
    const long SE_BOMB_FUSE = 0xC4F;
    const long SE_ITEM_CHANGE = 0xC41;
    const long EFFECT_BOMB_FUSE = 5;
    const long EFFECT_CHANGE_START = 0x4E7;
    const long EFFECT_CHANGE_END = 0x4E8;

    // Sizes the item model is drawn at (shells, bomb and banana; star and
    // mushroom).
    const float ITEM_SCALE = 1.7f;
    const float PLAIN_SCALE = 1.0f;

    inline void SetMdlScale(bcs::CMdlObj *pMdl, float scale)
    {
        if (pMdl != 0) {
            pMdl->m_vScale.x = scale;
            pMdl->m_vScale.y = scale;
            pMdl->m_vScale.z = scale;
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

    CItemBallHockeyGShell::CItemBallHockeyGShell(CBall *pBall)
        : CItemBallHockey(pBall, 0)
    {
        SetMdlScale(m_pMdl, ITEM_SCALE);
    }

    CItemBallHockeyGShell::~CItemBallHockeyGShell()
    {
    }

    void CItemBallHockeyGShell::MainFunc_()
    {
    }

    long CItemBallHockeyGShell::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    long CItemBallHockeyGShell::TrgChrHit_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    long CItemBallHockeyGShell::TrgGoalIn_()
    {
        long nSe = SE_SHELL_BREAK;
        if (IsShot()) {
            nSe = SE_SHELL_BREAK_SHOT;
        }
        BreakShell(this, 0, nSe);
        return true;
    }

    long CItemBallHockeyGShell::TrgFreeMode_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    // Returns 4 when the ball has not been shot.
    long CItemBallHockeyGShell::TrgDamage_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return 4;
    }

    long CItemBallHockeyGShell::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    // Red shell ------------------------------------------------------------

    CItemBallHockeyRShell::CItemBallHockeyRShell(CBall *pBall)
        : CItemBallHockey(pBall, 1)
    {
        SetMdlScale(m_pMdl, ITEM_SCALE);
    }

    CItemBallHockeyRShell::~CItemBallHockeyRShell()
    {
    }

    void CItemBallHockeyRShell::MainFunc_()
    {
    }

    long CItemBallHockeyRShell::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    long CItemBallHockeyRShell::TrgChrHit_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    long CItemBallHockeyRShell::TrgGoalIn_()
    {
        long nSe = SE_SHELL_BREAK;
        if (IsShot()) {
            nSe = SE_SHELL_BREAK_SHOT;
        }
        BreakShell(this, 1, nSe);
        return true;
    }

    long CItemBallHockeyRShell::TrgFreeMode_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    long CItemBallHockeyRShell::TrgDamage_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return 4;
    }

    long CItemBallHockeyRShell::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    // Bomb -----------------------------------------------------------------

    inline void CItemBallHockeyBomb::StartFuse_()
    {
        if (GetMdl() == 0) {
            return;
        }
        if (m_hEffect == 0) {
            m_hEffect = CEffectCtrl::CreateAttachEffect(EFFECT_BOMB_FUSE, m_pMdl, 0, 0, 1, 1, 0, 3, 0, 0, 0);
        }
        if (m_hSe == 0) {
            m_hSe = CGameSnd::Play(SE_BOMB_FUSE, &m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    inline void CItemBallHockeyBomb::StopFuse_()
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

    inline void CItemBallHockeyBomb::Explode_(int bAttach)
    {
        if (m_bExploded == 0) {
            StopFuse_();
            CBall *pBall = GetBall();
            if (pBall != 0) {
                unsigned long nHandle = bAttach ? pBall->GetMdlObj()->GetHandle() : 0;
                bcs::FVEC3 pos = pBall->GetPos();
                new (0) CBombExplosion(pBall, pos, nHandle, 180.0f, m_fPower, 0.0f, 0, 0, 0);
            }
            m_bExploded = 1;
        }
    }

    CItemBallHockeyBomb::CItemBallHockeyBomb(CBall *pBall, float fPower)
        : CItemBallHockey(pBall, 3), m_bExploded(0), m_bShot(0), m_hEffect(0), m_hSe(0), m_fPower(fPower)
    {
        SetMdlScale(m_pMdl, ITEM_SCALE);
        StartFuse_();
    }

    CItemBallHockeyBomb::~CItemBallHockeyBomb()
    {
        StopFuse_();
    }

    void CItemBallHockeyBomb::MainFunc_()
    {
    }

    // The first call only marks the bomb as shot; any later one sets it off.
    long CItemBallHockeyBomb::TrgShootStart_()
    {
        if (m_bShot != 0) {
            Explode_(1);
            return true;
        }
        m_bShot = 1;
        return CItemBall::TrgShootStart_();
    }

    long CItemBallHockeyBomb::TrgGoalIn_()
    {
        Explode_(1);
        return true;
    }

    long CItemBallHockeyBomb::TrgDamage_()
    {
        Explode_(0);
        return true;
    }

    long CItemBallHockeyBomb::TrgDestroy_()
    {
        Explode_(1);
        return CItemBall::TrgDestroy_();
    }

    // Star -----------------------------------------------------------------

    CItemBallHockeyStar::CItemBallHockeyStar(CBall *pBall)
        : CItemBallHockey(pBall, 4)
    {
        SetMdlScale(m_pMdl, PLAIN_SCALE);
    }

    CItemBallHockeyStar::~CItemBallHockeyStar()
    {
    }

    void CItemBallHockeyStar::MainFunc_()
    {
    }

    long CItemBallHockeyStar::TrgShootStart_()
    {
        return false;
    }

    long CItemBallHockeyStar::TrgGoalIn_()
    {
        return true;
    }

    long CItemBallHockeyStar::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    // Banana ---------------------------------------------------------------

    CItemBallHockeyBanana::CItemBallHockeyBanana(CBall *pBall)
        : CItemBallHockey(pBall, 5)
    {
        SetMdlScale(m_pMdl, ITEM_SCALE);
    }

    CItemBallHockeyBanana::~CItemBallHockeyBanana()
    {
    }

    void CItemBallHockeyBanana::MainFunc_()
    {
    }

    long CItemBallHockeyBanana::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    long CItemBallHockeyBanana::TrgChrHit_()
    {
        return IsShot() != 0;
    }

    long CItemBallHockeyBanana::TrgGoalIn_()
    {
        return true;
    }

    long CItemBallHockeyBanana::TrgFreeMode_()
    {
        return IsShot() != 0;
    }

    long CItemBallHockeyBanana::TrgDamage_()
    {
        if (IsShot()) {
            return true;
        }
        return 4;
    }

    long CItemBallHockeyBanana::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    // Mushroom -------------------------------------------------------------

    CItemBallHockeyToadstool::CItemBallHockeyToadstool(CBall *pBall)
        : CItemBallHockey(pBall, 2)
    {
        SetMdlScale(m_pMdl, PLAIN_SCALE);
    }

    CItemBallHockeyToadstool::~CItemBallHockeyToadstool()
    {
    }

    void CItemBallHockeyToadstool::MainFunc_()
    {
    }

    long CItemBallHockeyToadstool::TrgShootStart_()
    {
        return false;
    }

    long CItemBallHockeyToadstool::TrgGoalIn_()
    {
        return true;
    }

    long CItemBallHockeyToadstool::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    // Change effect --------------------------------------------------------

    CItemBallChangeEffectHockey::CItemBallChangeEffectHockey(bcs::CTask *pParent, unsigned long nBallId)
        : CItemBallChangeEffect(pParent, nBallId)
    {
    }

    void CItemBallChangeEffectHockey::SetStartEffect()
    {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall != 0) {
            CEffectCtrl::SimpleEffect(EFFECT_CHANGE_START, pBall->GetPos(), 3, 0, 0);
            CGameSnd::Play(SE_ITEM_CHANGE, pBall->GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    void CItemBallChangeEffectHockey::SetEndEffect()
    {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall != 0) {
            CEffectCtrl::SimpleEffect(EFFECT_CHANGE_END, pBall->GetPos(), 3, 0, 0);
        }
    }

    CItemBallChangeEffectHockey::~CItemBallChangeEffectHockey()
    {
    }
}
