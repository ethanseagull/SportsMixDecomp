// The character markers of the basket game: msw::CChrMarkerBasket (and its
// variant for character 14), the arrow signal over a character of the
// user's team, the marker of a character's block moves, and the factory
// that creates them. The arrow signal is HockeyF's CArrowSignalHockey
// (game/hockey/CShootMarker.cpp) with other timings.
#include "game/msw/CChrMarkerBasket.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CArrowSignalBase.hpp"
#include "game/msw/CChr1on1MarkerBase.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "math.h"

namespace msw {

    // The arrow over a character of the user's team: it shows for a while,
    // then moves away from the character and fades.
    class CArrowSignalBasket : public CArrowSignalBase {
        public:
            CArrowSignalBasket(bcs::CTask *pParent, long nChrIdx, long nTeam, float fRotY);
            virtual ~CArrowSignalBasket() {}
            virtual void Main();
    };

    // The one-on-one marker put in front of a character for his block
    // moves (name from the vtable). Allocated with the base's 0x9C bytes.
    class CChrMoveBlockMarkerBasket : public CChr1on1MarkerBase {
        public:
            CChrMoveBlockMarkerBasket(bcs::CTask *pParent, long nChr)
                : CChr1on1MarkerBase(pParent, nChr)
            {
                Create();
            }
            virtual ~CChrMoveBlockMarkerBasket() {}

            virtual int CheckAppear();
            virtual float GetAppearFrameNum() const { return 16.0f; }
            virtual float GetDisappearFrameNum() const { return 12.0f; }
            virtual float GetDispScale() const { return 5.7f; }
            virtual void GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long nIdx);
    };
}

// Built by the file's static initialiser: nine pairs of marker colours, the
// same table as in DodgeF's CChrMarkerDodge.cpp (CChrMoveBlockMarkerBasket's
// GetColor_ reads it), so it most likely comes from a shared header (which
// one, and the names, are not known).
namespace {
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

    // (guessed helper) rad brought into [0, 2 pi].
    inline float NormalizeRad(float rad)
    {
        while (rad > 2.0f * 3.1415927f) {
            rad -= 2.0f * 3.1415927f;
        }
        while (rad < 0.0f) {
            rad += 2.0f * 3.1415927f;
        }
        return rad;
    }

    // (guessed helper) x limited to [0, 1].
    inline float Clamp01(float x)
    {
        return x > 1.0f ? 1.0f : (x < 0.0f ? 0.0f : x);
    }
}

// ---------------------------------------------------------------------------
// CChrMarkerBasket

// When the user's team changes (and no demo or pause is on, flag bits
// 0x4306), an arrow points out the character if he is on that team.
void msw::CChrMarkerBasket::ArrowSignalCheck()
{
    if (m_nFlag & 1) {
        if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
            m_Unk9C = CGame::g_GameWork.m_nUserTeam;
            return;
        }
        long nTeam = CGame::g_GameWork.m_nUserTeam;
        if (nTeam != (long)m_Unk9C) {
            CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
            if (pChr != 0 && pChr->IsEnable()) {
                long nChrTeam = pChr->GetTeamNo();
                if (nTeam == nChrTeam) {
                    long nGoal = pChr->GetTargetGoalIdx();
                    new (0x100) CArrowSignalBasket(this, m_nChr, nChrTeam, nGoal != 0 ? 0.0f : 3.1415927f);
                }
            }
            m_Unk9C = nTeam;
        }
    }
}

msw::CChrMarkerBasket::~CChrMarkerBasket()
{
}

// ---------------------------------------------------------------------------
// CChrMarkerBasketNinja

msw::CChrMarkerBasketNinja::~CChrMarkerBasketNinja()
{
}

// No marker in states 0x27 and 0x1E (meaning not known); otherwise as
// CChrMarker.
int msw::CChrMarkerBasketNinja::AppearCheck()
{
    CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr != 0) {
        long nState = pChr->GetState();
        if (nState == 0x27 || nState == 0x1E) {
            return 0;
        }
    }
    return CChrMarker::AppearCheck();
}

int msw::CChrMarkerBasketNinja::DisappearCheck()
{
    if (CChrMan::GetChrCore(m_nChr) == 0) {
        return 0;
    }
    return AppearCheck() == 0;
}

// ---------------------------------------------------------------------------
// CArrowSignalBasket
//
// Two timers: m_nState counts the arrow's approach (50 frames up, then 42
// frames moving out from 3.5 to 12.5 units away), m_nCount its fading (in
// over 5 frames, 77 frames up, out over 10).

msw::CArrowSignalBasket::CArrowSignalBasket(bcs::CTask *pParent, long nChrIdx, long nTeam, float fRotY)
    : CArrowSignalBase(pParent, nChrIdx, nTeam)
{
    CreateMdl_(2.6f, fRotY);

    m_Unk48 = 50.0f;
    m_nState = 0;
    if (m_Unk48 <= 0.0f) {
        m_Unk48 = 42.0f;
        m_Unk4C = 42.0f;
        m_Unk4C = 1.0f < m_Unk4C ? m_Unk4C : 1.0f;
        m_nState = 1;
    }
    m_Unk58 = 3.5f;

    m_Unk50 = 5.0f;
    m_Unk54 = 5.0f;
    m_nCount = 0;
    if (m_Unk50 <= 0.0f) {
        m_Unk5C = 1.0f;
        m_Unk50 = 77.0f;
        m_Unk54 = 77.0f;
        m_nCount = 1;
    }
    if (m_Unk50 <= 0.0f) {
        m_Unk5C = 1.0f;
        m_Unk50 = 10.0f;
        m_Unk54 = 10.0f;
        m_nCount++;
    }
    if (m_Unk50 <= 0.0f) {
        m_Unk5C = 0.0f;
        m_Unk50 = 0.0f;
        m_Unk54 = 0.0f;
        m_nCount++;
    }
}

