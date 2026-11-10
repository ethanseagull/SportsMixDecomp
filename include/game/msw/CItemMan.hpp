#ifndef MSW_CITEMMAN_HPP_
#define MSW_CITEMMAN_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    class CChrCore;
    class CItem;

    // Creates and owns the items of a game. A DOL class; each game module
    // derives its own (CItemManBoss, CItemManBasket, ...), which supplies
    // CreateItem and ExParamInitialize_ (no base version exists anywhere).
    //
    // The virtual functions are in the order of the slots of
    // __vt__Q23msw12CItemManBoss (BossF). MainFunc and CreateThrowItem are
    // inline: the modules emit them. Return types not seen in a body are
    // guesses.
    //
    // INCOMPLETE: the members are not mapped. Size 0x344 (CItemManBoss is
    // allocated with that size and adds nothing).
    class CItemMan : public bcs::CUnivTask {
        public:
            // Item types, the values GetItemType returns. 4 is not used in
            // BossF.
            enum {
                ITEM_GSHELL = 0,
                ITEM_RSHELL = 1,
                ITEM_TOADSTOOL = 2,
                ITEM_BOMB = 3,
                ITEM_BANANA = 5,
                ITEM_MUSHROOM = 6
            };

            CItemMan(bcs::CTask *pParent);
            virtual ~CItemMan();
            virtual void Main();

            virtual long SendMessage(long, long, long, long);
            // Creates an item of `type` at rPos; null if out of memory.
            virtual CItem *CreateItem(long type, const bcs::FVEC3 &rPos) = 0;
            virtual CItem *CreateStockItem(long type, CChrCore *pChr);
            virtual CItem *CreateThrowItem(long, const bcs::FVEC3 &) { return 0; }
            virtual CItem *CreateDriftItem(long type, const bcs::FVEC3 &rPos, float, float);
            virtual void EffectItemGetCreate(long type, const bcs::FVEC3 &rPos, float);
            virtual void MainFunc() {}
            virtual void ExParamInitialize_() = 0;

        protected:
            unsigned char m_Unk34[0x344 - 0x34];
    };
}

#endif /* !MSW_CITEMMAN_HPP_ */
