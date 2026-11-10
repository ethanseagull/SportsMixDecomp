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
                // 0xC bytes: one entry of the function table at 0x14.
                struct FUNCTION_TAG {
                    unsigned char m_Unk00[2];
                    signed char nArgNum;  // 0x02, (meaning guessed) copied to CVMCpu 0x20
                    signed char nGroup;   // 0x03, matched against FindFunctionTag's last argument unless -1
                    long nName;           // 0x04, string offset of the name, or its hash
                    long nOffset;         // 0x08, code offset of the entry point; -1: none
                };

                void Init();
                // The tag of function `pName` in group nGroup (-1: any);
                // null when there is none.
                FUNCTION_TAG *FindFunctionTag(const char *pName, long nGroup);
                FUNCTION_TAG *FindFunctionTag(int nName, long nGroup);

                unsigned char *GetCode() const { return m_pCode; }

            private:
                unsigned long m_Unk00;    // used by Load and BindData
                void *m_pHeader;          // 0x04
                unsigned char *m_pCode;   // 0x08
                unsigned long m_Unk0C;
                const char *m_pString;    // 0x10
                FUNCTION_TAG *m_pFunction; // 0x14
                unsigned long m_Unk18;
                void *m_pUnk1C;           // 0x1C, freed with delete[] by Init
                unsigned long m_Unk20;
                long m_nUnk24;             // 0x24: FindFunctionTag hashes the name when it is above 0x10
        };
    }
}

#endif /* !MSW_VM_CVMDATA_HPP_ */
