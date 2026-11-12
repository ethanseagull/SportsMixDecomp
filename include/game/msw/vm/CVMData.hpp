#ifndef MSW_VM_CVMDATA_HPP_
#define MSW_VM_CVMDATA_HPP_

namespace msw {
    namespace vm {

        // One word of the VM: an integer, a float or four packed bytes (the
        // stack, the script's global values and its static data). The empty
        // constructor is why `new STACK_DATA[n]` keeps an array cookie
        // (TourF CTmScript's constructor and destructor, CVMData::Setup);
        // it is declared outside the classes because nested in one the
        // compiler passes the constructor to __construct_new_array, which
        // the original does not. (name guessed)
        union STACK_DATA {
            STACK_DATA() {}

            long i;
            float f;
            unsigned char b[4];
        };

        class CVMCpu;

        // A loaded script image of the event VM (DOL). (member names
        // guessed)
        //
        // The image ("SBD" and a version letter) is a 0x20-byte HEADER, then
        // the static data (strings and constants the code addresses), the
        // code, the function tags, the value tags and the name strings. The
        // name strings start with two hash multipliers (versions F and G;
        // zero means the tags hold string offsets). Versions E and F have
        // the static data scrambled; Setup unscrambles it once and marks the
        // image D or G.
        class CVMData {
            public:
                // Inline: the TourF copies are weak functions.
                CVMData() : m_pImage(0), m_pValue(0), m_pStatic(0) { Init(); }
                // Binds the script image pImage (DOL CVMScript::SetScript
                // inlines it).
                CVMData(void *pImage) : m_pImage(static_cast<unsigned char *>(pImage)), m_pValue(0), m_pStatic(0)
                {
                    Setup();
                }
                ~CVMData() { Init(); }

                // (return types guessed) Loads script file nFile, and binds
                // data file nFile to the script loaded.
                void Load(long nFile);
                // DOL 0x801F7034: whether file nFile has finished loading.
                // (name and return type guessed)
                int IsLoaded(long nFile);
                // DOL 0x801F703C: empty. (name and signature guessed)
                void Release();
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
                    unsigned char aMagic[4]; // 0x00: "SBD" and the version letter
                    long nCodeSize;       // 0x04
                    long nStaticSize;     // 0x08: bytes of static data after the header
                    long nUnk0C;          // 0x0C
                    long nValueTagNum;    // 0x10
                    long nFunctionNum;    // 0x14
                    long nSysCallNum;     // 0x18: entries the system call table needs
                    long nValueNum;       // 0x1C: words of global values
                };

                // One named value of the script: its words start at
                // m_pValue[nIndex]. Same layout as FUNCTION_TAG up to 0x08.
                // (names guessed)
                struct VALUE_TAG {
                    short nUnk00;         // 0x00
                    signed char nUnk02;   // 0x02
                    signed char nGroup;   // 0x03, matched against FindValueTag's last argument unless -1
                    long nName;           // 0x04, string offset of the name, or its hash
                    long nIndex;          // 0x08
                };

                void Init();
                // DOL 0x801F70E0: checks the "SBD" image at m_pImage and sets
                // up the pointers into it (BindData calls it after loading).
                // (name guessed)
                void Setup();
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
                long &Value(long nIndex) { return m_pValue[nIndex].i; }
                // Word nIndex of the static data. (name guessed)
                long &Static(long nIndex) { return m_pStatic[nIndex].i; }

            private:
                // The opcode handlers read the tables directly.
                friend class CVMCpu;

                unsigned char *m_pImage;  // 0x00: used by Load and BindData
                const HEADER *m_pHeader;  // 0x04
                unsigned char *m_pCode;   // 0x08
                unsigned char *m_pStaticData; // 0x0C: just after the header
                const char *m_pString;    // 0x10
                FUNCTION_TAG *m_pFunction; // 0x14
                VALUE_TAG *m_pValueTag;   // 0x18
                STACK_DATA *m_pValue;     // 0x1C, freed with delete[] by Init
                STACK_DATA *m_pStatic;    // 0x20: m_pString + 8 (after the two multipliers)
                long m_nFunctionHash;     // 0x24: FindFunctionTag hashes the name when it is above 0x10
                long m_nValueHash;        // 0x28: the same for FindValueTag (size 0x2C: msw::CTournamentMan allocates an array of them)
        };
    }
}

#endif /* !MSW_VM_CVMDATA_HPP_ */
