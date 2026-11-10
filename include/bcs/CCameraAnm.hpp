#ifndef BCS_CCAMERAANM_HPP_
#define BCS_CCAMERAANM_HPP_

#include "bcs/CCamera.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {

    // Size 0x1A4. A camera that plays a camera animation (the scene
    // camera of a g3d resource). The class lives in the DOL; only what the
    // modules need is mapped so far (constructor at
    // __ct__Q23bcs10CCameraAnmFlsPQ23bcs5CTask, vtable slots 6 to 8).
    class CCameraAnm : public CCamera {
        public:
            CCameraAnm(long view, short priority, CTask *pParent);
            virtual ~CCameraAnm();
            virtual long GetCamKind() const { return 4; } // value from the module copy
            virtual void MainCamera();
            virtual int OnActive();

            // Added for MenuF game/menu/CCameraMenuBG.cpp. Return types guessed
            // (the callers ignore them).
            void SetData(nw4r::g3d::ResFile file);
            // Selects the scene animation `pName` of the file given to SetData.
            void SetResAnmScn(const char *pName);
            // Selects which camera of that scene animation is played.
            void SetResAnmScnCamera(long nCamera);
            // flag bits 0 and 1 come from msw::CCameraMenuBG::AnimInfo.
            void Play(unsigned long flag, float rate, float frame);
            void SetFrame(float frame);

            // (names guessed) Inline readers of the frame controller and the
            // animation flags, as msw::CCameraMenuBG uses them.
            float GetRate() const { return m_fRate; }
            void SetRate(float rate) { m_fRate = rate; }
            float GetEndFrame() const { return m_fEndFrame; }
            int IsLooped() const { return m_nFrmFlag & 2; }
            int IsEnd() const { return m_nFrmFlag & 8; }
            int IsPlaying() const { return m_nAnmFlag & 0x100; }

        private:
            // 0xFC: a frame controller (SetFrame passes its address on to a
            // DOL function); only the words the modules read are named.
            unsigned long m_nFrmFlag;     // 0xFC (name guessed)
            unsigned char m_Unk100[4];    // 0x100
            float m_fRate;                // 0x104, set by Play (name guessed)
            float m_fEndFrame;            // 0x108 (name guessed)
            unsigned char m_Unk10C[0x30]; // 0x10C
            unsigned long m_nAnmFlag;     // 0x13C, Play sets bits 0x100 to 0x2000 (name guessed)
            unsigned char m_Unk140[0x64]; // 0x140 (not mapped yet)
    };
}

#endif /* !BCS_CCAMERAANM_HPP_ */
