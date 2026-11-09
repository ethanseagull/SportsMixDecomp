#ifndef CMENUPANEACCESSOR_HPP_
#define CMENUPANEACCESSOR_HPP_

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace bcs {
    namespace G2D {
        class CPaneCtrl;
        class CPaneObj;
    }
}

namespace msw {

    // Lives in the DOL. Size 0x10 (the size passed to operator new in
    // CSelectMenuLayoutBase::SetPaneAccessor), vptr at 0x0C. Gives access to
    // the child panes of a layout by their index in a name table. Member
    // names are guesses.
    //
    // The same class as in game/msw/pending/CMenuPaneAccessor.hpp (TourF
    // session), whose additions are merged below; that copy can go.
    class CMenuPaneAccessor {
        public:
            CMenuPaneAccessor(bcs::G2D::CPaneCtrl *pPaneCtrl, const char **ppPaneNames, long nNum);

            // -1 when the pane is not found, else 0. The names of the two
            // ints are guesses (they match the local statics `recursive`
            // and `reset` that callers pass).
            long PlayAnimation(long nPane, long nAnim, float fSpeed, float fFrame, int bRecursive, int bReset);
            // 1 when the pane is not found.
            int IsAnimEnd(long nPane, long nAnim);

            // Does not check the index.
            const char *GetPaneName(long nPane);
            // Both are null when pName or the layout is.
            nw4r::lyt::Pane *GetPane(const char *pName, int bRecursive, nw4r::lyt::Pane *pRoot);
            bcs::G2D::CPaneObj *GetPaneObj(const char *pName);

            // (no symbols; the overloads are a guess) The same by index,
            // null when the index is outside the name table. Inline in the
            // modules.
            nw4r::lyt::Pane *GetPane(long nPane)
            {
                return (unsigned long)nPane < m_nNum ? GetPane(GetPaneName(nPane), 1, 0) : 0;
            }

            bcs::G2D::CPaneObj *GetPaneObj(long nPane)
            {
                return (unsigned long)nPane < m_nNum ? GetPaneObj(GetPaneName(nPane)) : 0;
            }

            bcs::G2D::CPaneCtrl *m_pPaneCtrl; // 0x00
            const char **m_ppPaneNames;       // 0x04
            unsigned long m_nNum;             // 0x08

            // vptr at 0x0C
            virtual ~CMenuPaneAccessor();
    };
}

#endif /* !CMENUPANEACCESSOR_HPP_ */
