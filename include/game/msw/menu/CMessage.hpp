#ifndef MENU_CMESSAGE_HPP_
#define MENU_CMESSAGE_HPP_

namespace msw {
    namespace menu {

        // Size 0x14. Only the members used so far; the data starts with a
        // nw4r::ut::List.
        class CMessage {
            public:
                void Initialize();
                void Finalize();
                // Returns -1 when pData is null.
                long AttachResource(const void *pData); // return type guessed

                static CMessage s_Instance;

            private:
                unsigned char m_Unk00[0x14];
        };
    }
}

#endif /* !MENU_CMESSAGE_HPP_ */
