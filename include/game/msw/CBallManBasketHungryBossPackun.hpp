#ifndef MSW_CBALLMANBASKETHUNGRYBOSSPACKUN_HPP_
#define MSW_CBALLMANBASKETHUNGRYBOSSPACKUN_HPP_

#include "game/msw/CBallManBasket.hpp"

namespace msw {

    namespace BA_PARTY {
        class CHBData;
    }
    class CNpcStringBasket;

    // The ball manager of the basket party game (game type 2) against the
    // hungry boss Piranha Plant (BasketF), size 0x84 (the size
    // CBallManBasket::CreateInstance allocates). The fruit hang on strings
    // (CNpcStringBasket, one per wait entry of the party game's data); in
    // the bonus time of each turn some of them are picked for the players
    // to take. Member names are guesses.
    class CBallManBasketHungryBossPackun : public CBallManBasket {
        public:
            CBallManBasketHungryBossPackun(bcs::CTask *pParent);
            virtual ~CBallManBasketHungryBossPackun();

            virtual void MainFunc_();
            virtual void Create_();
            virtual void AllReset_();
            virtual int GetShootRateBase_() const;
            virtual int GetShootRate0_() const;
            virtual int GetShootRate1_() const;
            virtual int GetShootRate2_() const;
            virtual int GetShootRate3_() const;
            virtual float GetShootArea1_() const;
            virtual float GetShootArea2_() const;
            virtual float GetShootArea3_() const;
            virtual int GetAirBallRate_() const;
            virtual int IsAttachString_(const CBall *pBall) const;
            virtual int CatchEnable_(const CBall *pBall) const;
            virtual long GetFruitType_(const CBall *pBall) const;
            virtual long GetFruitScore_(const CBall *pBall) const;
            virtual long GetFruitScore_(long nType) const;

            // The radius of a fruit bomb's blast: BASKET_PRM_HB_BOMB_EXPLODE_R.
            // Static, out of line in the original. (used by
            // game/basket/CBallBasketFruit.cpp)
            static float GetBombExplodeR();
            // Not zero in the last turn (the bonus time).
            int IsNowBonusTime_() const;
            // Picks a free place of the grid to hang a fruit over: its index
            // goes to rIdx, its position to rPos.
            void GetAppendPos_(long &rIdx, bcs::FVEC3 &rPos) const;

        private:
            void UpDataWaitBall();
            // The turn the game time is in, from the time rates of the data.
            long CheckNowTurn() const;
            void UpDateBonusTelop();

            BA_PARTY::CHBData *m_pHBData;  // 0x60
            CNpcStringBasket **m_ppString; // 0x64, one per wait entry
            long m_nPickStep;              // 0x68, 0 off, 1 picking fruit, 2 a pick runs
            long m_nTelopStep;             // 0x6C
            long m_nTurn;                  // 0x70, -1 before the game
            float m_fBonusWait;            // 0x74, frames left of the bonus time
            float m_fPickTime;             // 0x78, frames left of the running pick
            float m_fPickWait;             // 0x7C, frames to the next pick
            float m_fTelopWait;            // 0x80

            // CNpcStringBasket reads the turn and the data.
            friend class CNpcStringBasket;
    };
}

#endif /* !MSW_CBALLMANBASKETHUNGRYBOSSPACKUN_HPP_ */
