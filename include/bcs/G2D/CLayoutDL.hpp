#ifndef BCS_G2D_CLAYOUTDL_HPP_
#define BCS_G2D_CLAYOUTDL_HPP_

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace bcs {
    namespace G2D {

        // Lives in the DOL. Only the members used so far.
        class CLayoutDL {
            public:
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
                const char *m_pMessageName;
        };
    }
}

#endif /* !BCS_G2D_CLAYOUTDL_HPP_ */
