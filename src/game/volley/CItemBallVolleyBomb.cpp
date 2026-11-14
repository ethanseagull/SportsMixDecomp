#include "game/msw/CItemBallVolleyKinds.hpp"
#include "game/msw/BallVolleySpikeShotArg.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBombExplosion.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CItemBallShellWreck.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"

// The item behaviours of the volley game's balls (see
// CItemBallVolleyKinds.hpp). One file: the kinds share their float
// constants. The same set as the boss game's (game/boss/CItemBallBossBomb.cpp)
// plus the volleyball parameters: spike, receive and block reactions,
// flick-down power and tail effects.

namespace {

    using msw::BALL::VOLLEY::SPIKE_SHOT_ARG;

    // Sound of a shell breaking; a shell that reaches the goal after it has
    // been shot plays the next one.
    const long SE_SHELL_BREAK = 0xC4C;
    const long SE_SHELL_BREAK_SHOT = 0xC4D;
    // The bomb's fuse and the change effect.
    const long SE_BOMB_FUSE = 0xC4F;
    const long SE_ITEM_CHANGE = 0xC41;
    const long EFFECT_BOMB_FUSE = 5;
    // Sound the shells' and the banana's spikes get.
    const long SE_ITEM_SPIKE = 0xC3F;

    // Tail effects of the item spikes and passes.
    const long EFFECT_TAIL_GSHELL = 0x4F0;
    const long EFFECT_TAIL_RSHELL = 0x4F1;
    const long EFFECT_TAIL_TOADSTOOL = 0x4F2;
    const long EFFECT_TAIL_BANANA = 0x4F3;
    const long EFFECT_TAIL_BOMB = 0x4F4;
    const long EFFECT_TAIL_STAR = 0x4F6;
    // Effects of the change (start and end).
    const long EFFECT_CHANGE_START = 0x4F9;
    const long EFFECT_CHANGE_END = 0x4FA;

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

    // The original .rodata holds 5.0f and 2.5f after the bomb's constants
    // and before the stripped mushroom ball's statics, read by nothing left
    // in the file: most likely by mushroom code the link stripped. These
    // dead comparisons, made from the last bomb function, keep them in
    // place.
    inline void UnusedReferences(float f)
    {
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

    // Picks 1 or 2 at random for field 0x18 of a spike unless it is one of
    // them already.
    inline void ChooseSpikeSide(SPIKE_SHOT_ARG &rArg)
    {
        if (rArg.m_nUnk18 < 1 || rArg.m_nUnk18 > 2) {
            long nSide;
            if (bcs::BCS_RandMax(1L) != 0) {
                nSide = 1;
            } else {
                nSide = 2;
            }
            rArg.m_nUnk18 = nSide;
        }
    }
}

namespace msw {

    // Green shell ----------------------------------------------------------

    CItemBallVolleyGShell::CItemBallVolleyGShell(CBall *pBall)
        : CItemBallVolley(pBall, 0)
    {
        SetMdlScale(m_pMdl, ITEM_SCALE);
    }

    CItemBallVolleyGShell::~CItemBallVolleyGShell()
    {
    }

    void CItemBallVolleyGShell::GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg) const
    {
        rArg.m_nUnk0C = 0;
        rArg.m_nUnk10 = 0;
        rArg.m_nUnk18 = 0;
        rArg.m_nUnk14 = 0;
        rArg.m_nUnk20 = 0;
        rArg.m_fUnk58 = 24.0f;
        rArg.m_fUnk5C = 45.0f;
        rArg.m_fUnk60 = 45.0f;
        rArg.m_fUnk68 = 0.8f;
        rArg.m_fUnk6C = 15.0f;
        rArg.m_fUnk7C = 0.0f;
        rArg.m_fUnk80 = 0.1f;
        rArg.m_fUnk84 = 0.4f;
        rArg.m_nTailEffect = EFFECT_TAIL_GSHELL;
        rArg.m_nSe = SE_ITEM_SPIKE;
    }

    void CItemBallVolleyGShell::MainFunc_()
    {
    }

