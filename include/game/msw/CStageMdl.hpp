#ifndef MSW_CSTAGEMDL_HPP_
#define MSW_CSTAGEMDL_HPP_

#include "bcs/CScnObj.hpp"

namespace msw {

    class CStageMdlNodeList;

    // A model of the stage, as held in CStageMan's model pool. Only the
    // scene object at 0x04 is mapped (BasketF CStageBasket13::UpdateMdl
    // turns it about Y and rebuilds its matrix).
    class CStageMdl {
        public:
            // (name guessed)
            bcs::CScnObj *GetScnObj() const { return m_pScnObj; }
            // (name guessed) The list of the model's nodes.
            CStageMdlNodeList *GetNodeList() const { return m_pNodeList; }

        private:
            unsigned char m_Unk00[0x4]; // 0x00
            bcs::CScnObj *m_pScnObj;    // 0x04
            unsigned char m_Unk08[0xC]; // 0x08
            CStageMdlNodeList *m_pNodeList; // 0x14 (name guessed)
    };
}

#endif /* !MSW_CSTAGEMDL_HPP_ */
