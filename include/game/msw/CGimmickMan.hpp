#ifndef MSW_CGIMMICKMAN_HPP_
#define MSW_CGIMMICKMAN_HPP_

#include "bcs/CUnivTask.hpp"
#include "game/msw/CDropCoinMan.hpp"
#include "game/msw/CQPanelMan.hpp"

namespace bcs {
    struct FVEC3;
}

namespace msw {

    class CChrCore;
    class CCollBodyUnit;
    class CDropCoin;
    class CItem;
    class CItemMan;
    class CQPanel;

    // The stage gimmicks' manager. A DOL class, size 0x40 (the constructor
    // calls bcs::CTask's with priority 0x13 and clears the three pointers).
    // Each game module derives its own (CGimmickManBoss, ...).
    //
    // The virtual functions are in the order of the slots of
    // __vt__Q23msw15CGimmickManBoss (BossF). Return types not seen in a body
    // are guesses.
    class CGimmickMan : public bcs::CUnivTask {
        public:
            CGimmickMan(bcs::CTask *pParent);
            virtual ~CGimmickMan();
            virtual void Main();

            virtual void Initialize_();
            virtual void SetUp_();
            virtual void AllReset_();
            virtual void QPanelAllReset_();
            virtual long SendMessage_(long, long, long, long);
            virtual CQPanel *QPanelGetFirst_();
            virtual CQPanel *QPanelGetNext_(CQPanel *);
            virtual CQPanel *QPanelGetNear_(const bcs::FVEC3 &);
            virtual long QPanelChoicePopupItem_(long, int);
            virtual void QPanelSetViewBaseRot_(float);
            // Returns what the panel manager's check returns, 0 without one.
            virtual long QPanelCheck_(const bcs::FVEC3 &, long, int, int, int, long *, CItem **);
            virtual void QPanelSetItemPopRate_(const CQPanelMan::ITEM_POP_SET_ARG *, long);
            virtual long QPanelGetPopupItemTypeNum_(long);
            virtual long QPanelGetSubstituteItem_(long);
            virtual long CreateDropCoin_(const bcs::FVEC3 &, long); // returns the number of coins made
            virtual long CreateBuryCoin_(const bcs::FVEC3 &, const bcs::FVEC3 &, long); // the number made
            virtual CDropCoin *CreateCoinSimple_();
            virtual void DropCoinPickup_(const bcs::FVEC3 &, float, float, void (*)(const CDropCoin *, void *),
                                         void *);
            virtual CDropCoin *DropCoinGetFirst_();
            virtual CDropCoin *DropCoinGetNext_(CDropCoin *);
            virtual CDropCoin *CreateStarPiece_(DROP_COIN::CoinType, long, const bcs::FVEC3 &, const bcs::FVEC3 &,
                                                long);
            virtual long GetStarPieceNum_() const;
            virtual void DeleteAllStarPiece_();
            virtual void SetCoinRefelctVelReduce_(float, float);
            virtual void SetStarPieceRefelctVelReduce_(float, float);
            virtual CItem *CreateItem_(long, const bcs::FVEC3 &);
            virtual CItem *CreateStockItem_(long, CChrCore *);
            virtual CItem *CreateThrowItem_(long, const bcs::FVEC3 &);
            virtual CItem *CreateDriftItem_(long, const bcs::FVEC3 &, const bcs::FVEC3 &, float, float);
            virtual void ItemPickup_(const bcs::FVEC3 &, float, float, void (*)(const CItem *, void *), void *);
            virtual void ItemPickup_(CCollBodyUnit *, void (*)(const CItem *, void *), void *);
            virtual void EffectItemGetCreate_(long, const bcs::FVEC3 &, float);
            virtual void *ItemGetIconTexObj_(long, int);
            virtual long ItemGetNum_();
            virtual CItem *ItemGetFirst_();
            virtual CItem *ItemGetNext_(CItem *);
            virtual long ThrowItemGetNum_();
            virtual CItem *ThrowItemGetFirst_();
            virtual CItem *ThrowItemGetNext_(CItem *);
            virtual void ItemExParamInitialize_();
            virtual void ItemExParamSet_Life_(long, float);
            virtual void ItemExParamSet_DispScale_(long, float);
            virtual void ItemExParamSet_ThrowSpeed_(long, float);
            virtual void ItemExParamSet_TurnSpeed_(long, float);
            virtual void ItemExParamSet_EffectFrame_(long, float);
            virtual void ItemExParamSet_DropCoinRate_(long, float);
            virtual void ItemExParamSet_DropCoinMin_(long, long);
            virtual void ItemExParamSet_DropCoinMax_(long, long);
            virtual void ItemExParamSet_ReboundNum_(long, long);
            virtual void ItemExParamSet_PThrowDist_(long, float);
            virtual void ItemExParamSet_PThrowHeight_(long, float);
            virtual void ItemExParamSet_PThrowFrame_(long, float);
            virtual void ItemExParamSet_ExplodeRange_(long, float);
            virtual void StgGmkCreate_Bubble_(const bcs::FVEC3 &);
            virtual void StgGmkCreate_Karon_(const bcs::FVEC3 &);
            virtual void EffectCoinGetCreate_(const bcs::FVEC3 &, DROP_COIN::CoinType);
            virtual void EffectCoinScoreCreate_(const bcs::FVEC3 &, const bcs::FVEC3 &, long);
            virtual long FloorHitCheck_(const bcs::FVEC3 &, long);
            // FloorHitCheck_ is the last slot: the DOL's vtable of
            // CGimmickMan and BasketF's __vt__Q23msw17CGimmickManBasket end
            // with it. QPanelChoicePopupCoin_ is a new virtual of the
            // Boss/Dodge/Hockey/Volley managers (declared in each of them).

            // Shows a bomb blast at rPos, scaled by fScale. What the two longs
            // select is not known; the boss bomb passes 0 and 0x48. (return
            // type guessed)
            static void EffectBombExplosionCreate(const bcs::FVEC3 &rPos, long, float fScale, long);
            // (DOL) The instance's EffectCoinGetCreate_. (return type guessed)
            static void EffectCoinGetCreate(const bcs::FVEC3 &rPos, DROP_COIN::CoinType type);
            // (DOL) The instance's QPanelChoicePopupItem_. (return type guessed)
            static long QPanelChoicePopupItem(long nPlayer, int n);
            // (DOL) The instance's CreateItem_: an item of type nType at rPos.
            static CItem *CreateItem(long nType, const bcs::FVEC3 &rPos);
            // DOL, static: AllReset_ on the instance.
            static void AllReset();

        protected:
            // The instance is a plain static pointer in the DOL (no task
            // handle). (return type of SetInstance_ guessed)
            static void SetInstance_(CGimmickMan *pInstance);
            static CGimmickMan *GetInstance_();

            CDropCoinMan *m_pDropCoinMan; // 0x34
            CQPanelMan *m_pQPanelMan;     // 0x38
            CItemMan *m_pItemMan;         // 0x3C
    };
}

#endif /* !MSW_CGIMMICKMAN_HPP_ */
