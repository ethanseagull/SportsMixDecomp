// msw::CChrDrvPadBasket: the driver of a basketball character played with a
// pad, and the stick trackers it uses (msw::BASKET::CStickState and kin).
#include "game/msw/CChrDrvPadBasket.hpp"

#include <math.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/CWifiPad.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CGame.hpp"

// Set by the file's static initialiser and used by nothing in the file, as
// in about 60 other files of the game modules (see CBallManBasket.cpp).
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);
}

namespace {
    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // (names guessed) An angle brought into [0, 2 pi), and into (-pi, pi].
    inline float RoundRad(float fRad)
    {
        while (fRad > PI2) {
            fRad -= PI2;
        }
        while (fRad < 0.0f) {
            fRad += PI2;
        }
        return fRad;
    }

    inline float RoundRadPI(float fRad)
    {
        fRad = RoundRad(fRad);
        if (fRad > PI) {
            fRad -= PI2;
        }
        return fRad;
    }
}

long msw::CChrDrvPadBasket::m_nTrgStroke[42];

msw::CChrDrvPadBasket::CChrDrvPadBasket()
{
    m_pStick2Tap = new BASKET::CStickState2Tap;
    m_pStickRoll = new BASKET::CStickStateRoll;
    m_fCrossOnTime = 0.0f;
    m_fCrossOffTime = 0.0f;
    m_fCrossLastDir = 0.0f;
    m_bCross2Tap = 0;
    m_nCrossFlag = 0;
    m_nCmdFlag = 0;
    m_nKeyHold = 0;
    m_nKeyHoldDone = 0;
    m_nKeyLong = 0;
    for (int i = 0; i < 42; i++) {
        m_nTrgStroke[i] = 0;
    }
}

msw::BASKET::CStickState::~CStickState() {}

msw::CChrDrvPadBasket::~CChrDrvPadBasket()
{
    if (m_pStickRoll != 0) {
        delete m_pStickRoll;
    }
    m_pStickRoll = 0;
    if (m_pStick2Tap != 0) {
        delete m_pStick2Tap;
    }
    m_pStick2Tap = 0;
}

