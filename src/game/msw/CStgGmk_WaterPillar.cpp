#include "game/msw/CStgGmk_WaterPillar.hpp"
#include "bcs/New.hpp"

// One water pillar: a stage gimmick that hands everything to its actor.
// Part of the same original file as CStgGmk_WaterPillar_Mdl.cpp (see there).

namespace msw {

    CStgGmk_WaterPillar::CStgGmk_WaterPillar(bcs::CTask *pParent, int) : CStgGmk(pParent, 1)
    {
    }

    CStgGmk_WaterPillar::~CStgGmk_WaterPillar()
    {
    }

    CStgMdl *CStgGmk_WaterPillar::NewMdl(long heap, unsigned long idx)
    {
        return new (heap) CStgGmk_WaterPillar_Mdl(this, idx);
    }

    void CStgGmk_WaterPillar::OnPostCreate()
    {
    }

    void CStgGmk_WaterPillar::OnSceneEntry()
    {
    }

    void CStgGmk_WaterPillar::OnSceneLeave()
    {
    }

    void CStgGmk_WaterPillar::Main()
    {
        GetFrameRateF();
        CStgObj::Main();
    }

    void CStgGmk_WaterPillar::Respawn()
    {
        GetAct();
    }

    // State 1 starts the water (the actor's START state, name guessed).
    void CStgGmk_WaterPillar::Activate()
    {
        CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0 && !pAct->IsActive()) {
            pAct->FsmqChangeState(1);
        }
    }

    // State 6 ends it.
    void CStgGmk_WaterPillar::Inactivate()
    {
        CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0 && pAct->IsActive()) {
            pAct->FsmqChangeState(6);
        }
    }

    void CStgGmk_WaterPillar::Start()
    {
        CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            pAct->ForceStart();
        }
    }

    void CStgGmk_WaterPillar::Stop()
    {
        CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            pAct->ForceStop();
        }
    }

    void CStgGmk_WaterPillar::Reset()
    {
        CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            pAct->ForceReset();
        }
    }

    void CStgGmk_WaterPillar::SetAuto(int bAuto)
    {
        CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            pAct->SetAuto(bAuto);
        }
    }

    bool CStgGmk_WaterPillar::IsSwoosh() const
    {
        bool bSwoosh = false;
        const CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            bSwoosh = pAct->IsSwoosh();
        }
        return bSwoosh;
    }

    int CStgGmk_WaterPillar::IsAuto() const
    {
        int bAuto = 0;
        const CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            bAuto = pAct->IsAuto();
        }
        return bAuto;
    }

    bool CStgGmk_WaterPillar::IsWarning() const
    {
        const CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            return pAct->IsWarning();
        }
        return false;
    }

    void CStgGmk_WaterPillar::Disp()
    {
    }
}
