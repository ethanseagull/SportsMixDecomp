#ifndef MSW_CSTAGEBASKET02_HPP_
#define MSW_CSTAGEBASKET02_HPP_

#include "game/msw/CStage02Base.hpp"
#include "game/msw/CStageManBasket.hpp"

namespace msw {

    // Basket stage 02, the wave stage. Size 0x24C: CStageManBasket, then
    // CStage02Base at 0x218. The waves (msw::CStgGmk_WaveCluster_Basket)
    // push and pull whoever stands on them; the tuning is in the
    // BASKET_PRM_S02_* globals. Every hook is handed on to CStage02Base.
    //
    // Overrides are declared in the order of their vtable slots, except
    // that the inline ones come in the order that gives the original's
    // order of their weak copies (reverse declaration order).
    class CStageBasket02 : public CStageManBasket, public CStage02Base {
        public:
            CStageBasket02(bcs::CTask *pParent);
            virtual ~CStageBasket02();

            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            // 3 on a wave (water, presumably), else the stage's own.
            virtual long GetHitMaterial(const bcs::FVEC3 &rPos) const;
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual bool OnPreCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                        nw4r::g3d::ResFile file, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnPostInitialize();
            virtual void OnGimmickReset();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            // No question panels on this stage.
            virtual long GetQPanelEnabled_() const { return 0; }
            virtual void OnPeriodInit() { Stage02_OnPeriodInit(); }
            virtual void OnGimmickPause() { Stage02_OnGimmickPause(); }
            virtual void OnGimmickResume() { Stage02_OnGimmickResume(); }
            virtual unsigned long OnCalcSum() const { return Stage02_OnCalcSum(); }

            virtual float GetParam_PushPower() const;
            virtual float GetParam_PullPower() const;
            virtual float GetParam_WavePosMargin() const;
            virtual CStgGmk_WaveCluster *NewWaveCluster(bcs::CTask *pParent) const;
    };
}

#endif /* !MSW_CSTAGEBASKET02_HPP_ */
