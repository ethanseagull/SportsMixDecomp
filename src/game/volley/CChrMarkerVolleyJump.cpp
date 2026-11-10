// msw::CChrMarkerVolley and msw::CChrMarkerVolleyJump: the markers above a
// character in the volley game.
#include "game/msw/CChrMarkerVolley.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"

// Radians in one degree.
static const float scDegToRad = 0.017453292f;

msw::CChrMarkerVolley::CChrMarkerVolley(bcs::CTask *pParent, long nChr, long nType, long nArg)
    : CChrMarker(pParent, nChr, nType, nArg)
{
    EnableViewCheck(1);
}

msw::CChrMarkerVolley::~CChrMarkerVolley()
{
}

void msw::CChrMarkerVolley::MainFunc()
{
}

// The marker comes up for a character that is in the game and, outside the
// party game, controlled by a pad. Never in state 0x58, and not while the
// spike jump marker is shown for the character (CChrMarkerVolleyJump takes
// over then). Character 14 is a special case: no marker while it is in
// state 0x27 or has bit 28 of its status flags set. (what the states, the
// bit and character 14 stand for is not known)
int msw::CChrMarkerVolley::AppearCheck()
{
    CChrCore *pChr;

    if (m_nChr < 0) {
        return 0;
    }
    pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr == 0 || pChr->IsDisable()) {
        return 0;
    }
    if (CGame::g_GameWork.GetGameType() != 2 && !pChr->IsPadCtrl()) {
        return 0;
    }
    if (pChr->GetState() == 0x58) {
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
int msw::CChrMarkerVolley::DisappearCheck()
{
    CChrCore *pChr;

    if (m_nChr < 0) {
        return 1;
    }
    pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr == 0 || pChr->IsDisable()) {
        return 1;
    }
    if (CGame::g_GameWork.GetGameType() != 2 && !pChr->IsPadCtrl()) {
        return 1;
    }
    if (pChr->GetState() == 0x58) {
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

// Tints the model's "marker_color01" material with colour m_Unk90, fully
// opaque.
msw::CChrMarkerVolleyJump::CChrMarkerVolleyJump(bcs::CTask *pParent, long nChr, long nType, long nArg)
    : CChrMarker(pParent, nChr, nType, nArg)
{
    EnableViewCheck(1);
    if (m_pMdl != 0) {
        bcs::BCS_Color color = GetColor_(m_Unk90);

        m_pMdl->SetMatTevColorRGB("marker_color01", GX_TEVREG0, color);
        m_pMdl->SetMatTevColorA("marker_color01", GX_TEVREG0, 0xFF);
    }
}

msw::CChrMarkerVolleyJump::~CChrMarkerVolleyJump()
{
}

// Turns the model about the Y axis by 5 degrees a frame, scaled by the frame
// rate, keeping the angle in [0, 2 pi]. (the original multiplies 5 by the
// factor at run time; here the compiler folds the product into one constant
// of the same value)
void msw::CChrMarkerVolleyJump::MainFunc()
{
    if (m_pMdl != 0) {
        float fSpeed = 5.0f * scDegToRad;
        float fStep = fSpeed * GetFrameRateF();
        float fRot = m_pMdl->m_fRotY + fStep;

        while (fRot > 6.2831855f) {
            fRot -= 6.2831855f;
        }
        while (fRot < 0.0f) {
            fRot += 6.2831855f;
        }
        m_pMdl->m_fRotY = fRot;
        m_pMdl->CalcRootMtx();
    }
}

// Shown while the spike jump marker is up for a character that is in the
// game and, outside the party game, controlled by a pad; character 14 again
// not in state 0x27 or with bit 28 of its status flags set.
int msw::CChrMarkerVolleyJump::AppearCheck()
{
    CChrCore *pChr;

    if (m_nChr < 0) {
        return 0;
    }
    pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr == 0 || pChr->IsDisable()) {
        return 0;
    }
    if (CGame::g_GameWork.GetGameType() != 2 && !pChr->IsPadCtrl()) {
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
        return 1;
    }
    return 0;
}

// AppearCheck reversed, except that without a character the marker stays
// (the original returns 0 there, unlike every other check).
int msw::CChrMarkerVolleyJump::DisappearCheck()
{
    CChrCore *pChr;

    if (m_nChr < 0) {
        return 0;
    }
    pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr == 0 || pChr->IsDisable()) {
        return 1;
    }
    if (CGame::g_GameWork.GetGameType() != 2 && !pChr->IsPadCtrl()) {
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
        return 0;
    }
    return 1;
}

float msw::CChrMarkerVolleyJump::GetDispScale() const
{
    return 1.6f;
}
