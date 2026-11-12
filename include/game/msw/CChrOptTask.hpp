#ifndef MSW_CCHROPTTASK_HPP_
#define MSW_CCHROPTTASK_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/CAnmObjMotBase.hpp"
#include "game/msw/CChrOptMdlObj.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CScnObj;
}

namespace msw {

    // (class name guessed; the DOL names only what earlier sessions named in
    // symbols.txt) A task that owns one extra model of a character (the
    // slime's squad), size 0xC0 (what CC_CMN::SLIME::CcSquadFactory
    // allocates). Its vtable is lbl_8046C258: a task part (destructor, Main,
    // CTask::Disp, CUnivTask::GetFrameRateF), the msw::CChrOptMdlObj base at
    // 0x34 (secondary vtable at lbl_8046C258+0x18) and the virtuals the class
    // adds, from 0x78. Defined in game/msw/CChrOptMdlObj.cpp (DOL). Names of
    // the members and of the functions not named in the DOL are guessed.
    //
    // The model follows another scene object (m_hParent) when flag 2 is
    // set, fades in and out with sounds and an effect at the start of each
    // fade, and keeps up to nine effects in slots, like msw::CChrCore.
    // The task part is a bcs::CUnivTaskEx: the destructor shows the
    // two inlined base destructors (two null tests before ~CTask).
    class CChrOptTask : public bcs::CUnivTaskEx, public CChrOptMdlObj {
        public:
            // bFollow sets flag 2: the model copies the motion of the object
            // it is attached to.
            CChrOptTask(bcs::CTask *pParent, int bFollow);
            virtual ~CChrOptTask();

            virtual void Main();

            // The class's own virtual functions and its overrides of
            // CChrOptMdlObj's, in the order of lbl_8046C258 after the
            // embedded CChrOptMdlObj vtable (slots 0x78 to 0xB8; HockeyF's
            // game/hockey/CChrCoreHockeySlime.cpp calls 0x84 and 0x90, as
            // BasketF's CcSquadFactoryBasket::AnimPlay does).
            virtual void OnMain_();                                  // 0x78, empty
            virtual void UpdateMotion_(float fStep);                 // 0x7C
            virtual void CreateObj(nw4r::g3d::ResMdl mdl);           // 0x80
            virtual void AnimPlay(long nMotId, int bLoop, float fInterFrame, float fFrame); // 0x84
            // 0x88 to 0xB0: the effect slots, as in msw::CChrCore (names
            // taken from there). An effect at node nNode (or the node named
            // pNode) of the model; bit 0 and bit 1 of nFlag go to
            // CEffectCtrl::CreateAttachEffect as two flags.
            virtual unsigned long Eff_CreateAttach(long nSlot, long nEff, unsigned long nNode, const bcs::FVEC3 *pOfs,
                                                   unsigned long nFlag, unsigned long);           // 0x88
            virtual unsigned long Eff_CreateAttach(long nSlot, long nEff, const char *pNode, const bcs::FVEC3 *pOfs,
                                                   unsigned long nFlag, unsigned long);           // 0x8C
            // 0x90: attaches the effect through CEffectCtrl::CreateAttachEffectAuto.
            virtual unsigned long Eff_CreateAttachAuto(long nSlot, long nEff, unsigned long);
            virtual unsigned long Eff_GetHandle(long nSlot);         // 0x94
            virtual long Eff_GetId(long nSlot);                      // 0x98
            virtual void Eff_SetSpeed(long nSlot, float fSpeed);     // 0x9C
            virtual int Eff_IsAlive(long nSlot);                     // 0xA0
            virtual void Eff_Stop(long nSlot);                       // 0xA4
            virtual void Eff_Erase(long nSlot);                      // 0xA8
            virtual void Eff_StopAll();                              // 0xAC
            virtual void Eff_EraseAll();                             // 0xB0
            // 0xB4, 0xB8: the looping sound m_nLoopSe at the model, and
            // stopping it over fFrames frames.
            virtual void SE_PlayLoop();
            virtual void SE_StopLoop(float fFrames);

            // Makes model pMdlName of res with the motions of mot, attaches
            // it to node nNode of pParent (when nNode >= 0), with the shadow
            // pShadowName (nShadowArg) and the reflection pRefName; nAppearEff
            // and nDisappearEff are the effects of the two fades.
            // (The reflection is made from the main model: the original
            // passes that one, not the one it looks up by pRefName.)
            void Create(const bcs::CScnObj *pParent, long nNode, nw4r::g3d::ResFile res, nw4r::g3d::ResFile mot,
                        const char *pMdlName, const char *pShadowName, const char *pRefName,
                        const bcs::CAnmObjMotBase::NODE_EX_INFO *pNodeExInfo, long nShadowArg, long nAppearEff,
                        long nDisappearEff);
            // (DOL fn_800D3CD4, called by msw::CChrCore) The same with the
            // models given by index (a negative index means none); the
            // motion file comes after the indices here.
            void Create(const bcs::CScnObj *pParent, long nNode, nw4r::g3d::ResFile res, long nMdl, long nShadow,
                        long nRef, nw4r::g3d::ResFile mot, const bcs::CAnmObjMotBase::NODE_EX_INFO *pNodeExInfo,
                        long nShadowArg, long nAppearEff, long nDisappearEff);
            // Fades the model in over fFrames frames, after fDelay frames
            // (or, with a motion nMot, from frame fDelay of the motion), and
            // the same for fading out (DOL fn_800D4168).
            void AnimPlay(long nMot, float fFrames, float fDelay);
            void AnimPlayOut(long nMot, float fFrames, float fDelay);
            // (DOL, called by msw::CChrCore) The sounds of the two fades,
            // showing or hiding the models (a hidden one starts no fade
            // effect), and the looping sound.
            void SetAppearSe(long nSe);
            void SetDisappearSe(long nSe);
            void SetVisible(int bVisible);
            void SetLoopSe(long nSe);

        private:
            // (DOL fn_800D43B8) Main's step of the fade.
            void UpdateFade(float fStep);
            // Inline helpers of Create and of the fades.
            void AttachTo_(const bcs::CScnObj *pParent, long nNode);
            bool CanFadeIn_() const;
            bool CanFadeOut_() const;
            void StartAppearEffect_();
            void StartDisappearEffect_();
            void StartFadeSe_(long nSe);

            enum {
                FLAG_START_FRAME0 = 0x1, // the next AnimPlay starts at frame 0
                FLAG_FOLLOW = 0x2,
                FLAG_HIDE = 0x4          // the models are hidden (SetVisible)
            };
            enum { EFF_SLOT_NUM = 9, EFF_SLOT_FADE = 8 };

            // 0x34 to 0x50 is the CChrOptMdlObj.
            unsigned long m_nFlag;                          // 0x50
            unsigned long m_hParent;                        // 0x54, the scene object followed
            long m_aEffId[EFF_SLOT_NUM];                    // 0x58
            unsigned long m_aEffHandle[EFF_SLOT_NUM];       // 0x7C
            long m_nAppearEff;                              // 0xA0
            long m_nDisappearEff;                           // 0xA4
            float m_fDelay;                                 // 0xA8
            long m_nMot;                                    // 0xAC
            long m_nAppearSe;                               // 0xB0
            long m_nDisappearSe;                            // 0xB4
            long m_nLoopSe;                                 // 0xB8
            unsigned long m_hLoopSe;                        // 0xBC
    };
}

#endif /* !MSW_CCHROPTTASK_HPP_ */
