// msw::CItemManVolley and msw::CItemManVolleyRR: the item managers of the
// volley games, and the items they create (msw::CItemVolley*).
#include "bcs/New.hpp"
#include "game/msw/CItemManVolley.hpp"
#include "game/msw/CItemVolley.hpp"
// In this order: the vtables in .data follow it (see CItemManVolleyRR.hpp).
#include "game/msw/CItemManVolleyRR.hpp"
#include "game/msw/CItemVolleyRR.hpp"

namespace msw {

CItem *CItemManVolley::CreateItem(long kind, const bcs::FVEC3 &pos) {
    CItem *pItem;

    switch (kind) {
        case CItem::KIND_MUSHROOM:
            pItem = new (0x100) CItemVolleyMushroom(this);
            break;
        case CItem::KIND_TOADSTOOL:
            pItem = new (0x100) CItemVolleyToadstool(this);
            break;
        case CItem::KIND_BOMB:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            pItem = new (0x100) CItemVolleyBomb(this);
            break;
        case CItem::KIND_GSHELL:
            pItem = new (0x100) CItemVolleyGShell(this);
            break;
        case CItem::KIND_RSHELL:
            pItem = new (0x100) CItemVolleyRShell(this);
            break;
        case CItem::KIND_BANANA:
            pItem = new (0x100) CItemVolleyBanana(this);
            break;
        default:
            pItem = new (0x100) CItemVolleyGShell(this);
            break;
    }

    if (pItem != 0) {
        pItem->Create(pos);
    }
    return pItem;
}

void CItemManVolley::ExParamInitialize_() {
}

// --- mushroom ---

void CItemVolleyMushroom::ExParamUpdate_() {
    m_fParamMainLife = 900.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.4f;
    m_fParamB8 = 9.0f;
    m_fParamBC = 4.0f;
    m_fParamC0 = 35.0f;
}

// --- toadstool ---

void CItemVolleyToadstool::ExParamUpdate_() {
    m_fParamMainLife = 900.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.4f;
    m_fParamHitEffectFrame = 600.0f;
    m_fParamB8 = 9.0f;
    m_fParamBC = 4.0f;
    m_fParamC0 = 35.0f;
}

long CItemVolleyToadstool::GetParamThrowType() const {
    return 0;
}

float CItemVolleyToadstool::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemVolleyToadstool::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

void CItemVolleyToadstool::DoThrowInit(const bcs::FVEC3 &rDir, int arg) {
    CItemToadstool::DoThrowInit(rDir, arg);
}

// --- green shell ---

void CItemVolleyGShell::ExParamUpdate_() {
    m_fParamMainLife = 600.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamThrowSpeed = 15.0f;
    m_fParamHitEffectFrame = 30.0f;
    m_fParamHitDropCoinRate = 0.5f;
    m_nParamHitDropCoinNumMin = 2;
    m_nParamHitDropCoinNumMax = 5;
    m_nParamReboundNum = 1;
}

long CItemVolleyGShell::GetParamThrowType() const {
    return 1;
}

float CItemVolleyGShell::GetParamThrowSearchDirRangeMax() const {
    return 0.7853982f; // 45 degrees
}

float CItemVolleyGShell::GetParamThrowSearchDistRangeMax() const {
    return 8.0f;
}

// --- red shell ---

void CItemVolleyRShell::ExParamUpdate_() {
    m_fParamMainLife = 600.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamThrowSpeed = 15.0f;
    m_fParamA0 = 6.0f;
    m_fParamHitEffectFrame = 30.0f;
    m_fParamHitDropCoinRate = 0.5f;
    m_nParamHitDropCoinNumMin = 2;
    m_nParamHitDropCoinNumMax = 5;
    m_nParamReboundNum = 1;
}

long CItemVolleyRShell::GetParamThrowType() const {
    return 1;
}

float CItemVolleyRShell::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemVolleyRShell::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

// --- bomb ---

void CItemVolleyBomb::ExParamUpdate_() {
    m_fParamScale = 1.5f;
    m_fParamHitEffectFrame = 80.0f;
    m_fParamHitDropCoinRate = 1.0f;
    m_nParamHitDropCoinNumMin = 1;
    m_nParamHitDropCoinNumMax = 10;
    m_fParamB8 = 8.5f;
    m_fParamBC = 4.5f;
    m_fParamC0 = 80.0f;
    m_fExplosionCollSize = 3.5f;
}

long CItemVolleyBomb::GetParamThrowType() const {
    return 1;
}

float CItemVolleyBomb::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemVolleyBomb::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

// --- banana ---

void CItemVolleyBanana::ExParamUpdate_() {
    m_fParamMainLife = 900.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamHitEffectFrame = 12.0f;
    m_fParamHitDropCoinRate = 0.5f;
    m_nParamHitDropCoinNumMin = 2;
    m_nParamHitDropCoinNumMax = 10;
    m_fParamB8 = 7.0f;
    m_fParamBC = 3.0f;
    m_fParamC0 = 25.0f;
}

long CItemVolleyBanana::GetParamThrowType() const {
    return 0;
}

float CItemVolleyBanana::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemVolleyBanana::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

// --- RR game ---

// Items thrown in the RR game: a toadstool, or a banana for any other kind.
CItem *CItemManVolleyRR::CreateThrowItem(long kind, const bcs::FVEC3 &pos) {
    CItem *pItem;

    if (kind == CItem::KIND_TOADSTOOL) {
        pItem = new (0x100) CItemVolleyToadstoolRR(this);
    } else {
        pItem = new (0x100) CItemVolleyBananaRR(this);
    }

    if (pItem != 0) {
        pItem->Create(pos);
    }
    return pItem;
}

namespace {
    // Spin of the RR items in flight, radians per frame: it grows by
    // 1/6 degree a frame up to 5 degrees.
    const float SPIN_ACCEL = 0.0029088822f;
    const float SPIN_MAX = 0.08726646f;
}

// In both LoopStateThrow functions the original multiplies by 1.0f (and
// by -1.0f for the banana's turn) at run time, so those factors were not
// literals in its source; they are written as locals here (`scale`, `dir`,
// names guessed). The register order still differs slightly.

void CItemVolleyToadstoolRR::ExParamUpdate_() {
    m_fParamMainLife = 300.0f;
    m_fParamDropLife = 300.0f;
    m_fParamScale = 1.4f;
    m_fParamHitEffectFrame = 300.0f;
}

long CItemVolleyToadstoolRR::GetParamThrowType() const {
    return 0;
}

float CItemVolleyToadstoolRR::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemVolleyToadstoolRR::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

void CItemVolleyToadstoolRR::Create(const bcs::FVEC3 &pos) {
    CItemVolleyToadstool::Create(pos);
    CollBodyResetTargetFlag(0, 0x10);
}

void CItemVolleyToadstoolRR::DoTouchDown() {
    PlaySePos(0xDC3);
}

// Spins the toadstool up while it flies (the DOL toadstool turns by it).
void CItemVolleyToadstoolRR::LoopStateThrow() {
    float rate = GetFrameRateF();
    float scale = 1.0f;
    m_fUnk1AC += scale * (SPIN_ACCEL * rate);
    m_fUnk1AC = m_fUnk1AC > SPIN_MAX ? SPIN_MAX : (m_fUnk1AC < 0.0f ? 0.0f : m_fUnk1AC);
    CItemVolleyToadstool::LoopStateThrow();
}

void CItemVolleyBananaRR::ExParamUpdate_() {
    m_fParamMainLife = 240.0f;
    m_fParamDropLife = 240.0f;
    m_fParamScale = 1.5f;
    m_fParamHitEffectFrame = 12.0f;
    m_fParamHitDropCoinRate = 0.0f;
    m_nParamHitDropCoinNumMin = 0;
    m_nParamHitDropCoinNumMax = 0;
}

long CItemVolleyBananaRR::GetParamThrowType() const {
    return 0;
}

float CItemVolleyBananaRR::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemVolleyBananaRR::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

void CItemVolleyBananaRR::Create(const bcs::FVEC3 &pos) {
    CItemVolleyBanana::Create(pos);
    CollBodyResetTargetFlag(0, 0x10);
}

void CItemVolleyBananaRR::DoTouchDown() {
    PlaySePos(0xDC3);
}

// Spins the banana in flight until it has touched the ground.
void CItemVolleyBananaRR::LoopStateThrow() {
    if ((m_nFlag & 0x100) == 0) {
        float rate = GetFrameRateF();
        float scale = 1.0f;
        rate *= scale;
        m_fSpin += SPIN_ACCEL * rate;
        m_fSpin = m_fSpin > SPIN_MAX ? SPIN_MAX : (m_fSpin < 0.0f ? 0.0f : m_fSpin);
        float dir = -1.0f; // turns clockwise
        AddRotY(dir * m_fSpin * rate);
    }
    CItemVolleyBanana::LoopStateThrow();
}

} // namespace msw
