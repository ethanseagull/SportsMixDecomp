// msw::CChrDrvPadHockey: the pad driver of the hockey game. It turns the
// Wii Remote (held sideways, or with the Nunchuk) into the character's
// actions.
#include "game/msw/CChrDrvPadHockey.hpp"

#include <math.h>

#include "bcs/CMainApp.hpp"
#include "bcs/CWifiPad.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameVib.hpp"
#include "nw4r/math/triangular.h"

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

// Brings an angle into [-pi, pi].
inline float RoundRadPM(float fRad)
{
    fRad = RoundRad(fRad);
    if (fRad > 3.1415927f) {
        fRad -= 6.2831855f;
    }
    return fRad;
}

// The states (meaning not known) in which a pass can be called for.
inline int IsPassCallState(long nState)
{
    if (nState == 0x01 || nState == 0x04 || nState == 0x43 || nState == 0x46 || nState == 0x55) {
        return 1;
    }
    return 0;
}

// rOut = r1 - r2. A stand-in (a guess) for a vector operator of the
// original, as in game/hockey/CChrManHockey.cpp: called one inline level
// deeper, the paired-single routine numbers its float registers as the
// original does.
inline void VecSub(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
{
    bcs::FVEC3Sub(&rOut, &r1, &r2);
}

// The quarter of a turn (0 to 3) direction fDir points to, quarters
// centred on 0, pi/2, pi and 3pi/2.
inline long GetQuarter(float fDir)
{
    return RoundRad(0.7853982f + fDir) / 1.5707964f;
}

}

long CChrDrvPadHockey::m_nTrgStroke[19];

CChrDrvPadHockey::CChrDrvPadHockey()
{
    m_nUnk9C = 0;
    m_fNeutralFrame = 0.0f;
    m_fTiltFrame = 0.0f;
    m_fFlickDir = -1.0f;
    m_nVibPattern = -1;
    m_fHoldA = 0.0f;
    m_fHoldB = 0.0f;
    m_fHold1 = 0.0f;
    m_fHold2 = 0.0f;
    m_fStickEnableFrame = 0.0f;
    m_fStickFrame = 0.0f;
    m_fStickStartDeg = -1.0f;
    m_fStickLastDeg = -1.0f;
    m_nInputFlag = 0;
    m_nUnkD0 = 0;
    m_nStickFlag = 0;
    m_nStickArea = 0;
    for (long i = 0; i < 19; i++) {
        m_nTrgStroke[i] = 0;
    }
    m_bCmdBufReady = 1;
    for (long i = 0; i < 3; i++) {
        m_CmdBuf[i].m_fTime = -1.0f;
    }
    for (long i = 0; i < 5; i++) {
        m_PressInfo[i].m_bPress = 0;
        m_PressInfo[i].m_nPressFrame = 0;
        m_PressInfo[i].m_nReleaseFrame = 0;
    }
}

CChrDrvPadHockey::~CChrDrvPadHockey()
{
}

// Forgets the quick push of the stick.
inline void CChrDrvPadHockey::ClearFlick_()
{
    m_fNeutralFrame = 0.0f;
    m_fTiltFrame = 0.0f;
    m_fFlickDir = -1.0f;
    m_fTiltDir = -1.0f;
}

// The checks as bools. The result is set after the call, as in the
// original (cmpwi, li 0, beq, li 1); a plain "!= 0" compiles branch-free.
// A swing of the Remote (or of the kind m_nTrgStroke gives).
inline bool CChrDrvPadHockey::IsStroke_() const
{
    int nRet = CheckStroke(0, 1);
    bool bRet = false;
    if (nRet) {
        bRet = true;
    }
    return bRet;
}

inline bool CChrDrvPadHockey::IsPressKey_(long nKey) const
{
    int nRet = CheckPress(nKey);
    bool bRet = false;
    if (nRet) {
        bRet = true;
    }
    return bRet;
}

inline bool CChrDrvPadHockey::IsTrgKey_(long nKey) const
{
    int nRet = CheckTrg(nKey);
    bool bRet = false;
    if (nRet) {
        bRet = true;
    }
    return bRet;
}

// A swing (Nunchuk), or 2 released (or a swing).
inline int CChrDrvPadHockey::IsShootInput_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsStroke_()) {
            return 1;
        }
    } else if (IsRelease2()) {
        return 1;
    }
    return 0;
}

// Z (Nunchuk) or 2 pressed this frame.
inline int CChrDrvPadHockey::IsKeeperInput_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsTrgZ()) {
            return 1;
        }
    } else if (IsTrg2()) {
        return 1;
    }
    return 0;
}

