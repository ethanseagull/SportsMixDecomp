#ifndef SELECTMENU_CNWLAYOUT_HPP_
#define SELECTMENU_CNWLAYOUT_HPP_

#include "bcs/CGDLObj.hpp"
#include "nw4r/g3d/ResMat.hpp"
#include "nw4r/lyt/Animation.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/lyt/layout.h"
#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXTexture.h"

namespace nw4r {
    namespace lyt {
        class ArcResourceAccessor;
        class FontRefLink;
    }
}

namespace msw {
    namespace SelectMenu {

        // Size 0x168 (layout.py). A scene object that draws an NW4R layout
        // of its own (the select menu's background display, CBgDisplay) in
        // the opaque pass: it clears the copy of the frame buffer it draws
        // into, draws the layout, copies the result into up to two model
        // textures (m_ResTexObj) and restores the frame. Defined in
        // game/menu/CBgDisplay.cpp; member names are guesses.
        class CNwLayout : public bcs::CGDLObj {
            public:
                enum {
                    FONT_LINK_NUM = 4,
                    ANIM_NUM = 6,
                    DRAW_TEX_NUM = 2
                };

                CNwLayout() { Initialize(); }
                virtual ~CNwLayout();
                virtual void ObjProc(unsigned long nTask, unsigned long nParam, void *pInfo);
                virtual void MainLoop();
                virtual void DispLoop();
                // 0 on success, -1 without a layout controller, -2 when the
                // archive cannot be opened.
                virtual long CreateLayout(const char *pLayoutName, const char *pArcName, const char *pRootName);
                virtual void RemoveLayout();

                void Initialize();
                // The length of pText, -1 without a text, -2 when the pane
                // is not a text box.
                long SetTextString(const char *pPaneName, const wchar_t *pText);
                void EFBClear(void *pBuffer, unsigned short nX, unsigned short nY);
                void EFBRestore(void *pBuffer, unsigned short nX, unsigned short nY);
                void EFBCopyToTex(const GXTexObj *pTexObj, unsigned short nX, unsigned short nY,
                                  unsigned short nWidth, unsigned short nHeight, unsigned char bClear);
                void SetRestoreBuffer();
                void GetLayoutDrawTexture();
                void UpdateAnimFrame();

                // The pane called pName, or null. Defined in the .cpp.
                inline nw4r::lyt::Pane *GetPane(const char *pName);

                nw4r::lyt::Layout *GetLayout() const { return m_pLayout; }
                nw4r::lyt::AnimResource &GetAnimResource(long nAnim) { return m_pAnimRes[nAnim]; }
                long GetAnimNum() const { return m_nAnimNum; }

                // Draws only while a layout is loaded (the draw tasks 6 and 7
                // of ObjProc are switched off otherwise).
                void UpdateDrawFlag()
                {
                    if (m_pLayout == 0) {
                        OnFlag(0xC0);
                    } else {
                        OffFlag(0xC0);
                    }
                }

                // The model texture texture nTex of the layout is copied into.
                void SetDrawTexture(long nTex, nw4r::g3d::ResTexObj texObj)
                {
                    m_ResTexObj[nTex] = texObj;
                    m_TexMapId[nTex] = GX_TEXMAP0;
                }

            private:
                nw4r::lyt::Layout *m_pLayout;                       // 0x128
                nw4r::lyt::FontRefLink *m_pFontRefLink[FONT_LINK_NUM]; // 0x12C
                nw4r::lyt::ArcResourceAccessor *m_pResAccessor;     // 0x13C
                nw4r::lyt::AnimResource *m_pAnimRes;                // 0x140, ANIM_NUM of them
                long m_nAnimNum;                                    // 0x144
                void *m_pRestoreBuf;                                // 0x148, copy of the frame
                unsigned short m_nRestoreWidth;                     // 0x14C
                unsigned short m_nRestoreHeight;                    // 0x14E
                GXTexFmt m_RestoreFormat;                           // 0x150
                unsigned char m_bRestoreMipmap;                     // 0x154
                nw4r::g3d::ResTexObj m_ResTexObj[DRAW_TEX_NUM];     // 0x158
                GXTexMapID m_TexMapId[DRAW_TEX_NUM];                // 0x160
        };
    }
}

#endif /* !SELECTMENU_CNWLAYOUT_HPP_ */
