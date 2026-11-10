#ifndef CCOCKPITCHRICON2D_HPP_
#define CCOCKPITCHRICON2D_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace bcs {
    struct FVEC3;
}

namespace msw {

    // Lives in the DOL. A 2D icon over a character. Layout from the
    // constructor: CCockpitLayoutBase, then two words. Each sport derives
    // its own (CCockpitChrIcon2DDodge), which builds the layout in Set.
    // Return types and argument names of the virtuals are guesses. Set and
    // Remove have no name in the DOL (fn_80258EF0, fn_80258EF8), nor has the
    // four-argument SetChrIcon (fn_80259390), which is inline: the module
    // units that emit a derived vtable emit it weak, returning -1.
    class CCockpitChrIcon2D : public CCockpitLayoutBase {
        public:
            CCockpitChrIcon2D(long nId, float fSortZ, long nLayer, long nType);
            virtual ~CCockpitChrIcon2D();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            virtual long SetChrIcon(long nChr, const bcs::FVEC3 &rPos, long nUnk);
            virtual long SetChrIcon(long nChr, const bcs::FVEC3 &rPos, long nUnk, const void *pUnk) { return -1; }
            virtual void ClearChrIcon(long nChr, int bUnk, long nUnk);

            long SetChrIconImpl(long nChr, const bcs::FVEC3 &rPos, long nUnk);

        protected:
            long m_nType;  // 0x60: the constructor's last argument; which icon
            long m_nLayer; // 0x64: the constructor's third argument (name guessed)
    };
}

#endif /* !CCOCKPITCHRICON2D_HPP_ */
