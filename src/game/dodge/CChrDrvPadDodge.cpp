// msw::CChrDrvPadDodge: the pad driver of the dodgeball game. It turns the
// Wii Remote (held sideways, or with the Nunchuk) into the character's
// actions, and calls on a CPU team mate who holds a ball to pass or shoot.
#include "game/msw/CChrDrvPadDodge.hpp"

#include <math.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CWifiPad.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCoreDodge.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameDodge.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGameVib.hpp"

namespace msw {

namespace {

// Set by the file's static initialiser and used by nothing in the file, as
// in about 60 other files of the game modules (see CBallManBasket.cpp).
float s_fAngle60 = bcs::DEG_TO_RAD(60.0f);

// The driver's timings, in frames (names guessed). The original reads them
// from memory (first in the file's .rodata) instead of folding them, also
// where the result is known (m_fTapReleaseFrame in GetPassSide_); const
// float scalars would be folded by this compiler.
struct PAD_PRM {
    // How long a call of UpdateCmdBuf_ stays queued (the frames message
    // 0x4F answers): the unused first slot, MethdPassCall, MethdShootCall.
    float m_fCmdBufFrame[3];
    // Above 0 a pass is a tap released (IsReleaseTap_), not a press.
    float m_fTapReleaseFrame;
    float m_fChargeFrame;      // message 0x23: frames held for a charge
    float m_fLongPressFrame;   // a press held this long is no tap
    float m_fShakeClearFrame;  // the Remote swing forgotten after a shake
};

const PAD_PRM s_Prm = { { 30.0f, 0.0f, 0.0f }, 6.0f, 15.0f, 6.0f, 7.0f };

const float PI = 3.1415927f;
const float PI2 = 6.2831855f;

// Brings an angle into [0, 2pi].
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

// Brings an angle into [-pi, pi].
inline float RoundRadPM(float fRad)
{
    fRad = RoundRad(fRad);
    if (fRad > PI) {
        fRad -= PI2;
    }
    return fRad;
}

// The quarter of a turn (0 to 3) direction fDir points to, quarters
// centred on 0, pi/2, pi and 3pi/2.
inline long GetQuarter(float fDir)
{
    return RoundRad(0.7853982f + fDir) / 1.5707964f;
}

}

long CChrDrvPadDodge::m_nTrgStroke[30];

// Forgets the state of the keys of UpdatePressInfo (not the flags).
inline void CChrDrvPadDodge::ClearPressInfo_()
{
    for (long i = 0; i < 7; i++) {
        m_PressInfo[i].m_bPress = 0;
        m_PressInfo[i].m_nPressFrame = 0;
        m_PressInfo[i].m_nReleaseFrame = 0;
    }
}

CChrDrvPadDodge::CChrDrvPadDodge()
{
    m_fSwitchDir = -1.0f;
    m_fUnkA0 = 0.0f;
    m_bCatchReady = 0;
    m_nFrame = 0;
    m_bOuterAssigned = 0;
    for (long i = 0; i < 30; i++) {
        m_nTrgStroke[i] = 0;
    }
    m_bCmdBufReady = 1;
    for (long i = 0; i < 3; i++) {
        m_CmdBuf[i].m_fTime = -1.0f;
    }
    ClearPressInfo_();
}

CChrDrvPadDodge::~CChrDrvPadDodge()
{
}

void CChrDrvPadDodge::ClearFunc_()
{
    CChrDrvPad::ClearFunc_();
    m_nFrame = 0;
    m_bOuterAssigned = 0;
}

// The PRESS_* flags of key id nKey that a charge needs.
inline unsigned long CChrDrvPadDodge::GetPressFlag_(long nKey) const
{
    long nIdx = GetPressInfoIdx(nKey);
    if (nIdx < 0) {
        return 0;
    }
    return m_PressInfo[nIdx].m_nFlag & (PRESS_TAP | PRESS_RELEASE);
}

// The kind of catch the held keys ask for (messages 0x19 and 0x44): 1, 5,
// 2 (outfield, B) or 0. Key id 0 is no key: CheckPress answers 0 for it.
inline long CChrDrvPadDodge::GetCatchType_() const
{
    if (!(m_pChr->GetStatusFlag() & 1)) {
        if (IsPadTypeAnalogStick_() ? CheckPress(0) : CheckPress(0)) {
            return 1;
        }
        if (IsPadTypeAnalogStick_() ? CheckPress(5) : CheckPress(5)) {
            return 5;
        }
        if (IsPadTypeAnalogStick_() ? CheckPress(0) : CheckPress(0)) {
            return 5;
        }
    } else if (IsPadTypeAnalogStick_() ? CheckPress(5) : CheckPress(5)) {
        return 2;
    }
    return 0;
}

// The checks as bools. The result is set after the call, as in the
// original (cmpwi, li 0, beq, li 1).
inline bool CChrDrvPadDodge::IsStroke_(long nType) const
{
    int nRet = CheckStroke(0, nType);
    bool bRet = false;
    if (nRet) {
        bRet = true;
    }
    return bRet;
}

inline bool CChrDrvPadDodge::IsTrgKey_(long nKey) const
{
    int nRet = CheckTrg(nKey);
    bool bRet = false;
    if (nRet) {
        bRet = true;
    }
    return bRet;
}

// Key id nKey held and a swing of the Remote (other keys than nKey).
inline bool CChrDrvPadDodge::IsPressStroke_(long nKey) const
{
    int bPress = CheckPress(nKey);
    int bStroke = CheckStroke(nKey, 1);
    bool bRet = false;
    if (bPress && bStroke) {
        bRet = true;
    }
    return bRet;
}

// Starts the key states afresh; a key held through it is no tap.
inline void CChrDrvPadDodge::ResetInput_()
{
    ClearWork_();
    ClearPressInfo_();
    UpdatePressInfo();
    PRESS_INFO *pInfo = m_PressInfo;
    for (long i = 0; i < 7; i++, pInfo++) {
        if (pInfo->m_bPress) {
            pInfo->m_nFlag &= ~(PRESS_TAP | PRESS_RELEASE);
        }
    }
}

long CChrDrvPadDodge::OnMsg(MSG_ID id, _CHR_MSG *pMsg)
{
    long nRet = 0;

    switch (id) {
    case 0x00:
        if (m_pChr == 0) {
            nRet = 0;
        } else {
            UpdatePadState();
            UpdatePressInfo();
            MainDefense_(pMsg);
            m_nFrame++;
            nRet = 0;
        }
        break;
    case 0x01:
        ResetInput_();
        break;
    case 0x02:
        if (m_pChr != 0) {
            bcs::CWifiPad::StopMotor(m_pChr->GetPadNo());
        }
        break;
    case 0x03:
        ResetInput_();
        break;
    case 0x04:
        ResetInput_();
        break;
    case 0x08: {
        long nKind = 0;
        if (pMsg != 0) {
            nKind = pMsg->m_nParam0;
        }
        if (nKind == 9 || nKind == 10) {
            if (m_pChr != 0) {
                CGameVib::Start(3, 3, -2, m_pChr->GetPadNo());
            }
        }
        break;
    }
    case 0x0A:
        if (m_pChr != 0) {
            long nKind = 0;
            if (pMsg != 0) {
                nKind = (unsigned char)pMsg->m_nParam0;
            }
            // Both kinds rumble the same.
            if (nKind == 1 || nKind == 5) {
                if (m_pChr != 0) {
                    CGameVib::Start(3, 3, -2, m_pChr->GetPadNo());
                }
            } else if (m_pChr != 0) {
                CGameVib::Start(3, 3, -2, m_pChr->GetPadNo());
            }
        }
        break;
    case 0x0D:
        ClearStrokeCmd(IsPadTypeAnalogStick_() ? 6 : 0x18, 0);
        break;
    case 0x19:
        if (GetCatchType_() != 0) {
            nRet = 1;
        }
        break;
    case 0x46:
        return (IsPadTypeAnalogStick_() ? CheckPress(5) : CheckPress(5)) != 0;
    case 0x47:
        return (IsPadTypeAnalogStick_() ? CheckPress(3) : CheckPress(3)) != 0;
    case 0x4B:
        // The stick: its push (per mille) and direction (tenths of a
        // degree), packed.
        if (GetStickDir() >= 0.0f) {
            float fDeg = (180.0f / PI) * RoundRad(m_pChr->GetCamRotY() + m_fStickDir);
            nRet = ((long)(1000.0f * m_fStickPower) << 16) | (long)(10.0f * fDeg);
        }
        break;
    case 0x44:
        return GetCatchType_();
    case 0x3B:
    case 0x3D:
    case 0x3E:
        nRet = 0;
        if (bcs::CWifiPad::GetNunchukPower(m_pChr->GetPadNo()) > 0.25f
            || bcs::CWifiPad::GetScratchPower(m_pChr->GetPadNo()) > 0.2f) {
            nRet = 0x100;
            ClearStroke_(s_Prm.m_fShakeClearFrame);
        }
        if (GetStickDir() >= 0.0f) {
            // A new direction once the stick turns more than 45 degrees.
            bool bNew = false;
            if (m_fSwitchDir < 0.0f) {
                bNew = true;
            } else if (fabsf(RoundRadPM(m_fStickDir - m_fSwitchDir)) > 0.7853982f) {
                bNew = true;
            }
            if (bNew) {
                m_fSwitchDir = m_fStickDir;
            }
        } else {
            m_fSwitchDir = -1.0f;
        }
        if (IsTrgA() || IsTrgB()) {
            // (nothing: the result is not used)
        }
        break;
    case 0x41:
        // The side (-1 or 1) the stick points to from ball nBall's flight,
        // 0 for ahead or behind.
        if (GetStickDir() >= 0.0f) {
            float fRot = RoundRad(m_pChr->GetCamRotY() + m_fStickDir);
            CBall *pBall = CBallMan::GetBallPtr(pMsg->m_nParam0);
            if (pBall != 0 && pBall->IsActive()) {
                bcs::FVEC3 vBall = pBall->GetVelocity();
                vBall.y = 0.0f;
                bcs::BCS_Vec_Normalize(vBall, vBall);
                bcs::FVEC3 vStick;
                bcs::BCS_VecZ_RotY(vStick, 1.0f, fRot);
                float fAngle = bcs::BCS_Vec_GetRot(vStick, vBall);
                bcs::FVEC3 vCross;
                PSVECCrossProduct((const Vec *)&vStick, (const Vec *)&vBall, (Vec *)&vCross);
                if (vCross.y > 0.0f) {
                    fAngle *= -1.0f;
                }
                if (fAngle > PI) {
                    fAngle -= PI2;
                }
                float fDeg = (180.0f / PI) * fAngle;
                if (fDeg < -30.0f && fDeg > -150.0f) {
                    nRet = -1;
                } else if (fDeg > 30.0f && fDeg < 150.0f) {
                    nRet = 1;
                }
            }
        }
        break;
    case 0x21:
        nRet = (IsPadTypeAnalogStick_() ? IsStroke_(1) : IsTrgKey_(0x18)) != 0;
        if (nRet) {
            ClearStroke_(0.0f);
        }
        break;
    case 0x40:
        if (IsPadTypeAnalogStick_() ? IsTrgKey_(6) : IsTrgKey_(0x18)) {
            nRet = 1;
        }
        break;
    case 0x42:
        if (IsPadTypeAnalogStick_() ? IsStroke_(1) : IsTrgKey_(0x18)) {
            if (IsPadTypeAnalogStick_()) {
                ClearStrokeCmd(0, 1);
            } else {
                ClearStrokeCmd(0x18, 0);
            }
            nRet = 1;
        }
        break;
    case 0x43:
        if (IsPadTypeAnalogStick_() ? CheckPress(0) : CheckPress(0)) {
            nRet = 1;
        }
        break;
    case 0x4A:
        if (IsPadTypeAnalogStick_() ? CheckPress(3) : CheckPress(2)) {
            nRet = 1;
        }
        break;
    case 0x49:
        if (IsPadTypeAnalogStick_() ? CheckPress(3) : CheckPress(2)) {
            nRet = 1;
        }
        break;
    case 0x53: {
        CChrCore *pChr = m_pChr;
        long nPattern = pMsg->m_nParam0;
        if (pChr != 0 && nPattern >= 0) {
            CGameVib::Start(3, nPattern, -2, pChr->GetPadNo());
        }
        break;
    }
    case 0x54:
        if (m_pChr != 0) {
            CGameVib::Stop(m_pChr->GetPadNo());
        }
        break;
    case 0x23:
        // A (Nunchuk) or 1 held; with nParam0 0, held long enough for a
        // charge.
        if (m_pChr->IsDodgeInfield()) {
            if (pMsg->m_nParam0 == 0) {
                if (IsPadTypeAnalogStick_() ? CheckPress(2) : CheckPress(0x17)) {
                    if (IsPadTypeAnalogStick_() ? GetPressFlag_(2) : GetPressFlag_(0x17)) {
                        long nFrame = IsPadTypeAnalogStick_() ? m_PressInfo[0].m_nPressFrame
                                                              : m_PressInfo[5].m_nPressFrame;
                        if ((float)nFrame >= s_Prm.m_fChargeFrame) {
                            nRet = 1;
                        }
                    }
                }
            } else if (IsPadTypeAnalogStick_() ? CheckPress(2) : CheckPress(0x17)) {
                nRet = 1;
            }
        }
        break;
    case 0x4D:
        if (GetStickDir() >= 0.0f) {
            nRet = 1;
        }
        break;
    case 0x4F:
        // How long a call is queued (UpdateCmdBuf_), with the Nunchuk.
        if (IsPadTypeAnalogStick_()) {
            switch (pMsg->m_nParam0) {
            case 0:
                nRet = s_Prm.m_fCmdBufFrame[0];
                break;
            case 1:
                nRet = s_Prm.m_fCmdBufFrame[1];
                break;
            case 2:
                nRet = s_Prm.m_fCmdBufFrame[2];
                break;
            }
        } else {
            nRet = 0;
        }
        break;
    case 0x50:
        nRet = IsPadTypeAnalogStick_() ? 0 : 0;
        break;
    }
    return nRet;
}

// A tap of key id nKey: let go before it was held long, this frame, or
// held long (PRESS_LONG, one frame).
inline int CChrDrvPadDodge::IsTap_(long nKey) const
{
    long nIdx = GetPressInfoIdx(nKey);
    if (nIdx < 0) {
        return 0;
    }
    const PRESS_INFO &rInfo = m_PressInfo[nIdx];
    if ((rInfo.m_bPress == 0 && (rInfo.m_nFlag & PRESS_TAP) && rInfo.m_nReleaseFrame == 0)
        || (rInfo.m_bPress != 0 && (rInfo.m_nFlag & PRESS_LONG))) {
        return 1;
    }
    return 0;
}

// Key id nKey let go this frame, held long or not.
inline int CChrDrvPadDodge::IsReleaseTap_(long nKey) const
{
    long nIdx = GetPressInfoIdx(nKey);
    if (nIdx < 0) {
        return 0;
    }
    const PRESS_INFO &rInfo = m_PressInfo[nIdx];
    if (rInfo.m_bPress == 0 && (rInfo.m_nFlag & PRESS_RELEASE) && rInfo.m_nReleaseFrame == 0) {
        return 1;
    }
    return 0;
}

// A pass asked for (A with the Nunchuk, 1 sideways): the side to pass to
// (-1 or 1, 1 without a direction), 0 for none.
inline long CChrDrvPadDodge::GetPassSide_() const
{
    IsPadTypeAnalogStick_();
    int bPass;
    if (s_Prm.m_fTapReleaseFrame <= 0.0f) {
        bPass = IsPadTypeAnalogStick_() ? IsTrgKey_(2) : IsTrgKey_(0x17);
    } else {
        bPass = IsReleaseTap_(IsPadTypeAnalogStick_() ? 2 : 0x17);
    }
    long nSide;
    if (bPass) {
        if (GetStickDir() >= 0.0f) {
            nSide = RoundRad(m_fStickDir) < PI ? 1 : -1;
        } else {
            nSide = 1;
        }
    } else {
        nSide = 0;
    }
    return nSide;
}

void CChrDrvPadDodge::MainDefense_(_CHR_MSG *)
{
    float fDir = -1.0f;
    float fCamRot = m_pChr->GetCamRotY();
    CChrCoreDodge *pChr;

    if (m_pChr != 0 && static_cast<CChrCoreDodge *>(m_pChr)->IsOuterAssigned()) {
        m_bOuterAssigned = 1;
    } else {
        m_bOuterAssigned = 0;
    }

    if (UpdateCmdBuf_()) {
        return;
    }

    if (GetStickDir() >= 0.0f) {
        static_cast<CChrCoreDodge *>(m_pChr)->MoveDodgeArtsTargetMarkerPos(RoundRad(fCamRot + m_fStickDir));
    }
    if (m_pChr->GetStatusFlag2() & 0x0A000000) {
        return;
    }
    if (GetStickDir() >= 0.0f) {
        fDir = RoundRad(fCamRot + m_fStickDir);
        m_pChr->MethdSetMoveRevise(fDir);
    }

    m_bCatchReady = 1;
    if (m_pChr != 0 && !m_pChr->IsHaveStockItem()) {
        if (IsPadTypeAnalogStick_()) {
            ClearStrokeCmd(3, 0);
        } else {
            ClearStrokeCmd(2, 0);
        }
    }

    pChr = static_cast<CChrCoreDodge *>(m_pChr);
    if (pChr->GetState() != 0x3B) {
        if (pChr->GetState() == 0x17) {
            // In the air: a shot.
            if (IsPadTypeAnalogStick_() ? IsStroke_(1) : IsTrgKey_(0x18)) {
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(0, 1);
                } else {
                    ClearStrokeCmd(0x18, 0);
                }
                m_pChr->MethdAerialShoot();
            }
        } else if (pChr->GetStatusFlag() & 1) {
            // Holding a ball.
            bool bCatch;
            if ((IsPadTypeAnalogStick_() ? IsTrgKey_(6) : IsTrgKey_(0x19)) && m_bCatchReady != 0) {
                bCatch = true;
            } else {
                bCatch = false;
            }
            if (bCatch) {
                ClearStrokeCmd(IsPadTypeAnalogStick_() ? 6 : 0x19, 0);
                if (!(m_pChr->GetStatusFlag() & 0x400) && m_pChr->MethdAirTrick()) {
                    ClearStroke_(0.0f);
                    return;
                }
            }

            bool bTrick;
            if (IsPadTypeAnalogStick_() ? IsStroke_(6) : IsStroke_(1)) {
                bTrick = true;
            } else {
                bTrick = (IsPadTypeAnalogStick_() ? IsPressStroke_(1) : false) != 0;
            }
            if (bTrick) {
                if (m_pChr->MethdTrickMove(fDir) || m_pChr->GetState() == 0x22) {
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(0, 6);
                    } else {
                        ClearStrokeCmd(0, 1);
                    }
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(1, 1);
                    } else {
                        ClearStrokeCmd(0, 0);
                    }
                    return;
                }
            }

