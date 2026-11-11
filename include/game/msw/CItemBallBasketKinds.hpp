#ifndef MSW_CITEMBALLBASKETKINDS_HPP_
#define MSW_CITEMBALLBASKETKINDS_HPP_

#include "game/msw/CItemBallBasket.hpp"
#include "game/msw/CItemBallChangeEffect.hpp"

// The item behaviours a ball can take in the basket game (BasketF), one class
// per item kind, and the basket game's change effect. All are defined in
// game/basket/CItemBallBasketBomb.cpp.
//
// The kind passed to CItemBall's constructor selects the model: 0 green
// shell, 1 red shell, 2 mushroom, 3 bomb, 4 star, 5 banana.
//
// TrgChrHit_ returns 5 (shells, mushroom), 4 (star) or 1 (banana) in the
// original. CItemBall declares its triggers bool, so those values cannot be
// returned yet; see TrgChrHit_ in the .cpp.

namespace msw {

    // A green shell.
    class CItemBallBasketGShell : public CItemBallBasket {
        public:
            CItemBallBasketGShell(CBall *pBall);
            virtual ~CItemBallBasketGShell();

            virtual long GetJumpShootTailEffect() const;
            virtual bool TrgChrHit_();
            virtual float GetDispScale() const;
            virtual void GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const;
    };

    // A red shell.
    class CItemBallBasketRShell : public CItemBallBasket {
        public:
            CItemBallBasketRShell(CBall *pBall);
            virtual ~CItemBallBasketRShell();

            virtual long GetJumpShootTailEffect() const;
            virtual bool TrgChrHit_();
            virtual float GetDispScale() const;
            virtual void GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const;
    };

    // A bomb: lit (fuse effect and sound) from the start, it flashes red
    // and swells for the last part of a random fuse time and goes off at
    // the end of it, or when the ball item is destroyed while the ball is
    // not held.
    class CItemBallBasketBomb : public CItemBallBasket {
        public:
            CItemBallBasketBomb(CBall *pBall);
            virtual ~CItemBallBasketBomb();

            virtual void MainFunc_();
            virtual bool TrgGoalIn_();
            virtual bool TrgChrHit_();
            virtual bool TrgDestroy_();
            virtual float GetDispScale() const;
            virtual void GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const;
            virtual void SetExplodeCenterPos(const bcs::FVEC3 &rPos) { m_vExplodeCenterPos = rPos; }
            virtual bcs::FVEC3 GetExplodeCenterPos() const { return m_vExplodeCenterPos; }
            virtual float GetExplodeR() const;

        private:
            // Starts the fuse effect and sound (once the model exists).
            inline void StartFuse_();
            // Stops the fuse effect and sound.
            inline void StopFuse_();
            // Sets off the blast at the ball, unless TrgDestroy_ has run.
            inline void Explode_();

            unsigned long m_hEffect;     // 0x64, the fuse effect (CEffectCtrl handle)
            unsigned long m_hSe;         // 0x68, the fuse sound (sound task id)
            float m_fFuseTime;           // 0x6C, drawn at construction (names guessed)
            float m_fFuseTimer;          // 0x70, counts down to the blast
            float m_fFlashTime;          // 0x74, flashing starts below this
            float m_fFlashPhase;         // 0x78, in [0, 1)
            float m_fSwellPhase;         // 0x7C, radians
            bcs::FVEC3 m_vExplodeCenterPos; // 0x80
    };

    // A star.
    class CItemBallBasketStar : public CItemBallBasket {
        public:
            CItemBallBasketStar(CBall *pBall);
            virtual ~CItemBallBasketStar();

            virtual long GetJumpShootTailEffect() const;
            virtual bool TrgChrHit_();
            virtual float GetDispScale() const;
            virtual void GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const;
    };

    // A mushroom.
    class CItemBallBasketToadstool : public CItemBallBasket {
        public:
            CItemBallBasketToadstool(CBall *pBall);
            virtual ~CItemBallBasketToadstool();

            virtual bool IsDisableRebound() const { return false; }
            virtual long GetJumpShootTailEffect() const;
            virtual bool TrgChrHit_();
            virtual float GetDispScale() const;
            virtual void GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const;
    };

    // A banana.
    class CItemBallBasketBanana : public CItemBallBasket {
        public:
            CItemBallBasketBanana(CBall *pBall);
            virtual ~CItemBallBasketBanana();

            virtual long GetJumpShootTailEffect() const;
            virtual bool TrgChrHit_();
            virtual float GetDispScale() const;
            virtual void GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const;
    };

    // The change effect of the basket game: an effect at the ball at the
    // start and at the end, and the change sound at the start.
    class CItemBallChangeEffectBasket : public CItemBallChangeEffect {
        public:
            CItemBallChangeEffectBasket(bcs::CTask *pParent, unsigned long nBallId);
            virtual ~CItemBallChangeEffectBasket();
            virtual void SetStartEffect();
            virtual void SetEndEffect();
    };
}

#endif /* !MSW_CITEMBALLBASKETKINDS_HPP_ */
