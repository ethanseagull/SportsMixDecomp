#ifndef BCS_CLOADMAN_HPP_
#define BCS_CLOADMAN_HPP_

namespace bcs {

    // Only the members used so far. All are static: their bodies in the DOL
    // read globals and take no object. Return types the callers ignore are
    // guessed as void.
    class CLoadMan {
        public:
            // What a file is loaded from: msw::CLoadData keeps a table of
            // these, 0x110 bytes each, starting with the path. Mapped as far
            // as msw::CGameSnd::SetMusicFiles uses it.
            struct FILE_INPUT {
                char m_szPath[0x104];      // 0x000
                // 0x104: bit 0 set skips the file (CGameSnd::SetMusicFiles
                // sets it on the music it does not need; meaning guessed)
                unsigned short m_nFlags;
                unsigned char m_Unk106[0x110 - 0x106];
            };

            static void SetRawDataBufferMem(long);
            static int IsComplete(); // return type guessed
            // Data of a loaded file. What the second argument selects is
            // not known yet.
            static void *GetPtr(const char *pPath, unsigned long);
            static void UnLoad(const char *pPath, unsigned long);
    };
}

#endif /* !BCS_CLOADMAN_HPP_ */