            int bJump;
            if (!(m_pChr->GetStatusFlag() & 1) && (IsPadTypeAnalogStick_() ? IsTrgKey_(0xB) : IsStroke_(1))) {
                bJump = 1;
            } else {
                bJump = 0;
            }
            if (bJump && !(m_pChr->GetStatusFlag() & 0x400) && m_pChr->MethdJump()) {
                ClearStrokeCmd(0, 0);
                return;
            }

            if (GetPassSide_() != 0) {
                float fPassDir;
                if (GetStickDir() >= 0.0f && m_fStickPower >= 0.9f) {
                    fPassDir = fCamRot + m_fStickDir;
                } else {
                    fPassDir = -1.0f;
                }
                if (m_pChr->MethdPass(fPassDir, 1)) {
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(2, 0);
                    } else {
                        ClearStrokeCmd(0x17, 0);
                    }
                    return;
                }
            }

            if (IsPadTypeAnalogStick_() ? IsPressStroke_(3) : IsTrgKey_(2)) {
                if (m_pChr->MethdItemBallAttack()) {
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(3, 1);
                    } else {
                        ClearStrokeCmd(2, 0);
                    }
                    return;
                }
            }

            if (IsPadTypeAnalogStick_() ? IsPressStroke_(5) : IsTrgKey_(0x18)) {
                if (m_pChr->MethdSubShoot(-1.0f)) {
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(5, 1);
                    } else {
                        ClearStrokeCmd(0x18, 0);
                    }
                    return;
                }
            }

            if (IsPadTypeAnalogStick_() ? IsStroke_(1) : IsTrgKey_(0x18)) {
                if (m_pChr->MethdShoot(-1.0f)) {
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(0, 1);
                    } else {
                        ClearStrokeCmd(0x18, 0);
                    }
                    return;
                }
            }

            if (IsPadTypeAnalogStick_() ? IsTrgKey_(5) : false) {
                static_cast<CChrCoreDodge *>(m_pChr)->SwitchAutoAimTarget();
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(5, 0);
                } else {
                    ClearStrokeCmd(0, 0);
                }
                return;
            }

            bool bChange;
            if (m_nFrame > 1 && (IsPadTypeAnalogStick_() ? IsTrgKey_(4) : IsTrgKey_(0xC))) {
                bChange = true;
            } else {
                bChange = false;
            }
            if (bChange && MethdChrChange()) {
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(4, 0);
                } else {
                    ClearStrokeCmd(0xC, 0);
                }
                return;
            }
        } else {
            // Without a ball.
            if ((CGame::g_GameWork.m_nFlag18 & 8) && !(pChr->GetStatusFlag() & 1)) {
                if (IsPadTypeAnalogStick_() ? IsStroke_(1) : IsTrgKey_(0x18)) {
                    if (!(m_pChr->GetStatusFlag() & 0x400) && m_pChr->MethdJump()) {
                        if (IsPadTypeAnalogStick_()) {
                            ClearStrokeCmd(0, 1);
                        } else {
                            ClearStrokeCmd(0x18, 0);
                        }
                        return;
                    }
                }
            }

            if (IsPadTypeAnalogStick_() ? IsTrgKey_(6) : IsTrgKey_(0x18)) {
                if (m_pChr->MethdBallCatch(-1.0f)) {
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(6, 0);
                    } else {
                        ClearStrokeCmd(0x18, 0);
                    }
                    return;
                }
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(6, 0);
                } else {
                    ClearStrokeCmd(0x18, 0);
                }
            }

            if (IsPadTypeAnalogStick_() ? IsStroke_(1) : IsTrgKey_(0x17)) {
                float fDiveDir = -1.0f;
                if (GetStickDir() >= 0.0f) {
                    fDiveDir = fCamRot + m_fStickDir;
                }
                if (m_pChr->MethdDivingCatch(fDiveDir)) {
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(0, 1);
                    } else {
                        ClearStrokeCmd(0x17, 0);
                    }
                    return;
                }
            }

            if (GetPassSide_() != 0) {
                float fPassDir = -1.0f;
                if (GetStickDir() >= 0.0f && m_fStickPower >= 0.9f) {
                    fPassDir = RoundRad(fCamRot + m_fStickDir);
                }
                int bCalled = MethdPassCall(fPassDir);
                // (as in the original: the second test compares the pad
                // type with key id 0x18, so it is always true)
                long nKey = IsPadTypeAnalogStick_() ? 6 : (IsPadTypeAnalogStick_() != 0x18 ? 2 : 0);
                if (nKey != 0 && m_pChr->MethdTrickDance()) {
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(2, 0);
                    } else {
                        ClearStrokeCmd(0x17, 0);
                    }
                    return;
                }
                if (bCalled) {
                    return;
                }
            }

            if ((IsPadTypeAnalogStick_() ? IsStroke_(6) : false) && MethdFeintCall()) {
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(0, 6);
                } else {
                    ClearStrokeCmd(0, 0);
                }
                return;
            }

            if ((IsPadTypeAnalogStick_() ? IsPressStroke_(1) : false) && MethdFeintCall()) {
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(1, 1);
                } else {
                    ClearStrokeCmd(0, 0);
                }
                return;
            }

            if (IsPadTypeAnalogStick_() ? IsPressStroke_(2) : false) {
                if (MethdShootCall(1)) {
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(0, 1);
                    } else {
                        ClearStrokeCmd(0x18, 0);
                    }
                    return;
                }
            } else if ((IsPadTypeAnalogStick_() ? IsStroke_(1) : IsTrgKey_(0x18)) && MethdShootCall(0)) {
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(0, 1);
                } else {
                    ClearStrokeCmd(0x18, 0);
                }
                return;
            }

            if ((IsPadTypeAnalogStick_() ? IsTrgKey_(2) : IsTrgKey_(0x18)) && m_pChr->MethdSpinAttack()) {
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(2, 0);
                } else {
                    ClearStrokeCmd(0x18, 0);
                }
                return;
            }

            int bJump;
            if (!(m_pChr->GetStatusFlag() & 1) && (IsPadTypeAnalogStick_() ? IsTrgKey_(0xB) : IsStroke_(1))) {
                bJump = 1;
            } else {
                bJump = 0;
            }
            if (bJump && !(m_pChr->GetStatusFlag() & 0x400) && m_pChr->MethdJump()) {
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(0xB, 0);
                } else {
                    ClearStrokeCmd(0, 1);
                }
                return;
            }

            // A side step: towards the stick, or on the spot.
            if (GetStickDir() >= 0.0f) {
                if (IsPadTypeAnalogStick_() ? IsStroke_(1) : IsTrgKey_(0x17)) {
                    float fStepDir = fCamRot + m_fStickDir;
                    if (m_pChr->MethdSideStep(fStepDir, fStepDir)) {
                        if (IsPadTypeAnalogStick_()) {
                            ClearStrokeCmd(0, 1);
                        } else {
                            ClearStrokeCmd(0x17, 0);
                        }
                        return;
                    }
                }
            } else if (IsPadTypeAnalogStick_() ? IsStroke_(1) : IsTrgKey_(0x17)) {
                if (m_pChr->MethdSideStep(m_pChr->GetRotY(), -1.0f)) {
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(0, 1);
                    } else {
                        ClearStrokeCmd(0x17, 0);
                    }
                    return;
                }
            }

            IsPadTypeAnalogStick_();
            if (IsReleaseTap_(IsPadTypeAnalogStick_() ? 3 : 2) && m_pChr->MethdItemShoot(-1.0f)) {
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(3, 0);
                } else {
                    ClearStrokeCmd(2, 0);
                }
                return;
            }

            bool bChange;
            if (m_nFrame > 1 && (IsPadTypeAnalogStick_() ? IsTrgKey_(4) : IsTrgKey_(0xC))) {
                bChange = true;
            } else {
                bChange = false;
            }
            if (bChange && MethdChrChange()) {
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(4, 0);
                } else {
                    ClearStrokeCmd(0xC, 0);
                }
                return;
            }

            if (IsPadTypeAnalogStick_() ? IsTrgKey_(5) : false) {
                if (MethdSwitchTargetCall()) {
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(5, 0);
                    } else {
                        ClearStrokeCmd(0, 0);
                    }
                    return;
                }
            }

            if (IsPadTypeAnalogStick_() ? IsPressStroke_(5) : IsTrgKey_(0x18)) {
                if (m_pChr->MethdSubShoot(-1.0f)) {
                    if (IsPadTypeAnalogStick_()) {
                        ClearStrokeCmd(5, 1);
                    } else {
                        ClearStrokeCmd(0x18, 0);
                    }
                    return;
                }
            } else if ((IsPadTypeAnalogStick_() ? IsStroke_(1) : IsTrgKey_(0x18)) && m_pChr->MethdShoot(-1.0f)) {
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(0, 1);
                } else {
                    ClearStrokeCmd(0x18, 0);
                }
                return;
            }
        }
    }

    // Nothing done: run where the stick points.
    if (GetStickDir() >= 0.0f) {
        float fRunDir = RoundRad(fCamRot + m_fStickDir);
        m_pChr->MethdSliding(fRunDir);
        m_pChr->MethdRun(fRunDir, m_fStickPower);
    }
}

