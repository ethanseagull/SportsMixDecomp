#ifndef MSW_CBALLMANBASKET_HPP_
#define MSW_CBALLMANBASKET_HPP_

#include "game/msw/CBallMan.hpp"

namespace msw {

    // The ball manager of the basketball game (BasketF), size 0x60 (the size
    // CreateInstance allocates). Its functions are in
    // game/basket/CBallManBasket.cpp; the party game's manager
    // (CBallManBasketHungryBossPackun) derives from it.
    //
    // The overrides of CBallMan's virtual functions and the class's own
    // ones (vtable slots 0xB0 to 0xF0) are declared in the order of
    // __vt__Q23msw14CBallManBasket. The functions with a body are the ones
    // BasketF emits as weak functions. Return types are read from the bodies
    // where they are one instruction, otherwise guessed.
    class CBallManBasket : public CBallMan {
        public:
            // Makes the ball manager the game type needs (the party game's
            // own for game type 2) and stores it in CBallMan::m_pInstance.
            static CBallMan *CreateInstance(bcs::CTask *pParent);

            CBallManBasket(bcs::CTask *pParent, long nBallMax);
            virtual ~CBallManBasket();

            virtual CBall *GetNearPosBall_(const bcs::FVEC3 &rPos);
            // One ball and its marker.
            virtual void Create_();
            virtual void AllReset_();
            virtual int IsInfinityReboundMode_() const;
            virtual int IsDisableReboundMode_() const;
            virtual int IsDisableAirBallMode_() const;
            virtual void SetInfinityReboundMode_(int bOn);
            virtual void SetDisableReboundMode_(int bOn);
            virtual void SetDisableAirBallMode_(int bOn);
            virtual int IsBallNowRebound_(long nBall);
            virtual float GetRaiseGoalInRate_() const { return m_fRaiseGoalInRate; }

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
            // The fruit of the party game; none here.
            virtual CBall *CreateFruitBall_(long, long, int) { return 0; } // 0xDC
            // Not zero while pBall hangs on a string (zero in this class).
            virtual int IsAttachString_(const CBall *pBall) const { return 0; } // 0xE0
            virtual int CatchEnable_(const CBall *pBall) const;       // 0xE4
            virtual long GetFruitType_(const CBall *pBall) const { return -1; } // 0xE8
            virtual long GetFruitScore_(const CBall *pBall) const { return 0; } // 0xEC
            virtual long GetFruitScore_(long nType) const { return 0; }        // 0xF0

            // Not zero for a dummy ball (CBallBasketDummy).
            int IsDummyBall_(const CBall *pBall) const;
            // Makes a dummy ball with the lowest free ball number and adds
            // it to the manager's table; returns it (CBallMan::AddBall's
            // result; CChrCoreBasketWaluigi uses it). (static: the body
            // takes no object)
            static CBall *CreateDummyBall_();
            // (meaning of the result guessed) Whether a ball curving round a
            // circle of radius fArg + fRadius between rFrom and rTo stays on
            // the court: 0 when both sides do, 4 when only the right one
            // does, 2 otherwise.
            long CheckCurveDir_(float fArg, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius) const;
            // The lowest ball number no ball in the table has.
            long GetUseEnableIndex_() const;
            // The start and the end frame of the "just shot" timing for a
            // shot from rFrom to rTo with pBall, blended by the horizontal
            // distance between the ball's frames for the three distances
            // BASKET_PRM_S70_JS_*_DIST. 0 without a ball, or while the boss
            // Piranha Plant is in state 0.
            long GetJustShotTimingMin_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, const CBall *pBall) const;
            long GetJustShotTimingMax_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, const CBall *pBall) const;

            // (guessed) The instance as this class: the callers call
            // CBallMan::GetInstance and use the result without a null check.
            static CBallManBasket *GetInstance()
            {
                return static_cast<CBallManBasket *>(CBallMan::GetInstance());
            }

        private:
            // Bits of m_nModeFlag. (names from the functions that set them)
            enum {
                MODE_DISABLE_AIRBALL = 0x1,
                MODE_DISABLE_REBOUND = 0x2,
                MODE_INFINITY_REBOUND = 0x4
            };

            float m_fRaiseGoalInRate;  // 0x58, 0 from the constructor (name from GetRaiseGoalInRate_)
            unsigned long m_nModeFlag; // 0x5C
    };
}

#endif /* !MSW_CBALLMANBASKET_HPP_ */
