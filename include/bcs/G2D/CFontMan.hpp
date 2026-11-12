#ifndef BCS_G2D_CFONTMAN_HPP_
#define BCS_G2D_CFONTMAN_HPP_

namespace nw4r {
    namespace ut {
        class ArchiveFont;
    }
}

namespace bcs {
    namespace G2D {

        // (class and member names guessed; DOL fn_8002A734 to fn_8002A89C,
        // no symbols) The game's shared fonts: a singleton of 0x14 bytes,
        // four font slots and the vptr at 0x10 (vtable lbl_80465590).
        // msw::ArchiveFont fills the slots.
        class CFontMan {
            public:
                // fn_8002A734: makes the instance when there is none and
                // returns it.
                static CFontMan *CreateInstance();
                // fn_8002A794
                static CFontMan *GetInstance();
                // fn_8002A79C
                static void DestroyInstance();
                // fn_8002A7E4: frees pFont and empties its slot.
                void DeleteFont(nw4r::ut::ArchiveFont *pFont);
                // fn_8002A890 / fn_8002A89C
                nw4r::ut::ArchiveFont *GetFont(long nSlot);
                void SetFont(long nSlot, nw4r::ut::ArchiveFont *pFont);

            private:
                nw4r::ut::ArchiveFont *m_pFont[4]; // 0x00

            public:
                virtual ~CFontMan();
        };
    }
}

#endif /* !BCS_G2D_CFONTMAN_HPP_ */
