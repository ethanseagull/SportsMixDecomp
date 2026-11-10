#ifndef SELECTMENU_CCOURTTABLESETOBJ_HPP_
#define SELECTMENU_CCOURTTABLESETOBJ_HPP_

#include "game/msw/CTableSetMdlObj.hpp"

namespace bcs {
    class CMdlObj;
}

namespace msw {
    namespace SelectMenu {

        // The court model driven by the court selection menu. Size 0x2C,
        // same layout as its base.
        class CCourtTableSetObj : public CTableSetMdlObj {
            public:
                CCourtTableSetObj(bcs::CMdlObj *pMdl);
                virtual ~CCourtTableSetObj();
        };
    }
}

#endif /* !SELECTMENU_CCOURTTABLESETOBJ_HPP_ */
