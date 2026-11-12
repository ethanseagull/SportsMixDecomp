#ifndef BCS_G2D_CLINESTRIPDL_HPP_
#define BCS_G2D_CLINESTRIPDL_HPP_

#include "bcs/G2D/CPaneDL.hpp"
#include "nw4r/math/types.h"
#include "nw4r/ut/Color.hpp"

namespace bcs {
    namespace G2D {

        // A pane that draws a strip of lines through a vertex table (name
        // from the symbols). Lives in the DOL. Size 0x180 (what MenuF
        // game/menu/CFriendInfo.cpp allocates for one through
        // nw4r::lyt::Layout::NewObj). Only the members used so far; names
        // are guesses. Added for that unit. Built on bcs::G2D::CPaneDL
        // (DOL game/bcs/G2D/CLineStripDL.cpp, which also holds CScissorDL).
        class CLineStripDL : public CPaneDL {
            public:
                // Size 0xC. One vertex. Its constructor and destructor are
                // implicit (emitted weak by the module units that hold
                // arrays of them): the colour starts white.
                struct VERTEX_INFO {
                    nw4r::math::VEC2 pos;  // 0x00
                    nw4r::ut::Color color; // 0x08
                };

                CLineStripDL();
                virtual ~CLineStripDL();

                // Draws nNum vertices of pInfo; nMode is the line width (and
                // point size) MakeDL sets, 2 and 3 are passed. The table is
                // not copied.
                void SetVertexInfo(const VERTEX_INFO *pInfo, unsigned long nNum, unsigned long nMode);

                // Writes a line strip through the vertices, each in its
                // colour times the pane's global alpha (or all in the first
                // vertex's colour while m_Unk17C is set).
                virtual void MakeDL();

            private:
                const VERTEX_INFO *m_pVertexInfo; // 0x170
                unsigned long m_nVertexNum;       // 0x174
                unsigned long m_nMode;            // 0x178

            public:
                // 0x17C. Cleared by CFriendInfo after each SetVertexInfo;
                // set (the constructor's default), every vertex takes the
                // first one's colour. (name kept)
                unsigned char m_Unk17C;

            private:
                unsigned char m_Unk17D[0x3];
        };
    }
}

#endif /* !BCS_G2D_CLINESTRIPDL_HPP_ */
