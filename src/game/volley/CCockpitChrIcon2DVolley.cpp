// Constructor, destructor, Set and Remove written from
// src/game/hockey/CCockpitChrIcon2DHockey.cpp by tools/decomp/port_unit.py
// (Hockey -> Volley); the face functions are VolleyF's own.
// msw::CCockpitChrIcon2DVolley: the volley character icons (radar icons
// with the characters' faces, and the comb).
#include "game/msw/CCockpitChrIcon2DVolley.hpp"
#include "bcs/G2D/CPictureDL.hpp"
#include "bcs/G2D/CTexMap.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Material.hpp"

namespace {
    // (names guessed) The layout of one icon type. The table is in .rodata
    // in the original, so it is const; the layout functions take non-const
    // name tables, hence the casts in Set.
    struct IconData {
        long nResFile;              // 0x00: for CCockpit::GetResourceFileName
        const char *pLayoutName;    // 0x04
        long nAnimNum;              // 0x08
        const char *pAnimNames[5];  // 0x0C
        const char *pPaneNames[26]; // 0x20
    };

    // The panes of the radar icon layout, by index into pPaneNames.
    enum {
        PANE_ICON_NULL = 0, // 0 to 3, twice
        PANE_BASE_ICON = 8, // 8 to 13, one per icon
        PANE_FACE = 14,     // 14 to 19
        PANE_PIC_FACE = 20, // 20 to 25
        PANE_NUM = 26
    };

    const IconData s_IconData[] = {
        {
            2, "VO_RadarIcon.brlyt",
            5, { "VO_RadarIcon_vo_r_00.brlan", "VO_RadarIcon_vo_r_02.brlan", "VO_RadarIcon_vo_r_03.brlan",
                 "VO_RadarIcon_vo_r_04.brlan", "VO_RadarIcon_vo_r_00.brlan" },
            { "IconNull_00", "IconNull_01", "IconNull_02", "IconNull_03",
              "IconNull_00", "IconNull_01", "IconNull_02", "IconNull_03",
              "BaseIcon_vo_00", "BaseIcon_vo_01", "BaseIcon_vo_02",
              "BaseIcon_vo_03", "BaseIcon_vo_04", "BaseIcon_vo_05",
              "Face_00", "Face_01", "Face_02", "Face_03", "Face_04", "Face_05",
              "picFace00", "picFace01", "picFace02", "picFace03", "picFace04", "picFace05" },
        },
        {
            1, "TE_comb_A.brlyt",
            5, { "TE_comb_A_Comb_00.brlan", "TE_comb_A_Comb_02.brlan", "TE_comb_A_Comb_03.brlan",
                 "TE_comb_A_Comb_04.brlan", "TE_comb_A_F_00.brlan" },
            { "comb_a_Null", "comb_b_Null", "comb_c_Null", "comb_a_Null",
              "comb_a_Null", "comb_b_Null", "comb_c_Null", "comb_a_Null" },
        },
        {
            1, "TE_comb_A.brlyt",
            5, { "TE_comb_A_Comb_00.brlan", "TE_comb_A_Comb_02.brlan", "TE_comb_A_Comb_03.brlan",
                 "TE_comb_A_Comb_04.brlan", "TE_comb_A_F_01.brlan" },
            { "comb_a_Null", "comb_b_Null", "comb_c_Null", "comb_a_Null",
              "comb_a_Null", "comb_b_Null", "comb_c_Null", "comb_a_Null" },
        },
    };

    // Per icon: the pane that frames the face, the picture pane with the
    // face, and the base pane that turns with the character.
    const unsigned char s_FacePane[6] = {
        PANE_FACE + 0, PANE_FACE + 1, PANE_FACE + 2, PANE_FACE + 3, PANE_FACE + 4, PANE_FACE + 5
    };
    const unsigned char s_PicFacePane[6] = {
        PANE_PIC_FACE + 0, PANE_PIC_FACE + 1, PANE_PIC_FACE + 2,
        PANE_PIC_FACE + 3, PANE_PIC_FACE + 4, PANE_PIC_FACE + 5
    };
    const unsigned char s_BaseIconPane[6] = {
        PANE_BASE_ICON + 0, PANE_BASE_ICON + 1, PANE_BASE_ICON + 2,
        PANE_BASE_ICON + 3, PANE_BASE_ICON + 4, PANE_BASE_ICON + 5
    };

    // The animation that a layout with more than four starts with.
    const long ANIM_START = 4;
}

msw::CCockpitChrIcon2DVolley::CCockpitChrIcon2DVolley(long nId, float fSortZ, long nLayer, long nType)
    : CCockpitChrIcon2D(nId, fSortZ, nLayer, nType)
{
}

// (guess) The original destructor stores both vtable pointers, which the
// compiler only keeps when the body calls something; see
// game/boss/CCockpitBossLayoutBase.cpp and notes/decisions.md.
static inline void Nothing() {}

