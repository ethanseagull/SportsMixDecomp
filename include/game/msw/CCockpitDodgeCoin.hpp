#ifndef CCOCKPITDODGECOIN_HPP_
#define CCOCKPITDODGECOIN_HPP_

#include "game/msw/CCockpitDodgeLayoutBase.hpp"
#include "game/msw/cockpit/C3DCoin.hpp"

namespace msw {

    // Size 0x90. The dodge cockpit's coin counters: one per team, each a
    // number in two text boxes and a 3D coin that pops up or drops when the
    // count changes (game/dodge/CCockpitDodgeCoin.cpp). The same class
    // exists per sport (CCockpitBasketCoin, CCockpitVolleyCoin,
    // CCockpitDodgeCoin, CCockpitBossCoin).
    class CCockpitDodgeCoin : public CCockpitDodgeLayoutBase {
        public:
            // What SetCoinState plays (names guessed from the C3DCoin action
            // each one starts).
            enum {
                COIN_STATE_RESET = 0, // at rest
                COIN_STATE_POPUP = 1, // a coin gained
                COIN_STATE_USE = 2,   // coins spent (CCockpitDodge::SetCoinSpendAnim)
                COIN_STATE_DROP = 4   // a coin lost
            };

            CCockpitDodgeCoin(long nId);
            virtual ~CCockpitDodgeCoin();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // The counters slide in and out with the cockpit's coin part.
            virtual int CoinOut();
            virtual int CoinIn();
            virtual int CoinOutWait();
            virtual int CoinInWait();
            // New virtuals here (the dodge layout base has no score part;
            // nothing in DodgeF seen calling them): the same slides, as the
            // other sports' coin counters have them.
            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();

            // Shows nValue on team nTeam's counter.
            void SetCoinValue(long nTeam, long nValue);
            // The count of team nTeam goes from nOld to nNew.
            void AddCoinValue(long nTeam, long nOld, long nNew);
            void SetCoinDisp(int bDisp);
            // nState is a COIN_STATE_*; nNum is the number of coins for
            // COIN_STATE_USE.
            void SetCoinState(long nTeam, long nState, long nNum);

        private:
            // The text for a count: empty for none. Inline, defined in the
            // .cpp (where its string has to come out).
            const wchar_t *GetCountText(long nValue);

            // Plays animation nAnim on every team's counter pane.
            void PlayCoinAnim(long nAnim);

            cockpit::C3DCoin m_3DCoin[2]; // 0x60, one per team
    };
}

#endif /* !CCOCKPITDODGECOIN_HPP_ */
