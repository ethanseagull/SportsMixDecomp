#include "bcs/New.hpp"
#include "game/msw/CItemManHockey.hpp"
#include "game/msw/CItemHockey.hpp"

namespace msw {

CItem *CItemManHockey::CreateItem(long kind, const bcs::FVEC3 &pos) {
    CItem *pItem;

    switch (kind) {
        case CItem::KIND_MUSHROOM:
            pItem = new (0x100) CItemHockeyMushroom(this);
            break;
        case CItem::KIND_TOADSTOOL:
            pItem = new (0x100) CItemHockeyToadstool(this);
            break;
        case CItem::KIND_BOMB:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            pItem = new (0x100) CItemHockeyBomb(this);
            break;
        case CItem::KIND_GSHELL:
            pItem = new (0x100) CItemHockeyGShell(this);
            break;
        case CItem::KIND_RSHELL:
            pItem = new (0x100) CItemHockeyRShell(this);
            break;
        case CItem::KIND_BANANA:
            pItem = new (0x100) CItemHockeyBanana(this);
            break;
        default:
            pItem = new (0x100) CItemHockeyGShell(this);
            break;
    }

    if (pItem != 0) {
        pItem->Create(pos);
    }
    return pItem;
}

void CItemManHockey::ExParamInitialize_() {
}

void CItemHockeyBanana::ExParamUpdate_() {
    m_fParamMainLife = 900.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamHitEffectFrame = 30.0f;
    m_fParamHitDropCoinRate = 0.5f;
    m_nParamHitDropCoinNumMin = 2;
    m_nParamHitDropCoinNumMax = 10;
    m_fParamB8 = 7.0f;
    m_fParamBC = 2.8f;
    m_fParamC0 = 30.0f;
}

long CItemHockeyBanana::GetParamThrowType() const {
    return 0;
}

float CItemHockeyBanana::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemHockeyBanana::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

CItemHockeyBanana::~CItemHockeyBanana() {
}

} // namespace msw
