#ifndef CCOCKPITBASKETCODER_HPP_
#define CCOCKPITBASKETCODER_HPP_

#include "game/msw/CCockpitBasketLayoutBase.hpp"
#include "game/msw/cockpit/CEffectSet.hpp"
#include "game/msw/cockpit/CMissionCount.hpp"
#include "game/msw/cockpit/CSPGauge.hpp"
#include "game/msw/cockpit/CSPGaugeInputDisp.hpp"
#include "game/msw/cockpit/CTimeCount.hpp"

namespace bcs {
    namespace G2D {
        class CPaneCtrl;
    }
}

namespace msw {
    class CChrCore;

    // Size 0x164 (CCockpitBasket::CreateObjImpl allocates it). The basket
    // game's score board: scores, coins, time, period, special gauges and
    // the mission counter (BasketF game/basket/CCockpitBasketCoder). Same
    // layout as CCockpitHockeyCoder. Return types and argument names are
    // guesses.
    class CCockpitBasketCoder : public CCockpitBasketLayoutBase {
        public:
            CCockpitBasketCoder(long nId);
            virtual ~CCockpitBasketCoder();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();
            virtual int CoinOut();
            virtual int CoinIn();
            virtual int CoinOutWait();
            virtual int CoinInWait();
            virtual int TimeOut();
            virtual int TimeIn();
            virtual int TimeOutWait();
            virtual int TimeInWait();
            virtual int PeriodOut();
            virtual int PeriodIn();
            virtual int PeriodOutWait();
            virtual int PeriodInWait();

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
            void SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl);
            void SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl, long nTeam, long nPlayer, const CChrCore *pChr);

        private:
            int SetupGame();
            // Plays animation nAnim on both scores; with bEffect, also
            // starts the score effect on each.
            int PlayAnimScore(long nAnim, int bEffect);
            // (inline, defined in the .cpp) Plays slide animation nAnim on
            // both coin counters.
            void PlayAnimCoin(long nAnim);
            // (inline, defined in the .cpp) Plays the "coin lost" animation
            // on team nTeam's score.
            void PlayAnimCoinBack(long nTeam);
            // (name taken from CCockpitHockeyCoder; dead-stripped in the
            // module) Plays the special gauges' animation for bDisp.
            void SetSPGaugeDisp(int bDisp);

            unsigned char m_nUnk60;                       // 0x60, set to 0, not read here
            unsigned char m_nUnk61[4];                    // 0x61, set to 0xFF, not read here
            unsigned char m_nUnk65[4];                    // 0x65, set to 0xFF, not read here
            unsigned char m_nUnk69;                       // 0x69, set to 4 by Set, not read here
            cockpit::CSPGauge m_SPGauge[4];               // 0x6C, by team
            cockpit::CSPGaugeInputDisp m_SPInputDisp[4];  // 0xCC, by team
            cockpit::CTimeCount m_TimeCount;              // 0xFC
            cockpit::CMissionCount m_MissionCount;        // 0x104
            cockpit::CEffectSet m_EffectSet;              // 0x120
    };
}

#endif /* !CCOCKPITBASKETCODER_HPP_ */
