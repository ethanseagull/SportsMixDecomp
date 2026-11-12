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
    class CStgGmk_Bubble;
    class CStgGmk_Karon;

    // The stage gimmicks' manager. A DOL class, size 0x40 (the constructor
    // calls bcs::CTask's with priority 0x13 and clears the three pointers).
    // Each game module derives its own (CGimmickManBoss, ...).
    //
    // The virtual functions are in the order of the slots of
    // __vt__Q23msw15CGimmickManBoss (BossF). Return types not seen in a body
    // are guesses.
    //
    // The base is bcs::CUnivTaskEx: the DOL destructor tests `this` three
    // times before calling bcs::CTask's.
    class CGimmickMan : public bcs::CUnivTaskEx {
        public:
            CGimmickMan(bcs::CTask *pParent);
            virtual ~CGimmickMan();
            // (DOL, static; added for game/dodge/CChrCoreDodgeDodgeOrDie.cpp;
            // return type and meanings of the ints guessed) Checks the
            // question panels at rPos for team nTeam.
            static long QPanelCheck(const bcs::FVEC3 &rPos, long nTeam, int, int, int, long *, CItem **);
            virtual void Main();

            virtual void Initialize_();
            virtual void SetUp_();
            virtual void AllReset_();
            virtual void QPanelAllReset_();
            // Passes the message on to the three managers (returns nothing:
            // the DOL body leaves the last call's result in r3 by chance).
            virtual void SendMessage_(long, long, long, long);
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
            // (returns nothing: the DOL body ends in a tail call after a
            // beqlr)
            virtual void CreateStarPiece_(DROP_COIN::CoinType, long, const bcs::FVEC3 &, const bcs::FVEC3 &, long);
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
            virtual CStgGmk_Bubble *StgGmkCreate_Bubble_(const bcs::FVEC3 &rPos); // a new bubble at rPos (stage 11 only)
            virtual CStgGmk_Karon *StgGmkCreate_Karon_(const bcs::FVEC3 &rPos); // a new karon at rPos
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
            // (DOL; added for BasketF game/basket/CComManBasketGame.cpp) The
            // instance, and the walk of the items on the field (static: the
            // DOL bodies take no object).
            static CGimmickMan *GetInstance();
            static CItem *ItemGetFirst();
            static CItem *ItemGetNext(CItem *pItem);

            static void EffectBombExplosionCreate(const bcs::FVEC3 &rPos, long, float fScale, long);
            // (DOL) The instance's EffectCoinGetCreate_. (return type guessed)
            static void EffectCoinGetCreate(const bcs::FVEC3 &rPos, DROP_COIN::CoinType type);
            // (DOL) The instance's QPanelChoicePopupItem_. (return type guessed)
            static long QPanelChoicePopupItem(long nPlayer, int n);
            // (DOL) The instance's CreateItem_: an item of type nType at rPos.
            static CItem *CreateItem(long nType, const bcs::FVEC3 &rPos);
            // (DOL, static; added for game/msw/CStgGmk_Wave.cpp) The
            // instance's CreateBuryCoin_: nNum coins buried in the rectangle
            // from rMin to rMax; 0 without an instance.
            static long CreateBuryCoin(const bcs::FVEC3 &rMin, const bcs::FVEC3 &rMax, long nNum);
            // (DOL, static; added for game/msw/CStgGmk_Wave.cpp) The
            // instance's CreateDriftItem_; null without an instance.
            static CItem *CreateDriftItem(long nType, const bcs::FVEC3 &rMin, const bcs::FVEC3 &rMax, float, float);
            // (DOL fn_8012DFFC, static; added for game/msw/CStgGmk_Case.cpp)
            // The instance's StgGmkCreate_Karon_; null without an instance.
            static CStgGmk_Karon *StgGmkCreate_Karon(const bcs::FVEC3 &rPos);
            // DOL, static: AllReset_ on the instance.
            static void AllReset();
            // (DOL, static; added for BasketF
            // game/basket/CGameBasketHungryBossPackun.cpp) SetUp_ and
            // QPanelSetViewBaseRot_ on the instance (guessed from the names).
            static void SetUp();
            static void QPanelSetViewBaseRot(float fRot);
            // (DOL, static; added for BossF game/boss/GameBoss.cpp)
            // QPanelAllReset_ on the instance (guessed from the name).
            static void QPanelAllReset();

            // (DOL, static; added for main game/msw/CGimmickMan.cpp) The
            // remaining wrappers: each calls the virtual function of the same
            // name with a trailing underscore on the instance and returns
            // zero (QPanelChoicePopupItem: -1) without one. The ones whose
            // symbol the DOL does not export are named after the virtual
            // they call.
            static int IsActive();
            static void SendMessage(long, long, long, long);
            static CQPanel *QPanelGetFirst();
            static CQPanel *QPanelGetNext(CQPanel *pPanel);
            static CQPanel *QPanelGetNear(const bcs::FVEC3 &rPos);
            // Passes rPos, nTeam, the first two ints, the fourth int and the
            // two out pointers on: rPos2 and the third int are not used.
            static long QPanelCheck(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rPos2, long nTeam, int, int, int, int,
                                    long *, CItem **);
            static long FloorHitCheck(const bcs::FVEC3 &rPos, long);
            static void QPanelSetItemPopRate(const CQPanelMan::ITEM_POP_SET_ARG *pArg, long);
            static long QPanelGetSubstituteItem(long);
            static CDropCoin *CreateCoinSimple();
            static long CreateDropCoin(const bcs::FVEC3 &rPos, long nNum);
            static void DropCoinPickup(const bcs::FVEC3 &rPos, float fRadius, float fHeight,
                                       void (*pFunc)(const CDropCoin *, void *), void *pArg);
            static CDropCoin *DropCoinGetFirst();
            static CDropCoin *DropCoinGetNext(CDropCoin *pCoin);
            static void EffectCoinScoreCreate(const bcs::FVEC3 &, const bcs::FVEC3 &, long);
            static void CreateStarPiece(DROP_COIN::CoinType type, long, const bcs::FVEC3 &rPos,
                                        const bcs::FVEC3 &rVel, long);
            static long GetStarPieceNum();
            static void DeleteAllStarPiece();
            static void SetStarPieceRefelctVelReduce(float fY, float fXZ);
            static void SetCoinRefelctVelReduce(float fY, float fXZ);
            static CItem *CreateStockItem(long nType, CChrCore *pChr);
            static CItem *CreateThrowItem(long nType, const bcs::FVEC3 &rPos);
            // Does nothing without a body.
            static void ItemPickup(CCollBodyUnit *pBody, void (*pFunc)(const CItem *, void *), void *pArg);
            static void *ItemGetIconTexObj(long, int);
            static long ThrowItemGetNum();
            static CItem *ThrowItemGetFirst();
            static CItem *ThrowItemGetNext(CItem *pItem);
            static void ItemExParamInitialize();
            static void ItemExParamSet_Life(long nType, float);
            static void ItemExParamSet_DispScale(long nType, float);
            static void ItemExParamSet_ThrowSpeed(long nType, float);
            static void ItemExParamSet_TurnSpeed(long nType, float);
            static void ItemExParamSet_EffectFrame(long nType, float);
            static void ItemExParamSet_DropCoinRate(long nType, float);
            static void ItemExParamSet_DropCoinMin(long nType, long);
            static void ItemExParamSet_DropCoinMax(long nType, long);
            static void ItemExParamSet_ReboundNum(long nType, long);
            static void ItemExParamSet_PThrowDist(long nType, float);
            static void ItemExParamSet_PThrowHeight(long nType, float);
            static void ItemExParamSet_PThrowFrame(long nType, float);
            static void ItemExParamSet_ExplodeRange(long nType, float);
            static CStgGmk_Bubble *StgGmkCreate_Bubble(const bcs::FVEC3 &rPos);

        protected:
            // The instance is a plain static pointer in the DOL (no task
            // handle). (return type of SetInstance_ guessed)
            static void SetInstance_(CGimmickMan *pInstance);
            static CGimmickMan *GetInstance_();

            static CGimmickMan *m_pInstance;

            CDropCoinMan *m_pDropCoinMan; // 0x34
            CQPanelMan *m_pQPanelMan;     // 0x38
            CItemMan *m_pItemMan;         // 0x3C
    };
}

#endif /* !MSW_CGIMMICKMAN_HPP_ */
