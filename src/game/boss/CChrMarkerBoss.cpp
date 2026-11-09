// msw::CChrMarkerBoss: the marker above a character in the boss game.
#include "game/msw/CChrMarkerBoss.hpp"

#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"

msw::CChrMarkerBoss::CChrMarkerBoss(bcs::CTask *pParent, long nChr, long nType, long nArg)
    : CChrMarker(pParent, nChr, nType, nArg)
{
    EnableViewCheck(1);
}

msw::CChrMarkerBoss::~CChrMarkerBoss()
{
}

void msw::CChrMarkerBoss::MainFunc()
{
}

// The marker comes up for a character that is in the game and driven by a
// type 1 driver, unless the game is showing the spike jump marker for it.
// Character 14 is a special case: no marker while it is in state 0x27 or
// has bit 28 of its status flags set. (which character 14 is, and what the
// state and the bit stand for, is not known)
int msw::CChrMarkerBoss::AppearCheck()
{
    CChrCore *pChr;

    if (m_nChr < 0) {
        return 0;
    }
    pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr == 0 || pChr->IsDisable()) {
        return 0;
    }
    if (pChr->DriverGetType() != 1) {
        return 0;
    }
    if (pChr->GetChrId() == 14) {
        if (pChr->GetState() == 0x27) {
            return 0;
        }
        if (pChr->GetStatusFlag() & 0x10000000) {
            return 0;
        }
    }
    if (pChr->IsVolleyDispSpikeJumpMarker()) {
        return 0;
    }
    return 1;
}

// The same conditions with the answer reversed: the marker goes away as soon
// as AppearCheck would no longer let it come up.
int msw::CChrMarkerBoss::DisappearCheck()
{
    CChrCore *pChr;

    if (m_nChr < 0) {
        return 1;
    }
    pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr == 0 || pChr->IsDisable()) {
        return 1;
    }
    if (pChr->DriverGetType() != 1) {
        return 1;
    }
    if (pChr->GetChrId() == 14) {
        if (pChr->GetState() == 0x27) {
            return 1;
        }
        if (pChr->GetStatusFlag() & 0x10000000) {
            return 1;
        }
    }
    if (pChr->IsVolleyDispSpikeJumpMarker()) {
        return 1;
    }
    return 0;
}
