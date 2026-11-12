#ifndef NW4R_LYT_LAYOUT_H_
#define NW4R_LYT_LAYOUT_H_

#include <new>

#include "nw4r/db/Warning.hpp"
#include "nw4r/lyt/animation.h"
#include "nw4r/lyt/Pane.hpp"
#include "revolution/mem.h"

// The name of this file is part of the binary: Layout::AllocMemory passes
// __FILE__ ("layout.h") to its warning. Written for this project from the
// code MenuF game/menu/CBackground.cpp emits for it (the two strings are
// @STRING@AllocMemory__Q34nw4r3lyt6LayoutFUl objects); the names follow the
// public ogws decompilation's description of nw4r::lyt::Layout (no code
// copied).

namespace nw4r {
    namespace lyt {

        class ResourceAccessor;
        class DrawInfo;
        class GroupContainer;

        // Lives in the DOL. Size 0x20 (bcs::G2D::CLayoutDL's own members
        // start there). Virtuals in vtable order from 0x08 (up to 0x2C added
        // for DOL game/bcs/G2D/CLayoutBase.cpp, the constructor and 0x30 on
        // for game/msw/CNowLoading.cpp).
        class Layout {
            public:
                typedef ut::LinkList<AnimTransform, 4> AnimTransformList;

                Layout();
                virtual ~Layout();                                                  // 0x08
                virtual bool Build(const void *pLytResBuf, ResourceAccessor *pAccessor); // 0x0C
                virtual AnimTransform *CreateAnimTransform();                       // 0x10
                virtual AnimTransform *CreateAnimTransform(const void *pAnimResBuf, ResourceAccessor *pAccessor); // 0x14
                virtual AnimTransform *CreateAnimTransform(const AnimResource &rRes, ResourceAccessor *pAccessor); // 0x18
                virtual void BindAnimation(AnimTransform *pAnimTrans);              // 0x1C
                virtual void UnbindAnimation(AnimTransform *pAnimTrans);            // 0x20
                virtual void UnbindAllAnimation();                                  // 0x24
                virtual bool BindAnimationAuto(const AnimResource &rRes, ResourceAccessor *pAccessor); // 0x28
                virtual void SetAnimationEnable(AnimTransform *pAnimTrans, bool bEnable); // 0x2C
                virtual void CalculateMtx(const DrawInfo &rInfo);                   // 0x30
                virtual void Draw(const DrawInfo &rInfo);                           // 0x34
                virtual void Animate(unsigned long option);                         // 0x38

                AnimTransformList &GetAnimTransformList() { return mAnimTransList; }
                Pane *GetRootPane() const { return mpRootPane; }
                // (added for MenuF game/menu/CBgDisplay.cpp)
                GroupContainer *GetGroupContainer() const { return static_cast<GroupContainer *>(mpGroupContainer); }

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

            protected:
                AnimTransformList mAnimTransList; // 0x04
                Pane *mpRootPane;                 // 0x10
                void *mpGroupContainer;           // 0x14
                Size mLayoutSize;                 // 0x18
        };

        // (the DOL's fn_8041E6C0, added for game/msw/CNowLoading.cpp)
        // Registers the library version and sets up the quantization
        // registers it uses.
        void LytInit();
    }
}

#endif /* !NW4R_LYT_LAYOUT_H_ */
