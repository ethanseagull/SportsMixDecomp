#include "game/msw/CEventDemo.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CEventCamera.hpp"
#include "game/msw/CEventDemoBase.hpp"
#include "game/msw/CGame.hpp"

// Defined further down this file, outside the part written so far: the
// function Cleanup queues for the end of the fade. Its body is Cleanup(0)
// inlined. (unnamed in the DOL)
extern "C" void fn_8011AEC4();

namespace msw {

    // Whether event demos are played outside the case CGame::g_GameWork
    // decides (name guessed; .sdata, starts at 1).
    static int s_bEnable = 1;

    // The order in which the attract mode shows the four game kinds
    // (SetRandomDemoGameMode shuffles it; name guessed).
    static signed char s_anDemoGameMode[4];

    int CEventDemo::IsEnable()
    {
        if (CGame::g_GameWork.m_Unk151 != 0) {
            return 1;
        }
        return s_bEnable;
    }

    void CEventDemo::Cleanup(int bWaitFade)
    {
        if (bWaitFade) {
            CEventDemoBase::AddFadeEndFunc(fn_8011AEC4);
            return;
        }

        for (int i = 0; i < 4; i++) {
            bcs::CTask *pTask = m_Work.apTask[i];
            if (pTask != 0) {
                pTask->Delete();
            }
            m_Work.apTask[i] = 0;
            m_Work.anFlags[i] = 0;
        }
    }

    long CEventDemo::GetRandomDemoGameMode(int bNext)
    {
        long nMode = s_anDemoGameMode[0];

        if (bNext) {
            s_anDemoGameMode[0] = s_anDemoGameMode[1];
            s_anDemoGameMode[1] = s_anDemoGameMode[2];
            s_anDemoGameMode[2] = s_anDemoGameMode[3];
            s_anDemoGameMode[3] = nMode;
        }
        return nMode;
    }

    void CEventDemo::SetRandomDemoGameMode()
    {
        long anMode[4];
        anMode[0] = 0;
        anMode[1] = 1;
        anMode[2] = 2;
        anMode[3] = 3;

        int i;
        long nLeft;
        for (i = 0, nLeft = 4; nLeft > 1; i++) {
            long nPick = bcs::BCS_RandMax(nLeft - 1);
            nLeft--;
            s_anDemoGameMode[i] = anMode[nPick];
            anMode[nPick] = anMode[nLeft];
        }
        s_anDemoGameMode[i] = anMode[0];
    }

    void CEventDemo::SetFlags(unsigned long nFlags)
    {
        for (int i = 0; i < 4; i++) {
            m_Work.anFlags[i] |= nFlags;
        }
    }

    void CEventDemo::SendMessage(long nArg0, long nArg1, long nArg2, long nArg3)
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (pDemo != 0) {
            pDemo->SendMessage(nArg0, nArg1, nArg2, nArg3);
        }
    }

    void CEventDemo::SetupDemoCamera(const char *pName, long nTeam)
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (pDemo != 0) {
            pDemo->m_pCamera->Setup(pName, nTeam);
        }
    }

    void CEventDemo::SetDemoCameraInfo(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rTarget, long nSide)
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (pDemo != 0) {
            CEventCameraBase *pCamera = pDemo->m_pCamera;
            pCamera->m_vPos.x = rPos.x;
            pCamera->m_vPos.y = rPos.y;
            pCamera->m_vPos.z = rPos.z;
            pCamera->m_vTarget.x = rTarget.x;
            pCamera->m_vTarget.y = rTarget.y;
            pCamera->m_vTarget.z = rTarget.z;
            if (nSide != 0) {
                pCamera->m_fUnk508 = 3.1415927f;
            }
        }
    }

    void CEventDemo::PlayDemoCamera(int nMode)
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (pDemo != 0) {
            CEventCameraBase *pCamera = pDemo->m_pCamera;
            pCamera->m_nPlayMode = nMode;
            pCamera->Play(1.0f);
        }
    }

    int CEventDemo::IsEndDemoCamera()
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (pDemo == 0) {
            return 1;
        }
        return pDemo->m_pCamera->m_nState > 0;
    }

    void CEventDemo::SetEndDemoCamera()
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (pDemo != 0) {
            CEventCameraBase *pCamera = pDemo->m_pCamera;
            pCamera->m_nFrameMode = 0;
            pCamera->Stop(2);
        }
    }

    void CEventDemo::SetDemoCameraParam(float fParam)
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (pDemo != 0) {
            pDemo->m_pCamera->m_fParam = fParam;
        }
    }

    void CEventDemo::SetDemoCameraFrame(float fFrame, int nMode)
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (pDemo != 0) {
            CEventCameraBase *pCamera = pDemo->m_pCamera;
            pCamera->m_fFrame = fFrame;
            pCamera->m_nFrameMode = nMode;
        }
    }
}