// Steps the timers of m_CmdBuf; a call still queued is retried until it
// succeeds or its time runs out.
int CChrDrvPadDodge::UpdateCmdBuf_()
{
    int nRet = 0;
    m_bCmdBufReady = 0;
    CMD_BUF *pBuf = m_CmdBuf;
    for (long i = 0; i < 3; i++, pBuf++) {
        if (pBuf->m_fTime < 0.0f) {
            continue;
        }
        pBuf->m_fTime -= 1.0f;
        if (pBuf->m_fTime > 0.0f) {
            switch (i) {
            case 1:
                nRet = MethdPassCall(-1.0f);
                break;
            case 2:
                nRet = MethdShootCall(pBuf->m_nParam);
                break;
            }
            if (nRet) {
                pBuf->m_fTime = -1.0f;
                break;
            }
        } else {
            pBuf->m_fTime = -1.0f;
        }
    }
    m_bCmdBufReady = 1;
    return nRet;
}

// Counts the frames the keys of nKeyTbl are held and released, and follows
// the stick's push.
void CChrDrvPadDodge::UpdatePressInfo()
{
    static const long nKeyTbl[7] = { 2, 3, 4, 5, 1, 0x17, 0x18 };

    for (long i = 0; i < 7; i++) {
        UpdatePressInfoSub_(i, CheckPress(nKeyTbl[i]));
    }

    if (m_bStickOn) {
        if (m_fStickPower < 0.3f) {
            m_bStickOn = 0;
            m_nStickOffFrame = 0;
        } else {
            m_nStickOnFrame++;
            if (m_fStickPower >= 0.8f) {
                m_fStickOnDir = m_fStickDir;
            }
        }
    } else if (m_fStickPower >= 0.8f) {
        m_bStickOn = 1;
        m_nStickOnFrame = 0;
        m_fStickOnDir = m_fStickDir;
        m_bStickOnce = 1;
    } else {
        m_nStickOffFrame++;
    }
    // Dead comparison (a guess): the original's .rodata has 0.6f here,
    // ahead of CheckTrg's constant, though only CheckStrokeRaw reads it.
    if (m_fStickPower == 0.6f) {
    }
}

