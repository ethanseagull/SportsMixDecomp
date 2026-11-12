#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CCockpitDodge.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameDodge.hpp"
#include "game/msw/CGimmickMan.hpp"

// msw::CChrCoreDodgeDodgeOrDie: a character of the party game "dodge or
// die". The characters cannot catch, pass, jump or use items; they only
// dodge the balls, and their stamina gauge charges while they run or
// crouch and drains when they stop.

// 60 degrees in radians; unused here, as in the other DodgeF chr cores
// (most likely from a header they all include).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

// (names guessed) The frames the stamina waits after it stopped charging
// before it drains, and the two values of the wait before a drain starts.
static const float s_fChargeWait = 30.0f;
static const float s_fDrainWaitNone = 0.0f;
static const float s_fDrainWaitFull = 1.0f;

msw::CChrCoreDodgeDodgeOrDie::CChrCoreDodgeDodgeOrDie()
{
}

msw::CChrCoreDodgeDodgeOrDie::~CChrCoreDodgeDodgeOrDie()
{
}

long msw::CChrCoreDodgeDodgeOrDie::GetAreaCode_(const bcs::FVEC3 &) const
{
    return 0;
}

int msw::CChrCoreDodgeDodgeOrDie::IsNeedNearAlert() const
{
    return 0;
}

int msw::CChrCoreDodgeDodgeOrDie::CanbeBallPick_() const
{
    return 0;
}

int msw::CChrCoreDodgeDodgeOrDie::IsLineOver_(int) const
{
    return 0;
}

int msw::CChrCoreDodgeDodgeOrDie::IsForceReflect_()
{
    return 0;
}

// Question panels are only picked up with empty hands.
void msw::CChrCoreDodgeDodgeOrDie::CheckQPanel_()
{
    if (IsDisable()) {
        return;
    }
    if (m_nStatusFlag & (0x40000 | 0x400)) {
        return;
    }
    if (m_nStatusFlag & 1) {
        return;
    }
    if (m_UnkEC & 0x20) {
        return;
    }

    bcs::FVEC3 pos;
    GetCompPos(pos);
    long nItem;
    CGimmickMan::QPanelCheck(pos, m_nTeamNo, m_nStatusFlag & 1, 0, (m_nStatusFlag >> 15) & 1, &nItem, 0);
}

// A hit by a ball (damage kinds 0x1F and 0x15) empties the stamina and
// counts the damage against the character; when that was its last life
// the state's work is told so.
int msw::CChrCoreDodgeDodgeOrDie::MethdDamage(unsigned long nType, float f0, float fDamage, float f2, long n0,
                                             float f3, long n1, long n2, long n3, long n4)
{
    int bHit = CChrCoreDodge::MethdDamage(nType, f0, fDamage, f2, n0, f3, n1, n2, n3, n4);
    if (bHit) {
        unsigned char nKind = nType >> 8;
        if (nKind == 0x1F || nKind == 0x15) {
            AddStamina(-GetMaxStamina());
            static_cast<CGameDodge *>(CGame::m_pInstance)->AddChrLostScore_(m_nPlayerNo, static_cast<long>(fDamage), -1);
            if (GetLife() <= 0.0f) {
                FsmqGetWork<unsigned long>()[2] &= ~0xFF;
            }
        }
    }
    return bHit;
}

void msw::CChrCoreDodgeDodgeOrDie::UpdateStamina(float fFrame)
{
    GetCcParam_();
    float fStamina = GetStamina();
    float fMax = GetMaxStamina();
    int bCharge = 0;
    // Never set in this mode, so the drain branch below never runs; the
    // original still has the variable (a register kept at 0, its test and
    // the whole branch are in the code), so it is kept here. Most likely
    // the common CChrCoreDodge::UpdateStamina with this mode's settings.
    int bDrain = 0;
    int bWasFull = 0;
    if (fMax > 0.0f && GetStamina() >= fMax) {
        bWasFull = 1;
    }
    int bCharging = 0;
    int bFilled = 0;

    switch (GetState()) {
    case STATE_NEUTRAL:
        bCharge = 1;
        break;
    case STATE_RUN:
        bCharge = 1;
        break;
    }
    if (bCharge && fStamina < fMax) {
        bCharging = 1;
    }
    if ((m_nStatusFlag & 1) && GetState() == STATE_JUMP) {
        bDrain = 0;
    }
    if (fMax <= 0.0f) {
        bCharge = 0;
    }
    if (m_UnkEC & 0x20000000) {
        bCharge = 0;
    }

    if (bCharge) {
        if (m_fStaminaChargeWait > 0.0f) {
            m_fStaminaChargeWait -= fFrame;
            if (m_fStaminaChargeWait <= 0.0f) {
                fFrame = -m_fStaminaChargeWait;
                m_fStaminaChargeWait = 0.0f;
            }
        }
        // The frames a full charge takes: 90 while standing, in stances
        // 1 and 5 and otherwise. The original keeps the GetStanceType test
        // and three separate loads of the same constant, so the arms
        // probably held different settings in the common code; an if/else
        // with equal arms is merged by the compiler, this switch is not.
        float fChargeFrame;
        if (GetState() == STATE_NEUTRAL) {
            fChargeFrame = 90.0f;
        } else {
            switch (GetStanceType()) {
            case 1:
            case 5:
                fChargeFrame = 90.0f;
                break;
            default:
                fChargeFrame = 90.0f;
                break;
            }
        }
        if (m_fStaminaChargeWait <= 0.0f && fChargeFrame > 0.0f) {
            AddStamina(fFrame / fChargeFrame);
            bCharging = 1;
            if (GetStamina() < fMax) {
                m_fStaminaDrainWait = s_fDrainWaitNone;
            } else {
                m_fStaminaDrainWait = s_fDrainWaitFull;
            }
        }
    } else if (bDrain) {
        m_fStaminaChargeWait = s_fChargeWait;
        if (m_fStaminaDrainWait > 0.0f) {
            m_fStaminaDrainWait -= fFrame;
            if (m_fStaminaDrainWait <= 0.0f) {
                m_fStaminaDrainWait = 0.0f;
            }
        }
        float fDrainFrame = 6.0f;
        if (m_fStaminaDrainWait <= 0.0f && fDrainFrame > 0.0f) {
            AddStamina(-fFrame / fDrainFrame);
        }
    }

    SetStamina(GetStamina());
    if (!bWasFull && fMax > 0.0f && GetStamina() >= fMax) {
        bFilled = 1;
    }

    if (bCharging) {
        if (GetStamina() >= fMax) {
            if (bFilled) {
                Eff_Stop(5);
                m_nDispFlag &= ~1;
                CEffectCtrl::CreateAttachEffect(0x676, m_pMdl, 0, m_nEffNode, 0, 0, 0, 3, 0, 0, GetEffChrType());
                PlaySePos(0xC11, GetPos(), 1, -1, 0.0f, 0.5f, 0.0f);
            }
        } else if (!IsPlaySeSlot(0)) {
            PlaySeSlot(0, 0);
        }
    } else {
        Eff_Stop(5);
        StopSeSlot(0);
        m_nDispFlag &= ~1;
    }

    float fNow = GetStamina();
    float fRate = 0.0f;
    float fNowRate = fRate;
    if (fMax > fRate) {
        fRate = fStamina / fMax;
        fNowRate = fNow / fMax;
    }
    CCockpitDodge::SetChargeGauge(m_nTeamNo, m_nPlayerNo >> 1, fRate, fNowRate);
}

