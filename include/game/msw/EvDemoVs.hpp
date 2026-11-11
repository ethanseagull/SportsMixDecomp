#ifndef EVDEMOVS_HPP_
#define EVDEMOVS_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    // Whether "EvDemoVs" is a namespace or a class with static members
    // cannot be told from the symbol; a namespace is a guess.
    namespace EvDemoVs {
        // One loaded resource of the versus demo (16 bytes; 19 of them at
        // 0x804D8850). Field meanings from fn_8021FB28 (stores the flag, the
        // buffer and the size), CopyRes (the path) and SetChrData.
        struct RES_DATA {
            signed char m_bResFile; // 0x00: the buffer holds a g3d ResFile
            signed char m_nChrPos;  // 0x01: index into s_ChrPos, -1 if none
            short m_nFileID;        // 0x02: msw::CLoadData id, -1 if none
            const char *m_pszPath;  // 0x04
            unsigned char *m_pData; // 0x08: new[]-allocated copy
            unsigned long m_nSize;  // 0x0C
        };

        enum {
            RES_DATA_NUM = 19,
            RES_DATA_CHR = 5 // first entry SetChrData addresses
        };

        // Storage outside this unit's slice (.bss 0x804D8850 / 0x804D87F0,
        // .sbss 0x805C0DAC); names are ours.
        extern RES_DATA s_ResData[RES_DATA_NUM];
        extern bcs::FVEC3 s_ChrPos[8];
        extern long s_nCopyResMode;

        void Initialize();
        // fn_8021FFCC: frees every buffer, then does what Initialize does.
        // Name guessed (called from the tournament reset in main.dol).
        void Finalize();
        // Added for MenuF game/menu/CMenu.cpp (meanings guessed from the
        // names: the menu passes 0 on start-up, 3 before leaving for a step
        // other than 3).
        void CopyResMode(long nMode);
        void CopyRes();
        void SetChrData(long nChr, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rDir);
    }
}

#endif /* !EVDEMOVS_HPP_ */
