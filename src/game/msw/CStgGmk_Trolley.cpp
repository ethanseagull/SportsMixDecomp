#include "game/msw/CStgGmk_Trolley.hpp"
#include "game/msw/CStgGmk_Trolley_Act.hpp"
#include "game/msw/CStgGmk_Trolley_Mdl.hpp"
#include "bcs/New.hpp"

// The trolley of stage 17. In the original one file holds the model
// (CStgGmk_Trolley_Mdl, from 0x802219F0), the actor
// (CStgGmk_Trolley_Act), the driver (CStgGmk_Trolley_Drv) and the gimmick
// (CStgGmk_Trolley), with their weak functions at the end of .text (to
// 0x80226894) and the vtables in one .data block (0x80489318 to
// 0x80489C80). Evidence: the gimmick's NewMdl inlines the model's
// constructor, the weak NewDrv the driver's, and the getters of the
// gimmick the actor calls are emitted between the actor's functions.
//
// Only the driver and the gimmick are written so far, with the model's
// constructor that NewMdl needs.

namespace msw {

    CStgGmk_Trolley_Mdl::CStgGmk_Trolley_Mdl(CStgGmk *pParent, long idx)
        : CStgMdl(pParent), m_nMdlIdx(idx), m_pUnk134(0)
    {
        long n = 2;
        if (idx == 0) {
            n = 0;
        }
        m_Unk130 = n;
    }

    CStgGmk_Trolley_Drv::CStgGmk_Trolley_Drv(CStgGmk *pParent) : CStgDrv(pParent) {}

    CStgGmk_Trolley_Drv::~CStgGmk_Trolley_Drv() {}

    void CStgGmk_Trolley_Drv::Main(float rate)
    {
        CStgDrv::Main(rate);
    }

    CStgGmk_Trolley::CStgGmk_Trolley(bcs::CTask *pParent, int arg)
        : CStgGmk(pParent, 0x14), m_Unk68(arg), m_fDefaultRotY(0.0f), m_fRunDist(0.0f)
    {
        SetType(13);
        m_DefaultPos.Set(0.0f, 0.0f, 0.0f);
    }

    CStgGmk_Trolley::~CStgGmk_Trolley() {}

    CStgMdl *CStgGmk_Trolley::NewMdl(long heap, unsigned long idx)
    {
        return new (heap) CStgGmk_Trolley_Mdl(this, idx);
    }

    void CStgGmk_Trolley::OnPostCreate() {}

    void CStgGmk_Trolley::OnSceneEntry() {}

    void CStgGmk_Trolley::OnSceneLeave() {}

    void CStgGmk_Trolley::Main()
    {
        GetFrameRateF();
        CStgObj::Main();
    }

    void CStgGmk_Trolley::Respawn()
    {
        CStgGmk_Trolley_Act *pAct = GetTrolleyAct();
        if (pAct) {
            pAct->FsmqChangeState(CStgGmk_Trolley_Act::STATE_RESPAWN);
        }
    }

    int CStgGmk_Trolley::IsActive() const
    {
        const CStgGmk_Trolley_Act *pAct = GetTrolleyAct();
        if (pAct) {
            return pAct->IsAlive();
        }
        return 0;
    }

    void CStgGmk_Trolley::Inactivate()
    {
        CStgGmk_Trolley_Act *pAct = GetTrolleyAct();
        if (pAct) {
            pAct->FsmqChangeState(CStgGmk_Trolley_Act::STATE_INACTIVE);
        }
    }

    void CStgGmk_Trolley::Disp() {}
}
