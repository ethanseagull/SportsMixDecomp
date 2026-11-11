#ifndef CCOCKPITDODGECODER_HPP_
#define CCOCKPITDODGECODER_HPP_

#include "game/msw/CCockpitDodgeLayoutBase.hpp"

namespace msw {

    // Size 0x118 (CCockpitDodge::CreateObjImpl allocates it). The dodge
    // game's score board: coins, time, sets won, passes, life, special
    // gauges, ball power and the mission counter
    // (DodgeF game/dodge/CCockpitDodgeCoder). It overrides every gauge, set
    // count and coin slide of CCockpitDodgeLayoutBase. Only what
    // CCockpitDodge calls so far; the members past CCockpitDodgeLayoutBase
    // are not mapped. Return types and argument names are guesses; nIdx is
    // a team colour (CGame::GetTeamColorIdx).
    class CCockpitDodgeCoder : public CCockpitDodgeLayoutBase {
        public:
            CCockpitDodgeCoder(long nId);
            virtual ~CCockpitDodgeCoder();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            void SetCoinValue(long nIdx, long nCoin);
            void SetPassBallValue(long nPass);
            void AddPassBallValue(long nPass, long nAdd);
            void SetCoinDisp(int bDisp);
            void SetCoinMaxState(long nIdx, long nState);
            void SetCoinState(long nIdx, long nState);
            void SetSetCountDisp(int bDisp);
            void SetTimeDisp(int bDisp);
            void SetLifeDisp(int bDisp);
            void SetPassBallDisp(int bDisp);
            void SetTimeValue(float fTime, int nArg, int bArg);
            void SetSetCountPoint(long nIdx, long nCount);
            void SetPeriodValue(long nPeriod);
            void SetSPGauge(long nIdx, float fRatio);
            void SetSPControllerState(long nIdx, int nState);
            void SetSPGaugeDisp(int bDisp);
            void SetSPGaugeTelop(long nIdx, float fRatio);
            void SetMissionCountDisp(int bDisp);
            void SetMissionCountNum(long nNum, long nMax);
            void SetMissionCountUpNum(long nNum, long nMax);
            void SetMissionCountActiveColor(int bActive);
            void SetBallPowerState(long nIdx, long nState);
            void SetBallPowerShift(long nIdx, long nShift);
            void SetTelopPowerUp(long nIdx, long nState);

        private:
            unsigned char m_Unk60[0x118 - 0x60];
    };
}

#endif /* !CCOCKPITDODGECODER_HPP_ */