// Z (Nunchuk) or 2 held.
inline int CChrDrvPadHockey::IsStanceInput_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsPressKey_(7)) {
            return 1;
        }
    } else if (IsPress2()) {
        return 1;
    }
    return 0;
}

// B (Nunchuk) or 2 held.
inline int CChrDrvPadHockey::IsStance2Input_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsPressKey_(5)) {
            return 1;
        }
    } else if (IsPress2()) {
        return 1;
    }
    return 0;
}

// A (Nunchuk) or 2 pressed this frame.
inline int CChrDrvPadHockey::IsTrgA2_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsTrgA()) {
            return 1;
        }
    } else if (IsTrg2()) {
        return 1;
    }
    return 0;
}

// A (Nunchuk) or 1 held.
inline int CChrDrvPadHockey::IsPressA1_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsPressA()) {
            return 1;
        }
    } else if (IsPress1()) {
        return 1;
    }
    return 0;
}

// A or Z (Nunchuk), or 2 held.
inline int CChrDrvPadHockey::IsPressAZ2_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsPressA() || IsPressZ()) {
            return 1;
        }
    } else if (IsPress2()) {
        return 1;
    }
    return 0;
}

// B held and a swing (Nunchuk), or A pressed this frame.
inline int CChrDrvPadHockey::IsItemBallInput_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsPressKey_(5) && IsStroke_()) {
            return 1;
        }
    } else if (IsTrgA()) {
        return 1;
    }
    return 0;
}

long CChrDrvPadHockey::OnMsg(MSG_ID id, _CHR_MSG *pMsg)
{
    long nRet = 0;

    switch (id) {
    case 0x00:
        nRet = OnMsgMain(pMsg);
        break;
    case 0x01:
        ClearWork_();
        ClearFlick_();
        m_nInputFlag = 0;
        break;
    case 0x02:
        if (m_pChr != 0) {
            CGameVib::Stop(m_pChr->GetPadNo());
        }
        m_nVibPattern = -1;
        break;
    case 0x03:
        ClearWork_();
        ClearFlick_();
        m_nInputFlag = 0;
        break;
    case 0x04:
        ClearWork_();
        ClearFlick_();
        m_nInputFlag = 0;
        break;
    case 0x19:
        if (IsPressA()) {
            nRet = 1;
        }
        break;
    case 0x3B:
    case 0x3E:
    case 0x5F:
        if (IsRemoconStroke(0.45f)) {
            nRet = 1;
        }
        break;
    case 0x59:
        if (IsShoot2Input_()) {
            nRet = 1;
        }
        break;
    case 0x26:
    case 0x57:
    case 0x5D: {
        float fPower = bcs::CWifiPad::GetScratchPower(m_pChr->GetPadNo());
        if (fPower > 0.2f) {
            nRet = 1024.0f * fPower;
        }
        break;
    }
    case 0x1A:
    case 0x21:
    case 0x66:
    case 0x6B:
    case 0x6E:
    case 0x80:
        if (IsShootInput_()) {
            nRet = 1;
        }
        break;
    case 0x28:
        if (GetStickDir() >= 0.0f) {
            m_pChr->MoveTargetMarkerPos(RoundRad(m_fStickDir + m_pChr->GetCamRotY()));
            nRet = 1;
        }
        break;
    case 0x53: {
        CChrCore *pChr = m_pChr;
        long nFrame = pMsg->m_nParam1;
        long nPattern = pMsg->m_nParam0;
        if (pChr != 0) {
            if (nPattern >= 0) {
                CGameVib::Start(4, nPattern, nFrame > 0 ? nFrame : -2, pChr->GetPadNo());
                m_nVibPattern = nPattern;
            } else {
                float fFrame = fabsf((float)nPattern);
                if (fFrame <= 1.0f) {
                    fFrame = 1.0f;
                }
                CGameVib::Start(fFrame, pChr->GetPadNo());
                m_nVibPattern = -1;
            }
        }
        break;
    }
    case 0x54: {
        CChrCore *pChr = m_pChr;
        long nPattern = pMsg->m_nParam0;
        if (pChr != 0) {
            if (nPattern >= 0 && m_nVibPattern != nPattern) {
                break;
            }
            CGameVib::Stop(pChr->GetPadNo());
        }
        m_nVibPattern = -1;
        break;
    }
    case 0x5E:
        if (IsTrgA2_()) {
            nRet = 1;
        }
        break;
    case 0x23:
        if (IsPressA1_()) {
            nRet = 1;
        }
        break;
    case 0x7E:
        if (IsKeeperInput_()) {
            nRet = 1;
        }
        break;
    case 0x58:
    case 0x7F:
        if (IsPressAZ2_()) {
            nRet = 1;
        }
        break;
    case 0x62:
    case 0x69:
        if (IsStanceInput_()) {
            nRet = 1;
        }
        break;
    case 0x63:
    case 0x65:
        if (IsStanceInput_()) {
            nRet = 1;
        } else if (IsStance2Input_()) {
            nRet = 1;
        }
        break;
    case 0x70:
        if (IsItemBallInput_()) {
            nRet = 1;
        }
        break;
    case 0x6C:
        if (IsStroke_()) {
            nRet = 1;
        }
        break;
    }
    return nRet;
}

