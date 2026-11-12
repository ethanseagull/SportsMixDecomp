#include "game/msw/ArchiveFont.hpp"
#include "bcs/CLoadMan.hpp"
#include "bcs/New.hpp"
#include "bcs/G2D/CFontMan.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CLoadData.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"
#include "nw4r/ut/Font.hpp"

// msw::ArchiveFont: the game's four archive fonts, read from the font
// archive (CLoadData entry 0x57) into bcs::G2D::CFontMan.

namespace msw {
    namespace ArchiveFont {

        enum {
            FONT_NUM = 4,
            FILE_FONT = 0x57 // CLoadData entry of the font archive
        };

        static const char *const s_pRootDir = "arc";

        static const char *const s_FontName[FONT_NUM] = {
            "MARIO_FONT.brfna",
            "MF_EB_01.brfna",
            "MF_EB_02.brfna",
            "MF_EB_03.brfna",
        };

        static const long s_FontSlot[FONT_NUM] = { 0, 1, 2, 3 };

        static const char *s_GlyphList[FONT_NUM] = {
            bcs::G2D::GLYPH_LIST_WII_NAME,
            bcs::G2D::GLYPH_LIST_WII_NAME,
            bcs::G2D::GLYPH_LIST_ALL,
            bcs::G2D::GLYPH_LIST_WII_NAME,
        };

        void Initialize()
        {
            bcs::G2D::CFontMan::CreateInstance();
        }

        // Builds the fonts from the font archive, then frees the archive.
        void AttachResource()
        {
            bcs::G2D::CFontMan *pFontMan = bcs::G2D::CFontMan::GetInstance();
            CLoadData::GetFilePath(FILE_FONT);
            void *pArchive = CLoadData::GetPtr(FILE_FONT);
            if (pArchive == 0) {
                return;
            }
            nw4r::lyt::ArcResourceAccessor *pAccessor = new (0) nw4r::lyt::ArcResourceAccessor;
            if (pAccessor) {
                if (pArchive && pAccessor->Attach(pArchive, s_pRootDir)) {
                    for (long i = 0; i < FONT_NUM; i++) {
                        pFontMan->SetFont(s_FontSlot[i],
                                          bcs::G2D::CreateArchiveFont(
                                              pAccessor->GetResource('fnta', s_FontName[i], 0), s_GlyphList[i]));
                    }
                    pAccessor->Detach();
                }
                delete pAccessor;
            }
            bcs::CLoadMan::UnLoad(CLoadData::GetFilePath(FILE_FONT), CLoadData::GetFileId(FILE_FONT));
        }

        void Dispose()
        {
            bcs::G2D::CFontMan::DestroyInstance();
        }

        long GetFontNum()
        {
            return FONT_NUM;
        }

        const char *GetFontName(long nFont)
        {
            return s_FontName[nFont];
        }

        const char *GetGlyphList(long nFont)
        {
            return s_GlyphList[nFont];
        }

        long GetFontSlot(long nFont)
        {
            return s_FontSlot[nFont];
        }

        nw4r::ut::Font *GetFont(long nFont)
        {
            bcs::G2D::CFontMan *pFontMan = bcs::G2D::CFontMan::GetInstance();
            if (pFontMan == 0) {
                return 0;
            }
            return pFontMan->GetFont(nFont);
        }
    }
}
