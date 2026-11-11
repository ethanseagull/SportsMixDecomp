// msw::CGoalMdlBasket: the models of the basket game's goal. The model
// index picks the part: the backboard (1) and its stand (5) fade out while
// the camera looks from behind the goal, and the stand and the figures (4)
// get view-fading alpha test ranges. Stages whose goal has moving parts
// give per-node motion settings for the backboard.
#include "game/msw/CGoalMdlBasket.hpp"

#include "bcs/New.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameCamera.hpp"

namespace {

    // The model indices of the goal's parts (names guessed).
    enum {
        MDL_BOARD = 1,
        MDL_FIGURE = 4,
        MDL_STAND = 5
    };

    // The view-fading settings of the stand and the figures.
    inline void SetFadeRange(msw::CFgMdl *pFg)
    {
        if (pFg != 0) {
            pFg->SetUnk214And218(0.0f);
            pFg->SetAlphaTestNearRange(2.0f);
            pFg->SetAlphaTestFarRange(5.0f);
        }
    }

    // Stage 1.
    const bcs::CAnmObjMotBase::NODE_EX_INFO sNodeExInfo01[] = {
        { 0, 2, 0, 0 }, { 1, 2, 1, 0 }, { 2, 2, 1, 0 }, { 3, 2, 1, 0 },
        { 4, 2, 1, 0 }, { 5, 2, 1, 0 }, { 6, 2, 1, 0 }, { 7, 2, 1, 0 },
        { 8, 2, 1, 0 }, { 9, 2, 1, 0 }, { 10, 2, 1, 0 }, { 11, 2, 1, 0 },
        { 12, 2, 1, 0 }, { 13, 2, 1, 0 }, { 14, 2, 1, 0 }, { 15, 2, 1, 0 },
        { 16, 2, 1, 0 }, { 17, 2, 1, 0 }, { 18, 2, 1, 0 }, { 19, 2, 1, 0 },
        { 20, 2, 1, 0 }, { 21, 2, 1, 0 }, { 22, 2, 1, 0 }, { 23, 3, 1, 0 },
        { 24, 3, 1, 0 }, { 25, 3, 1, 0 }, { 26, 3, 1, 0 }, { 27, 3, 1, 0 },
        { 28, 3, 1, 0 }, { 29, 3, 1, 0 }, { 30, 3, 1, 0 }, { 31, 3, 1, 0 },
        { 32, 3, 1, 0 },
    };

    // Stage 2.
    const bcs::CAnmObjMotBase::NODE_EX_INFO sNodeExInfo02[] = {
        { 0, 2, 0, 0 }, { 1, 2, 1, 0 }, { 2, 2, 1, 0 }, { 3, 2, 1, 0 },
        { 12, 0, 1, 0 }, { 13, 1, 1, 0 }, { 14, 1, 1, 0 }, { 15, 3, 1, 0 },
        { 16, 3, 1, 0 }, { 17, 3, 1, 0 }, { 18, 3, 1, 0 }, { 19, 2, 1, 0 },
        { 4, 0, 1, 0 }, { 5, 1, 1, 0 }, { 6, 1, 1, 0 }, { 7, 3, 1, 0 },
        { 8, 3, 1, 0 }, { 9, 3, 1, 0 }, { 10, 3, 1, 0 }, { 11, 2, 1, 0 },
    };

    // Stage 3.
    const bcs::CAnmObjMotBase::NODE_EX_INFO sNodeExInfo03[] = {
        { 0, 2, 0, 0 }, { 1, 2, 1, 0 }, { 2, 2, 1, 0 }, { 3, 2, 1, 0 },
        { 4, 2, 1, 0 }, { 5, 2, 1, 0 }, { 6, 2, 1, 0 },
    };

    // Stage 5.
    const bcs::CAnmObjMotBase::NODE_EX_INFO sNodeExInfo05[] = {
        { 0, 3, 0, 0 }, { 1, 2, 1, 0 }, { 2, 2, 1, 0 }, { 3, 2, 1, 0 },
        { 4, 2, 1, 0 }, { 5, 2, 1, 0 }, { 6, 2, 1, 0 }, { 7, 2, 1, 0 },
        { 8, 2, 1, 0 }, { 9, 2, 1, 0 }, { 10, 2, 1, 0 }, { 11, 2, 1, 0 },
        { 12, 2, 1, 0 }, { 13, 2, 1, 0 }, { 14, 2, 1, 0 }, { 15, 2, 1, 0 },
    };

