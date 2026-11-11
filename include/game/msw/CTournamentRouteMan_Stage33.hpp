#ifndef CTOURNAMENTROUTEMAN_STAGE33_HPP_
#define CTOURNAMENTROUTEMAN_STAGE33_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CTournamentRouteMan.hpp"

namespace msw {

    // The route manager of tournament stage 33 (stage number 0x21; the map
    // in the sky: the airships, the rainbow roads and the turning bridges).
    // 0x7CC bytes. Created by CTournamentRouteMan::CreateInstance. The
    // destructor is the implicit one: the original emits it after the inline
    // functions of the file. GetChipType is the base class's.
    class CTournamentRouteMan_Stage33 : public CTournamentRouteMan {
        public:
            virtual void Initialize();
            virtual CRouteBase *CreateRoute(long nType, long nFrom, long nTo);
            virtual void TimerEvent(long nEvent, long nParam, float fParam, unsigned long ulParam, int b);
            virtual void PushCamera(CRouteBase::_PUSH_INFO *pInfo, long n, float f, CEventModel *pModel);
            virtual int SetParamData(long nId, const GET_PARAM &param);
            virtual int GetParamData(long nId, GET_PARAM &param);
            virtual void StageDispSe(int bDisp);

            // Sets up the map models. Also called without an instance, by
            // CTournamentRouteMan::CreateInstanceDemo.
            static void InitializeModel();

        private:
            // The airship animations. ulData is the address of the
            // animation's entry in the file's table (see
            // CTournamentRouteMan_Stage33.cpp); nPoint the map point the
            // ship is placed at (0 for none). (parameter names guessed)
            void _ShipAnimNodeCamera(unsigned long ulData, long nPoint, float fFrame);
            void _ShipAnimNode(unsigned long ulData, long nPoint, float fParam, unsigned long ulParam, long nEvent,
                               int bDisp);
            void _ShipAnimNodeAp(unsigned long ulData, long nPoint, float fParam, unsigned long ulParam, long nEvent,
                                 int bDisp);

            // (names guessed) Where the shown map's ships are, set by the
            // timer events.
            bcs::FVEC3 m_vRainbowPos;  // 0x7A0: where the rainbow roads start (timer event 0x14)
            long m_anShipPoint[4];     // 0x7AC: the map point of each ship (objects 0x27 to 0x2A), -1 for none
            float m_afShipRot[4];      // 0x7BC: the direction of each ship, in radians
    }; // size 0x7CC
}

#endif /* !CTOURNAMENTROUTEMAN_STAGE33_HPP_ */