// A swing (Nunchuk), or 2 released or a swing.
inline int CChrDrvPadHockey::IsShoot2Input_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsStroke_()) {
            return 1;
        }
    } else if (IsRelease2() || IsStroke_()) {
        return 1;
    }
    return 0;
}

long CChrDrvPadHockey::OnMsgMain(_CHR_MSG *pMsg)
{
    if (m_pChr == 0) {
        return 0;
    }

    UpdatePadState();
    UpdatePressInfo();

    // A push of the stick let go within 14 frames is a flick; it is kept
    // for 10 frames.
    if (!(GetStickDir() >= 0.0f)) {
        m_fNeutralFrame += 1.0f;
        if (m_fNeutralFrame >= 10.0f) {
            m_fTiltDir = -1.0f;
            m_fFlickDir = -1.0f;
        } else if (m_fTiltFrame < 14.0f) {
            m_fFlickDir = m_fTiltDir;
        }
        m_fTiltFrame = 0.0f;
    } else {
        m_fNeutralFrame = 0.0f;
        m_fTiltFrame += 1.0f;
        m_fTiltDir = m_fStickDir;
        if (m_fTiltFrame >= 14.0f) {
            m_fTiltDir = -1.0f;
            m_fFlickDir = -1.0f;
        }
    }

    m_nStickRoll &= ~(ROLL_RIGHT | ROLL_LEFT);
    CheckStickParam();

    if (CGame::g_GameWork.GetGameType() == CGame::GAME_TYPE_PARTY) {
        MainCrashSkate_(pMsg);
    } else if (m_pChr->GetStatusFlag() & 1) {
        MainOffenseOnBall_(pMsg);
    } else if (m_pChr->GetTeamNo() == CGame::g_GameWork.m_nUserTeam
               || (long)(CGame::g_GameWork.m_nFlag18 & 1) == 1) {
        MainOffense_(pMsg);
    } else {
        MainDefense_(pMsg);
    }

    if ((m_nInputFlag & INPUT_ITEM) && !m_pChr->CanbeItemShoot()) {
        m_nInputFlag &= ~INPUT_ITEM;
    }
    return 0;
}

inline void CChrDrvPadHockey::ClearStickParam_()
{
    m_fStickEnableFrame = 0.0f;
    m_fStickFrame = 0.0f;
    m_nUnkD0 = 0;
    m_nStickFlag = 0;
    m_nStickArea = 0;
    m_fStickStartDeg = -1.0f;
    m_fStickLastDeg = -1.0f;
}

void CChrDrvPadHockey::CheckStickParam()
{
    static const float fEnableFrame = 10.0f;

    if (m_fStickPower < 0.4f) {
        ClearStickParam_();
        return;
    }

    float fDeg = (180.0f / 3.1415927f) * GetStickDir();

    if (!(m_nStickFlag & STICK_ON)) {
        m_nStickFlag |= STICK_ON;
        m_fStickEnableFrame = fEnableFrame;
        m_fStickStartDeg = fDeg;
        m_fStickLastDeg = fDeg;
        m_fStickFrame = 0.0f;
        m_nStickArea |= 1 << (long)(fDeg / 45.0f);
        return;
    }

    m_fStickFrame += m_pChr->GetFrameStep();
    if (m_fStickFrame >= 25.0f) {
        ClearStickParam_();
        return;
    }
    m_fStickEnableFrame -= m_pChr->GetFrameStep();
    if (m_fStickEnableFrame <= 0.0f) {
        ClearStickParam_();
        return;
    }

    // Turns smaller than fMinRot are not counted; a turn starts with one
    // of up to fStartRot.
    float fMinRot = 0.08726646f;
    float fStartRot = 0.7853982f;
    float fDiff = RoundRadPM(0.017453292f * (fDeg - m_fStickLastDeg));
    float fAbs = fabsf(fDiff);
    if (fAbs < fMinRot) {
        return;
    }

    if (((m_nStickFlag >> 1) & 1) == 1) {
        if (fDiff > 0.0f) {
            m_fStickLastDeg = fDeg;
            float fTurn = RoundRadPM(0.017453292f * (fDeg - m_fStickStartDeg));
            if (!(m_nStickFlag & STICK_HALF)) {
                if (fTurn < 0.0f) {
                    m_nStickFlag |= STICK_HALF;
                }
            } else if (fabsf(fTurn) <= 2.443461f) {
                m_nStickRoll |= ROLL_RIGHT;
                ClearStickParam_();
            }
            m_fStickEnableFrame = fEnableFrame;
        }
    } else if (((m_nStickFlag >> 2) & 1) == 1) {
        if (fDiff < 0.0f) {
            m_fStickLastDeg = fDeg;
            float fTurn = RoundRadPM(0.017453292f * (fDeg - m_fStickStartDeg));
            if (!(m_nStickFlag & STICK_HALF)) {
                if (fTurn > 0.0f) {
                    m_nStickFlag |= STICK_HALF;
                }
            } else if (fabsf(fTurn) <= 2.443461f) {
                m_nStickRoll |= ROLL_LEFT;
                ClearStickParam_();
            }
            m_fStickEnableFrame = fEnableFrame;
        }
    } else if (fAbs <= fStartRot) {
        if (fDiff > 0.0f) {
            m_nStickFlag |= STICK_RIGHT;
        } else {
            m_nStickFlag |= STICK_LEFT;
        }
        m_fStickLastDeg = fDeg;
        m_fStickEnableFrame = fEnableFrame;
    }
}

