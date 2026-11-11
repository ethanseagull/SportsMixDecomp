#ifndef MSW_CITEMBALLVOLLEYKINDS_HPP_
#define MSW_CITEMBALLVOLLEYKINDS_HPP_

#include "game/msw/CItemBallVolley.hpp"
#include "game/msw/CItemBallChangeEffect.hpp"

// The item behaviours a ball can take in the volley game (VolleyF), one class
// per item kind, and the volley game's change effect. All are defined in
// game/volley/CItemBallVolleyBomb.cpp. The boss game has the same set
// (CItemBallBossKinds.hpp); the volley kinds also change the spike, the
// receive and block reactions and the flick-down power.
//
// The kind passed to CItemBall's constructor selects the model: 0 green
// shell, 1 red shell, 2 mushroom, 3 bomb, 4 star, 5 banana.
//
// Trigger results: the shells, the bomb and the banana return 4 from
// TrgDamage_ when the ball has not been shot, and the banana 0x40 from
// TrgPreparationTouch_ when it has. The Trg*_ functions are declared bool in
// CItemBall, so these cannot be returned yet; see the .cpp.

namespace msw {

    // A green shell: breaks on the first hit after it has been shot.
    class CItemBallVolleyGShell : public CItemBallVolley {
        public:
            CItemBallVolleyGShell(CBall *pBall);
            virtual ~CItemBallVolleyGShell();

