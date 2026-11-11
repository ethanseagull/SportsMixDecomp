#ifndef MSW_CSTGGMK_TROLLEY_DODGE_HPP_
#define MSW_CSTGGMK_TROLLEY_DODGE_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Trolley.hpp"
#include "game/msw/CStgGmk_Trolley_Act.hpp"

namespace msw {

    // The trolley of stage 17 in the dodgeball mode.

    // The actor, size 0x55C (nothing added to msw::CStgGmk_Trolley_Act),
    // with the DODGE_PRM_S17_* parameters. Its functions are in
    // game/dodge/CStgGmk_Trolley_Act_Dodge.cpp (not written yet); only the
    // constructor is inline (msw::CStgGmk_Trolley_Dodge::NewAct).
    class CStgGmk_Trolley_Act_Dodge : public CStgGmk_Trolley_Act {
        public:
            CStgGmk_Trolley_Act_Dodge(CStgGmk *pParent) : CStgGmk_Trolley_Act(pParent) {}
            virtual ~CStgGmk_Trolley_Act_Dodge();

            virtual float GetParam_MdlWidth() const;
            virtual float GetParam_MdlDepth() const;
            virtual float GetParam_MdlHeight() const;
            virtual float GetParam_Width() const;
            virtual float GetParam_Depth() const;
            virtual float GetParam_Height(long type) const;
            virtual float GetParam_AtkWidth() const;
            virtual float GetParam_AtkDepth() const;
            virtual float GetParam_AtkHeight() const;
            virtual float GetParam_VelLimit() const;
            virtual long GetParam_MdlMinNum() const;
            virtual long GetParam_MdlMaxNum() const;
            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_TrolleyPatType() const;
            virtual long GetParam_TrolleyPatNo() const;
            virtual long GetParam_TrolleyPatValidNum() const;
            virtual float GetParam_CoinModeRate() const;
            virtual float GetParam_ThrowDistMin(long type) const;
            virtual float GetParam_ThrowDistMax(long type) const;
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual bool IsEnableCatchColl() const;
            virtual bool IsEnableDefColl() const;
            virtual long GetLocRawTblIdx(long, long) const;
    };

    // The trolley, size 0x80 (nothing added to msw::CStgGmk_Trolley). Made
    // by msw::CStageDodge17::NewTrolley. All inline: game/dodge/CStageDodge17.cpp
    // emits the vtable and, after CStgGmk_Trolley's weak functions, NewAct
    // and the destructor (reverse order of the declarations here).
    class CStgGmk_Trolley_Dodge : public CStgGmk_Trolley {
        public:
            CStgGmk_Trolley_Dodge(bcs::CTask *pParent) : CStgGmk_Trolley(pParent, 0) {}
            virtual ~CStgGmk_Trolley_Dodge() {}

            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Trolley_Act_Dodge(this); }
    };
}

#endif /* !MSW_CSTGGMK_TROLLEY_DODGE_HPP_ */
