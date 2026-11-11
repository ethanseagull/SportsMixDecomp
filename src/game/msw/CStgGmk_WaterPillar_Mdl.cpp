#include "game/msw/CStgGmk_WaterPillar.hpp"
#include "game/msw/CFgMdl.hpp"
#include "game/msw/CLoadData.hpp"
#include "bcs/New.hpp"

// The water pillar model. The original file goes on past this unit's slice:
// .rodata (s03_gm_funsui_00_top, the model table), .data (the animation
// suffixes, then the actor's state table and every vtable of the pillar
// classes) and .sdata2 show that msw::CStgGmk_WaterPillar_Act,
// CStgGmk_WaterPillar_Drv, CStgGmk_WaterPillar and
// CStgGmk_WaterPillarCluster are the same file, with their weak functions
// at its end (0x80195D7C to 0x801960B4).

namespace {

    // Suffixes of the pillar's animations, appended to the model name.
    const char *s_AnmSuffix[] = { "", "_start", "_end", "_loop", 0, 0 };

    // The pillar models; GetMdlIdx picks one.
    const msw::CStgMdl::MDL_PARAM s_MdlParam[] = {
        { "s03_gm_funsui_00", 0, 0, 0, s_AnmSuffix, "s03_gm_funsui_00", 0, 0 },
    };
}

namespace msw {

    CStgGmk_WaterPillar_Mdl::CStgGmk_WaterPillar_Mdl(CStgGmk *pParent, long idx) : CStgMdl(pParent)
    {
        m_nMdlIdx = idx;
    }

    CStgGmk_WaterPillar_Mdl::~CStgGmk_WaterPillar_Mdl()
    {
    }

    void CStgGmk_WaterPillar_Mdl::Create()
    {
        long idx = m_nMdlIdx;
        idx = idx > 0 ? 0 : (idx < 0 ? 0 : idx);
        CGenericMVAGimmickCreator creator(CLoadData::GetRes(2), &s_MdlParam[idx]);
        CStgMdl::Create(creator);
        SetAnm(0, 0, 1.0f, 0.0f, 0.0f, 8);
    }

    bcs::CMdlObj *CStgGmk_WaterPillar_Mdl::NewMdl(long heap) const
    {
        CFgMdl *pMdl = new (heap) CFgMdl;
        if (pMdl != 0) {
            pMdl->SetAlphaTestNearRange(2.0f);
            pMdl->SetAlphaTestFarRange(6.0f);
            pMdl->SetExBlendMode(1);
            pMdl->CalcBoundingVolume();
            pMdl->OnFlag(0x10000);
        }
        return pMdl;
    }

    // Stretches the model to the height of the actor's water column.
    void CStgGmk_WaterPillar_Mdl::Main(float rate)
    {
        CStgAct *pAct = GetParent()->GetAct();
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pAct != 0 && pMdl != 0) {
            float mdlHeight = GetParam_MdlHeight();
            float scale = static_cast<CStgGmk_WaterPillar_Act *>(pAct)->GetParam_Height() / mdlHeight;
            pMdl->m_vScale.x = 1.0f;
            pMdl->m_vScale.y = scale;
            pMdl->m_vScale.z = 1.0f;
            pMdl->CalcRootMtx();
            pMdl->CalcWorldMtx(0);
        }
        CStgMdl::Main(rate);
    }

    float CStgGmk_WaterPillar_Mdl::GetParam_MdlHeight() const
    {
        return 5.0f;
    }
}
