#ifndef CCOCKPITDODGECODER_HPP_
#define CCOCKPITDODGECODER_HPP_

#include "game/msw/CCockpitDodgeLayoutBase.hpp"
#include "game/msw/cockpit/CEffectSet.hpp"
#include "game/msw/cockpit/CMissionCount.hpp"
#include "game/msw/cockpit/CSPGauge.hpp"
#include "game/msw/cockpit/CSPGaugeInputDisp.hpp"
#include "game/msw/cockpit/CTimeCount.hpp"

namespace msw {

    // Size 0x118 (CCockpitDodge::CreateObjImpl allocates it). The dodge
    // game's score board: coins, time, sets won, passes, life, special
    // gauges and the mission counter (DodgeF game/dodge/CCockpitDodgeCoder).
    // It overrides every gauge, set count and coin slide of
    // CCockpitDodgeLayoutBase. Return types and argument names are guesses;
    // nIdx is a team (0 or 1).
    class CCockpitDodgeCoder : public CCockpitDodgeLayoutBase {
        public:
            CCockpitDodgeCoder(long nId);
            virtual ~CCockpitDodgeCoder();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            virtual int GaugeOut();
            virtual int GaugeIn();
            virtual int GaugeOutWait();
            virtual int GaugeInWait();
            virtual int SetCountOut();
            virtual int SetCountIn();
            virtual int SetCountOutWait();
            virtual int SetCountInWait();
            virtual int CoinOut();
            virtual int CoinIn();
            virtual int CoinOutWait();
            virtual int CoinInWait();

            // Lights the won-set marks one after the other (the argument
            // is not read).
            void SetCountEffect(int bArg);
            // nCoin above 0: a coin gained, else a coin lost.
            void SetCoinValue(long nIdx, long nCoin);
            void SetPassBallValue(long nPass);
            void AddPassBallValue(long nPass, long nAdd);
            void SetCoinDisp(int bDisp);
            // 5: the special gauge is not full, 6: it is.
            void SetCoinMaxState(long nIdx, long nState);
            void SetCoinState(long nIdx, long nState);
            void SetSetCountDisp(int bDisp);
            void SetTimeDisp(int bDisp);
            void SetLifeDisp(int bDisp);
            void SetPassBallDisp(int bDisp);
            // The clock at fTime frames; below 0, the "no time limit" mark.
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
            int SetBallPowerState(long nIdx, long nState);
            int SetBallPowerShift(long nIdx, long nShift);
            int SetTelopPowerUp(long nIdx, long nState);

        private:
            int SetupGame();
            // (inline, defined in the .cpp; names guessed) Play slide
            // animation nAnim on both life gauges, on the set counters (if
            // the match has sets), and on the coin counters and power panels.
            void PlayAnimGauge(long nAnim);
            void PlayAnimSetCount(long nAnim);
            void PlayAnimCoin(long nAnim);
            // (inline, defined in the .cpp) Plays the "coin lost" animation
            // on team nIdx's power counter.
            void CoinBackAnim(long nIdx);

            unsigned char m_nSetNum;                      // 0x60, sets in the match: 1, 3 or 5
            unsigned char m_nSetPoint[2];                 // 0x61, sets won by each team
            unsigned char m_nChrNum;                      // 0x63, CGameDodge::GetVariableChrNum(0)
            unsigned char m_nUnk64[2];                    // 0x64, set to 0xFF, not read here
            cockpit::CSPGauge m_SPGauge[2];               // 0x68, by team
            cockpit::CSPGaugeInputDisp m_SPInputDisp[2];  // 0x98, by team
            cockpit::CTimeCount m_TimeCount;              // 0xB0
            cockpit::CMissionCount m_MissionCount;        // 0xB8
            cockpit::CEffectSet m_EffectSet;              // 0xD4
    };
}

#endif /* !CCOCKPITDODGECODER_HPP_ */
