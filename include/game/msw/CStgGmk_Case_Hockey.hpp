#ifndef MSW_CSTGGMK_CASE_HOCKEY_HPP_
#define MSW_CSTGGMK_CASE_HOCKEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Case.hpp"
#include "game/msw/CStgGmk_Case_Act.hpp"

namespace msw {

    // The cases of the ship stage (stage 15) in the hockey mode.

    // The actor, size 0x218 (one float added to msw::CStgGmk_Case_Act), with
    // the HOCKEY_PRM_S15_* parameters. Its functions are in
    // game/hockey/CStgGmk_Case_Act_Hockey.cpp (not written yet; unlike the
    // dodgeball actor it keeps CStgGmk_Case_Act::GetParam_OuterRect); only the
    // constructor is inline (msw::CStgGmk_Case_Hockey::NewAct).
    class CStgGmk_Case_Act_Hockey : public CStgGmk_Case_Act {
        public:
            CStgGmk_Case_Act_Hockey(CStgGmk *pParent) : CStgGmk_Case_Act(pParent) { m_fUnk214 = 0.0f; }
            virtual ~CStgGmk_Case_Act_Hockey();

            virtual float GetParam_Width() const;
            virtual float GetParam_Depth() const;
            virtual float GetParam_Height() const;
            virtual float GetParam_MdlWidth() const;
            virtual float GetParam_MdlDepth() const;
            virtual float GetParam_MdlHeight() const;
            virtual float GetParam_Acc() const;
            virtual float GetParam_Dec() const;
            virtual float GetParam_VelLimit() const;
            virtual float GetParam_Interval() const;
            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_SE(long type) const;
            virtual float GetParam_MaxLife() const;
            virtual float GetParam_LocPosY() const;
            virtual float GetParam_KaronScale() const;
            virtual float GetParam_KaronCollScale() const;
            virtual float GetParam_KaronSpeed() const;
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual void OnBreak();
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);

        private:
            float m_fUnk214; // 0x214, used by OnReflectHit (meaning not known)
    };

    // One case, size 0x78 (nothing added to msw::CStgGmk_Case). Made by
    // msw::CStageHockey15::NewCase. All inline: game/hockey/CStageHockey15.cpp
    // emits the vtable and, after CStgGmk_Case's weak functions, NewAct and
    // the destructor (reverse order of the declarations here).
    class CStgGmk_Case_Hockey : public CStgGmk_Case {
        public:
            CStgGmk_Case_Hockey(bcs::CTask *pParent) : CStgGmk_Case(pParent, 0) {}
            virtual ~CStgGmk_Case_Hockey() {}

            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Case_Act_Hockey(this); }
    };
}

#endif /* !MSW_CSTGGMK_CASE_HOCKEY_HPP_ */
