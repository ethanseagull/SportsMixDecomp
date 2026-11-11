#ifndef MSW_CSTGGMK_TROLLEY_HOCKEY_HPP_
#define MSW_CSTGGMK_TROLLEY_HOCKEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Trolley.hpp"
#include "game/msw/CStgGmk_Trolley_Act.hpp"
#include "game/msw/HockeyPrm.hpp"

namespace msw {

    class CCollDefUnit;

    // The trolley of stage 17 in the hockey mode.

    // The actor, size 0x560 (one float added to msw::CStgGmk_Trolley_Act),
    // with the HOCKEY_PRM_S17_* parameters. Unlike the other modules' it
    // reflects the puck: game/hockey/CStgGmk_Trolley_Act_Hockey.cpp. The
    // getters are inline: the original emits them after the destructor, in
    // reverse order of these declarations.
    class CStgGmk_Trolley_Act_Hockey : public CStgGmk_Trolley_Act {
        public:
            CStgGmk_Trolley_Act_Hockey(CStgGmk *pParent) : CStgGmk_Trolley_Act(pParent) { m_fReflectWait = 0.0f; }
            virtual ~CStgGmk_Trolley_Act_Hockey();

            virtual void OnInit();
            virtual void OnUpdate(float rate);
            virtual void UpdateColl();
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);

            virtual float GetParam_MdlWidth() const { return HOCKEY_PRM_S17_TROLLEY_MDL_WIDTH; }
            virtual float GetParam_MdlDepth() const { return HOCKEY_PRM_S17_TROLLEY_MDL_DEPTH; }
            virtual float GetParam_MdlHeight() const { return HOCKEY_PRM_S17_TROLLEY_MDL_HEIGHT; }
            virtual float GetParam_Width() const { return HOCKEY_PRM_S17_TROLLEY_WIDTH; }
            virtual float GetParam_Depth() const { return HOCKEY_PRM_S17_TROLLEY_DEPTH; }
            virtual float GetParam_Height(long type) const { return HOCKEY_PRM_S17_TROLLEY_HEIGHT; }
            virtual float GetParam_AtkWidth() const { return HOCKEY_PRM_S17_TROLLEY_ATK_WIDTH; }
            virtual float GetParam_AtkDepth() const { return HOCKEY_PRM_S17_TROLLEY_ATK_DEPTH; }
            virtual float GetParam_AtkHeight() const { return HOCKEY_PRM_S17_TROLLEY_ATK_HEIGHT; }
            virtual float GetParam_VelLimit() const { return HOCKEY_PRM_S17_TROLLEY_SPEED; }
            virtual long GetParam_MdlMinNum() const { return HOCKEY_PRM_S17_TROLLEY_NUM_MIN; }
            virtual long GetParam_MdlMaxNum() const { return HOCKEY_PRM_S17_TROLLEY_NUM_MAX; }
            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_TrolleyPatType() const { return HOCKEY_PRM_S17_TROLLEY_PAT_TYPE; }
            virtual long GetParam_TrolleyPatNo() const { return HOCKEY_PRM_S17_TROLLEY_PAT_NO; }
            virtual long GetParam_TrolleyPatValidNum() const;
            // The rate is a percentage.
            virtual float GetParam_CoinModeRate() const { return 0.01f * HOCKEY_PRM_S17_HEYHO_COIN_RATE; }
            virtual float GetParam_ThrowDistMin(long type) const {
                if (type != 0) {
                    return HOCKEY_PRM_S17_HEYHO_C_DIST_MIN;
                }
                return HOCKEY_PRM_S17_HEYHO_B_DIST_MIN;
            }
            virtual float GetParam_ThrowDistMax(long type) const {
                if (type != 0) {
                    return HOCKEY_PRM_S17_HEYHO_C_DIST_MAX;
                }
                return HOCKEY_PRM_S17_HEYHO_B_DIST_MAX;
            }
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual bool IsEnableDefColl() const { return true; }
            virtual long GetLocRawTblIdx(long, long) const;

        private:
            float m_fReflectWait; // 0x55C, frames before the trolley reflects the puck again (name guessed)
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
