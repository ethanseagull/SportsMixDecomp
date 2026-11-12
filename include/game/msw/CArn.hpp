#ifndef MSW_CARN_HPP_
#define MSW_CARN_HPP_

namespace bcs {
    struct FVEC3;
}

namespace msw {

    // An archive of the stage's resources (meaning of the name not known).
    // Only what the modules call is declared.
    class CArn {
        public:
            // In the DOL. The name of entry n.
            const char *GetName(long n) const;
            // (name guessed) In the DOL (fn_8014A920). The position of
            // entry n (entries of 0x94 bytes, the position at 0x04).
            void GetPos(long n, bcs::FVEC3 *pPos) const;

            // (added for game/msw/CStgMdl.cpp; names guessed) One entry,
            // 0x94 bytes; only what is read so far.
            struct ENTRY {
                unsigned char m_Unk00[0x30]; // 0x00
                unsigned long nKind;         // 0x30, an index into s_MdlKindTbl
                unsigned long nDrawPriority; // 0x34, the low half is used; 0: none
                unsigned char m_Unk38[0x5C]; // 0x38
            };

            long GetNum() const { return m_nNum; }
            const ENTRY &GetEntry(long n) const { return m_pEntry[n]; }

            // (DOL lbl_8043C740, name guessed) The kind of model of each
            // entry kind; CStgMdl::CGenericCreator treats kind 4 apart.
            static const unsigned short s_MdlKindTbl[];

        private:
            unsigned char m_Unk00[0xC];  // 0x00
            ENTRY *m_pEntry;             // 0x0C
            unsigned char m_Unk10[0x10]; // 0x10
            long m_nNum;                 // 0x20, the number of entries
    };
}

#endif /* !MSW_CARN_HPP_ */
