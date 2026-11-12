#ifndef BCS_CRSOMODULE_HPP_
#define BCS_CRSOMODULE_HPP_

#include "bcs/CLoadMan.hpp"
#include "revolution/rso.h"

namespace bcs {

    // One of the game's RSO modules (Module/Final/<name>F.cmp). A DOL class,
    // 0x134 bytes; msw's sinit functions build one static instance per
    // module (constructor fn_800786E8). The vptr is at 0x00. Layout from the
    // constructor, Mount and UnMount; member names are guesses.
    class CRsoModule {
        public:
            // A function the main program takes from the module: its export
            // name, and the pointer that receives its address.
            struct IMPORT {
                const char *m_pszName;  // 0x00
                void (**m_ppFunc)();    // 0x04
            };

            // pFileName goes into the path; nUnk130 is kept but not read
            // here (meaning not known; the sinits pass 2).
            CRsoModule(const char *pName, const char *pFileName, IMPORT *pImport, long nImport,
                       long nUnk130);
            virtual ~CRsoModule();

            void LoadAsync();
            int Mount(); // 1 when the module is mounted, 0 on failure; type guessed
            void UnMount();

            // (names guessed; DOL fn_80078680 and fn_8007868C) Loading and
            // preparing the static module Module/Final/MSW.sel, which every
            // module links against. MountStatic returns 1 once it is ready.
            static void LoadStaticAsync();
            static int MountStatic();

        private:
            // Points every import at a function that does nothing.
            void ResetImports();

            int m_bMounted;                // 0x004 (not set by the constructor)
            const char *m_pszName;         // 0x008
            IMPORT *m_pImport;             // 0x00C
            long m_nImport;                // 0x010
            RSOObjectHeader *m_pRso;       // 0x014
            unsigned char *m_pBss;         // 0x018
            unsigned long m_nSize;         // 0x01C, size of the loaded file
            CLoadMan::FILE_INPUT m_File;   // 0x020
            long m_nUnk130;                // 0x130

            static CLoadMan::FILE_INPUT s_StaticFile;
            static void *s_pStaticRso;
    };
}

#endif /* !BCS_CRSOMODULE_HPP_ */
