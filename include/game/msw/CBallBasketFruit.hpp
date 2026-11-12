#ifndef MSW_CBALLBASKETFRUIT_HPP_
#define MSW_CBALLBASKETFRUIT_HPP_

#include "game/msw/CBallBasket.hpp"

namespace msw {

    // A fruit ball of the Hungry Boss Packun court (BasketF), size 0x478
    // (CBallManBasketHungryBossPackun allocates it): CBallBasket plus four
    // words. A fruit hangs on a string NPC or is held by a character, and
    // once loose it fades out when its life runs out. One kind is a bomb.
    // Member names are guesses.
    class CBallBasketFruit : public CBallBasket {
        public:
            // Values of m_nKind: the order of the BASKET_PRM_S70_* tables.
            enum KIND {
                KIND_ORANGE,
                KIND_BLUE_APPLE,
                KIND_RED_APPLE,
                KIND_SUICA,
                KIND_MELON, // (name guessed from the "ME" parameters)
                KIND_BOMB
            };

            // Bits of m_nFruitFlag. (names guessed)
            enum {
                FRUIT_FLAG_DEAD = 0x1,      // life ran out (or scored): fades out
                FRUIT_FLAG_2 = 0x2,         // (meaning not known) cleared when a normal shot starts
                FRUIT_FLAG_ON_CHR = 0x8     // attached to a character, not a string
            };

            CBallBasketFruit(bcs::CTask *pParent, long nBallNo, long nKind);
            virtual ~CBallBasketFruit();

            virtual void Reset();
            virtual float GetBgCollRadius() const;
            virtual float GetBgCollRadiusXZ() const;
            virtual float GetGravityDefault() const;
            virtual void DoExplode(float, float);
            virtual int IsBaExplode() const;
            virtual int IsActive() const;
            virtual int IsCanbeTurnOver() const { return 1; }
            virtual int CanbeCatch() const;
            virtual void ResetOnDetach_();
            virtual void UpdateDisp_();
            virtual void CtorStateFreeMove();
            virtual void LoopStateFreeMove();
            virtual void CtorStateAttach();
            virtual void CtorStateNormalShoot();
            virtual void DtorStateNormalShoot();
            virtual void DtorStateLayupShoot();
            virtual long CallbackBodyDamage_(const CCollAtkUnit *, const bcs::FVEC3 &);
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);
            virtual void TrgGoalIn_();
            virtual long GetJustShotMinStartFrm() const;
            virtual long GetJustShotMidStartFrm() const;
            virtual long GetJustShotMaxStartFrm() const;
            virtual long GetJustShotMinEndFrm() const;
            virtual long GetJustShotMidEndFrm() const;
            virtual long GetJustShotMaxEndFrm() const;
            virtual long GetJustShotSignEffectID() const;
            virtual float GetFloorBoundRateY() const;
            virtual float GetFloorBoundRateXZ() const;
            virtual float GetWallBoundRateXZ() const;
            virtual void CalcBoundRandXZBG();

            // (names guessed; added for
            // game/basket/CBallManBasketHungryBossPackun.cpp) See m_nStringId
            // and m_nKind.
            unsigned long GetStringId() const { return m_nStringId; }
            void SetStringId(unsigned long nId) { m_nStringId = nId; }
            long GetKind() const { return m_nKind; }

        private:
            // The life of a loose fruit, drawn between the
            // BASKET_PRM_COMMON_HUNGRY_*_BALL_LIFE_MIN/MAX of a fruit held by
            // a character or of one on a string.
            inline float RandLife() const;

        private:
            // Sets m_nStringId when it hangs a fruit on a string.
            friend class CBallManBasketHungryBossPackun;

            // The task id of the string NPC (msw::CNpcStringBasket) the fruit
            // hangs on, zero when there is none.
            unsigned long m_nStringId; // 0x468
            unsigned long m_nFruitFlag; // 0x46C
            long m_nKind;              // 0x470, a KIND
            float m_fLife;             // 0x474, frames left once loose
    };
}

#endif /* !MSW_CBALLBASKETFRUIT_HPP_ */
