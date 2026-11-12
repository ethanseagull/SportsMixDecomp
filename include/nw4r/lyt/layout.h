#ifndef NW4R_LYT_LAYOUT_H_
#define NW4R_LYT_LAYOUT_H_

#include <new>

#include "nw4r/db/Warning.hpp"
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
        class AnimTransform;
        class DrawInfo;

        // Lives in the DOL. The static allocation helpers and (added for
        // game/msw/CNowLoading.cpp) the virtuals it calls, in vtable order
        // from 0x08; 0x10, 0x18, 0x20, 0x24 and 0x28 are not called yet and
        // stand in with the names of the ogws description. The object is
        // 0x20 bytes (root pane at 0x10), not described.
        class Layout {
            public:
                Layout();
                virtual ~Layout();
                virtual bool Build(const void *pLytRes, ResourceAccessor *pAccessor);
                virtual AnimTransform *CreateAnimTransform();
                virtual AnimTransform *CreateAnimTransform(const void *pAnmRes, ResourceAccessor *pAccessor);
                virtual void Unk18();
                virtual void BindAnimation(AnimTransform *pAnim);
                virtual void UnbindAnimation(AnimTransform *pAnim);
                virtual void UnbindAllAnimation();
                virtual void Unk28();
                virtual void SetAnimationEnable(AnimTransform *pAnim, bool bEnable);
                virtual void CalculateMtx(const DrawInfo &rInfo);
                virtual void Draw(const DrawInfo &rInfo);
                virtual void Animate(unsigned long option);

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
                unsigned char mUnk04[0x1C]; // 0x04
        };

        // (the DOL's fn_8041E6C0, added for game/msw/CNowLoading.cpp)
        // Registers the library version and sets up the quantization
        // registers it uses.
        void LytInit();
    }
}

#endif /* !NW4R_LYT_LAYOUT_H_ */
