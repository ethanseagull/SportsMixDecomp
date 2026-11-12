#ifndef NW4R_LYT_ANIMATION_HPP_
#define NW4R_LYT_ANIMATION_HPP_

#include "nw4r/ut/LinkList.h"

// Layout animations of the DOL's NW4R library. Only what
// bcs::G2D::CLayoutBase uses; offsets are what its code reads. Names follow
// the public ogws decompilation's description of nw4r::lyt (from memory, no
// code copied).

namespace nw4r {
    namespace lyt {

        namespace res {
            // The animation block of a .brlan file.
            struct AnimationBlock {
                unsigned char m_Unk00[0x8];   // block header
                unsigned short frameSize;     // 0x08
                unsigned char loop;           // 0x0A, nonzero for a looping animation
            };
        }

        // One .brlan file. Size 0x10, no destructor (arrays of it are built
        // with __construct_new_array and no destructor).
        class AnimResource {
            public:
                AnimResource();
                void Set(const void *pAnimResBuf);

                const res::AnimationBlock *GetResourceBlock() const { return mpResBlock; }
                unsigned short GetFrameSize() const { return mpResBlock->frameSize; }
                bool IsLoopData() const { return mpResBlock->loop != 0; }

            private:
                const void *mpFileHeader;              // 0x00
                const res::AnimationBlock *mpResBlock; // 0x04
                const void *mpTagBlock;                // 0x08
                const void *mpShareBlock;              // 0x0C
        };

        // An animation bound to a layout; Layout keeps them in a list
        // through the node at 0x04.
        class AnimTransform {
            public:
                virtual ~AnimTransform();

                unsigned short GetFrameSize() const;
                float GetFrame() const { return mFrame; }
                void SetFrame(float fFrame) { mFrame = fFrame; }
                const res::AnimationBlock *GetAnimResource() const { return mpRes; }

                ut::LinkListNode mLink;           // 0x04

            protected:
                const res::AnimationBlock *mpRes; // 0x0C
                float mFrame;                     // 0x10
        };
    }
}

#endif /* !NW4R_LYT_ANIMATION_HPP_ */
