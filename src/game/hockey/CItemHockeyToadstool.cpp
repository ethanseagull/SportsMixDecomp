#include "game/msw/CItemHockey.hpp"

namespace msw {

void CItemHockeyMushroom::ExParamUpdate_() {
    m_fParamMainLife = 900.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamB8 = 7.0f;
    m_fParamBC = 2.5f;
    m_fParamC0 = 30.0f;
}

void CItemHockeyToadstool::ExParamUpdate_() {
    m_fParamMainLife = 900.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamHitEffectFrame = 360.0f;
    m_fParamB8 = 7.0f;
    m_fParamBC = 2.5f;
    m_fParamC0 = 30.0f;
}

long CItemHockeyToadstool::GetParamThrowType() const {
    return 0;
}

float CItemHockeyToadstool::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemHockeyToadstool::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

} // namespace msw
