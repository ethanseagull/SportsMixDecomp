#include "bcs/New.hpp"
#include "game/msw/CItemManBasket.hpp"
#include "game/msw/CItemBasket.hpp"

// One file with the basketball banana (CItemBasketBanana), as in HockeyF's
// CItemManHockey.cpp: the banana's vtable sits in this file's .data and the
// manager's inline destructor follows the banana code.

namespace msw {

CItem *CItemManBasket::CreateItem(long kind, const bcs::FVEC3 &pos) {
    CItem *pItem;

    switch (kind) {
        case CItem::KIND_MUSHROOM:
            pItem = new (0x100) CItemBasketMushroom(this);
            break;
        case CItem::KIND_TOADSTOOL:
            pItem = new (0x100) CItemBasketToadstool(this);
            break;
        case CItem::KIND_BOMB:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            pItem = new (0x100) CItemBasketBomb(this);
            break;
        case CItem::KIND_GSHELL:
            pItem = new (0x100) CItemBasketGShell(this);
            break;
        case CItem::KIND_RSHELL:
            pItem = new (0x100) CItemBasketRShell(this);
            break;
        case CItem::KIND_BANANA:
            pItem = new (0x100) CItemBasketBanana(this);
            break;
        default:
            pItem = new (0x100) CItemBasketGShell(this);
            break;
    }

    if (pItem != 0) {
        pItem->Create(pos);
    }
    return pItem;
}

void CItemManBasket::ExParamInitialize_() {
}

void CItemBasketBanana::ExParamUpdate_() {
    m_fParamMainLife = 900.0f;
    m_fParamDropLife = 600.0f;
    m_fParamScale = 1.5f;
    m_fParamA4 = 12.0f;
    m_fParamA8 = 0.5f;
    m_nParamAC = 2;
    m_nParamB0 = 10;
    m_fParamB8 = 4.2f;
    m_fParamBC = 2.5f;
    m_fParamC0 = 20.0f;
}

long CItemBasketBanana::GetParamThrowType() const {
    return 0;
}

float CItemBasketBanana::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemBasketBanana::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

CItemBasketBanana::~CItemBasketBanana() {
}

} // namespace msw
