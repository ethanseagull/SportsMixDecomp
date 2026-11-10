#ifndef MSW_CSTAGENET_HPP_
#define MSW_CSTAGENET_HPP_

#include "game/msw/CStgGmk.hpp"

namespace msw {

    // The net of a stage, a gimmick. A DOL class, size 0x68 (the size of
    // CStageNetVolley without its one member), the same as msw::CStgGmk:
    // it adds no members of its own.
    class CStageNet : public CStgGmk {
        public:
            CStageNet(bcs::CTask *pParent, unsigned long);

            // (DOL; CStgObj leaves it pure)
            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
    };
}

#endif /* !MSW_CSTAGENET_HPP_ */
