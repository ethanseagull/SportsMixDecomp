#ifndef MSW_CGAMEBASKETHUNGRYBOSSPACKUN_HPP_
#define MSW_CGAMEBASKETHUNGRYBOSSPACKUN_HPP_

#include "game/msw/CGameBasket.hpp"

namespace msw {

    class CGame;

    namespace BASKET {
        // Makes the game task of the basket party game (CGame's game type
        // 2) against the hungry boss Piranha Plant. (BasketF)
        CGame *CreateGameBasketHungryBossPackun();
    }

    // The basket party game ("Harapeko", hungry boss Piranha Plant):
    // every character plays for itself and scores by feeding fruit to the
    // boss. Lives in BasketF (game/basket/CGameBasketHungryBossPackun.cpp),
    // size 0x2A8 (the size CreateGameBasketHungryBossPackun allocates).
    class CGameBasketHungryBossPackun : public CGameBasket {
        public:
            enum {
                PLAYER_MAX = 6 // the point-up telops kept per character
            };

            // Inline: CreateGameBasketHungryBossPackun builds the object
            // itself (only the base constructor is called).
            CGameBasketHungryBossPackun() : CGameBasket(0) {}
            virtual ~CGameBasketHungryBossPackun();

            virtual void PrepareRestartGame_();
            virtual void SendMessage_(long nMsg, long, long, long);
            virtual void CtorStateGameInit();
            virtual void CtorStateGameSetup();
            virtual void LoopStateGameSetup();
            virtual void CtorStateWaitStart();
            virtual void LoopStateWaitStart();
            virtual void DtorStateWaitStart();
            virtual void LoopStateMainLoop();
            virtual void CtorStateTimeUp();
            virtual void LoopStateTimeUp();
            virtual void DtorStateTimeUp();
            virtual void CtorStateGameSet();
            virtual void LoopStateGameSet();
            virtual void DtorStateGameSet();
            virtual void CtorStateGameResult();
            virtual void LoopStateGameResult();
            virtual void DtorStateGameResult();
            virtual void SetGameResult_();
            virtual void TrgGoalIn_(long nChr, long nTeam, long nShootType, long nKind, float,
                                    const bcs::FVEC3 &rPos);
            virtual int IsNowBonusTime_() const;
            virtual void EntranceDemoSetUp();
            virtual int IsGameSet_();
            virtual int MenuCallGameResult_();

            // Shows the crown over the characters of the leading teams.
            void UpDateCrownDisp();
            // Shows the "point up" telop over each character that has
            // just scored.
            void UpDatePointUpTelopInfo();

        private:
            // (inline, defined in the .cpp; names guessed) Takes the crown
            // off every character; not zero while a point-up telop waits.
            void ClearCrownDisp();
            bool IsPointUpWait() const;

            // 0x274: bit n set while character n wears the crown.
            unsigned long m_nCrownFlag;
            // 0x278: per character, a point-up telop is waiting to show.
            long m_bPointUp[PLAYER_MAX];
            // 0x290: the points it shows (-1 for none).
            long m_nPointUp[PLAYER_MAX];
    };
}

#endif /* !MSW_CGAMEBASKETHUNGRYBOSSPACKUN_HPP_ */