void CChrDrvPadDodge::UpdatePressInfoSub_(long nIdx, int bPress)
{
    PRESS_INFO &rInfo = m_PressInfo[nIdx];
    if (rInfo.m_bPress != bPress) {
        if (bPress) {
            rInfo.m_nPressFrame = 0;
            rInfo.m_nFlag = PRESS_TAP | PRESS_RELEASE;
        } else {
            rInfo.m_nReleaseFrame = 0;
        }
    } else if (bPress) {
        rInfo.m_nPressFrame++;
        long nLong = s_Prm.m_fLongPressFrame;
        if ((rInfo.m_nFlag & PRESS_TAP) && nLong > 0 && rInfo.m_nPressFrame > nLong) {
            rInfo.m_nFlag = (rInfo.m_nFlag | PRESS_LONG) & ~PRESS_TAP;
        } else if (rInfo.m_nFlag & PRESS_LONG) {
            rInfo.m_nFlag &= ~PRESS_LONG;
        }
    } else {
        rInfo.m_nReleaseFrame++;
    }
    rInfo.m_bPress = bPress;
}

long CChrDrvPadDodge::GetPressInfoIdx(long nKey) const
{
    long nIdx = -1;
    switch (nKey) {
    case 1:
        nIdx = 4;
        break;
    case 2:
        nIdx = 0;
        break;
    case 3:
        nIdx = 1;
        break;
    case 4:
        nIdx = 2;
        break;
    case 5:
        nIdx = 3;
        break;
    case 0x17:
        nIdx = 5;
        break;
    case 0x18:
        nIdx = 6;
        break;
    }
    return nIdx;
}

