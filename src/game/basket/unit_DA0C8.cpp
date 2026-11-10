// msw::CComDrvBasketHB::OnGameMsgProc: the messages the script-driven
// computer driver of the HB court answers itself.
#include "game/msw/CComDrvBasketHB.hpp"

#include "bcs/BCS_Math.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrMan.hpp"

// 70 degrees in radians, unused (see CRadarIcon2DBasket.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

// The message numbers are those of CChrDrv::MSG_ID, whose names are not
// known.
long msw::CComDrvBasketHB::OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg)
{
    long result = CComDrvBasket::OnGameMsgProc(id, pMsg);

    switch (id) {
    case 14:
        Reset();
        break;
    case 26:
        result = 0;
        if (pMsg != 0) {
            result = IsShootRelease(pMsg->m_nParam0, pMsg->m_nParam1);
        }
        break;
    case 33:
        // A 2% chance of answering 4.
        result = 0;
        if (RandMaxI(999) < 20) {
            result = 4;
        }
        break;
    case 32:
        result = GetScriptVariable("g_CancelLayup");
        break;
    case 37:
        result = GetScriptVariable("g_SwatIt");
        break;
    case 48:
        // The script takes the character as a number.
        CallMainScriptFunc("OnShootStart..II.", reinterpret_cast<long>(CChrMan::GetChrCore(pMsg->m_nParam0)),
                           pMsg->m_nParam1);
        break;
    case 53:
        CallMainScriptFunc("OnHBBonusTimeStart...");
        break;
    case 54:
        CallMainScriptFunc("OnHBBonusTimeEnd...");
        break;
    case 52:
        CallMainScriptFunc("OnHBBombFall..I.", CBallMan::GetBallHandle(pMsg->m_nParam0));
        break;
    }
    return result;
}
