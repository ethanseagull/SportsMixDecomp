#ifndef BCS_G2D_CLAYOUTDL_HPP_
#define BCS_G2D_CLAYOUTDL_HPP_

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace bcs {
    namespace G2D {

        // Lives in the DOL. Only the members used so far. Size 0x34
        // (bcs::G2D::CLayoutBase::CreateLayoutObject allocates it).
        //
        // Built on nw4r::lyt::Layout: its constructor (DOL fn_8002D918)
        // calls Layout's first, and the vtable calls of CLayoutBase are
        // Layout's. The base is not spelled out here because
        // nw4r/lyt/layout.h brings in placement new, which makes the
        // `new (0)` of the units that include this header ambiguous;
        // CLayoutBase.cpp converts to the base itself (it is at offset 0).
        class CLayoutDL {
            public:
                // (added for DOL game/bcs/G2D/CLayoutBase.cpp) The argument
                // is stored at 0x24; what it means is not known.
                CLayoutDL(long nUnk);

                void SetGDLMemArea(long nArea);

                // Added for MenuF game/menu/CBackground.cpp: the layout's
                // root pane (nw4r::lyt::Layout's at 0x10; the class is
                // probably built on it).
                nw4r::lyt::Pane *GetRootPane() const { return m_pRootPane; }

            private:
                unsigned char m_Unk00[0x10];
                nw4r::lyt::Pane *m_pRootPane; // 0x10
                unsigned char m_Unk14[0x18];

            public:
                // 0x2C. (name guessed) What msw::CSelectMenuLayoutBase passes
                // to msw::CLytCtrl::SetTextBoxFromMessage with the layout.
                // bcs::G2D::CLayoutBase::BuildLayout stores the .brlyt name
                // here.
                const char *m_pMessageName;

            private:
                unsigned long m_Unk30; // 0x30
        };
    }
}

#endif /* !BCS_G2D_CLAYOUTDL_HPP_ */
