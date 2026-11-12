#ifndef NW4R_LYT_ANIMATION_HPP_
#define NW4R_LYT_ANIMATION_HPP_

#include "nw4r/ut/LinkList.h"

// Layout animations. Live in the DOL. Only what MenuF
// game/menu/CBgDisplay.cpp needs; names follow the public ogws
// decompilation's description of nw4r::lyt (from memory, no code copied).

namespace nw4r {
    namespace lyt {

        // A group an animation file names. Size 0x14: the name and a flag.
        struct AnimationGroupRef {
            const char *GetName() const { return name; }

            char name[17];       // 0x00
            unsigned char flag;  // 0x11
            unsigned char padding[2];
        };

        // An animation file (brlan) as the layout reads it. Size 0x10.
        class AnimResource {
            public:
                AnimResource();

                void Set(const void *pAnimResBuf);
                unsigned short GetGroupNum() const;
                const AnimationGroupRef *GetGroupArray() const;

            private:
                const void *mpResBuf;     // 0x00
                const void *mpTagBlock;   // 0x04
                const void *mpShareBlock; // 0x08
                const void *mpBlock;      // 0x0C
        };

        // One animation playing on a layout.
        class AnimTransform {
            public:
                virtual ~AnimTransform();

                unsigned short GetFrameSize() const;
                bool IsLoopData() const;

                float GetFrame() const { return mFrame; }
                void SetFrame(float frame) { mFrame = frame; }

            private:
                ut::LinkListNode mLink; // 0x04
                const void *mpRes;      // 0x0C
                float mFrame;           // 0x10
        };

        // An animation bound to a pane or a material.
        class AnimationLink {
            public:
                AnimTransform *GetAnimTransform() const { return mAnimTrans; }
                bool IsEnable() const { return !mbDisable; }

            private:
                ut::LinkListNode mLink;     // 0x00
                AnimTransform *mAnimTrans;  // 0x08
                unsigned short mIdx;        // 0x0C
                bool mbDisable;             // 0x0E
        };
    }
}

#endif /* !NW4R_LYT_ANIMATION_HPP_ */