// C (Nunchuk), or B held for six frames (on the Remote).
inline int CChrDrvPadHockey::IsChgLeaderInput_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsTrgKey_(6)) {
            return 1;
        }
    } else if (IsTrgB()) {
        m_nInputFlag |= INPUT_CHANGE;
    } else if (IsPressB()) {
        if (m_fHoldB >= 6.0f && (m_nInputFlag & INPUT_CHANGE)) {
            m_nInputFlag &= ~INPUT_CHANGE;
            return 1;
        }
    } else {
        m_nInputFlag &= ~INPUT_CHANGE;
    }
    return 0;
}

// A and B (Nunchuk), or 1 and 2, held together.
inline int CChrDrvPadHockey::IsArtsInput_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsPressA() && IsPressB()) {
            m_nInputFlag = (m_nInputFlag & ~(INPUT_TACKLE | INPUT_ITEM)) | INPUT_ARTS;
            return 1;
        }
    } else if (IsPress1() && IsPress2()) {
        m_nInputFlag = (m_nInputFlag & ~(INPUT_TACKLE | INPUT_ITEM)) | INPUT_ARTS;
        return 1;
    }
    return 0;
}

// The release of a button tracked by m_PressInfo and Z (Nunchuk), or 2
// held and a swing.
inline int CChrDrvPadHockey::IsFeintInput_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsPressKey_(11) && IsPressKey_(7)) {
            return 1;
        }
    } else if (IsPress2() && IsStroke_()) {
        return 1;
    }
    return 0;
}

// The release of the first button of nKeyTbl (Nunchuk), or 1 released.
inline int CChrDrvPadHockey::IsPassInput_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsPressKey_(11)) {
            return 1;
        }
    } else if (IsRelease1()) {
        return 1;
    }
    return 0;
}

// A swing; the Nunchuk only.
inline int CChrDrvPadHockey::IsSwingInput_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsStroke_()) {
            return 1;
        }
    }
    return 0;
}

// Brakes, runs or skates towards fDir with the stick.
inline void CChrDrvPadHockey::Move_(float fDir)
{
    if (m_pChr->MethdSwitchBrake(fDir, m_fStickPower)) {
        ClearFlick_();
        return;
    }
    if (GetStickDir() >= 0.0f) {
        if (m_pChr->MethdRun(fDir, m_fStickPower)) {
            m_fNeutralFrame = 0.0f;
            m_fFlickDir = -1.0f;
            m_fTiltDir = -1.0f;
            m_fTiltFrame = 9999.9f;
            return;
        }
        if (m_pChr->MethdSkate(fDir, 0, m_fStickPower, 0)) {
            m_fNeutralFrame = 0.0f;
            m_fFlickDir = -1.0f;
            m_fTiltDir = -1.0f;
            m_fTiltFrame = 9999.9f;
        }
    }
}

