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
            //
            // (MenuF game/menu/CRuleInfo.cpp) The path is 0x100 bytes: that
            // unit clears only those with memset, and its initialised table
            // (msw::SelectMenu::CRuleHelpTex::g_sFileInput) holds 0x000F0010
            // at 0x100, then 1, 3, 1 and -32 (an alignment?) as shorts.
            struct FILE_INPUT {
                char m_szPath[0x100];      // 0x000
                unsigned long m_nId;       // 0x100 (meaning guessed)
                // 0x104: bit 0 set skips the file (CGameSnd::SetMusicFiles
                // sets it on the music it does not need; meaning guessed).
                // CRuleHelpTex clears it and sets bit 1 before loading.
                unsigned short m_nFlags;
                short m_Unk106;            // 0x106
                short m_Unk108;            // 0x108
                short m_nAlign;            // 0x10A (meaning guessed)
                unsigned long m_Unk10C;    // 0x10C
            };

            static void SetRawDataBufferMem(long);
            static int IsComplete(); // return type guessed
            // Data of a loaded file. What the second argument selects is
            // not known yet.
            static void *GetPtr(const char *pPath, unsigned long);
            static void UnLoad(const char *pPath, unsigned long);
            // (added for MenuF game/menu/CRuleInfo.cpp; return types and
            // the meaning of the longs guessed) Loading a FILE_INPUT in the
            // background, freeing it, and its data once loaded (null
            // before).
            static void LoadAsync(const FILE_INPUT *pFile, long nUnk);
            static void UnLoad(const FILE_INPUT *pFile, long nUnk);
            static void *GetRes(const FILE_INPUT *pFile);
    };
}

#endif /* !BCS_CLOADMAN_HPP_ */
