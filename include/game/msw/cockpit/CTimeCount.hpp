#ifndef MSW_COCKPIT_CTIMECOUNT_HPP_
#define MSW_COCKPIT_CTIMECOUNT_HPP_

namespace msw {
    class CMenuPaneAccessor;

    namespace cockpit {

        // Size 8, vptr at 0x04. Lives in the DOL. The time display of a
        // cockpit: puts a time into text box panes and counts the last
        // seconds down. Its constructor and destructor are inline (BasketF
        // CCockpitBasketHarapeko and CCockpitBasketCoder hold one; the
        // modules emit the destructor and vtable weak). Argument names and
        // the return types the callers ignore are guesses.
        class CTimeCount {
            public:
                CTimeCount() { Initialize(); }
                virtual ~CTimeCount() { Finalize(); }

                // Sets m_nSec to -1.
                void Initialize();
                // Empty in the DOL.
                void Finalize();
                // The whole seconds of fTime.
                long GetSec(float fTime);
                // Shows fTime in the text boxes nPaneA to nPaneD of pAccessor.
                void SetTimeValue(CMenuPaneAccessor *pAccessor, float fTime, int nArg, long nPaneA, long nPaneB,
                                  long nPaneC, long nPaneD, int bArg);
                // Shows the count-down second nSec.
                void SetCountDown(CMenuPaneAccessor *pAccessor, long nSec, long nPaneA, long nPaneB, long nAnim);

                // The second shown last; -1 before the first.
                long GetShowSec() const { return m_nSec; }
                void SetShowSec(long nSec) { m_nSec = nSec; }

            private:
                long m_nSec; // 0x00
        };
    }
}

#endif /* !MSW_COCKPIT_CTIMECOUNT_HPP_ */
