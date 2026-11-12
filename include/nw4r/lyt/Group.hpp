#ifndef NW4R_LYT_GROUP_HPP_
#define NW4R_LYT_GROUP_HPP_

#include "nw4r/ut/LinkList.h"

// Groups of panes in a layout. Live in the DOL. Only what MenuF
// game/menu/CBgDisplay.cpp needs; names follow the public ogws
// decompilation's description of nw4r::lyt (from memory, no code copied).

namespace nw4r {
    namespace lyt {
        class Pane;

        namespace detail {
            // A pane's entry in a group.
            struct PaneLink {
                ut::LinkListNode mLink; // 0x00
                Pane *mTarget;          // 0x08
            };
        }

        typedef ut::LinkList<detail::PaneLink, 0> PaneLinkList;

        class Group {
            public:
                virtual ~Group();

                PaneLinkList &GetPaneList() { return mPaneLinkList; }

            private:
                ut::LinkListNode mLink;      // 0x04
                PaneLinkList mPaneLinkList;  // 0x0C
        };

        class GroupContainer {
            public:
                Group *FindGroupByName(const char *pName);
        };
    }
}

#endif /* !NW4R_LYT_GROUP_HPP_ */
