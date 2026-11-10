#ifndef BCS_G2D_CLAYOUTDL_HPP_
#define BCS_G2D_CLAYOUTDL_HPP_

namespace bcs {
    namespace G2D {

        // Lives in the DOL. Only the members used so far.
        class CLayoutDL {
            public:
                void SetGDLMemArea(long nArea);

            private:
                unsigned char m_Unk00[0x2C];

            public:
                // 0x2C. (name guessed) What msw::CSelectMenuLayoutBase passes
                // to msw::CLytCtrl::SetTextBoxFromMessage with the layout.
                const char *m_pMessageName;
        };
    }
}

#endif /* !BCS_G2D_CLAYOUTDL_HPP_ */
