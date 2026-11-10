#include "bcs/New.hpp"
#include "game/msw/CItemManDodge.hpp"
#include "game/msw/CItemDodge.hpp"

// The original file goes on with the dodgeball item classes and the
// CItemManDodgeDD / CItemDodgeBombDD_* family (the split's unit
// game/dodge/CItemDodgeBombDD_Base, not written yet): this unit's .data holds
// their vtables.

namespace msw {

CItem *CItemManDodge::CreateItem(long kind, const bcs::FVEC3 &pos) {
    CItem *pItem;

    switch (kind) {
        case CItem::KIND_MUSHROOM:
            pItem = new (0x100) CItemDodgeMushroom(this);
            break;
        case CItem::KIND_TOADSTOOL:
            pItem = new (0x100) CItemDodgeToadstool(this);
            break;
        case CItem::KIND_BOMB:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            pItem = new (0x100) CItemDodgeBomb(this);
            break;
        case CItem::KIND_GSHELL:
            pItem = new (0x100) CItemDodgeGShell(this);
            break;
        case CItem::KIND_RSHELL:
            pItem = new (0x100) CItemDodgeRShell(this);
            break;
        case CItem::KIND_BANANA:
            pItem = new (0x100) CItemDodgeBanana(this);
            break;
        default:
            pItem = new (0x100) CItemDodgeGShell(this);
            break;
    }

    if (pItem != 0) {
        pItem->Create(pos);
    }
    return pItem;
}

void CItemManDodge::ExParamInitialize_() {
}

} // namespace msw
