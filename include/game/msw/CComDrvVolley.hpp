#ifndef MSW_CCOMDRVVOLLEY_HPP_
#define MSW_CCOMDRVVOLLEY_HPP_

#include "game/msw/CComDrv.hpp"
#include "game/msw/CChrCore.hpp"

namespace msw {

    // The computer driver of the volleyball game (VolleyF). No members of
    // its own were found.
    //
    // Virtual functions in the order of __vt__Q23msw13CComDrvVolley: two
    // overrides of CComDrv's slots, then six of its own (0xE0-0xF4). The
    // serve and receive methods (MethdServe to MethdDiveReceive) are not in
    // game/volley/CComDrvVolley.cpp: they follow its weak functions in the
    // original, so they belong to another file. Return types other than the
    // vtable's are guesses.
    class CComDrvVolley : public CComDrv {
        public:
            CComDrvVolley(long nArg);
            virtual ~CComDrvVolley();

            virtual void ClearAll();
            virtual void UpdateInfo();
            // Not zero when the driver's team is the user's team.
            // (meaning guessed from GameWork::GetUserTeam)
            virtual int IsMyTurn() const;
            virtual int MethdServe();
            virtual int CanbeReceive();
            virtual int MethdReceive();
            virtual int CanbeDiveReceive();
            virtual int MethdDiveReceive();

            // CComManVolley's answers for m_pBall / m_pChr.
            int GetBallTargetPos(bcs::FVEC3 &rPos) const;
            int GetBallSpikeJumpTargetPos(bcs::FVEC3 &rPos) const;
            int GetTargetMarkerPos(bcs::FVEC3 &rPos) const;
            // Not zero when the character may touch the ball in play now.
            int CanbeMyTouch() const;
            // Not zero when some character is in its serve mode.
            int IsExistServeMode() const;
            // The driven character's number, widened (the other sports have
            // this out of line; in VolleyF it is inline, in
            // CComDrvVolleyGame's special shot actions).
            long GetChrTypeID() const { return m_pChr->GetChrTypeID(); }
    };
}

#endif /* !MSW_CCOMDRVVOLLEY_HPP_ */
