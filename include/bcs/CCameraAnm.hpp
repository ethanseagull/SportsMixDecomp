#ifndef BCS_CCAMERAANM_HPP_
#define BCS_CCAMERAANM_HPP_

#include "bcs/CCamera.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {

    class CCameraAnmCalc;

    // Size 0x1A4. A camera that plays a camera animation (the scene
    // camera of a g3d resource). The class lives in the DOL; only what the
    // modules need is mapped so far (constructor at
    // __ct__Q23bcs10CCameraAnmFlsPQ23bcs5CTask, vtable slots 6 to 8).
    class CCameraAnm : public CCamera {
        public:
            // One camera of the scene animation, 0xC bytes (names guessed;
            // read by msw::CTournamentMan in TourF).
            struct SCENE_INFO {
                const char *pName; // 0x00: the node animation of the camera
                long nGroup;       // 0x04
                float fFrame;      // 0x08: its length; 0 or less for an unused camera
            };

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

            // Added for TourF game/tour/CTournamentMan.cpp (return types
            // guessed). Makes the scene info from the nodes named pName.
            void MakeSceneInfo(const char *pName, long);
            // Selects scene camera nCamera.
            void SetData(long nCamera, long);
            // The camera of scene camera nCamera at frame fFrame.
            void GetDataFrame(FVEC3 &rPos, FVEC3 &rAt, float &rRoll, float &rFovy, long nCamera, long,
                              float fFrame, unsigned long ulFlag);

            // (names guessed) The scene cameras; entry nCamera, or the
            // default entry when there is no such camera.
            short GetSceneNum() const { return m_nSceneNum; }
            const SCENE_INFO &GetSceneInfo(long nCamera) const
            {
                if (m_pSceneInfo != 0 && nCamera >= 0 && nCamera < m_nSceneNum) {
                    return m_pSceneInfo[nCamera];
                }
                return m_SceneDefault;
            }
            // (names guessed) Bit 0 of the word at 0x138; offsets added to
            // the animated camera (written directly by TourF).
            void SetUnk138Bit0() { m_nUnk138 |= 1; }

            // (names guessed) The calculation run every frame, and the one
            // the constructor made (msw::CSceneCalc::SetCalc).
            void SetCalc(CCameraAnmCalc *pCalc) { m_pCalc = pCalc; }
            CCameraAnmCalc *GetDefaultCalc() const { return m_pCalcDefault; }

        private:
            // 0xFC: a frame controller (SetFrame passes its address on to a
            // DOL function); only the words the modules read are named.
            unsigned long m_nFrmFlag;     // 0xFC (name guessed)
            unsigned char m_Unk100[4];    // 0x100
            float m_fRate;                // 0x104, set by Play (name guessed)
            float m_fEndFrame;            // 0x108 (name guessed)
            unsigned char m_Unk10C[0xC];  // 0x10C
            CCameraAnmCalc *m_pCalcDefault; // 0x118: made by the constructor (name guessed)
            CCameraAnmCalc *m_pCalc;      // 0x11C: called every frame (name guessed)
            unsigned char m_Unk120[4];    // 0x120
            SCENE_INFO *m_pSceneInfo;     // 0x124 (name guessed)
            SCENE_INFO m_SceneDefault;    // 0x128 (name guessed)
            long m_nSceneNum;             // 0x134 (name guessed)
            unsigned long m_nUnk138;      // 0x138
            unsigned long m_nAnmFlag;     // 0x13C, Play sets bits 0x100 to 0x2000 (name guessed)

        public:
            FVEC3 m_vUnk140;               // 0x140
        private:
            unsigned char m_Unk14C[0x18];  // 0x14C
        public:
            FVEC3 m_vPosOffset;            // 0x164
            FVEC3 m_vAtOffset;             // 0x170
        private:
            unsigned char m_Unk17C[0x28];  // 0x17C (not mapped yet)
    };
}

#endif /* !BCS_CCAMERAANM_HPP_ */