void CChrDrvPadHockey::MainOffenseOnBall_(_CHR_MSG *)
{
    float fDir = m_pChr->GetCamRotY();

    if (m_pChr->GetStatusFlag2() & 0x0A000000) {
        return;
    }
    // A call in the original; ours inlines it (see notes/toolchain.md).
    if (UpdateCmdBuf_() != 0) {
        return;
    }
    // The result is not used.
    bcs::CWifiPad::IsRemoconStroke(0.0f, m_pChr->GetPadNo());

    if (GetStickDir() >= 0.0f) {
        fDir = RoundRad(fDir + m_fStickDir);
    } else {
        fDir = -1.0f;
    }
    m_pChr->MethdHockeySetMoveRevise(fDir, m_fStickPower);

    if (m_pChr->GetStatusFlag() & 0x40400) {
        return;
    }

    if (IsChgLeaderInput_() && m_pChr->MethdChgLeader()) {
        return;
    }
    if (IsArtsInput_() && m_pChr->MethdAirTrick()) {
        return;
    }
    if (IsFeintInput_() && m_pChr->MethdFeintShoot(fDir)) {
        return;
    }
    if (IsPassInput_() && m_pChr->MethdPassStance(fDir, 0)) {
        return;
    }
    if (IsStanceInput_() && m_pChr->MethdShootStance(fDir, 0)) {
        return;
    }
    if (IsItemBallInput_() && m_pChr->MethdItemBallShoot(fDir)) {
        return;
    }
    if (IsShootInput_() && m_pChr->MethdShoot(-1.0f)) {
        return;
    }
    if (IsSwingInput_() && m_pChr->MethdBlockAttack(0)) {
        return;
    }
    Move_(fDir);
}

void CChrDrvPadHockey::MainOffense_(_CHR_MSG *)
{
    float fDir = m_pChr->GetCamRotY();

    if (m_pChr->GetStatusFlag2() & 0x0A000000) {
        return;
    }
    // A call in the original; ours inlines it (see notes/toolchain.md).
    if (UpdateCmdBuf_() != 0) {
        return;
    }
    // The result is not used.
    bcs::CWifiPad::IsRemoconStroke(0.0f, m_pChr->GetPadNo());

    if (GetStickDir() >= 0.0f) {
        fDir = RoundRad(fDir + m_fStickDir);
    } else {
        fDir = -1.0f;
    }
    m_pChr->MethdHockeySetMoveRevise(fDir, m_fStickPower);

    if (m_pChr->GetStatusFlag() & 0x40400) {
        return;
    }

    if (IsChgLeaderInput_() && m_pChr->MethdChgLeader()) {
        return;
    }
    if (IsKeeperInput_()) {
        m_pChr->MethdKeeperDivingCatch();
    }
    if (IsFeintInput_() && m_pChr->MethdFeintShoot(fDir)) {
        return;
    }
    if (IsPassInput_() && MethdPassCall_(0)) {
        return;
    }
    if (IsItem_() && m_pChr->MethdItemShoot(fDir)) {
        return;
    }
    if (IsSwingInput_()) {
        if (m_pChr->MethdDivingSteal(fDir, 0, 1)) {
            return;
        }
        if (m_pChr->MethdBlockAttack(0)) {
            return;
        }
    }
    if (IsStanceInput_()) {
        int bB = 0;
        if (IsPadTypeAnalogStick_() && IsPressB()) {
            bB = 1;
        }
        if (m_pChr->MethdOffBallStance(fDir, bB)) {
            return;
        }
    }
    Move_(fDir);
}

void CChrDrvPadHockey::MainDefense_(_CHR_MSG *)
{
    float fDir = m_pChr->GetCamRotY();

    if (m_pChr->GetStatusFlag2() & 0x0A000000) {
        return;
    }
    // A call in the original; ours inlines it (see notes/toolchain.md).
    if (UpdateCmdBuf_() != 0) {
        return;
    }
    // The result is not used.
    bcs::CWifiPad::IsRemoconStroke(0.0f, m_pChr->GetPadNo());

    if (GetStickDir() >= 0.0f) {
        fDir = RoundRad(fDir + m_fStickDir);
    } else {
        fDir = -1.0f;
    }
    m_pChr->MethdHockeySetMoveRevise(fDir, m_fStickPower);

    if (m_pChr->GetStatusFlag() & 0x40400) {
        return;
    }

    if (IsChgLeaderInput_() && m_pChr->MethdChgLeader()) {
        return;
    }
    if (IsKeeperInput_()) {
        m_pChr->MethdKeeperDivingCatch();
    }
    if (IsItem_() && m_pChr->MethdItemShoot(fDir)) {
        return;
    }
    if (DefenseTackle_()) {
        return;
    }
    Move_(fDir);
}

