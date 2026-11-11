#ifndef CCOCKPITHOCKEYTELOPOUT_HPP_
#define CCOCKPITHOCKEYTELOPOUT_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x68 (CCockpitHockey::CreateObjImpl allocates it, object 31).
    // A telop of the hockey game (HockeyF
    // game/hockey/CCockpitHockeyTelopReady.cpp, with CCockpitHockeyTelopReady).
    // Argument names are guesses.
    class CCockpitHockeyTelopOut : public CCockpitLayoutBase {
        public:
            CCockpitHockeyTelopOut(long nId, long nType, float fSortZ);
            virtual ~CCockpitHockeyTelopOut();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // Shows the pad label ("1P" to "4P") of character nChr; -1 to -3
            // when it has none.
            long SetChrPadNo(long nChr);
            // (inline; its local static is kept) Puts character nChr's face
            // in the telop.
            void SetChrFaceIcon(long nChr);

        private:
            long m_nType; // 0x60, the constructor's nType: which layout
            long m_nChr;  // 0x64, the character (CChrMan index) shown
    };
}

#endif /* !CCOCKPITHOCKEYTELOPOUT_HPP_ */
