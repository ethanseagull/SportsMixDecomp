#ifndef CTOURNAMENTROUTEMAN_STAGE31_HPP_
#define CTOURNAMENTROUTEMAN_STAGE31_HPP_

#include "game/msw/CTournamentRouteMan.hpp"

namespace msw {

    // The route manager of tournament stage 31 (stage number 0x1F; the map
    // with the mushroom jumps and the rope bridges). 0x7A0 bytes, no
    // members of its own. Created by CTournamentRouteMan::CreateInstance.
    class CTournamentRouteMan_Stage31 : public CTournamentRouteMan {
        public:
            virtual ~CTournamentRouteMan_Stage31();
            virtual void Initialize();
            virtual CRouteBase *CreateRoute(long nType, long nFrom, long nTo);
            virtual void TimerEvent(long nEvent, long nParam, float fParam, unsigned long ulParam, int b);
            virtual void PushCamera(CRouteBase::_PUSH_INFO *pInfo, long n, float f, CEventModel *pModel);
            virtual int GetParamData(long nId, GET_PARAM &param);
            virtual long GetChipType(long nPoint);
            virtual void StageDispSe(int bDisp);

            // Sets up the map models. Also called without an instance, by
            // CTournamentRouteMan::CreateInstanceDemo.
            static void InitializeModel();
    };
}

#endif /* !CTOURNAMENTROUTEMAN_STAGE31_HPP_ */