// The party game (crash skate).
void CChrDrvPadHockey::MainCrashSkate_(_CHR_MSG *)
{
    float fDir = m_pChr->GetCamRotY();

    if (m_pChr->GetStatusFlag2() & 0x0A000000) {
        return;
    }
    // A call in the original; ours inlines it (see notes/toolchain.md).
    if (UpdateCmdBuf_() != 0) {
        return;
    }
    // The result is not used.
    bcs::CWifiPad::IsRemoconStroke(0.0f, m_pChr->GetPadNo());

    if (GetStickDir() >= 0.0f) {
        fDir = RoundRad(fDir + m_fStickDir);
    } else {
        fDir = -1.0f;
    }
    m_pChr->MethdHockeySetMoveRevise(fDir, m_fStickPower);

    if (m_pChr->GetStatusFlag() & 0x40400) {
        return;
    }

    if (KSAttack_()) {
        return;
    }
    Move_(fDir);
}

// Calls for a pass from the team mate that has the puck, when he is played
// by the computer and (for a keeper) faces the character.
int CChrDrvPadHockey::MethdPassCall_(long)
{
    if (!IsPassCallState(m_pChr->GetState())) {
        return 0;
    }

    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CChrCore *pChr = CChrMan::Util_SearchHaveBallChr(i);
        if (pChr == 0) {
            continue;
        }
        if (pChr->GetTeamNo() != m_pChr->GetTeamNo() || m_pChr->GetPlayerNo() == pChr->GetPlayerNo()) {
            continue;
        }
        if (pChr->IsPadCtrl()) {
            continue;
        }
        if (pChr->IsKeeper()) {
            bcs::FVEC3 vec;
            const bcs::FVEC3 &rPos = pChr->GetPos();
            VecSub(vec, m_pChr->GetPos(), rPos);
            float fRot = nw4r::math::Atan2Rad(vec.x, vec.z);
            float fDiff = RoundRadPM(pChr->GetGuardGoalYRot() - fRot);
            if (fabsf(fDiff) > 1.5707964f) {
                continue;
            }
        }
        pChr->MethdPass(m_pChr, 0);
        m_pChr->PlayVoice(0x70, 0);
        pChr->DriverSendMessage((CChrDrv::MSG_ID)0x52, m_pChr->GetPlayerNo(), 0, 0, 0);
        return 1;
    }
    return 0;
}

// A swing (Nunchuk), or 2 released within 15 frames: a tackle.
int CChrDrvPadHockey::DefenseTackle_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsStroke_()) {
            if (m_pChr->MethdDivingSteal(-1.0f, 0, 0)) {
                return 1;
            }
            if (m_pChr->MethdBlockAttack(0)) {
                return 1;
            }
        }
    } else if (IsTrg2()) {
        m_nInputFlag |= INPUT_TACKLE;
    } else if (IsPress2()) {
        if (m_fHold2 >= 15.0f && (m_nInputFlag & INPUT_TACKLE)) {
            m_nInputFlag &= ~INPUT_TACKLE;
        }
    } else if (IsRelease2()) {
        if (m_nInputFlag & INPUT_TACKLE) {
            if (m_pChr->MethdDivingSteal(-1.0f, 0, 0)) {
                return 1;
            }
            if (m_pChr->MethdBlockAttack(0)) {
                return 1;
            }
        }
        m_nInputFlag &= ~INPUT_TACKLE;
    }
    return 0;
}

// The attacks of the crash skate: A or Z (Nunchuk), or 2 held for ten
// frames, smashes; a swing, or 2 released earlier, attacks.
int CChrDrvPadHockey::KSAttack_()
{
    if (IsPadTypeAnalogStick_()) {
        if ((IsPressA() || IsPressZ()) && m_pChr->MethdSmashAttack(-1.0f)) {
            return 1;
        }
        if (bcs::CWifiPad::IsRemoconStroke(0.0f, m_pChr->GetPadNo()) && m_pChr->MethdBlockAttack(0)) {
            return 1;
        }
    } else {
        if (IsTrg2()) {
            m_nInputFlag |= INPUT_TACKLE;
        } else if (IsRelease2()) {
            if (m_fHold2 < 10.0f && (m_nInputFlag & INPUT_TACKLE) && m_pChr->MethdBlockAttack(0)) {
                return 1;
            }
            m_nInputFlag &= ~INPUT_TACKLE;
        }
        if (IsPress2() && m_fHold2 >= 10.0f && (m_nInputFlag & INPUT_TACKLE)
            && m_pChr->MethdSmashAttack(-1.0f)) {
            return 1;
        }
    }
    return 0;
}

