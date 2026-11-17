#ifndef MSW_CEFFECTITEMGET_HPP_
#define MSW_CEFFECTITEMGET_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    // INCOMPLETE: data members are not mapped. (class name guessed; nothing
    // of it is named in the DOL: constructor fn_8012C260, vtable
    // lbl_8046FC40) The effect shown where an item is picked up, made by
    // CItemMan::EffectItemGetCreate. A task of priority 0x17 holding a
    // model of the item kind's file. Size 0x60 (EffectItemGetCreate's new).
    // Defined in the DOL's CItemMan.cpp, after CItem.
    class CEffectItemGet : public bcs::CUnivTask {
        public:
            CEffectItemGet(bcs::CTask *pParent, const bcs::FVEC3 &rPos, long nKind, float f);
            virtual ~CEffectItemGet();
            virtual void Main();

        private:
            unsigned char m_Unk34[0x60 - 0x34]; // 0x34
    };
}

#endif /* !MSW_CEFFECTITEMGET_HPP_ */
