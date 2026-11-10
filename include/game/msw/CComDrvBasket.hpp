#ifndef MSW_CCOMDRVBASKET_HPP_
#define MSW_CCOMDRVBASKET_HPP_

#include "game/msw/CComDrv.hpp"

namespace msw {

    class CChrCore;

    // The computer driver of the basketball game (BasketF). No members of
    // its own were found.
    //
    // Virtual functions in the order of __vt__Q23msw13CComDrvBasket: the
    // overrides of CComDrv's slots, then three of its own (0xE0-0xE8). The
    // four with a body here come out as weak functions after CComDrv's in
    // the original, so they are inline; the others (MethdJump, MethdBKRun,
    // CanbeJumpBlock) and the shot/dunk/cut helpers are in
    // game/basket/unit_7E8FC.cpp. Return types other than the vtable's are
    // guesses.
    class CComDrvBasket : public CComDrv {
        public:
            CComDrvBasket(long nArg);
            virtual ~CComDrvBasket();

            virtual int MethdJump();
            virtual void Reset() { CComDrv::Reset(); }
            virtual void ClearAll() { CComDrv::ClearAll(); }
            virtual void UpdateInfo() { CComDrv::UpdateInfo(); }
            // (spelling as in the symbol)
            virtual int IsScoureWin() const { return 0; }
            virtual int MethdBKRun(const bcs::FVEC3 &pos, float, float, int);
            virtual int CanbeJumpBlock();

            long GetChrTypeID() const;
            CChrCore *GetChrCore(long nChr);
            float GetRotY_P1ToP2XZ(const CChrCore *p1, const CChrCore *p2);
            float GetRotY_P1ToP2XZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2);
            // The computer driver of pChr; null when a computer does not
            // drive it.
            CComDrvBasket *GetComDrv(const CChrCore *pChr) const;
            // Not zero when this character can cut (steal from) pChr, which
            // must hold the ball.
            int IsCutHit(const CChrCore *pChr) const;

            int CanbeJumpShoot();
            int MethdJumpShoot(float);
            int CanbeDunk();
            int MethdDunk();
            int CanbeLayUp() const;
            int CanbeCut();
            int MethdCut(float);
            int MethdJumpBlock(float);
            int CanbeUpperCut();
            int MethdUpperCut();
            int CanbeJumpCut() const;
    };
}

#endif /* !MSW_CCOMDRVBASKET_HPP_ */
