#ifndef MSW_CBALLMANVOLLEY_HPP_
#define MSW_CBALLMANVOLLEY_HPP_

#include "game/msw/CBallMan.hpp"

namespace msw {

    // The ball manager of the volley game (VolleyF). It adds no members to
    // CBallMan (size 0x58). The game has one ball, at place 0 of a table of
    // eight.
    class CBallManVolley : public CBallMan {
        public:
            CBallManVolley(bcs::CTask *pParent, long nBallMax) : CBallMan(pParent, nBallMax) {}
            virtual ~CBallManVolley();

            virtual void MainFunc_();
            // All four return ball 0 here.
            virtual CBall *GetNearPosBall_(const bcs::FVEC3 &rPos);
            virtual CBall *GetFarPosBall_(const bcs::FVEC3 &rPos);
            virtual CBall *GetNearTargetPosBall_(const bcs::FVEC3 &rPos);
            virtual CBall *GetFarTargetPosBall_(const bcs::FVEC3 &rPos);
            virtual void Create_();
            // Gives each added ball a target marker of its own.
            virtual CBall *AddBall_(CBall *pBall);

            // Returns the instance, creating it if there is none yet: the
            // manager of the game type being played (see
            // CGame::GameWork::GetGameType).
            static CBallMan *CreateInstance(bcs::CTask *pParent);
    };

    // The managers of the volley game's two other game types. The
    // constructors are inline (CBallManVolley::CreateInstance has them).
    namespace BALL_MAN_VOLLEY {

        // (name guessed) A base between CBallMan and the two managers below,
        // with no destructor of its own: their destructors show its
        // implicit one inlined (a second null check around the call to
        // ~CBallMan). Nothing else of it is visible in the game: the
        // compiler drops its vtable store in the inline constructors, and
        // the original link stripped its weak vtable and destructor.
        class CBallManVolleyBase : public CBallMan {
            public:
                CBallManVolleyBase(bcs::CTask *pParent, long nBallMax) : CBallMan(pParent, nBallMax) {}
        };

        // Game type 1, against a boss. (table of eight) Near/Far*Pos look
        // at the balls in play; the Target ones return ball 0.
        class CBallManVolleyVsBoss : public CBallManVolleyBase {
            public:
                CBallManVolleyVsBoss(bcs::CTask *pParent, long nBallMax) : CBallManVolleyBase(pParent, nBallMax) {}
                virtual ~CBallManVolleyVsBoss();

                virtual void MainFunc_();
                virtual CBall *GetNearPosBall_(const bcs::FVEC3 &rPos);
                virtual CBall *GetFarPosBall_(const bcs::FVEC3 &rPos);
                virtual CBall *GetNearTargetPosBall_(const bcs::FVEC3 &rPos);
                virtual CBall *GetFarTargetPosBall_(const bcs::FVEC3 &rPos);
                virtual void Create_();
                virtual CBall *AddBall_(CBall *pBall);
        };

        // Game type 2, the rhythm receive party game. (table of sixteen)
        // Create_ makes no ball of its own; the functions are otherwise
        // those of CBallManVolleyVsBoss.
        class CBallManVolleyRhythmReceive : public CBallManVolleyBase {
            public:
                CBallManVolleyRhythmReceive(bcs::CTask *pParent, long nBallMax) : CBallManVolleyBase(pParent, nBallMax) {}
                virtual ~CBallManVolleyRhythmReceive();

                virtual void MainFunc_();
                virtual CBall *GetNearPosBall_(const bcs::FVEC3 &rPos);
                virtual CBall *GetFarPosBall_(const bcs::FVEC3 &rPos);
                virtual CBall *GetNearTargetPosBall_(const bcs::FVEC3 &rPos);
                virtual CBall *GetFarTargetPosBall_(const bcs::FVEC3 &rPos);
                virtual void Create_();
                virtual CBall *AddBall_(CBall *pBall);
        };
    }
}

#endif /* !MSW_CBALLMANVOLLEY_HPP_ */
