#include "game/msw/CItemHockey.hpp"

namespace msw {

void CItemHockeyBomb::ExParamUpdate_() {
    m_fParamScale = 1.5f;
    m_fParamA4 = 55.0f;
    m_fParamA8 = 1.0f;
    m_nParamAC = 1;
    m_nParamB0 = 30;
    m_fParamB8 = 10.0f;
    m_fParamBC = 3.0f;
    m_fParamC0 = 60.0f;
    m_fExplosionCollSize = 5.0f;
}

long CItemHockeyBomb::GetParamThrowType() const {
    return 0;
}

float CItemHockeyBomb::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemHockeyBomb::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

void CItemHockeyBomb::GetExplosionParam(float &a, float &size, float &c, long &d, long &e, unsigned long &f) {
    CItemBomb::GetExplosionParam(a, size, c, d, e, f);
}

CItemHockeyBomb::~CItemHockeyBomb() {
}

} // namespace msw
