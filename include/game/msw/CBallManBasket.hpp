#ifndef MSW_CBALLMANBASKET_HPP_
#define MSW_CBALLMANBASKET_HPP_

#include "game/msw/CBallMan.hpp"

namespace msw {

    // The ball manager of the basketball game (BasketF).
    //
    // INCOMPLETE: only what game/basket/CCameraBasketHB.cpp needs. The class's
    // own virtual functions are declared in the order of the slots 0xB0 to
    // 0xF0 of __vt__Q23msw14CBallManBasket, after CBallMan's; its overrides of
    // CBallMan's, its constructor and its members are not declared yet, so it
    // must not be instantiated or derived from. Return types are read from
    // the bodies where they are one instruction, otherwise guessed.
    class CBallManBasket : public CBallMan {
        public:
            virtual ~CBallManBasket();

            virtual int GetShootRateBase_() const;                    // 0xB0
            virtual int GetShootRate0_() const;                       // 0xB4
            virtual int GetShootRate1_() const;                       // 0xB8
            virtual int GetShootRate2_() const;                       // 0xBC
            virtual int GetShootRate3_() const;                       // 0xC0
            virtual float GetShootArea1_() const;                     // 0xC4
            virtual float GetShootArea2_() const;                     // 0xC8
            virtual float GetShootArea3_() const;                     // 0xCC
            virtual int GetAirBallRate_() const;                      // 0xD0
            virtual float GetAirBallRangeDistMin_() const;            // 0xD4
            virtual float GetAirBallRangeDistMax_() const;            // 0xD8
            virtual CBall *CreateFruitBall_(long, long, int);         // 0xDC
            // Not zero while pBall hangs on a string (zero in this class).
            virtual int IsAttachString_(const CBall *pBall) const;    // 0xE0
            virtual int CatchEnable_(const CBall *pBall) const;       // 0xE4
            virtual long GetFruitType_(const CBall *pBall) const;     // 0xE8
            virtual long GetFruitScore_(const CBall *pBall) const;    // 0xEC
            virtual long GetFruitScore_(long nType) const;            // 0xF0

            // (guessed) The instance as this class: the callers call
            // CBallMan::GetInstance and use the result without a null check.
            static CBallManBasket *GetInstance()
            {
                return static_cast<CBallManBasket *>(CBallMan::GetInstance());
            }
    };
}

#endif /* !MSW_CBALLMANBASKET_HPP_ */
