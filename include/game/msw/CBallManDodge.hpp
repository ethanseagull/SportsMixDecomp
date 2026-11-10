#ifndef MSW_CBALLMANDODGE_HPP_
#define MSW_CBALLMANDODGE_HPP_

#include "game/msw/CBallMan.hpp"

namespace msw {

    // The ball manager of the dodge game (DodgeF). It adds no members to
    // CBallMan (size 0x58). The table has BALL_MAX places; the balls are
    // grouped by their place modulo 3 (see AddBall_).
    //
    // The virtual functions are declared in the order of the slots of
    // __vt__Q23msw13CBallManDodge; NotifyMessage_ is the class's own, after
    // CBallMan's.
    class CBallManDodge : public CBallMan {
        public:
            enum {
                BALL_MAX = 8
            };

            CBallManDodge(bcs::CTask *pParent, long nBallMax) : CBallMan(pParent, nBallMax) {}
            virtual ~CBallManDodge();

            // The ball in play whose position is nearest to, or furthest
            // from, rPos; null when no ball is in play.
            virtual CBall *GetNearPosBall_(const bcs::FVEC3 &rPos);
            virtual CBall *GetFarPosBall_(const bcs::FVEC3 &rPos);
            // One ball and its marker for each place of the table.
            virtual void Create_();
            // Puts a new ball in the group of pBall (see the .cpp) and starts
            // it as a jump ball; returns it, or null when the table is full.
            virtual CBall *AddBall_(CBall *pBall);
            virtual int CanbeTurnOver_() const;
            virtual int IsBallPassMode_(long nBall);
            virtual int IsBallShootMode_(long nBall);
            virtual int IsBallInvincible_(long nBall);
            virtual int IsBallScored_(long nBall);
            virtual void SetBallPos_(long nBall, const bcs::FVEC3 *pPos);
            virtual void SetJumpBallMode_(long nBall);
            virtual void SetSleepMode_(long nBall);
            // Both ask ball 0.
            virtual float GetPassFlyFrame_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, long nArg) const;
            virtual float GetShootFlyFrame_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, long nArg) const;
            // Passes the message of pFrom to the other balls in play.
            virtual int NotifyMessage_(CBall *pFrom, long nMsg);

            // Returns the instance, creating it if there is none yet.
            static CBallMan *CreateInstance(bcs::CTask *pParent);
            // NotifyMessage_ on the instance.
            static int NotifyMessage(CBall *pFrom, long nMsg);
    };
}

#endif /* !MSW_CBALLMANDODGE_HPP_ */