long msw::CChrDrvPadBasket::OnMsg(MSG_ID id, _CHR_MSG *pMsg)
{
    long nRet = 0;

    switch (id) {
        case 0:
            nRet = OnMsgMain(pMsg);
            break;
        case 1:
            ClearWork_();
            break;
        case 3:
            ClearWork_();
            break;
        case 4:
            ClearWork_();
            break;
        case 0x27:
            nRet = 0;
            break;
        case 7:
            if (pMsg != 0 && m_pChr != 0 && pMsg->m_nParam0 == m_pChr->GetPlayerNo()) {
                m_nUnk94 = 0;
                m_nFlag84 &= ~0xC;
            }
            break;
        case 9:
            break;
        case 0xA:
            if (m_pChr != 0) {
                VibStart_(1, 0xB, 1);
            }
            break;
        case 0xD:
            m_nFlag84 &= ~0xC;
            if (IsPressKeyID(0)) {
                m_nFlag84 |= 8;
            }
            m_nUnk94 = 0;
            VibStart_(1, 0, 1);
            break;
        case 0x19:
            if (IsPressKeyID(0)) {
                nRet = 1;
            }
            break;
        case 0x1A:
            nRet = IsCmd_(0x10, 0, 0, 1);
            break;
        case 0x1B:
            nRet = 1024.0f * bcs::CWifiPad::GetScratchPower(m_pChr->GetPadNo());
            break;
        case 0x1E:
            m_nFlag84 |= 1;
            m_nFlag84 &= ~0xC;
            if (IsPressKeyID(0)) {
                m_nFlag84 |= 0x10;
            }
            break;
        case 0x20: {
            unsigned long nState;

            nRet = -1;
            nState = m_pStickRoll->GetState();
            if (nState & 8) {
                nRet = 0;
            } else if (nState & 0x10) {
                nRet = 1;
            }
            if (IsCmd_(0xF, 0, 1, 0)) {
                nRet = 2;
            }
            break;
        }
        case 0x2B:
            if (IsCmd_(0x10, 0, 0, 1)) {
                nRet = 4;
            }
            break;
        case 0x2C:
            nRet = 1;
            break;
        case 0x21:
            if (IsCmd_(0x10, 0, 0, 1)) {
                nRet = 4;
            }
            break;
        case 0x22:
            if (pMsg != 0) {
                nRet = CheckArtsChrInput(pMsg);
            }
            break;
        case 0x28:
            if (m_fStickDir >= 0.0f) {
                float fRot = m_fStickDir + m_pChr->m_fRotY;
                if (m_pChr->GetChrId() == 0xE) {
                    fRot += PI + CGame::GetGoalYRot(m_pChr->GetTargetGoalIdx());
                }
                fRot = RoundRad(fRot);
                m_pChr->MoveTargetMarkerPos(fRot);
                nRet = 1;
            }
            break;
        case 0x37:
            m_nFlag84 &= ~0xC;
            if (IsPressKeyID(0)) {
                m_nFlag84 |= 0x10;
            }
            m_nUnk94 = 0;
            break;
        case 0x38:
            if (m_nStroke != -1) {
                ClearStroke_(0.0f);
                nRet = 1;
            }
            break;
        case 0x3A:
            if (IsPressKeyID(3)) {
                nRet = 1;
            }
            break;
        case 0x26:
        case 0x3B:
        case 0x3D:
        case 0x3E: {
            float fPower;

            nRet = 0;
            fPower = bcs::CWifiPad::GetScratchPower(m_pChr->GetPadNo());
            if (fPower > 0.2f) {
                nRet = 1024.0f * fPower;
            }
            break;
        }
        case 0x3C:
            if (m_nStroke != -1) {
                ClearStroke_(0.0f);
            }
            nRet = m_nStroke;
            break;
        case 0x25:
            nRet = IsCmd_(0xF, 0, 1, 0);
            break;
        case 0x53:
            if (pMsg != 0) {
                VibStart_(1, pMsg->m_nParam0, pMsg->m_nParam1);
            }
            break;
        case 0x29:
            nRet = GetChr()->Methd1on1(0);
            break;
        case 0x2A:
            nRet = IsCmd_(1, 0, 0x13, 1);
            break;
        case 0x2D:
            nRet = 0;
            if (m_fStickDir >= 0.0f) {
                float fDiff;

                if (RoundRad(m_fStickDir) <= PI) {
                    nRet = 2;
                } else {
                    nRet = 4;
                }
                fDiff = RoundRadPI(CGame::GetGoalYRot(m_pChr->GetTargetGoalIdx()) - m_pChr->m_fRotY);
                if (fabsf(fDiff) < PI / 2) {
                    if (nRet == 2) {
                        nRet = 4;
                    } else {
                        nRet = 2;
                    }
                }
            }
            break;
        case 0x2E:
            VibStart_(1, 9, 1);
            break;
        case 0x23:
            nRet = IsCmd_(0x28, 0, 0x24, 0);
            break;
        case 999:
            nRet = 1;
            break;
    }
    return nRet;
}

void msw::CChrDrvPadBasket::UpDateCross2Tap()
{
    // The second push counts when it is within 20 degrees of the first.
    float fRange = 0.34906584f;

    if (m_fStickDir >= 0.0f) {
        if (!(m_nCrossFlag & 0x40)) {
            float fLast = m_fCrossLastDir;
            m_fCrossLastDir = m_fStickDir;
            if (fLast != -1.0f && m_fCrossOffTime < 8.0f) {
                float fDiff = RoundRadPI(m_fStickDir - fLast);
                m_bCross2Tap = fabsf(fDiff) < fRange;
            }
            m_nCrossFlag |= 0x40;
        }
        m_fCrossOffTime = 0.0f;
        m_fCrossOnTime += m_pChr->m_fFrameStep;
        if (m_fCrossOnTime > 8.0f) {
            m_bCross2Tap = 0;
            m_fCrossLastDir = -1.0f;
            m_fCrossOnTime = 8.0f;
        }
    } else {
        if (m_nCrossFlag & 0x40) {
            m_nCrossFlag &= ~0x40;
        }
        m_fCrossOnTime = 0.0f;
        m_bCross2Tap = 0;
        m_fCrossOffTime += m_pChr->m_fFrameStep;
        if (m_fCrossOffTime > 8.0f) {
            m_fCrossOffTime = 8.0f;
            m_fCrossLastDir = -1.0f;
        }
    }
}

