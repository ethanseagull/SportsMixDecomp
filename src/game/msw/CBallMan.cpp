#include "game/msw/CBallMan.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CGame.hpp"

// (DOL, unnamed; in game/msw/CStageMan.cpp) The ball colour of the stage
// being played.
extern "C" bcs::BCS_Color fn_80163CC8();

namespace msw {

    CBallMan *CBallMan::m_pInstance;

    CBallMan::CBallMan(bcs::CTask *pParent, long nBallMax)
        : bcs::CUnivTaskEx(pParent, 22),
          m_fWallBoundDecRate(0.625f),
          m_fFloorBoundDecRateXZ(0.875f),
          m_fFloorBoundDecRateY(0.75f),
          m_fFloorRoundDecRateXZ(0.9375f),
          m_fChrReflectSpeed(0.05f),
          m_StageBallColor(0),
          m_bStageBallColor(0),
          m_pBallHandle(0),
          m_nBallNum(nBallMax)
    {
        if (nBallMax > 0) {
            m_pBallHandle = new unsigned long[nBallMax];
            for (long i = 0; i < m_nBallNum; i++) {
                m_pBallHandle[i] = 0;
            }
        }
    }

    CBallMan::~CBallMan()
    {
        m_bStageBallColor = 0;
        m_StageBallColor = 0;
        DeleteAllBall_();
        if (m_pBallHandle != 0) {
            delete[] m_pBallHandle;
        }
        m_pBallHandle = 0;
        m_pInstance = 0;
    }

    CBallMan *CBallMan::GetInstance()
    {
        return m_pInstance;
    }

    int CBallMan::IsActive()
    {
        return m_pInstance != 0;
    }

