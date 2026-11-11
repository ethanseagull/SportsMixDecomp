#ifndef MSW_CSTGGMK_CASE_VOLLEY_HPP_
#define MSW_CSTGGMK_CASE_VOLLEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Case.hpp"
#include "bcs/BCS_Rand.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgGmk_Case_Act.hpp"
#include "game/msw/VolleyPrm.hpp"

namespace msw {

    // The cases of the ship stage (stage 15) in the basketball mode.

    // The actor, size 0x214 (nothing added to msw::CStgGmk_Case_Act), with
    // the VOLLEY_PRM_S15_* parameters. Its functions are in
    // game/volley/CStgSeSlot.cpp (the unit named after the CStgSeSlot weak
    // functions at its end); unlike the dodgeball actor it keeps
    // CStgGmk_Case_Act::GetParam_OuterRect and OnDamage. The constructor and
    // the parameter getters are inline: the unit emits the getters as weak
    // functions, in reverse order of the definitions here.
    class CStgGmk_Case_Act_Volley : public CStgGmk_Case_Act {
        public:
            CStgGmk_Case_Act_Volley(CStgGmk *pParent) : CStgGmk_Case_Act(pParent) {}
            virtual ~CStgGmk_Case_Act_Volley();

            virtual float GetParam_Width() const { return VOLLEY_PRM_S15_CASE_WIDTH; }
            virtual float GetParam_Depth() const { return VOLLEY_PRM_S15_CASE_DEPTH; }
            virtual float GetParam_Height() const { return VOLLEY_PRM_S15_CASE_HEIGHT; }
            virtual float GetParam_MdlWidth() const { return 2.3f; }
            virtual float GetParam_MdlDepth() const { return 1.0f; }
            virtual float GetParam_MdlHeight() const { return 1.0f; }
            virtual float GetParam_Acc() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return VOLLEY_PRM_S15_CASE_ACC_0;
                    case 1:
                        return VOLLEY_PRM_S15_CASE_ACC_1;
                    case 2:
                        return VOLLEY_PRM_S15_CASE_ACC_2;
                }
                return VOLLEY_PRM_S15_CASE_ACC_0;
            }
            virtual float GetParam_Dec() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return VOLLEY_PRM_S15_CASE_DEC_0 + bcs::BCS_RandMax(VOLLEY_PRM_S15_CASE_DEC_RND_0);
                    case 1:
                        return VOLLEY_PRM_S15_CASE_DEC_1 + bcs::BCS_RandMax(VOLLEY_PRM_S15_CASE_DEC_RND_1);
                    case 2:
                        return VOLLEY_PRM_S15_CASE_DEC_2 + bcs::BCS_RandMax(VOLLEY_PRM_S15_CASE_DEC_RND_2);
                }
                return VOLLEY_PRM_S15_CASE_DEC_0 + bcs::BCS_RandMax(VOLLEY_PRM_S15_CASE_DEC_RND_0);
            }
            virtual float GetParam_VelLimit() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return VOLLEY_PRM_S15_CASE_VEL_0;
                    case 1:
                        return VOLLEY_PRM_S15_CASE_VEL_1;
                    case 2:
                        return VOLLEY_PRM_S15_CASE_VEL_2;
                }
                return VOLLEY_PRM_S15_CASE_VEL_0;
            }
            // The cases have no life in volleyball: every kind gets 0.
            virtual float GetParam_MaxLife() const
            {
                static const float sMaxLife[] = { 0.0f, 0.0f, 0.0f };
                const float *pMaxLife = sMaxLife;
                switch (GetSubType_()) {
                    case 0:
                        return pMaxLife[0];
                    case 1:
                        return pMaxLife[1];
                    case 2:
                        return pMaxLife[2];
                }
                return pMaxLife[0];
            }
            // The parameter is in seconds; the actor counts frames.
            virtual float GetParam_Interval() const { return 60.0f * VOLLEY_PRM_S15_CASE_INTERVAL; }
            virtual void GetParam_CourtRect(float *pLeft, float *pRight, float *pFront, float *pBack) const
            {
                CStageMan::GetInstance()->GetGroundRect(pLeft, pRight, pFront, pBack);
            }
            virtual float GetParam_LocPosY() const { return VOLLEY_PRM_S15_CASE_POS_Y; }
            virtual float GetParam_KaronScale() const { return VOLLEY_PRM_S15_KARON_SCALE; }
            virtual float GetParam_KaronCollScale() const { return VOLLEY_PRM_S15_KARON_COLL_SCALE; }
            virtual float GetParam_KaronSpeed() const { return VOLLEY_PRM_S15_KARON_SPEED; }
            virtual void OnBreak() { CreateKaron(); }

            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_SE(long type) const;
            virtual void UpdateColl();
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);
    };

    // One case, size 0x78 (nothing added to msw::CStgGmk_Case). Made by
    // msw::CStageVolley15::NewCase. All inline: game/volley/CStageVolley15.cpp
    // emits the vtable and, after CStgGmk_Case's weak functions, NewAct and
    // the destructor (reverse order of the declarations here).
    class CStgGmk_Case_Volley : public CStgGmk_Case {
        public:
            CStgGmk_Case_Volley(bcs::CTask *pParent) : CStgGmk_Case(pParent, 0) {}
            virtual ~CStgGmk_Case_Volley() {}

            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Case_Act_Volley(this); }
    };
}

#endif /* !MSW_CSTGGMK_CASE_VOLLEY_HPP_ */
