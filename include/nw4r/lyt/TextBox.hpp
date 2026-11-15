#ifndef NW4R_LYT_TEXTBOX_HPP_
#define NW4R_LYT_TEXTBOX_HPP_

#include "nw4r/lyt/Pane.hpp"
#include "nw4r/ut/RuntimeTypeInfo.h"

namespace nw4r {
    namespace lyt {

        // A pane that draws a string. Lives in the DOL. Only the members used
        // so far. The vtable slots are read from
        // msw::CMenuPaneAccessor::SetTextBoxString in the DOL (0x74, 0x80)
        // and msw::CCockpitHockeyCoin (0x7C); the names follow the public
        // ogws decompilation's description of nw4r::lyt::TextBox (from
        // memory, no code copied). Return types are guesses.
        class TextBox : public Pane {
            public:
                // Added for MenuF game/menu/CBgDisplay.cpp (ut::DynamicCast).
                static const ut::detail::RuntimeTypeInfo typeInfo;

                virtual ~TextBox();
                // 0x70 is Pane's LoadMtx, which TextBox overrides (DOL vtable
                // lbl_804BE668); GetStringBufferLength is not virtual.
                virtual void LoadMtx(const DrawInfo &rInfo);                    // 0x70
                unsigned short GetStringBufferLength() const;
                virtual void AllocStringBuffer(unsigned short nLen);            // 0x74
                virtual void FreeStringBuffer();                                // 0x78
                virtual unsigned short SetString(const wchar_t *pStr, unsigned short nDstIdx); // 0x7C
                virtual unsigned short SetString(const wchar_t *pStr, unsigned short nDstIdx,
                                                 unsigned short nLen);          // 0x80

                const wchar_t *GetStringBuffer() const { return mTextBuf; }
                // Added for BossF game/boss/CCockpitBossCoder.cpp.
                const Size &GetFontSize() const { return mFontSize; }
                void SetFontSize(const Size &rSize) { mFontSize = rSize; }
                // Added for game/msw/CMenuPaneAccessor.cpp: the number of
                // characters in the string (name guessed).
                unsigned short GetStringLength() const { return mTextLen; }

            private:
                wchar_t *mTextBuf;         // 0xD8
                unsigned char m_UnkDC[0xC];
                Size mFontSize;            // 0xE8 (name from the ogws description)
                // Added for game/msw/CMenuPaneAccessor.cpp; offsets from
                // tools/decomp/layout.py (u16 at 0xFC and 0xFE, u8 at 0x100
                // and 0x101), names from the ogws description.
                unsigned char m_UnkF0[0xC];
                unsigned short mTextBufBytes; // 0xFC
                unsigned short mTextLen;      // 0xFE
                unsigned char mTextPosition;  // 0x100
                unsigned char m_Unk101[3];
        };
    }
}

#endif /* !NW4R_LYT_TEXTBOX_HPP_ */
