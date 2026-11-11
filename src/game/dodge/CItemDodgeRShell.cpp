// Written from src/game/hockey/CItemHockeyRShell.cpp by tools/decomp/port_unit.py (Hockey -> Dodge); parameters and DoThrowInit from DodgeF.
#include "game/msw/CItemDodge.hpp"

namespace msw {

void CItemDodgeGShell::ExParamUpdate_() {
    m_fParamMainLife = 600.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamThrowSpeed = 13.0f;
    m_fParamHitEffectFrame = 30.0f;
    m_fParamHitDropCoinRate = 0.0f;
    m_nParamHitDropCoinNumMin = 0;
    m_nParamHitDropCoinNumMax = 0;
    m_nParamReboundNum = 1;
}

long CItemDodgeGShell::GetParamThrowType() const {
    return 1;
}

float CItemDodgeGShell::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemDodgeGShell::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

void CItemDodgeGShell::DoThrowInit(const bcs::FVEC3 &rDir, int arg) {
    SetMapDetectCode(0);
    CItemGShell::DoThrowInit(rDir, arg);
}

void CItemDodgeRShell::ExParamUpdate_() {
    m_fParamMainLife = 600.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamThrowSpeed = 13.0f;
    m_fParamA0 = 6.0f;
    m_fParamHitEffectFrame = 30.0f;
    m_fParamHitDropCoinRate = 0.0f;
    m_nParamHitDropCoinNumMin = 0;
    m_nParamHitDropCoinNumMax = 0;
    m_nParamReboundNum = 1;
}

long CItemDodgeRShell::GetParamThrowType() const {
    return 1;
}

float CItemDodgeRShell::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemDodgeRShell::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

void CItemDodgeRShell::DoThrowInit(const bcs::FVEC3 &rDir, int arg) {
    SetMapDetectCode(0);
    CItemRShell::DoThrowInit(rDir, arg);
}

} // namespace msw
