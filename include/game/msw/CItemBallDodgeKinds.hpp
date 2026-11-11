#ifndef MSW_CITEMBALLDODGEKINDS_HPP_
#define MSW_CITEMBALLDODGEKINDS_HPP_

#include "game/msw/CItemBallDodge.hpp"
#include "game/msw/CItemBallChangeEffect.hpp"

namespace msw {

    // The item behaviours of the dodgeball game (DodgeF), all defined in
    // game/dodge/CItemBallBombDodge.cpp. The number passed to CItemBall is
    // the item type (0 green shell, 1 red shell, 2 mushroom, 3 bomb, 4 star,
    // 5 banana).
    //
    // In each class the functions of CItemBallBaseDodge it overrides are
    // declared in the order of the extra slots the compiler gives them after
    // the CItemBallBaseDodge part of the vtable; the bodies given here are
    // the ones the module emits as weak functions. Member names are guessed.

    // Bomb -----------------------------------------------------------------

    class CItemBallBombDodge : public CItemBallDodge {
        public:
            CItemBallBombDodge(CBall *pBall);
            virtual ~CItemBallBombDodge();

            virtual void MainFunc_();
            virtual long TrgShootStart_();
            virtual long TrgRebound_();
            virtual long TrgReboundToFree_();
            virtual long TrgGoalIn_();
            virtual long TrgFreeMode_();
            virtual long TrgChrHit_();
            virtual long TrgAttach_();
            virtual long TrgDamage_();
            virtual long TrgConstruct_();

            virtual void OnCtorNormalShoot();
            virtual void OnCtorFreeMove();
            virtual void OnDtorFreeMove();
            virtual void OnReflectWall();
            virtual void OnReflectFloor();
            virtual void OnTrgTouch(long nChr);
            virtual void OnCatch(long nChr);
            virtual long GetHitResult() { return 0; }
            virtual long GetCatchResult() { return 0; }
            virtual long GetFlickResult() { return 0; }
            virtual long GetOuterHitResult() { return 0; }
            virtual long GetFriendHitResult() { return 3; }
            virtual float GetFloorBoundDecRateXZ();
            virtual float GetFloorBoundDecRateY();
            virtual float GetFloorRoundDecRateXZ();
            virtual float GetGravityDefault() const;
            virtual long GetHitType() const;
            virtual float GetDmgEffectFrame() const;

            virtual void SetFreeMove();
            // Sends the ball off towards the floor ahead of it.
            virtual void SetVelocity();
            // The speed along the floor and the upward speed after a bounce.
            virtual float GetMyVelXZ() const;
            virtual float GetMyVelY() const;
            // Turns the ball after a bounce, sometimes towards the nearest
            // character.
            virtual void SetBoundVel();
            virtual void AttachTailEffect_();
            virtual void DetachTailEffect_();

            // Sets the bomb off once: a blast at the ball (attached to its
            // model with bAttach) that does not hit the thrower's team.
            void Explode_(int bAttach);
            float GetExplosionCollSize();

        private:
            void StartFuse_();
            void StopFuse_();

            int m_bExploded;               // 0x68
            float m_fFuseTime;             // 0x6C, 60, then set by TrgConstruct_ (not read here)
            float m_fFuseDelay;            // 0x70, set by TrgConstruct_ (not read here)
            float m_fUnk74;                // 0x74
            unsigned long m_hFuseEffect;   // 0x78
            unsigned long m_hFuseSe;       // 0x7C
            float m_fUnk80;                // 0x80
            unsigned long m_hTailEffect;   // 0x84
            long m_nShotTeam;              // 0x88, the thrower's team, -1 before the shot
    };

    // Green shell ----------------------------------------------------------

    class CItemBallGShellDodge : public CItemBallDodge {
        public:
            CItemBallGShellDodge(CBall *pBall);
            virtual ~CItemBallGShellDodge();

            virtual void MainFunc_();
            virtual long TrgGoalIn_();
            virtual long TrgChrHit_();
            virtual long TrgAttach_();
            virtual long TrgDamage_();
            virtual long TrgConstruct_();

            virtual long GetFlickResult() { return 0; }
            virtual long GetCatchResult() { return 3; }
            virtual long GetFriendHitResult() { return 0; }
            virtual void OnCtorNormalShoot();
            virtual void OnReflectWall();
            virtual void OnCatch(long nChr);
            virtual long TranslateCatchMethod(long nMethod, long nKind) const;
            virtual long TranslateCatchParamType(long nType, int nArg) const;
            virtual long GetTailEffect(long nEffect) const;
            virtual long GetShootSe(long nSe) const;
            virtual float GetDmgEffectFrame() const;
            virtual float GetWallBoundDecRateXZ();

        private:
            long m_nReflectNum;  // 0x68, wall bounces left that home in
            float m_fTimer;      // 0x6C, frames since the item came
            float m_fCatchTime;  // 0x70, after this a catch by the own team ends it
    };

    // Red shell ------------------------------------------------------------

    class CItemBallRShellDodge : public CItemBallDodge {
        public:
            CItemBallRShellDodge(CBall *pBall);
            virtual ~CItemBallRShellDodge();

            virtual void MainFunc_();
            virtual long TrgGoalIn_();
            virtual long TrgChrHit_();
            virtual long TrgAttach_();
            virtual long TrgDamage_();
            virtual long TrgConstruct_();

