#ifndef MSW_CBALLBASKET_HPP_
#define MSW_CBALLBASKET_HPP_

#include "game/msw/CBall.hpp"

namespace msw {

    // The ball of the basket game (BasketF), size 0x468 (the size
    // CBallManBasket::Create_ allocates).
    //
    // INCOMPLETE: only what game/basket/CBallManBasket.cpp needs. The
    // class's own virtual functions are declared in the order of the slots
    // 0x4F4 to 0x540 of __vt__Q23msw11CBallBasket, after CBall's; its
    // overrides of CBall's and its members are not, so it must not be
    // derived from or have its vtable emitted yet. Return types are
    // guessed (the JustShot frames are read as words and converted to
    // float by CBallManBasket).
    class CBallBasket : public CBall {
        public:
            CBallBasket(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallBasket();

            virtual void ReleaseBall(const bcs::FVEC3 &);                 // 0x4F4
            virtual void SetChrReflectMode(const bcs::FVEC3 &);           // 0x4F8
            virtual void SetChrReflectMode(long, float, float, float);    // 0x4FC
            virtual void CheckCollision__();                              // 0x500
            virtual void CollSubCheckBG_(bcs::FVEC3 &, bcs::FVEC3 &);     // 0x504
            virtual void TrgGoalIn_();                                    // 0x508
            virtual void RingHit();                                       // 0x50C
            // Not zero for the dummy balls (CBallBasketDummy).
            virtual int IsDummy() const;                                  // 0x510
            // The frames of the "just shot" timing window at the three
            // shot distances BASKET_PRM_S70_JS_{MIN,MID,MAX}_DIST.
            virtual long GetJustShotMinStartFrm() const;                  // 0x514
            virtual long GetJustShotMidStartFrm() const;                  // 0x518
            virtual long GetJustShotMaxStartFrm() const;                  // 0x51C
            virtual long GetJustShotMinEndFrm() const;                    // 0x520
            virtual long GetJustShotMidEndFrm() const;                    // 0x524
            virtual long GetJustShotMaxEndFrm() const;                    // 0x528
            virtual long GetJustShotSignEffectID() const;                 // 0x52C
            virtual float GetFloorBoundRateY() const;                     // 0x530
            virtual float GetFloorBoundRateXZ() const;                    // 0x534
            virtual float GetWallBoundRateXZ() const;                     // 0x538
            virtual void CalcBoundRandXZBG();                             // 0x53C
            virtual void SetShootResult(int);                             // 0x540

        private:
            unsigned char m_Unk318[0x468 - 0x318]; // 0x318
    };

    // A dummy ball of the basket game, size 0x468 (CBallManBasket's
    // CreateDummyBall_). (what it is used for is not known)
    class CBallBasketDummy : public CBallBasket {
        public:
            CBallBasketDummy(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallBasketDummy();
    };
}

#endif /* !MSW_CBALLBASKET_HPP_ */
