#ifndef MSW_CSTAGEMDL_HPP_
#define MSW_CSTAGEMDL_HPP_

#include "bcs/CScnObj.hpp"

namespace msw {

    // A model of the stage, as held in CStageMan's model pool. Only the
    // scene object at 0x04 is mapped (BasketF CStageBasket13::UpdateMdl
    // turns it about Y and rebuilds its matrix).
    class CStageMdl {
        public:
            // (name guessed)
            bcs::CScnObj *GetScnObj() const { return m_pScnObj; }

        private:
            unsigned char m_Unk00[0x4]; // 0x00
            bcs::CScnObj *m_pScnObj;    // 0x04
    };
}

#endif /* !MSW_CSTAGEMDL_HPP_ */
