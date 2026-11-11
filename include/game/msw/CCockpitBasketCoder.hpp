#ifndef CCOCKPITBASKETCODER_HPP_
#define CCOCKPITBASKETCODER_HPP_

#include "game/msw/CCockpitBasketLayoutBase.hpp"

namespace msw {

    // Size 0x164 (CCockpitBasket::CreateObjImpl allocates it). The basket
    // game's score board: scores, coins, time, period, special gauges and
    // the mission counter (BasketF game/basket/CCockpitBasketCoder). Only
    // what CCockpitBasket calls so far; the members past
    // CCockpitBasketLayoutBase (a cockpit::CMissionCount and a
    // cockpit::CEffectSet among them) are not mapped. Return types and
    // argument names are guesses.
    class CCockpitBasketCoder : public CCockpitBasketLayoutBase {
        public:
            CCockpitBasketCoder(long nId);
            virtual ~CCockpitBasketCoder();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // nIdx: a team colour (CGame::GetTeamColorIdx).
            void SetScoreValue(long nIdx, long nScore);
            void SetCoinValue(long nIdx, long nCoin);
            void SetTimeValue(float fTime, int nArg, int bArg);
            void SetPeriodValue(long nPeriod);
            void SetCoinMaxState(long nIdx, long nState);
            void SetArrowStateAnim(long nAnim);
            void SetScoreLineAnim(long nAnim);
            void SetScoreFont(long nIdx, int nFont);
            void SetSPGauge(long nIdx, float fRatio);
            void SetSPControllerState(long nIdx, int nState);
            void SetSPGaugeTelop(long nIdx, float fRatio);
            void SetMissionCountDisp(int bDisp);
            void SetMissionCountNum(long nNum, long nMax);
            void SetMissionCountUpNum(long nNum, long nMax);
            void SetMissionCountActiveColor(int bActive);
            void SetFaceIcon();

        private:
            unsigned char m_Unk60[0x164 - 0x60];
    };
}

#endif /* !CCOCKPITBASKETCODER_HPP_ */
