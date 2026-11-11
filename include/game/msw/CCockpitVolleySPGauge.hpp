#ifndef CCOCKPITVOLLEYSPGAUGE_HPP_
#define CCOCKPITVOLLEYSPGAUGE_HPP_

#include "game/msw/CCockpitVolleyLayoutBase.hpp"
#include "game/msw/cockpit/CEffectSet.hpp"
#include "game/msw/cockpit/CSPGauge.hpp"
#include "game/msw/cockpit/CSPGaugeInputDisp.hpp"

namespace msw {

    // Size 0x138. The volley game's special-shot gauges, one per team, with
    // their input prompts, coin flashes and telops (layouts VO_sp and, on a
    // split screen, VO_cockpit_03_sp). Member names are guesses.
    class CCockpitVolleySPGauge : public CCockpitVolleyLayoutBase {
        public:
            // nType 1 is the variant drawn behind the others (sort z -970,
            // layer 6) without the receive panes.
            CCockpitVolleySPGauge(long nId, long nType);
            virtual ~CCockpitVolleySPGauge();
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

            // nState 5: the gauge is no longer full; 6: it is full.
            void SetCoinMaxState(long nTeam, long nState);
            void SetSPGauge(long nTeam, float fRate);
            void SetSPControllerState(long nTeam, int bDisp);
            void SetSPGaugeDisp(int bDisp);
            void SetSPGaugeTelop(long nTeam, float fRate);

        private:
            long m_nType;                                // 0x60
            cockpit::CSPGauge m_SPGauge[4];              // 0x64, by team (two used)
            cockpit::CSPGaugeInputDisp m_SPInputDisp[4]; // 0xC4, by team (two used)
            cockpit::CEffectSet m_EffectSet;             // 0xF4
    };
}

#endif /* !CCOCKPITVOLLEYSPGAUGE_HPP_ */
