#ifndef CTOURNAMENTROUTEMAN_STAGE32_HPP_
#define CTOURNAMENTROUTEMAN_STAGE32_HPP_

#include "game/msw/CTournamentRouteMan.hpp"

namespace msw {

    // The route manager of tournament stage 32 (stage number 0x20; the map
    // with the cannons and the gates). 0x7C0 bytes. Created by
    // CTournamentRouteMan::CreateInstance. The destructor is the implicit
    // one: the original emits it after the inline functions of the file.
    class CTournamentRouteMan_Stage32 : public CTournamentRouteMan {
        public:
            virtual void Initialize();
            virtual CRouteBase *CreateRoute(long nType, long nFrom, long nTo);
            virtual void TimerEvent(long nEvent, long nParam, float fParam, unsigned long ulParam, int b);
            virtual void PushCamera(CRouteBase::_PUSH_INFO *pInfo, long n, float f, CEventModel *pModel);
            virtual int SetParamData(long nId, const GET_PARAM &param);
            virtual int GetParamData(long nId, GET_PARAM &param);
            virtual long GetChipType(long nPoint);
            virtual void StageDispSe(int bDisp);

            // Sets up the map models. Also called without an instance, by
            // CTournamentRouteMan::CreateInstanceDemo.
            static void InitializeModel();

        private:
            // Read from the cannon animations of "map_big_gun" by Initialize
            // and handed to the cannon routes through GetParamData(1..3).
            // (names guessed)
            bcs::FVEC3 m_vGunInPos;  // 0x7A0: the character's place in the cannon ("big_gun_00", frame 0)
            bcs::FVEC3 m_vGunOutPos; // 0x7AC: the same at the end of "big_gun_01"
            float m_fGunOutFrame;    // 0x7B8: length of "big_gun_02"
            // 0x7BC: the cannons used so far (GetParamData/SetParamData(4));
            // each one has two map objects from 0x25 on.
            long m_nGunCount;
    }; // size 0x7C0
}

#endif /* !CTOURNAMENTROUTEMAN_STAGE32_HPP_ */
