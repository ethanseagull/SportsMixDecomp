#ifndef CCOCKPITDODGETELOPINOUTCHANGE_HPP_
#define CCOCKPITDODGETELOPINOUTCHANGE_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x6C (CCockpitDodge::CreateObjImpl allocates it). The telop of a
    // player coming in or going out (DodgeF
    // game/dodge/CCockpitDodgeTelopInOutChange.cpp). CCockpitLayoutBase at
    // 0, CCockpitFrameOutBase at 0x60 (its destructor thunk is @96@). Only
    // what CCockpitDodge calls so far; argument names are guesses.
    class CCockpitDodgeTelopInOutChange : public CCockpitLayoutBase, public CCockpitFrameOutBase {
        public:
            CCockpitDodgeTelopInOutChange(long nId, float fSortZ);
            virtual ~CCockpitDodgeTelopInOutChange();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // nIdx: a team colour; nMode 0 or 1.
            void SetParameter(unsigned long nIdx, long nMode, const wchar_t *pText0, const wchar_t *pText1);
    };
}

#endif /* !CCOCKPITDODGETELOPINOUTCHANGE_HPP_ */
