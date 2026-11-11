#ifndef CCOCKPITDODGETELOPOUT_HPP_
#define CCOCKPITDODGETELOPOUT_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x68 (CCockpitDodge::CreateObjImpl allocates it). The "out"
    // telop of the dodge game (DodgeF
    // game/dodge/CCockpitDodgeTelopInOutChange.cpp). Only what
    // CCockpitDodge calls so far; argument names are guesses.
    class CCockpitDodgeTelopOut : public CCockpitLayoutBase {
        public:
            CCockpitDodgeTelopOut(long nId, long nType, float fSortZ);
            virtual ~CCockpitDodgeTelopOut();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // Entry nIndex of the telop's text parameters (CCockpitDodge
            // sets 0 to the character, 1 to the team colour).
            void SetAppendTableParam(unsigned long nIndex, short nValue);

        private:
            unsigned char m_Unk60[0x68 - 0x60];
    };
}

#endif /* !CCOCKPITDODGETELOPOUT_HPP_ */
