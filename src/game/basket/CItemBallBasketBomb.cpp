#include "game/msw/CItemBallBasketKinds.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CBombExplosion.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "nw4r/math/triangular.h"
#include <math.h>

// The item behaviours of the basket game's balls (see
// CItemBallBasketKinds.hpp). One file: the kinds share their float
// constants. Also the bomb's blast (CBombExplosionBasket, used only here).

// Set by the file's static initialiser and used by nothing in the file; most
// BasketF files have it (see game/basket/CBallManBasket.cpp).
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);
}

namespace msw {

    // The blast of the basket bomb: it does not hit the character who shot
    // the ball.
    class CBombExplosionBasket : public CBombExplosion {
        public:
            CBombExplosionBasket(CBall *pBall, const bcs::FVEC3 &rPos, unsigned long nMdlHandle, float fPower,
                                 float fRadius, float f3, long l1, long l2, unsigned long ul)
                : CBombExplosion(pBall, rPos, nMdlHandle, fPower, fRadius, f3, l1, l2, ul)
            {
                m_nShotChrIdx = -1;
                if (pBall != 0) {
                    m_nShotChrIdx = pBall->GetShotChrIdx();
                }
            }
            virtual ~CBombExplosionBasket() {}

            // The bit of the shooter (characters 0 to 5), or none.
            virtual unsigned long GetNoHitFlag() const
            {
                return (unsigned long)m_nShotChrIdx <= 5 ? 1 << m_nShotChrIdx : 0;
            }

        private:
            long m_nShotChrIdx; // 0x7C
    };
}

namespace {

    // Sounds and effects.
    const long SE_BOMB_FUSE = 0xC4F;
    const long SE_ITEM_CHANGE = 0xC41;
    const long EFFECT_BOMB_FUSE = 4;
    const long EFFECT_CHANGE_START = 0x4EC;
    const long EFFECT_CHANGE_END = 0x4ED;

    // rad brought into [0, 2 pi].
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

    inline void SetMdlScale(bcs::CMdlObj *pMdl, float scale)
    {
        pMdl->m_vScale.x = scale;
        pMdl->m_vScale.y = scale;
        pMdl->m_vScale.z = scale;
    }

    inline float Clamp(float value, float min, float max)
    {
        return value > max ? max : (value < min ? min : value);
    }
}

namespace msw {

    // Base -----------------------------------------------------------------

    void CItemBallBasket::MainFunc_()
    {
        // (guess) An unused local: the original's .rodata starts with 1.0f,
        // ahead of GetPower's constants, so some code before GetPower
        // mentioned it; which code is not known. Behaviour-free (see the
        // dead float comparison ruling in notes/decisions.md).
        float fUnused = 1.0f;
        (void)fUnused;
    }

    long CItemBallBasket::TrgFreeMode_()
    {
        return !IsDestroyed();
    }

    long CItemBallBasket::DoTrg(long trg)
    {
        return CItemBall::DoTrg(trg);
    }

    float CItemBallBasket::GetPower() const
    {
        float fPower;
        switch (GetType()) {
            case 3:
                fPower = 80.0f;
                break;
            case 0:
                fPower = 30.0f;
                break;
            case 1:
                fPower = 30.0f;
                break;
            case 2:
                fPower = 0.0f;
                break;
            case 4:
                fPower = 30.0f;
                break;
            case 5:
                fPower = 10.0f;
                break;
            default:
                fPower = 0.0f;
                break;
        }
        return fPower;
    }

    float CItemBallBasket::GetCurveValue(long type)
    {
        switch (type) {
            case 1:
                return 3.3f;
            case 5:
                return 1.5f;
            default:
                return 0.0f;
        }
    }

    // Green shell ----------------------------------------------------------

    CItemBallBasketGShell::CItemBallBasketGShell(CBall *pBall)
        : CItemBallBasket(pBall, 0)
    {
    }

    CItemBallBasketGShell::~CItemBallBasketGShell()
    {
    }

    float CItemBallBasketGShell::GetDispScale() const
    {
        return 1.4f;
    }

    // Aimed at half the height between the ball and the goal.
    void CItemBallBasketGShell::GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const
    {
        rParam.m_nUnk00 = 1;
        rParam.m_nUnk04 = 0;
        rParam.m_fUnk08 = 1.0f;
        rParam.m_fUnk0C = 18.0f;
        CBall *pBall = GetBall();
        const bcs::FVEC3 &rGoal = CGame::GetGoalPos(pBall->GetTargetGoalIdx());
        float y = (rGoal.y - pBall->GetPos().y) * 0.5f + pBall->GetPos().y;
        rParam.m_fUnk10 = y;
        rParam.m_fUnk14 = y;
        rParam.m_fUnk18 = 15.0f;
        rParam.m_fUnk1C = 400.0f;
        rParam.m_fCurve = 0.0f;
    }

    long CItemBallBasketGShell::GetJumpShootTailEffect() const
    {
        return 0x4F3;
    }

    long CItemBallBasketGShell::TrgChrHit_()
    {
        return 5;
    }

