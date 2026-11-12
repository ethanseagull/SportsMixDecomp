#ifndef NW4R_LYT_LAYOUT_H_
#define NW4R_LYT_LAYOUT_H_

#include <new>

#include "nw4r/db/Warning.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/ut/LinkList.h"
#include "revolution/mem.h"

// The name of this file is part of the binary: Layout::AllocMemory passes
// __FILE__ ("layout.h") to its warning. Written for this project from the
// code MenuF game/menu/CBackground.cpp emits for it (the two strings are
// @STRING@AllocMemory__Q34nw4r3lyt6LayoutFUl objects); the names follow the
// public ogws decompilation's description of nw4r::lyt::Layout (no code
// copied).

namespace nw4r {
    namespace lyt {

        class AnimResource;
        class AnimTransform;
        class DrawInfo;
        class GroupContainer;
        class ResourceAccessor;

        // Lives in the DOL. The static allocation helpers, and (added for
        // MenuF game/menu/CBgDisplay.cpp) the object itself: 0x20 bytes,
        // virtuals in vtable order, root pane at 0x10, groups at 0x14.
        class Layout {
            public:
                Layout();
                virtual ~Layout();
                virtual bool Build(const void *pLytResBuf, ResourceAccessor *pResAccessor);
                virtual AnimTransform *CreateAnimTransform();
                virtual AnimTransform *CreateAnimTransform(const void *pAnimResBuf, ResourceAccessor *pResAccessor);
                virtual AnimTransform *CreateAnimTransform(const AnimResource &rAnimRes, ResourceAccessor *pResAccessor);
                virtual void BindAnimation(AnimTransform *pAnimTrans);
                virtual void UnbindAnimation(AnimTransform *pAnimTrans);
                virtual void UnbindAllAnimation();
                virtual bool BindAnimationAuto(const AnimResource &rAnimRes, ResourceAccessor *pResAccessor);
                virtual void SetAnimationEnable(AnimTransform *pAnimTrans, bool bEnable);
                virtual void CalculateMtx(const DrawInfo &rDrawInfo);
                virtual void Draw(const DrawInfo &rDrawInfo);
                virtual void Animate(unsigned long option);

                Pane *GetRootPane() const { return mpRootPane; }
                GroupContainer *GetGroupContainer() const { return mpGroupContainer; }

                static MEMAllocator *mspAllocator;

                static void *AllocMemory(unsigned long size)
                {
                    void *pMem = MEMAllocFromAllocator(mspAllocator, size);
// The original has this check on line 73 (the value passed in r4).
#line 73
                    NW4R_WARNING(pMem, "can't alloc memory.");
                    return pMem;
                }

                // A new T built in memory from mspAllocator; null when the
                // allocation fails.
                template <typename T>
                static T *NewObj()
                {
                    void *pMem = AllocMemory(sizeof(T));
                    if (pMem == 0) {
                        return 0;
                    }
                    return new (pMem) T();
                }

            private:
                ut::detail::LinkListImpl mAnimTransList; // 0x04
                Pane *mpRootPane;                        // 0x10
                GroupContainer *mpGroupContainer;        // 0x14
                Size mLayoutSize;                        // 0x18
        };
    }
}

#endif /* !NW4R_LYT_LAYOUT_H_ */
