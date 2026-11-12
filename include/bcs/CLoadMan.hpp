#ifndef BCS_CLOADMAN_HPP_
#define BCS_CLOADMAN_HPP_

namespace bcs {

    // Only the members used so far. All are static: their bodies in the DOL
    // read globals and take no object. Return types the callers ignore are
    // guessed as void.
    class CLoadMan {
        public:
            // (added for TourF game/tour/CTournamentStep.cpp; static)
            static void ClearDvdReadSize();
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
            // (name guessed; the DOL's fn_8004E3C8, added for
            // game/msw/CNowLoading.cpp) Non-zero while the sbss word at
            // lbl_805C08C4 is zero.
            static int IsIdle();
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
            // (added for MenuF game/menu/CSelectMenu.cpp; return types
            // guessed) Freeing a FILE_INPUT, and whether file pPath (id
            // nId) has loaded or is still being read.
            static void UnLoad(const FILE_INPUT *pFile);
            static int IsLoaded(const char *pPath, unsigned long nId);
            static int IsReading(const char *pPath, unsigned long nId);
            static void *GetRes(const FILE_INPUT *pFile);
            // (added for DOL game/bcs/CMiiMan.cpp; names and argument
            // meanings guessed) DOL fn_8004C0A8 builds a FILE_INPUT from its
            // arguments (path, m_nId, m_nFlags, 0x106 = 4, 0x108, 0x10A,
            // 0x10C) and starts loading it; bcs::CMiiMan passes 0, 0, 0x101,
            // -0x20, 2. fn_8004D4D0 is non-zero once the file has loaded
            // (its entry's state is 2); fn_8004D130 returns the word at 0x118
            // of the file's entry (perhaps the size).
            static void Load(const char *pPath, unsigned long nId, unsigned short nFlags, short nUnk108,
                             short nAlign, short nUnk10C);
            static int IsLoadEnd(const char *pPath, unsigned long nId);
            // (added for DOL game/msw/CGameVib.cpp; names guessed from the
            // char-path versions just before them) fn_8004CD30: data of a
            // loaded FILE_INPUT; fn_8004D890: non-zero once all nNum files
            // from pFile on have loaded.
            static void *GetPtr(const FILE_INPUT *pFile);
            static int IsLoadEnd(const FILE_INPUT *pFile, long nNum);
            static unsigned long GetSize(const char *pPath, unsigned long nId);
    };
}

#endif /* !BCS_CLOADMAN_HPP_ */
