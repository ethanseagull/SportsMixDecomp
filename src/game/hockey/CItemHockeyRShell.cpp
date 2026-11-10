#include "game/msw/CItemHockey.hpp"

namespace msw {

void CItemHockeyGShell::ExParamUpdate_() {
    m_fParamMainLife = 600.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamThrowSpeed = 19.0f;
    m_fParamA4 = 40.0f;
    m_fParamA8 = 0.5f;
    m_nParamAC = 2;
    m_nParamB0 = 10;
    m_nParamReboundNum = 1;
}

long CItemHockeyGShell::GetParamThrowType() const {
    return 1;
}

float CItemHockeyGShell::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemHockeyGShell::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

void CItemHockeyRShell::ExParamUpdate_() {
    m_fParamMainLife = 600.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamThrowSpeed = 19.0f;
    m_fParamA0 = 6.0f;
    m_fParamA4 = 40.0f;
    m_fParamA8 = 0.5f;
    m_nParamAC = 2;
    m_nParamB0 = 10;
    m_nParamReboundNum = 1;
}

long CItemHockeyRShell::GetParamThrowType() const {
    return 1;
}

float CItemHockeyRShell::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemHockeyRShell::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

} // namespace msw