long msw::CChrDrvPadBasket::OnMsgMain(_CHR_MSG *pMsg)
{
    float fFrame;

    if (m_pChr == 0) {
        return 0;
    }
    UpdatePadState();
    fFrame = m_pChr->m_fFrameStep;
    m_pStickRoll->UpDate(m_fStickDir, m_fStickPower, fFrame);
    m_pStick2Tap->UpDate(m_fStickDir, m_fStickPower, fFrame);
    if ((IsTrgA() || IsPressA()) && (IsTrgB() || IsPressB())) {
        m_nCmdFlag |= 2;
    }
    UpDateMyKeyState(0, IsPressA());
    UpDateMyKeyState(1, IsPressB());
    UpDateMyKeyState(2, IsPressC());
    UpDateMyKeyState(3, IsPressZ());
    UpDateMyKeyState(4, bcs::CWifiPad::IsDown(0x200, m_nPadNo));
    UpDateMyKeyState(5, bcs::CWifiPad::IsDown(0x100, m_nPadNo));
    if (!IsCmd_(0x28, 0, 0x24, 0)) {
        m_nCmdFlag &= ~1;
    }
    UpDateCross2Tap();
    OnMsgMain_(pMsg);
    if (!IsTrgA() && !IsPressA() && !IsTrgB() && !IsPressB()) {
        m_nCmdFlag &= ~2;
    }
    if (!IsCmd_(0x28, 0, 0x24, 0)) {
        m_nCmdFlag &= ~4;
    }
    return 0;
}

void msw::BASKET::CStickState::UpDate(float, float fPower, float)
{
    if (fPower >= m_fThreshold) {
        if (m_nState == 0) {
            m_nState = 1;
        } else if (m_nState & 1) {
            m_nState = 2;
        }
    } else if (m_nState & 3) {
        m_nState = 4;
    } else {
        m_nState = 0;
    }
}

long msw::CChrDrvPadBasket::OnMsgMain_(_CHR_MSG *pMsg)
{
    if (GetChr()->IsAutoChrChangeToPassCatch()) {
        long nState = m_pChr->GetState();
        if (nState == 0x15 || nState == 0x16) {
            return 0;
        }
    }
    if (CGame::g_GameWork.GetGameType() == 2) {
        if (m_pChr->IsHoldBall()) {
            MainOffense_(pMsg);
        } else {
            SubDefNormal_();
        }
    } else {
        if (((m_pChr->GetStatusFlag() & 1) || CGame::g_GameWork.m_nUserTeam == m_pChr->GetTeamNo()) &&
            !(CGame::g_GameWork.m_nFlag18 & 1)) {
            MainOffense_(pMsg);
        } else {
            SubDefNormal_();
        }
    }
    return 0;
}

void msw::CChrDrvPadBasket::MainOffense_(_CHR_MSG *)
{
    if (ChgLeader_()) {
        return;
    }
    if (m_pChr->GetStatusFlag() & 1) {
        if (m_pChr->GetStatusFlag() & 0x8000) {
            if (m_fStickDir >= 0.0f) {
                float fRot = RoundRad(m_pChr->m_fRotY + m_fStickDir);
                m_pChr->MethdSliding(fRot);
                m_pChr->MethdRun(fRot);
            }
        } else if (m_pChr->IsStepCatchDunk()) {
            if (IsYokoStroke_()) {
                if (m_pChr->MethdAerialPass(GetInputRotY_(), 0)) {
                    ClearStroke_(0.0f);
                }
            }
        } else if (!m_pChr->IsStepAirTrick()) {
            if (m_pChr->IsStepNormalShoot()) {
                MethdPass();
            } else if (m_pChr->IsStepPassCatch()) {
                MethdPass();
            } else if (m_pChr->IsStepTrickMove()) {
                if (!MethdShoot()) {
                    MethdPass();
                }
            } else if (m_pChr->IsStepCatch() || m_pChr->GetState() == 0x36) {
                if (!MethdShoot() && !MethdPass() && !MethdTrickMove() && m_fStickDir >= 0.0f) {
                    CChrCore::INPUT_WORK work;
                    float fRot;

                    work.m_nUnk00 = 0;
                    fRot = RoundRad(m_pChr->m_fRotY + m_fStickDir);
                    if (!m_pChr->MethdRDB(&work, fRot, m_fStickPower)) {
                        m_pChr->MethdCatchToSDB();
                    }
                }
            } else if (m_pChr->GetState() == 0x28) {
                MethdPass();
            } else if (m_pChr->GetState() == 8) {
                m_pChr->MethdItemShoot(GetMoveRotY_());
            } else {
                SubOffDribble_();
            }
        }
        if (!m_pChr->IsStepAirTrick()) {
            m_nFlag84 &= ~3;
        }
    } else {
        SubOffNotHaveBall_();
    }
}

