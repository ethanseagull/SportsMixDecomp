// msw::CComDrvDodgeGame's frame, message and alarm handlers. A file of its
// own after game/dodge/unit_9D2C4.cpp, with its own float constants; the
// file name is not known.
#include "game/msw/CComDrvDodge.hpp"
#include "game/msw/CChrCoreDodge.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComMan.hpp"

namespace {
    inline float DegToRad(float fDeg)
    {
        return fDeg * (3.1415927f / 180.0f);
    }

    // Set by the file's static initialiser and used by nothing in the file
    // (most game module files start their .bss with such an angle; see
    // CChrMarkerDodge.cpp).
    float s_fAngle60 = DegToRad(60.0f);

    // (guessed helper) v brought into lo..hi.
    inline long ClampI(long v, long lo, long hi)
    {
        return v > hi ? hi : (v < lo ? lo : v);
    }
}

namespace msw {

// (guessed helper) Whether the driven character holds the ball.
inline int CComDrvDodgeGame::IsChrHoldBall() const
{
    return m_pChr->IsHoldBall();
}

// Every frame: nothing while the character is in a combination or passing;
// otherwise runs the action in progress, or thinks of the next one.
long CComDrvDodgeGame::OnMain(_CHR_MSG *pMsg)
{
    if (static_cast<CChrCoreDodge *>(m_pChr)->IsNowCombinationMode()) {
        Reset();
        return 0;
    }
    CComDrv::OnMain(pMsg);
    UpdateAssignOuterChr();
    long nState = m_pChr->GetState();
    if (nState >= CChrCore::STATE_PASS && nState <= CChrCore::STATE_ARTS_PASS) {
        return 0;
    }
    if (nState == CChrCore::STATE_JUMP_SHOOT && !IsChrHoldBall()) {
        return 0;
    }
    ActInterrupt();
    long nAct = GetCurrentActType();
    if (nAct >= 0) {
        if (ActMain()) {
            ActFunc func = g_ActFuncMap[nAct];
            (this->*func)();
        }
    } else {
        ThkMain();
    }
    return 0;
}

long CComDrvDodgeGame::OnInit(_CHR_MSG *pMsg)
{
    long nRet = CComDrv::OnInit(pMsg);
    UpdateHomePos();
    bcs::FVEC3 pos;
    GetHomePos(pos);
    return nRet;
}

// The game's questions and news. (message numbers only; their names are
// not known)
long CComDrvDodgeGame::OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg)
{
    long nRet = CComDrv::OnGameMsgProc(id, pMsg);
    switch (id) {
        case 0x10:
            if (GetRootActType() == 0x2A) {
                Reset();
            }
            break;
        case 0x45:
            if (GetRootActType() != 0x1D) {
                Reset();
            }
            break;
        case 0x13:
        case 0x52:
            Reset();
            break;
        case 0xE: {
            // A character got the ball.
            m_nUnk228 = 0;
            m_nUnk25C = 0;
            m_fUnk260 = 0.0f;
            SetGetBallWait(1.0f);
            FeintInit();
            SetAssignOuterChr();
            long nChr = pMsg->m_nParam1;
            CChrCore *pChr = CChrMan::GetChrCore(nChr);
            if (pChr) {
                if (nChr == m_nMyChrIdx) {
                    m_nUnk20C = 0;
                    m_nShootKind = 0;
                    if (m_pChr->IsDodgeInfield()) {
                        Reset();
                        ActCallInFieldCatchWait();
                    } else {
                        Reset();
                        ActCallOutFieldCatchWait();
                    }
                } else if (m_pChr->IsDodgeInfield() &&
                           (pChr->IsDodgeInfield() || m_nMyTeam != pChr->GetTeamNo())) {
                    Reset();
                }
            }
            break;
        }
        case 0x44:
            nRet = GetStanceResult();
            break;
        case 0x46:
            if (m_pChr->IsHoldBall() && GetStanceResult()) {
                nRet = 1;
            }
            break;
        case 0x3B:
        case 0x3D:
        case 0x3E: {
            long nMin = CComMan::GetDatI(0x89, GetComLvF());
            long nMax = CComMan::GetDatI(0x91, GetComLvF());
            nMin = ClampI(nMin, 0, 0x400);
            nMax = ClampI(nMax, 0, 0x400);
            nRet = RandRangeI(nMin, nMax);
            break;
        }
        case 0x6: {
            // A pass.
            long nChr = pMsg->m_nParam1;
            LotPassStealRate();
            if (m_nMyChrIdx == nChr) {
                Reset();
                ActCallPassCatch();
            }
            break;
        }
        case 0x7: {
            // A shot.
            long nParam = pMsg->m_nParam1;
            CChrCore *pChr = CChrMan::GetChrCore(pMsg->m_nParam0);
            if (pChr && m_nMyTeam != pChr->GetTeamNo()) {
                bcs::FVEC3 shooterPos = CComMan::GetChrWorldPos(pChr);
                m_fShooterDis = CComMan::GetDisXZ(shooterPos, m_MyWorldPos);
                LotDodgeRollRate(nParam);
                LotShootDefStance();
            }
            if (pChr && m_nMyTeam == pChr->GetTeamNo() && pChr->IsDodgeInfield() &&
                !m_pChr->IsDodgeInfield() && m_nMyChrIdx == Util_GetNearMyTeamOuterChr(m_Unk238)) {
                Reset();
                ActCallOutFieldRunCatchPos();
            }
            break;
        }
        case 0x48: {
            long nFeint = LotFeintDamage(pMsg->m_nParam0);
            if (nFeint >= 0) {
                Reset();
                ActCallFeintReaction(nFeint);
            }
            break;
        }
        case 0x3F:
            if (m_pChr->IsDodgeInfield()) {
                nRet = GetCatchResult(pMsg->m_nParam0, pMsg->m_nParam1);
                Reset();
            }
            break;
        case 0x21: {
            long nRate = pMsg->m_nParam1 * 100 / pMsg->m_nParam0;
            if (nRate >= CComMan::GetDatI(0x7CE, GetComLvF())) {
                if (!m_bUnk258) {
                    bcs::FVEC3 pos;
                    if (GetSpTargetMarkerPos(pos)) {
                        long nChr = GetNearEmInnerChr(pos, 1, 0);
                        if (nChr >= 0 && GetDisXZ(CComMan::GetChrPosXZ(nChr), pos) < 1.0f) {
                            nRet = 1;
                        }
                    }
                } else if (RandMaxI(120) == 0) {
                    nRet = 1;
                }
            }
            break;
        }
        case 0x49:
            if (!IsExistPadDrv() && m_nShootKind) {
                nRet = 1;
            }
            break;
        case 0x4B:
            nRet = GetLockPosValue(m_fUnk1F0);
            break;
        case 0x4C:
            if (GetRootActType() != 0x1D) {
                nRet = 1;
            }
            break;
        case 0x4E:
            LotDivingCatchRate();
            break;
        case 0x18:
            // A special shot started.
            if (GetLastSpChrCore()) {
                m_nUnk254 = 1;
                Reset();
                if (m_pChr->IsDodgeInfield()) {
                    if (GetLastSpChrCore() == m_pChr) {
                        LotRateSpBefore();
                        WraperActCallOffSpStart();
                    } else if (m_nMyTeam == GetLastSpChrCore()->GetTeamNo()) {
                        WraperActCallOffSpSub();
                    } else {
                        WraperActCallDefSpStart();
                    }
                }
            }
            break;
        case 0x23:
            if (!IsExistPadDrv() && m_nUnk25C) {
                if (m_fUnk260 < 0.0f) {
                    nRet = 1;
                    m_fUnk260 += m_pChr->m_fFrameStep;
                }
            }
            break;
    }
    return nRet;
}

