// Free helpers of the cockpits (namespace msw::cockpit): the clock and
// count-down text, the special-shot telop, the "next" window, merged
// strings, billboard offsets, text box fonts and the number of split views.
#include "game/msw/cockpit/CockpitUtil.hpp"

#include "bcs/CWorldEnv.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitMenuNext.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/CMessage.hpp"
#include "nw4r/ef/Emitter.hpp"
#include "nw4r/g3d/Camera.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"
#include "nw4r/math/arithmetic.h"
#include "revolution/mtx.h"
#include "wchar.h"

namespace {
    // The text box of a pane object, null when it is something else.
    inline bcs::G2D::CTextBoxDL *GetTextBox(nw4r::lyt::Pane *pPane)
    {
        return nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pPane);
    }

    // The text of label pLabel in message file pFile.
    inline const wchar_t *GetMessageText(const char *pFile, const char *pLabel)
    {
        msw::menu::CMessage &rMessage = msw::menu::CMessage::s_Instance;
        return rMessage.GetTextByLabel(rMessage.GetFileIndex(pFile), pLabel);
    }

    // *pOut = *p, skipped when both are the same matrix.
    inline void CopyMtx(nw4r::math::MTX34 *pOut, const nw4r::math::MTX34 *p)
    {
        if (pOut != p) {
            PSMTXCopy(*p, *pOut);
        }
    }
}

namespace msw {
    namespace cockpit {

        long SetTimeValue(CMenuPaneAccessor *pAccessor, float fTime, int nArg, int bForce, long nAnim,
                          long nAnimAll, long nPane, long nShadowPane, long nCountPane, long nCountTextPane,
                          long nCountAnim)
        {
            long nResult = -1;
            long nFrame = (long)fTime;
            long nTenth = nFrame % 60 / 6;
            long nSec = nFrame / 60 % 60;
            long nMin = nFrame / 3600;

            if (pAccessor == 0) {
                return -1;
            }

            // Rounded up from the tenth of a second.
            if (nTenth > 0) {
                nSec++;
                if (nSec >= 60) {
                    nSec -= 60;
                    nMin++;
                }
            }

            bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(nPane);
            if (pObj != 0) {
                bcs::G2D::CTextBoxDL *pTextBox = GetTextBox(pObj->GetPane());
                if (pTextBox != 0) {
                    wchar_t text[] = L"x";
                    long nLen = wcslen(text) + 1;
                    swprintf(text, nLen, L"%1d", nMin);
                    if (bForce == 1 || wcsncmp(text, pTextBox->GetStringBuffer(), nLen) != 0) {
                        pTextBox->AllocStringBuffer(nLen);
                        pTextBox->SetString(text, 0);
                        pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
                    }
                }
            }

            bcs::G2D::CPaneObj *pSecObj = pAccessor->GetPaneObj(nShadowPane);
            if (pSecObj != 0) {
                bcs::G2D::CTextBoxDL *pTextBox = GetTextBox(pSecObj->GetPane());
                if (pTextBox != 0) {
                    wchar_t text[] = L":xx";
                    long nLen = wcslen(text) + 1;
                    swprintf(text, nLen, L":%02d", nSec);
                    if (bForce == 1 || wcsncmp(text, pTextBox->GetStringBuffer(), nLen) != 0) {
                        pTextBox->AllocStringBuffer(nLen);
                        pTextBox->SetString(text, 0);
                        pSecObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
                    }
                }
            }

            if (nCountPane < 0 || nCountTextPane < 0) {
                return -1;
            }

            // The count-down in the last ten seconds.
            if (nMin == 0 && (unsigned long)nSec <= 9) {
                if (pAccessor->SetTextBoxString(nCountTextPane, nSec, 0) > 0) {
                    bcs::G2D::CPaneObj *pCountObj = pAccessor->GetPaneObj(nCountPane);
                    if (pCountObj != 0) {
                        pCountObj->PlayAnimation(nCountAnim, 1.0f, 0.0f, 0, 1, 1);
                        nResult = nSec;
                        pCountObj->GetPane()->SetVisible(true);
                    }
                }
            } else {
                const wchar_t *pText = L"x";
                pAccessor->SetTextBoxString(nCountTextPane, pText, wcslen(pText));
                pAccessor->SetPaneDisp(nCountPane, 0);
            }
            return nResult;
        }
    }
}

// (name unknown) Opens the cockpit's "next" window (object nId), its A
// button labelled with CockpitTelop's Btn_A_Next. 0; -1 when it could not
// be made, -2 without a cockpit.
extern "C" long fn_801DFB88(long nId, float fSortZ)
{
    msw::CCockpitMenuNext *pMenu = new msw::CCockpitMenuNext(nId, fSortZ, 0);
    if (pMenu == 0) {
        return -1;
    }

    msw::CCockpit *pCockpit = msw::CCockpit::GetTask();
    if (pCockpit == 0) {
        return -2;
    }

    pCockpit->AddObj(pMenu);
    pMenu->SetMenuResultId(0, 1000);
    pMenu->SetButtonText(0, GetMessageText("CockpitTelop", "Btn_A_Next"));
    pMenu->Setup();
    return 0;
}