    // Red shell ------------------------------------------------------------

    CItemBallBasketRShell::CItemBallBasketRShell(CBall *pBall)
        : CItemBallBasket(pBall, 1)
    {
    }

    CItemBallBasketRShell::~CItemBallBasketRShell()
    {
    }

    float CItemBallBasketRShell::GetDispScale() const
    {
        return 1.4f;
    }

    long CItemBallBasketRShell::TrgChrHit_()
    {
        return 5;
    }

    void CItemBallBasketRShell::GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const
    {
        rParam.m_nUnk00 = 0;
        rParam.m_nUnk04 = 1;
        rParam.m_fUnk08 = 1.0f;
        rParam.m_fUnk0C = 9.0f;
        rParam.m_fUnk10 = 5.0f;
        rParam.m_fUnk14 = 5.0f;
        rParam.m_fUnk18 = 50.0f;
        rParam.m_fUnk1C = 400.0f;
        rParam.m_fCurve = GetCurveValue(GetType());
    }

    long CItemBallBasketRShell::GetJumpShootTailEffect() const
    {
        return 0x4F4;
    }

    // Bomb -----------------------------------------------------------------

    inline void CItemBallBasketBomb::StartFuse_()
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

    inline void CItemBallBasketBomb::StopFuse_()
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

    inline void CItemBallBasketBomb::Explode_()
    {
        if (!IsDestroyed()) {
            CBall *pBall = GetBall();
            if (pBall != 0) {
                unsigned long nMdlHandle = pBall->GetMdlObj()->GetHandle();
                new (0) CBombExplosionBasket(pBall, pBall->GetPos(), nMdlHandle, GetPower(), GetExplodeR(), 0.0f,
                                             0, 0, 0);
            }
        }
    }

    CItemBallBasketBomb::CItemBallBasketBomb(CBall *pBall)
        : CItemBallBasket(pBall, 3), m_hEffect(0), m_hSe(0), m_fFuseTime(60.0f), m_fFuseTimer(60.0f),
          m_fFlashTime(1.0f), m_fFlashPhase(0.0f), m_fSwellPhase(0.0f), m_vExplodeCenterPos(0.0f, 0.0f, 0.0f)
    {
        // The fuse burns for a random time; the bounds are sorted first.
        float fMin = 1800.0f;
        float fMax = 24000.0f;
        if (fMin > fMax) {
            float fTmp = fMin;
            fMin = fMax;
            fMax = fTmp;
        }
        float fTime = floor(bcs::BCS_RandRange(fMin, fMax));
        m_fFuseTime = fTime;
        m_fFuseTimer = fTime;
        m_fFlashTime = m_fFuseTime * bcs::BCS_RandRange(0.3f, 0.7f);
        StartFuse_();
    }

    CItemBallBasketBomb::~CItemBallBasketBomb()
    {
        StopFuse_();
    }

    void CItemBallBasketBomb::MainFunc_()
    {
        if (IsDestroyed()) {
            return;
        }

        m_fFuseTimer -= GetFrameRateF();
        if (m_fFuseTimer <= 0.0f) {
            m_fFuseTimer = 0.0f;
            Explode_();
            static_cast<CBallBasket *>(GetBall())->DetachItemEffect(this);
        } else if (m_fFuseTimer < m_fFlashTime) {
            // Flash red, faster as the end comes near.
            m_fFlashPhase += (1.0f - (1.0f / 60.0f) * m_fFuseTimer) * (0.4f * GetFrameRateF());
            m_fFlashPhase = fmod(m_fFlashPhase, 1.0);
            // and swell and shrink.
            m_fSwellPhase += 1.5707964f * ((1.0f / 7.0f) * GetFrameRateF());
            m_fSwellPhase = WrapRad(m_fSwellPhase);

            float flash = Clamp(fabs(nw4r::math::SinRad(WrapRad(3.1415927f * m_fFlashPhase))), 0.0f, 1.0f);

            unsigned char level = (unsigned char)(255.0f - 200.0f * flash);
            bcs::BCS_Color base;
            base.r = base.g = base.b = level;
            base.a = 0xFF;
            if (m_pMdl != 0) {
                m_pMdl->SetSilhouetteColor(bcs::BCS_Color(0xFF2020FF));
                m_pMdl->SetBaseColor(base);

                float scale = GetScale();
                float swell = Clamp(nw4r::math::SinRad(m_fSwellPhase), -1.0f, 1.0f);
                SetMdlScale(m_pMdl, scale * (0.25f * swell + 0.5f) + 1.0f);
            }
        }
    }

    float CItemBallBasketBomb::GetDispScale() const
    {
        return 1.65f;
    }

    long CItemBallBasketBomb::TrgChrHit_()
    {
        return !IsDestroyed();
    }

    long CItemBallBasketBomb::TrgGoalIn_()
    {
        return !IsDestroyed();
    }