// B (Nunchuk: on release, or after five frames held), or A without 2.
int CChrDrvPadHockey::IsItem_()
{
    if (IsPadTypeAnalogStick_()) {
        if (IsTrgB()) {
            m_nInputFlag |= INPUT_ITEM;
        } else if (IsReleaseB()) {
            unsigned long nFlag = m_nInputFlag;
            m_nInputFlag = nFlag & ~INPUT_ITEM;
            if ((nFlag >> 1) & 1) {
                return 1;
            }
        }
        if (IsPressB() && m_fHoldB > 5.0f && (m_nInputFlag & INPUT_ITEM)) {
            return 1;
        }
    } else if (IsTrgA() && !IsPress2()) {
        return 1;
    }
    return 0;
}

// Counts the frames each button has been held.
void CChrDrvPadHockey::UpdateFunc_()
{
    if (IsPressA() || IsTrgA()) {
        m_fHoldA += bcs::CMainApp::GetAppFrameRateF();
    } else {
        m_fHoldA = 0.0f;
    }
    if (IsPressB() || IsTrgB()) {
        m_fHoldB += bcs::CMainApp::GetAppFrameRateF();
    } else {
        m_fHoldB = 0.0f;
    }
    if (IsPress1() || IsTrg1()) {
        m_fHold1 += bcs::CMainApp::GetAppFrameRateF();
    } else {
        m_fHold1 = 0.0f;
    }
    if (IsPress2() || IsTrg2()) {
        m_fHold2 += bcs::CMainApp::GetAppFrameRateF();
    } else {
        m_fHold2 = 0.0f;
    }
    if (IsReleaseA() && IsReleaseB()) {
        m_nInputFlag &= ~INPUT_ARTS;
    }
}

void CChrDrvPadHockey::ClearFunc_()
{
    ClearStroke_(0.0f);
    m_fHoldA = 0.0f;
    m_fHoldB = 0.0f;
    m_fHold1 = 0.0f;
    m_fHold2 = 0.0f;
    m_nInputFlag &= ~INPUT_ARTS;
}

int CChrDrvPadHockey::CheckTrg(long nKey) const
{
    switch (nKey) {
    case 1:
        if (IsTrgKeyID(4)) {
            return 1;
        }
        break;
    case 4:
        if (IsTrgA()) {
            return 1;
        }
        break;
    case 5:
        if (IsTrgB()) {
            return 1;
        }
        break;
    case 6:
        if (IsTrgC()) {
            return 1;
        }
        break;
    case 7:
        if (IsTrgZ()) {
            return 1;
        }
        break;
    case 8:
        if ((IsTrgA() && IsPressB()) || (IsPressA() && IsTrgB())) {
            return 1;
        }
        break;
    case 9:
        if ((IsTrgA() && IsPressZ()) || (IsPressA() && IsTrgZ())) {
            return 1;
        }
        break;
    case 10:
        if ((IsTrgB() && IsPressZ()) || (IsPressB() && IsTrgZ())) {
            return 1;
        }
        break;
    case 2:
    case 15:
    case 16:
        if (IsTrgKeyID(4) && !(m_fStickDir2 < 0.0f)) {
            long nQuarter = GetQuarter(m_fStickDir2);
            if (nQuarter == 0 || nQuarter == 2) {
                if (nKey == 15 && nQuarter != 0) {
                    return 0;
                }
                if (nKey == 16 && nQuarter != 2) {
                    return 0;
                }
                return 1;
            }
        }
        break;
    case 3:
    case 17:
    case 18:
        if (IsTrgKeyID(4) && !(m_fStickDir2 < 0.0f)) {
            long nQuarter = GetQuarter(m_fStickDir2);
            if (nQuarter == 1 || nQuarter == 3) {
                if (nKey == 17 && nQuarter != 1) {
                    return 0;
                }
                if (nKey == 18 && nQuarter != 3) {
                    return 0;
                }
                return 1;
            }
        }
        break;
    }
    return 0;
}

// Button nIdx of nKeyTbl released this frame, after a press.
inline int CChrDrvPadHockey::IsPressRelease_(long nIdx) const
{
    if (m_PressInfo[nIdx].m_bPress == 0 && m_PressInfo[nIdx].m_bPressed == 1
        && m_PressInfo[nIdx].m_nReleaseFrame == 0) {
        return 1;
    }
    return 0;
}

