#include "game/msw/CGameGuideBasket.hpp"
#include "game/msw/CGameGuideManBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Util.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CGameGuideMan.hpp"

// Set by the file's static initialiser and used by nothing in the file (see
// game/basket/CBallManBasket.cpp: most likely from a shared header).
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);
}

namespace msw {

    // The guide telop types this file uses (names guessed from the
    // functions that start them).
    enum {
        GUIDE_JUMP_BLOCK = 1,
        GUIDE_JUMP_ATTACK = 3,
        GUIDE_SHOOT = 8,
        GUIDE_JUMP_SHOOT = 9
    };

    // Character states the guides wait for (meanings guessed).
    enum {
        CHR_STATE_SHOOT = 0xC,
        CHR_STATE_JUMP_SHOOT = 0x22,
        CHR_STATE_DUNK = 0x28
    };

    CGameGuideBasket *CGameGuideBasket::g_pInstance;
}

namespace {
    // (guessed helper) The horizontal distance between r1 and r2.
    inline float GetDistXZ(const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 xz;
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &r1, &r2);
        bcs::FVEC3 d = diff;
        xz.Set(d.x, 0.0f, d.z);
        return PSVECMag((const Vec *)&xz);
    }

    // (guessed helper) The character on pad nPadNo that takes part, null
    // for none.
    inline msw::CChrCore *SearchPadChr(long nPadNo)
    {
        for (long i = 0; i < msw::CChrMan::GetChrCoreNum(); i++) {
            msw::CChrCore *pChr = msw::CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable() && pChr->IsPadCtrl() && nPadNo == pChr->GetPadNo()) {
                return pChr;
            }
        }
        return 0;
    }

    // (guessed helper) Not zero when pChr can jump up now.
    inline int CanJumpUp(const msw::CChrCore *pChr)
    {
        return pChr->CanbeNowJumpUpper() != 0;
    }
}

namespace msw {

    inline void CGameGuideBasket::Reset()
    {
        m_pChr = 0;
        m_nType = -1;
        m_nLastType = -1;
        m_nPadNo = -1;
        m_nFlags &= ~1;
        CGameGuideMan::RemoveGuideTelop();
    }

    inline void CGameGuideBasket::Clear()
    {
        for (int i = 0; i < 4; i++) {
            m_nDoneBits[i] = 0;
        }
        m_fChrChangeWait = 0.0f;
        m_fUnk4C = 0.0f;
        m_nUnk48 = -1;
        m_fHoldTime = 0.0f;
        Reset();
    }

    // `IsPadCtrl() == 0` rather than `!IsPadCtrl()` in the two helpers below
    // keeps the original's block order (the "not a pad player" result falls
    // through instead of being moved after the rest).
    inline int CGameGuideBasket::IsGuideDone(const CChrCore *pChr, long nType) const
    {
        if (pChr == 0) {
            return 1;
        }
        if (pChr->IsPadCtrl() == 0) {
            return 1;
        }
        return (m_nDoneBits[pChr->GetPadNo()] >> nType) & 1;
    }

    inline int CGameGuideBasket::IsGuideChr(const CChrCore *pChr) const
    {
        if (pChr == 0) {
            return 0;
        }
        if (pChr->IsDisable()) {
            return 0;
        }
        if (pChr->IsPadCtrl() == 0) {
            return 0;
        }
        return !CChrManBasket::IsBadConditon(pChr);
    }

    inline int CGameGuideBasket::IsShootGuide(const CChrCore *pChr) const
    {
        if (IsGuideDone(pChr, GUIDE_JUMP_SHOOT)) {
            return 0;
        }
        if (!IsGuideChr(pChr)) {
            return 0;
        }
        if (!const_cast<CChrCore *>(pChr)->CanbeJumpShoot()) {
            return 0;
        }
        if (900.0f > m_fHoldTime) {
            return 0;
        }
        float fDist = GetDistXZ(pChr->GetPos(), pChr->GetTargetGoalPos());
        // A little more room once the guide is up.
        float fMax = m_nType != GUIDE_SHOOT ? 5.5f : 7.0f;
        return !(fDist > fMax);
    }

