#ifndef CEFFECTTABLE_HPP_
#define CEFFECTTABLE_HPP_

namespace msw {

    // Names inside an effect archive (DOL data).
    extern const char *g_EffectTableFileName;
    extern const char *g_EffectAttachTableFileName;
    extern const char *g_EffectCommonDataArchiveRoot;

    // Base of the tables parsed from an effect archive. Two halfwords, then
    // the vptr at 0x04. Member names are guesses.
    //
    // The constructor and destructor are inline: the DOL constructors of
    // CEffectTable and CEffectAttachTable clear the two halfwords before
    // storing their own vtable, and their destructors call nothing for the
    // base. The four getters read the entry of table index idx (idx counts
    // from m_Unk00, see CEffectTableList::Add); both tables' versions convert
    // four signed halfwords of the entry to float. (names guessed)
    class CTableBase {
        public:
            CTableBase() : m_Unk00(0), m_Unk02(0) {}

            unsigned short m_Unk00; // 0x00: index of the first entry
            unsigned short m_Unk02; // 0x02: number of entries

            // Each file that makes a table emits CTableBase's vtable and this
            // destructor as weak objects; nothing refers to them once the
            // dead vtable store is gone, and the DOL has neither (dead-stripped).
            virtual ~CTableBase() {}
            virtual float GetParam0(long idx) const = 0;
            virtual float GetParam1(long idx) const = 0;
            virtual float GetParam2(long idx) const = 0;
            virtual float GetParam3(long idx) const = 0;
    };

    // Size 0x24 (what the callers allocate), vtable lbl_8046F8AC. The data
    // is a 0x10-byte header, the entries (0x10 bytes each), then
    // zero-terminated names in four groups: the entries' names, a group with
    // one name per entry (use not known), the entries' child names, and a
    // last group (use not known). Parse keeps a pointer to each name. The getters other than the constructor and
    // Parse have no exported name (fn_801246B4 and on); their names are
    // guesses from CEffectCtrl, which calls them.
    class CEffectTable : public CTableBase {
        public:
            // The header of the data (names guessed).
            struct HEADER {
                unsigned char m_Unk00[4];
                unsigned short nNameNum;      // 0x04: entry names
                unsigned short nEntryNum;     // 0x06: entries, and names of the second group
                unsigned short nChildNameNum; // 0x08: child names
                unsigned short m_Unk0A;
                unsigned short nGroup3Num;    // 0x0C: names of the last group
                unsigned short m_Unk0E;
            };

            // One entry (names guessed).
            struct ENTRY {
                unsigned short nChildNum;     // 0x00
                unsigned short nChildTop;     // 0x02: its first child name
                unsigned short nName;         // 0x04: its name
                short aParam[4];              // 0x06: GetParam0 to GetParam3
                unsigned short m_Unk0E;
            };

            CEffectTable();
            virtual ~CEffectTable();
            // 0 when parsed; -1 without data, -2 when the name table could
            // not be allocated.
            long Parse(unsigned char *pData);

            // (fn_801246B4) The name of entry idx; null without a table.
            const char *GetName(long idx) const;
            // (fn_801246FC) The number of children of entry idx.
            unsigned short GetChildNum(long idx) const;
            // (fn_80124724) The name of child nChild of entry idx.
            const char *GetChildName(long idx, long nChild) const;

            virtual float GetParam0(long idx) const;
            virtual float GetParam1(long idx) const;
            virtual float GetParam2(long idx) const;
            virtual float GetParam3(long idx) const;

        private:
            const ENTRY &GetEntry(long idx) const { return m_pEntry[idx - m_Unk00]; }
            unsigned short GetEntryNum() const
            {
                if (m_pEntry == 0) {
                    return 0;
                }
                return m_pHeader->nEntryNum;
            }

            const HEADER *m_pHeader;  // 0x08: the data passed to Parse
            const ENTRY *m_pEntry;    // 0x0C
            const char **m_ppName;    // 0x10: every name, allocated by Parse
            long m_nChildNameTop;     // 0x14: index of the first child name
            long m_nNameTop;          // 0x18: index of the first entry name (0)
            long m_nGroup1Top;        // 0x1C: index of the second group
            long m_nGroup3Top;        // 0x20: index of the last group
    };

    // Size 0x1C (what the callers allocate), vtable lbl_8046F890. The data
    // is a 0x10-byte header (CEffectTable's), the entries (0xC bytes each),
    // then zero-terminated names in two groups: the entries' names and one
    // name per entry that GetAttachName appends. Getter names are guesses.
    class CEffectAttachTable : public CTableBase {
        public:
            // One entry (names guessed).
            struct ENTRY {
                unsigned short nName;         // 0x00: its name
                unsigned short m_Unk02;
                short aParam[4];              // 0x04: GetParam0 to GetParam3
            };

            CEffectAttachTable();
            virtual ~CEffectAttachTable();
            // 0 when parsed; -1 without data, -2 when the name table could
            // not be allocated.
            long Parse(unsigned char *pData);

            // (fn_801249A4) The name of entry idx; null without a table.
            const char *GetName(long idx) const;
            // (fn_801249E8) "<name>_<second name>" of entry idx, in a static
            // buffer; null without a table or name.
            const char *GetAttachName(long idx) const;

            // DOL fn_80124AAC to fn_80124B00 (no exported names).
            virtual float GetParam0(long idx) const;
            virtual float GetParam1(long idx) const;
            virtual float GetParam2(long idx) const;
            virtual float GetParam3(long idx) const;

        private:
            const ENTRY &GetEntry(long idx) const { return m_pEntry[idx - m_Unk00]; }
            unsigned short GetEntryNum() const
            {
                if (m_pEntry == 0) {
                    return 0;
                }
                return m_pHeader->nEntryNum;
            }

            const CEffectTable::HEADER *m_pHeader; // 0x08: the data passed to Parse
            const ENTRY *m_pEntry;    // 0x0C
            const char **m_ppName;    // 0x10: every name, allocated by Parse
            long m_nNameTop;          // 0x14: index of the first entry name (0)
            long m_nSubNameTop;       // 0x18: index of the second group
    };

    // Size 0x18 (what the callers allocate): up to four tables, their
    // count, then the vptr at 0x14 (the vtable has only the destructor).
    // Member names are guesses.
    class CEffectTableList {
        public:
            CEffectTableList();
            // Appends pTable and sets its first index after the previous
            // table's range. -1 for a null table, -2 when full.
            long Add(CTableBase *pTable);

        private:
            CTableBase *m_apTable[4]; // 0x00
            long m_nNum;              // 0x10

        public:
            virtual ~CEffectTableList();
    };
}

#endif /* !CEFFECTTABLE_HPP_ */
