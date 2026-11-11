#ifndef SELECTMENU_CBACKGROUND_HPP_
#define SELECTMENU_CBACKGROUND_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x170. The background of the select menu screens ("me_bg"):
        // the tag in the corner with the mode's colour bar and icon, and the
        // line of text that scrolls along the bottom. Driven by events sent
        // from the screens (SendEvent). Member names are guesses.
        class CBackground : public CSelectMenuLayoutBase {
            public:
                CBackground(long nId);
                virtual ~CBackground();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                // Puts a bullet and pText into the scrolling line and starts
                // the scroll; -1 when pText is null (the line is hidden),
                // -2 when the text is too long, else 0.
                long SetLineTextString(const wchar_t *pText);
                // Plays line animation nAnim (negative: stops them and hides
                // the line), unless it is the current one.
                void SetLineAnim(long nAnim);
                // Plays text animation nAnim; any other value stops them and
                // hides the text.
                void SetTextAnim(long nAnim);

            private:
                // Inline in the game; only its local statics have symbols.
                void SetTagAnim(long nAnim);

                wchar_t m_wLineText[0x80];    // 0x5C, the text of the scrolling line
                long m_nTagNo;                // 0x15C, the colour bar shown, -1 for none
                float m_fTextWidth;           // 0x160, width of the line's text
                nw4r::lyt::Size m_RangeSize;  // 0x164, size of the "anzen_range" pane
                long m_nLineAnim;             // 0x16C, the current line animation, -1 at first
        };
    }
}

#endif /* !SELECTMENU_CBACKGROUND_HPP_ */
