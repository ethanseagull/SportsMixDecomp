#ifndef CCOCKPITHOCKEYVSCOUNT_HPP_
#define CCOCKPITHOCKEYVSCOUNT_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x80 (CCockpitHockey::CreateObjImpl allocates it): HockeyF
    // game/hockey/CCockpitHockeyVSCount.cpp. Members not mapped yet.
    class CCockpitHockeyVSCount : public CCockpitLayoutBase {
        public:
            CCockpitHockeyVSCount(long nId, float fSortZ);
            virtual ~CCockpitHockeyVSCount();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual int Set();
            virtual void Remove();

        private:
            unsigned char m_Unk60[0x80 - 0x60];
    };
}

#endif /* !CCOCKPITHOCKEYVSCOUNT_HPP_ */
