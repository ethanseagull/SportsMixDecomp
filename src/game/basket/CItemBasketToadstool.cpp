#include "game/msw/CItemBasket.hpp"

namespace msw {

void CItemBasketMushroom::ExParamUpdate_() {
    m_fParamMainLife = 900.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamB8 = 5.0f;
    m_fParamBC = 3.0f;
    m_fParamC0 = 30.0f;
}

void CItemBasketToadstool::ExParamUpdate_() {
    m_fParamMainLife = 900.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.3f;
    m_fParamA4 = 600.0f;
    m_fParamB8 = 5.0f;
    m_fParamBC = 3.0f;
    m_fParamC0 = 30.0f;
}

long CItemBasketToadstool::GetParamThrowType() const {
    return 0;
}

float CItemBasketToadstool::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemBasketToadstool::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

void CItemBasketToadstool::DoThrowInit(const bcs::FVEC3 &pos, int n) {
    CItemToadstool::DoThrowInit(pos, n);
}

} // namespace msw
