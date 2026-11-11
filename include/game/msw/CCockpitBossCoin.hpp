#ifndef CCOCKPITBOSSCOIN_HPP_
#define CCOCKPITBOSSCOIN_HPP_

#include "game/msw/CCockpitBossLayoutBase.hpp"
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

    // Size 0x90. The boss cockpit's coin counter (game/boss/CCockpitBossCoin.cpp).
    // The same class as the other sports' CCockpit<Sport>Coin, but with one
    // team and its own layout tables instead of CoinRes's: the boss game's
    // copy of the code.
    class CCockpitBossCoin : public CCockpitBossLayoutBase {
        public:
            // What SetCoinState plays (names guessed from the C3DCoin action
            // each one starts).
            enum {
                COIN_STATE_RESET = 0, // at rest
                COIN_STATE_POPUP = 1, // a coin gained
                COIN_STATE_DROP = 4   // a coin lost
            };

            CCockpitBossCoin(long nId);
            virtual ~CCockpitBossCoin();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // The counter does not move with the life gauge; it slides with
            // the coin part.
            virtual int LifeGaugeOut();
            virtual int LifeGaugeIn();
            virtual int LifeGaugeOutWait();
            virtual int LifeGaugeInWait();
            virtual int CoinOut();
            virtual int CoinIn();
            virtual int CoinOutWait();
            virtual int CoinInWait();

            // Shows nValue on team nTeam's counter.
            void SetCoinValue(long nTeam, long nValue);
            // The count of team nTeam goes from nOld to nNew.
            void AddCoinValue(long nTeam, long nOld, long nNew);
            void SetCoinDisp(int bDisp);
            // nState is a COIN_STATE_*.
            void SetCoinState(long nTeam, long nState);
            // Screen position and size of team nTeam's 3D coin pane: -1 for
            // an unknown team, -2 without the pane, else 0. Nothing in BossF
            // calls it.
            long GetTargetInfo(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize);
            // Team nIdx's 3D coin (CCockpitBoss::Get3DCoinTaskImpl).
            cockpit::C3DCoin &Get3DCoin(long nIdx) { return m_3DCoin[nIdx]; }

        private:
            // Inline, defined in the .cpp (where their constants have to
            // come out).
            // Shows the counters when the game starts.
            void SetupGame();
            // Plays animation nAnim on every team's counter pane.
            void PlayAnimCoin(long nAnim);
            // The text for a count: empty for none.
            const wchar_t *GetCountText(long nValue);

            cockpit::C3DCoin m_3DCoin[2]; // 0x60, one per team (only the first is used)
    };
}

#endif /* !CCOCKPITBOSSCOIN_HPP_ */