int CChrDrvPadDodge::CheckTrg(long nKey) const
{
    switch (nKey) {
    case 1:
        if (IsTrgKeyID(4)) {
            return 1;
        }
        break;
    case 2:
        if (IsTrgA()) {
            return 1;
        }
        break;
    case 3:
        if (IsTrgB()) {
            return 1;
        }
        break;
    case 4:
        if (IsTrgC()) {
            return 1;
        }
        break;
    case 5:
        if (IsTrgZ()) {
            return 1;
        }
        break;
    case 6:
        if ((IsTrgA() && IsPressB()) || (IsPressA() && IsTrgB())) {
            return 1;
        }
        break;
    case 7:
        if ((IsTrgA() && IsPressZ()) || (IsPressA() && IsTrgZ())) {
            return 1;
        }
        break;
    case 8:
        if ((IsTrgB() && IsPressZ()) || (IsPressB() && IsTrgZ())) {
            return 1;
        }
        break;
    case 9:
        if (IsTrgKeyID(4) && !(m_fStickDir2 < 0.0f)) {
            long nQuarter = GetQuarter(m_fStickDir2);
            if (nQuarter == 0 || nQuarter == 2) {
                return 1;
            }
        }
        break;
    case 10:
        if (IsTrgKeyID(4) && !(m_fStickDir2 < 0.0f)) {
            long nQuarter = GetQuarter(m_fStickDir2);
            if (nQuarter == 1 || nQuarter == 3) {
                return 1;
            }
        }
        break;
    case 11:
        if (IsTap_(2)) {
            return 1;
        }
        break;
    case 12:
        if (IsTap_(3)) {
            return 1;
        }
        break;
    case 13:
        if (IsTap_(4)) {
            return 1;
        }
        break;
    case 14:
        if (IsTap_(5)) {
            return 1;
        }
        break;
    case 15:
        if (IsTrgKeyID(4) && IsPressB() && !(m_fStickDir2 < 0.0f)) {
            long nQuarter = GetQuarter(m_fStickDir2);
            if (nQuarter == 0 || nQuarter == 2) {
                return 1;
            }
        }
        break;
    case 16:
        if (IsTrgKeyID(4) && IsPressB() && !(m_fStickDir2 < 0.0f)) {
            long nQuarter = GetQuarter(m_fStickDir2);
            if (nQuarter == 1 || nQuarter == 3) {
                return 1;
            }
        }
        break;
    case 17:
        if (bcs::CWifiPad::IsTrg(0x10, m_nPadNo)) {
            return 1;
        }
        break;
    case 18:
        if (bcs::CWifiPad::IsTrg(0x1000, m_nPadNo)) {
            return 1;
        }
        break;
    case 19:
        if (bcs::CWifiPad::IsTrg(0x8, m_nPadNo)) {
            return 1;
        }
        break;
    case 20:
        if (bcs::CWifiPad::IsTrg(0x4, m_nPadNo)) {
            return 1;
        }
        break;
    case 21:
        if (bcs::CWifiPad::IsTrg(0x1, m_nPadNo)) {
            return 1;
        }
        break;
    case 22:
        if (bcs::CWifiPad::IsTrg(0x2, m_nPadNo)) {
            return 1;
        }
        break;
    case 23:
        if (bcs::CWifiPad::IsTrg(0x200, m_nPadNo)) {
            return 1;
        }
        break;
    case 24:
        if (bcs::CWifiPad::IsTrg(0x100, m_nPadNo)) {
            return 1;
        }
        break;
    case 25:
        if ((bcs::CWifiPad::IsTrg(0x200, m_nPadNo) && bcs::CWifiPad::IsDown(0x100, m_nPadNo))
            || (bcs::CWifiPad::IsTrg(0x100, m_nPadNo) && bcs::CWifiPad::IsDown(0x200, m_nPadNo))) {
            return 1;
        }
        break;
    case 26:
        if (IsTap_(0x17)) {
            return 1;
        }
        break;
    case 27:
        if (IsTap_(0x18)) {
            return 1;
        }
        break;
    case 28:
        if ((bcs::CWifiPad::IsTrg(0x400, m_nPadNo) && bcs::CWifiPad::IsDown(0x200, m_nPadNo))
            || (bcs::CWifiPad::IsTrg(0x200, m_nPadNo) && bcs::CWifiPad::IsDown(0x400, m_nPadNo))) {
            return 1;
        }
        break;
    case 29:
        if ((bcs::CWifiPad::IsTrg(0x400, m_nPadNo) && bcs::CWifiPad::IsDown(0x100, m_nPadNo))
            || (bcs::CWifiPad::IsTrg(0x100, m_nPadNo) && bcs::CWifiPad::IsDown(0x400, m_nPadNo))) {
            return 1;
        }
        break;
    }
    return 0;
}

