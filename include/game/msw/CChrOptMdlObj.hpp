#ifndef MSW_CCHROPTMDLOBJ_HPP_
#define MSW_CCHROPTMDLOBJ_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace msw {

    // (DOL class) The model of a character's extra object (Toad's
    // mushrooms), with its animation. 0x1C bytes (the base of
    // CC_CMN::KINOPIO::CcBoundMushroom at 0x94, the next base at 0xB0).
    //
    // INCOMPLETE: the virtual functions, in the order of the secondary
    // vtable in __vt__Q33msw11CC09_VOLLEY22CcKinopioBoundMushroom (VolleyF);
    // return types guessed. No member is mapped.
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

        private:
            unsigned char m_Unk04[0x1C - 0x4];
    };
}

#endif /* !MSW_CCHROPTMDLOBJ_HPP_ */