namespace msw {
    namespace cockpit {

        const wchar_t *GetMergeString(wchar_t *pBuf, long nLen, const wchar_t *pText0, const wchar_t *pText1,
                                      const wchar_t *pSep)
        {
            if (pText0 == 0 || pText1 == 0) {
                return 0;
            }

            wcsncpy(pBuf, pText0, nLen);
            if (pSep != 0) {
                wcsncat(pBuf, pSep, nLen);
            }
            wcsncat(pBuf, pText1, nLen);
            return pBuf;
        }

        long SetSPGaugeTelop(CMenuPaneAccessor *pAccessor, long nPane, long nInAnim, long nLoopAnim,
                             long nOutAnim, long nHideAnim, float fRate)
        {
            bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(nPane);
            if (pObj == 0) {
                return -1;
            }

            if (fRate >= 0.0f) {
                pObj->PlayAnimation(nInAnim, 1.0f, 0.0f, 0, 1, 1);
                if (fRate > 1.0f) {
                    pObj->PushNextAnimation(nLoopAnim, 1.0f / fRate, 0.0f, 0, 1);
                }
                pObj->PushNextAnimation(nOutAnim, 1.0f, 0.0f, 0, 1);
            } else {
                pObj->PlayAnimation(nHideAnim, 1.0f, 0.0f, 0, 1, 1);
            }
            return 0;
        }

        CChrCore *GetTeamLeaderChrCore(long nTeam)
        {
            return CChrMan::GetTeamLeader(nTeam);
        }

        long GetBillboardOffset(const bcs::FVEC3 *pOfs, const bcs::FMTX34 *pMtx, long nCamera, bcs::FVEC3 *pOut)
        {
            if (pOfs == 0 || pMtx == 0) {
                return -1;
            }

            nw4r::math::MTX34 mtx;
            nw4r::math::MTX34 viewMtx;
            nw4r::math::MTX34 camMtx;
            nw4r::math::MTX34 scaleMtx;

            PSMTXIdentity(mtx);
            nw4r::ef::Emitter::RestructMatrix(
                &mtx, reinterpret_cast<nw4r::math::MTX34 *>(const_cast<bcs::FMTX34 *>(pMtx)), false, false, 100,
                false);

            nw4r::g3d::Camera camera(
                reinterpret_cast<nw4r::g3d::CameraData *>(bcs::CWorldEnv::GetViewCameraData(nCamera)));
            camera.GetCameraMtx(&camMtx);
            PSMTXConcat(camMtx, mtx, viewMtx);

            // The rotation of the model in view space, reduced to the
            // inverse length of each axis on the diagonal.
            CopyMtx(&scaleMtx, &viewMtx);
            scaleMtx._00 = nw4r::math::FrSqrt(scaleMtx._00 * scaleMtx._00 + scaleMtx._10 * scaleMtx._10 +
                                              scaleMtx._20 * scaleMtx._20);
            scaleMtx._10 = 0.0f;
            scaleMtx._20 = 0.0f;
            scaleMtx._11 = nw4r::math::FrSqrt(scaleMtx._01 * scaleMtx._01 + scaleMtx._11 * scaleMtx._11 +
                                              scaleMtx._21 * scaleMtx._21);
            scaleMtx._01 = 0.0f;
            scaleMtx._21 = 0.0f;
            scaleMtx._22 = nw4r::math::FrSqrt(scaleMtx._02 * scaleMtx._02 + scaleMtx._12 * scaleMtx._12 +
                                              scaleMtx._22 * scaleMtx._22);
            scaleMtx._02 = 0.0f;
            scaleMtx._12 = 0.0f;

            PSMTXInverse(viewMtx, viewMtx);
            PSMTXConcat(viewMtx, scaleMtx, viewMtx);
            PSMTXMultVec(viewMtx, reinterpret_cast<const Vec *>(pOfs), reinterpret_cast<Vec *>(pOut));
            return 0;
        }

        long SetTextFont(bcs::G2D::CPaneCtrl *pPaneCtrl, nw4r::lyt::Pane *pPane, const char *pLayoutName,
                         const char *pFontName)
        {
            if (pPaneCtrl == 0) {
                return -1;
            }
            if (pPane == 0) {
                return -2;
            }

            nw4r::lyt::ResourceAccessor *pAccessor = pPaneCtrl->GetResAccessor();
            if (pAccessor == 0) {
                return -11;
            }

            void *pFont = pAccessor->GetResource('font', pFontName, 0);
            if (pFont == 0) {
                return -12;
            }

            bcs::G2D::CTextBoxDL *pTextBox = GetTextBox(pPane);
            if (pTextBox == 0) {
                return -13;
            }

            // Setting the font resets the font size: keep it.
            nw4r::lyt::Size size = pTextBox->GetFontSize();
            pTextBox->SetFontByResource(pFont);
            pTextBox->SetFontSize(size);
            return 0;
        }

        long GetViewSplitNum()
        {
            switch (CGame::g_GameWork.m_nViewSplit) {
                case 0:
                    return 1;
                case 3:
                    return 4;
                default:
                    return 2;
            }
        }
    }
}