int CChrDrvPadDodge::CheckPress(long nKey) const
{
    switch (nKey) {
    case 1:
        if (IsPressKeyID(4)) {
            return 1;
        }
        break;
    case 2:
        if (bcs::CWifiPad::IsDown(0x800, m_nPadNo)) {
            return 1;
        }
        break;
    case 3:
        if (bcs::CWifiPad::IsDown(0x400, m_nPadNo)) {
            return 1;
        }
        break;
    case 4:
        if (bcs::CWifiPad::IsDown(0x4000, m_nPadNo)) {
            return 1;
        }
        break;
    case 5:
        if (bcs::CWifiPad::IsDown(0x2000, m_nPadNo)) {
            return 1;
        }
        break;
    case 6:
        if (bcs::CWifiPad::IsDown(0x800, m_nPadNo) && bcs::CWifiPad::IsDown(0x400, m_nPadNo)) {
            return 1;
        }
        break;
    case 7:
        if (bcs::CWifiPad::IsDown(0x800, m_nPadNo) && bcs::CWifiPad::IsDown(0x2000, m_nPadNo)) {
            return 1;
        }
        break;
    case 8:
        if (bcs::CWifiPad::IsDown(0x400, m_nPadNo) && bcs::CWifiPad::IsDown(0x2000, m_nPadNo)) {
            return 1;
        }
        break;
    case 9:
        if (!(m_fStickDir2 < 0.0f)) {
            long nQuarter = GetQuarter(m_fStickDir2);
            if (nQuarter == 0 || nQuarter == 2) {
                return 1;
            }
        }
        break;
    case 10:
        if (!(m_fStickDir2 < 0.0f)) {
            long nQuarter = GetQuarter(m_fStickDir2);
            if (nQuarter == 1 || nQuarter == 3) {
                return 1;
            }
        }
        break;
    case 17:
        if (bcs::CWifiPad::IsDown(0x10, m_nPadNo)) {
            return 1;
        }
        break;
    case 18:
        if (bcs::CWifiPad::IsDown(0x1000, m_nPadNo)) {
            return 1;
        }
        break;
    case 19:
        if (bcs::CWifiPad::IsDown(0x8, m_nPadNo)) {
            return 1;
        }
        break;
    case 20:
        if (bcs::CWifiPad::IsDown(0x4, m_nPadNo)) {
            return 1;
        }
        break;
    case 21:
        if (bcs::CWifiPad::IsDown(0x1, m_nPadNo)) {
            return 1;
        }
        break;
    case 22:
        if (bcs::CWifiPad::IsDown(0x2, m_nPadNo)) {
            return 1;
        }
        break;
    case 23:
        if (IsPress1()) {
            return 1;
        }
        break;
    case 24:
        if (bcs::CWifiPad::IsDown(0x100, m_nPadNo)) {
            return 1;
        }
        break;
    // (no break after 25 and 28 in the original: 1+2 held falls through
    // to the tests of B+1 and B+2)
    case 25:
        if (bcs::CWifiPad::IsDown(0x200, m_nPadNo) && bcs::CWifiPad::IsDown(0x100, m_nPadNo)) {
            return 1;
        }
    case 28:
        if (bcs::CWifiPad::IsDown(0x400, m_nPadNo) && bcs::CWifiPad::IsDown(0x200, m_nPadNo)) {
            return 1;
        }
    case 29:
        if (bcs::CWifiPad::IsDown(0x400, m_nPadNo) && bcs::CWifiPad::IsDown(0x100, m_nPadNo)) {
            return 1;
        }
        break;
    }
    return 0;
}

