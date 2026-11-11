// msw::CChrDrvPadBoss: the pad driver of the boss game. It turns the Wii
// Remote (held sideways, or with the Nunchuk) into the character's actions.
#include "game/msw/CChrDrvPadBoss.hpp"

#include <math.h>

#include "game/msw/CChrCore.hpp"

namespace msw {

namespace {

// Brings an angle into [0, 2pi].
inline float RoundRad(float fRad)
{
    while (fRad > 6.2831855f) {
        fRad -= 6.2831855f;
    }
    while (fRad < 0.0f) {
        fRad += 6.2831855f;
    }
    return fRad;
}

}

CChrDrvPadBoss::CChrDrvPadBoss()
{
    m_nInputFlag = 0;
    m_nHoldA = 0;
    m_nHoldB = 0;
    m_nHold1 = 0;
    m_nHold2 = 0;
}

CChrDrvPadBoss::~CChrDrvPadBoss()
{
}

// The answers for the Remote with the Nunchuk: the same as for the Remote
// alone in this game.
inline long CChrDrvPadBoss::OnMsgW_(MSG_ID id, _CHR_MSG *pMsg)
{
    return OnMsgN(id, pMsg);
}

long CChrDrvPadBoss::OnMsg(MSG_ID id, _CHR_MSG *pMsg)
{
    if (m_nPadType == 0) {
        return OnMsgN(id, pMsg);
    }
    return OnMsgW_(id, pMsg);
}

// Sets rSide to the side the stick direction fDir points to, 1 or 2 (left
// or right of the half turn); leaves it alone while fDir is within fRange of
// straight up or down.
inline void CChrDrvPadBoss::GetStickSide_(long &rSide, float fDir, float fRange)
{
    float fAbs = fabsf(fRange);
    if (fDir <= fAbs || fDir >= 6.2831855f - fAbs
        || (fDir >= 3.1415927f - fAbs && fDir <= 3.1415927f + fAbs)) {
        return;
    }
    rSide = fDir > 3.1415927f ? 2 : 1;
}

long CChrDrvPadBoss::OnMsgN(MSG_ID id, _CHR_MSG *pMsg)
{
    long nRet = 0;

    switch (id) {
    case 0x00:
        nRet = OnMsgMain(pMsg);
        break;
    case 0x01:
        ClearWork_();
        m_nInputFlag = 0;
        break;
    case 0x03:
        ClearWork_();
        m_nInputFlag = 0;
        break;
    case 0x04:
        ClearWork_();
        m_nInputFlag = 0;
        break;
    case 0x1A:
        if (m_nPadType == 0) {
            if (IsPress2()) {
                nRet = 1;
            } else {
                nRet = 2;
                m_nInputFlag &= ~INPUT_2;
            }
        } else if (IsRemoconStroke(0.45f)) {
            ClearStroke_(20.0f);
            nRet = 2;
        } else {
            nRet = IsPressZ() != 0;
        }
        break;
    case 0x3B:
    case 0x3D:
    case 0x3E:
        nRet = GetScratchPower() * 1024.0f;
        break;
    case 0x8C:
        nRet = AskReceive_();
        break;
    case 0x8F:
        nRet = AskSpikeStart_();
        break;
    case 0x90:
        if (m_nPadType == 0) {
            if (IsRemoconStroke(0.45f)) {
                nRet = 1;
                break;
            }
        } else if (IsPressB()) {
            nRet = 1;
            break;
        }
        nRet = 0;
        break;
    case 0x91:
        nRet = 4;
        if (GetStickDir() >= 0.0f) {
            GetStickSide_(nRet, m_fStickDir, 0.7853982f);
        }
        break;
    case 0x8A:
        if (m_nPadType == 0) {
            if (IsTrg2()) {
                nRet = 1;
                break;
            }
        } else if (m_nStroke != -1) {
            nRet = 1;
            break;
        }
        nRet = 0;
        break;
    case 0x8B:
        if (m_nPadType == 0) {
            nRet = 4;
        } else {
            nRet = m_nStroke;
            if (nRet != -1 && nRet != 6) {
                ClearStroke_(20.0f);
            } else {
                nRet = -1;
            }
        }
        break;
    case 0x93:
        if (m_nPadType == 0) {
            if (IsTrg2()) {
                nRet = 1;
                break;
            }
        } else if (m_nStroke != -1) {
            nRet = 1;
            break;
        }
        nRet = 0;
        break;
    case 0x94:
        nRet = m_nStroke;
        if (nRet != -1 && nRet != 6) {
            ClearStroke_(20.0f);
        } else {
            nRet = -1;
        }
        break;
    case 0x97: {
        nRet = 0;
        float fNunchuk = GetNunchukPower();
        float fScratch = GetScratchPower();
        if (fNunchuk >= 0.25f || fScratch >= 0.2f) {
            nRet = 512.0f * fNunchuk + 512.0f * fScratch;
            ClearStroke_(7.0f);
        }
        break;
    }
    case 0x98:
        nRet = 0;
        if (GetStickDir() >= 0.0f) {
            GetStickSide_(nRet, m_fStickDir, 0.2617994f);
        }
        break;
    case 0x99:
        nRet = 0;
        break;
    case 0x53:
        VibStart_(2, pMsg->m_nParam0, pMsg->m_nParam1);
        break;
    case 0x54:
        VibStop_(-1);
        break;
    }
    return nRet;
}

long CChrDrvPadBoss::OnMsgMain(_CHR_MSG *pMsg)
{
    if (m_pChr == 0) {
        return 0;
    }

    // State 0x2D is the spike (meaning guessed): the spike button stays
    // held only while it lasts.
    if ((m_nInputFlag & INPUT_SPIKE) && m_pChr->GetState() != 0x2D) {
        m_nInputFlag &= ~INPUT_SPIKE;
    }

    UpdatePadState();

    if (GetStickDir() >= 0.0f) {
        m_pChr->MoveVolleyTargetMarkerPos(RoundRad(m_fStickDir + m_pChr->GetCamRotY()));
    }

    if (m_pChr->GetStatusFlag() & 1) {
        MainOnBall_(pMsg);
        m_nInputFlag &= ~INPUT_ITEM;
    } else if (m_pChr->IsStepAirTrick()) {
        m_nInputFlag &= ~(INPUT_ITEM | INPUT_JUMP);
    } else {
        MainOffBall_(pMsg);
        if ((m_nInputFlag & INPUT_ITEM) && !m_pChr->CanbeItemShoot()) {
            m_nInputFlag &= ~INPUT_ITEM;
        }
        if ((m_nInputFlag & INPUT_JUMP) && !m_pChr->CanbeJump() && !m_pChr->CanbeSpike(1)) {
            m_nInputFlag &= ~INPUT_JUMP;
        }
    }
    return 0;
}

// 1 (pass, or a swing with the Nunchuk): on release, or after six frames
// held.
inline int CChrDrvPadBoss::IsBtn1Input_()
{
    if (m_nPadType == 0) {
        if (IsTrg1()) {
            m_nInputFlag |= INPUT_PASS;
        } else if (IsRelease1() && (m_nInputFlag & INPUT_PASS)) {
            return 1;
        }
        if (IsPress1() && m_nHold1 > 5 && (m_nInputFlag & INPUT_PASS)) {
            return 1;
        }
    } else if (IsRemoconStroke(0.45f)) {
        return 1;
    }
    return 0;
}

// 2 (or a swing with the Nunchuk): on release only.
inline int CChrDrvPadBoss::IsBtn2Input_()
{
    if (m_nPadType == 0) {
        if (IsTrg2()) {
            m_nInputFlag |= INPUT_2;
        } else if (IsRelease2() && (m_nInputFlag & INPUT_2)) {
            return 1;
        }
    } else if (IsRemoconStroke(0.45f)) {
        return 1;
    }
    return 0;
}

// 2 held for more than ten frames (Z with the Nunchuk).
inline int CChrDrvPadBoss::IsStanceInput_()
{
    if (m_nPadType == 0) {
        if (IsTrg2()) {
            m_nInputFlag |= INPUT_STANCE;
        } else if (IsRelease2()) {
            m_nInputFlag &= ~INPUT_STANCE;
        }
        if (IsPress2() && m_nHold2 > 10 && (m_nInputFlag & INPUT_STANCE)) {
            m_nInputFlag &= ~INPUT_JUMP;
            return 1;
        }
    } else if (IsPressZ()) {
        return 1;
    }
    return 0;
}

// A, or B with the Nunchuk (on release, or after six frames held).
inline int CChrDrvPadBoss::IsItemInput_()
{
    if (m_nPadType == 0) {
        if (IsTrgA()) {
            return 1;
        }
    } else {
        if (IsTrgB()) {
            m_nInputFlag |= INPUT_ITEM;
        } else if (IsReleaseB() && (m_nInputFlag & INPUT_ITEM)) {
            return 1;
        }
        if (IsPressB() && m_nHoldB > 5 && (m_nInputFlag & INPUT_ITEM)) {
            return 1;
        }
    }
    return 0;
}

void CChrDrvPadBoss::MainOnBall_(_CHR_MSG *)
{
    float fCamRot = m_pChr->GetCamRotY();

    if (m_pChr->GetStatusFlag2() & 0x0A000000) {
        return;
    }

    if (IsArtsInput_() && m_pChr->MethdAirTrick()) {
        ClearStroke_(20.0f);
        return;
    }
    if (IsChrChangeInput_() && m_pChr->MethdChgLeader()) {
        m_nInputFlag &= ~INPUT_CHANGE;
        return;
    }
    if (GetStickDir() >= 0.0f) {
        m_pChr->MethdSetMoveRevise(RoundRad(fCamRot + m_fStickDir));
    }

    if (m_pChr->GetStatusFlag() & 0x400) {
        return;
    }

    if (IsPassBtnInput_() && m_pChr->MethdPass((CChrCore *)0, 0)) {
        ClearStroke_(20.0f);
        return;
    }
    if (IsBtn1Input_() && m_pChr->MethdJump()) {
        ClearStroke_(20.0f);
        return;
    }
    if (IsBtn2Input_()) {
        if (GetStickDir() >= 0.0f && m_pChr->MethdDunk()) {
            ClearStroke_(20.0f);
            return;
        }
        if (m_pChr->MethdShoot(-1.0f)) {
            ClearStroke_(20.0f);
            return;
        }
    }
    if (GetStickDir() >= 0.0f) {
        float fDir = RoundRad(fCamRot + m_fStickDir);
        float fPower = m_fStickPower;
        m_pChr->MethdSliding(fDir);
        m_pChr->MethdRun(fDir, fPower);
    }
}

void CChrDrvPadBoss::MainOffBall_(_CHR_MSG *)
{
    float fDir = -1.0f;
    float fCamRot = m_pChr->GetCamRotY();

    if (m_pChr->GetStatusFlag2() & 0x0A000000) {
        return;
    }

    if (IsChrChangeInput_() && m_pChr->MethdChgLeader()) {
        m_nInputFlag &= ~INPUT_CHANGE;
        return;
    }
    if (GetStickDir() >= 0.0f) {
        fDir = RoundRad(fCamRot + m_fStickDir);
        m_pChr->MethdSetMoveRevise(fDir);
    }

    if (m_pChr->GetStatusFlag() & 0x400) {
        return;
    }

    if (IsArtsInput_() && m_pChr->MethdAirTrick()) {
        ClearStroke_(20.0f);
        return;
    }

    if (IsStanceInput_()) {
        float fStanceDir = -1.0f;
        if (GetStickDir() >= 0.0f) {
            fStanceDir = RoundRad(fCamRot + m_fStickDir);
        }
        if (m_pChr->MethdShootStance(fStanceDir, 0)) {
            return;
        }
    }

    if (IsBtn1Input_()) {
        float fRot = m_pChr->GetRotY();
        if (GetStickDir() >= 0.0f) {
            fDir = RoundRad(fCamRot + m_fStickDir);
            fRot = fDir;
        }
        if (m_nPadType == 1 && m_pChr->MethdReceive(0, 0, fDir)) {
            ClearStroke_(20.0f);
            return;
        }
        if (m_pChr->MethdSideStep(fRot, fDir)) {
            m_nInputFlag &= ~INPUT_PASS;
            ClearStroke_(20.0f);
            return;
        }
    }

    if (IsJumpBtnInput_()) {
        if (m_nPadType == 0) {
            if (m_pChr->MethdSpike(0)) {
                m_nInputFlag &= ~INPUT_2;
                return;
            }
            m_pChr->GetRotY();
            if (GetStickDir() >= 0.0f) {
                fDir = RoundRad(fCamRot + m_fStickDir);
            }
            if (m_pChr->MethdReceive(0, 0, fDir)) {
                ClearStroke_(20.0f);
                return;
            }
        }
        if (m_pChr->MethdJump()) {
            ClearStroke_(20.0f);
            return;
        }
        if (m_nPadType == 1 && m_pChr->MethdSpike(1)) {
            m_nInputFlag &= ~INPUT_2;
            ClearStroke_(20.0f);
            return;
        }
    }

    if (IsItemInput_()) {
        float fItemDir = -1.0f;
        if (GetStickDir() >= 0.0f) {
            fItemDir = RoundRad(fCamRot + m_fStickDir);
        }
        if (m_pChr->MethdItemShoot(fItemDir)) {
            ClearStroke_(20.0f);
            return;
        }
    }

    if (IsBtn2Input_()) {
        m_pChr->GetRotY();
        if (GetStickDir() >= 0.0f) {
            fDir = RoundRad(fCamRot + m_fStickDir);
        }
        if (m_pChr->MethdReceive(0, 1, fDir)) {
            ClearStroke_(20.0f);
            return;
        }
    }

    if (GetStickDir() >= 0.0f) {
        float fRunDir = RoundRad(fCamRot + m_fStickDir);
        float fPower = m_fStickPower;
        m_pChr->MethdSliding(fRunDir);
        m_pChr->MethdRun(fRunDir, fPower);
    }
}

// 1 and 2 (A and B with the Nunchuk) pressed together.
int CChrDrvPadBoss::IsArtsInput_()
{
    if (m_nPadType == 0) {
        if (IsPress1() && IsPress2() && m_nHold1 <= 5 && m_nHold2 <= 5) {
            m_nInputFlag &= ~(INPUT_ITEM | INPUT_JUMP | INPUT_STANCE | INPUT_2);
            m_nInputFlag |= INPUT_ARTS;
            return 1;
        }
    } else if (IsPressA() && IsPressB() && m_nHoldA <= 5 && m_nHoldB <= 5) {
        m_nInputFlag &= ~(INPUT_ITEM | INPUT_JUMP | INPUT_STANCE | INPUT_2);
        m_nInputFlag |= INPUT_ARTS;
        return 1;
    }
    return 0;
}

int CChrDrvPadBoss::IsJumpBtnInput_()
{
    if (m_nPadType == 0) {
        if (IsTrg2()) {
            m_nInputFlag |= INPUT_JUMP;
        } else if (IsRelease2() && (m_nInputFlag & INPUT_JUMP)) {
            return 1;
        }
    } else {
        if (IsTrgA()) {
            m_nInputFlag |= INPUT_JUMP;
        } else if (IsReleaseA() && (m_nInputFlag & INPUT_JUMP)) {
            return 1;
        }
        if (IsPressA() && m_nHoldA > 5 && (m_nInputFlag & INPUT_JUMP)) {
            return 1;
        }
    }
    return 0;
}

int CChrDrvPadBoss::IsPassBtnInput_()
{
    if (m_nPadType == 0) {
        if (IsTrg1()) {
            m_nInputFlag |= INPUT_PASS;
        } else if (IsRelease1() && (m_nInputFlag & INPUT_PASS)) {
            return 1;
        }
        // The hold count of 2, not 1 (as in the original).
        if (IsPress1() && m_nHold2 > 5 && (m_nInputFlag & INPUT_PASS)) {
            return 1;
        }
    } else {
        if (IsTrgA()) {
            m_nInputFlag |= INPUT_JUMP;
        } else if (IsReleaseA() && (m_nInputFlag & INPUT_JUMP)) {
            return 1;
        }
        if (IsPressA() && m_nHoldA > 5 && (m_nInputFlag & INPUT_JUMP)) {
            return 1;
        }
    }
    return 0;
}

int CChrDrvPadBoss::IsChrChangeInput_()
{
    if (m_nPadType == 0) {
        if (IsTrgB()) {
            m_nInputFlag |= INPUT_CHANGE;
        } else if (IsReleaseB()) {
            if ((m_nInputFlag & INPUT_CHANGE) && m_nHoldB > 5) {
                m_nInputFlag &= ~INPUT_CHANGE;
                return 1;
            }
            m_nInputFlag &= ~INPUT_CHANGE;
        }
        if (IsPressB() && m_nHoldB > 5 && (m_nInputFlag & INPUT_CHANGE)) {
            return 1;
        }
    } else if (IsTrgC()) {
        return 1;
    }
    return 0;
}

inline int CChrDrvPadBoss::IsPressAB_() const
{
    if (IsPressA() && IsPressB()) {
        return 1;
    }
    return 0;
}

long CChrDrvPadBoss::AskReceive_()
{
    long nRet = 0;

    if (m_nPadType == 0) {
        if (IsTrg2()) {
            m_nInputFlag |= INPUT_2;
        } else if (IsRelease2() && (m_nInputFlag & INPUT_2)) {
            nRet = 1;
        }
        if (IsPress2() && m_nHold2 > 5 && (m_nInputFlag & INPUT_2)) {
            nRet = 1;
        }
    } else if (IsRemoconStroke(0.45f)) {
        nRet = 1;
        if (IsPressAB_()) {
            nRet = 2;
        }
        ClearStroke_(20.0f);
    }
    return nRet;
}

long CChrDrvPadBoss::AskSpikeStart_()
{
    if (m_nPadType == 0) {
        if (IsRemoconStroke(0.45f)) {
            m_nInputFlag &= ~INPUT_2;
            return 1;
        }
        if (IsTrg2()) {
            m_nInputFlag |= INPUT_2;
        } else if (IsRelease2() && (m_nInputFlag & INPUT_2)) {
            return 1;
        }
        if (IsPress2() && m_nHold2 > 5 && (m_nInputFlag & INPUT_2)) {
            return 1;
        }
        if (m_nInputFlag & INPUT_SPIKE) {
            if (!IsPress1()) {
                m_nInputFlag &= ~INPUT_SPIKE;
                return 2;
            }
            return 3;
        }
        if (IsTrg1()) {
            m_nInputFlag |= INPUT_SPIKE;
            return 3;
        }
    } else if (m_nInputFlag & INPUT_SPIKE) {
        if (IsRemoconStroke(0.45f)) {
            m_nInputFlag &= ~INPUT_SPIKE;
            ClearStroke_(20.0f);
            return 2;
        }
        if (!IsPressA()) {
            m_nInputFlag &= ~INPUT_SPIKE;
            return 2;
        }
        return 3;
    } else if (IsRemoconStroke(0.45f)) {
        ClearStroke_(20.0f);
        return 1;
    } else if (!(m_nInputFlag & INPUT_SPIKE) && IsTrgA()) {
        m_nInputFlag |= INPUT_SPIKE;
        return 3;
    }
    return 0;
}

// Counts the frames each button has been held.
void CChrDrvPadBoss::UpdateFunc_()
{
    if (IsPressA() || IsTrgA()) {
        m_nHoldA++;
    } else {
        m_nHoldA = 0;
    }
    if (IsPressB() || IsTrgB()) {
        m_nHoldB++;
    } else {
        m_nHoldB = 0;
    }
    if (IsPress1() || IsTrg1()) {
        m_nHold1++;
    } else {
        m_nHold1 = 0;
    }
    if (IsPress2() || IsTrg2()) {
        m_nHold2++;
    } else {
        m_nHold2 = 0;
    }
}

void CChrDrvPadBoss::ClearFunc_()
{
    ClearStroke_(0.0f);
    m_nHoldA = 0;
    m_nHoldB = 0;
    m_nHold1 = 0;
    m_nHold2 = 0;
    m_nInputFlag &= ~INPUT_ARTS;
}

}
