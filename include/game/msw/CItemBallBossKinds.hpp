#ifndef MSW_CITEMBALLBOSSKINDS_HPP_
#define MSW_CITEMBALLBOSSKINDS_HPP_

#include "game/msw/CItemBallBoss.hpp"
#include "game/msw/CItemBallChangeEffect.hpp"

// The item behaviours a ball can take in the boss game (BossF), one class per
// item kind, and the boss game's change effect. All are defined in
// game/boss/CItemBallBossBomb.cpp. HockeyF and VolleyF have the same set
// (CItemBallHockey*, CItemBallVolley*).
//
// The kind passed to CItemBall's constructor selects the model: 0 green
// shell, 1 red shell, 2 mushroom, 3 bomb, 4 star, 5 banana.
//
// Trigger results: the shells and the banana return 4 from TrgDamage_ when
// the ball has not been shot. The Trg*_ functions are declared bool in
// CItemBall, so 4 cannot be returned yet; see TrgDamage_ in the .cpp.

namespace msw {

    // A green shell: breaks on the first hit after it has been shot.
    class CItemBallBossGShell : public CItemBallBoss {
        public:
            CItemBallBossGShell(CBall *pBall);
            virtual ~CItemBallBossGShell();

            virtual long GetVolleySpikeTailEffect(long effect, long) const;
            virtual long GetVolleyPassTailEffect(long effect, long, long) const;

            virtual void MainFunc_();
            virtual long TrgShootStart_();
            virtual long TrgGoalIn_();
            virtual long TrgFreeMode_();
            virtual long TrgChrHit_();
            virtual long TrgDamage_();
            virtual long TrgDestroy_();
            virtual long TrgReceive_();
            virtual long TrgBlock_();
    };

    // A red shell: as the green one.
    class CItemBallBossRShell : public CItemBallBoss {
        public:
            CItemBallBossRShell(CBall *pBall);
            virtual ~CItemBallBossRShell();

            virtual long GetVolleySpikeTailEffect(long effect, long) const;
            virtual long GetVolleyPassTailEffect(long effect, long, long) const;

            virtual void MainFunc_();
            virtual long TrgShootStart_();
            virtual long TrgGoalIn_();
            virtual long TrgFreeMode_();
            virtual long TrgChrHit_();
            virtual long TrgDamage_();
            virtual long TrgDestroy_();
            virtual long TrgReceive_();
            virtual long TrgBlock_();
    };

    // A bomb: lit (fuse effect and sound) from the start, explodes on the
    // first trigger after it has been shot, or when it reaches the goal.
    class CItemBallBossBomb : public CItemBallBoss {
        public:
            CItemBallBossBomb(CBall *pBall);
            virtual ~CItemBallBossBomb();

            virtual long GetVolleySpikeTailEffect(long effect, long) const;
            virtual long GetVolleyPassTailEffect(long effect, long, long) const;

            virtual void MainFunc_();
            virtual long TrgShootStart_();
            virtual long TrgGoalIn_();
            virtual long TrgDamage_();
            virtual long TrgReceive_();
            virtual long TrgBlock_();
            virtual long TrgPreparationTouch_();

        private:
            // Starts the fuse effect and sound (once the model exists).
            inline void StartFuse_();
            // Stops the fuse effect and sound.
            inline void StopFuse_();
            // Stops the fuse and sets off the blast at the ball, once. With
            // bAttach the blast gets the handle of the ball's model (all
            // triggers but TrgDamage_).
            inline void Explode_(int bAttach);

            int m_bExploded;          // 0x64
            int m_bShot;              // 0x68, set by the first TrgShootStart_
            // 0x6C to 0x7C: set by the constructor, not read in this file.
            // (names guessed)
            float m_fTime;            // 0x6C
            float m_fTimer;           // 0x70
            float m_fRate;            // 0x74
            float m_fUnk78;           // 0x78
            float m_fUnk7C;           // 0x7C
            unsigned long m_hEffect;  // 0x80, the fuse effect (CEffectCtrl handle)
            unsigned long m_hSe;      // 0x84, the fuse sound (sound task id)
    };

    // A mushroom: drawn larger; every trigger clears the shot flag.
    class CItemBallBossToadstool : public CItemBallBoss {
        public:
            CItemBallBossToadstool(CBall *pBall);
            virtual ~CItemBallBossToadstool();

            virtual float GetScale() const;
            virtual long GetVolleySpikeTailEffect(long effect, long) const;
            virtual long GetVolleyPassTailEffect(long effect, long, long) const;

            virtual void MainFunc_();
            virtual long TrgShootStart_();
            virtual long TrgGoalIn_();
            virtual long TrgFreeMode_();
            virtual long TrgChrHit_();
            virtual long TrgDamage_();
            virtual long TrgDestroy_();
            virtual long TrgReceive_();
            virtual long TrgBlock_();
    };

    // A star.
    class CItemBallBossStar : public CItemBallBoss {
        public:
            CItemBallBossStar(CBall *pBall);
            virtual ~CItemBallBossStar();

            virtual float GetScale() const;
            virtual float GetVolleyFlickDownPower(long, float power) const;
            virtual long GetVolleySpikeTailEffect(long effect, long) const;
            virtual long GetVolleyPassTailEffect(long effect, long, long) const;

            virtual void MainFunc_();
            virtual long TrgShootStart_();
            virtual long TrgGoalIn_();
            virtual long TrgFreeMode_();
            virtual long TrgChrHit_();
            virtual long TrgDamage_();
            virtual long TrgDestroy_();
            virtual long TrgReceive_();
            virtual long TrgBlock_();
    };

    // A banana.
    class CItemBallBossBanana : public CItemBallBoss {
        public:
            CItemBallBossBanana(CBall *pBall);
            virtual ~CItemBallBossBanana();

            virtual long GetVolleySpikeTailEffect(long effect, long) const;
            virtual long GetVolleyPassTailEffect(long effect, long, long) const;

            virtual void MainFunc_();
            virtual long TrgShootStart_();
            virtual long TrgGoalIn_();
            virtual long TrgFreeMode_();
            virtual long TrgChrHit_();
            virtual long TrgDamage_();
            virtual long TrgDestroy_();
            virtual long TrgReceive_();
            virtual long TrgBlock_();
    };

    // The change effect of the boss game: plays the change sound at the ball.
    class CItemBallChangeEffectBoss : public CItemBallChangeEffect {
        public:
            CItemBallChangeEffectBoss(bcs::CTask *pParent, unsigned long nBallId);
            virtual ~CItemBallChangeEffectBoss();
            virtual void SetStartEffect();
            virtual void SetEndEffect();
    };
}

#endif /* !MSW_CITEMBALLBOSSKINDS_HPP_ */
