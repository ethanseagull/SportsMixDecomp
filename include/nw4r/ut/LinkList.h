#ifndef NW4R_UT_LINKLIST_H_
#define NW4R_UT_LINKLIST_H_

#include "nw4r/db/Panic.hpp"

// The intrusive doubly linked list of nw4r. Written for this project from
// the code MenuF game/menu/CBgDisplay.cpp emits for it (the iterator's
// pointer check, with "LinkList.h" and its line in @STRING@ objects); the
// names follow the public ogws decompilation's description of nw4r::ut
// (from memory, no code copied). Only iteration is written.

namespace nw4r {
    namespace ut {

        // The links an element of a LinkList carries.
        struct LinkListNode {
            LinkListNode *mpNext; // 0x00
            LinkListNode *mpPrev; // 0x04
        };

        namespace detail {
            // The list itself: a count and a sentinel node.
            class LinkListImpl {
                protected:
                    unsigned long mSize; // 0x00
                    LinkListNode mNode;  // 0x04, the end of the list
            };
        }

        // A list of T whose LinkListNode is at byte offset Ofs of T.
        template <typename T, int Ofs>
        class LinkList : public detail::LinkListImpl {
            public:
                class Iterator {
                    public:
                        explicit Iterator(LinkListNode *pNode) : mpNode(pNode) {}

                        T *operator->() const { return GetPointerFromNode(mpNode); }
                        Iterator &operator++()
                        {
                            mpNode = mpNode->mpNext;
                            return *this;
                        }
                        bool operator!=(const Iterator &rOther) const { return mpNode != rOther.mpNode; }

                    private:
                        LinkListNode *mpNode;
                };

                Iterator GetBeginIter() { return Iterator(mNode.mpNext); }
                Iterator GetEndIter() { return Iterator(&mNode); }

                static T *GetPointerFromNode(LinkListNode *p)
                {
                    if (p == 0) {
                        db::Panic("LinkList.h", 573, "NW4R:Pointer must not be NULL (p)");
                    }
                    return reinterpret_cast<T *>(reinterpret_cast<char *>(p) - Ofs);
                }
        };
    }
}

#endif /* !NW4R_UT_LINKLIST_H_ */
