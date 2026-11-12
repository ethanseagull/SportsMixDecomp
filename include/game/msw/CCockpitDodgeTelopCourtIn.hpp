#ifndef CCOCKPITDODGETELOPCOURTIN_HPP_
#define CCOCKPITDODGETELOPCOURTIN_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {
    class CCockpit;

    // Size 0x6C (CCockpitDodge::CreateObjImpl allocates it). A telop shown
    // when a player enters the court (DodgeF
    // game/dodge/CCockpitDodgeTelopInOutChange.cpp). CCockpitLayoutBase at
    // 0, CCockpitFrameOutBase at 0x60 (its destructor thunk is @96@). Only
    // the constructor and virtuals so far.
    class CCockpitDodgeTelopCourtIn : public CCockpitLayoutBase, public CCockpitFrameOutBase {
        public:
            CCockpitDodgeTelopCourtIn(long nId, float fSortZ);
            virtual ~CCockpitDodgeTelopCourtIn();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // (static) Shows name pName on the side of team nTeam of telop
            // nObjId of pCockpit. Argument names guessed.
            static void SetParameter(CCockpit *pCockpit, long nObjId, unsigned long nTeam, const wchar_t *pName);
    };
}

#endif /* !CCOCKPITDODGETELOPCOURTIN_HPP_ */
