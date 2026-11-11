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

            void SetChrPadNo(long nChr);
            void SetChrFaceIcon(long nChr);

        private:
            unsigned char m_Unk60[0x68 - 0x60];
    };
}

#endif /* !CCOCKPITHOCKEYTELOPOUT_HPP_ */