void msw::CChrDrvPadBasket::SubOffDribble_()
{
    if (!MethdShoot() && !MethdPass() && !MethdTrickMove() && m_fStickDir >= 0.0f) {
        int b2Tap;

        if (m_nPadType == 0) {
            b2Tap = m_bCross2Tap;
        } else {
            b2Tap = (m_pStick2Tap->GetState() >> 5) & 1;
        }
        if (b2Tap) {
            CChrCoreBasket *pChr = GetChr();
            if (pChr->GetState() != 0x3C && pChr->MethdDash()) {
                return;
            }
        }

        CChrCoreBasket *pChr = GetChr();
        float fRot = RoundRad(pChr->m_fRotY + m_fStickDir);
        if (!pChr->MethdAutoDPM(fRot, m_fStickPower)) {
            CChrCore::INPUT_WORK work;

            work.m_nUnk00 = 0;
            m_pChr->MethdRDB(&work, fRot, m_fStickPower);
        }
    }
}

void msw::CChrDrvPadBasket::SubOffNotHaveBall_()
{
    float fRotY;

    if (m_pChr->GetFlagEC() & 0x0A000000) {
        return;
    }
    fRotY = m_pChr->m_fRotY;
    if (m_fStickDir >= 0.0f) {
        m_pChr->MethdSetMoveRevise(RoundRad(fRotY + m_fStickDir));
    }
    if (IsCmd_(1, 0, 2, 0) && m_pChr->MethdItemShoot(GetMoveRotY_())) {
        return;
    }
    if (m_pChr->GetStatusFlag() & 0x40400) {
        if (!(IsCmd_(0x28, 0, 0x24, 0) && PlayCombination_())) {
            if (IsCmd_(0x1C, 0, 0x18, 0)) {
                m_pChr->MethdPassRequest();
            }
        }
        return;
    }
    if (IsCmd_(0x23, 0, 0, 1)) {
        if (m_pChr->MethdReboundDunk()) {
            ClearStroke_(0.0f);
            return;
        }
        if (GetChr()->MethdAlleyoop()) {
            ClearStroke_(0.0f);
            return;
        }
    }
    if (IsJumpCmd_()) {
        float fRot = -1.0f;
        if (m_fStickDir >= 0.0f) {
            fRot = RoundRad(fRotY + m_fStickDir);
        }
        if (GetChr()->MethdJumpBlock(fRot)) {
            ClearStroke_(15.0f);
            return;
        }
    }
    if (!(IsCmd_(0x28, 0, 0x24, 0) && PlayCombination_())) {
        if (IsCmd_(0x1C, 0, 0x18, 0)) {
            m_pChr->MethdPassRequest();
        }
    }
    if (m_fStickDir >= 0.0f) {
        float fRot = RoundRad(m_pChr->m_fRotY + m_fStickDir);
        m_pChr->MethdSliding(fRot);
        m_pChr->MethdRun(fRot);
    }
}

void msw::CChrDrvPadBasket::SubDefNormal_()
{
    float fRotY;

    if (m_pChr->GetFlagEC() & 0x0A000000) {
        return;
    }
    if (ChgLeader_()) {
        return;
    }
    if (m_pChr->GetState() == 8) {
        m_pChr->MethdItemShoot(GetMoveRotY_());
        return;
    }
    fRotY = m_pChr->m_fRotY;
    if (m_fStickDir >= 0.0f) {
        m_pChr->MethdSetMoveRevise(RoundRad(fRotY + m_fStickDir));
    }
    if (IsCmd_(1, 0, 2, 0) && m_pChr->MethdItemShoot(GetMoveRotY_())) {
        return;
    }
    if (m_pChr->GetStatusFlag() & 0x40400) {
        if (IsCmd_(0x10, 0, 0, 1) && m_pChr->MethdJumpUpper()) {
            ClearStroke_(0.0f);
        }
        return;
    }
    if (IsJumpCmd_()) {
        float fRot;

        if (m_pChr->MethdReboundDunk()) {
            ClearStroke_(0.0f);
            return;
        }
        fRot = -1.0f;
        if (m_fStickDir >= 0.0f) {
            fRot = RoundRad(fRotY + m_fStickDir);
            m_pChr->MethdSetMoveRevise(fRot);
        }
        if (GetChr()->MethdJumpBlock(fRot)) {
            ClearStroke_(15.0f);
            return;
        }
    }
    if (m_pChr->IsHoldItem() != 1 && IsCmd_(0xF, 0, 1, 0)) {
        float fRot = -1.0f;
        if (m_fStickDir >= 0.0f) {
            fRot = RoundRad(fRotY + m_fStickDir);
        }
        if (m_pChr->MethdBlockCut(fRot)) {
            return;
        }
    }
    if (m_fStickDir >= 0.0f) {
        float fRot = RoundRad(m_pChr->m_fRotY + m_fStickDir);
        m_pChr->MethdSliding(fRot);
        m_pChr->MethdRun(fRot);
    }
}

