#ifndef NW4R_UT_LINKLIST_H_
#define NW4R_UT_LINKLIST_H_

#include "nw4r/db/Panic.hpp"

// The DOL's NW4R intrusive doubly linked list (nw4r::ut::LinkList). Written
// for this project from what bcs::G2D::CLayoutBase::FindAnimTransform emits:
// the list's size word, then its end node; a node is (next, prev). The two
// iterator checks report file "LinkList.h", lines 403 and 573, which is why
// the line numbers are literal here. Names follow the public ogws
// decompilation's description of nw4r::ut (from memory, no code copied).

namespace nw4r {
    namespace ut {

        class LinkListNode {
            public:
                LinkListNode *GetNext() const { return mNext; }
                LinkListNode *GetPrev() const { return mPrev; }

                LinkListNode *mNext; // 0x00
                LinkListNode *mPrev; // 0x04
        };

        namespace detail {

            class LinkListImpl {
                public:
                    unsigned long GetSize() const { return mSize; }

                    unsigned long mSize; // 0x00
                    LinkListNode mNode;  // 0x04, the end of the list
            };
        }

        // T holds its LinkListNode at byte offset Ofs.
        template <typename T, int Ofs>
        class LinkList : public detail::LinkListImpl {
            public:
                class Iterator {
                    public:
                        Iterator(LinkListNode *pNode) : mPointer(pNode) {}

                        T &operator*() const
                        {
                            T *p = this->operator->();
                            if (p == 0) {
                                nw4r::db::Panic("LinkList.h", 403, "NW4R:Pointer must not be NULL (p)");
                            }
                            return *p;
                        }

                        T *operator->() const { return GetPointerFromNode(mPointer); }

                        Iterator &operator++()
                        {
                            mPointer = mPointer->GetNext();
                            return *this;
                        }

                        Iterator operator++(int)
                        {
                            Iterator old = *this;
                            ++*this;
                            return old;
                        }

                        bool operator==(const Iterator &rOther) const { return mPointer == rOther.mPointer; }
                        bool operator!=(const Iterator &rOther) const { return mPointer != rOther.mPointer; }

                    private:
                        LinkListNode *mPointer;
                };

                Iterator GetBeginIter() { return Iterator(mNode.GetNext()); }
                Iterator GetEndIter() { return Iterator(&mNode); }

                static T *GetPointerFromNode(LinkListNode *p)
                {
                    if (p == 0) {
                        nw4r::db::Panic("LinkList.h", 573, "NW4R:Pointer must not be NULL (p)");
                    }
                    return reinterpret_cast<T *>(reinterpret_cast<char *>(p) - Ofs);
                }
        };
    }
}

#endif /* !NW4R_UT_LINKLIST_H_ */
