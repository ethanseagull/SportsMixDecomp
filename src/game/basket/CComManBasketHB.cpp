// msw::CComManBasketHB: computer helpers of the HB court.
#include "game/msw/CComManBasketHB.hpp"

#include "bcs/BCS_Math.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CNpcBossPackun.hpp"

// 70 degrees in radians, unused (see CRadarIcon2DBasket.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

int msw::CComManBasketHB::IsSleepBossPackun()
{
    CNpcBossPackun *pPackun = static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetBossPackun_(0);

    if (pPackun != 0) {
        // State 0 is taken to be the sleep (CNpcBossPackunBasketBase::
        // MethdSleep queues it).
        return pPackun->GetState() == 0;
    }
    return 0;
}
