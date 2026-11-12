#ifndef MSW_CCHROPTMDLOBJ_HPP_
#define MSW_CCHROPTMDLOBJ_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {
    class CMdlObj;
    class CMdlShadow;
    class CMdlRef;
    class CAnmMotion;
}

namespace msw {

    // (DOL class) The model of a character's extra object (Toad's
    // mushrooms), with its animation. 0x1C bytes (the base of
    // CC_CMN::KINOPIO::CcBoundMushroom at 0x94, the next base at 0xB0).
    //
    // The virtual functions are in the order of the DOL vtable
    // (lbl_8046C318) and of the secondary vtable in
    // __vt__Q33msw11CC09_VOLLEY22CcKinopioBoundMushroom (VolleyF); return
    // types guessed. Members mapped from game/msw/CChrOptMdlObj.cpp (DOL;
    // names guessed).
    class CChrOptMdlObj {
        public:
            CChrOptMdlObj();
            virtual ~CChrOptMdlObj();
            virtual void RecalcMtx();
            virtual void AnimPlay(long nMotId, int, float, float);
            virtual void AnimStop();
            virtual void AnimSetSpeed(float fSpeed);
            virtual float AnimGetSpeed() const;
            virtual float AnimGetFrame() const;
            virtual float AnimGetFrameNum() const;
            virtual float AnimGetResFrameNum(long nMotId) const;
            virtual long AnimGetMotID() const;
            virtual void AnimSetFrame(float fFrame);
            virtual void AnimResetLoopedFlag();
            virtual int IsAnimFlip() const;
            virtual int IsAnimLoop() const;
            virtual int IsAnimLooped() const;
            virtual int IsAnimEnd() const;
            virtual int IsAnimInter() const;
            virtual int IsAnimResLoopType(long nMotId) const;
            virtual void CreateObj(nw4r::g3d::ResMdl mdl);
            virtual void CreateShadow(nw4r::g3d::ResMdl mdl, long);
            virtual void CreateRef(nw4r::g3d::ResMdl mdl);
            virtual void CreateAnm(nw4r::g3d::ResFile file, const bcs::CAnmObjMotBase::NODE_EX_INFO *pInfo);

            // (DOL) The index of node pName of the model; nDefault when there
            // is no model or no such node.
            long GetNodeIdx(const char *pName, long nDefault) const;

        protected:
            // (DOL; names guessed) Start fading the model in (out) over
            // fFrames frames, unless it is already doing so; one step of the
            // fade, by fStep frames.
            void FadeIn_(float fFrames);
            void FadeOut_(float fFrames);
            void UpdateFade_(float fStep);

            bcs::CMdlObj *m_pMdl;         // 0x04
            bcs::CMdlShadow *m_pShadow;   // 0x08
            bcs::CMdlRef *m_pRef;         // 0x0C
            bcs::CAnmMotion *m_pMotion;   // 0x10
            // The change of m_fAlpha per frame: positive while fading in,
            // negative while fading out, 0 when not fading.
            float m_fFadeSpeed;           // 0x14
            float m_fAlpha;               // 0x18
    };
}

#endif /* !MSW_CCHROPTMDLOBJ_HPP_ */
