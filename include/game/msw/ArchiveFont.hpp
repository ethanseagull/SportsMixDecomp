#ifndef ARCHIVEFONT_HPP_
#define ARCHIVEFONT_HPP_

namespace nw4r {
    namespace ut {
        class Font;
    }
}

namespace msw {

    // Whether "ArchiveFont" is a namespace or a class with static members
    // cannot be told from the symbols; a namespace is a guess.
    namespace ArchiveFont {
        void Initialize();
        void AttachResource();
        void Dispose();
        // (names guessed; the DOL's fn_80211500 and fn_8021153C, added for
        // game/msw/CNowLoading.cpp) The resource name of font nFont (a
        // four-entry table) and the font itself, null when not loaded.
        const char *GetFontName(long nFont);
        nw4r::ut::Font *GetFont(long nFont);
        // (names guessed; the DOL's fn_802114F8, fn_80211514 and
        // fn_80211528, added for game/msw/ArchiveFont.cpp) The number of
        // fonts (4), the glyph list font nFont is built with, and the
        // bcs::G2D::CFontMan slot it goes in.
        long GetFontNum();
        const char *GetGlyphList(long nFont);
        long GetFontSlot(long nFont);
    }
}

#endif /* !ARCHIVEFONT_HPP_ */
