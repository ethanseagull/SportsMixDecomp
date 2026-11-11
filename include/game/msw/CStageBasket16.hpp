#ifndef MSW_CSTAGEBASKET16_HPP_
#define MSW_CSTAGEBASKET16_HPP_

#include "game/msw/CStage16Base.hpp"
#include "game/msw/CStageManBasket.hpp"

namespace msw {

    // Basket stage 16, the star piece stage (BasketF), made by
    // CStageFactoryBasket. Size 0x234: CStageManBasket (0x218), then
    // CStage16Base at 0x218, a DOL class that does the work. This class
    // hands the stage manager's hooks on to it and gives it the
    // BASKET_PRM_S16_* tuning.
    //
    // Overrides are declared in the order of their vtable slots; the
    // CStage16Base functions follow in the order of the new slots of
    // __vt__Q23msw14CStageBasket16.
    class CStageBasket16 : public CStageManBasket, public CStage16Base {
        public:
            CStageBasket16(bcs::CTask *pParent);
            virtual ~CStageBasket16();

            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnPostInitialize();
            virtual void OnGameInit();
            virtual void OnPeriodInit();
            virtual void OnTurnInit();
            virtual void OnEntranceEventStart();
            virtual void OnGimmickReset();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            virtual bool Stage16_IsGimmickReady() const;
            virtual float GetParam_StarPieceAreaZ() const;
            virtual float GetParam_StarPiecePopTime() const;
            virtual float GetParam_StarPiecePopTimeRnd() const;
            virtual float GetParam_StarPiecePopPosY() const;
            virtual float GetParam_StarPieceSpeed() const;
            virtual float GetParam_StarPieceDropAngle() const;
            virtual long GetParam_StarPieceRate(long kind) const;
            virtual long GetParam_StarPieceAddCoin(long kind) const;
            virtual long GetParam_StarPieceMaxNum() const;
            virtual long GetParam_StarPieceBoundNum() const;
            virtual float GetParam_StarPieceBoundReduceY() const;
            virtual float GetParam_StarPieceBoundReduceXZ() const;
            virtual bcs::BCS_Color GetParam_FlashColor() const;
            // A new function, not an override: CStage16Base::GetParam_FlashTime
            // keeps its own constant.
            virtual float GetParam_FlashFrm() const;
            virtual float GetParam_FlashGain() const;
            virtual float GetParam_FlashDecay() const;
            virtual float GetParam_WarpInterval() const;
            virtual float GetParam_WarpTime() const;
            virtual float GetParam_WarpBlurShift() const;
            virtual float GetParam_WarpGameSpeed() const;
    };
}

#endif /* !MSW_CSTAGEBASKET16_HPP_ */
