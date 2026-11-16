// msw::menu: free helpers for menus and cockpits (picture panes for faces
// and Miis, numbers as text, screen positions, team sizes, key texts of the
// controller style). Lives in the DOL. The file name is a guess.

#include <string.h>
#include <wchar.h>

#include "bcs/CMainApp.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CTexMap.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/menu/CMessage.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "game/msw/menu/PadStyle.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/lyt/TextBox.hpp"
#include "nw4r/math/types.h"

extern "C" {
    // (in CChrMan.cpp, no symbol) The face icon texture of character nChr,
    // or null.
    const _GXTexObj *fn_800EF0DC(long nChr);
}

namespace {
    // Face icon scales: characters below 0x13, and the others.
    nw4r::math::VEC2 s_FaceIconScale(1.0f, 1.0f);
    nw4r::math::VEC2 s_FaceIconScaleL(1.1f, 1.1f);

    // GetNumString's text.
    wchar_t s_NumString[9];
}

namespace msw {
    namespace menu {

        nw4r::lyt::Pane *CreateMiiPicturePane(const _GXTexObj *pTexObj, bcs::G2D::CPaneCtrl *pPaneCtrl,
                                              nw4r::lyt::Pane *pPane, const char *pName)
        {
            if (pTexObj == 0 || pPaneCtrl == 0 || pName == 0) {
                return 0;
            }

            bcs::G2D::CTexMap texMap(*pTexObj);
            nw4r::lyt::Pane *pPicture = bcs::G2D::CreatePicturePane(pName, texMap);
            if (pPicture == 0) {
                return 0;
            }
            pPane->AppendChild(pPicture);
            pPicture->SetVisible(true);
            return pPicture;
        }

        nw4r::lyt::Pane *CreateFaceIconPicturePane(long nPlayer, bcs::G2D::CPaneCtrl *pPaneCtrl,
                                                   nw4r::lyt::Pane *pParent, const char *pName)
        {
            CChrCore *pChr = CChrMan::GetChrCore(nPlayer);
            if (pChr == 0) {
                return 0;
            }
            if (pChr->GetRadarIconTexObj() == 0) {
                return 0;
            }
            const _GXTexObj *pTexObj = pChr->GetCockpitIconTexObj();
            if (pTexObj == 0) {
                return 0;
            }
            return CreateMiiPicturePane(pTexObj, pPaneCtrl, pParent, pName);
        }

        const nw4r::math::VEC2 &GetCockpitFaceIconScale(long nChr)
        {
            if (nChr >= 0x13) {
                return s_FaceIconScaleL;
            }
            return s_FaceIconScale;
        }
    }
}

// (name unknown) A picture pane named pName showing character nChr's face
// icon, made under pParent; null on failure.
extern "C" nw4r::lyt::Pane *fn_8019B49C(long nChr, bcs::G2D::CPaneCtrl *pPaneCtrl, nw4r::lyt::Pane *pParent,
                                        const char *pName)
{
    const _GXTexObj *pTexObj = fn_800EF0DC(nChr);
    if (pTexObj == 0) {
        return 0;
    }
    return msw::menu::CreateMiiPicturePane(pTexObj, pPaneCtrl, pParent, pName);
}

namespace msw {
    namespace menu {

        const wchar_t *GetNumString(unsigned long nNum, unsigned long nArg)
        {
            wchar_t format[6] = L"%d";
            if (nArg != 0) {
                swprintf(format, 6, L"%%0%dd", nArg);
            }
            swprintf(s_NumString, 9, format, nNum);
            return s_NumString;
        }

        wchar_t *GetNumericText(long nValue, const wchar_t *pFormat, wchar_t *pOut)
        {
            unsigned long nSize = wcslen(pOut) + 1;
            int nLen = swprintf(pOut, nSize, pFormat, nValue);
            if (nLen >= 0) {
                pOut[nLen] = 0;
            }
            return pOut;
        }

        int GetWorldScreenPosition(const bcs::FVEC3 &rPos, long nView, bcs::FVEC3 &rOut)
        {
            bcs::G2D::GetWorldScreenPosition(rPos, nView, rOut);
            rOut.z = 0.0f;

            int nWidth = bcs::CMainApp::g_Rmode.fbWidth;
            int nHeight = bcs::CMainApp::g_Rmode.efbHeight;
            float fHalfW = 0.5f * nWidth;
            float fHalfH = 0.5f * nHeight;

            switch (bcs::CWorldEnv::m_vstViewSplitType) {
            case 1:
                if (nView == 1) {
                    rOut.x -= fHalfW;
                }
                break;
            case 2:
                if (nView == 1) {
                    rOut.y += fHalfH;
                }
                break;
            case 3:
                if (nView & 1) {
                    rOut.x -= fHalfW;
                }
                if (nView & 2) {
                    rOut.y += fHalfH;
                }
                break;
            }
            return 0;
        }
    }
}

