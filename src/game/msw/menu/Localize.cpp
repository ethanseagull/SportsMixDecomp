// Language-dependent file paths for the menus and cockpits (file name
// guessed). All names except GetLocalizeFolderName are guessed.
#include "game/msw/menu/Localize.hpp"
#include "bcs/CLoadMan.hpp"
#include "bcs/CMainApp.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CMainStep.hpp"
#include <stdio.h>

// Another file's table of the sports' two-letter codes ("DO", "VO", ...),
// indexed by CGame::GameWork::m_nGameMode (unnamed in the DOL).
extern "C" const char *lbl_8047F2E8[];

namespace msw {
    namespace menu {

        namespace {
            // A file is one of the localised layout files.
            const unsigned long FILE_LAYOUT = 0x00200000;
            // A file is one of the localised message files.
            const unsigned long FILE_MESSAGE = 0x00600000;

            // Flag bits of FILE_INPUT::m_nFlags.
            const unsigned short FILE_FLAG_SKIP = 0x0001;
            const unsigned short FILE_FLAG_LOAD = 0x0002;

            // The i-th entry of a file array: the same as pFiles[i]. Written
            // as a byte offset only to steer code generation (a guess at the
            // original's form; the two cockpit loops keep base and offset
            // apart this way), not needed for behaviour.
            inline bcs::CLoadMan::FILE_INPUT &FileAt(bcs::CLoadMan::FILE_INPUT *pFiles, long i)
            {
                return *(bcs::CLoadMan::FILE_INPUT *)((char *)pFiles +
                                                      i * sizeof(bcs::CLoadMan::FILE_INPUT));
            }

            // Clears or sets the skip bit of a file.
            inline void SetFileUse(bcs::CLoadMan::FILE_INPUT &file, long bUse)
            {
                if (bUse == 1) {
                    file.m_nFlags &= ~FILE_FLAG_SKIP;
                } else {
                    file.m_nFlags |= FILE_FLAG_SKIP;
                }
            }
        }

        // The general pack's path, in a buffer of its own (0x80 bytes, first
        // in the DOL's .rodata of this file; name guessed).
        const char s_szGeneralPack[0x80] = "menu/bin/General_pack.bin";

        // The language's layout folder (names of the variables guessed).
        const char *s_pLytJpJa = "lyt/lc00_JpJa";
        const char *s_pLytUsEn = "lyt/lc10_UsEn";
        const char *s_pLytUsFr = "lyt/lc11_UsFr";
        const char *s_pLytUsSp = "lyt/lc12_UsSp";
        const char *s_pLytEuEn = "lyt/lc20_EuEn";
        const char *s_pLytEuGe = "lyt/lc21_EuGe";
        const char *s_pLytEuFr = "lyt/lc22_EuFr";
        const char *s_pLytEuIt = "lyt/lc23_EuIt";
        const char *s_pLytEuSp = "lyt/lc24_EuSp";
        const char *s_pLytEuDu = "lyt/lc25_EuDu";

        // The language's message file.
        const char *s_pMesgJpJa = "JpJa";
        const char *s_pMesgUsEn = "UsEn";
        const char *s_pMesgUsFr = "UsFr";
        const char *s_pMesgUsSp = "UsSp";
        const char *s_pMesgEuEn = "EuEn";
        const char *s_pMesgEuGe = "EuGe";
        const char *s_pMesgEuFr = "EuFr";
        const char *s_pMesgEuIt = "EuIt";
        const char *s_pMesgEuSp = "EuSp";
        const char *s_pMesgEuDu = "EuDu";

        // The language's font file.
        const char *s_pFontJa = "Ja";
        const char *s_pFontEn = "En";
        const char *s_pFontGe = "Ge";
        const char *s_pFontFr = "Fr";
        const char *s_pFontIt = "It";
        const char *s_pFontSp = "Sp";
        const char *s_pFontDu = "Du";

        const char *GetLocalizeLayoutFolder()
        {
            const char *pName;
            switch (bcs::CMainApp::GetRegionLanguage()) {
            case LOCALIZE_JP_JA:
                pName = s_pLytJpJa;
                break;
            case LOCALIZE_US_EN:
                pName = s_pLytUsEn;
                break;
            case LOCALIZE_US_FR:
                pName = s_pLytUsFr;
                break;
            case LOCALIZE_US_SP:
                pName = s_pLytUsSp;
                break;
            case LOCALIZE_EU_EN:
                pName = s_pLytEuEn;
                break;
            case LOCALIZE_EU_GE:
                pName = s_pLytEuGe;
                break;
            case LOCALIZE_EU_FR:
                pName = s_pLytEuFr;
                break;
            case LOCALIZE_EU_IT:
                pName = s_pLytEuIt;
                break;
            case LOCALIZE_EU_SP:
                pName = s_pLytEuSp;
                break;
            case LOCALIZE_EU_DU:
                pName = s_pLytEuDu;
                break;
            default:
                pName = 0;
                break;
            }
            return pName;
        }

