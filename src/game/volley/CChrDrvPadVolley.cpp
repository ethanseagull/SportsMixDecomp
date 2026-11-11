// msw::CChrDrvPadVolley: the pad driver of the volleyball game. It turns the
// Wii Remote (held sideways, or with the Nunchuk) into the character's
// actions. CChrDrvPadVolleyRhythmReceive is the driver of the rhythm receive
// party game, where the character only moves and receives.
#include "game/msw/CChrDrvPadVolley.hpp"

#include <math.h>

#include "game/msw/CBall.hpp"
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

CChrDrvPadVolley::CChrDrvPadVolley()
{
    m_nInputFlag = 0;
    m_pA0 = 0;
    m_nHoldA = 0;
    m_nHoldB = 0;
    m_nHold1 = 0;
    m_nHold2 = 0;
}

CChrDrvPadVolley::~CChrDrvPadVolley()
{
    if (m_pA0 != 0) {
        delete m_pA0;
    }
    m_pA0 = 0;
}

// The answers for the Remote with the Nunchuk: the same as for the Remote
// alone in this game.
inline long CChrDrvPadVolley::OnMsgW_(MSG_ID id, _CHR_MSG *pMsg)
{
    return OnMsgN(id, pMsg);
}

long CChrDrvPadVolley::OnMsg(MSG_ID id, _CHR_MSG *pMsg)
{
    if (m_nPadType == 0) {
        return OnMsgN(id, pMsg);
    }
    return OnMsgW_(id, pMsg);
}

// Sets rSide to the side the stick direction fDir points to, 1 or 2 (left
// or right of the half turn); leaves it alone while fDir is within fRange of
// straight up or down.
inline void CChrDrvPadVolley::GetStickSide_(long &rSide, float fDir, float fRange)
{
    float fAbs = fabsf(fRange);
    if (fDir <= fAbs || fDir >= 6.2831855f - fAbs
        || (fDir >= 3.1415927f - fAbs && fDir <= 3.1415927f + fAbs)) {
        return;
    }
    rSide = fDir > 3.1415927f ? 2 : 1;
}