void msw::CChrCoreDodgeDodgeOrDie::DispFunc()
{
    CChrCoreDodge::DispFunc();
}

int msw::CChrCoreDodgeDodgeOrDie::CanbeJump(int)
{
    return 0;
}

msw::CChrCore *msw::CChrCoreDodgeDodgeOrDie::Util_GetNearInnerChrExclusive(long, float f0, float f1) const
{
    return CChrCoreDodge::Util_GetNearInnerChrExclusive(-1, f0, f1);
}

msw::CChrCore *msw::CChrCoreDodgeDodgeOrDie::Util_GetNearOuterChrExclusive(long, float f0, float f1) const
{
    return CChrCoreDodge::Util_GetNearOuterChrExclusive(-1, f0, f1);
}

int msw::CChrCoreDodgeDodgeOrDie::QueryUseItem_(int)
{
    return 0;
}

void msw::CChrCoreDodgeDodgeOrDie::ResetTeamItem_()
{
}

int msw::CChrCoreDodgeDodgeOrDie::PopTeamItem_()
{
    return -1;
}

void msw::CChrCoreDodgeDodgeOrDie::PushTeamItem_(long)
{
}

long msw::CChrCoreDodgeDodgeOrDie::GetTeamItem_() const
{
    return -1;
}

void msw::CChrCoreDodgeDodgeOrDie::CheckQPanel_(CBall *)
{
}

int msw::CChrCoreDodgeDodgeOrDie::IsNormalDivingCatch_() const
{
    return 1;
}

int msw::CChrCoreDodgeDodgeOrDie::IsNormalDodgeRoll_() const
{
    return 1;
}

int msw::CChrCoreDodgeDodgeOrDie::MethdDamage(unsigned long nType, float f0)
{
    return CChrCoreDodge::MethdDamage(nType, f0);
}

int msw::CChrCoreDodgeDodgeOrDie::MethdDamage(unsigned long nType, float f0, float f1, float f2, long n0, float f3,
                                             long n1, long n2)
{
    return MethdDamage(nType, f0, f1, f2, n0, f3, n1, n2, -1, -1);
}

int msw::CChrCoreDodgeDodgeOrDie::CanbeAerialShoot()
{
    return 0;
}

int msw::CChrCoreDodgeDodgeOrDie::CanbeAerialPassIdx(long)
{
    return 0;
}

int msw::CChrCoreDodgeDodgeOrDie::CanbeAerialPass(CChrCore *)
{
    return 0;
}

int msw::CChrCoreDodgeDodgeOrDie::CanbePassIdx(long)
{
    return 0;
}

int msw::CChrCoreDodgeDodgeOrDie::CanbePass(CChrCore *)
{
    return 0;
}

int msw::CChrCoreDodgeDodgeOrDie::CanbeAirTrick()
{
    return 0;
}

// No shot while bit 0x400 of the status is set.
int msw::CChrCoreDodgeDodgeOrDie::CanbePowerShot()
{
    if (m_nStatusFlag & 0x400) {
        return 0;
    }
    return CChrCoreDodge::CanbePowerShot();
}

int msw::CChrCoreDodgeDodgeOrDie::CanbeShoot()
{
    if (m_nStatusFlag & 0x400) {
        return 0;
    }
    return CChrCoreDodge::CanbeShoot();
}

int msw::CChrCoreDodgeDodgeOrDie::CanbeDivingCatch()
{
    return 0;
}

int msw::CChrCoreDodgeDodgeOrDie::CanbeAutoJump()
{
    return 0;
}

int msw::CChrCoreDodgeDodgeOrDie::CanbeBallCatch()
{
    return 0;
}