    inline int CGameGuideBasket::IsJumpShootGuide(const CChrCore *pChr) const
    {
        if (IsGuideDone(pChr, GUIDE_JUMP_SHOOT)) {
            return 0;
        }
        if (!IsGuideChr(pChr)) {
            return 0;
        }
        if (!pChr->IsHoldBall()) {
            return 0;
        }
        return pChr->GetState() == CHR_STATE_JUMP_SHOOT;
    }

    inline int CGameGuideBasket::StartJumpShootGuide(const CChrCore *pChr)
    {
        if (IsJumpShootGuide(pChr)) {
            StartGuideTelop(GUIDE_JUMP_SHOOT, pChr);
            return 1;
        }
        return 0;
    }

    inline CGameGuideBasket::CGameGuideBasket(bcs::CTask *pParent) : bcs::CTask(pParent, -1), m_pChr(0), m_nFlags(0)
    {
        m_fUnk54 = 0.0f;
        m_fTelopTime = 0.0f;
        Clear();
    }

    CGameGuideBasket::~CGameGuideBasket()
    {
        Clear();
    }

    void CGameGuideBasket::Main()
    {
        if (CGameBasket::GetInstance()->IsNowMainLoopEnable_()) {
            UpDateGuid();
        }
    }

    void CGameGuideBasket::UpDateGuid()
    {
        if (CChrMan::Util_SearchHoldBallChr(0) != 0) {
            m_fHoldTime += GetFrameRateF();
            if (m_fHoldTime > 9999.9f) {
                m_fHoldTime = 9999.9f;
            }
        } else {
            m_fHoldTime = 0.0f;
        }
        if (m_fUnk4C > 0.0f) {
            m_fUnk4C -= GetFrameRateF();
            if (m_fUnk4C < 0.0f) {
                m_fUnk4C = 0.0f;
            }
        }
        if (m_fChrChangeWait > 0.0f) {
            m_fChrChangeWait -= GetFrameRateF();
            if (m_fChrChangeWait < 0.0f) {
                m_fChrChangeWait = 0.0f;
            }
        }
        if (m_fUnk54 > 0.0f) {
            m_fUnk54 -= GetFrameRateF();
            if (m_fUnk54 < 0.0f) {
                m_fUnk54 = 0.0f;
            }
        }

        if (m_nType == -1 && !IsWait()) {
            m_pChr = 0;
            if (m_fUnk4C > 0.0f || m_fChrChangeWait > 0.0f || m_fUnk54 > 0.0f || JudgeJumpBlock() ||
                JudgeJumpAttack()) {
                return;
            }
            CChrCore *pChr = CChrMan::Util_SearchHoldBallChr(0);
            if (IsShootGuide(pChr)) {
                m_fTelopTime = 0.0f;
                StartGuideTelop(GUIDE_SHOOT, pChr);
            }
            return;
        }

        DispContinue();
        if (m_nType == -1 && IsWait()) {
            // The guide that was up has gone: wait for the move it showed
            // to end before the next one.
            int bEnd = 0;
            switch (m_nLastType) {
                case GUIDE_JUMP_BLOCK: {
                    CBall *pBall = CBallMan::GetBallPtr(0);
                    if (pBall != 0 && !pBall->IsShootMode()) {
                        bEnd = 1;
                    }
                    break;
                }
                case GUIDE_JUMP_ATTACK: {
                    CChrCore *pHold = CChrMan::Util_SearchHoldBallChr(0);
                    if (pHold != 0 && pHold->GetState() == CHR_STATE_DUNK) {
                        CChrCore *pChr = SearchPadChr(m_nPadNo);
                        if (pChr != 0 && pChr->GetState() == CHR_STATE_SHOOT) {
                            break;
                        }
                    }
                    bEnd = 1;
                    break;
                }
                default:
                    bEnd = 1;
                    break;
            }
            if (bEnd) {
                m_nFlags &= ~1;
            }
        }
    }