long CChrDrvPadVolley::OnMsgN(MSG_ID id, _CHR_MSG *pMsg)
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
    case 0x02:
        VibStop_(-1);
        break;
    case 0x03:
        ClearWork_();
        m_nInputFlag = 0;
        break;
    case 0x04:
        ClearWork_();
        m_nInputFlag = 0;
        break;
    case 0x39:
        if (IsPressZ()) {
            nRet = 1;
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
    {
        int bTrgA = 0;
        if (pMsg != 0) {
            bTrgA = pMsg->m_nParam0 != 0;
        }
        nRet = AskSpikeStart_(bTrgA);
        break;
    }
    case 0x90:
        if (m_nPadType == 0) {
            if (IsTrgA()) {
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
        float fScratch = GetScratchPower();
        if (fScratch >= 0.2f) {
            nRet = fScratch * 512.0f;
        }
        break;
    }
    case 0x98:
        nRet = 0;
        if (GetStickDir() >= 0.0f) {
            GetStickSide_(nRet, m_fStickDir, 0.2617994f);
        }
        break;
    case 0x95:
        nRet = 0;
        if (GetStickDir() >= 0.0f) {
            GetStickSide_(nRet, m_fStickDir, 0.2617994f);
        }
        break;
    case 0x99:
        nRet = 0;
        break;
    case 0x23:
        // 1 (A with the Nunchuk) held for more than 20 frames since its press.
        if (m_nPadType == 0) {
            if (IsPress1() && m_nHold1 > 20 && (m_nInputFlag & INPUT_HOLD)) {
                nRet = 1;
                break;
            }
        } else if (IsPressA() && m_nHoldA > 20 && (m_nInputFlag & INPUT_HOLD)) {
            nRet = 1;
            break;
        }
        nRet = 0;
        break;
    case 0x9A:
        if (GetStickDir() >= 0.0f) {
            nRet = 1;
        }
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

long CChrDrvPadVolley::OnMsgMain(_CHR_MSG *pMsg)
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

    if (m_pChr->IsVolleyServeMode()) {
        MainServe_(pMsg);
        m_nInputFlag &= ~(INPUT_ITEM | INPUT_JUMP | INPUT_HOLD);
    } else if (m_pChr->IsStepAirTrick()) {
        if (IsChrChangeInput_() && m_pChr->MethdChgLeader()) {
            m_nInputFlag &= ~INPUT_CHANGE;
        }
        m_nInputFlag &= ~(INPUT_ITEM | INPUT_JUMP | INPUT_HOLD);
    } else {
        MainNormal_(pMsg);
        if ((m_nInputFlag & INPUT_ITEM) && !m_pChr->CanbeItemShoot()) {
            m_nInputFlag &= ~INPUT_ITEM;
        }
        if ((m_nInputFlag & INPUT_JUMP) && !m_pChr->CanbeJump() && !m_pChr->CanbeSpike(1)) {
            m_nInputFlag &= ~INPUT_JUMP;
        }
    }
    return 0;
}

// 1 and 2 (A and B with the Nunchuk) pressed together.
inline int CChrDrvPadVolley::IsArtsInput_()
{
    if (m_nPadType == 0) {
        if (IsPress1() && IsPress2() && m_nHold1 <= 5 && m_nHold2 <= 5) {
            m_nInputFlag &= ~(INPUT_ITEM | INPUT_JUMP | INPUT_RECEIVE | INPUT_HOLD);
            return 1;
        }
    } else if (IsPressA() && IsPressB() && m_nHoldA <= 5 && m_nHoldB <= 5) {
        m_nInputFlag &= ~(INPUT_ITEM | INPUT_JUMP | INPUT_RECEIVE | INPUT_HOLD);
        return 1;
    }
    return 0;
}

// A, or B with the Nunchuk (on release, or after six frames held).
inline int CChrDrvPadVolley::IsItemInput_()
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

// 2 (on release, or after six frames held), or a swing with the Nunchuk.
inline int CChrDrvPadVolley::IsReceiveInput_()
{
    if (m_nPadType == 0) {
        if (IsTrg2()) {
            m_nInputFlag |= INPUT_RECEIVE;
        } else if (IsRelease2() && (m_nInputFlag & INPUT_RECEIVE)) {
            return 1;
        }
        if (IsPress2() && m_nHold2 > 5 && (m_nInputFlag & INPUT_RECEIVE)) {
            return 1;
        }
    } else if (IsRemoconStroke(0.45f)) {
        return 1;
    }
    return 0;
}

void CChrDrvPadVolley::MainNormal_(_CHR_MSG *)
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

    if (IsArtsInput_() && m_pChr->MethdAirTrick()) {
        ClearStroke_(20.0f);
        return;
    }
    if (m_pChr->CanbeAutoSpikeJump() && m_pChr->MethdSpike(1)) {
        ClearStroke_(20.0f);
        return;
    }
    if (IsJumpBtnInput_() && m_pChr->MethdSpike(1)) {
        m_nInputFlag &= ~INPUT_RECEIVE;
        ClearStroke_(20.0f);
        return;
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

    if (m_pChr->GetStatusFlag() & 0x400) {
        return;
    }

    if (IsReceiveInput_()) {
        if (m_pChr->MethdJump()) {
            m_nInputFlag &= ~INPUT_RECEIVE;
            ClearStroke_(20.0f);
            return;
        }

        // A spike when the team has touched the ball enough already.
        CBall *pBall = m_pChr->GetVolleyTargetBall();
        int bSpike = 0;
        if (pBall != 0) {
            if (pBall->GetVolleyTouchNum(m_pChr->GetTeamNo()) >= 2) {
                bSpike = 1;
            }
            if (pBall->GetVolleyTouchNum(m_pChr->GetTeamNo()) >= 1
                && !m_pChr->IsVolleyRightBallTouch(0)) {
                bSpike = 1;
            }
        }
        if (bSpike && m_pChr->MethdSpike(0)) {
            m_nInputFlag &= ~INPUT_RECEIVE;
            ClearStroke_(20.0f);
            return;
        }

        if (GetStickDir() >= 0.0f) {
            fDir = RoundRad(fCamRot + m_fStickDir);
        }
        if (m_pChr->MethdReceive(0, 1, fDir)) {
            m_nInputFlag &= ~INPUT_RECEIVE;
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

void CChrDrvPadVolley::MainServe_(_CHR_MSG *)
{
    float fCamRot = m_pChr->GetCamRotY();

    if (m_pChr->GetStatusFlag2() & 0x0A000000) {
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

    // The kind of serve: 2 with down on the pad (or with A or Z held), 1 with
    // up (or a swing of kind 0), 0 otherwise (a swing of kind 3).
    if (m_nPadType == 0) {
        if (IsTrg2()) {
            long nServe = 0;
            if (m_nButton & 4) {
                nServe = 2;
            } else if (m_nButton & 8) {
                nServe = 1;
            }
            if (nServe >= 0 && m_pChr->MethdServe(nServe)) {
                ClearStroke_(20.0f);
                return;
            }
        }
    } else if (m_nStrokeKind != -1 && m_nStrokeKind != 6) {
        long nServe = -1;
        switch (m_nStrokeKind) {
        case 3:
            nServe = 0;
            break;
        case 0:
            nServe = 1;
            break;
        }
        if (nServe >= 0) {
            if (IsPressA() || IsPressZ()) {
                nServe = 2;
            }
            if (m_pChr->MethdServe(nServe)) {
                ClearStroke_(20.0f);
                return;
            }
        }
    }

    if (GetStickDir() >= 0.0f) {
        float fRunDir = RoundRad(fCamRot + m_fStickDir);
        float fPower = m_fStickPower;
        m_pChr->MethdSliding(fRunDir);
        m_pChr->MethdRun(fRunDir, fPower);
    }
}

// 1 (A with the Nunchuk), on release; held for more than 40 frames it no
// longer counts.
int CChrDrvPadVolley::IsJumpBtnInput_()
{
    if (m_nPadType == 0) {
        if (IsTrg1()) {
            m_nInputFlag |= INPUT_JUMP | INPUT_HOLD;
        } else if (IsRelease1()) {
            int bJump = (m_nInputFlag & INPUT_JUMP) != 0;
            m_nInputFlag &= ~(INPUT_JUMP | INPUT_HOLD);
            if (bJump) {
                return 1;
            }
        }
        if (IsPress1() && m_nHold1 > 40 && (m_nInputFlag & INPUT_JUMP)) {
            m_nInputFlag &= ~INPUT_JUMP;
        }
    } else {
        if (IsTrgA()) {
            m_nInputFlag |= INPUT_JUMP | INPUT_HOLD;
        } else if (IsReleaseA()) {
            int bJump = (m_nInputFlag & INPUT_JUMP) != 0;
            m_nInputFlag &= ~(INPUT_JUMP | INPUT_HOLD);
            if (bJump) {
                return 1;
            }
        }
        if (IsPressA() && m_nHoldA > 40 && (m_nInputFlag & INPUT_JUMP)) {
            m_nInputFlag &= ~INPUT_JUMP;
        }
    }
    return 0;
}

int CChrDrvPadVolley::IsChrChangeInput_()
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

long CChrDrvPadVolley::AskReceive_()
{
    long nRet = 0;

    if (m_nPadType == 0) {
        if (IsTrg2()) {
            m_nInputFlag |= INPUT_RECEIVE;
        } else if (IsRelease2() && (m_nInputFlag & INPUT_RECEIVE)) {
            nRet = 1;
        }
        if (IsPress2() && m_nHold2 > 5 && (m_nInputFlag & INPUT_RECEIVE)) {
            nRet = 1;
        }
    } else if (IsRemoconStroke(0.45f)) {
        nRet = 1;
        ClearStroke_(20.0f);
    }
    return nRet;
}

// bTrgA: A starts the spike too (with the Remote held sideways).
long CChrDrvPadVolley::AskSpikeStart_(int bTrgA)
{
    if (m_nPadType == 0) {
        if (IsTrgA() && bTrgA) {
            m_nInputFlag &= ~INPUT_RECEIVE;
            return 1;
        }
        if (IsRemoconStroke(0.45f)) {
            m_nInputFlag &= ~INPUT_RECEIVE;
            return 1;
        }
        if (IsTrg2()) {
            m_nInputFlag |= INPUT_RECEIVE;
        } else if (IsRelease2() && (m_nInputFlag & INPUT_RECEIVE)) {
            return 1;
        }
        if (IsPress2() && m_nHold2 > 5 && (m_nInputFlag & INPUT_RECEIVE)) {
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
void CChrDrvPadVolley::UpdateFunc_()
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

void CChrDrvPadVolley::ClearFunc_()
{
    ClearStroke_(0.0f);
    m_nHoldA = 0;
    m_nHoldB = 0;
    m_nHold1 = 0;
    m_nHold2 = 0;
}

CChrDrvPadVolleyRhythmReceive::CChrDrvPadVolleyRhythmReceive()
{
}

CChrDrvPadVolleyRhythmReceive::~CChrDrvPadVolleyRhythmReceive()
{
}

long CChrDrvPadVolleyRhythmReceive::OnMsg(MSG_ID id, _CHR_MSG *pMsg)
{
    if (id == 0) {
        if (m_pChr == 0) {
            return 0;
        }
        UpdatePadState();
        if (GetStickDir() >= 0.0f) {
            m_pChr->MoveVolleyTargetMarkerPos(RoundRad(m_fStickDir + m_pChr->GetCamRotY()));
        }
        MainNormal_(pMsg);
        return 0;
    }
    return CChrDrvPadVolley::OnMsg(id, pMsg);
}

void CChrDrvPadVolleyRhythmReceive::MainNormal_(_CHR_MSG *)
{
    float fDir = -1.0f;
    float fCamRot = m_pChr->GetCamRotY();

    if (m_pChr->GetStatusFlag2() & 0x0A000000) {
        return;
    }

    if (GetStickDir() >= 0.0f) {
        fDir = RoundRad(fCamRot + m_fStickDir);
        m_pChr->MethdSetMoveRevise(fDir);
    }

    if (m_pChr->GetStatusFlag() & 0x400) {
        return;
    }

    if (IsReceiveInput_()) {
        if (GetStickDir() >= 0.0f) {
            fDir = RoundRad(fCamRot + m_fStickDir);
        }
        if (m_pChr->MethdReceive(0, 1, fDir)) {
            m_nInputFlag &= ~INPUT_RECEIVE;
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

}
