// msw::SelectMenu::CCharaName: the character name plates of the character
// select screen.
//
// Only SetRewriteTextureData is left in the module; the link stripped the
// rest of the file and kept its data (the pane name table and the colour
// below). Names of the file-scope objects are guesses.
#include "game/msw/SelectMenu/CCharaName.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/G2D/CLayoutDL.hpp"
#include "bcs/New.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "revolution/gx/GXTexture.h"

namespace {

    // The panes of the name plate layout.
    enum {
        PANE_T_P_01,
        PANE_T_P_02,
        PANE_T_P_03,
        PANE_T_P_04,
        PANE_T_P_05,
        PANE_T_P_06,
        PANE_T_P_01_01,
        PANE_T_P_02_01,
        PANE_T_P_03_01,
        PANE_T_P_04_01,
        PANE_T_P_05_01,
        PANE_T_P_06_01,
        PANE_WINDOW_00,
        PANE_NUM
    };

    const char *s_PaneName[PANE_NUM + 1] = {
        "t_p_01",    "t_p_02",    "t_p_03",    "t_p_04",    "t_p_05",    "t_p_06",
        "t_p_01_01", "t_p_02_01", "t_p_03_01", "t_p_04_01", "t_p_05_01", "t_p_06_01",
        "Window_00", 0,
    };

    // (guessed) A colour of the stripped code, white until it is set: the
    // file's static initialiser stores one opaque white word in .bss.
    bcs::BCS_Color s_Color;
}

namespace msw {
    namespace SelectMenu {

        void CCharaName::SetRewriteTextureData(nw4r::g3d::ResTexObj texObj, _GXTexMapID mapId)
        {
            m_ResTexObj = texObj;
            m_TexMapId = mapId;

            _GXTexObj *pTexObj = texObj.GetTexObj(mapId);
            unsigned short nWidth = GXGetTexObjWidth(pTexObj);
            unsigned short nHeight = GXGetTexObjHeight(pTexObj);

            unsigned long nSize = GXGetTexBufferSize(nWidth, nHeight, GX_TF_RGB565, 0, 0);
            void *pImage = new (1, 0x20) unsigned char[nSize];
            if (pImage == 0) {
                return;
            }

            if (m_pTexImage != 0) {
                delete[] (unsigned char *)m_pTexImage;
            }
            m_pTexImage = pImage;
            m_nTexWidth = nWidth;
            m_nTexHeight = nHeight;
            m_nTexFormat = GX_TF_RGB565;
            m_bTexMipmap = 0;

            // Stretch the layout horizontally so the window pane is as wide
            // as the texture.
            nw4r::lyt::Pane *pRoot = m_pLayout->GetRootPane();
            if (pRoot != 0) {
                nw4r::lyt::Pane *pWindow = pRoot->FindPaneByName(s_PaneName[PANE_WINDOW_00], true);
                if (pWindow != 0) {
                    nw4r::lyt::Size size = pWindow->GetSize();
                    pRoot->SetScale(nw4r::math::VEC2(m_nTexWidth / size.width, 1.0f));
                }
            }
        }
    }
}
