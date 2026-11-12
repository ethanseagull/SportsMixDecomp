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
                virtual unsigned short GetStringBufferLength() const;           // 0x70
                virtual void AllocStringBuffer(unsigned short nLen);            // 0x74
                virtual void FreeStringBuffer();                                // 0x78
                virtual unsigned short SetString(const wchar_t *pStr, unsigned short nDstIdx); // 0x7C
                virtual unsigned short SetString(const wchar_t *pStr, unsigned short nDstIdx,
                                                 unsigned short nLen);          // 0x80

                const wchar_t *GetStringBuffer() const { return mTextBuf; }
                // Added for BossF game/boss/CCockpitBossCoder.cpp.
                const Size &GetFontSize() const { return mFontSize; }
                void SetFontSize(const Size &rSize) { mFontSize = rSize; }

            private:
                wchar_t *mTextBuf;         // 0xD8
                unsigned char m_UnkDC[0xC];
                Size mFontSize;            // 0xE8 (name from the ogws description)
        };
    }
}

#endif /* !NW4R_LYT_TEXTBOX_HPP_ */
