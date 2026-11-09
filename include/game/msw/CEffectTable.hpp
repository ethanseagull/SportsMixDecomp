#ifndef CEFFECTTABLE_HPP_
#define CEFFECTTABLE_HPP_

namespace msw {

    // Names inside an effect archive (DOL data).
    extern const char *g_EffectTableFileName;
    extern const char *g_EffectAttachTableFileName;
    extern const char *g_EffectCommonDataArchiveRoot;

    // Base of the tables parsed from an effect archive. Two halfwords, then
    // the vptr at 0x04. Member names are guesses.
    class CTableBase {
        public:
            unsigned short m_Unk00; // 0x00
            unsigned short m_Unk02; // 0x02

            virtual ~CTableBase();
    };

    // Size 0x24 (what the callers allocate).
    class CEffectTable : public CTableBase {
        public:
            CEffectTable();
            virtual ~CEffectTable();
            void Parse(unsigned char *pData);

        private:
            unsigned char m_Unk08[0x1C];
    };

    // Size 0x1C (what the callers allocate).
    class CEffectAttachTable : public CTableBase {
        public:
            CEffectAttachTable();
            virtual ~CEffectAttachTable();
            void Parse(unsigned char *pData);

        private:
            unsigned char m_Unk08[0x14];
    };

    // Size 0x18 (what the callers allocate).
    class CEffectTableList {
        public:
            CEffectTableList();
            void Add(CTableBase *pTable);

        private:
            unsigned char m_Unk00[0x18];
    };
}

#endif /* !CEFFECTTABLE_HPP_ */
