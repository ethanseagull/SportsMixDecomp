// msw::CStageBasket13: basket stage 13, whose "null_scr" models turn slowly
// about Y. DodgeF, HockeyF and VolleyF have the same stage on their own
// stage manager; the basket-specific parts are the base class
// (CStageManBasket) and the class name.
#include "game/msw/CStageBasket13.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/CRegex.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CStageMdl.hpp"
#include <stdio.h>

// (guessed helper) rad brought into [0, 2 pi].
static inline float NormalizeRad(float rad)
{
    while (rad > 2.0f * 3.14159265f) {
        rad -= 2.0f * 3.14159265f;
    }
    while (rad < 0.0f) {
        rad += 2.0f * 3.14159265f;
    }
    return rad;
}

msw::CStageBasket13::CStageBasket13(bcs::CTask *pParent) : CStageManBasket(pParent), m_Unk218(0)
{
}

msw::CStageBasket13::ROT_MDL::ROT_MDL()
{
}

msw::CStageBasket13::ROT_MDL::~ROT_MDL()
{
}

msw::CStageBasket13::~CStageBasket13()
{
}

void msw::CStageBasket13::Main_(float frame)
{
    for (unsigned long i = 0; i < ROT_MDL_NUM; i++) {
        UpdateMdl(i, frame);
    }
    CStageManBasket::Main_(frame);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageBasket13::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageManBasket.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^null_scr([0-9]+)", 1, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageManBasket::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageBasket13::OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CMdlPtrPool::Handle &rHandle, CArn &rArn, long n)
{
    if (pRule->m_nId != 1) {
        return;
    }

    long no;
    if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0 && no >= 1 && no <= 4) {
        ROT_MDL &rMdl = m_aRotMdl[no];
        // The original clears the handle before copying the new one in
        // (two zero stores, then the copy); maybe a reset helper of the
        // handle that is not known yet.
        rMdl.m_Handle = CMdlPtrPool::Handle();
        rMdl.m_Handle = rHandle;
        rMdl.m_fPeriod = 7800.0f * no + 9000.0f;
        rMdl.m_fTime = rMdl.m_fPeriod * bcs::BCS_RandF();
    }
}

void msw::CStageBasket13::UpdateMdl(unsigned long i, float frame)
{
    ROT_MDL &rMdl = m_aRotMdl[i];
    if (!rMdl.m_Handle.IsValid()) {
        return;
    }

    rMdl.m_fTime += frame;
    while (rMdl.m_fTime > rMdl.m_fPeriod) {
        rMdl.m_fTime -= rMdl.m_fPeriod;
    }
    while (rMdl.m_fTime < 0.0f) {
        rMdl.m_fTime += rMdl.m_fPeriod;
    }
    float fRotY = NormalizeRad(2.0f * 3.14159265f * (rMdl.m_fTime / rMdl.m_fPeriod));

    if (rMdl.m_Handle.IsValid() && (*rMdl.m_Handle.Get())->GetScnObj() != 0) {
        bcs::CScnObj *pObj = (*rMdl.m_Handle.Get())->GetScnObj();
        pObj->SetRotY(fRotY);
        pObj->CalcRootMtx();
    }
}

void msw::CStageBasket13::OnSceneEntry()
{
}