    // Stage 6.
    const bcs::CAnmObjMotBase::NODE_EX_INFO sNodeExInfo06[] = {
        { 0, 2, 0, 0 }, { 1, 2, 1, 0 }, { 2, 2, 1, 0 }, { 3, 2, 1, 0 },
        { 5, 2, 1, 0 }, { 4, 2, 1, 0 }, { 6, 2, 1, 0 }, { 7, 2, 1, 0 },
        { 8, 2, 1, 0 }, { 9, 2, 1, 0 }, { 10, 2, 1, 0 }, { 11, 2, 1, 0 },
        { 12, 2, 1, 0 }, { 13, 2, 1, 0 }, { 14, 2, 1, 0 }, { 15, 2, 1, 0 },
    };

    // Stage 9.
    const bcs::CAnmObjMotBase::NODE_EX_INFO sNodeExInfo09[] = {
        { 0, 3, 0, 0 }, { 1, 2, 1, 0 }, { 2, 2, 1, 0 }, { 3, 2, 1, 0 },
        { 4, 2, 1, 0 }, { 5, 2, 1, 0 }, { 6, 2, 1, 0 }, { 7, 2, 1, 0 },
        { 8, 2, 1, 0 }, { 9, 2, 1, 0 }, { 10, 2, 1, 0 },
    };

    // Stage 10.
    const bcs::CAnmObjMotBase::NODE_EX_INFO sNodeExInfo10[] = {
        { 0, 2, 0, 0 }, { 1, 2, 1, 0 }, { 2, 2, 1, 0 }, { 3, 2, 1, 0 },
        { 5, 2, 1, 0 }, { 4, 2, 1, 0 }, { 6, 2, 1, 0 }, { 7, 2, 1, 0 },
        { 8, 2, 1, 0 }, { 9, 2, 1, 0 }, { 10, 2, 1, 0 }, { 11, 2, 1, 0 },
        { 12, 2, 1, 0 }, { 13, 2, 1, 0 }, { 14, 2, 1, 0 }, { 15, 2, 1, 0 },
        { 16, 2, 1, 0 }, { 17, 2, 1, 0 },
    };

    // Stage 11.
    const bcs::CAnmObjMotBase::NODE_EX_INFO sNodeExInfo11[] = {
        { 0, 3, 0, 0 }, { 1, 2, 1, 0 }, { 2, 2, 1, 0 }, { 3, 2, 1, 0 },
        { 4, 2, 1, 0 }, { 5, 2, 1, 0 }, { 6, 2, 1, 0 }, { 11, 2, 1, 0 },
        { 12, 2, 1, 0 }, { 13, 2, 1, 0 }, { 14, 2, 1, 0 }, { 7, 2, 1, 0 },
        { 8, 2, 1, 0 }, { 9, 2, 1, 0 }, { 10, 2, 1, 0 },
    };

    // Stage 15.
    const bcs::CAnmObjMotBase::NODE_EX_INFO sNodeExInfo15[] = {
        { 0, 3, 0, 0 }, { 1, 2, 1, 0 }, { 2, 2, 1, 0 }, { 3, 2, 1, 0 },
        { 4, 2, 1, 0 }, { 5, 2, 1, 0 }, { 6, 2, 1, 0 }, { 7, 2, 1, 0 },
        { 8, 2, 1, 0 },
    };

    // Stage 16.
    const bcs::CAnmObjMotBase::NODE_EX_INFO sNodeExInfo16[] = {
        { 0, 3, 0, 0 }, { 1, 2, 1, 0 }, { 2, 2, 1, 0 }, { 3, 3, 1, 0 },
        { 4, 2, 1, 0 }, { 5, 2, 1, 0 },
    };

    // Stage 17.
    const bcs::CAnmObjMotBase::NODE_EX_INFO sNodeExInfo17[] = {
        { 0, 2, 0, 0 }, { 1, 2, 1, 0 }, { 2, 2, 1, 0 }, { 3, 2, 1, 0 },
        { 4, 2, 1, 0 }, { 5, 2, 1, 0 },
    };

