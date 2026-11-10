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
                // Added for BasketF game/basket/CCockpitBasketPauseResult.cpp.
                // The index of message file pName, and the text called
                // pLabel in file nFile (null when not found; types guessed).
                short GetFileIndex(const char *pName);
                const wchar_t *GetTextByLabel(short nFile, const char *pLabel);

                // (no symbol; a guess) The callers test the address of
                // s_Instance against null, so they reach it through an
                // inline function.
                static CMessage *GetInstance()
                {
                    return &s_Instance;
                }

                static CMessage s_Instance;

            private:
                unsigned char m_Unk00[0x14];
        };
    }
}

#endif /* !MENU_CMESSAGE_HPP_ */
