#ifndef MSW_CSTAGENET_HPP_
#define MSW_CSTAGENET_HPP_

#include "game/msw/CStgGmk.hpp"

namespace msw {

    // The net of a stage, a gimmick. A DOL class, size 0x68 (the size of
    // CStageNetVolley without its one member). How the members from 0x34
    // divide among CStgObj, CStgGmk and this class is not mapped.
    class CStageNet : public CStgGmk {
        public:
            CStageNet(bcs::CTask *pParent, unsigned long);

        private:
            unsigned char m_Unk34[0x34]; // 0x34
    };
}

#endif /* !MSW_CSTAGENET_HPP_ */
