// msw::CComDrvHockeyGame's frame and message handlers, the frame's view of
// the field players (SetCommonData and its getters) and the simple moves.
// A file of its own, with its own float constants, ahead of
// game/hockey/unit_AFCA0.cpp; the file name is not known.
#include "game/msw/CComDrvHockeyGame.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CGame.hpp"

namespace unit_AE7E8_cpp {
// uncalled stand-in / pool-off stand-in: the original addresses every float
// constant of this file with its own lis (OnGameMsgProc reads three), which
// this compiler does when the first .rodata object of the file is weak (a
// function-local const). The module link strips this function and its
// constant.
void UnusedPoolOff()
{
    const long k = 0;
}
}

namespace {
    // (guessed helper) From fFrom towards fTo by fRate.
    inline float Blend(float fRate, float fFrom, float fTo)
    {
        return fRate * (fTo - fFrom) + fFrom;
    }
}

namespace msw {

long CComDrvHockeyGame::OnInit(_CHR_MSG *pMsg)
{
    long nRet = CComDrvHockey::OnInit(pMsg);

    m_nUnk42C = 0;
    m_nUnk39C = 0;
    m_nUnk3BC = 0;
    m_nUnk3C0 = 0;
    m_bLot380 = 1;
    m_bLot384 = 1;
    m_bLot388 = 1;
    m_fLotTime = 0.0f;
    m_fUnk358 = 0.0f;
    m_fUnk408 = 0.0f;
    m_nUnk35C = 0;
    m_fUnk360 = 0.0f;
    m_bRotY = 0;
    m_nUnk40C = 0;
    m_nUnk410 = 0;
    m_fShootWait = 0.0f;
    m_fUnk41C = 0.0f;
    m_fUnk420 = 0.0f;
    m_fUnk424 = 0.0f;
    m_nUnk428 = 0;

    long nChr = GetSpShootCharIdx();
    if (nChr >= 0) {
        m_nSpShootChr = nChr;
        m_nUnk458 = 1;
        m_nUnk45C = 0;
    } else {
        m_nSpShootChr = -1;
        m_nUnk458 = 0;
        m_nUnk45C = 0;
    }

    m_nUnk430 = 1;
    m_nUnk378 = 1;
    m_bSwitchBrake = 0;
    m_bSwitchBrakeLot = 0;
    m_nUnk36C = 0;
    m_nUnk370 = 0;
    m_fUnk374 = 0.0f;
    m_fUnk390 = 0.0f;
    m_nUnk394 = 0;
    m_fUnk3A0 = 0.0f;
    m_fUnk3A8 = 0.0f;
    m_fUnk3A4 = 0.0f;
    m_nPassChr = -1;
    m_nUnk464 = -1;
    ResetShotClock();
    SpCatchTimingInit();
    SetThkStatus(0);

    unsigned char nStage = CGame::g_GameWork.m_nStage;
    m_nStage = nStage;
    if (nStage > 0x11) {
        m_nStage = 0;
    }
    m_fUnk43C = CComMan::GetDatF(0xF64, 0L);
    return nRet;
}

// Every frame: counts the timers, draws the per-second rates, then, unless
// the character is busy, gathers the field and runs the action in progress
// (or thinks of the next one).
long CComDrvHockeyGame::OnMain(_CHR_MSG *pMsg)
{
    CComDrvHockey::OnMain(pMsg);
    float fLv = GetHockeyComLvF();
    m_bRotY = 0;

    float fTime = CComMan::GetDatF(0x111, fLv);
    if (GetElapsedTime() >= fTime) {
        m_bEarlyGame = 0;
    } else {
        m_bEarlyGame = 1;
    }

    if (m_fShootWait > 0.0f) {
        m_fShootWait -= m_pChr->GetFrameStep();
        if (m_fShootWait < 0.0f) {
            m_fShootWait = 0.0f;
        }
    }

    if (!isResetShotClock()) {
        if (!(m_nUnk354 & 1)) {
            m_fShotClock -= m_pChr->GetFrameStep();
            if (m_fShotClock <= 0.0f) {
                m_fShotClock = 0.0f;
            }
        }
        if (m_fUnk3F4 > 0.0f) {
            m_fUnk3F4 -= m_pChr->GetFrameStep();
            if (m_fUnk3F4 < 0.0f) {
                m_fUnk3F4 = 0.0f;
            }
        }
    } else {
        ResetShotClock();
    }

    // Once a second: draw the rates again.
    m_fLotTime += m_pChr->GetFrameStep();
    if (m_fLotTime >= 60.0f) {
        m_fLotTime = 0.0f;
        switch (CGame::g_GameWork.m_nStage) {
            case 7: {
                long nRate = CComMan::GetDatI(0x5DE, fLv);
                if (LotRate() < nRate) {
                    m_bLot380 = 1;
                } else {
                    m_bLot380 = 0;
                }
                break;
            }
            case 0xB: {
                long nRate = CComMan::GetDatI(0x5E6, fLv);
                if (LotRate() < nRate) {
                    m_bLot380 = 1;
                } else {
                    m_bLot380 = 0;
                }
                break;
            }
            case 0xC: {
                long nRate = CComMan::GetDatI(0x5EE, fLv);
                if (LotRate() < nRate) {
                    m_bLot380 = 1;
                } else {
                    m_bLot380 = 0;
                }
                break;
            }
            case 0x11: {
                long nRate = CComMan::GetDatI(0x5F6, fLv);
                if (LotRate() < nRate) {
                    m_bLot380 = 1;
                } else {
                    m_bLot380 = 0;
                }
                break;
            }
        }

        // The level's rate, scaled by the character's percentage.
        long nLvRate = CComMan::GetDatI(0x5CE, fLv);
        long nRate = CComMan::GetDatI(0x5B9, GetChrTypeID()) * nLvRate / 100;
        if (LotRate() < nRate) {
            m_bLot384 = 1;
        } else {
            m_bLot384 = 0;
        }
        nRate = CComMan::GetDatI(0x5D6, fLv);
        if (LotRate() < nRate) {
            m_bLot388 = 1;
        } else {
            m_bLot388 = 0;
        }
    }

    switch (m_pChr->GetState()) {
        case CChrCore::STATE_BALL_CATCH_BACK:
            return 0;
        case CChrCore::STATE_OFF_BALL_STANCE:
            if (!IsEntryAct(0x28) && !IsEntryAct(0x2B)) {
                return 0;
            }
            break;
        case CChrCore::STATE_CHARGE_CUT:
            return 0;
        case CChrCore::STATE_DAMAGE:
        case CChrCore::STATE_DIZZY:
        case CChrCore::STATE_SLIDING:
        case CChrCore::STATE_PASS:
        case CChrCore::STATE_PASS_CATCH:
        case CChrCore::STATE_SPIN_STAGGER:
            m_nUnk3FC = 0;
            m_bShootReq = 0;
            m_bItemShootReq = 0;
            m_nUnk428 = 0;
            break;
        case CChrCore::STATE_AERIAL_PASS:
            break;
    }

    if (GetRootActType() >= 0) {
        m_nLastRootActType = GetRootActType();
    }
    SetCommonData();
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

// Gathers the ball holder and the field players (keepers left out), all of
// them and by team.
void CComDrvHockeyGame::SetCommonData()
{
    m_pHoldBallCore = CComMan::GetHoldBallChrCore(-1);
    if (m_pHoldBallCore) {
        CChrCore *pHolder = m_pHoldBallCore;
        m_nHoldBallCoreIndex = pHolder->GetPlayerNo();
        m_HoldBallWorldPos = CComMan::GetChrWorldPos(pHolder);
        m_nHoldBallerTeamNo = m_pHoldBallCore->GetTeamNo();
    } else if (m_nSpShootChr >= 0) {
        // A special shot is on its way: its shooter counts as the holder.
        m_pHoldBallCore = CChrMan::GetChrCore(m_nSpShootChr);
        m_nHoldBallCoreIndex = m_nSpShootChr;
        m_HoldBallWorldPos = CComMan::GetChrWorldPos(m_pHoldBallCore);
        m_nHoldBallerTeamNo = m_pHoldBallCore->GetTeamNo();
    } else {
        m_nHoldBallCoreIndex = -1;
        m_HoldBallWorldPos.x = 0.0f;
        m_HoldBallWorldPos.y = 0.0f;
        m_HoldBallWorldPos.z = 0.0f;
        m_nHoldBallerTeamNo = -1;
    }

    long i;
    for (i = 0; i < 8; i++) {
        m_apCharaCore[i] = 0;
    }
    for (i = 0; i < 4; i++) {
        m_apFriendCore[i] = 0;
        m_apEnemyCore[i] = 0;
    }

    long nNum = CChrMan::GetChrCoreNum();
    long nFriend = 0;
    long nEnemy = 0;
    for (i = 0; i < nNum; i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr && !pChr->IsDisable() && !pChr->IsKeeper()) {
            long n = nEnemy + nFriend;
            m_apCharaCore[n] = pChr;
            m_aCharaWorldPos[n] = CComMan::GetChrWorldPos(pChr);
            m_aCharaNo[n] = i;
            m_aCharaIdx[n] = pChr->GetPlayerNo();
            if (pChr->GetTeamNo() != m_nMyTeam) {
                m_apEnemyCore[nEnemy] = pChr;
                m_aEnemyWorldPos[nEnemy] = m_aCharaWorldPos[n];
                m_aEnemyNo[nEnemy] = i;
                m_aEnemyIdx[nEnemy] = pChr->GetPlayerNo();
                nEnemy++;
            } else {
                m_apFriendCore[nFriend] = pChr;
                m_aFriendWorldPos[nFriend] = m_aCharaWorldPos[n];
                m_aFriendNo[nFriend] = i;
                m_aFriendIdx[nFriend] = pChr->GetPlayerNo();
                nFriend++;
            }
        }
    }
    m_nCharaNum = nEnemy + nFriend;
    m_nFriendNum = nFriend;
    m_nEnemyNum = nEnemy;
}

CChrCore *CComDrvHockeyGame::GetHoldBallCore() const
{
    return m_pHoldBallCore;
}

long CComDrvHockeyGame::GetHoldBallCoreIndex() const
{
    return m_nHoldBallCoreIndex;
}

long CComDrvHockeyGame::GetHoldBallerTeamNo() const
{
    return m_nHoldBallerTeamNo;
}

long CComDrvHockeyGame::GetCharaNum() const
{
    return m_nCharaNum;
}

long CComDrvHockeyGame::GetFriendNum() const
{
    return m_nFriendNum;
}

long CComDrvHockeyGame::GetEnemyNum() const
{
    return m_nEnemyNum;
}

CChrCore *CComDrvHockeyGame::GetCharaCore(long n) const
{
    return m_apCharaCore[n];
}

CChrCore *CComDrvHockeyGame::GetFriendCore(long n) const
{
    return m_apFriendCore[n];
}

CChrCore *CComDrvHockeyGame::GetEnemyCore(long n) const
{
    return m_apEnemyCore[n];
}

long CComDrvHockeyGame::GetMyTeamNo() const
{
    return m_nMyTeam;
}

bcs::FVEC3 CComDrvHockeyGame::GetFriendWorldPos(long n) const
{
    return m_aFriendWorldPos[n];
}

bcs::FVEC3 CComDrvHockeyGame::GetEnemyWorldPos(long n) const
{
    return m_aEnemyWorldPos[n];
}

long CComDrvHockeyGame::GetFriendIdx(long n) const
{
    return m_aFriendIdx[n];
}

long CComDrvHockeyGame::GetFriendNo(long n) const
{
    return m_aFriendNo[n];
}

long CComDrvHockeyGame::GetEnemyIdx(long n) const
{
    return m_aEnemyIdx[n];
}

int CComDrvHockeyGame::CheckEnemyCore(long nIdx) const
{
    for (long i = 0; i < m_nEnemyNum; i++) {
        if (nIdx == m_aEnemyIdx[i]) {
            return 1;
        }
    }
    return 0;
}

long CComDrvHockeyGame::OnDamage(_CHR_MSG *pMsg)
{
    if (GetRootActType() != 0x23) {
        m_nUnk370 = 0;
        m_nUnk36C = 0;
        Reset();
        // (meaning guessed) The kind of the damage, in the first word's
        // third byte; 7 knocks the character down.
        if (((unsigned long)pMsg->m_nParam0 >> 8 & 0xFF) == 7) {
            ActCallDamageFall();
        } else {
            ActCallDamage();
        }
    }
    return 0;
}

// The game's questions and news. (message numbers only; their names are
// not known)
long CComDrvHockeyGame::OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg)
{
    long nRet = CComDrv::OnGameMsgProc(id, pMsg);

    if (m_nUnk36C) {
        if (m_nUnk370 && id == 0x23) {
            if (m_pChr->IsHoldBall()) {
                nRet = 1;
            } else {
                m_nUnk370 = 0;
                m_nUnk36C = 0;
            }
        }
        return nRet;
    }

    switch (id) {
        case 0x7A:
            if (m_bRotY) {
                nRet = 1;
            }
            break;
        case 0x23:
            if (m_nUnk370) {
                if (m_pChr->IsHoldBall()) {
                    nRet = 1;
                } else {
                    m_nUnk370 = 0;
                }
            }
            break;
        case 0x62:
            if (!m_pChr->IsHoldBall()) {
                m_nUnk3FC = 0;
            }
            if (m_nUnk3FC) {
                nRet = 1;
            }
            break;
        case 0x73:
            nRet = GetShootCourse();
            break;
        case 0x1A:
            if (m_pChr->IsHoldBall() && GetShootKind() == 0 && m_fUnk3F4 <= 0.0f) {
                m_fShootWait = CComMan::GetDatF(0x119, GetHockeyComLvF());
                nRet = 1;
            }
            break;
        case 0x6D:
            if (m_pChr->IsHoldBall() && GetShootKind() == 1 && m_fUnk3F4 <= 0.0f) {
                m_fShootWait = CComMan::GetDatF(0x119, GetHockeyComLvF());
                nRet = 1;
            }
            break;
        case 0x70:
            if (m_pChr->IsHoldBall() && GetShootKind() == 2 && m_fUnk3F4 <= 0.0f) {
                m_fShootWait = CComMan::GetDatF(0x119, GetHockeyComLvF());
                nRet = 1;
            }
            break;
        case 0x65:
            // Whether a shot asked for on a pass still stands.
            if (!m_pBall) {
                m_bShootReq = 0;
                m_bItemShootReq = 0;
                break;
            }
            if (CGame::g_GameWork.m_nFlag18 & 1) {
                m_bShootReq = 0;
                m_bItemShootReq = 0;
                break;
            }
            if (!IsEntryAct(0x28) && !IsEntryAct(0x2B)) {
                if (m_pBall->GetState() == 2) {
                    long nHolder = CComMan::GetHoldBallChrIdx(-1);
                    if (nHolder < 0 || nHolder != m_nPassChr) {
                        m_bShootReq = 0;
                        m_bItemShootReq = 0;
                        break;
                    }
                    if (CChrMan::GetChrCore(nHolder)->IsHockeyFighting()) {
                        m_bShootReq = 0;
                        m_bItemShootReq = 0;
                        break;
                    }
                } else if (m_pBall->GetState() != 3) {
                    m_bShootReq = 0;
                    m_bItemShootReq = 0;
                    break;
                }
            }
            if (m_bShootReq == 1 || m_bItemShootReq == 1) {
                nRet = 1;
            }
            break;
        case 0x6F:
            if (m_bShootReq == 1) {
                m_bShootReq = 0;
                m_fShootWait = CComMan::GetDatF(0x119, GetHockeyComLvF());
                nRet = 1;
            }
            break;
        case 0x71:
            if (m_bItemShootReq == 1) {
                m_bItemShootReq = 0;
                m_fShootWait = CComMan::GetDatF(0x119, GetHockeyComLvF());
                nRet = 1;
            }
            break;
        case 0x72:
            PassMsgJobs(pMsg->m_nParam0, pMsg->m_nParam1);
            break;
        case 0x6:
            m_nUnk464 = pMsg->m_nParam0;
            break;
        case 0x57:
            m_nUnk3DC = 1;
            nRet = GetFaceOffPower();
            break;
        case 0x5C:
            ThkFaceoffStart(pMsg->m_nParam0, pMsg->m_nParam1);
            break;
        case 0x5A:
            m_nUnk3D0 = 1;
            m_nUnk3E0 = pMsg->m_nParam0;
            break;
        case 0x58:
            if (m_nUnk3CC) {
                nRet = 1;
            }
            break;
        case 0x59:
            if (m_nUnk3D4) {
                nRet = 1;
            }
            break;
        case 0x5B:
            m_nUnk3D8 = 1;
            m_fUnk3E4 = pMsg->m_nParam0;
            break;
        case 0x7C:
            m_nSpShootChr = pMsg->m_nParam0;
            m_nUnk458 = 1;
            m_nUnk45C = 0;
            break;
        case 0x7D:
            m_nSpShootChr = -1;
            m_nUnk458 = 0;
            m_nUnk45C = 0;
            Reset();
            break;
        case 0x21:
            if (GetChrTypeID() != 1 && m_nUnk428) {
                m_fUnk41C -= m_pChr->m_fFrameStep;
                if (m_fUnk41C <= 0.0f) {
                    nRet = 1;
                }
            }
            break;
        case 0x74:
            if (GetChrTypeID() != 1) {
                m_fUnk420 = (m_fUnk424 - m_fUnk420) / 2.0f + m_fUnk420;
                nRet = m_fUnk420;
            }
            break;
        case 0x84:
            if (isSpCatchSuccess()) {
                nRet = 1;
            }
            break;
        case 0x5D:
            nRet = GetFigthPower();
            break;
        case 0x56:
            if (m_nSpShootChr < 0) {
                long nChr;
                if (GetRootActType() != 0xB || !AreYouPass(0, nChr) || nChr != m_nMyChrIdx) {
                    Reset();
                }
            }
            break;
        case 0x52:
            Reset();
            break;
        case 0x55:
            Reset();
            break;
        case 0x19:
            if (m_pChr->IsHoldBall()) {
                nRet = 1;
            }
            break;
        case 0x28:
            m_pChr->AutoMoveTargetMarkerPos();
            nRet = 1;
            break;
        case 0x63:
            nRet = 1;
            break;
        case 0x64:
            nRet = 1;
            break;
        case 0x66:
            nRet = 1;
            break;
    }
    return nRet;
}

