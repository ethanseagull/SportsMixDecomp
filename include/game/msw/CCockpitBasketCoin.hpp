#ifndef CCOCKPITBASKETCOIN_HPP_
#define CCOCKPITBASKETCOIN_HPP_

#include "game/msw/CCockpitBasketLayoutBase.hpp"
#include "game/msw/cockpit/C3DCoin.hpp"

namespace bcs {
    struct FVEC3;
}

namespace nw4r {
    namespace lyt {
        struct Size;
    }
}

namespace msw {

    // Size 0x90. The basket cockpit's coin counters: one per team, each a
    // number in two text boxes and a 3D coin that pops up or drops when the
    // count changes (game/basket/CCockpitBasketCoin.cpp). The same class
    // exists per sport (CCockpitBasketCoin, CCockpitVolleyCoin,
    // CCockpitDodgeCoin, CCockpitBossCoin).
    class CCockpitBasketCoin : public CCockpitBasketLayoutBase {
        public:
            // What SetCoinState plays (names guessed from the C3DCoin action
            // each one starts).
            enum {
                COIN_STATE_RESET = 0, // at rest
                COIN_STATE_POPUP = 1, // a coin gained
                COIN_STATE_DROP = 4   // a coin lost
            };

            CCockpitBasketCoin(long nId);
            virtual ~CCockpitBasketCoin();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // The counters slide in and out with the cockpit's score part,
            // so the Score* parts play their animations and the Coin* parts
            // do nothing.
            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();
            virtual int CoinOut();
            virtual int CoinIn();
            virtual int CoinOutWait();
            virtual int CoinInWait();

            // Shows nValue on team nTeam's counter.
            void SetCoinValue(long nTeam, long nValue);
            // The count of team nTeam goes from nOld to nNew.
            void AddCoinValue(long nTeam, long nOld, long nNew);
            // Shows or hides both counters. Inline, defined in the .cpp.
            void SetCoinDisp(int bDisp);
            // nState is a COIN_STATE_*.
            void SetCoinState(long nTeam, long nState);
            // Empty in basket.
            void TelopMove(long, long);
            // Screen position and size of team nTeam's 3D coin pane: -1 for
            // an unknown team, -2 without the pane, else 0.
            long GetTargetInfo(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize);
            // (no symbol; added for game/basket/CCockpitBasket.cpp, which
            // reads the member directly) The 3D coin of team colour nIdx.
            cockpit::C3DCoin &Get3DCoin(long nIdx) { return m_3DCoin[nIdx]; }

        private:
            // The text for a count: empty for none. Inline, defined in the
            // .cpp (where its string has to come out).
            const wchar_t *GetCountText(long nValue);

            // Plays animation nAnim on every team's counter pane.
            void PlayCoinAnim(long nAnim);

            cockpit::C3DCoin m_3DCoin[2]; // 0x60, one per team
    };
}

#endif /* !CCOCKPITBASKETCOIN_HPP_ */
