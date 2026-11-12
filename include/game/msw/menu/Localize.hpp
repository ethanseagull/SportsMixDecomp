#ifndef MENU_LOCALIZE_HPP_
#define MENU_LOCALIZE_HPP_

#include "bcs/CLoadMan.hpp"

// Free functions of msw::menu at 0x80142284-0x80142F04 in the DOL: the
// language-dependent folders and codes, and the paths of the cockpit, menu,
// message and font files built from them. Only GetLocalizeFolderName is
// named in the DOL; the header, the other names and all argument types are
// guesses.

namespace msw {
    namespace menu {
        // The region's language as bcs::CMainApp::GetRegionLanguage gives it
        // (0-9). The names are guessed from the folder codes.
        enum LOCALIZE_LANGUAGE {
            LOCALIZE_JP_JA,
            LOCALIZE_US_EN,
            LOCALIZE_US_FR,
            LOCALIZE_US_SP,
            LOCALIZE_EU_EN,
            LOCALIZE_EU_GE,
            LOCALIZE_EU_FR,
            LOCALIZE_EU_SP,
            LOCALIZE_EU_IT,
            LOCALIZE_EU_DU
        };

        // "lyt/lc10_UsEn" and so on, null for an unknown language.
        const char *GetLocalizeFolderName();
        // (name guessed) The same folder; the other functions here call it
        // directly, GetLocalizeFolderName only forwards to it.
        const char *GetLocalizeLayoutFolder();
        // (name guessed) "UsEn" and so on: the message file of the language.
        const char *GetLocalizeRegionCode();
        // (name guessed) "En" and so on: the font file of the language.
        const char *GetLocalizeLanguageCode();

        // (name guessed) The number of the cockpit variant of the sport
        // whose two-letter code is nMode ('VO', 'HO', 'BA' or another) for
        // the game type nGameType (CGame::GameWork::m_nGameType); -1 if
        // there is none.
        long GetCockpitVariation(unsigned long nMode, long nGameType);

        // (names guessed) Fill in the paths of the files nStart to nEnd of
        // pFiles that belong to the localised layout (id 0x0020xxxx), and
        // set bit 0 of the flags of those that are not needed.
        // SetCockpitFilePath is for a sport's game, pMode its two-letter
        // code; SetTourCockpitFilePath for the tournament ("TO").
        void SetCockpitFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd,
                                const char *pMode);
        void SetMainFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd);
        void SetLayoutFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd);
        void SetTourCockpitFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd);
        void SetSelectFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd);
        void SetCommonFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd);
        // (names guessed) The message files (id 0x0060xxxx) and one font
        // file; bLoad 1 marks the file to be loaded (flag bit 1).
        void SetMessageFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd,
                                long bLoad);
        void SetFontFilePath(bcs::CLoadMan::FILE_INPUT *pFile, const char *pName, long bLoad);
    }
}

#endif /* !MENU_LOCALIZE_HPP_ */