// (name unknown) Writes the file name pPath without its extension to pName;
// returns its size with the terminator, or 0 when it does not fit.
extern "C" int fn_8019B780(const char *pPath, char *pName)
{
    unsigned long nSize = strlen(pPath) + 1;
    if (nSize == 0 || nSize >= 0x7F) {
        pName[0] = 0;
        return 0;
    }

    strncpy(pName, pPath, nSize);
    for (long i = nSize - 1; i > 0; i--) {
        if (pName[i] == '.') {
            pName[i] = 0;
            break;
        }
    }
    return nSize;
}

namespace msw {
    namespace menu {

        long GetTeamPlayerNum(long nType)
        {
            switch (nType) {
            case 0:
            case 1:
            case 9:
            case 10:
            case 13:
            case 14:
            case 15:
            case 16:
            case 18:
            case 19:
            case 21:
            case 24:
            case 25:
                return 3;
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 12:
            case 17:
            case 20:
            case 22:
            case 23:
                return 2;
            case 2:
            case 3:
            case 11:
                return 1;
            }
            return 1;
        }

        long GetTeamPlayerNum()
        {
            long nNum = 0;
            long nChrNum = CChrMan::GetChrCoreNum();
            for (long i = 0; i < nChrNum; i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr == 0 || pChr->IsDisable() || pChr->GetTeamNo() != 0) {
                    continue;
                }
                if (CGame::IsCharEx_DodgeOutfielder(i) || CGame::IsCharEx_HockeyKeeper(i)) {
                    continue;
                }
                nNum++;
            }
            return nNum;
        }
    }
}

namespace {
    // The message file holding the key texts.
    const char *s_KeyTextFile = "Button_Icon";
}

// (name unknown) Replaces the key marks in pTextBox's text with those of the
// controller style: with nKind set "A" and "B" become "2" and "1", otherwise
// the reverse. -1 when pTextBox is null.
extern "C" int fn_8019B918(nw4r::lyt::TextBox *pTextBox, int nKind)
{
    static const char *s_KeyLabels[2][2] = {
        { "A", "2" },
        { "B", "1" },
    };

    if (pTextBox == 0) {
        return -1;
    }

    long nLen = pTextBox->GetStringLength();
    const wchar_t *pSrc = pTextBox->GetStringBuffer();
    wchar_t *pText = (wchar_t *)__alloca((nLen + 1) * sizeof(wchar_t));
    if (pText == 0) {
        return -1;
    }

    int nFrom;
    int nTo;
    if (nKind != 0) {
        nFrom = 0;
        nTo = 1;
    } else {
        nFrom = 1;
        nTo = 0;
    }

    msw::menu::CMessage &rMessage = msw::menu::CMessage::s_Instance;
    long nFile = rMessage.GetFileIndex(s_KeyTextFile);

    for (long i = 0; i < nLen; i++) {
        pText[i] = pSrc[i];
    }
    pText[nLen] = 0;

    for (int i = 0; i < 2; i++) {
        const wchar_t *pFrom = rMessage.GetTextByLabel(nFile, s_KeyLabels[i][nFrom]);
        const wchar_t *pTo = rMessage.GetTextByLabel(nFile, s_KeyLabels[i][nTo]);
        if (pFrom == 0 || pTo == 0) {
            continue;
        }
        for (long j = 0; j < nLen; j++) {
            if (pText[j] == *pFrom) {
                pText[j] = *pTo;
            }
        }
    }

    pTextBox->SetString(pText, 0, nLen);
    return 0;
}

namespace msw {
    namespace menu {

        void ReplaceKeyTextByPane(nw4r::lyt::Pane *pPane, int nKind)
        {
            fn_8019B918(nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pPane), nKind);
        }

        int SetDispPaneByPadStyle(nw4r::lyt::Pane *pPane0, nw4r::lyt::Pane *pPane1, int nStyle)
        {
            int nShow0;
            int nShow1;
            if (nStyle != 0) {
                nShow0 = 0;
                nShow1 = 1;
            } else {
                nShow0 = 1;
                nShow1 = 0;
            }

            if (pPane0 != 0) {
                pPane0->SetVisible(nShow0 == 1);
            }
            if (pPane1 != 0) {
                pPane1->SetVisible(nShow1 == 1);
            }
            return 0;
        }
    }
}
