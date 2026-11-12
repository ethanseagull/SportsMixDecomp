#ifndef MSW_CITEMMAN_HPP_
#define MSW_CITEMMAN_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/ut/List.hpp"

namespace msw {

    class CItem;
    class CChrCore;
    class CCollBodyUnit;

    // INCOMPLETE: data members are not mapped. Size 0x344 (CItemManBoss is
    // allocated with that size and adds nothing). Return types are guesses
    // unless a body is given.
    //
    // Creates and owns the items of a game. A DOL class; each game module
    // derives its own (CItemManHockey in HockeyF) to create its own item
    // classes. The virtual functions are in the order of the DOL's vtable
    // (lbl_8046FC58); the bodies given here are those of the DOL's unnamed
    // functions in that vtable, which HockeyF also emits as weak functions.
    class CItemMan : public bcs::CUnivTask {
        public:
            CItemMan(bcs::CTask *pParent);
            virtual ~CItemMan();
            virtual void Main();

            virtual long SendMessage(long, long, long, long);
            // Creates an item of the given kind (CItem::KIND_*) at pos.
            virtual CItem *CreateItem(long kind, const bcs::FVEC3 &pos) { return 0; }
            virtual CItem *CreateStockItem(long kind, CChrCore *pChr);
            virtual CItem *CreateThrowItem(long kind, const bcs::FVEC3 &pos) { return 0; }
            virtual CItem *CreateDriftItem(long kind, const bcs::FVEC3 &pos, float, float);
            virtual void EffectItemGetCreate(long kind, const bcs::FVEC3 &pos, float);
            virtual void MainFunc() {}
            virtual void ExParamInitialize_() {}

            // (DOL, unnamed fn_80126DB4 ... fn_80127754; names guessed from
            // the CGimmickMan functions that call them) Removes every item;
            // calls pFunc for the items touched at rPos or by pBody; the
            // icon of an item kind; the number of items and the walk of the
            // thrown items; the per-kind extra parameters.
            void AllReset();
            void ItemPickup(const bcs::FVEC3 &rPos, float fRadius, float fHeight,
                            void (*pFunc)(const CItem *, void *), void *pArg);
            void ItemPickup(CCollBodyUnit *pBody, void (*pFunc)(const CItem *, void *), void *pArg);
            void *GetIconTexObj(long nKind, int);
            long ItemGetNum();
            long ThrowItemGetNum();
            CItem *ThrowItemGetFirst();
            CItem *ThrowItemGetNext(CItem *pItem);
            void ExParamInitialize();
            void ExParamSet_Life(long nKind, float);
            void ExParamSet_DispScale(long nKind, float);
            void ExParamSet_ThrowSpeed(long nKind, float);
            void ExParamSet_TurnSpeed(long nKind, float);
            void ExParamSet_EffectFrame(long nKind, float);
            void ExParamSet_DropCoinRate(long nKind, float);
            void ExParamSet_DropCoinMin(long nKind, long);
            void ExParamSet_DropCoinMax(long nKind, long);
            void ExParamSet_ReboundNum(long nKind, long);
            void ExParamSet_PThrowDist(long nKind, float);
            void ExParamSet_PThrowHeight(long nKind, float);
            void ExParamSet_PThrowFrame(long nKind, float);
            void ExParamSet_ExplodeRange(long nKind, float);

            // The walk of every item (inline: CGimmickMan's ItemGetFirst_
            // and ItemGetNext_ call List_GetNext on the list at 0x34).
            CItem *ItemGetFirst() { return static_cast<CItem *>(nw4r::ut::List_GetFirst(&m_ItemList)); }
            CItem *ItemGetNext(CItem *pItem) { return static_cast<CItem *>(nw4r::ut::List_GetNext(&m_ItemList, pItem)); }

        protected:
            nw4r::ut::List m_ItemList;           // 0x34, the items (name guessed)
            unsigned char m_Unk40[0x344 - 0x40]; // 0x40
    };
}

#endif /* !MSW_CITEMMAN_HPP_ */
