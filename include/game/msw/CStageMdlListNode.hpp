#ifndef MSW_CSTAGEMDLLISTNODE_HPP_
#define MSW_CSTAGEMDLLISTNODE_HPP_

namespace bcs {
    class CScnObj;
}

namespace msw {

    // (member names guessed) A node of a stage's model list, handed to
    // CStageMan::OnPostAppendNode. Only what BasketF's stages read and
    // write is mapped; the size is not known.
    class CStageMdlListNode {
        public:
            // Bits of m_nFlag that BasketF's CStageBasket01 sets on some
            // models (meaning not known).
            enum {
                FLAG_4 = 0x4,
                FLAG_8 = 0x8
            };

            bcs::CScnObj *GetObj() const { return m_pObj; }
            void OnFlag(unsigned long flag) { m_nFlag |= flag; }

        private:
            unsigned char m_Unk00[0x10]; // 0x00
            bcs::CScnObj *m_pObj;        // 0x10, the model's scene object
            unsigned char m_Unk14[0x8];  // 0x14
            unsigned long m_nFlag;       // 0x1C
    };
}

#endif /* !MSW_CSTAGEMDLLISTNODE_HPP_ */
