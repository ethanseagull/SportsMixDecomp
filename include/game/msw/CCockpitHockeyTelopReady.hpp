#ifndef CCOCKPITHOCKEYTELOPREADY_HPP_
#define CCOCKPITHOCKEYTELOPREADY_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x78 (CCockpitHockey::CreateObjImpl allocates it, types 0 and 1):
    // HockeyF game/hockey/CCockpitHockeyTelopReady.cpp. The "ready" telop of
    // the hockey party game with one face (type 0) or the two faces of a
    // match (type 1). CCockpitFrameOutBase is at 0x60 (vptr at 0x68).
    // Member names are guesses.
    class CCockpitHockeyTelopReady : public CCockpitLayoutBase, public CCockpitFrameOutBase {
        public:
            CCockpitHockeyTelopReady(long nId, long nType, float fSortZ);
            virtual ~CCockpitHockeyTelopReady();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // Shows the pad label ("1P" to "4P") of character nChr; -1 to -3
            // when it has none.
            long SetChrPadNo(long nChr);

        private:
            long m_nType;    // 0x6C, the constructor's nType: which layout
            long m_nChr[2];  // 0x70, the characters (CChrMan index) shown; -1 for none
    };
}

#endif /* !CCOCKPITHOCKEYTELOPREADY_HPP_ */
