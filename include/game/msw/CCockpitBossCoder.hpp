#ifndef CCOCKPITBOSSCODER_HPP_
#define CCOCKPITBOSSCODER_HPP_

#include "game/msw/CCockpitBossLayoutBase.hpp"
#include "game/msw/cockpit/CEffectSet.hpp"
#include "game/msw/cockpit/CSPGauge.hpp"
#include "game/msw/cockpit/CSPGaugeInputDisp.hpp"

namespace msw {
    class CChrCore;

    // Size 0xD4. The boss game's main cockpit layout
    // (game/boss/CCockpitBossCoder.cpp): the players' life gauges with their
    // face icons and labels, the boss's life gauge, the coin (special shot)
    // gauge and the "out" telops. Member names are guesses; return types
    // of the setters are guesses too (no caller reads them).
    class CCockpitBossCoder : public CCockpitBossLayoutBase {
        public:
            CCockpitBossCoder(long nId);
            virtual ~CCockpitBossCoder();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            virtual int LifeGaugeOut();
            virtual int LifeGaugeIn();
            virtual int LifeGaugeOutWait();
            virtual int LifeGaugeInWait();
            virtual int BossGaugeOut();
            virtual int BossGaugeIn();
            virtual int BossGaugeOutWait();
            virtual int BossGaugeInWait();
            virtual int CoinOut();
            virtual int CoinIn();
            virtual int CoinOutWait();
            virtual int CoinInWait();

            // The gauges of player nPlayer: fLife and fLife2 in [0, 1].
            void SetLifeValue(long nPlayer, float fLife, float fLife2);
            // Plays the gauge animation of state nState (0 to 3) on player
            // nPlayer's gauge.
            void SetLifeGaugeAnim(long nPlayer, long nState);
            // Layout 0 shows two players per team, layout 1 three.
            void SetLifeGaugeLayout(long nLayout);
            void SetSPGauge(float fRate);
            void SetSPGaugeDisp(int bDisp);
            void SetSPControllerState(long nTeam, int bDisp);
            // nDiff above 0: a coin gained, else a coin lost.
            void SetCoinValue(long nTeam, long nDiff);
            // 5: the gauge is not full, 6: it is.
            void SetCoinMaxState(long nState);
            void SetCoinState(long nTeam, long nState);
            void SetBossLifePercent(float fRate);
            // Above 0, 0 or below 0: which boss icon is shown (none for
            // below 0).
            void SetBossIcon(long nIcon);
            void SetFaceIcon();
            void SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl);
            void SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl, long nTeam, long nPlayer, const CChrCore *pChr);
            int SetTelopPowerUp(long nTeam, long nPlayer);
            // Shows (bOut set) or hides player nPlayer's "out" telop.
            void SetOutTelop(long nPlayer, int bOut);

        private:
            int SetupGame();

            long m_nPlayerNum;                           // 0x60, players per team in the layout
            long m_nLayout;                              // 0x64, see SetLifeGaugeLayout
            unsigned char m_Unk68[4];
            cockpit::CSPGauge m_SPGauge[1];              // 0x6C, one per team
            cockpit::CSPGaugeInputDisp m_SPInputDisp[1]; // 0x84, one per team
            cockpit::CEffectSet m_EffectSet;             // 0x90
    };
}

#endif /* !CCOCKPITBOSSCODER_HPP_ */