    void CGameGuideBasket::DispContinue()
    {
        // Not read anywhere (the original has it in .rodata all the same).
        static const int bEnable = 1;

        if (m_nType == -1) {
            return;
        }
        switch (m_nType) {
            case GUIDE_JUMP_ATTACK:
                if (CanJumpUp(m_pChr) &&
                    JudgeJumpAttack__(m_pChr, CChrMan::Util_SearchHoldBallChr(0))) {
                    return;
                }
                break;
            case GUIDE_SHOOT:
                m_fTelopTime += GetFrameRateF();
                if (999.0f < m_fTelopTime) {
                    m_fTelopTime = 999.0f;
                }
                if (IsShootGuide(m_pChr)) {
                    return;
                }
                m_fTelopTime = 0.0f;
                m_nFlags |= 1;
                // fall through: the shot may go on as a jump shot
            case GUIDE_JUMP_SHOOT:
                if (m_nType == GUIDE_SHOOT) {
                    if (StartJumpShootGuide(m_pChr)) {
                        return;
                    }
                } else if (IsJumpShootGuide(m_pChr)) {
                    return;
                }
                break;
            case GUIDE_JUMP_BLOCK:
                if (JudgeJumpBlock_(m_pChr, CBallMan::GetBallPtr(0))) {
                    return;
                }
                break;
        }

        // The telop is no longer right: take it down.
        m_fUnk54 = 0.0f;
        long nPlayer = m_pChr != 0 ? m_pChr->GetPlayerNo() : -1;
        m_pChr = 0;
        m_nLastType = m_nType;
        m_nType = -1;
        CGameGuideMan::EndGuideTelop(nPlayer);
    }

