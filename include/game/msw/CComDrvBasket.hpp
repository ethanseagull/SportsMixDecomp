#ifndef MSW_CCOMDRVBASKET_HPP_
#define MSW_CCOMDRVBASKET_HPP_

#include "game/msw/CComDrv.hpp"

namespace msw {

    // INCOMPLETE: the computer driver of the basketball game (BasketF).
    // Its vtable (__vt__Q23msw13CComDrvBasket, 59 words) adds three virtuals
    // after CComDrv's: IsScoureWin, MethdBKRun, CanbeJumpBlock. Only the
    // overrides that derived classes need are declared; layout.py finds
    // members up to 0x16C in the CComDrv part only. Return types are guesses.
    class CComDrvBasket : public CComDrv {
        public:
            CComDrvBasket(long nArg);
            virtual ~CComDrvBasket();

            virtual int MethdJump();
            virtual void Reset();

            virtual int IsScoureWin() const;
            virtual int MethdBKRun(const bcs::FVEC3 &pos, float, float, int);
            virtual int CanbeJumpBlock();
    };
}

#endif /* !MSW_CCOMDRVBASKET_HPP_ */
