// bcs::G2D::CLineStripDL and bcs::G2D::CScissorDL: the two display-list
// panes built on bcs::G2D::CPaneDL. One file in the DOL: the two
// destructor thunks of the second base close its text, and the two
// vtables (CScissorDL first) are its data.

#include "bcs/G2D/CLineStripDL.hpp"
#include "bcs/G2D/CScissorDL.hpp"
#include "bcs/G2D/GDUtil.hpp"
#include "bcs/CMainApp.hpp"
#include "nw4r/lyt/common.h"
#include "revolution/gd.h"
#include "FastCast.hpp"

namespace bcs {
    namespace G2D {

        CLineStripDL::CLineStripDL()
            : m_pVertexInfo(0), m_nVertexNum(0), m_nMode(1), m_Unk17C(1)
        {
        }

        CLineStripDL::~CLineStripDL()
        {
        }

        void CLineStripDL::MakeDL()
        {
            unsigned long nNum = m_nVertexNum;
            if (nNum == 0) {
                return;
            }

            SetGDColorVtxState();
            SetCurrentMtxDL();
            SetGDLineState(m_nMode, m_nMode, GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);

            const VERTEX_INFO *pInfo = m_pVertexInfo;
            unsigned char nAlpha = GetGlobalAlpha();
            nw4r::ut::Color color = nw4r::lyt::detail::MultipleAlpha(pInfo->color, nAlpha);

            GDBegin(GX_LINESTRIP, GX_VTXFMT0, nNum);
            for (unsigned long i = 0; i < nNum; pInfo++, i++) {
                GDPosition2f32(pInfo->pos.x, pInfo->pos.y);
                if (!m_Unk17C) {
                    color = nw4r::lyt::detail::MultipleAlpha(pInfo->color, nAlpha);
                }
                GDColor1u32(color.ToU32());
            }
        }

        void CLineStripDL::SetVertexInfo(const VERTEX_INFO *pInfo, unsigned long nNum, unsigned long nMode)
        {
            m_nVertexNum = nNum;
            if (nNum != 0) {
                m_pVertexInfo = pInfo;
            }
            m_nMode = nMode;
        }

        CScissorDL::CScissorDL()
        {
        }

        CScissorDL::~CScissorDL()
        {
        }

        void CScissorDL::MakeDL()
        {
            const GXRenderModeObj *pRmode = CMainApp::GetRenderModeObj();
            unsigned short nWidth;
            unsigned short nHeight;
            F32ToU16(GetSize().width, &nWidth);
            F32ToU16(GetSize().height, &nHeight);

            if (nWidth >= pRmode->fbWidth && nHeight >= pRmode->efbHeight) {
                SetGDScissor(0, 0, pRmode->fbWidth, pRmode->efbHeight);
                return;
            }

            // The pane's centre in screen space is the origin moved by
            // half the frame buffer; y points up in the layout.
            const nw4r::math::MTX34 &rMtx = GetGlobalMtx();
            int nLeft = rMtx._03 + (pRmode->fbWidth >> 1) - (nWidth >> 1);
            int nTop = -rMtx._13 + (pRmode->efbHeight >> 1) - (nHeight >> 1);
            if (nLeft + nWidth >= pRmode->fbWidth) {
                nWidth = pRmode->fbWidth - nLeft;
            }
            if (nTop + nHeight >= pRmode->efbHeight) {
                nHeight = pRmode->efbHeight - nTop;
            }
            SetGDScissor(nLeft, nTop, nWidth, nHeight);
        }
    }
}
