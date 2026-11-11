#ifndef CCOCKPITHOCKEYTELOPREADY_HPP_
#define CCOCKPITHOCKEYTELOPREADY_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x78 (CCockpitHockey::CreateObjImpl allocates it, types 0 and 1):
    // HockeyF game/hockey/CCockpitHockeyTelopReady.cpp. Members not mapped
    // yet.
    class CCockpitHockeyTelopReady : public CCockpitLayoutBase {
        public:
            CCockpitHockeyTelopReady(long nId, long nType, float fSortZ);
            virtual ~CCockpitHockeyTelopReady();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

        private:
            unsigned char m_Unk60[0x78 - 0x60];
    };
}

#endif /* !CCOCKPITHOCKEYTELOPREADY_HPP_ */
