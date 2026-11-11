#ifndef MSW_CSTAGEMDLLISTNODE_HPP_
#define MSW_CSTAGEMDLLISTNODE_HPP_

#include "bcs/CListNodeBase.hpp"

namespace bcs {
    class CScnObj;
}

namespace msw {

    // (member names guessed) A node of a stage's model list, handed to
    // CStageMan::OnPostAppendNode. Only what BasketF's stages read and
    // write is mapped; the size is not known.
    class CStageMdlListNode : public bcs::CListNodeBase {
        public:
            // Bits of m_nFlag that BasketF's CStageBasket01 sets on some
            // models (meaning not known).
            enum {
                FLAG_4 = 0x4,
                FLAG_8 = 0x8
            };

            bcs::CScnObj *GetObj() const { return m_pObj; }
            void OnFlag(unsigned long flag) { m_nFlag |= flag; }
            unsigned long GetFlag() const { return m_nFlag; }

        private:
            // 0x00: the list links (bcs::CListNodeBase)
            bcs::CScnObj *m_pObj;        // 0x10, the model's scene object
            unsigned char m_Unk14[0x8];  // 0x14
            unsigned long m_nFlag;       // 0x1C
    };
}

namespace msw {

    // (name guessed) What a CStageMdl holds at 0x14: the list of its
    // CStageMdlListNode at 0x30 (HockeyF CStageHockey01::SwitchAudienceMdl_
    // and the DOL's CStageMan::SetDispByTag walk it). Nothing else mapped.
    class CStageMdlNodeList {
        public:
            bcs::CListBase &GetList() { return m_List; }

        private:
            unsigned char m_Unk00[0x30]; // 0x00
            bcs::CListBase m_List;       // 0x30
    };
}

#endif /* !MSW_CSTAGEMDLLISTNODE_HPP_ */