    // Goes off unless the ball is being held.
    long CItemBallBasketBomb::TrgDestroy_()
    {
        int bExplode = 1;
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall != 0 && pBall->IsAttach()) {
            bExplode = 0;
        }
        if (!IsDestroyed() && bExplode) {
            Explode_();
        }
        return CItemBall::TrgDestroy_();
    }

    void CItemBallBasketBomb::GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const
    {
        rParam.m_nUnk00 = 0;
        rParam.m_nUnk04 = 0;
        rParam.m_fUnk08 = 1.0f;
        rParam.m_fUnk0C = 12.0f;
        rParam.m_fUnk10 = 5.0f;
        rParam.m_fUnk14 = 5.0f;
        rParam.m_fUnk18 = 60.0f;
        rParam.m_fUnk1C = 400.0f;
        rParam.m_fCurve = 0.0f;
    }

    float CItemBallBasketBomb::GetExplodeR() const
    {
        return 3.5f;
    }

    // Star -----------------------------------------------------------------

    CItemBallBasketStar::CItemBallBasketStar(CBall *pBall)
        : CItemBallBasket(pBall, 4)
    {
    }

    CItemBallBasketStar::~CItemBallBasketStar()
    {
    }

    long CItemBallBasketStar::TrgChrHit_()
    {
        return 4;
    }

    float CItemBallBasketStar::GetDispScale() const
    {
        return 1.5f;
    }

    void CItemBallBasketStar::GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const
    {
        rParam.m_nUnk00 = 1;
        rParam.m_nUnk04 = 0;
        rParam.m_fUnk08 = 1.0f;
        rParam.m_fUnk0C = 37.0f;
        rParam.m_fUnk10 = 5.0f;
        rParam.m_fUnk14 = 5.0f;
        rParam.m_fUnk18 = 0.0f;
        rParam.m_fUnk1C = 0.0f;
        rParam.m_fCurve = 0.0f;
    }

    long CItemBallBasketStar::GetJumpShootTailEffect() const
    {
        return 0x4F6;
    }

    // Mushroom -------------------------------------------------------------

    CItemBallBasketToadstool::CItemBallBasketToadstool(CBall *pBall)
        : CItemBallBasket(pBall, 2)
    {
    }

    CItemBallBasketToadstool::~CItemBallBasketToadstool()
    {
    }

    long CItemBallBasketToadstool::TrgChrHit_()
    {
        return 5;
    }

    float CItemBallBasketToadstool::GetDispScale() const
    {
        return 0.5f;
    }

    void CItemBallBasketToadstool::GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const
    {
        rParam.m_nUnk00 = 0;
        rParam.m_nUnk04 = 0;
        rParam.m_fUnk08 = 0.0f;
        rParam.m_fUnk0C = 9.0f;
        rParam.m_fUnk10 = 4.2f;
        rParam.m_fUnk14 = 4.2f;
        rParam.m_fUnk18 = 35.0f;
        rParam.m_fUnk1C = 400.0f;
        rParam.m_fCurve = 0.0f;
    }

    long CItemBallBasketToadstool::GetJumpShootTailEffect() const
    {
        return 0x4F5;
    }

    // Banana ---------------------------------------------------------------

    CItemBallBasketBanana::CItemBallBasketBanana(CBall *pBall)
        : CItemBallBasket(pBall, 5)
    {
    }

    CItemBallBasketBanana::~CItemBallBasketBanana()
    {
    }

    float CItemBallBasketBanana::GetDispScale() const
    {
        return 1.0f;
    }

    long CItemBallBasketBanana::TrgChrHit_()
    {
        return true;
    }

    void CItemBallBasketBanana::GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const
    {
        rParam.m_nUnk00 = 0;
        rParam.m_nUnk04 = 1;
        rParam.m_fUnk08 = 1.0f;
        rParam.m_fUnk0C = 7.0f;
        rParam.m_fUnk10 = 5.0f;
        rParam.m_fUnk14 = 5.0f;
        rParam.m_fUnk18 = 50.0f;
        rParam.m_fUnk1C = 400.0f;
        rParam.m_fCurve = GetCurveValue(GetType());
    }

    long CItemBallBasketBanana::GetJumpShootTailEffect() const
    {
        return 0x4F7;
    }

    // Change effect --------------------------------------------------------

    CItemBallChangeEffectBasket::CItemBallChangeEffectBasket(bcs::CTask *pParent, unsigned long nBallId)
        : CItemBallChangeEffect(pParent, nBallId)
    {
    }

    void CItemBallChangeEffectBasket::SetStartEffect()
    {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall != 0) {
            CEffectCtrl::SimpleEffect(EFFECT_CHANGE_START, pBall->GetPos(), 3, 0, 0);
            CGameSnd::Play(SE_ITEM_CHANGE, pBall->GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    void CItemBallChangeEffectBasket::SetEndEffect()
    {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall != 0) {
            CEffectCtrl::SimpleEffect(EFFECT_CHANGE_END, pBall->GetPos(), 3, 0, 0);
        }
    }

    CItemBallChangeEffectBasket::~CItemBallChangeEffectBasket()
    {
    }
}
