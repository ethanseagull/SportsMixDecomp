#ifndef MSW_CITEMBALLDODGE_HPP_
#define MSW_CITEMBALLDODGE_HPP_

#include "game/msw/CItemBall.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    class CBallDodge;

    // How a dodgeball ball moves and reacts in its normal state and when it
    // carries an item (DodgeF; all defined in game/dodge/CItemBallBombDodge.cpp).
    // CBallDodge asks this interface for its bounce rates, hit results and
    // the like. A plain ball has a CItemBallNormalDodge; the item kinds
    // (CItemBallDodgeKinds.hpp) derive from CItemBallDodge, which is a
    // CItemBall and a CItemBallBaseDodge (at 0x64).
    //
    // The order of the virtual functions is the order of the slots in
    // __vt__Q23msw20CItemBallNormalDodge. Bodies given here are the ones the
    // module emits as weak functions. Names are from the symbols; return
    // types and argument names are guessed.
    class CItemBallBaseDodge {
        public:
            // Declared without a body and defined inline below the class:
            // the original has no vtable of this class anywhere (see
            // notes/toolchain.md, "Where a vtable is emitted").
            virtual ~CItemBallBaseDodge();

            // Hooks of the ball's normal shot and free move states.
            virtual void OnCtorNormalShoot();
            virtual void OnPreNormalShoot();
            // One step of the shot: moves rPos by rVel over fFrame frames
            // under the acceleration fAccel and updates rVel.
            virtual void DoNormalShootPhysMove(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fAccel, float fFrame);
            virtual void OnPostNormalShoot();
            virtual void OnDtorNormalShoot();
            virtual void OnCtorFreeMove();
            virtual void OnDtorFreeMove();
            virtual void OnReflectWall() = 0;
            virtual void OnReflectFloor();
            virtual void OnTrgTouch(long nChr);
            virtual void OnCatch(long nChr);

            // What a hit, catch and so on does (values not known).
            virtual long GetHitResult() { return 0; }
            virtual long GetCatchResult() { return 1; }
            virtual long GetFlickResult() { return 3; }
            virtual long GetOuterHitResult() { return 1; }
            virtual long GetFriendHitResult();

            virtual float GetFloorBoundDecRateXZ();
            virtual float GetFloorBoundDecRateY();
            virtual float GetFloorRoundDecRateXZ();
            virtual float GetWallBoundDecRateXZ();
            virtual float GetWallBoundTime();
            virtual float GetGravityDefault() const;
            virtual long GetShootSe(long nSe) const;
            virtual int CanbeWallReflect() const { return 1; }
            virtual long GetTailEffect(long nEffect) const { return nEffect; }
            virtual long GetMapCollDetectParam() const { return 0; }
            virtual void SetOutBallMode(const bcs::FVEC3 &rPos, long nChr, long nArg);
            virtual long GetHitType() const;
            virtual long GetFriendHitType() const;
            virtual float GetDmgEffectFrame() const;
            virtual long TranslateCatchMethod(long nMethod, long) const { return nMethod; }
            virtual long TranslateCatchParamType(long nType, int) const { return nType; }
            virtual CBallDodge *GetBallDodge() const = 0;
            virtual const bcs::FVEC3 &GetVel() const;
            virtual void SetVel(const bcs::FVEC3 &rVel);
            virtual void SetTargetPos(const bcs::FVEC3 &rPos);
            virtual void CancelOffensive_();
    };

    inline CItemBallBaseDodge::~CItemBallBaseDodge() {}

    // The behaviour of a ball without an item (size 8). Constructed by
    // CBallDodge.
    class CItemBallNormalDodge : public CItemBallBaseDodge {
        public:
            CItemBallNormalDodge(CBallDodge *pBall) : m_pBall(pBall) {}

            virtual void OnReflectWall();
            virtual long GetFriendHitResult();
            virtual CBallDodge *GetBallDodge() const;

        private:
            CBallDodge *m_pBall; // 0x04
    };

    // Base of the dodgeball item behaviours (size 0x68).
    class CItemBallDodge : public CItemBall, public CItemBallBaseDodge {
        public:
            // Inline: the kinds call CItemBall's constructor directly.
            CItemBallDodge(CBall *pBall, long type) : CItemBall(pBall, type) {}

            // Declared first and defined inline below the class (guess): the
            // original object has no CItemBallDodge vtable.
            virtual ~CItemBallDodge();

            virtual float GetWallBoundDecRateXZ();
            virtual CBallDodge *GetBallDodge() const;
    };

    inline CItemBallDodge::~CItemBallDodge() {}
}

#endif /* !MSW_CITEMBALLDODGE_HPP_ */
