#ifndef MSW_CSTGGMK_TROLLEY_VOLLEY_HPP_
#define MSW_CSTGGMK_TROLLEY_VOLLEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Trolley.hpp"
#include "game/msw/CStgGmk_Trolley_Act.hpp"
#include "game/msw/CStgGmk_Trolley_Mdl.hpp"
#include "game/msw/VolleyPrm.hpp"

namespace msw {

    class CCollDefUnit;

    // The trolley of stage 17 in the volley mode.

    // A trolley car, with its own size (game/volley/CStgGmk_Trolley_Act_Volley.cpp).
    class CStgGmk_Trolley_Mdl_Volley : public CStgGmk_Trolley_Mdl {
        public:
            CStgGmk_Trolley_Mdl_Volley(CStgGmk *pParent, long idx) : CStgGmk_Trolley_Mdl(pParent, idx) {}
            virtual ~CStgGmk_Trolley_Mdl_Volley() {}

            virtual float GetParam_MdlWidth() const;
            virtual float GetParam_MdlHeight() const;
            virtual float GetParam_MdlDepth() const;
    };

    // The actor, size 0x560 (one float added to msw::CStgGmk_Trolley_Act),
    // with the VOLLEY_PRM_S17_* parameters. Unlike the other modules' it
    // has a defence collision per car that reflects the ball:
    // game/volley/CStgGmk_Trolley_Act_Volley.cpp. The inline functions are
    // emitted there, those called by a function right after it, the others
    // with the vtable in reverse order of these declarations.
    class CStgGmk_Trolley_Act_Volley : public CStgGmk_Trolley_Act {
        public:
            CStgGmk_Trolley_Act_Volley(CStgGmk *pParent);
            virtual ~CStgGmk_Trolley_Act_Volley();

            virtual void OnInit();
            virtual void OnUpdate(float rate);
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);

            virtual float GetParam_MdlWidth() const { return VOLLEY_PRM_S17_TROLLEY_MDL_WIDTH; }
            virtual float GetParam_MdlDepth() const { return VOLLEY_PRM_S17_TROLLEY_MDL_DEPTH; }
            virtual float GetParam_MdlHeight() const { return VOLLEY_PRM_S17_TROLLEY_MDL_HEIGHT; }
            virtual float GetParam_Width() const { return VOLLEY_PRM_S17_TROLLEY_WIDTH; }
            virtual float GetParam_Depth() const { return VOLLEY_PRM_S17_TROLLEY_DEPTH; }
            virtual float GetParam_Height(long type) const { return VOLLEY_PRM_S17_TROLLEY_HEIGHT; }
            virtual float GetParam_AtkWidth() const { return VOLLEY_PRM_S17_TROLLEY_ATK_WIDTH; }
            virtual float GetParam_AtkDepth() const { return VOLLEY_PRM_S17_TROLLEY_ATK_DEPTH; }
            virtual float GetParam_AtkHeight() const { return VOLLEY_PRM_S17_TROLLEY_ATK_HEIGHT; }
            virtual float GetParam_DefWidth() const { return VOLLEY_PRM_S17_TROLLEY_DEF_WIDTH; }
            // The height of the defence collision of a car of type `type`.
            virtual float GetParam_DefHeight(long type) const
            {
                switch (type) {
                    case 0:
                        return VOLLEY_PRM_S17_TROLLEY_DEF_HEIGHT0;
                    case 1:
                        return VOLLEY_PRM_S17_TROLLEY_DEF_HEIGHT1;
                    case 2:
                        return VOLLEY_PRM_S17_TROLLEY_DEF_HEIGHT2;
                    case 3:
                        return VOLLEY_PRM_S17_TROLLEY_DEF_HEIGHT2;
                }
                return 0.0f;
            }
            virtual float GetParam_VelLimit() const { return VOLLEY_PRM_S17_TROLLEY_SPEED; }
            virtual long GetParam_MdlMinNum() const { return VOLLEY_PRM_S17_TROLLEY_NUM_MIN; }
            virtual long GetParam_MdlMaxNum() const { return VOLLEY_PRM_S17_TROLLEY_NUM_MAX; }
            virtual long GetParam_Effect(long type) const;
            virtual bool GetParam_IsRailMotRelate() const { return false; }
            virtual long GetParam_TrolleyPatType() const { return VOLLEY_PRM_S17_TROLLEY_PAT_TYPE; }
            virtual long GetParam_TrolleyPatNo() const { return VOLLEY_PRM_S17_TROLLEY_PAT_NO; }
            virtual long GetParam_TrolleyPatValidNum() const;
            // The rate is a percentage.
            virtual float GetParam_CoinModeRate() const { return 0.01f * VOLLEY_PRM_S17_HEYHO_COIN_RATE; }
            virtual float GetParam_ThrowDistMin(long type) const
            {
                if (type != 0) {
                    return VOLLEY_PRM_S17_HEYHO_C_DIST_MIN;
                }
                return VOLLEY_PRM_S17_HEYHO_B_DIST_MIN;
            }
            virtual float GetParam_ThrowDistMax(long type) const
            {
                if (type != 0) {
                    return VOLLEY_PRM_S17_HEYHO_C_DIST_MAX;
                }
                return VOLLEY_PRM_S17_HEYHO_B_DIST_MAX;
            }
            virtual void InitDefColl();
            virtual void UpdateColl();
            virtual void UpdateDefColl(const bcs::FVEC3 &rPos, float width, float, float, float rotY);
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual bool IsEnableCatchColl() const { return true; }
            virtual bool IsEnableDefColl() const { return true; }
            virtual long GetLocRawTblIdx(long, long) const;

        private:
            float m_fReflectWait; // 0x55C, frames before the trolley reflects the ball again (name guessed)
    };

    // The trolley, size 0x80 (nothing added to msw::CStgGmk_Trolley). Made
    // by msw::CStageVolley17::NewTrolley. All inline: game/volley/CStageVolley17.cpp
    // emits the vtable, NewAct, NewMdl and the destructor (reverse order
    // of the declarations here).
    class CStgGmk_Trolley_Volley : public CStgGmk_Trolley {
        public:
            CStgGmk_Trolley_Volley(bcs::CTask *pParent) : CStgGmk_Trolley(pParent, 0) {}
            virtual ~CStgGmk_Trolley_Volley() {}

            virtual CStgMdl *NewMdl(long heap, unsigned long idx)
            {
                return new (heap) CStgGmk_Trolley_Mdl_Volley(this, idx);
            }
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Trolley_Act_Volley(this); }
    };
}

#endif /* !MSW_CSTGGMK_TROLLEY_VOLLEY_HPP_ */