void msw::CArrowSignalBasket::Main()
{
    float rate = GetFrameRateF();

    switch (m_nState) {
        case 0:
            m_Unk48 -= rate;
            if (m_Unk48 <= 0.0f) {
                m_Unk48 = 42.0f;
                m_Unk4C = 42.0f;
                m_Unk4C = 1.0f < m_Unk4C ? m_Unk4C : 1.0f;
                m_nState = 1;
            }
            break;
        case 1:
            m_Unk48 -= rate;
            if (m_Unk48 <= 0.0f) {
                m_nState = 2;
            }
            break;
    }

    switch (m_nCount) {
        case 0:
            m_Unk50 -= rate;
            if (m_Unk50 <= 0.0f) {
                m_Unk5C = 1.0f;
                m_Unk50 = 77.0f;
                m_Unk54 = 77.0f;
                m_Unk54 = 1.0f < m_Unk54 ? m_Unk54 : 1.0f;
                m_nCount++;
            } else {
                m_Unk5C = 1.0f - Clamp01(m_Unk50 / m_Unk54);
            }
            break;
        case 1:
            m_Unk50 -= rate;
            if (m_Unk50 <= 0.0f) {
                m_Unk50 = 10.0f;
                m_Unk54 = 10.0f;
                m_Unk54 = 1.0f < m_Unk54 ? m_Unk54 : 1.0f;
                m_nCount++;
            }
            break;
        case 2:
            m_Unk50 -= rate;
            if (m_Unk50 <= 0.0f) {
                m_Unk5C = 0.0f;
                m_Unk50 = 0.0f;
                m_Unk54 = 0.0f;
                m_nCount++;
            } else {
                m_Unk5C = Clamp01(m_Unk50 / m_Unk54);
            }
            break;
    }

    if (m_nState > 0) {
        float rest = Clamp01(m_Unk48 / m_Unk4C);
        float fEase = pow(1.0f - rest, 2.0);
        m_Unk58 = fEase * 9.0f + 3.5f;
    }

    CChrCore *pChr = CChrMan::GetChrCore(m_nChrIdx);
    if (pChr != 0 && pChr->IsEnable() && m_pMdl != 0) {
        bcs::FVEC3 chrPos = pChr->GetPos();
        bcs::FVEC3 pos;
        bcs::BCS_VecZ_RotY(pos, m_Unk58, m_pMdl->m_fRotY);
        pos.x += chrPos.x;
        pos.z += chrPos.z;
        m_pMdl->m_vTrans = pos;
        m_pMdl->SetAlphaRateF(Clamp01(200.0f * m_Unk5C / 255.0f));
    }

    if ((m_nState > 1 && m_nCount > 2) || m_nTeam != CGame::g_GameWork.m_nUserTeam) {
        Delete();
    }
}

// ---------------------------------------------------------------------------
// CChrMoveBlockMarkerBasket

// 2 (show) for a character in play, without bits 25 and 27 of the flag
// word at 0xEC (with which the pad driver ignores the pad), that shows the marker; it is put 1.5 units in front of him,
// along his block direction. Otherwise 3 (meanings as read from
// CChr1on1MarkerBase; guesses).
int msw::CChrMoveBlockMarkerBasket::CheckAppear()
{
    CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr != 0 && pChr->IsEnable() && !(pChr->GetFlagEC() & 0x0A000000) && pChr->IsDisp1On1Maker()) {
        bcs::FVEC3 chrPos = pChr->GetPos();
        bcs::FVEC3 pos;
        bcs::BCS_VecZ_RotY(pos, 1.5f, pChr->GetBlockWorkDir());
        pos.x += chrPos.x;
        pos.z += chrPos.z;
        m_fPosX = pos.x;
        m_fPosZ = pos.z;
        m_fRotY = NormalizeRad(pChr->GetBlockWorkDir());
        return 2;
    }
    return 3;
}

// The colour pair for nIdx (what the indices stand for is not known).
void msw::CChrMoveBlockMarkerBasket::GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long nIdx)
{
    long nPair = 0;
    switch (nIdx) {
        case 3:
            nPair = 1;
            break;
        case 9:
            nPair = 2;
            break;
        case 2:
            nPair = 3;
            break;
        case 6:
            nPair = 4;
            break;
        case 7:
            nPair = 5;
            break;
        case 10:
            nPair = 6;
            break;
        case 11:
            nPair = 7;
            break;
        case 17:
            nPair = 8;
            break;
    }
    rColor0 = s_MarkerColor[nPair * 2];
    rColor1 = s_MarkerColor[nPair * 2 + 1];
}

// ---------------------------------------------------------------------------
// CChrMarkerBasketFactory

msw::CChrMarker *msw::CChrMarkerBasketFactory::CreateMarker(bcs::CTask *pParent, long nChr, long nType, long nArg)
{
    CChrCore *pChr = CChrMan::GetChrCore(nChr);

    if (CGame::g_GameWork.m_nGameType == 0 && ((nType >= 2 && nType <= 3) || nType == 0)) {
        new (0) CChrMoveBlockMarkerBasket(pParent, nChr);
    }
    if (pChr->GetChrId() == 14) {
        return new (0) CChrMarkerBasketNinja(pParent, nChr, nType, nArg);
    }
    return new (0) CChrMarkerBasket(pParent, nChr, nType, nArg);
}