msw::CCockpitChrIcon2DVolley::~CCockpitChrIcon2DVolley()
{
    Nothing();
}

int msw::CCockpitChrIcon2DVolley::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const IconData &rData = s_IconData[m_nType];
    const char *pFileName = CCockpit::GetResourceFileName(rData.nResFile);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        long nLayer;

        // default first: the original lays its block out right after the
        // compare chain, which this order reproduces (notes/toolchain.md).
        switch (m_nLayer) {
            default:
                nLayer = 1;
                break;
            case 1:
                nLayer = 2;
                break;
            case 2:
                nLayer = 3;
                break;
            case 3:
                nLayer = 4;
                break;
        }

        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, nLayer, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->BuildLayout(0, rData.pLayoutName);
            pPaneCtrl->BuildAnim(0, const_cast<const char **>(rData.pAnimNames), rData.nAnimNum);
            SetPaneAccessor(pPaneCtrl, const_cast<const char **>(rData.pPaneNames), PANE_NUM);
            pPaneCtrl->SetSortZ(m_fSortZ);
            if (pPaneCtrl->GetAnimNum() > ANIM_START) {
                pPaneCtrl->PlayAnim(ANIM_START, 1.0f, 0.0f, 1, 1);
            }
            m_nStep = 1;
            m_nState = 1;
        }
    }
    return 0;
}

void msw::CCockpitChrIcon2DVolley::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

long msw::CCockpitChrIcon2DVolley::SetIconFace(long nIcon, long nChr)
{
    nw4r::lyt::Pane *pFacePane = m_pPaneAccessor->GetPane(s_FacePane[nIcon]);

    if (pFacePane == 0) {
        return -1;
    }
    if (CChrMan::GetChrCore(nChr) == 0) {
        return -10;
    }

    long nPicPane = s_PicFacePane[nIcon];
    nw4r::lyt::Pane *pPicPane = m_pPaneAccessor->GetPane(nPicPane);

    // No picture yet: make one in the face pane's place.
    if (pPicPane == 0) {
        nw4r::lyt::Pane *pNewPane = menu::CreateFaceIconPicturePane(nChr, GetLytObj(), pFacePane,
                                                                    m_pPaneAccessor->GetPaneName(nPicPane));

        if (pNewPane == 0) {
            return -2;
        }
        pNewPane->SetVisible(true);
        pNewPane->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
        pNewPane->SetSize(pFacePane->GetSize());
        return 0;
    }

    // Otherwise put the character's texture into the existing picture.
    bcs::G2D::CPictureDL *pPicture = nw4r::ut::DynamicCast<bcs::G2D::CPictureDL *>(pPicPane);

    if (pPicture != 0) {
        const _GXTexObj *pTexObj = CChrMan::GetChrCore(nChr)->GetRadarIconTexObj();

        if (pTexObj != 0 && pPicture->GetMaterial()->GetTextureNum() != 0) {
            bcs::G2D::CTexMap texMap(*pTexObj);
            nw4r::lyt::Material *pMaterial = pPicture->GetMaterial();

            pMaterial->SetTexture(0, texMap);
            static_cast<bcs::G2D::CMaterialDL *>(pMaterial)->SetModified();

            // (as in the original, which looks the same pane up again) The
            // texture keeps the transform the material had.
            nw4r::lyt::Pane *pSrcPane = m_pPaneAccessor->GetPane(nPicPane);

            if (pSrcPane != 0) {
                bcs::G2D::CPictureDL *pSrcPicture = nw4r::ut::DynamicCast<bcs::G2D::CPictureDL *>(pSrcPane);

                if (pSrcPicture != 0) {
                    nw4r::lyt::Material *pSrcMaterial = pSrcPicture->GetMaterial();

                    if (pSrcMaterial != 0) {
                        pMaterial->SetTexSRT(0, pSrcMaterial->GetTexSRT(0));
                    }
                }
            }
        }
    }
    return 0;
}

long msw::CCockpitChrIcon2DVolley::SetChrIcon(long nIcon, const bcs::FVEC3 &rPos, long nChr)
{
    return SetChrIconImpl(nIcon, rPos, nChr);
}

// pRot points to the icon's rotation (a float, degrees about Z).
long msw::CCockpitChrIcon2DVolley::SetChrIcon(long nIcon, const bcs::FVEC3 &rPos, long nChr, const void *pRot)
{
    long nResult = SetChrIconImpl(nIcon, rPos, nChr);

    if (nResult >= 0) {
        SetIconFace(nIcon, nChr);

        nw4r::lyt::Pane *pBasePane = m_pPaneAccessor->GetPane(s_BaseIconPane[nIcon]);

        if (pBasePane != 0) {
            pBasePane->SetRotate(nw4r::math::VEC3(0.0f, 0.0f, *static_cast<const float *>(pRot)));
        }
    }
    return nResult;
}
