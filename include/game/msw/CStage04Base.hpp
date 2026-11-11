#ifndef MSW_CSTAGE04BASE_HPP_
#define MSW_CSTAGE04BASE_HPP_

namespace bcs {
    class CMdlObj;
}

namespace msw {

    class CStageMan;

    // (purpose guessed from the names) The extra part of a sport's stage 04,
    // the stage with the cart: a second base class next to the sport's
    // stage manager (DodgeF's msw::CStageDodge04 has it at 0x244). A DOL
    // class, size 0x30: the stage it belongs to at 0x00 and a member object
    // at 0x04 (data declared first), vptr at 0x2C. Its constructor,
    // destructor and virtual functions are in the DOL; the order of the
    // virtual functions is that of its DOL vtable (lbl_8048E4B0).
    class CStage04Base {
        public:
            CStage04Base(CStageMan *pStage);

        private:
            CStageMan *m_pStage;          // 0x00
            // 0x04: an object of 0x28 bytes with its own vptr at 0x24 and a
            // bcs::CListNodeBase at 0x10 (the DOL constructor builds it in
            // place); not mapped. 0x28 of it is cleared by
            // Stage04_ReleaseCart.
            unsigned char m_Unk04[0x28];

        public:
            virtual ~CStage04Base();
            // (meaning of the second argument not known: CStageDodge04
            // passes 4) The cart model ("^s[0-9]+_cart$").
            virtual void Stage04_RegistCart(bcs::CMdlObj *pMdl, long);
            virtual void Stage04_ReleaseCart();
            // Called by the stage's Main_ every frame.
            virtual void Stage04_Main(float frame);
    };
}

#endif /* !MSW_CSTAGE04BASE_HPP_ */
