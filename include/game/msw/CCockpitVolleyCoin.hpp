#ifndef CCOCKPITVOLLEYCOIN_HPP_
#define CCOCKPITVOLLEYCOIN_HPP_

#include "game/msw/CCockpitVolleyLayoutBase.hpp"
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

    // Size 0x90. The volley cockpit's coin counters: one per team, each a
    // number in two text boxes and a 3D coin that pops up or drops when the
    // count changes (game/volley/CCockpitVolleyCoin.cpp). The same class
    // exists per sport (CCockpitBasketCoin, CCockpitVolleyCoin,
    // CCockpitDodgeCoin, CCockpitBossCoin).
    class CCockpitVolleyCoin : public CCockpitVolleyLayoutBase {
        public:
            // What SetCoinState plays (names guessed from the C3DCoin action
            // each one starts).
            enum {
                COIN_STATE_RESET = 0, // at rest
                COIN_STATE_POPUP = 1, // a coin gained
                COIN_STATE_DROP = 4   // a coin lost
            };

            CCockpitVolleyCoin(long nId);
            virtual ~CCockpitVolleyCoin();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // As in hockey: the counters slide in and out with the cockpit's
            // score part, so the Score* parts play their animations and the
            // Coin* parts do nothing.
            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();
            virtual int CoinOut();
            virtual int CoinIn();
            virtual int CoinOutWait();
            virtual int CoinInWait();

            // Plays the counters' part of serve-score animation nKind (0-3).
            long SetServeScoreAnim(long nKind);
            // Shows nValue on team nTeam's counter.
            void SetCoinValue(long nTeam, long nValue);
            // The count of team nTeam goes from nOld to nNew.
            void AddCoinValue(long nTeam, long nOld, long nNew);
            void SetCoinDisp(int bDisp);
            // nState is a COIN_STATE_*.
            void SetCoinState(long nTeam, long nState);
            // Empty in volley.
            void TelopMove(long, long);
            // Screen position and size of team nTeam's 3D coin pane: -1 for
            // an unknown team, -2 without the pane, else 0.
            long GetTargetInfo(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize);
            // Team colour nIdx's 3D coin (CCockpitVolley::Get3DCoinTaskImpl).
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

#endif /* !CCOCKPITVOLLEYCOIN_HPP_ */