    long CItemBallVolleyGShell::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    long CItemBallVolleyGShell::TrgChrHit_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    long CItemBallVolleyGShell::TrgGoalIn_()
    {
        long nSe = SE_SHELL_BREAK;
        if (IsShot()) {
            nSe = SE_SHELL_BREAK_SHOT;
        }
        BreakShell(this, 0, nSe);
        return true;
    }

    long CItemBallVolleyGShell::TrgFreeMode_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    // Returns 4 when the ball has not been shot.
    long CItemBallVolleyGShell::TrgDamage_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return 4;
    }

    long CItemBallVolleyGShell::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    long CItemBallVolleyGShell::TrgReceive_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    long CItemBallVolleyGShell::TrgBlock_()
    {
        if (IsShot()) {
            BreakShell(this, 0, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    unsigned long CItemBallVolleyGShell::GetVolleyReceiveReaction(long, unsigned long reaction) const
    {
        static const unsigned long uRet = 0x40907;
        if (IsShot()) {
            return uRet;
        }
        return reaction;
    }

    unsigned long CItemBallVolleyGShell::GetVolleyBlockReaction(long nKind, unsigned long reaction) const
    {
        static const unsigned long uRet = 6;
        if (IsShot()) {
            if (nKind == 5) {
                return 1;
            }
            return uRet;
        }
        return reaction;
    }

    float CItemBallVolleyGShell::GetVolleyFlickDownPower(long, float power) const
    {
        if (IsShot()) {
            return 20.0f;
        }
        return power;
    }

    long CItemBallVolleyGShell::GetVolleyPassTailEffect(long, long, long) const
    {
        return -1;
    }

    long CItemBallVolleyGShell::GetVolleySpikeTailEffect(long, long) const
    {
        if (IsShot()) {
            return EFFECT_TAIL_GSHELL;
        }
        return -1;
    }

    // Red shell ------------------------------------------------------------

    CItemBallVolleyRShell::CItemBallVolleyRShell(CBall *pBall)
        : CItemBallVolley(pBall, 1)
    {
        SetMdlScale(m_pMdl, ITEM_SCALE);
    }

    CItemBallVolleyRShell::~CItemBallVolleyRShell()
    {
    }

    void CItemBallVolleyRShell::GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg) const
    {
        rArg.m_nUnk0C = 3;
        rArg.m_nUnk10 = 0;
        rArg.m_nUnk14 = 0;
        rArg.m_nUnk20 = 0;
        rArg.m_fUnk28 = 0.0f;
        rArg.m_fUnk58 = 12.0f;
        rArg.m_fUnk5C = 20.0f;
        rArg.m_fUnk60 = 25.0f;
        rArg.m_fUnk68 = 0.8f;
        rArg.m_fUnk6C = 19.0f;
        rArg.m_fUnk7C = 0.0f;
        rArg.m_fUnk80 = 0.1f;
        rArg.m_fUnk84 = 0.4f;
        rArg.m_nTailEffect = EFFECT_TAIL_RSHELL;
        rArg.m_nSe = SE_ITEM_SPIKE;
        ChooseSpikeSide(rArg);
        rArg.m_fUnk88 = 0.05f;
        rArg.m_fUnk8C = 2.2f;
        rArg.m_fUnk90 = 2.2f;
        rArg.m_fUnk94 = 0.4f;
        rArg.m_fUnk98 = 3.0f;
        rArg.m_fUnk9C = 3.0f;
    }

    void CItemBallVolleyRShell::MainFunc_()
    {
    }

    long CItemBallVolleyRShell::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    long CItemBallVolleyRShell::TrgChrHit_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    long CItemBallVolleyRShell::TrgGoalIn_()
    {
        long nSe = SE_SHELL_BREAK;
        if (IsShot()) {
            nSe = SE_SHELL_BREAK_SHOT;
        }
        BreakShell(this, 1, nSe);
        return true;
    }

    long CItemBallVolleyRShell::TrgFreeMode_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    long CItemBallVolleyRShell::TrgDamage_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return 4;
    }

    long CItemBallVolleyRShell::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    long CItemBallVolleyRShell::TrgReceive_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    long CItemBallVolleyRShell::TrgBlock_()
    {
        if (IsShot()) {
            BreakShell(this, 1, SE_SHELL_BREAK);
            return true;
        }
        return false;
    }

    unsigned long CItemBallVolleyRShell::GetVolleyReceiveReaction(long, unsigned long reaction) const
    {
        static const unsigned long uRet = 0x908;
        if (IsShot()) {
            return uRet;
        }
        return reaction;
    }

    unsigned long CItemBallVolleyRShell::GetVolleyBlockReaction(long nKind, unsigned long reaction) const
    {
        static const unsigned long uRet = 6;
        if (IsShot()) {
            if (nKind == 5) {
                return 1;
            }
            return uRet;
        }
        return reaction;
    }

    float CItemBallVolleyRShell::GetVolleyFlickDownPower(long, float power) const
    {
        if (IsShot()) {
            return 20.0f;
        }
        return power;
    }

    long CItemBallVolleyRShell::GetVolleyPassTailEffect(long, long, long) const
    {
        return -1;
    }

    long CItemBallVolleyRShell::GetVolleySpikeTailEffect(long, long) const
    {
        if (IsShot()) {
            return EFFECT_TAIL_RSHELL;
        }
        return -1;
    }

    // Bomb -----------------------------------------------------------------

    inline void CItemBallVolleyBomb::StartFuse_()
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

    inline void CItemBallVolleyBomb::StopFuse_()
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

    inline void CItemBallVolleyBomb::Explode_(int bAttach)
    {
        if (m_bExploded == 0) {
            StopFuse_();
            CBall *pBall = GetBall();
            if (pBall != 0) {
                unsigned long nHandle = bAttach ? pBall->GetMdlObj()->GetHandle() : 0;
                bcs::FVEC3 pos = pBall->GetPos();
                float fRate = 1.0f;
                new (0x100) CBombExplosion(pBall, pos, nHandle, 120.0f, GetExplosionCollSize(), fRate, 1, 10, 0);
            }
            m_bExploded = 1;
        }
    }

    CItemBallVolleyBomb::CItemBallVolleyBomb(CBall *pBall)
        : CItemBallVolley(pBall, 3), m_bExploded(0), m_bShot(0), m_fTime(60.0f), m_fTimer(60.0f), m_fRate(1.0f),
          m_fUnk78(0.0f), m_fUnk7C(0.0f), m_hEffect(0), m_hSe(0)
    {
        SetMdlScale(m_pMdl, BOMB_SCALE);
        StartFuse_();
    }

    CItemBallVolleyBomb::~CItemBallVolleyBomb()
    {
        StopFuse_();
    }

    float CItemBallVolleyBomb::GetExplosionCollSize()
    {
        return 4.5f;
    }

    void CItemBallVolleyBomb::GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg) const
    {
        rArg.m_nUnk0C = 0;
        rArg.m_nUnk10 = 0;
        rArg.m_nUnk18 = 0;
        rArg.m_nUnk14 = 0;
        rArg.m_nUnk20 = 2;
        rArg.m_fUnk58 = 6.0f;
        rArg.m_fUnk5C = 12.0f;
        rArg.m_fUnk60 = 14.0f;
        rArg.m_fUnk68 = 0.8f;
        rArg.m_fUnk6C = 40.0f;
        rArg.m_fUnk7C = 0.0f;
        rArg.m_fUnk80 = 1.1f;
        rArg.m_fUnk84 = 1.4f;
        rArg.m_nTailEffect = EFFECT_TAIL_BOMB;
    }

    void CItemBallVolleyBomb::MainFunc_()
    {
    }

    // The first call only marks the bomb as shot; any later one sets it off.
    long CItemBallVolleyBomb::TrgShootStart_()
    {
        if (m_bShot != 0) {
            Explode_(1);
            return true;
        }
        m_bShot = 1;
        return CItemBall::TrgShootStart_();
    }

    long CItemBallVolleyBomb::TrgReceive_()
    {
        if (IsShot()) {
            Explode_(1);
            return true;
        }
        return false;
    }

    long CItemBallVolleyBomb::TrgBlock_()
    {
        if (IsShot()) {
            Explode_(1);
            return true;
        }
        return false;
    }

    long CItemBallVolleyBomb::TrgGoalIn_()
    {
        Explode_(1);
        return true;
    }

    long CItemBallVolleyBomb::TrgDamage_()
    {
        if (IsShot()) {
            Explode_(0);
            return true;
        }
        return 4;
    }

    long CItemBallVolleyBomb::GetVolleyPassTailEffect(long, long, long) const
    {
        return -1;
    }

    long CItemBallVolleyBomb::GetVolleySpikeTailEffect(long, long) const
    {
        return EFFECT_TAIL_BOMB;
    }

    unsigned long CItemBallVolleyBomb::GetVolleyReceiveReaction(long, unsigned long reaction) const
    {
        static const unsigned long uRet = 0x10000;
        if (IsShot()) {
            return uRet;
        }
        return reaction;
    }

    unsigned long CItemBallVolleyBomb::GetVolleyBlockReaction(long nKind, unsigned long reaction) const
    {
        static const unsigned long uRet = 10;
        UnusedReferences(m_fRate);
        if (IsShot()) {
            if (nKind == 5) {
                return 1;
            }
            return uRet;
        }
        return reaction;
    }

    // Mushroom ball (stripped by the link, see the header) ------------------

    unsigned long CItemBallVolleyMushroom::GetVolleyReceiveReaction(long, unsigned long reaction) const
    {
        static const unsigned long uRet = 0x40903;
        if (IsShot()) {
            return uRet;
        }
        return reaction;
    }

    unsigned long CItemBallVolleyMushroom::GetVolleyBlockReaction(long nKind, unsigned long reaction) const
    {
        static const unsigned long uRet = 6;
        if (IsShot()) {
            if (nKind == 5) {
                return 1;
            }
            return uRet;
        }
        return reaction;
    }

    // Mushroom -------------------------------------------------------------

    CItemBallVolleyToadstool::CItemBallVolleyToadstool(CBall *pBall)
        : CItemBallVolley(pBall, 2)
    {
    }

    CItemBallVolleyToadstool::~CItemBallVolleyToadstool()
    {
    }

    float CItemBallVolleyToadstool::GetScale() const
    {
        return 0.55f;
    }

    void CItemBallVolleyToadstool::GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg) const
    {
        rArg.m_nUnk0C = 0;
        rArg.m_nUnk10 = 0;
        rArg.m_nUnk18 = 0;
        rArg.m_nUnk14 = 0;
        rArg.m_nUnk20 = 0;
        rArg.m_fUnk58 = 15.0f;
        rArg.m_fUnk5C = 25.0f;
        rArg.m_fUnk60 = 30.0f;
        rArg.m_fUnk68 = 0.8f;
        rArg.m_fUnk6C = 19.0f;
        rArg.m_fUnk7C = 0.0f;
        rArg.m_fUnk80 = 0.1f;
        rArg.m_fUnk84 = 0.4f;
        rArg.m_nTailEffect = EFFECT_TAIL_TOADSTOOL;
    }

    void CItemBallVolleyToadstool::MainFunc_()
    {
    }

    long CItemBallVolleyToadstool::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    long CItemBallVolleyToadstool::TrgChrHit_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return true;
    }

    long CItemBallVolleyToadstool::TrgGoalIn_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return true;
    }

    long CItemBallVolleyToadstool::TrgFreeMode_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return true;
    }

    long CItemBallVolleyToadstool::TrgDamage_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return true;
    }

    long CItemBallVolleyToadstool::TrgDestroy_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return CItemBall::TrgDestroy_();
    }

    long CItemBallVolleyToadstool::TrgReceive_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return true;
    }

    long CItemBallVolleyToadstool::TrgBlock_()
    {
        m_nTrgFlag &= ~(1 << (TRG_SHOOT_START + 1));
        return true;
    }

    // Always overrides the reaction; the lowest bit only once shot.
    unsigned long CItemBallVolleyToadstool::GetVolleyReceiveReaction(long, unsigned long) const
    {
        static const unsigned long uRet = 0x44105;
        if (IsShot()) {
            return uRet;
        }
        return uRet & ~1;
    }

    unsigned long CItemBallVolleyToadstool::GetVolleyBlockReaction(long nKind, unsigned long reaction) const
    {
        static const unsigned long uRet = 0x12;
        if (IsShot()) {
            if (nKind == 5) {
                return 1;
            }
            return uRet;
        }
        return reaction;
    }

    long CItemBallVolleyToadstool::GetVolleyPassParam(long param, long) const
    {
        if (param < 4) {
            return 4;
        }
        return param;
    }

    long CItemBallVolleyToadstool::GetVolleyChanceBallParam(long param) const
    {
        if (param < 4) {
            return 4;
        }
        return param;
    }

    long CItemBallVolleyToadstool::GetVolleySpikeTailEffect(long, long) const
    {
        return EFFECT_TAIL_TOADSTOOL;
    }

    long CItemBallVolleyToadstool::GetVolleyPassTailEffect(long, long, long) const
    {
        return -1;
    }

    // Star -----------------------------------------------------------------

    CItemBallVolleyStar::CItemBallVolleyStar(CBall *pBall)
        : CItemBallVolley(pBall, 4)
    {
        SetMdlScale(m_pMdl, STAR_SCALE);
    }

    CItemBallVolleyStar::~CItemBallVolleyStar()
    {
    }

    float CItemBallVolleyStar::GetScale() const
    {
        return STAR_SCALE;
    }

    void CItemBallVolleyStar::GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg) const
    {
        rArg.m_nUnk0C = 0;
        rArg.m_nUnk10 = 0;
        rArg.m_nUnk18 = 0;
        rArg.m_nUnk14 = 0;
        rArg.m_nUnk20 = 1;
        rArg.m_nUnk24 = 1;
        rArg.m_fUnk58 = 30.0f;
        rArg.m_fUnk5C = 53.0f;
        rArg.m_fUnk60 = 53.0f;
        rArg.m_fUnk68 = 0.8f;
        rArg.m_fUnk6C = 15.0f;
        rArg.m_fUnk7C = 0.0f;
        rArg.m_fUnk80 = 0.1f;
        rArg.m_fUnk84 = 0.4f;
        rArg.m_nTailEffect = EFFECT_TAIL_STAR + 1;
    }

    void CItemBallVolleyStar::MainFunc_()
    {
    }

    long CItemBallVolleyStar::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    long CItemBallVolleyStar::TrgChrHit_()
    {
        return true;
    }

    long CItemBallVolleyStar::TrgGoalIn_()
    {
        return true;
    }

    long CItemBallVolleyStar::TrgFreeMode_()
    {
        return true;
    }

    long CItemBallVolleyStar::TrgDamage_()
    {
        return true;
    }

    long CItemBallVolleyStar::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    long CItemBallVolleyStar::TrgReceive_()
    {
        return true;
    }

    long CItemBallVolleyStar::TrgBlock_()
    {
        return true;
    }

    unsigned long CItemBallVolleyStar::GetVolleyReceiveReaction(long, unsigned long reaction) const
    {
        static const unsigned long uRet = 0x48103;
        if (IsShot()) {
            return uRet;
        }
        return reaction;
    }

    unsigned long CItemBallVolleyStar::GetVolleyBlockReaction(long nKind, unsigned long reaction) const
    {
        static const unsigned long uRet = 0x25;
        if (IsShot()) {
            if (nKind == 5) {
                return 1;
            }
            return uRet;
        }
        return reaction;
    }

    long CItemBallVolleyStar::GetVolleyPassTailEffect(long, long, long) const
    {
        return EFFECT_TAIL_STAR;
    }

    // The next effect once shot.
    long CItemBallVolleyStar::GetVolleySpikeTailEffect(long, long) const
    {
        return IsShot() ? EFFECT_TAIL_STAR + 1 : EFFECT_TAIL_STAR;
    }

    float CItemBallVolleyStar::GetVolleyFlickDownPower(long, float) const
    {
        return 180.0f;
    }

    // Banana ---------------------------------------------------------------

    CItemBallVolleyBanana::CItemBallVolleyBanana(CBall *pBall)
        : CItemBallVolley(pBall, 5)
    {
        SetMdlScale(m_pMdl, ITEM_SCALE);
    }

    CItemBallVolleyBanana::~CItemBallVolleyBanana()
    {
    }

    void CItemBallVolleyBanana::GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg) const
    {
        rArg.m_nUnk0C = 3;
        rArg.m_nUnk10 = 0;
        rArg.m_nUnk14 = 0;
        rArg.m_nUnk20 = 0;
        rArg.m_fUnk28 = 0.0f;
        rArg.m_fUnk58 = 12.0f;
        rArg.m_fUnk5C = 21.0f;
        rArg.m_fUnk60 = 26.0f;
        rArg.m_fUnk68 = 0.8f;
        rArg.m_fUnk6C = 19.0f;
        rArg.m_fUnk7C = 0.0f;
        rArg.m_fUnk80 = 0.1f;
        rArg.m_fUnk84 = 0.4f;
        rArg.m_nTailEffect = EFFECT_TAIL_BANANA;
        rArg.m_nSe = SE_ITEM_SPIKE;
        ChooseSpikeSide(rArg);
        rArg.m_fUnk88 = 0.0f;
        rArg.m_fUnk8C = 3.0f;
        rArg.m_fUnk90 = 3.0f;
        rArg.m_fUnk94 = 0.0f;
        rArg.m_fUnk98 = 0.0f;
        rArg.m_fUnk9C = 0.0f;
    }

    void CItemBallVolleyBanana::MainFunc_()
    {
    }

    long CItemBallVolleyBanana::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    long CItemBallVolleyBanana::TrgChrHit_()
    {
        return IsShot() != 0;
    }

    long CItemBallVolleyBanana::TrgGoalIn_()
    {
        return true;
    }

    long CItemBallVolleyBanana::TrgFreeMode_()
    {
        return IsShot() != 0;
    }

    long CItemBallVolleyBanana::TrgDamage_()
    {
        if (IsShot()) {
            return true;
        }
        return 4;
    }

    long CItemBallVolleyBanana::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    long CItemBallVolleyBanana::TrgReceive_()
    {
        return IsShot() != 0;
    }

    long CItemBallVolleyBanana::TrgBlock_()
    {
        return IsShot() != 0;
    }

    unsigned long CItemBallVolleyBanana::GetVolleyReceiveReaction(long, unsigned long reaction) const
    {
        static const unsigned long uRet = 0x40106;
        if (IsShot()) {
            return uRet;
        }
        return reaction;
    }

    unsigned long CItemBallVolleyBanana::GetVolleyBlockReaction(long nKind, unsigned long reaction) const
    {
        static const unsigned long uRet = 0x82;
        if (IsShot()) {
            if (nKind == 5) {
                return 1;
            }
            return uRet;
        }
        return reaction;
    }

    float CItemBallVolleyBanana::GetVolleyFlickDownPower(long, float power) const
    {
        if (IsShot()) {
            return 12.0f;
        }
        return power;
    }

    long CItemBallVolleyBanana::GetVolleyPassTailEffect(long, long, long) const
    {
        return EFFECT_TAIL_BANANA;
    }

    long CItemBallVolleyBanana::GetVolleySpikeTailEffect(long, long) const
    {
        return EFFECT_TAIL_BANANA;
    }

    long CItemBallVolleyBanana::TrgPreparationTouch_()
    {
        return IsShot() ? 0x40 : 0;
    }

    // Change effect --------------------------------------------------------

    CItemBallChangeEffectVolley::CItemBallChangeEffectVolley(bcs::CTask *pParent, unsigned long nBallId)
        : CItemBallChangeEffect(pParent, nBallId)
    {
    }

    void CItemBallChangeEffectVolley::SetStartEffect()
    {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall != 0) {
            CEffectCtrl::SimpleEffect(EFFECT_CHANGE_START, pBall->GetPos(), 3, 0, 0);
            CGameSnd::Play(SE_ITEM_CHANGE, pBall->GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    void CItemBallChangeEffectVolley::SetEndEffect()
    {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall != 0) {
            CEffectCtrl::SimpleEffect(EFFECT_CHANGE_END, pBall->GetPos(), 3, 0, 0);
        }
    }

    CItemBallChangeEffectVolley::~CItemBallChangeEffectVolley()
    {
    }
}