int msw::CChrDrvPadBasket::CheckCmd_(long nCmd, long nStroke) const
{
    int bKey;
    int bStroke;

    if (nCmd == 0 && nStroke == 0) {
        return 0;
    }
    bKey = 1;
    if (nCmd != 0) {
        int bEnable = 1;

        // While A and B are pressed together, the commands of A or B alone
        // do not count.
        if (m_nCmdFlag & 2) {
            switch (nCmd) {
                case 1:
                case 2:
                case 0x12:
                case 0x13:
                case 0x18:
                case 0x19:
                case 0x1E:
                case 0x1F:
                case 0x24:
                case 0x25:
                    bEnable = 0;
                    break;
            }
        }
        if (bEnable) {
            switch (nCmd) {
                case 0x12:
                    bKey = IsPressA();
                    break;
                case 0x13:
                    bKey = IsPressB();
                    break;
                case 0x14:
                    bKey = IsPressC();
                    break;
                case 0x15:
                    bKey = IsPressZ();
                    break;
                case 0x16:
                    bKey = bcs::CWifiPad::IsDown(0x200, m_nPadNo);
                    break;
                case 0x17:
                    bKey = bcs::CWifiPad::IsDown(0x100, m_nPadNo);
                    break;
                case 0x18:
                case 0x19:
                case 0x1A:
                case 0x1B:
                case 0x1C:
                case 0x1D:
                    bKey = IsRelease_(nCmd);
                    break;
                case 0x1E:
                    bKey = m_nKeyHold & 1;
                    break;
                case 0x1F:
                    bKey = (m_nKeyHold >> 1) & 1;
                    break;
                case 0x20:
                    bKey = (m_nKeyHold >> 2) & 1;
                    break;
                case 0x21:
                    bKey = (m_nKeyHold >> 3) & 1;
                    break;
                case 0x22:
                    bKey = (m_nKeyHold >> 4) & 1;
                    break;
                case 0x23:
                    bKey = (m_nKeyHold >> 5) & 1;
                    break;
                case 0x24:
                    bKey = m_nKeyLong & 1;
                    break;
                case 0x25:
                    bKey = (m_nKeyLong >> 1) & 1;
                    break;
                case 0x26:
                    bKey = (m_nKeyLong >> 2) & 1;
                    break;
                case 0x27:
                    bKey = (m_nKeyLong >> 3) & 1;
                    break;
                case 0x28:
                    bKey = (m_nKeyLong >> 4) & 1;
                    break;
                case 0x29:
                    bKey = (m_nKeyLong >> 5) & 1;
                    break;
                default:
                    bKey = CheckTrg(nCmd);
                    break;
            }
        } else {
            bKey = 0;
        }
    }
    bStroke = 1;
    if (nStroke != 0) {
        bStroke = CheckStroke(nCmd, nStroke);
    }
    if (bKey && bStroke) {
        return 1;
    }
    return 0;
}

int msw::CChrDrvPadBasket::CheckTrg(long nCmd) const
{
    switch (nCmd) {
        case 0:
            return 0;
        case 8:
            return IsTrgKeyID(4);
        case 1:
            return IsTrgA();
        case 2:
            return IsTrgB();
        case 3:
            return IsTrgC();
        case 4:
            return IsTrgZ();
        case 5: {
            int bRet = 0;
            if ((IsTrgA() && IsPressB()) || (IsPressA() && IsTrgB())) {
                bRet = 1;
            }
            return bRet;
        }
        case 6: {
            int bRet = 0;
            if ((IsTrgA() && IsPressZ()) || (IsPressA() && IsTrgZ())) {
                bRet = 1;
            }
            return bRet;
        }
        case 7: {
            int bRet = 0;
            if ((IsTrgB() && IsPressZ()) || (IsPressB() && IsTrgZ())) {
                bRet = 1;
            }
            return bRet;
        }
        case 9:
            return GetCrossDir_() == 1;
        case 10:
            return GetCrossDir_() == 3;
        case 11:
            return GetCrossDir_() == 2;
        case 12:
            return GetCrossDir_() == 4;
        case 13:
            return bcs::CWifiPad::IsTrg(0x1000, m_nPadNo);
        case 14:
            return bcs::CWifiPad::IsTrg(0x10, m_nPadNo);
        case 15:
            return bcs::CWifiPad::IsTrg(0x200, m_nPadNo);
        case 16:
            return bcs::CWifiPad::IsTrg(0x100, m_nPadNo);
        case 17: {
            int bRet = 0;
            if ((bcs::CWifiPad::IsTrg(0x200, m_nPadNo) && bcs::CWifiPad::IsDown(0x100, m_nPadNo)) ||
                (bcs::CWifiPad::IsDown(0x200, m_nPadNo) && bcs::CWifiPad::IsTrg(0x100, m_nPadNo))) {
                bRet = 1;
            }
            return bRet;
        }
    }
    return 0;
}