int CChrDrvPadDodge::CheckStroke(long nExcept, long nType) const
{
    long nStroke = 0;
    for (long i = 1; i < 30; i++) {
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

int CChrDrvPadDodge::CheckStrokeRaw(long nType) const
{
    switch (nType) {
    case 1:
        if (IsRemoconStroke(0.6f)) {
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

inline void CChrDrvPadDodge::ClearPressFlag_(long nKey)
{
    long nIdx = GetPressInfoIdx(nKey);
    if (nIdx >= 0) {
        m_PressInfo[nIdx].m_nFlag = 0;
    }
}

void CChrDrvPadDodge::ClearStrokeCmd(long nKey, long bStroke)
{
    if (bStroke) {
        ClearStroke_(0.0f);
    }
    switch (nKey) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 0x17:
    case 0x18:
        ClearPressFlag_(nKey);
        break;
    case 6:
        ClearPressFlag_(2);
        ClearPressFlag_(3);
        break;
    case 7:
        ClearPressFlag_(2);
        ClearPressFlag_(5);
        break;
    case 8:
        ClearPressFlag_(3);
        ClearPressFlag_(5);
        break;
    case 25:
        ClearPressFlag_(0x17);
        ClearPressFlag_(0x18);
        break;
    }
}

// Queues call nIdx of UpdateCmdBuf_ for fFrame frames (unless it is
// running); null when not queued.
inline CChrDrvPadDodge::CMD_BUF *CChrDrvPadDodge::SetCmdBuf_(long nIdx, float fFrame)
{
    if (!m_bCmdBufReady) {
        return 0;
    }
    CMD_BUF *pBuf = &m_CmdBuf[nIdx];
    pBuf->m_fTime = fFrame;
    return pBuf;
}

int CChrDrvPadDodge::MethdPassCall(float)
{
    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CChrCore *pChr = CChrMan::Util_SearchHaveBallChr(i);
        if (pChr != 0 && pChr->GetTeamNo() == m_pChr->GetTeamNo() && !pChr->IsPadCtrl()) {
            pChr->IsDodgeInfield();
            m_pChr->IsDodgeInfield();
            if (pChr->MethdPassIdx(m_pChr->GetPlayerNo())) {
                if (IsPadTypeAnalogStick_()) {
                    ClearStrokeCmd(2, 0);
                } else {
                    ClearStrokeCmd(0x17, 0);
                }
                pChr->DriverSendMessage((MSG_ID)0x52, m_pChr->GetPlayerNo(), 0, 0, 0);
                return 1;
            }
        }
    }
    SetCmdBuf_(1, s_Prm.m_fCmdBufFrame[1]);
    return 0;
}

int CChrDrvPadDodge::MethdShootCall(long nType)
{
    int nRet = 0;
    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CChrCoreDodge *pChr = static_cast<CChrCoreDodge *>(CChrMan::Util_SearchHaveBallChr(i));
        if (pChr != 0 && pChr->GetTeamNo() == m_pChr->GetTeamNo() && !pChr->IsPadCtrl()) {
            int bDone = 0;
            if (pChr->GetLockTargetChrIdx() < 0) {
                pChr->SwitchAutoAimTarget();
            }
            switch (nType) {
            default:
                if (pChr->MethdShoot(-1.0f)) {
                    bDone = 1;
                }
                break;
            case 1:
                if (pChr->MethdSubShoot(-1.0f)) {
                    bDone = 1;
                }
                break;
            case 2:
                if (pChr->MethdItemBallAttack()) {
                    bDone = 1;
                }
                break;
            }
            if (bDone) {
                m_pChr->PlayVoice(0x58, 0);
                return 1;
            }
            nRet = 1;
        }
    }
    CMD_BUF *pBuf = SetCmdBuf_(2, s_Prm.m_fCmdBufFrame[2]);
    if (pBuf != 0) {
        pBuf->m_nParam = nType;
    }
    return nRet;
}

int CChrDrvPadDodge::MethdFeintCall()
{
    int nRet = 0;
    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CChrCore *pChr = CChrMan::Util_SearchHaveBallChr(i);
        if (pChr != 0 && pChr->GetTeamNo() == m_pChr->GetTeamNo() && !pChr->IsPadCtrl()) {
            if (pChr->MethdTrickMove(pChr->GetRotY())) {
                return 1;
            }
            nRet = 1;
        }
    }
    return nRet;
}

int CChrDrvPadDodge::MethdChrChange()
{
    long nNum = CChrMan::GetChrCoreNum();
    long nSelf = m_pChr->GetPlayerNo();
    for (long i = nSelf + 1; i != nSelf; i = (i + 1) % nNum) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && pChr->IsEnable() && m_pChr != pChr && CGame::IsPlayTypeCanChangeLeader(pChr->GetTeamNo())
            && pChr->GetTeamNo() == m_pChr->GetTeamNo() && !CGame::IsCharEx_DodgeOutfielder(pChr->GetPlayerNo())) {
            pChr->IsDodgeInfield();
            if (!pChr->IsPadCtrl()) {
                CGameDodge::ChrChange(pChr->GetPlayerNo(), m_pChr->GetPlayerNo());
                CGameSnd::Play(pChr->GetTeamNo() == 0 ? 0xC17 : 0xC18, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                pChr->PlayVoice(0x4F, 0);
                return 1;
            }
        }
    }
    return 0;
}

int CChrDrvPadDodge::MethdSwitchTargetCall()
{
    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CChrCore *pChr = CChrMan::Util_SearchHaveBallChr(i);
        if (pChr != 0 && pChr->GetTeamNo() == m_pChr->GetTeamNo() && !pChr->IsPadCtrl()) {
            pChr->SwitchAutoAimTarget();
            return 1;
        }
    }
    return 0;
}

}
