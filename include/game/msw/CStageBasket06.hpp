#ifndef MSW_CSTAGEBASKET06_HPP_
#define MSW_CSTAGEBASKET06_HPP_

#include "game/msw/CStage06Base.hpp"
#include "game/msw/CStageManBasket.hpp"

namespace msw {

    // Basket stage 06. Size 0x248: CStageManBasket, then CStage06Base at
    // 0x218. The lights over a goal come on as the goal's team scores; the
    // tuning is in the BASKET_PRM_S06_* globals.
    class CStageBasket06 : public CStageManBasket, public CStage06Base {
        public:
            CStageBasket06(bcs::CTask *pParent);
            virtual ~CStageBasket06();

            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long SE_GetSlotNum() const { return 5; }
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnPostInitialize();
            virtual void OnPeriodInit();
            virtual void OnEventStart();
            virtual void OnGimmickReset();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            virtual long GetParam_SeSlot_Illumi() const { return 4; }
            virtual bcs::BCS_Color GetParam_IllumiOnColor() const;
            virtual bcs::BCS_Color GetParam_IllumiOffColor() const;
            virtual long GetParam_IllumiOnGoalCnt() const;
            virtual float GetParam_IllumiOnTime() const;
            virtual bcs::BCS_Color GetParam_FlashColor() const;
            virtual float GetParam_FlashTime() const;
            virtual float GetParam_FlashGain() const;
            virtual float GetParam_FlashDecay() const;
    };
}

#endif /* !MSW_CSTAGEBASKET06_HPP_ */