int msw::CChrDrvPadBasket::CheckStroke(long nCmd, long nStroke) const
{
    long nKeys = 0;

    for (long i = 1; i < 42; i++) {
        if (m_nTrgStroke[i] != 0 && nCmd != i) {
            if (IsPress_(i)) {
                nKeys |= m_nTrgStroke[i];
            }
        }
    }
    if (nKeys != 0 && (nStroke == 1 || nStroke == 2)) {
        if (nKeys == 1) {
            nStroke = 5;
        } else if (nKeys == 2) {
            nStroke = 4;
        } else {
            nStroke = 0;
        }
    }
    return CheckStrokeRaw(nStroke);
}

int msw::CChrDrvPadBasket::CheckStrokeRaw(long nStroke) const
{
    switch (nStroke) {
        case 1:
            return IsRemoconStroke(0.45f);
        case 2:
            if (m_pChr != 0) {
                return bcs::CWifiPad::IsTateStroke(m_pChr->GetPadNo());
            }
            return 0;
        case 3:
            if (m_pChr != 0) {
                return bcs::CWifiPad::IsYokoStroke(m_pChr->GetPadNo());
            }
            return 0;
        case 4:
            return m_nStroke == 3;
        case 5:
            return m_nStroke == 0;
        case 6:
            return m_nStroke == 1;
        case 7:
            return m_nStroke == 2;
        case 8:
            return bcs::CWifiPad::IsNunchukStroke(0.65f, m_pChr->GetPadNo());
        case 9:
            return bcs::CWifiPad::IsBothHandStroke(0.2f, 0.2f, m_pChr->GetPadNo());
        case 10:
            return (unsigned long)m_nStroke <= 2;
    }
    return 0;
}

int msw::CChrDrvPadBasket::MethdPass()
{
    CChrCoreBasket *pChr = GetChr();

    if (pChr->GetState() == 0x25) {
        return 0;
    }
    if (pChr->GetState() == 0x28) {
        int bAerial = 0;
        if (pChr->m_nBasketFlag & 0x800) {
            bAerial = IsCmd_(0x1C, 0, 0x18, 0);
        }
        if (bAerial) {
            float fRot = GetInputRotY_();
            if ((m_nCmdFlag & 4) && GetChr()->MethdCombinationPass(fRot)) {
                m_nCmdFlag &= ~4;
                return 1;
            }
            if (m_pChr->MethdAerialPass(fRot, 0)) {
                return 1;
            }
        } else if (!(pChr->m_nBasketFlag & 0x800)) {
            return 0;
        }
    }
    if (IsCmd_(0x28, 0, 0x24, 0) && PlayCombination_()) {
        m_nCmdFlag |= 4;
        return 1;
    }
    if (!IsCmd_(0x1C, 0, 0x18, 0)) {
        return 0;
    }
    float fRot = GetInputRotY_();
    if ((m_nCmdFlag & 4) && GetChr()->MethdCombinationPass(fRot)) {
        m_nCmdFlag &= ~4;
        return 1;
    }
    if (m_pChr->MethdAerialPass(fRot, 0)) {
        return 1;
    }
    return m_pChr->MethdPass(fRot, 0) != 0;
}

int msw::CChrDrvPadBasket::MethdTrickMove()
{
    if (m_nPadType == 0) {
        if (IsCmd_(0, 1, 4, 0)) {
            float fSide;

            if (m_fStickDir >= 0.0f) {
                float fDiff = RoundRadPI(m_pChr->m_fRotY + m_fStickDir - m_pChr->GetRotY_ToGoal());
                if (fDiff < 0.0f) {
                    fSide = -1.0f;
                } else {
                    fSide = 1.0f;
                }
            } else {
                // No stick: turn away from the farthest-off blocker.
                float fMax = 0.0f;
                float fGoal = m_pChr->GetRotY_ToGoal();
                for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                    const CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr != 0 && !pChr->IsDisable() && pChr->IsNowBlockWork()) {
                        float fDiff = RoundRadPI(m_pChr->GetRotY_ToPosXZ(pChr->GetPos()) - fGoal);
                        if (fabsf(fDiff) > fabsf(fMax)) {
                            fMax = fDiff;
                        }
                    }
                }
                if (fMax > 0.0f) {
                    fSide = -1.0f;
                } else {
                    fSide = 1.0f;
                }
            }
            if (m_pChr->MethdTrickMove(fSide)) {
                return 1;
            }
        }
        return 0;
    }

    float fSide = 0.0f;
    unsigned long nState = m_pStickRoll->GetState();
    if ((nState & 8) || (nState & 0x10)) {
        if (nState & 8) {
            fSide = -1.0f;
        } else {
            fSide = 1.0f;
        }
    } else if (IsCmd_(0, 1, 4, 0)) {
        if (m_fStickDir >= 0.0f) {
            float fDiff = RoundRadPI(m_pChr->m_fRotY + m_fStickDir - m_pChr->GetRotY_ToGoal());
            if (fDiff < 0.0f) {
                fSide = -1.0f;
            } else {
                fSide = 1.0f;
            }
        } else if (bcs::BCS_RandMax(1L)) {
            fSide = -1.0f;
        } else {
            fSide = 1.0f;
        }
    }
    if (fSide != 0.0f && m_pChr->MethdTrickMove(fSide)) {
        return 1;
    }
    return 0;
}