// A pass from nPasser to nReceiver. When the receiver is this character,
// near enough the goal and with a free course, it may ask for a quick shot
// (or item ball shot) on catching; otherwise it goes to get the puck.
void CComDrvHockeyGame::PassMsgJobs(long nPasser, long nReceiver)
{
    m_nPassChr = nPasser;
    if (m_pChr->IsHockeyDamage() == 1) {
        return;
    }
    if (nReceiver != m_nMyChrIdx) {
        return;
    }
    if (!CheckGoalArea(m_pChr, 0.0f)) {
        return;
    }
    if (CheckShootCourse(m_pChr, 0.0f)) {
        int bOutside = ButToShootOutsideShots();
        int bGimmick = ButToShootGimmickShots();
        if (bOutside == 1 && bGimmick == 1 && isQuickShootOK()) {
            long nRate;
            if (GetExistPadDrvNum() > 0) {
                nRate = CComMan::GetDatI(0x38F, GetHockeyComLvF());
            } else {
                nRate = CComMan::GetDatI(0x387, GetHockeyComLvF());
            }
            if (LotRate() < nRate && m_pChr->MethdOffBallStance(-1.0f, 0)) {
                Reset();
                if (CheckItemBallShootProbability()) {
                    m_bItemShootReq = 1;
                    m_bShootReq = 0;
                } else {
                    m_bItemShootReq = 0;
                    m_bShootReq = 1;
                }
                return;
            }
        }
    }
    if (GetRootActType() != 0xB) {
        Reset();
        ActCallGetBall(0);
    }
}

int CComDrvHockeyGame::MethdHockeyPass(const CChrCore *pChr)
{
    return m_pChr->MethdPass(const_cast<CChrCore *>(pChr), 0);
}

int CComDrvHockeyGame::MethdMove2(const bcs::FVEC3 &rDir, const bcs::FVEC3 &rPos, float fDis)
{
    float fRotY = m_pChr->GetRotY_ToPosXZ(rDir);
    float fNow = m_pChr->GetDist_ToPosXZ(rPos);
    if (isHitInvisibleWall(rDir)) {
        return 1;
    }
    if (fNow <= fDis) {
        return 1;
    }
    return MethdMoveMain(fRotY);
}

int CComDrvHockeyGame::MethdMove(const bcs::FVEC3 &rPos, float fDis)
{
    float fRotY = m_pChr->GetRotY_ToPosXZ(rPos);
    float fNow = m_pChr->GetDist_ToPosXZ(rPos);
    if (isHitInvisibleWall(rPos)) {
        return 1;
    }
    if (fNow <= fDis) {
        return 1;
    }
    return MethdMoveMain(fRotY);
}

}
