#ifndef MSW_CSTGGMK_HPP_
#define MSW_CSTGGMK_HPP_

#include "game/msw/CStgObj.hpp"

namespace msw {

    // A stage gimmick. A DOL class. (meaning of the constructor's second
    // argument not known: CStageNetVolley passes 7)
    class CStgGmk : public CStgObj {
        public:
            CStgGmk(bcs::CTask *pParent, unsigned long);
    };
}

#endif /* !MSW_CSTGGMK_HPP_ */
