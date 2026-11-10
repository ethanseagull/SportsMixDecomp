#ifndef MSW_CSTAGEMDL_HPP_
#define MSW_CSTAGEMDL_HPP_

#include "bcs/CScnObj.hpp"

namespace msw {

    // A model of the stage, as held in CStageMan's model pool. Size 0x10CC
    // (CStageMan::DoCreateMdl allocates it), with the vptr at 0x10C8 after
    // the data. Only the scene object at 0x04 is mapped (BasketF
    // CStageBasket13::UpdateMdl turns it about Y and rebuilds its matrix).
    // None of the virtual functions is named in the DOL (vtable at
    // 0x804740F8); the slots before the one called are placeholders.
    class CStageMdl {
        public:
            // (name guessed)
            bcs::CScnObj *GetScnObj() const { return m_pScnObj; }

        private:
            unsigned char m_Unk00[0x4];     // 0x00
            bcs::CScnObj *m_pScnObj;        // 0x04
            unsigned char m_Unk08[0x10C0];  // 0x08

        public:
            virtual void Unk08();
            virtual void Unk0C();
            virtual void Unk10();
            virtual void Unk14();
            virtual void Unk18();
            virtual void Unk1C();
            virtual void Unk20();
            virtual void Unk24();
            virtual void Unk28();
            virtual void Unk2C();
            virtual void Unk30();
            virtual void Unk34();
            virtual void Unk38();
            virtual void Unk3C();
            virtual void Unk40();
            virtual void Unk44();
            virtual void Unk48();
            virtual void Unk4C();
            virtual void Unk50();
            virtual void Unk54();
            virtual void Unk58();
            virtual void Unk5C();
            // (name guessed) 0x60, fn_8016A3BC: passes the rate on to every
            // object of the model (their slot 0x4C, with -1). BasketF's
            // CStageBasketHungryBossPackun72 sets its belt's speed with it.
            virtual void SetAnmRate(float rate);
    };
}

#endif /* !MSW_CSTAGEMDL_HPP_ */
