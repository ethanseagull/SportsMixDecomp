// The character markers of the dodge game: msw::CChrMarkerDodge and its
// Arrow, CloseRange and DodgeOrDie variants.
// Started from src/game/boss/CChrMarkerBoss.cpp; the checks are Dodge's own.
#include "game/msw/CChrMarkerDodge.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGameDodge.hpp"

// The file's static initialiser sets up the two objects below, which
// nothing in this file uses. Both most likely come from a header shared
// with other files: about 60 files of the game modules start their .bss
// with the same float, and BasketF's CBallMarkerBasket.cpp and
// CArrowSignalBasket.cpp build the same colour table (CBallMarkerBasket's
// GetColor_ reads its first pair). Which header, and the names, are not
// known; the inline conversion is a guess at why the float is set at run
// time rather than stored as data.
namespace {
    inline float DegToRad(float fDeg)
    {
        return fDeg * (3.1415927f / 180.0f);
    }

    float s_fAngle60 = DegToRad(60.0f);

    // Nine pairs of colours (what each pair is for is not known).
    bcs::BCS_Color s_MarkerColor[18] = {
        bcs::BCS_Color(0xFF, 0xBE, 0x8C, 0x4B), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0x8C, 0xBE, 0x6E, 0x50), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0xB4, 0xC8, 0xBE, 0x32), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0x82, 0x5A, 0x5A, 0x5A), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x32), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0xB4, 0xAA, 0x8C, 0x4B), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0x28, 0xFF, 0xFF, 0x32), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0xFF, 0x82, 0x6E, 0x32), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x3C), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
    };
}

msw::CChrMarkerDodge::CChrMarkerDodge(bcs::CTask *pParent, long nChr, long nType, long nArg)
    : CChrMarker(pParent, nChr, nType, nArg)
{
}

msw::CChrMarkerDodge::~CChrMarkerDodge()
{
}

// The marker comes up for a character that is drawn and, with marker type 2,
// driven by a type 1 driver (any other type takes the other characters).
// Characters not driven by type 1 need to be in the infield. In the party
// game a character with no lives left gets none, nor does character 14 in
// state 0x27, nor a character whose target marker is up (unless it is in
// state 0x27). (what the types, character 14 and state 0x27 stand for is not
// known)
int msw::CChrMarkerDodge::AppearCheck()
{
    CChrCore *pChr;

    if (m_nChr < 0) {
        return 0;
    }
    pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr != 0) {
        if (pChr->GetDispFlag() & 4) {
            return 0;
        }
        if (m_nType == 2 && pChr->DriverGetType() != 1) {
            return 0;
        }
        if (m_nType != 2 && pChr->DriverGetType() == 1) {
            return 0;
        }
        if (pChr->DriverGetType() != 1 && !pChr->IsDodgeInfield()) {
            return 0;
        }
        if (CGame::g_GameWork.GetGameType() == 2 && CGameDodge::GetChrRestLife(m_nChr) <= 0) {
            return 0;
        }
        if (pChr->GetState() == 0x27 && pChr->GetChrId() == 14) {
            return 0;
        }
        if (pChr->IsDodgeTargetMarkerActive() && pChr->GetState() != 0x27) {
            return 0;
        }
        return 1;
    }
    return 0;
}

// The marker goes away as soon as AppearCheck would no longer let it come up.
int msw::CChrMarkerDodge::DisappearCheck()
{
    return AppearCheck() == 0;
}

// ---------------------------------------------------------------------------
// CChrMarkerDodgeArrow

// Marker type 4. Sets bit 0x4000 of the model's scene-object flags
// (meaning not known).
msw::CChrMarkerDodgeArrow::CChrMarkerDodgeArrow(bcs::CTask *pParent, long nChr, long nArg)
    : CChrMarker(pParent, nChr, 4, nArg)
{
    if (m_pMdl != 0) {
        m_pMdl->OnFlag(0x4000);
    }
}

msw::CChrMarkerDodgeArrow::~CChrMarkerDodgeArrow()
{
}

// Turns the arrow towards the character's target marker.
void msw::CChrMarkerDodgeArrow::MainFunc()
{
    CChrCore *pChr;
    bcs::FVEC3 pos;

    if (m_nChr < 0 || m_pMdl == 0) {
        return;
    }
    pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr == 0) {
        return;
    }
    if (pChr->GetState() == 0x27) {
        return;
    }
    if (!pChr->IsDodgeTargetMarkerActive()) {
        return;
    }
    if (!pChr->GetDodgeTargetMarkerPos(pos)) {
        return;
    }
    m_pMdl->SetRotY(pChr->GetRotY_ToPosXZ(pos));
}

// Up while the character's target marker is active, except in state 0x27.
int msw::CChrMarkerDodgeArrow::AppearCheck()
{
    CChrCore *pChr;

    if (m_nChr < 0) {
        return 0;
    }
    pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr != 0 && pChr->GetState() != 0x27) {
        return pChr->IsDodgeTargetMarkerActive();
    }
    return 0;
}

int msw::CChrMarkerDodgeArrow::DisappearCheck()
{
    return AppearCheck() == 0;
}

// Draws nothing itself.
void msw::CChrMarkerDodgeArrow::Disp()
{
}

// ---------------------------------------------------------------------------
// CChrMarkerDodgeCloseRange

msw::CChrMarkerDodgeCloseRange::CChrMarkerDodgeCloseRange(bcs::CTask *pParent, long nChr, long nArg)
    : CChrMarker(pParent, nChr, 5, nArg)
{
    if (m_pMdl != 0) {
        m_pMdl->OnFlag(0x4000);
    }
}

msw::CChrMarkerDodgeCloseRange::~CChrMarkerDodgeCloseRange()
{
}

int msw::CChrMarkerDodgeCloseRange::AppearCheck()
{
    return 0;
}

int msw::CChrMarkerDodgeCloseRange::DisappearCheck()
{
    return AppearCheck() == 0;
}

// ---------------------------------------------------------------------------
// CChrMarkerDodgeDodgeOrDie

msw::CChrMarkerDodgeDodgeOrDie::CChrMarkerDodgeDodgeOrDie(bcs::CTask *pParent, long nChr, long nType, long nArg)
    : CChrMarker(pParent, nChr, nType, nArg)
{
}

msw::CChrMarkerDodgeDodgeOrDie::~CChrMarkerDodgeDodgeOrDie()
{
}

// CChrMarker's check, and only for a character with lives left.
int msw::CChrMarkerDodgeDodgeOrDie::AppearCheck()
{
    if (!CChrMarker::AppearCheck()) {
        return 0;
    }
    if (m_nChr < 0) {
        return 0;
    }
    if (CChrMan::GetChrCore(m_nChr) != 0) {
        return CGameDodge::GetChrRestLife(m_nChr) > 0;
    }
    return 0;
}

int msw::CChrMarkerDodgeDodgeOrDie::DisappearCheck()
{
    return AppearCheck() == 0;
}
