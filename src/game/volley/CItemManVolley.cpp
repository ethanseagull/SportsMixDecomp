// Written from src/game/hockey/CItemManHockey.cpp by tools/decomp/port_unit.py (Hockey -> Volley); check against VolleyF.
#include "bcs/New.hpp"
#include "game/msw/CItemManVolley.hpp"
#include "game/msw/CItemVolley.hpp"

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

void CItemVolleyBanana::ExParamUpdate_() {
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

long CItemVolleyBanana::GetParamThrowType() const {
    return 0;
}

float CItemVolleyBanana::GetParamThrowSearchDirRangeMax() const {
    return 0.0f;
}

float CItemVolleyBanana::GetParamThrowSearchDistRangeMax() const {
    return 0.0f;
}

CItemVolleyBanana::~CItemVolleyBanana() {
}

} // namespace msw