void CComDrvDodgeGame::OnAlarm0(long nAlarm)
{
    UpdateHomePos();
    SetAlarm(nAlarm, 120.0f);
}

void CComDrvDodgeGame::OnAlarm1(long nAlarm)
{
    m_nEscGimmick = 1;
    SetAlarm(nAlarm, 3600.0f);
}

// Every two seconds: draws again what the driver escapes.
void CComDrvDodgeGame::OnAlarm2(long nAlarm)
{
    m_bEscItemBomb = 0;
    if (LotRate() < CComMan::GetDatI(0xBE, GetComLvF())) {
        m_bEscItemBomb = 1;
    }
    m_bEscItem = 0;
    if (LotRate() < CComMan::GetDatI(0xC6, GetComLvF())) {
        m_bEscItem = 1;
    }
    m_bEscObstacle = 0;
    if (LotRate() < CComMan::GetDatI(0xC68, GetComLvF())) {
        m_bEscObstacle = 1;
    }
    m_bUnk22F = 0;
    if (LotRate() < CComMan::GetDatI(0xC70, GetComLvF())) {
        m_bUnk22F = 1;
    }
    m_bUnk230 = 0;
    if (LotRate() < CComMan::GetDatI(0xB6, GetComLvF())) {
        m_bUnk230 = 1;
    }
    if (m_nUnk228 >= 0) {
        if (LotRate() < CComMan::GetDatI(0x7B6, GetComLvF())) {
            m_nUnk228 = 1;
        }
    }
    SetAlarm(nAlarm, 120.0f);
}

}
