#ifndef MSW_VM_CVMDATA_HPP_
#define MSW_VM_CVMDATA_HPP_

namespace msw {
    namespace vm {

        // INCOMPLETE: only what game/msw/vm/CVMCpu.cpp needs.
        //
        // A loaded script image of the event VM (DOL). Init clears the words
        // from 0x04 to 0x18 and frees an array at 0x1C. (member names
        // guessed)
        class CVMData {
            public:
                // Inline: the TourF copies are weak functions.
                CVMData() : m_Unk00(0), m_pValue(0), m_Unk20(0) { Init(); }
                ~CVMData() { Init(); }

                // (return types guessed) Loads script file nFile, and binds
                // data file nFile to the script loaded.
                void Load(long nFile);
                void BindData(long nFile);

                // 0xC bytes: one entry of the function table at 0x14.
                struct FUNCTION_TAG {
                    // 0x00: for a system call, its number (TourF
                    // CTmScript::_BindSysCall). (name guessed)
                    short nNo;
                    signed char nArgNum;  // 0x02, (meaning guessed) copied to CVMCpu 0x20
                    signed char nGroup;   // 0x03, matched against FindFunctionTag's last argument unless -1
                    long nName;           // 0x04, string offset of the name, or its hash
                    long nOffset;         // 0x08, code offset of the entry point; -1: none
                };

                // The start of the script image. (names guessed)
                struct HEADER {
                    unsigned char m_Unk00[0x18];
                    long nSysCallNum;     // 0x18: entries the system call table needs
                };

                // One named value of the script: its words start at
                // m_pValue[nIndex]. (names guessed)
                struct VALUE_TAG {
                    unsigned char m_Unk00[8];
                    long nIndex;          // 0x08
                };

                void Init();
                // The tag of function `pName` in group nGroup (-1: any);
                // null when there is none.
                FUNCTION_TAG *FindFunctionTag(const char *pName, long nGroup);
                FUNCTION_TAG *FindFunctionTag(int nName, long nGroup);

                // Value `pName` of group nGroup (TourF passes 1); null when
                // there is none. (meaning of nGroup guessed)
                VALUE_TAG *FindValueTag(const char *pName, long nGroup);

                unsigned char *GetCode() const { return m_pCode; }
                // TourF's CTmScript::UpdateBind calls a weak copy: that
                // function is too large for the compiler to inline into.
                long GetSysCallNum() const { return m_pHeader->nSysCallNum; }
                // Word nIndex of the values (see VALUE_TAG). (name guessed)
                long &Value(long nIndex) { return m_pValue[nIndex]; }

            private:
                unsigned long m_Unk00;    // used by Load and BindData
                const HEADER *m_pHeader;  // 0x04
                unsigned char *m_pCode;   // 0x08
                unsigned long m_Unk0C;
                const char *m_pString;    // 0x10
                FUNCTION_TAG *m_pFunction; // 0x14
                unsigned long m_Unk18;
                long *m_pValue;           // 0x1C, freed with delete[] by Init
                unsigned long m_Unk20;
                long m_nUnk24;             // 0x24: FindFunctionTag hashes the name when it is above 0x10
                unsigned long m_Unk28;     // 0x28 (size 0x2C: msw::CTournamentMan allocates an array of them)
        };
    }
}

#endif /* !MSW_VM_CVMDATA_HPP_ */