    // The node settings by stage; the table ends with stage -1.
    struct NODE_EX_INFO_TBL {
        long nStage;
        const bcs::CAnmObjMotBase::NODE_EX_INFO *pInfo;
    };

    const NODE_EX_INFO_TBL sNodeExInfoTbl[] = {
        { 1, sNodeExInfo01 },
        { 2, sNodeExInfo02 },
        { 3, sNodeExInfo03 },
        { 5, sNodeExInfo05 },
        { 6, sNodeExInfo06 },
        { 9, sNodeExInfo09 },
        { 10, sNodeExInfo10 },
        { 11, sNodeExInfo11 },
        { 15, sNodeExInfo15 },
        { 16, sNodeExInfo16 },
        { 17, sNodeExInfo17 },
        { -1, 0 },
    };
}

msw::CGoalMdlBasket::CGoalMdlBasket(CStgGmk *pParent) : CGoalMdl(pParent)
{
}

msw::CGoalMdlBasket::~CGoalMdlBasket()
{
}

bcs::CMdlObj *msw::CGoalMdlBasket::NewMdl(long heap) const
{
    bcs::CMdlObj *pMdl = 0;
    unsigned long idx = GetIndex();

    switch (idx) {
        case MDL_BOARD:
        case MDL_STAND:
            // The parent is looked up and not used (perhaps by code
            // compiled out of this build).
            GetParent();
            if (CGameCamera::IsBehindCamera()) {
                CMdlObj_Goal *pGoal = new (heap) CMdlObj_Goal;
                pGoal->SetUnk224(idx != MDL_BOARD);
                pGoal->SetUnk214And218(0.0f);
                pGoal->SetAlphaTestNearRange(2.0f);
                pGoal->SetAlphaTestFarRange(5.0f);
                pMdl = pGoal;
            } else if (idx == MDL_BOARD) {
                pMdl = new (heap) CMdlObj_Goal;
            }
            break;
        case MDL_FIGURE: {
            CFgMdl *pFg = new (heap) CFgMdl;
            SetFadeRange(pFg);
            pMdl = pFg;
            break;
        }
        default:
            pMdl = new (heap) bcs::CMdlObj;
            break;
    }
    return pMdl;
}

void msw::CGoalMdlBasket::Create(long res, const MDL_PARAM *pParam)
{
    unsigned long idx = GetIndex();
    if (idx == MDL_FIGURE || idx == MDL_STAND) {
        CStgMdl::Create(CGenericMVAGimmickCreator(CLoadData::GetRes(res), pParam));
    } else {
        CStgMdl::Create(CGenericGimmickCreator(CLoadData::GetRes(res), pParam));
    }
}

const bcs::CAnmObjMotBase::NODE_EX_INFO *msw::CGoalMdlBasket::GetNodeExInfo() const
{
    if (GetIndex() != MDL_BOARD) {
        return 0;
    }

    long stage = CGame::g_GameWork.m_nStage;
    for (const NODE_EX_INFO_TBL *p = sNodeExInfoTbl; p->nStage >= 0; p++) {
        if (p->nStage == stage) {
            return p->pInfo;
        }
    }
    return 0;
}

void msw::CGoalMdlBasket::Main(float rate)
{
    unsigned long idx = GetIndex();

    if (idx == MDL_BOARD || idx == MDL_STAND) {
        CStgObj *pParent = GetParent();
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pParent != 0 && pMdl != 0) {
            CMdlObj_Goal *pGoal = static_cast<CMdlObj_Goal *>(pMdl);
            if (CGameCamera::IsBehindCamera() && !(CGame::g_GameWork.m_nFlag18 & 2)) {
                pGoal->SetUnk22C(0);
            } else {
                pGoal->SetUnk22C(1);
            }
        }
    }

    if (idx == MDL_FIGURE || idx == MDL_STAND) {
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pMdl != 0) {
            SetFadeRange(static_cast<CFgMdl *>(pMdl));
        }
    }

    CGoalMdl::Main(rate);
}
