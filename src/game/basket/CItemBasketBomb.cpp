#include "game/msw/CItemBasket.hpp"

namespace msw {

void CItemBasketBomb::ExParamUpdate_() {
    m_fParamScale = 1.5f;
    m_fParamHitEffectFrame = 80.0f;
    m_fParamHitDropCoinRate = 1.0f;
    m_nParamHitDropCoinNumMin = 1;
    m_nParamHitDropCoinNumMax = 30;
    m_fParamB8 = 6.5f;
    m_fParamBC = 3.8f;
    m_fParamC0 = 40.0f;
    m_fExplosionCollSize = 3.5f;
}

long CItemBasketBomb::GetParamThrowType() const {
    return 1;
}

float CItemBasketBomb::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemBasketBomb::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

float CItemBasketBomb::GetExplosionCollSize() const {
    return m_fExplosionCollSize;
}

void CItemBasketBomb::DoThrowInit(const bcs::FVEC3 &pos, int) {
    SetVelParabola_(pos, m_fParamB8, m_fParamBC, m_fParamC0);
}

void CItemBasketBomb::GetExplosionParam(float &a, float &size, float &c, long &d, long &e, unsigned long &f) {
    a = m_fParamHitEffectFrame;
    size = m_fExplosionCollSize;
    c = m_fParamHitDropCoinRate;
    d = m_nParamHitDropCoinNumMin;
    e = m_nParamHitDropCoinNumMax;
    f = 0;
}

CItemBasketBomb::~CItemBasketBomb() {
}

} // namespace msw
