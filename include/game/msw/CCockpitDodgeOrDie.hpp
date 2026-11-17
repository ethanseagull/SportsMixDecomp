#ifndef CCOCKPITDODGEORDIE_HPP_
#define CCOCKPITDODGEORDIE_HPP_

#include "game/msw/CCockpitDodgeLayoutBase.hpp"
#include "game/msw/cockpit/CEffectSet.hpp"
#include "game/msw/cockpit/CTimeCount.hpp"

namespace msw {
    class CChrCore;

    // Size 0xB4 (CCockpitDodge::CreateObjImpl allocates it). The board of
    // the "dodge or die" game: time, each player's life, face icons and the
    // "out" telop (DodgeF game/dodge/CCockpitDodgeOrDie.cpp). It overrides
    // only the gauge slides of CCockpitDodgeLayoutBase. Return types and
    // argument names are guesses. nNo is a player slot on the board (0 to
    // 3); the first argument of the Set* functions (nIdx) is not read.
    class CCockpitDodgeOrDie : public CCockpitDodgeLayoutBase {
        public:
            CCockpitDodgeOrDie(long nId);
            virtual ~CCockpitDodgeOrDie();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            virtual int GaugeOut();
            virtual int GaugeIn();
            virtual int GaugeOutWait();
            virtual int GaugeInWait();

            void SetTimeDisp(int bDisp);
            void SetLifeDisp(int bDisp);
            // The clock at fTime frames, and the countdown of the last
            // seconds.
            void SetTimeValue(float fTime, int nArg, int bArg);
            // The two life bars of player nNo, each a ratio from 0 to 1.
            void SetLifeValue(long nIdx, long nNo, float fRate, float fRate2);
            // Puts pChr's face, its player or COM mark and its star (shown
            // for a star player) on player nNo's gauge.
            void SetFaceIcon(long nIdx, long nNo, const CChrCore *pChr);
            // nState 1 to 3: a hit animation of the gauge, else the plain one.
            void SetLifeGaugeAnim(long nIdx, long nNo, long nState);
            // 0: a gauge per player; 1: the first two players share the
            // left gauge; 2: the first three share it.
            void SetLifeGaugeLayout(long nLayout);
            // Plays the "out" telop on player nNo's gauge (bOut nonzero) or
            // takes it away.
            void SetOutTelop(long nIdx, long nNo, int bOut);

            // The panes of one player's gauge (defined in the .cpp).
            struct LifePane;

        private:
            // (defined in the .cpp) The panes of player nNo's gauge in the
            // current layout; null for an unknown layout.
            const LifePane *GetLifePane(long nNo);
            // (stripped, auto-inlined into Main) Slides the gauges in and
            // plays the first animation of every player's gauge.
            void SetupGame();
            // (stripped, auto-inlined into SetLifeGaugeAnim)
            void LifeGaugeAnim(long nIdx, long nNo, long nAnim, float fSpeed, float fFrame);
            // (inline, defined in the .cpp) Plays slide animation nAnim on
            // both halves of the board and on the clock.
            void PlayAnimGauge(long nAnim);

            cockpit::CTimeCount m_TimeCount;  // 0x60
            cockpit::CEffectSet m_EffectSet;  // 0x68
            long m_nPlayerNum;                // 0xAC, 4
            long m_nLayout;                   // 0xB0, see SetLifeGaugeLayout
    };
}

#endif /* !CCOCKPITDODGEORDIE_HPP_ */
