#ifndef SELECTMENU_CCOURTCHR_HPP_
#define SELECTMENU_CCOURTCHR_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "revolution/rfl.h"

namespace msw {
    namespace SelectMenu {

        // Size 0xFC (layout.py). The characters standing on the court of
        // the menu background (one CCourtCharaObj each). Menu object 0x3B of
        // CSelectMenu. Only what game/menu/CPlayMode.cpp needs so far: the
        // members are not written, so do not create one through this
        // header. Argument names and return types are guesses.
        class CCourtChr : public CSelectMenuLayoutBase {
            public:
                virtual ~CCourtChr();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual int Set();
                virtual void Remove();

                // Puts a character model in place nIndex; its handle, or 0.
                unsigned long SetCharaModel(long nIndex, long nChara, long nUnk2, long nUnk3, long nUnk4,
                                            RFLDataSource source, long nMii, int bStar);
                void SetCharaMotion(long nIndex, long nMotion, long nUnk, float fSpeed, float fFrame,
                                    float fBlend, int bUnk);
                // The model handle of place nIndex, current or about to
                // replace it.
                unsigned long GetMdlHandleByCharaId(long nIndex);
                unsigned long GetNextMdlHandleByCharaId(long nIndex);
                void WaitToPlayAnim(float fWait, long nAnim, float fSpeed);
        };
    }
}

#endif /* !SELECTMENU_CCOURTCHR_HPP_ */