    void CBallMan::Main()
    {
        if (m_pBallHandle != 0) {
            for (long i = 0; i < m_nBallNum; i++) {
                if (m_pBallHandle[i] != 0) {
                    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_pBallHandle[i]));
                    if (pBall != 0 && !pBall->IsDelete() && pBall->IsDeleteRequest()) {
                        pBall->Delete();
                    }
                }
            }
        }
        MainFunc_();
    }

    long CBallMan::GetActiveBallNum_()
    {
        long nActive = 0;
        if (m_pBallHandle != 0) {
            for (long i = 0; i < m_nBallNum; i++) {
                if (m_pBallHandle[i] != 0) {
                    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_pBallHandle[i]));
                    if (pBall != 0 && pBall->IsActive()) {
                        nActive++;
                    }
                }
            }
        }
        return nActive;
    }

    void CBallMan::DeleteAllBall_()
    {
        if (m_pBallHandle != 0) {
            for (long i = 0; i < m_nBallNum; i++) {
                if (m_pBallHandle[i] != 0) {
                    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_pBallHandle[i]);
                    m_pBallHandle[i] = 0;
                    if (pTask != 0 && !pTask->IsDelete()) {
                        pTask->Delete();
                    }
                }
            }
        }
    }

    long CBallMan::GetBallNum()
    {
        if (m_pInstance == 0) {
            return 0;
        }
        return m_pInstance->GetBallNum_();
    }

    long CBallMan::GetActiveBallNum()
    {
        if (m_pInstance == 0) {
            return 0;
        }
        return m_pInstance->GetActiveBallNum_();
    }

    unsigned long CBallMan::GetBallHandle(long nBall)
    {
        if (m_pInstance == 0) {
            return 0;
        }
        return m_pInstance->GetBallHandle_(nBall);
    }

    unsigned long CBallMan::GetBallHandle_(long nBall)
    {
        if (m_pBallHandle != 0) {
            return m_pBallHandle[nBall];
        }
        return 0;
    }

    CBall *CBallMan::GetBallPtr(long nBall)
    {
        if (m_pInstance == 0) {
            return 0;
        }
        return m_pInstance->GetBallPtr_(nBall);
    }

    CBall *CBallMan::GetBallPtr_(long nBall)
    {
        if (m_pBallHandle != 0) {
            return static_cast<CBall *>(bcs::CTaskMan::GetTask(m_pBallHandle[nBall]));
        }
        return 0;
    }

    const CBall *CBallMan::GetBallPtr_(long nBall) const
    {
        if (m_pBallHandle != 0) {
            return static_cast<CBall *>(bcs::CTaskMan::GetTask(m_pBallHandle[nBall]));
        }
        return 0;
    }

    CBall *CBallMan::GetNearPosBall(const bcs::FVEC3 &rPos)
    {
        if (m_pInstance == 0) {
            return 0;
        }
        return m_pInstance->GetNearPosBall_(rPos);
    }

    // The static wrappers from here to IsBallNowRebound are not in the game:
    // nothing called them and the link removed them. They are written because
    // the inline virtual functions they call are emitted right after them, in
    // the order the DOL has.
    CBall *CBallMan::GetFarPosBall(const bcs::FVEC3 &rPos)
    {
        if (m_pInstance == 0) {
            return 0;
        }
        return m_pInstance->GetFarPosBall_(rPos);
    }

    CBall *CBallMan::GetNearTargetPosBall(const bcs::FVEC3 &rPos)
    {
        if (m_pInstance == 0) {
            return 0;
        }
        return m_pInstance->GetNearTargetPosBall_(rPos);
    }

    CBall *CBallMan::GetFarTargetPosBall(const bcs::FVEC3 &rPos)
    {
        if (m_pInstance == 0) {
            return 0;
        }
        return m_pInstance->GetFarTargetPosBall_(rPos);
    }

    float CBallMan::GetWallBoundDecRate()
    {
        if (m_pInstance == 0) {
            return 1.0f;
        }
        return m_pInstance->m_fWallBoundDecRate;
    }

    float CBallMan::GetFloorBoundDecRateXZ()
    {
        if (m_pInstance == 0) {
            return 1.0f;
        }
        return m_pInstance->m_fFloorBoundDecRateXZ;
    }

    float CBallMan::GetFloorBoundDecRateY()
    {
        if (m_pInstance == 0) {
            return 1.0f;
        }
        return m_pInstance->m_fFloorBoundDecRateY;
    }

    float CBallMan::GetFloorRoundDecRateXZ()
    {
        if (m_pInstance == 0) {
            return 1.0f;
        }
        return m_pInstance->m_fFloorRoundDecRateXZ;
    }

    float CBallMan::GetChrReflectSpeed()
    {
        if (m_pInstance == 0) {
            return 1.0f;
        }
        return m_pInstance->m_fChrReflectSpeed;
    }

    float CBallMan::GetRaiseGoalInRate()
    {
        if (m_pInstance == 0) {
            return 0.0f;
        }
        return m_pInstance->GetRaiseGoalInRate_();
    }

    int CBallMan::IsInfinityReboundMode()
    {
        if (m_pInstance == 0) {
            return 0;
        }
        return m_pInstance->IsInfinityReboundMode_();
    }

    void CBallMan::SetInfinityReboundMode(int bOn)
    {
        if (m_pInstance != 0) {
            m_pInstance->SetInfinityReboundMode_(bOn);
        }
    }

    int CBallMan::IsDisableReboundMode()
    {
        if (m_pInstance == 0) {
            return 0;
        }
        return m_pInstance->IsDisableReboundMode_();
    }

    void CBallMan::SetDisableReboundMode(int bOn)
    {
        if (m_pInstance != 0) {
            m_pInstance->SetDisableReboundMode_(bOn);
        }
    }

    int CBallMan::IsDisableAirBallMode()
    {
        if (m_pInstance == 0) {
            return 0;
        }
        return m_pInstance->IsDisableAirBallMode_();
    }

    void CBallMan::SetDisableAirBallMode(int bOn)
    {
        if (m_pInstance != 0) {
            m_pInstance->SetDisableAirBallMode_(bOn);
        }
    }

    float CBallMan::GetPassFlyFrame(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, long nType)
    {
        if (m_pInstance == 0) {
            return 0.0f;
        }
        return m_pInstance->GetPassFlyFrame_(rFrom, rTo, nType);
    }

    float CBallMan::GetPassFlyFrame_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, long nType) const
    {
        const CBall *pBall = GetBallPtr_(0);
        if (pBall != 0) {
            return pBall->GetPassFlyFrame(rFrom, rTo, nType);
        }
        return 0.0f;
    }

    float CBallMan::GetShootFlyFrame_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, long nType) const
    {
        const CBall *pBall = GetBallPtr_(0);
        if (pBall != 0) {
            return pBall->GetShootFlyFrame(rFrom, rTo, nType);
        }
        return 0.0f;
    }

    int CBallMan::CanbeTurnOver()
    {
        if (m_pInstance == 0) {
            return 1;
        }
        return m_pInstance->CanbeTurnOver_();
    }

    int CBallMan::CanbeTurnOver_() const
    {
        const CBall *pBall = GetBallPtr_(0);
        if (pBall != 0) {
            return pBall->IsCanbeTurnOver();
        }
        return 1;
    }

    int CBallMan::IsBallPassMode_(long nBall)
    {
        CBall *pBall = GetBallPtr_(nBall);
        if (pBall != 0) {
            return pBall->IsPassMode();
        }
        return 0;
    }

    int CBallMan::IsBallShootMode_(long nBall)
    {
        CBall *pBall = GetBallPtr_(nBall);
        if (pBall != 0) {
            return pBall->IsShootMode();
        }
        return 0;
    }

    int CBallMan::IsBallInvincible_(long nBall)
    {
        CBall *pBall = GetBallPtr_(nBall);
        if (pBall != 0) {
            return pBall->IsInvincible();
        }
        return 0;
    }

    int CBallMan::IsBallScored_(long nBall)
    {
        CBall *pBall = GetBallPtr_(nBall);
        if (pBall != 0) {
            return pBall->IsScored();
        }
        return 0;
    }

    void CBallMan::SetBallPos(long nBall, const bcs::FVEC3 *pPos)
    {
        if (m_pInstance != 0) {
            m_pInstance->SetBallPos_(nBall, pPos);
        }
    }

    void CBallMan::SetBallPos_(long nBall, const bcs::FVEC3 *pPos)
    {
        CBall *pBall = GetBallPtr_(nBall);
        if (pBall != 0) {
            pBall->SetPos(pPos);
        }
    }

    void CBallMan::SetJumpBallMode(long nBall)
    {
        if (m_pInstance != 0) {
            m_pInstance->SetJumpBallMode_(nBall);
        }
    }

    void CBallMan::SetJumpBallMode_(long nBall)
    {
        CBall *pBall = GetBallPtr_(nBall);
        if (pBall != 0) {
            pBall->SetJumpBallMode();
        }
    }

    int CBallMan::IsBallNowRebound(long nBall)
    {
        if (m_pInstance == 0) {
            return 0;
        }
        return m_pInstance->IsBallNowRebound_(nBall);
    }

    void CBallMan::SetSleepMode_(long nBall)
    {
        CBall *pBall = GetBallPtr_(nBall);
        if (pBall != 0) {
            pBall->SetSleepMode();
        }
    }

    void CBallMan::SetSleepModeAll_()
    {
        if (m_pBallHandle != 0) {
            for (long i = 0; i < m_nBallNum; i++) {
                CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_pBallHandle[i]));
                if (pBall != 0 && !pBall->IsDelete()) {
                    pBall->SetSleepMode();
                }
            }
        }
    }

    void CBallMan::Create()
    {
        if (m_pInstance != 0) {
            m_pInstance->SetStageBallColor_();
            m_pInstance->Create_();
        }
    }

    void CBallMan::AllReset()
    {
        if (m_pInstance != 0) {
            m_pInstance->AllReset_();
        }
    }

    void CBallMan::AllReset_()
    {
        if (m_pBallHandle != 0) {
            for (long i = 0; i < m_nBallNum; i++) {
                if (m_pBallHandle[i] != 0) {
                    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_pBallHandle[i]));
                    if (pBall != 0 && !pBall->IsDelete()) {
                        if (pBall->IsDestroyOnReset()) {
                            pBall->Delete();
                            m_pBallHandle[i] = 0;
                        } else {
                            pBall->Reset();
                        }
                    }
                }
            }
        }
    }

    void CBallMan::AllSleep()
    {
        if (m_pInstance != 0) {
            m_pInstance->AllSleep_();
        }
    }

    void CBallMan::AllSleep_()
    {
        long nBallNum = GetBallNum_();
        for (long i = 0; i < nBallNum; i++) {
            CBall *pBall = GetBallPtr_(i);
            if (pBall != 0 && !pBall->IsDelete()) {
                pBall->ChangeState(0);
            }
        }
    }

    CBall *CBallMan::AddBall(CBall *pBall)
    {
        if (m_pInstance != 0) {
            return m_pInstance->AddBall_(pBall);
        }
        return 0;
    }

    CBall *CBallMan::AddBall_(CBall *pBall)
    {
        long nFree = -1;
        for (long i = 0; i < m_nBallNum; i++) {
            if (m_pBallHandle[i] == 0) {
                nFree = i;
                break;
            }
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_pBallHandle[i]);
            if (pTask == 0 || pTask->IsDelete()) {
                m_pBallHandle[i] = 0;
                nFree = i;
                break;
            }
        }
        CBall *pAdded = 0;
        if (nFree >= 0) {
            m_pBallHandle[nFree] = pBall->GetId();
            pBall->SetBallNo(nFree);
            pAdded = pBall;
        }
        return pAdded;
    }

    void CBallMan::DelBall(long nBall)
    {
        if (m_pInstance != 0) {
            m_pInstance->DelBall_(nBall);
        }
    }

    void CBallMan::DelBall_(long nBall)
    {
        if (m_pBallHandle[nBall] != 0) {
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_pBallHandle[nBall]);
            m_pBallHandle[nBall] = 0;
            if (pTask != 0 && !pTask->IsDelete()) {
                pTask->Delete();
            }
        }
    }

    int CBallMan::IsExistStarBall()
    {
        if (m_pInstance != 0) {
            return m_pInstance->IsExistStarBall_();
        }
        return 0;
    }

    int CBallMan::IsExistStarBall_() const
    {
        if (m_pBallHandle != 0) {
            for (long i = 0; i < m_nBallNum; i++) {
                if (m_pBallHandle[i] != 0) {
                    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_pBallHandle[i]));
                    if (pBall != 0 && !pBall->IsDelete() && !pBall->IsSleep() && pBall->IsStarBall()) {
                        return 1;
                    }
                }
            }
        }
        return 0;
    }

    void CBallMan::SetStageBallColor_()
    {
        CGame::GameWork &rWork = CGame::g_GameWork;
        m_bStageBallColor = 0;
        if (rWork.GetGameType() == 0 && rWork.m_nGameMode <= 3) {
            m_StageBallColor = fn_80163CC8();
            m_bStageBallColor = 1;
        }
    }

    int CBallMan::GetStageBallColor(bcs::BCS_Color &rColor)
    {
        if (m_pInstance != 0) {
            return m_pInstance->GetStageBallColor_(rColor);
        }
        return 0;
    }

    int CBallMan::GetStageBallColor_(bcs::BCS_Color &rColor)
    {
        if (m_bStageBallColor != 0) {
            rColor = m_StageBallColor;
            return 1;
        }
        return 0;
    }
}