int CChrDrvPadHockey::CheckPress(long nKey) const
{
    switch (nKey) {
    case 1:
        if (IsPressKeyID(4)) {
            return 1;
        }
        break;
    case 4:
        if (IsPressA()) {
            return 1;
        }
        break;
    case 5:
        if (IsPressB()) {
            return 1;
        }
        break;
    case 6:
        if (IsPressC()) {
            return 1;
        }
        break;
    case 7:
        if (IsPressZ()) {
            return 1;
        }
        break;
    case 8:
        if (IsPressA() && IsPressB()) {
            return 1;
        }
        break;
    case 9:
        if (IsPressA() && IsPressZ()) {
            return 1;
        }
        break;
    case 10:
        if (IsPressB() && IsPressZ()) {
            return 1;
        }
        break;
    case 2:
    case 15:
    case 16:
        if (!(m_fStickDir2 < 0.0f)) {
            long nQuarter = GetQuarter(m_fStickDir2);
            if (nQuarter == 0 || nQuarter == 2) {
                if (nKey == 15 && nQuarter != 0) {
                    return 0;
                }
                if (nKey == 16 && nQuarter != 2) {
                    return 0;
                }
                return 1;
            }
        }
        break;
    case 3:
    case 17:
    case 18:
        if (!(m_fStickDir2 < 0.0f)) {
            long nQuarter = GetQuarter(m_fStickDir2);
            if (nQuarter == 1 || nQuarter == 3) {
                if (nKey == 17 && nQuarter != 1) {
                    return 0;
                }
                if (nKey == 18 && nQuarter != 3) {
                    return 0;
                }
                return 1;
            }
        }
        break;
    case 11:
        if (IsPressRelease_(0)) {
            return 1;
        }
        break;
    case 12:
        if (IsPressRelease_(1)) {
            return 1;
        }
        break;
    case 13:
        if (IsPressRelease_(2)) {
            return 1;
        }
        break;
    case 14:
        if (IsPressRelease_(3)) {
            return 1;
        }
        break;
    }
    return 0;
}

int CChrDrvPadHockey::CheckStroke(long nExcept, long nType) const
{
    long nStroke = 0;
    for (long i = 1; i < 19; i++) {
        if (m_nTrgStroke[i] != 0 && nExcept != i && CheckPress(i)) {
            nStroke |= m_nTrgStroke[i];
        }
    }
    if (nStroke != 0 && (nType == 1 || nType == 2)) {
        if (nStroke == 1) {
            nType = 5;
        } else {
            nType = nStroke == 2 ? 4 : 0;
        }
    }
    return CheckStrokeRaw(nType);
}

int CChrDrvPadHockey::CheckStrokeRaw(long nType) const
{
    switch (nType) {
    case 1:
        if (IsRemoconStroke(0.45f)) {
            return 1;
        }
        break;
    case 2:
        if (m_pChr != 0 ? bcs::CWifiPad::IsTateStroke(m_pChr->GetPadNo()) : 0) {
            return 1;
        }
        break;
    case 3:
        if (m_pChr != 0 ? bcs::CWifiPad::IsYokoStroke(m_pChr->GetPadNo()) : 0) {
            return 1;
        }
        break;
    case 4:
        if (m_nStrokeType == 3) {
            return 1;
        }
        break;
    case 5:
        if (m_nStrokeType == 0) {
            return 1;
        }
        break;
    case 6:
        if (bcs::CWifiPad::IsNunchukStroke(0.65f, m_pChr->GetPadNo())) {
            return 1;
        }
        break;
    case 7:
        if (bcs::CWifiPad::IsBothHandStroke(0.2f, 0.2f, m_pChr->GetPadNo())) {
            return 1;
        }
        break;
    }
    return 0;
}

// Steps the timers of m_CmdBuf.
int CChrDrvPadHockey::UpdateCmdBuf_()
{
    m_bCmdBufReady = 0;
    CMD_BUF *pBuf = m_CmdBuf;
    for (long i = 0; i < 3; i++, pBuf++) {
        if (pBuf->m_fTime < 0.0f) {
            continue;
        }
        pBuf->m_fTime -= 1.0f;
        if (!(pBuf->m_fTime > 0.0f)) {
            pBuf->m_fTime = -1.0f;
        }
    }
    m_bCmdBufReady = 1;
    return 0;
}


// Counts the frames the buttons of nKeyTbl are held and released.
inline void CChrDrvPadHockey::UpdatePressInfo()
{
    static const long nKeyTbl[5] = { 4, 5, 6, 7, 1 };

    for (long i = 0; i < 5; i++) {
        long bPress = CheckPress(nKeyTbl[i]);
        if (bPress != m_PressInfo[i].m_bPress) {
            if (bPress) {
                m_PressInfo[i].m_nPressFrame = 0;
                m_PressInfo[i].m_bPressed = 1;
            } else {
                m_PressInfo[i].m_nReleaseFrame = 0;
            }
        } else if (bPress) {
            m_PressInfo[i].m_nPressFrame++;
        } else {
            m_PressInfo[i].m_nReleaseFrame++;
        }
        m_PressInfo[i].m_bPress = bPress;
    }
}

}
