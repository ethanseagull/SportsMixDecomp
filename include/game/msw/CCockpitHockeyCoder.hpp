#ifndef CCOCKPITHOCKEYCODER_HPP_
#define CCOCKPITHOCKEYCODER_HPP_

#include "game/msw/CCockpitHockeyLayoutBase.hpp"
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

    // Size 0x164. The hockey game's score board (game/hockey/CCockpitHockeyCoder.cpp):
    // both teams' scores, coin counters, special-shot gauges and prompts,
    // the clock and its last-seconds countdown, the period, the players'
    // face icons and the mission counter. Same layout as
    // CCockpitBasketCoder. Member names and the setters' argument names
    // are guesses.
    class CCockpitHockeyCoder : public CCockpitHockeyLayoutBase {
        public:
            CCockpitHockeyCoder(long nId);
            virtual ~CCockpitHockeyCoder();
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

            // nTeam: 0 or 1 (others are ignored by most setters).
            void SetScoreValue(long nTeam, long nScore);
            // nDiff above 0: a coin gained, else a coin lost.
            void SetCoinValue(long nTeam, long nDiff);
            // The clock at fFrame frames; nArg is not read by
            // cockpit::CTimeCount, nMode 1 animates both clock panes.
            void SetTimeValue(float fFrame, int nArg, int nMode);
            // Shows the picture of period nPeriod (0 to 4).
            void SetPeriodValue(long nPeriod);
            void SetScoreDisp(int bDisp);
            void SetTimeDisp(int bDisp);
            void SetPeriodDisp(int bDisp);
            // 5: the special gauge is not full, 6: it is.
            void SetCoinMaxState(long nTeam, long nState);
            void SetArrowStateAnim(long nAnim);
            void SetScoreLineAnim(long nAnim);
            // The score digits' font: bColor set for the second one.
            void SetScoreFont(long nTeam, int bColor);
            void SetSPGauge(long nTeam, float fRate);
            void SetSPControllerState(long nTeam, int bDisp);
            void SetSPGaugeDisp(int bDisp);
            void SetSPGaugeTelop(long nTeam, float fRate);
            void SetMissionCountDisp(int bDisp);
            void SetMissionCountNum(long nNum, long nMax);
            void SetMissionCountUpNum(long nNum, long nMax);
            void SetMissionCountActiveColor(int bActive);
            void SetFaceIcon();
            void SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl);
            void SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl, long nTeam, long nPlayer, const CChrCore *pChr);

        private:
            int SetupGame();
            // (inline, defined in the .cpp) Plays slide animation nAnim on
            // both coin counters.
            void PlayAnimCoin(long nAnim);
            // (inline, defined in the .cpp; name guessed) Plays slide
            // animation nAnim on both scores.
            void PlayAnimScore(long nAnim);
            // (inline, defined in the .cpp) Plays the "coin lost" animation
            // on team nTeam's score.
            void PlayAnimCoinBack(long nTeam);

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

#endif /* !CCOCKPITHOCKEYCODER_HPP_ */