            virtual long GetFlickResult() { return 0; }
            virtual long GetCatchResult() { return 3; }
            virtual long GetFriendHitResult() { return 0; }
            virtual int CanbeWallReflect() const;
            virtual void OnCtorNormalShoot();
            virtual void OnPreNormalShoot();
            virtual void OnReflectWall();
            virtual void OnCatch(long nChr);
            virtual long TranslateCatchMethod(long nMethod, long nKind) const;
            virtual long TranslateCatchParamType(long nType, int nArg) const;
            virtual long GetTailEffect(long nEffect) const;
            virtual long GetShootSe(long nSe) const;
            virtual float GetDmgEffectFrame() const;
            virtual float GetWallBoundDecRateXZ();

        private:
            long m_nUnk68;       // 0x68, cleared when shot (not read here)
            int m_bHoming;       // 0x6C, still homing in on the target
            float m_fTimer;      // 0x70
            float m_fCatchTime;  // 0x74
    };

    // Banana ---------------------------------------------------------------

    class CItemBallBananaDodge : public CItemBallDodge {
        public:
            CItemBallBananaDodge(CBall *pBall);
            virtual ~CItemBallBananaDodge();

            virtual void MainFunc_();
            virtual long TrgGoalIn_();
            virtual long TrgChrHit_();
            virtual long TrgAttach_();
            virtual long TrgDamage_();
            virtual long TrgConstruct_();

            virtual long GetFlickResult() { return 3; }
            virtual long GetCatchResult() { return 3; }
            virtual long GetFriendHitResult() { return 0; }
            virtual int CanbeWallReflect() const;
            virtual void OnCtorNormalShoot();
            virtual void OnPreNormalShoot();
            virtual void OnReflectWall();
            virtual void OnCatch(long nChr);
            virtual void SetOutBallMode(const bcs::FVEC3 &rPos, long nChr, long nArg);
            virtual long TranslateCatchMethod(long nMethod, long nKind) const;
            virtual long TranslateCatchParamType(long nType, int nArg) const;
            virtual long GetTailEffect(long nEffect) const;
            virtual long GetShootSe(long nSe) const;
            virtual float GetDmgEffectFrame() const;
            virtual float GetWallBoundDecRateXZ();

        private:
            long m_nUnk68;       // 0x68, cleared when shot (not read here)
            int m_bHoming;       // 0x6C
            float m_fTimer;      // 0x70
            float m_fCatchTime;  // 0x74
    };

    // Mushroom -------------------------------------------------------------

    class CItemBallSmallDodge : public CItemBallDodge {
        public:
            CItemBallSmallDodge(CBall *pBall);
            virtual ~CItemBallSmallDodge();

            virtual void MainFunc_();
            virtual long TrgRebound_();
            virtual long TrgGoalIn_();
            virtual long TrgFreeMode_();
            virtual long TrgChrHit_();
            virtual long TrgAttach_();
            virtual long TrgDamage_();
            virtual long TrgDestroy_();
            virtual long TrgConstruct_();

            virtual long GetCatchResult() { return 3; }
            virtual float GetFloorBoundDecRateXZ() { return 1.0f; }
            virtual float GetFloorBoundDecRateY();
            virtual float GetFloorRoundDecRateXZ() { return 1.0f; }
            virtual float GetGravityDefault() const;
            virtual void OnCtorNormalShoot();
            virtual void OnPreNormalShoot();
            virtual void OnCtorFreeMove();
            virtual void OnReflectWall();
            virtual void OnReflectFloor();
            virtual void OnCatch(long nChr);
            virtual void SetOutBallMode(const bcs::FVEC3 &rPos, long nChr, long nArg);
            virtual long TranslateCatchMethod(long nMethod, long nKind) const;
            virtual long TranslateCatchParamType(long nType, int nArg) const;
            virtual long GetTailEffect(long nEffect) const;
            virtual float GetDmgEffectFrame() const;

            // The upward speed after a bounce.
            virtual float GetMyVelY() const;

        private:
            float m_fTimer;      // 0x68
            float m_fCatchTime;  // 0x6C
    };

    // Star -----------------------------------------------------------------

    class CItemBallStarDodge : public CItemBallDodge {
        public:
            CItemBallStarDodge(CBall *pBall);
            virtual ~CItemBallStarDodge();

            virtual void MainFunc_();
            virtual long TrgGoalIn_();
            virtual long TrgChrHit_();
            virtual long TrgAttach_();
            virtual long TrgDamage_();
            virtual long TrgConstruct_();

            virtual long GetCatchResult() { return 0; }
            virtual long GetFlickResult() { return 0; }
            virtual long GetOuterHitResult() { return 0; }
            virtual long GetFriendHitResult() { return 0; }
            virtual void OnCtorNormalShoot();
            virtual void DoNormalShootPhysMove(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fAccel, float fFrame);
            virtual void OnDtorNormalShoot();
            virtual void OnReflectWall();
            virtual float GetWallBoundTime();
            virtual void OnTrgTouch(long nChr);
            virtual long GetHitType() const;
            virtual long TranslateCatchMethod(long nMethod, long nKind) const;
            virtual long TranslateCatchParamType(long nType, int nArg) const;
            virtual long GetTailEffect(long nEffect) const;
            virtual long GetMapCollDetectParam() const;
            virtual float GetDmgEffectFrame() const;
            virtual float GetWallBoundDecRateXZ();

        private:
            long m_nUnk68;   // 0x68, cleared when shot (not read here)
            float m_fTimer;  // 0x6C, frames the star lasts (270)
    };

    // Change effect --------------------------------------------------------

    class CItemBallChangeEffectDodge : public CItemBallChangeEffect {
        public:
            CItemBallChangeEffectDodge(bcs::CTask *pParent, unsigned long nBallId);
            virtual ~CItemBallChangeEffectDodge();

            virtual void SetStartEffect();
            virtual void SetEndEffect();
    };
}

#endif /* !MSW_CITEMBALLDODGEKINDS_HPP_ */
