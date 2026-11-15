#ifndef MENU_CMESSAGE_HPP_
#define MENU_CMESSAGE_HPP_

#include "nw4r/ut/List.hpp"

namespace msw {
    namespace menu {

        // Size 0x14: the list of loaded message files, one unknown word,
        // then the vptr at 0x10 (the data is declared first so that the
        // vptr follows it; the static initialiser stores the vtable there).
        class CMessage {
            private:
                // The loaded message files (CMessage.cpp's Entry), linked at
                // offset 0.
                nw4r::ut::List m_FileList; // 0x00
                unsigned char m_Unk0C[0x4];
                // 0x10: vptr

            public:
                virtual ~CMessage();

                void Initialize();
                void Finalize();
                // Returns -1 when pData is null.
                long AttachResource(const void *pData); // return type guessed
                // Added for MenuF game/menu/CEntryExplain.cpp. The index of
                // the message file named pName, or -1 (return type guessed).
                // Returns short: the BasketF/DodgeF/HockeyF/VolleyF pause
                // result pages extend the result as a short (extsh) before
                // passing it on to GetTextByLabel, and all 16 callers score
                // lower with a long return.
                short GetFileIndex(const char *pName);
                // The text of label pLabel in file nFile; null when nFile is
                // negative or pLabel null.
                const wchar_t *GetTextByLabel(short nFile, const char *pLabel);
                // (added for MenuF game/menu/CSelectMenu.cpp; return types
                // guessed) Text nIndex of file nFile, and the index of label
                // pLabel in it.
                const wchar_t *GetText(short nFile, short nIndex);
                short GetLabelIndex(short nFile, const char *pLabel);
                // Size in bytes of text nIndex of file nFile, 0 if there is
                // none (names guessed; DOL fn_801B0724).
                short GetTextSize(short nFile, short nIndex);
                // The same in characters, by label (fn_801B078C) and by
                // index (fn_801B0818).
                short GetTextLengthByLabel(short nFile, const char *pLabel);
                // (inline in CMessage.cpp) The size by label.
                short GetTextSizeByLabel(short nFile, const char *pLabel);
                short GetTextLength(short nFile, short nIndex);
                // The number of texts in file nFile: 0 when nFile is
                // negative, -1 when there is no such file (DOL fn_801B0988).
                short GetTextNum(short nFile);
                // The label of text nIndex of file nFile, in a static
                // buffer; null if there is none (DOL fn_801B09D8).
                const char *GetLabelByIndex(short nFile, short nIndex);

                // (no symbol; a guess) The callers test the address of
                // s_Instance against null, so they reach it through an
                // inline function.
                static CMessage *GetInstance()
                {
                    return &s_Instance;
                }

                static CMessage s_Instance;
        };
    }
}

#endif /* !MENU_CMESSAGE_HPP_ */