            virtual void GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg) const;
            virtual unsigned long GetVolleyReceiveReaction(long, unsigned long reaction) const;
            virtual unsigned long GetVolleyBlockReaction(long nKind, unsigned long reaction) const;
            virtual float GetVolleyFlickDownPower(long, float power) const;
            virtual long GetVolleySpikeTailEffect(long effect, long) const;
            virtual long GetVolleyPassTailEffect(long effect, long, long) const;

            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgGoalIn_();
            virtual bool TrgFreeMode_();
            virtual bool TrgChrHit_();
            virtual bool TrgDamage_();
            virtual bool TrgDestroy_();
            virtual bool TrgReceive_();
            virtual bool TrgBlock_();
    };

    // A red shell: as the green one, with its own spike.
    class CItemBallVolleyRShell : public CItemBallVolley {
        public:
            CItemBallVolleyRShell(CBall *pBall);
            virtual ~CItemBallVolleyRShell();

            virtual void GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg) const;
            virtual unsigned long GetVolleyReceiveReaction(long, unsigned long reaction) const;
            virtual unsigned long GetVolleyBlockReaction(long nKind, unsigned long reaction) const;
            virtual float GetVolleyFlickDownPower(long, float power) const;
            virtual long GetVolleySpikeTailEffect(long effect, long) const;
            virtual long GetVolleyPassTailEffect(long effect, long, long) const;

            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgGoalIn_();
            virtual bool TrgFreeMode_();
            virtual bool TrgChrHit_();
            virtual bool TrgDamage_();
            virtual bool TrgDestroy_();
            virtual bool TrgReceive_();
            virtual bool TrgBlock_();
    };

    // A bomb: lit (fuse effect and sound) from the start, explodes on the
    // first trigger after it has been shot, or when it reaches the goal.
    class CItemBallVolleyBomb : public CItemBallVolley {
        public:
            CItemBallVolleyBomb(CBall *pBall);
            virtual ~CItemBallVolleyBomb();

            // Not virtual (no slot in the vtable) and not const.
            float GetExplosionCollSize();

            virtual void GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg) const;
            virtual unsigned long GetVolleyReceiveReaction(long, unsigned long reaction) const;
            virtual unsigned long GetVolleyBlockReaction(long nKind, unsigned long reaction) const;
            virtual long GetVolleySpikeTailEffect(long effect, long) const;
            virtual long GetVolleyPassTailEffect(long effect, long, long) const;

            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgGoalIn_();
            virtual bool TrgDamage_();
            virtual bool TrgReceive_();
            virtual bool TrgBlock_();

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

    // A mushroom ball. Only its receive and block reactions are known: their
    // static locals are left in the original .rodata, while the link
    // stripped all of its code (nothing creates it). Its constructor and
    // vtable are not in the game.
    class CItemBallVolleyMushroom : public CItemBallVolley {
        public:
            virtual ~CItemBallVolleyMushroom();

            virtual unsigned long GetVolleyReceiveReaction(long, unsigned long reaction) const;
            virtual unsigned long GetVolleyBlockReaction(long nKind, unsigned long reaction) const;
    };

    // A mushroom (the kind-2 item): drawn larger; every trigger clears the
    // shot flag.
    class CItemBallVolleyToadstool : public CItemBallVolley {
        public:
            CItemBallVolleyToadstool(CBall *pBall);
            virtual ~CItemBallVolleyToadstool();

            virtual float GetScale() const;
            virtual void GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg) const;
            virtual unsigned long GetVolleyReceiveReaction(long, unsigned long reaction) const;
            virtual unsigned long GetVolleyBlockReaction(long nKind, unsigned long reaction) const;
            virtual long GetVolleyPassParam(long param, long) const;
            virtual long GetVolleyChanceBallParam(long param) const;
            virtual long GetVolleySpikeTailEffect(long effect, long) const;
            virtual long GetVolleyPassTailEffect(long effect, long, long) const;

            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgGoalIn_();
            virtual bool TrgFreeMode_();
            virtual bool TrgChrHit_();
            virtual bool TrgDamage_();
            virtual bool TrgDestroy_();
            virtual bool TrgReceive_();
            virtual bool TrgBlock_();
    };

    // A star.
    class CItemBallVolleyStar : public CItemBallVolley {
        public:
            CItemBallVolleyStar(CBall *pBall);
            virtual ~CItemBallVolleyStar();

            virtual float GetScale() const;
            virtual void GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg) const;
            virtual unsigned long GetVolleyReceiveReaction(long, unsigned long reaction) const;
            virtual unsigned long GetVolleyBlockReaction(long nKind, unsigned long reaction) const;
            virtual float GetVolleyFlickDownPower(long, float power) const;
            virtual long GetVolleySpikeTailEffect(long effect, long) const;
            virtual long GetVolleyPassTailEffect(long effect, long, long) const;

            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgGoalIn_();
            virtual bool TrgFreeMode_();
            virtual bool TrgChrHit_();
            virtual bool TrgDamage_();
            virtual bool TrgDestroy_();
            virtual bool TrgReceive_();
            virtual bool TrgBlock_();
    };

    // A banana.
    class CItemBallVolleyBanana : public CItemBallVolley {
        public:
            CItemBallVolleyBanana(CBall *pBall);
            virtual ~CItemBallVolleyBanana();

            virtual void GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg) const;
            virtual unsigned long GetVolleyReceiveReaction(long, unsigned long reaction) const;
            virtual unsigned long GetVolleyBlockReaction(long nKind, unsigned long reaction) const;
            virtual float GetVolleyFlickDownPower(long, float power) const;
            virtual long GetVolleySpikeTailEffect(long effect, long) const;
            virtual long GetVolleyPassTailEffect(long effect, long, long) const;

            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgGoalIn_();
            virtual bool TrgFreeMode_();
            virtual bool TrgChrHit_();
            virtual bool TrgDamage_();
            virtual bool TrgDestroy_();
            virtual bool TrgReceive_();
            virtual bool TrgBlock_();
            virtual bool TrgPreparationTouch_();
    };

    // The change effect of the volley game: an effect and the change sound
    // at the ball, and a second effect at the end.
    class CItemBallChangeEffectVolley : public CItemBallChangeEffect {
        public:
            CItemBallChangeEffectVolley(bcs::CTask *pParent, unsigned long nBallId);
            virtual ~CItemBallChangeEffectVolley();
            virtual void SetStartEffect();
            virtual void SetEndEffect();
    };
}

#endif /* !MSW_CITEMBALLVOLLEYKINDS_HPP_ */
