#ifndef CGAMEBASKET_HPP_
#define CGAMEBASKET_HPP_

#include "game/msw/CGame.hpp"

namespace msw {

    // Lives in module BasketF. Size 0x274.
    class CGameBasket : public CGame {
        public:
            CGameBasket(bcs::CTask *pParent);

            // Creates the game the current settings ask for: this class or
            // the basketball party game.
            static CGame *CreateInstance(); // return type guessed

            // Added for game/basket/CNpcGhostBasketBase.cpp (names from the
            // vtable and the DOL symbols; return types and meanings
            // guessed). Slots 0x2A8 to 0x2B0 of __vt__Q23msw11CGameBasket.
            virtual void ForceGameExit_(long);
            // Gives the ball to team nTeam.
            virtual void SetOffenseTeam_(long nTeam);
            virtual void StopEffect_(float, float, float);

            // The instance as this class (guessed inline: the callers read
            // CGame::m_pInstance directly).
            static CGameBasket *GetInstance() { return static_cast<CGameBasket *>(m_pInstance); }
            // The NPC type of team nTeam's ghost (CChrMan::NpcSearch finds
            // it by that).
            static long GetTeamGhostType(long nTeam);
            // Not zero while the game's main loop runs (play is on).
            int IsNowMainLoopEnable_() const;
            // (added for game/basket/CComManBasket.cpp) The radius of the
            // three-point line.
            static float GetCourt3PointSize();

        private:
            unsigned char m_Unk150[0x274 - sizeof(CGame)];
    };
}

#endif /* !CGAMEBASKET_HPP_ */