        const char *GetLocalizeRegionCode()
        {
            const char *pName;
            switch (bcs::CMainApp::GetRegionLanguage()) {
            case LOCALIZE_JP_JA:
                pName = s_pMesgJpJa;
                break;
            case LOCALIZE_US_EN:
                pName = s_pMesgUsEn;
                break;
            case LOCALIZE_US_FR:
                pName = s_pMesgUsFr;
                break;
            case LOCALIZE_US_SP:
                pName = s_pMesgUsSp;
                break;
            case LOCALIZE_EU_EN:
                pName = s_pMesgEuEn;
                break;
            case LOCALIZE_EU_GE:
                pName = s_pMesgEuGe;
                break;
            case LOCALIZE_EU_FR:
                pName = s_pMesgEuFr;
                break;
            case LOCALIZE_EU_IT:
                pName = s_pMesgEuIt;
                break;
            case LOCALIZE_EU_SP:
                pName = s_pMesgEuSp;
                break;
            case LOCALIZE_EU_DU:
                pName = s_pMesgEuDu;
                break;
            default:
                pName = 0;
                break;
            }
            return pName;
        }

        const char *GetLocalizeLanguageCode()
        {
            const char *pName;
            switch (bcs::CMainApp::GetRegionLanguage()) {
            case LOCALIZE_JP_JA:
                pName = s_pFontJa;
                break;
            case LOCALIZE_US_EN:
                pName = s_pFontEn;
                break;
            case LOCALIZE_US_FR:
                pName = s_pFontFr;
                break;
            case LOCALIZE_US_SP:
                pName = s_pFontSp;
                break;
            case LOCALIZE_EU_EN:
                pName = s_pFontEn;
                break;
            case LOCALIZE_EU_GE:
                pName = s_pFontGe;
                break;
            case LOCALIZE_EU_FR:
                pName = s_pFontFr;
                break;
            case LOCALIZE_EU_IT:
                pName = s_pFontIt;
                break;
            case LOCALIZE_EU_SP:
                pName = s_pFontSp;
                break;
            case LOCALIZE_EU_DU:
                pName = s_pFontDu;
                break;
            default:
                pName = 0;
                break;
            }
            return pName;
        }

        void SetCockpitFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd,
                                const char *pMode)
        {
            const char *pFolder = GetLocalizeLayoutFolder();
            unsigned short nMode = pMode ? *(const unsigned short *)pMode : 0;

            for (long i = nStart; i <= nEnd; i++) {
                bcs::CLoadMan::FILE_INPUT &file = FileAt(pFiles, i);
                if ((file.m_nId & 0xFFFF0000) != FILE_LAYOUT) {
                    continue;
                }

                long bUse = 0;
                switch ((long)(file.m_nId & 0xF000)) {
                case 0x0000:
                    sprintf(file.m_szPath, "%s/cockpit/cpt%s.bin", pFolder, pMode);
                    bUse = 1;
                    break;
                case 0x1000: {
                    long nVariation = GetCockpitVariation(nMode, CGame::g_GameWork.m_nGameType);
                    if (nVariation >= 0) {
                        sprintf(file.m_szPath, "%s/cockpit/cpt%s%d.bin", pFolder, pMode,
                                nVariation);
                        bUse = 1;
                    }
                    break;
                }
                case 0x2000: {
                    long nNo = -1;
                    switch (nMode) {
                    case 'DO': {
                        long nChr = CGame::GetDodgeVariableChrNum(0);
                        if (nChr == 1) {
                            nChr = 2;
                        }
                        nNo = nChr + 10;
                        break;
                    }
                    case 'VO':
                        switch (CGame::g_GameWork.m_nViewSplit) {
                        case 1:
                            nNo = 20;
                            break;
                        case 2:
                            nNo = 21;
                            break;
                        default:
                            nNo = 10;
                            break;
                        }
                        break;
                    }
                    if (nNo >= 0) {
                        sprintf(file.m_szPath, "%s/cockpit/cpt%s%03d.bin", pFolder, pMode, nNo);
                        bUse = 1;
                    }
                    break;
                }
                case 0x5000:
                    sprintf(file.m_szPath, "%s/cockpit/pau%s.bin", pFolder, pMode);
                    bUse = 0;
                    break;
                case 0x6000:
                    sprintf(file.m_szPath, "%s/cockpit/res%s.bin", pFolder, pMode);
                    bUse = 1;
                    break;
                case 0x7000:
                    sprintf(file.m_szPath, "%s/main/main.bin", pFolder);
                    bUse = 0;
                    break;
                case 0xA000:
                    bUse = 0;
                    break;
                case 0xB000:
                    sprintf(file.m_szPath, "%s/cockpit/cptUL.bin", pFolder);
                    bUse = 0;
                    break;
                case 0xE000:
                    bUse = 0;
                    break;
                }
                SetFileUse(file, bUse);
            }
        }

