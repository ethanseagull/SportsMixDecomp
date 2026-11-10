// msw::CCockpitChrIcon2DBasket: the basketball character icons (comb).
#include "game/msw/CCockpitChrIcon2DBasket.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"

namespace {
    // (names guessed) The layout of one icon type. The table is in .rodata
    // in the original, so it is const; the layout functions take non-const
    // name tables, hence the casts in Set.
    struct IconData {
        long nResFile;             // 0x00: for CCockpit::GetResourceFileName
        const char *pLayoutName;   // 0x04
        long nAnimNum;             // 0x08
        const char *pAnimNames[5]; // 0x0C
        const char *pPaneNames[8]; // 0x20
    };

    const IconData s_IconData[] = {
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

    // The animation that a layout with more than four starts with.
    const long ANIM_START = 4;
}

msw::CCockpitChrIcon2DBasket::CCockpitChrIcon2DBasket(long nId, float fSortZ, long nLayer, long nType)
    : CCockpitChrIcon2D(nId, fSortZ, nLayer, nType)
{
}

// (guess) The original destructor stores both vtable pointers, which the
// compiler only keeps when the body calls something; see
// game/boss/CCockpitBossLayoutBase.cpp and notes/decisions.md.
static inline void Nothing() {}

msw::CCockpitChrIcon2DBasket::~CCockpitChrIcon2DBasket()
{
    Nothing();
}

int msw::CCockpitChrIcon2DBasket::Set()
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
            SetPaneAccessor(pPaneCtrl, const_cast<const char **>(rData.pPaneNames), 8);
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

void msw::CCockpitChrIcon2DBasket::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}
