#ifndef CRADARICON2DHOCKEYGOAL_HPP_
#define CRADARICON2DHOCKEYGOAL_HPP_

#include "game/msw/CMenuLinkBase.hpp"

namespace msw {

    // Size 0x44 (CCockpitHockey::CreateObjImpl allocates it): the radar icons
    // of the goals (HockeyF game/hockey/CRadarIcon2DHockeyGoal.cpp). Members
    // after CMenuLinkBase are not mapped yet.
    class CRadarIcon2DHockeyGoal : public CMenuLinkBase {
        public:
            CRadarIcon2DHockeyGoal(long nId);
            virtual ~CRadarIcon2DHockeyGoal();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();

            void SetDispGoalIcon(long nGoal, int bDisp);

        private:
            unsigned char m_Unk1C[0x44 - 0x1C];
    };
}

#endif /* !CRADARICON2DHOCKEYGOAL_HPP_ */
