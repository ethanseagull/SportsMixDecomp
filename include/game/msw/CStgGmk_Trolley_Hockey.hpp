#ifndef MSW_CSTGGMK_TROLLEY_HOCKEY_HPP_
#define MSW_CSTGGMK_TROLLEY_HOCKEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Trolley.hpp"
#include "game/msw/CStgGmk_Trolley_Act.hpp"

namespace msw {

    class CCollDefUnit;

    // The trolley of stage 17 in the hockey mode.

    // The actor, size 0x560 (one float added to msw::CStgGmk_Trolley_Act),
    // with the HOCKEY_PRM_S17_* parameters. Its functions are in
    // game/hockey/CStgGmk_Trolley_Act_Hockey.cpp (not written yet); only the
    // constructor is inline (msw::CStgGmk_Trolley_Hockey::NewAct). The
    // overrides are those of the HockeyF symbols, in no particular order.
    class CStgGmk_Trolley_Act_Hockey : public CStgGmk_Trolley_Act {
        public:
            CStgGmk_Trolley_Act_Hockey(CStgGmk *pParent) : CStgGmk_Trolley_Act(pParent) { m_fUnk55C = 0.0f; }
            virtual ~CStgGmk_Trolley_Act_Hockey();

            virtual void OnInit();
            virtual void OnUpdate(float rate);
            virtual void UpdateColl();
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);

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
            virtual bool IsEnableDefColl() const;
            virtual long GetLocRawTblIdx(long, long) const;

        private:
            float m_fUnk55C; // 0x55C (meaning not known)
    };

    // The trolley, size 0x80 (nothing added to msw::CStgGmk_Trolley). Made
    // by msw::CStageHockey17::NewTrolley. All inline: game/hockey/CStageHockey17.cpp
    // emits the vtable and, after CStgGmk_Trolley's weak functions, NewAct
    // and the destructor (reverse order of the declarations here).
    class CStgGmk_Trolley_Hockey : public CStgGmk_Trolley {
        public:
            CStgGmk_Trolley_Hockey(bcs::CTask *pParent) : CStgGmk_Trolley(pParent, 0) {}
            virtual ~CStgGmk_Trolley_Hockey() {}

            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Trolley_Act_Hockey(this); }
    };
}

#endif /* !MSW_CSTGGMK_TROLLEY_HOCKEY_HPP_ */
