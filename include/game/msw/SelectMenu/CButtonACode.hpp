#ifndef SELECTMENU_CBUTTONACODE_HPP_
#define SELECTMENU_CBUTTONACODE_HPP_

#include "game/msw/SelectMenu/CButtonA.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C. CButtonA's window with the "main_fmake_02" layout: the
        // same A button, plus a code text box ("t_fcode02") and a cursor pane
        // ("tex_csr") that plays its own animation. The other virtuals are
        // CButtonA's.
        class CButtonACode : public CButtonA {
            public:
                CButtonACode(long nId);
                virtual ~CButtonACode();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

            protected:
                // Inline in the game; only its local statics have symbols.
                // CButtonACode2::Set() uses it too.
                void InitPaneAnim();
        };

        // Size 0x5C. The "main_fmake_06" version of CButtonACode: two
        // message lines ("t_mes01", "t_mes02") and the code box "t_fcode".
        class CButtonACode2 : public CButtonACode {
            public:
                CButtonACode2(long nId);
                virtual ~CButtonACode2();
                virtual int Set();

                virtual long SetMessageText(long nLine, const wchar_t *pText);
        };
    }
}

#endif /* !SELECTMENU_CBUTTONACODE_HPP_ */
