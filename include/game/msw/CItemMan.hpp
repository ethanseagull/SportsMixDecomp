#ifndef MSW_CITEMMAN_HPP_
#define MSW_CITEMMAN_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/ut/List.hpp"
#include "revolution/gx/GXTexture.h"

namespace msw {

    class CItem;
    class CChrCore;
    class CCollBodyUnit;

    // Size 0x344 (CItemManBoss is allocated with that size and adds
    // nothing). Return types are guesses unless a body is given. Source:
    // src/game/msw/CItemMan.cpp (the DOL file also holds CItem).
    //
    // Creates and owns the items of a game. A DOL class; each game module
    // derives its own (CItemManHockey in HockeyF) to create its own item
    // classes. The virtual functions are in the order of the DOL's vtable
    // (lbl_8046FC58); the bodies given here are those of the DOL's unnamed
    // functions in that vtable, which HockeyF also emits as weak functions.
    // The base is CUnivTaskEx: the DOL destructor tests `this` twice before
    // calling bcs::CTask's.
    class CItemMan : public bcs::CUnivTaskEx {
        public:
            CItemMan(bcs::CTask *pParent);
            virtual ~CItemMan();
            virtual void Main();

            // (void: the DOL body leaves r3 as it is; nobody reads a result)
            // Message 0xB hides every item lying on the field or drifting
            // and removes the drifting ones.
            virtual void SendMessage(long nMsg, long, long, long);
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

            // Number of item kinds (CItem::KIND_*, 7 to 11 included).
            enum { KIND_NUM = 12 };

            // (names guessed) The per-kind parameters the ExParamSet_
            // functions set; each sets one value and its bit in nFlag.
            // Defaults from ExParamInitialize in brackets.
            struct EX_PARAM {
                unsigned long nFlag;   // 0x00, which values were set
                float fLife;           // 0x04 (600), bit 0x1
                float fUnk08;          // 0x08 (600), no setter
                float fDispScale;      // 0x0C (1), bit 0x2
                float fThrowSpeed;     // 0x10 (1), bit 0x4
                float fTurnSpeed;      // 0x14 (0), bit 0x8
                float fEffectFrame;    // 0x18 (0), bit 0x10
                float fDropCoinRate;   // 0x1C (0), bit 0x20
                long nDropCoinMin;     // 0x20 (0), bit 0x40
                long nDropCoinMax;     // 0x24 (0), bit 0x80
                long nReboundNum;      // 0x28 (1), bit 0x100
                float fPThrowDist;     // 0x2C (1), bit 0x200
                float fPThrowHeight;   // 0x30 (1), bit 0x400
                float fPThrowFrame;    // 0x34 (30), bit 0x800
                float fExplodeRange;   // 0x38 (1), bit 0x1000
            };

        protected:
            nw4r::ut::List m_ItemList;           // 0x34, the items (name guessed)
            GXTexObj *m_pIconTexBuf;             // 0x40, KIND_NUM texture objects
            GXTexObj *m_pIconTex[KIND_NUM];      // 0x44, per kind, null when its file has no icon
            EX_PARAM m_ExParam[KIND_NUM];        // 0x74
    };
}

#endif /* !MSW_CITEMMAN_HPP_ */
