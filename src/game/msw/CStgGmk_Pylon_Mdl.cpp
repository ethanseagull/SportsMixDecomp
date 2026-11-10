#include "game/msw/CStgGmk_Pylon.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStgMdlObjBase.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/New.hpp"

// The pylon model. The original file goes on past this unit's slice:
// CStgGmk_Pylon_Mdl::Main (0x80218098) and the three GetParam_Org* getters
// follow OnPostCreate directly. Main needs msw::CStgGmk_Pylon_Act's
// declaration (its GetParam_MdlRadius/GetParam_MdlHeight) and is left for
// that unit.

namespace {

    const msw::CStgMdl::MDL_PARAM s_MdlParam[] = {
        { "s04_redcorn_00", 0, 0, 0, 0, 0, 0, 0 },
    };
}

namespace msw {

    CStgGmk_Pylon_Mdl::CStgGmk_Pylon_Mdl(CStgGmk *pParent, long idx) : CStgMdl(pParent) {
        m_nMdlIdx = idx;
    }

    CStgGmk_Pylon_Mdl::~CStgGmk_Pylon_Mdl() {
    }

    void CStgGmk_Pylon_Mdl::Create() {
        long idx = m_nMdlIdx;
        idx = idx > 0 ? 0 : (idx < 0 ? 0 : idx);
        CGenericGimmickCreator creator(CLoadData::GetRes(0), &s_MdlParam[idx]);
        CStgMdl::Create(creator);
    }

    bcs::CMdlObj *CStgGmk_Pylon_Mdl::NewMdl(long heap) const {
        return new (heap) CStgMdlObjBase<bcs::CMdlObj>;
    }

    void CStgGmk_Pylon_Mdl::OnPostCreate() {
    }
}
