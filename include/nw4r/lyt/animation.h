#ifndef NW4R_LYT_ANIMATION_H_
#define NW4R_LYT_ANIMATION_H_

// Added for game/msw/CNowLoading.cpp: only the frame (at 0x10, read and
// written directly) and the non-virtual frame count. Names follow the
// public ogws decompilation's description of nw4r::lyt::AnimTransform (no
// code copied).

namespace nw4r {
    namespace lyt {

        class AnimTransform {
            public:
                virtual ~AnimTransform();
                unsigned short GetFrameSize() const;
                float GetFrame() const { return mFrame; }
                void SetFrame(float frame) { mFrame = frame; }

            private:
                unsigned char mUnk04[0xC];  // 0x04
                float mFrame;               // 0x10
        };
    }
}

#endif /* !NW4R_LYT_ANIMATION_H_ */
