// msw::CStageHockey05: hockey stage 05. The stage scrolls (a CStageScroll
// member does the work) and its models are turned with the scroll.
#include "game/msw/CStageHockey05.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/New.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdl_Raft.hpp"
#include <stdio.h>

// Ids of the model table.
enum {
    MDL_SCROLL = 1, // "null_scrN": the models of scroll entry N
    MDL_RAFT = 2
};

msw::CStageHockey05::CStageHockey05(bcs::CTask *pParent) : CStageManHockey(pParent), m_Scroll(this)
{
}

msw::CStageHockey05::~CStageHockey05()
{
}

void msw::CStageHockey05::Main_(float frame)
{
    m_Scroll.Main(frame);

    // Every model that is not marked otherwise turns with the scroll.
    for (long i = 0; i < GetMdlNum(); i++) {
        CStageMdl *pMdl = GetMdlByIdx(i);
        if (pMdl != 0 && pMdl->GetUnk2C() == 0) {
            bcs::CScnObj *pObj = pMdl->GetScnObj();
            if (pObj != 0) {
                pObj->SetRotXYZ(m_Scroll.GetRotX(), m_Scroll.GetRotY(), 0.0f);
            }
        }
    }

    CStageManHockey::Main_(frame);
}

// The long goes on to the allocation, as in the other model factories.
msw::CStageMdl *msw::CStageHockey05::NewMdl_Raft(CStageMan *pStage, long n, bcs::CRegex &)
{
    return new (n) CStageMdl_Raft(pStage);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageHockey05::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^null_scr([0-9]+)", MDL_SCROLL, 0, 0, NewMdl },
        { "^null_wavelog", MDL_RAFT, 0, 0, NewMdl_Raft },
        { 0, reinterpret_cast<long>(CStageManHockey::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

// The "null_scrN" models are not created here: scroll entry N builds them.
bool msw::CStageHockey05::OnPreCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                         nw4r::g3d::ResFile file, CArn &rArn, long n)
{
    if (pRule->m_nId == MDL_SCROLL) {
        long no;
        if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0) {
            m_Scroll.CreateMdl(no, file, rArn, n);
        }
        return false;
    }
    return true;
}

void msw::CStageHockey05::OnSceneEntry()
{
    m_Scroll.OnSceneEntry();
}
