#ifndef MSW_CSTAGE04BASE_HPP_
#define MSW_CSTAGE04BASE_HPP_

#include "bcs/CListNodeBase.hpp"

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
            // (name guessed) One cart: plays the cart's sound when it comes
            // past the camera. Defined in game/msw/CStage04Base.cpp.
            class CCartNode;

            CStage04Base(CStageMan *pStage);

        private:
            CStageMan *m_pStage;              // 0x00
            // 0x04 (name guessed): the carts Stage04_RegistCart made, updated
            // by Stage04_Main and released by Stage04_ReleaseCart (Clear,
            // through the list's vptr at 0x28). Its own vtable is
            // lbl_8048E4C8.
            bcs::CList<CCartNode> m_CartList;

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
