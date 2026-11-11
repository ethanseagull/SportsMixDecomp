#ifndef MSW_CITEMBALLHOCKEYKINDS_HPP_
#define MSW_CITEMBALLHOCKEYKINDS_HPP_

#include "game/msw/CItemBallHockey.hpp"
#include "game/msw/CItemBallChangeEffect.hpp"

// The item behaviours a ball can take in the hockey game (HockeyF), one class
// per item kind, and the hockey game's change effect. All are defined in
// game/hockey/CItemBallHockeyBomb.cpp. The boss game has the same set
// (CItemBallBossKinds.hpp), with the volleyball triggers added.
//
// The kind passed to CItemBall's constructor selects the model: 0 green
// shell, 1 red shell, 2 mushroom, 3 bomb, 4 star, 5 banana.
//
// Trigger results: the shells and the banana return 4 from TrgDamage_ when
// the ball has not been shot. The Trg*_ functions are declared bool in
// CItemBall, so 4 cannot be returned yet; see TrgDamage_ in the .cpp.

namespace msw {

    // A green shell: breaks on the first hit after it has been shot.
    class CItemBallHockeyGShell : public CItemBallHockey {
        public:
            CItemBallHockeyGShell(CBall *pBall);
            virtual ~CItemBallHockeyGShell();

            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgGoalIn_();
            virtual bool TrgFreeMode_();
            virtual bool TrgChrHit_();
            virtual bool TrgDamage_();
            virtual bool TrgDestroy_();
    };

    // A red shell: as the green one.
    class CItemBallHockeyRShell : public CItemBallHockey {
        public:
            CItemBallHockeyRShell(CBall *pBall);
            virtual ~CItemBallHockeyRShell();

            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgGoalIn_();
            virtual bool TrgFreeMode_();
            virtual bool TrgChrHit_();
            virtual bool TrgDamage_();
            virtual bool TrgDestroy_();
    };

    // A bomb: lit (fuse effect and sound) from the start, explodes when shot
    // a second time, on damage, at the goal or when destroyed. The blast
    // strength is given to the constructor.
    class CItemBallHockeyBomb : public CItemBallHockey {
        public:
            CItemBallHockeyBomb(CBall *pBall, float fPower);
            virtual ~CItemBallHockeyBomb();

            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgGoalIn_();
            virtual bool TrgDamage_();
            virtual bool TrgDestroy_();

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
            unsigned long m_hEffect;  // 0x6C, the fuse effect (CEffectCtrl handle)
            unsigned long m_hSe;      // 0x70, the fuse sound (sound task id)
            // 0x74, the constructor's argument, passed on to the blast as
            // its second float (name guessed).
            float m_fPower;
    };

    // A mushroom.
    class CItemBallHockeyToadstool : public CItemBallHockey {
        public:
            CItemBallHockeyToadstool(CBall *pBall);
            virtual ~CItemBallHockeyToadstool();

            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgGoalIn_();
            virtual bool TrgDestroy_();
    };

    // A star.
    class CItemBallHockeyStar : public CItemBallHockey {
        public:
            CItemBallHockeyStar(CBall *pBall);
            virtual ~CItemBallHockeyStar();

            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgGoalIn_();
            virtual bool TrgDestroy_();
    };

    // A banana.
    class CItemBallHockeyBanana : public CItemBallHockey {
        public:
            CItemBallHockeyBanana(CBall *pBall);
            virtual ~CItemBallHockeyBanana();

            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgGoalIn_();
            virtual bool TrgFreeMode_();
            virtual bool TrgChrHit_();
            virtual bool TrgDamage_();
            virtual bool TrgDestroy_();
    };

    // The change effect of the hockey game: an effect at the ball at the
    // start and at the end, and the change sound at the start.
    class CItemBallChangeEffectHockey : public CItemBallChangeEffect {
        public:
            CItemBallChangeEffectHockey(bcs::CTask *pParent, unsigned long nBallId);
            virtual ~CItemBallChangeEffectHockey();
            virtual void SetStartEffect();
            virtual void SetEndEffect();
    };
}

#endif /* !MSW_CITEMBALLHOCKEYKINDS_HPP_ */