        long GetCockpitVariation(unsigned long nMode, long nGameType)
        {
            long nVariation = -1;
            if (nMode == 'VO') {
                if (nGameType == 0) {
                    nVariation = 0;
                } else if (nGameType == 1) {
                    nVariation = 1;
                } else if (nGameType == 2) {
                    nVariation = 2;
                }
            } else if (nMode == 'HO') {
                if (nGameType == 0) {
                    nVariation = 0;
                } else if (nGameType == 1) {
                    nVariation = 0;
                } else if (nGameType == 2) {
                    nVariation = 2;
                }
            } else if (nMode == 'BA') {
                if (nGameType == 0) {
                    nVariation = 0;
                } else if (nGameType == 1) {
                    nVariation = 0;
                } else if (nGameType == 2) {
                    nVariation = 2;
                }
            } else {
                if (nGameType == 0) {
                    nVariation = 0;
                } else if (nGameType == 1) {
                    nVariation = 0;
                } else if (nGameType == 2) {
                    nVariation = 2;
                }
            }
            return nVariation;
        }

        void SetMainFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd)
        {
            const char *pFolder = GetLocalizeLayoutFolder();

            for (long i = nStart; i <= nEnd; i++) {
                bcs::CLoadMan::FILE_INPUT &file = pFiles[i];
                if ((file.m_nId & 0xFFFF0000) != FILE_LAYOUT) {
                    continue;
                }

                unsigned long nKind = file.m_nId & 0xF000;
                long bUse = 0;
                if (nKind == 0x4000) {
                    sprintf(file.m_szPath, "%s/main/load.bin", pFolder);
                    bUse = 1;
                } else if (nKind == 0x7000) {
                    sprintf(file.m_szPath, "%s/main/main.bin", pFolder);
                    bUse = 1;
                }
                SetFileUse(file, bUse);
            }
        }

        void SetLayoutFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd)
        {
            switch (CMainStep::GetStep()) {
            case 0:
                SetMainFilePath(pFiles, nStart, nEnd);
                break;
            case 2:
                SetCockpitFilePath(pFiles, nStart, nEnd,
                                   lbl_8047F2E8[CGame::g_GameWork.m_nGameMode]);
                break;
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                SetSelectFilePath(pFiles, nStart, nEnd);
                break;
            }
        }

        void SetTourCockpitFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd)
        {
            const char *pFolder = GetLocalizeLayoutFolder();
            const char *pMode = "TO";

            for (long i = nStart; i <= nEnd; i++) {
                if ((FileAt(pFiles, i).m_nId & 0xFFFF0000) != FILE_LAYOUT) {
                    continue;
                }

                long bUse = 0;
                switch ((long)(FileAt(pFiles, i).m_nId & 0xF000)) {
                case 0x0000:
                    sprintf(FileAt(pFiles, i).m_szPath, "%s/cockpit/cpt%s.bin", pFolder, pMode);
                    bUse = 1;
                    break;
                case 0x1000:
                    sprintf(FileAt(pFiles, i).m_szPath, "%s/cockpit/cpt%s%s.bin", pFolder, pMode,
                            lbl_8047F2E8[CGame::g_GameWork.m_nGameMode]);
                    bUse = 1;
                    break;
                case 0x2000:
                    sprintf(FileAt(pFiles, i).m_szPath, "%s/cockpit/cpt%s1.bin", pFolder, pMode);
                    bUse = 1;
                    break;
                case 0x5000:
                    bUse = 0;
                    break;
                case 0x6000:
                    bUse = 0;
                    break;
                case 0x7000:
                    sprintf(FileAt(pFiles, i).m_szPath, "%s/main/main.bin", pFolder);
                    bUse = 0;
                    break;
                case 0xA000:
                    sprintf(FileAt(pFiles, i).m_szPath, "%s/cockpit/cptVS.bin", pFolder);
                    bUse = 1;
                    break;
                case 0xB000:
                    sprintf(FileAt(pFiles, i).m_szPath, "%s/cockpit/cptUL.bin", pFolder);
                    bUse = 1;
                    break;
                case 0xE000:
                    bUse = 0;
                    break;
                }
                SetFileUse(FileAt(pFiles, i), bUse);
            }
        }

        void SetSelectFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd)
        {
            const char *pFolder = GetLocalizeLayoutFolder();

            for (long i = nStart; i <= nEnd; i++) {
                bcs::CLoadMan::FILE_INPUT &file = pFiles[i];
                if ((file.m_nId & 0xFFFF0000) != FILE_LAYOUT) {
                    continue;
                }

                unsigned long nKind = file.m_nId & 0xF000;
                long bUse = 0;
                if (nKind == 0x0000) {
                    sprintf(file.m_szPath, "%s/select/selMain.bin", pFolder);
                    bUse = 1;
                } else if (nKind == 0x7000) {
                    sprintf(file.m_szPath, "%s/main/main.bin", pFolder);
                    bUse = 0;
                } else if (nKind == 0x5000) {
                    bUse = 0;
                } else if (nKind == 0x6000) {
                    bUse = 0;
                } else if (nKind == 0xA000) {
                    sprintf(file.m_szPath, "%s/cockpit/cptVS.bin", pFolder);
                    bUse = 1;
                } else if (nKind == 0xB000) {
                    sprintf(file.m_szPath, "%s/cockpit/cptUL.bin", pFolder);
                    bUse = 1;
                } else if (nKind == 0xE000) {
                    sprintf(file.m_szPath, s_szGeneralPack);
                    bUse = 1;
                }
                SetFileUse(file, bUse);
            }
        }

        void SetCommonFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd)
        {
            const char *pFolder = GetLocalizeLayoutFolder();

            for (long i = nStart; i <= nEnd; i++) {
                bcs::CLoadMan::FILE_INPUT &file = pFiles[i];
                if ((file.m_nId & 0xFFFF0000) != FILE_LAYOUT) {
                    continue;
                }

                unsigned long nKind = file.m_nId & 0xF000;
                long bUse = 0;
                if (nKind == 0x7000) {
                    sprintf(file.m_szPath, "%s/main/main.bin", pFolder);
                    bUse = 0;
                } else if (nKind == 0xB000) {
                    sprintf(file.m_szPath, "%s/cockpit/cptUL.bin", pFolder);
                    bUse = 1;
                }
                SetFileUse(file, bUse);
            }
        }

        const char *GetLocalizeFolderName()
        {
            return GetLocalizeLayoutFolder();
        }

        void SetMessageFilePath(bcs::CLoadMan::FILE_INPUT *pFiles, long nStart, long nEnd,
                                long bLoad)
        {
            for (long i = nStart; i <= nEnd; i++) {
                bcs::CLoadMan::FILE_INPUT &file = pFiles[i];
                if ((file.m_nId & 0xFFFF0000) != FILE_MESSAGE) {
                    continue;
                }

                switch (file.m_nId & 0xF000) {
                case 0x1000:
                    file.m_nFlags |= FILE_FLAG_SKIP;
                    break;
                case 0x2000: {
                    const char *pCode = GetLocalizeRegionCode();
                    if (pCode != 0 && bLoad == 1) {
                        sprintf(file.m_szPath, "menu/mesg/%s.bin", pCode);
                        file.m_nFlags &= ~FILE_FLAG_SKIP;
                        file.m_nFlags |= FILE_FLAG_LOAD;
                    } else {
                        file.m_nFlags |= FILE_FLAG_SKIP;
                    }
                    break;
                }
                }
            }
        }

        void SetFontFilePath(bcs::CLoadMan::FILE_INPUT *pFile, const char *pName, long bLoad)
        {
            const char *pCode = GetLocalizeLanguageCode();
            sprintf(pFile->m_szPath, "menu/font/%s%s.bin", pName, pCode);
            if (pCode != 0 && bLoad == 1) {
                pFile->m_nFlags &= ~FILE_FLAG_SKIP;
                pFile->m_nFlags |= FILE_FLAG_LOAD;
            } else {
                pFile->m_nFlags |= FILE_FLAG_SKIP;
            }
        }
    }
}