    int CGameGuideBasket::JudgeJumpAttack()
    {
        CChrCore *pHold = CChrMan::Util_SearchHoldBallChr(0);
        if (pHold == 0) {
            return 0;
        }
        if (pHold->GetState() != CHR_STATE_DUNK) {
            return 0;
        }
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (JudgeJumpAttack__(pChr, pHold) && CanJumpUp(pChr)) {
                StartGuideTelop(GUIDE_JUMP_ATTACK, pChr);
                m_nFlags |= 1;
                return 1;
            }
        }
        return 0;
    }

    // Not zero when pChr stands where a jump would meet pHold's dunk: within
    // a strip in front of pHold.
    int CGameGuideBasket::JudgeJumpAttack__(const CChrCore *pChr, const CChrCore *pHold)
    {
        static const float fDist = 3.5f;

        if (pHold == 0) {
            return 0;
        }
        if (pHold->GetState() != CHR_STATE_DUNK) {
            return 0;
        }
        if (IsGuideDone(pChr, GUIDE_JUMP_ATTACK)) {
            return 0;
        }
        if (!IsGuideChr(pChr)) {
            return 0;
        }
        if (pChr->GetTeamNo() == pHold->GetTeamNo()) {
            return 0;
        }
        if (static_cast<const CChrCoreBasket *>(pHold)->GetDunkActionRate() > 80) {
            return 0;
        }

        bcs::FVEC3 dir;
        bcs::FVEC3 to;
        bcs::FVEC3 from = pHold->GetPos();
        bcs::BCS_VecZ_RotY(dir, 1.0f, pHold->GetRotY());
        to.Set(from.x + dir.x * fDist, 0.0f, from.z + dir.z * fDist);
        {
            static const float fDist = 1.0f;

            from.x -= dir.x * fDist;
            from.z -= dir.z * fDist;
        }
        {
            static const float fWidth = 1.2f;

            return bcs::BCS_Util_IsPointInFaceXZ(pChr->GetPos(), from, to, fWidth, GetDistXZ(from, to)) != 0;
        }
    }

    int CGameGuideBasket::JudgeJumpBlock()
    {
        CBall *pBall = CBallMan::GetBallPtr(0);
        CChrCore *pHold = CChrMan::Util_SearchHoldBallChr(pBall->GetBallNo());
        if (pHold != 0) {
            if (pHold->GetState() != CHR_STATE_JUMP_SHOOT) {
                return 0;
            }
        } else {
            if (pBall == 0) {
                return 0;
            }
            if (!pBall->IsShootMode()) {
                return 0;
            }
            if (pBall->GetItemEffect_() != 0) {
                return 0;
            }
            if ((unsigned long)pBall->GetShootType() > 1) {
                return 0;
            }
        }
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr->CanbeJumpBlock() && JudgeJumpBlock_(pChr, pBall)) {
                m_nFlags |= 1;
                StartGuideTelop(GUIDE_JUMP_BLOCK, pChr);
                return 1;
            }
        }
        return 0;
    }

    // Not zero when pChr can block: in front of pHold's jump shot, or near
    // the ball's way to the goal and facing it.
    int CGameGuideBasket::JudgeJumpBlock_(const CChrCore *pChr, const CBall *pBall)
    {
        static const float fDist = 11.0f;

        long nUserTeam = CGame::g_GameWork.GetUserTeam();
        if (IsGuideDone(pChr, GUIDE_JUMP_BLOCK)) {
            return 0;
        }
        if (!IsGuideChr(pChr)) {
            return 0;
        }
        if (!const_cast<CChrCore *>(pChr)->CanbeJumpBlock()) {
            return 0;
        }
        if (nUserTeam == pChr->GetTeamNo()) {
            return 0;
        }

        const CChrCore *pHold = CChrMan::Util_SearchHoldBallChr(pBall->GetBallNo());
        if (pHold != 0) {
            static const float fWidth = 2.0f;

            if (pHold->GetState() != CHR_STATE_JUMP_SHOOT) {
                return 0;
            }
            bcs::FVEC3 dir;
            bcs::BCS_VecZ_RotY(dir, 1.0f, pHold->GetRotY_ToGoal());
            bcs::FVEC3 from = pHold->GetPos();
            from.y = 0.1f;
            bcs::FVEC3 to(from.x + dir.x * fDist, from.y, from.z + dir.z * fDist);
            if (bcs::BCS_Util_IsPointInFaceXZ(pChr->GetPos(), from, to, fWidth, fDist)) {
                return 1;
            }
        }

        if (pBall == 0) {
            return 0;
        }
        if (!pBall->IsActive()) {
            return 0;
        }
        if (!pBall->IsShootMode()) {
            return 0;
        }
        {
            // fOffZ is not read (the original has it in .rodata all the same).
            static const float fOffZ = 0.0f;
            static const float fWidth = 2.0f;

            bcs::FVEC3 after;
            pBall->GetPosAfterFrame(after, 55.0f + static_cast<const CChrCoreBasket *>(pChr)->GetJumpBlockJumpFrame());
            const bcs::FVEC3 &rBall = pBall->GetPos();
            float fMin = m_nType != -1 ? 0.2f : 0.0f;
            if (fMin > GetDistXZ(rBall, pChr->GetPos())) {
                return 0;
            }
            if (!bcs::BCS_Util_IsPointInFaceXZ(pChr->GetPos(), rBall, after, fWidth, GetDistXZ(rBall, after))) {
                return 0;
            }
            float fRot = bcs::ROUND_PI(pChr->GetRotY_ToPosXZ(rBall) - pChr->GetRotY_ToGuardGoal());
            if (fRot > 3.1415927f) {
                fRot -= 6.2831855f;
            }
            if (bcs::BCS_Abs(fRot) < 1.5707964f) {
                return 0;
            }
            return 1;
        }
    }

    void CGameGuideBasket::StartGuideTelop(long nType, const CChrCore *pChr)
    {
        if (m_fChrChangeWait > 0.0f) {
            return;
        }
        if (pChr == 0) {
            return;
        }

        float f0 = -1.0f;
        float f1 = -1.0f;
        float f2 = -1.0f;
        long nTime = 0;
        bcs::FVEC3 ofs(0.0f, -1.0f, 0.0f);
        switch (nType) {
            case GUIDE_JUMP_BLOCK:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                f0 = 1.0f;
                f1 = 0.0f;
                f2 = 0.0f;
                nTime = 60;
                break;
            case GUIDE_JUMP_ATTACK:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                f0 = 1.0f;
                f1 = 0.0f;
                f2 = 0.0f;
                nTime = 60;
                break;
            case GUIDE_SHOOT:
            case GUIDE_JUMP_SHOOT:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                f0 = 0.0f;
                nTime = 60;
                break;
        }
        CGameGuideMan::RemoveGuideTelop();
        m_pChr = pChr;
        m_nType = nType;
        m_nPadNo = pChr->GetPadNo();
        CGameGuideMan::StartGuideTelop(nType, const_cast<CChrCore *>(pChr), nTime, &ofs, f0, f1, f2);
    }

    void CGameGuideBasket::AddSuccessCount(const CChrCore *pChr, long nType)
    {
        if (!(m_nFlags & 1)) {
            return;
        }
        m_nFlags &= ~1;
        if (m_nType != -1) {
            if (m_nType != nType) {
                return;
            }
        } else if (m_nLastType != nType) {
            return;
        }
        long nPadNo = pChr->GetPadNo();
        if (m_nPadNo != nPadNo) {
            return;
        }
        unsigned long nBit = 1 << nType;
        if (nBit & m_nDoneBits[nPadNo]) {
            return;
        }
        CGameGuideMan::AddGuideSuccessCount(nType, nPadNo);
        long nCount = CGameGuideMan::GetGuideSuccessCount(nType, nPadNo);
        long nMax;
        switch (nType) {
            case GUIDE_JUMP_BLOCK:
                nMax = 3;
                break;
            case GUIDE_JUMP_ATTACK:
                nMax = 3;
                break;
            case GUIDE_JUMP_SHOOT:
                nMax = 3;
                break;
            default:
                return;
        }
        if (nCount >= nMax) {
            m_nDoneBits[nPadNo] |= nBit;
        }
    }

    void CGameGuideManBasket::Create(CChrMan *pMan)
    {
        if (CGameGuideBasket::g_pInstance == 0 && CGameGuideMan::IsActive() &&
            CGame::g_GameWork.GetGameMode() != 2) {
            CGameGuideBasket::g_pInstance = new CGameGuideBasket(pMan);
        }
    }

    void CGameGuideManBasket::RemoveGuideTelop()
    {
        if (CGameGuideBasket::g_pInstance != 0) {
            CGameGuideBasket::g_pInstance->Reset();
        }
    }

    void CGameGuideManBasket::StartJumpBall()
    {
        if (CGame::g_GameWork.m_nUnk180 == 0 && CGame::g_GameWork.GetGameMode() == 0) {
            CCockpit::CreateObj(0x1077, 0);
        }
    }

    void CGameGuideManBasket::EndJumpBall()
    {
        if (CGame::g_GameWork.GetGameMode() == 0) {
            CCockpit::PlayEndAnim(0x1077);
        }
    }

    long CGameGuideManBasket::IsJumpBallGuide()
    {
        return CCockpit::IsEndObj(0x1077);
    }

    void CGameGuideManBasket::OnChrChange()
    {
        if (CGameGuideBasket::g_pInstance != 0) {
            CGameGuideBasket::g_pInstance->m_fChrChangeWait = 30.0f;
        }
    }

    void CGameGuideManBasket::AddSuccessCount(const CChrCore *pChr, long nType)
    {
        if (CGameGuideBasket::g_pInstance != 0) {
            CGameGuideBasket::g_pInstance->AddSuccessCount(pChr, nType);
        }
    }
}
