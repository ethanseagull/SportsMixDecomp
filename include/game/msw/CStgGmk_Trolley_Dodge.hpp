#ifndef MSW_CSTGGMK_TROLLEY_DODGE_HPP_
#define MSW_CSTGGMK_TROLLEY_DODGE_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Trolley.hpp"
#include "game/msw/CStgGmk_Trolley_Act.hpp"
#include "game/msw/DodgePrm.hpp"

namespace msw {

    // The trolley of stage 17 in the dodgeball mode.

    // The actor, size 0x55C (nothing added to msw::CStgGmk_Trolley_Act),
    // with the DODGE_PRM_S17_* parameters. The getters are inline: the
    // original emits them after the destructor, in reverse order of these
    // declarations, with their literals after CStgGmk_Trolley_Act's. The
    // rest is in game/dodge/CStgGmk_Trolley_Act_Dodge.cpp.
    class CStgGmk_Trolley_Act_Dodge : public CStgGmk_Trolley_Act {
        public:
            CStgGmk_Trolley_Act_Dodge(CStgGmk *pParent) : CStgGmk_Trolley_Act(pParent) {}
            virtual ~CStgGmk_Trolley_Act_Dodge();

            virtual float GetParam_MdlWidth() const { return DODGE_PRM_S17_TROLLEY_MDL_WIDTH; }
            virtual float GetParam_MdlDepth() const { return DODGE_PRM_S17_TROLLEY_MDL_DEPTH; }
            virtual float GetParam_MdlHeight() const { return DODGE_PRM_S17_TROLLEY_MDL_HEIGHT; }
            virtual float GetParam_Width() const { return DODGE_PRM_S17_TROLLEY_WIDTH; }
            virtual float GetParam_Depth() const { return DODGE_PRM_S17_TROLLEY_DEPTH; }
            virtual float GetParam_Height(long type) const { return DODGE_PRM_S17_TROLLEY_HEIGHT; }
            virtual float GetParam_AtkWidth() const { return DODGE_PRM_S17_TROLLEY_ATK_WIDTH; }
            virtual float GetParam_AtkDepth() const { return DODGE_PRM_S17_TROLLEY_ATK_DEPTH; }
            virtual float GetParam_AtkHeight() const { return DODGE_PRM_S17_TROLLEY_ATK_HEIGHT; }
            virtual float GetParam_VelLimit() const { return DODGE_PRM_S17_TROLLEY_SPEED; }
            virtual long GetParam_MdlMinNum() const { return DODGE_PRM_S17_TROLLEY_NUM_MIN; }
            virtual long GetParam_MdlMaxNum() const { return DODGE_PRM_S17_TROLLEY_NUM_MAX; }
            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_TrolleyPatType() const { return DODGE_PRM_S17_TROLLEY_PAT_TYPE; }
            virtual long GetParam_TrolleyPatNo() const { return DODGE_PRM_S17_TROLLEY_PAT_NO; }
            virtual long GetParam_TrolleyPatValidNum() const;
            // The rate is a percentage.
            virtual float GetParam_CoinModeRate() const { return 0.01f * DODGE_PRM_S17_HEYHO_COIN_RATE; }
            virtual float GetParam_ThrowDistMin(long type) const {
                if (type != 0) {
                    return DODGE_PRM_S17_HEYHO_C_DIST_MIN;
                }
                return DODGE_PRM_S17_HEYHO_B_DIST_MIN;
            }
            virtual float GetParam_ThrowDistMax(long type) const {
                if (type != 0) {
                    return DODGE_PRM_S17_HEYHO_C_DIST_MAX;
                }
                return DODGE_PRM_S17_HEYHO_B_DIST_MAX;
            }
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual bool IsEnableCatchColl() const { return true; }
            virtual bool IsEnableDefColl() const { return false; }
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
