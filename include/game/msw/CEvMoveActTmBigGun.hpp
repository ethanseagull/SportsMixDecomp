#ifndef CEVMOVEACTTMBIGGUN_HPP_
#define CEVMOVEACTTMBIGGUN_HPP_

#include "game/msw/CEventMoveAction.hpp"

namespace msw {

    // The action of a character on the cannon routes of tournament stage 32
    // (msw::CRouteBigGunIn): when such a route starts, it plays the cannon
    // motion stretched over the route, and returns to the idle pose when
    // that time is over. 0x18 bytes. Created inline by msw::CRouteMove's
    // constructor; defined in game/tour/CTournamentRouteMan_Stage32.cpp.
    // (member names guessed)
    class CEvMoveActTmBigGun : public CEventMoveAction {
        public:
            CEvMoveActTmBigGun(CEventModel *pModel) : CEventMoveAction(pModel), m_fWait(0.0f)
            {
            }

            // Inline: the original emits it among the file's inline functions.
            virtual ~CEvMoveActTmBigGun() {}
            virtual void UpdateFrame(const bcs::FVEC3 &pos, const bcs::FVEC3 &rot, float fFrame);

        private:
            float m_fWait; // 0x14: frames the cannon motion runs, 0 when it is not playing
    }; // size 0x18
}

#endif /* !CEVMOVEACTTMBIGGUN_HPP_ */
