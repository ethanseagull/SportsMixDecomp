#include "game/msw/CItemBasket.hpp"

namespace msw {

void CItemBasketGShell::ExParamUpdate_() {
    m_fParamMainLife = 600.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamThrowSpeed = 20.0f;
    m_fParamHitEffectFrame = 30.0f;
    m_fParamHitDropCoinRate = 0.5f;
    m_nParamHitDropCoinNumMin = 2;
    m_nParamHitDropCoinNumMax = 10;
    m_nParamReboundNum = 1;
}

long CItemBasketGShell::GetParamThrowType() const {
    return 1;
}

float CItemBasketGShell::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemBasketGShell::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

void CItemBasketRShell::ExParamUpdate_() {
    m_fParamMainLife = 600.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamThrowSpeed = 15.0f;
    m_fParamA0 = 6.0f;
    m_fParamHitEffectFrame = 30.0f;
    m_fParamHitDropCoinRate = 0.5f;
    m_nParamHitDropCoinNumMin = 2;
    m_nParamHitDropCoinNumMax = 10;
    m_nParamReboundNum = 1;
}

long CItemBasketRShell::GetParamThrowType() const {
    return 1;
}

float CItemBasketRShell::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemBasketRShell::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

} // namespace msw