int msw::CChrDrvPadBasket::MethdShoot()
{
    int bItem;

    if (IsCmd_(0x11, 0, 5, 0) && m_pChr->MethdAirTrick()) {
        ClearStroke_(0.0f);
        return 1;
    }
    if (!m_pChr->IsHaveStockItem()) {
        bItem = 0;
    } else {
        bItem = IsCmd_(1, 0, 0x13, 1);
    }
    if (IsCmd_(0x23, 0, 0, 1) || bItem) {
        float fRot = -1.0f;
        if (m_fStickDir >= 0.0f) {
            fRot = RoundRad(m_fStickDir + m_pChr->m_fRotY);
            if (bItem && m_pChr->IsHaveStockItem() && m_pChr->MethdJumpShoot(fRot, bItem)) {
                ClearStroke_(15.0f);
                return 1;
            }
            if (m_pChr->MethdDunk()) {
                ClearStroke_(15.0f);
                return 1;
            }
        }
        if (m_pChr->MethdJumpShoot(fRot, bItem)) {
            ClearStroke_(15.0f);
            return 1;
        }
    }
    return 0;
}

long msw::CChrDrvPadBasket::CheckArtsChrInput(const _CHR_MSG *pMsg)
{
    switch (pMsg->m_nParam0) {
        case 14: {
            unsigned long nState = m_pStickRoll->GetState();
            float fDiff;
            float fAbs;

            if (!(nState & 1) && !(nState & 2)) {
                return 0;
            }
            if (!(m_fStickDir >= 0.0f) || !(m_fStickPower >= 0.5f)) {
                return 0;
            }
            fDiff = RoundRadPI(m_fStickDir + m_pChr->m_fRotY);
            fAbs = fabsf(fDiff);
            if (fAbs < PI / 4) {
                return 1;
            }
            if (fAbs > PI * 3 / 4) {
                return 3;
            }
            if (fDiff > 0.0f) {
                return 2;
            }
            return 4;
        }
        case 1:
            if (m_fStickDir >= 0.0f && m_fStickPower >= 0.5f) {
                float fRot = RoundRad(m_fStickDir + m_pChr->m_fRotY);
                if (fRot >= PI / 4 && fRot <= PI * 3 / 4) {
                    return 2;
                }
                if (fRot >= PI * 5 / 4 && fRot <= PI * 7 / 4) {
                    return 4;
                }
            }
            return 0;
        case 15:
            if (m_fStickDir >= 0.0f) {
                return 57.295776f * RoundRad(m_fStickDir + m_pChr->m_fRotY);
            }
            return -1;
    }
    return 0;
}

void msw::CChrDrvPadBasket::ClearFunc_()
{
    m_pStickRoll->Reset();
    m_pStick2Tap->Reset();
    m_nCrossFlag = 0;
    m_fCrossOnTime = 0.0f;
    m_fCrossOffTime = 0.0f;
    m_fCrossLastDir = -1.0f;
    m_bCross2Tap = 0;
    m_nCmdFlag = 0;
    m_nKeyHoldDone &= ~0x3F;
    for (int i = 0; i < 6; i++) {
        m_fKeyHoldTime[i] = 0.0f;
    }
    m_nKeyHold &= ~0x3F;
    for (int i = 0; i < 6; i++) {
        m_fKeyLongTime[i] = 0.0f;
    }
    m_nKeyLong &= ~0x3F;
}

void msw::BASKET::CStickState::Reset()
{
    m_nState = 0;
}

