#ifndef MSW_COCKPIT_CTIMECOUNT_HPP_
#define MSW_COCKPIT_CTIMECOUNT_HPP_

namespace msw {

    class CMenuPaneAccessor;

    namespace cockpit {

        // A minutes:seconds clock and a one-digit countdown drawn in layout
        // text boxes. Lives in the DOL (game/msw/cockpit/CMissionCount.cpp).
        // Size 0x8: one word, then the vptr at 0x04 (its vtable and inline
        // destructor are emitted by the modules).
        class CTimeCount {
            public:
                // Inline: BasketF CCockpitBasketHarapeko and CCockpitBasketCoder
                // hold one.
                CTimeCount() { Initialize(); }

                void Initialize();
                void Finalize();
                // Whole seconds in fFrame frames (60 a second), rounded up
                // from the tenth of a second.
                long GetSec(float fFrame);
                // Writes fFrame as minutes into text box nMinPane and as
                // ":seconds" into nSecPane, and plays animation nAnim on a
                // pane whose text changed (on both when nMode is 1). The
                // other two arguments are not read. -1 without
                // pAccessor, else 0.
                long SetTimeValue(CMenuPaneAccessor *pAccessor, float fFrame, int nUnused0, long nAnim,
                                  long nUnused1, long nMinPane, long nSecPane, int nMode);
                // Shows nCount (0 to 9) in text box nTextPane and plays nAnim
                // on pane nPane; hides nPane for any other count. -1 without
                // pAccessor, else 0.
                long SetCountDown(CMenuPaneAccessor *pAccessor, long nCount, long nPane, long nTextPane,
                                  long nAnim);

                // The second shown last, -1 before the first (BasketF
                // CCockpitBasketHarapeko reads and writes it).
                long GetShowSec() const { return m_nUnk00; }
                void SetShowSec(long nSec) { m_nUnk00 = nSec; }

            private:
                long m_nUnk00; // 0x00, -1 after Initialize

            public:
                // Declared after the data: MWCC puts the vptr where the
                // first virtual function is declared. The destructor calls
                // Finalize (seen in the emitted copies in BasketF CCockpitBasketCoder).
                virtual ~CTimeCount() { Finalize(); }
        };
    }
}

#endif /* !MSW_COCKPIT_CTIMECOUNT_HPP_ */
