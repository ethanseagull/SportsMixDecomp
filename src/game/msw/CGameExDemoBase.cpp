#include "game/msw/CGameExDemoBase.hpp"
#include "game/msw/CGameExDemoMaskObj.hpp"
#include "bcs/CCamera.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/New.hpp"

namespace msw {

    CGameExDemoBase::CGameExDemoBase(bcs::CTask *pParent) : bcs::CTask(pParent, -1) {
        m_nStep = 0;
        m_Unk38 = 0;
        m_fUnk3C = 0.0f;
        m_pMask = 0;

        m_pMask = new (0x100) CGameExDemoMaskObj();
        if (m_pMask != 0) {
            bcs::CRenderMan::PushBack(m_pMask, 8);
        }

        for (int i = 0; i < CAMERA_NUM; i++) {
            m_apCamera[i] = 0;
            m_afFovy[i] = 40.0f;
        }
    }

    CGameExDemoBase::~CGameExDemoBase() {
        for (int i = 0; i < CAMERA_NUM; i++) {
            if (m_apCamera[i] != 0) {
                m_apCamera[i]->Delete();
                m_apCamera[i] = 0;
            }
        }
        if (m_pMask != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMask);
        }
        m_pMask = 0;
    }

    void CGameExDemoBase::Main() {
        MainProc();
        for (int i = 0; i < CAMERA_NUM; i++) {
            if (m_apCamera[i] != 0) {
                bcs::CWorldEnv::SetViewFovy(i, m_afFovy[i]);
            }
        }
    }

    void CGameExDemoBase::MainProc() {}

    int CGameExDemoBase::CanbeNext() {
        return m_nStep == 1;
    }

}