void msw::CChrDrvPadBasket::UpDateMyKeyState(long nKey, int bPress)
{
    float fFrame = m_pChr->m_fFrameStep;
    unsigned long nBit;

    if (!bPress) {
        nBit = 1 << nKey;
        m_nKeyHoldDone &= ~nBit;
        m_fKeyHoldTime[nKey] = 0.0f;
        m_nKeyHold &= ~nBit;
        m_fKeyLongTime[nKey] = 0.0f;
        m_nKeyLong &= ~nBit;
        return;
    }
    nBit = 1 << nKey;
    if (!(m_nKeyHoldDone & nBit)) {
        m_fKeyHoldTime[nKey] += fFrame;
        if (m_fKeyHoldTime[nKey] >= 6.0f) {
            m_fKeyHoldTime[nKey] = 6.0f;
            m_nKeyHold |= nBit;
            m_nKeyHoldDone |= nBit;
        }
    } else {
        m_fKeyHoldTime[nKey] = 0.0f;
        m_nKeyHold &= ~nBit;
    }
    m_fKeyLongTime[nKey] += fFrame;
    if (m_fKeyLongTime[nKey] >= 15.0f) {
        m_fKeyLongTime[nKey] = 15.0f;
        m_nKeyLong |= nBit;
    }
}

void msw::BASKET::CStickState2Tap::UpDate(float fDir, float fPower, float fFrame)
{
    m_fThreshold = 0.85f;
    m_nState &= ~0x20;
    switch (m_nStep) {
        case 0:
            if (fPower > m_fThreshold) {
                m_fDir = fDir;
                m_fTime = 0.0f;
                m_nStep++;
            } else {
                m_fTime = 0.0f;
            }
            break;
        case 1:
            m_fTime += fFrame;
            if (fPower > m_fThreshold - 0.1f) {
                if (m_fTime > 8.0f) {
                    m_nStep = 999;
                }
            } else {
                m_fTime = 0.0f;
                m_nStep++;
            }
            break;
        case 2:
            m_fTime += fFrame;
            if (fPower < m_fThreshold) {
                if (m_fTime > 8.0f) {
                    Clear_();
                }
            } else {
                float fDiff = RoundRadPI(fDir - m_fDir);
                Clear_();
                if (fabsf(fDiff) <= 0.34906584f) {
                    m_nState |= 0x20;
                }
            }
            break;
        default:
            if (fPower < m_fThreshold) {
                Clear_();
            }
            break;
    }
}

void msw::BASKET::CStickState2Tap::Reset()
{
    m_nStep = 0;
    m_fTime = 0.0f;
    m_fDir = -1.0f;
    m_nState = 0;
}

msw::BASKET::CStickState2Tap::~CStickState2Tap() {}

void msw::BASKET::CStickStateRoll::UpDate(float fDir, float fPower, float fFrame)
{
    float fDeg;
    float fDiff;

    CStickState::UpDate(fDir, fPower, fFrame);
    m_nState &= ~0x18;
    if (fPower < 0.4) {
        Clear_();
        return;
    }
    fDeg = 57.295776f * fDir;
    if (!(m_nStep & 1)) {
        m_fStartDeg = fDeg;
        m_fLastDeg = fDeg;
        m_fTime = 0.0f;
        m_fAngle = 0.0f;
        m_fTimer = 15.0f;
        m_nStep |= 1;
        return;
    }
    m_fTime += fFrame;
    if (m_fTime >= 30.0f) {
        Clear_();
        return;
    }
    m_fTimer -= fFrame;
    if (m_fTimer <= 0.0f) {
        Clear_();
        return;
    }
    fDiff = RoundRadPI(0.017453292f * (fDeg - m_fLastDeg));
    if (m_nStep & 6) {
        unsigned long nBit = 0;
        if (m_nStep & 2) {
            if (fDiff > 0.0f) {
                nBit = 8;
            } else {
                nBit = 0;
            }
        } else if (m_nStep & 4) {
            if (fDiff < 0.0f) {
                nBit = 0x10;
            } else {
                nBit = 0;
            }
        }
        if (nBit != 0) {
            m_fLastDeg = fDeg;
            m_fAngle += fabsf(fDiff);
            if (m_fAngle >= 2.6179938f) {
                m_nState |= nBit;
                Clear_();
            }
            m_fTimer = 15.0f;
            return;
        }
        if (fabsf(fDiff) > 0.08726646f) {
            Clear_();
        }
    } else if (fabsf(fDiff) <= PI / 2) {
        if (fDiff > 0.0f) {
            m_nStep |= 2;
        } else {
            m_nStep |= 4;
        }
        m_fLastDeg = fDeg;
        m_fTimer = 15.0f;
    }
}

void msw::BASKET::CStickStateRoll::Reset()
{
    m_nState = 0;
    Clear_();
}

msw::BASKET::CStickStateRoll::~CStickStateRoll() {}
