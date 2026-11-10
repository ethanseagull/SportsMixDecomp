#ifndef MSW_CITEMMAN_HPP_
#define MSW_CITEMMAN_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    class CItem;
    class CChrCore;

    // INCOMPLETE: data members are not mapped (the DOL functions touch
    // offsets up to 0x70; the size is not known). Return types are guesses
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

        private:
            unsigned char m_Unk34[0x40]; // 0x34
    };
}

#endif /* !MSW_CITEMMAN_HPP_ */
