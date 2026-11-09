#ifndef MENU_PADSTYLE_HPP_
#define MENU_PADSTYLE_HPP_

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace msw {
    namespace menu {

        // Shows one of two panes: the first when nStyle is 0, the second
        // otherwise, and hides the other. Either may be null. Lives in the
        // DOL; always returns 0. The file name is a guess.
        int SetDispPaneByPadStyle(nw4r::lyt::Pane *pPane0, nw4r::lyt::Pane *pPane1, int nStyle);
    }
}

#endif /* !MENU_PADSTYLE_HPP_ */
