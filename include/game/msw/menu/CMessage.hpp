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
                // Added for MenuF game/menu/CEntryExplain.cpp. The index of
                // the message file named pName, or -1 (return type guessed).
                // Returns short: the BasketF/DodgeF/HockeyF/VolleyF pause
                // result pages extend the result as a short (extsh) before
                // passing it on to GetTextByLabel.
                short GetFileIndex(const char *pName);
                // The text of label pLabel in file nFile; null when nFile is
                // negative or pLabel null.
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
