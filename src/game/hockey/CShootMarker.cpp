// The markers of the hockey game: msw::CChrMarkerHockey (and its arrow
// variant), the arrow signal over a character of the user's team, the shoot
// marker (where a character aims at the goal) and the one-on-one marker.
#include "game/msw/CChrMarkerHockey.hpp"
#include "game/msw/CShootMarker.hpp"
#include "game/msw/CChr1on1MarkerHockey.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CArrowSignalBase.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "math.h"
#include "nw4r/math/triangular.h"

namespace msw {

    // The arrow over a character of the user's team: it shows for a while,
    // then shrinks towards the character and fades. Not exported from
    // HockeyF (its functions are unnamed there); the name follows BasketF's
    // CArrowSignalBasket, which has the same code.
    class CArrowSignalHockey : public CArrowSignalBase {
        public:
            CArrowSignalHockey(bcs::CTask *pParent, long nChrIdx, long nTeam, float fRotY);
            virtual ~CArrowSignalHockey() {}
            virtual void Main();
    };
}

// Built by the file's static initialiser. The four team colours of the
// shoot marker (CreateObj_ reads the first two: the user's team and the
// other), and nine pairs of marker colours of which the one-on-one marker
// reads the first; the same pairs are in other marker files (BasketF's
// CBallMarkerBasket.cpp, HockeyF's CBallMarkerHockey.cpp), so they most
// likely come from a shared header (which one, and the names, are not known).
namespace {
    bcs::BCS_Color s_TeamColor[4] = {
        bcs::BCS_Color(0x0088FFB7),
        bcs::BCS_Color(0xCC0000B7),
        bcs::BCS_Color(0x22BB00B7),
        bcs::BCS_Color(0xFFFF00B7),
    };

    bcs::BCS_Color s_MarkerColor[18] = {
        bcs::BCS_Color(0x3C, 0x14, 0x00, 0xFF), bcs::BCS_Color(0xFF, 0xE6, 0x00, 0xB4),
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
// CChrMarkerHockey

msw::CChrMarkerHockey::CChrMarkerHockey(bcs::CTask *pParent, long nChr, long nType, long nArg)
    : CChrMarker(pParent, nChr, nType, nArg)
{
}

msw::CChrMarkerHockey::~CChrMarkerHockey()
{
}

// Types 6 and 7 turn with their character.
void msw::CChrMarkerHockey::MainFunc()
{
    if (m_nType == 6 || m_nType == 7) {
        CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
        if (pChr != 0) {
            m_pMdl->SetRotY(pChr->GetRotY());
            m_pMdl->CalcRootMtx();
            m_pMdl->CalcWorldMatrixAll(0);
        }
    }
}

// Up for a character in play that is not out of the game (status bits
// 0x1000, 0x80000), not in state 0x4E, not a keeper outside his move mode,
// nor character 14 in state 0x27 or with status bit 0x10000000. Then by the
// marker type: types 1 and 7 are for characters the computer drives, the
// others for pad-driven ones; types 6 and 7 show while the tackle marker is
// on, the others while it is off; none shows with the arrow marker on.
// (what the types, character 14 and the states stand for is not known)
int msw::CChrMarkerHockey::AppearCheck()
{
    if (m_nChr < 0) {
        return 0;
    }
    CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr != 0) {
        pChr->IsKeeper(); // (result not used)
        if (pChr->IsDisable()) {
            return 0;
        }
        if (pChr->GetStatusFlag() & 0x1000) {
            return 0;
        }
        if (pChr->GetStatusFlag() & 0x80000) {
            return 0;
        }
        if (pChr->GetState() == 0x4E) {
            return 0;
        }
        if (pChr->IsKeeper() && !pChr->IsHockeyNowKeeperMoveMode()) {
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
        if (pChr->IsPadCtrl() && m_nType != 1 && m_nType != 7) {
            if (!pChr->IsHockeyChgArrowMarker()) {
                if (m_nType == 6) {
                    if (pChr->IsHockeyTackleMarker()) {
                        return 1;
                    }
                } else if (!pChr->IsHockeyTackleMarker()) {
                    return 1;
                }
            }
        } else if (!pChr->IsPadCtrl() && !pChr->IsHockeyChgArrowMarker()) {
            if (m_nType == 7) {
                if (pChr->IsHockeyTackleMarker()) {
                    return 1;
                }
            } else if (m_nType == 1 && !pChr->IsHockeyTackleMarker()) {
                return 1;
            }
        }
    }
    return 0;
}

// The same tests as AppearCheck, the other way round.
int msw::CChrMarkerHockey::DisappearCheck()
{
    if (m_nChr < 0) {
        return 1;
    }
    CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr != 0) {
        pChr->IsKeeper(); // (result not used)
        if (pChr->IsDisable()) {
            return 1;
        }
        if (pChr->GetStatusFlag() & 0x1000) {
            return 1;
        }
        if (pChr->GetStatusFlag() & 0x80000) {
            return 1;
        }
        if (pChr->GetState() == 0x4E) {
            return 1;
        }
        if (pChr->IsKeeper() && !pChr->IsHockeyNowKeeperMoveMode()) {
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
        if (pChr->IsPadCtrl() && m_nType != 1 && m_nType != 7) {
            if (m_nType == 6) {
                if (pChr->IsHockeyTackleMarker()) {
                    return 0;
                }
            } else {
                if (pChr->IsHockeyTackleMarker()) {
                    return 1;
                }
                return pChr->IsHockeyChgArrowMarker();
            }
        } else if (!pChr->IsPadCtrl()) {
            if (m_nType == 7) {
                if (pChr->IsHockeyTackleMarker()) {
                    return 0;
                }
            } else if (m_nType == 1) {
                if (pChr->IsHockeyTackleMarker()) {
                    return 1;
                }
                return pChr->IsHockeyChgArrowMarker();
            }
        }
    }
    return 1;
}

// When the user's team changes (and no demo or pause is on, flag bits
// 0x4306), an arrow points out the character if he is on that team.
void msw::CChrMarkerHockey::ArrowSignalCheck()
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
                    new (0x100) CArrowSignalHockey(this, m_nChr, nChrTeam, nGoal != 0 ? 0.0f : 3.1415927f);
                }
            }
            m_Unk9C = nTeam;
        }
    }
}

// ---------------------------------------------------------------------------
// CArrowSignalHockey
//
// Two timers: m_nState counts the arrow's approach (50 frames up, then 60
// frames moving in from 3.5 to 17 units away), m_nCount its fading (in over
// 5 frames, 85 frames up, out over 20).

msw::CArrowSignalHockey::CArrowSignalHockey(bcs::CTask *pParent, long nChrIdx, long nTeam, float fRotY)
    : CArrowSignalBase(pParent, nChrIdx, nTeam)
{
    CreateMdl_(2.6f, fRotY);

    m_Unk48 = 50.0f;
    m_nState = 0;
    if (m_Unk48 <= 0.0f) {
        m_Unk48 = 60.0f;
        m_Unk4C = 60.0f;
        m_Unk4C = 1.0f < m_Unk4C ? m_Unk4C : 1.0f;
        m_nState = 1;
    }
    m_Unk58 = 3.5f;

    m_Unk50 = 5.0f;
    m_Unk54 = 5.0f;
    m_nCount = 0;
    if (m_Unk50 <= 0.0f) {
        m_Unk5C = 1.0f;
        m_Unk50 = 85.0f;
        m_Unk54 = 85.0f;
        m_nCount = 1;
    }
    if (m_Unk50 <= 0.0f) {
        m_Unk5C = 1.0f;
        m_Unk50 = 20.0f;
        m_Unk54 = 20.0f;
        m_nCount++;
    }
    if (m_Unk50 <= 0.0f) {
        m_Unk5C = 0.0f;
        m_Unk50 = 0.0f;
        m_Unk54 = 0.0f;
        m_nCount++;
    }
}

void msw::CArrowSignalHockey::Main()
{
    float rate = GetFrameRateF();

    switch (m_nState) {
        case 0:
            m_Unk48 -= rate;
            if (m_Unk48 <= 0.0f) {
                m_Unk48 = 60.0f;
                m_Unk4C = 60.0f;
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
                m_Unk50 = 85.0f;
                m_Unk54 = 85.0f;
                m_Unk54 = 1.0f < m_Unk54 ? m_Unk54 : 1.0f;
                m_nCount++;
            } else {
                m_Unk5C = 1.0f - Clamp01(m_Unk50 / m_Unk54);
            }
            break;
        case 1:
            m_Unk50 -= rate;
            if (m_Unk50 <= 0.0f) {
                m_Unk50 = 20.0f;
                m_Unk54 = 20.0f;
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
        m_Unk58 = fEase * 13.5f + 3.5f;
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
// CChrMarkerHockeyArrow

// Marker type 4. Sets bit 0x4000 of the model's scene-object flags
// (meaning not known).
msw::CChrMarkerHockeyArrow::CChrMarkerHockeyArrow(bcs::CTask *pParent, long nChr, long nArg)
    : CChrMarker(pParent, nChr, 4, nArg)
{
    if (m_pMdl != 0) {
        m_pMdl->OnFlag(0x4000);
    }
}

msw::CChrMarkerHockeyArrow::~CChrMarkerHockeyArrow()
{
}

// Turns the arrow the way the character's arrow marker points.
void msw::CChrMarkerHockeyArrow::MainFunc()
{
    if (m_nChr < 0 || m_pMdl == 0) {
        return;
    }
    CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr != 0 && pChr->IsHockeyChgArrowMarker()) {
        m_pMdl->SetRotY(pChr->GetHockeyArrowMarkerRotY());
    }
}

int msw::CChrMarkerHockeyArrow::AppearCheck()
{
    if (m_nChr < 0) {
        return 0;
    }
    CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr != 0) {
        return pChr->IsHockeyChgArrowMarker();
    }
    return 0;
}

int msw::CChrMarkerHockeyArrow::DisappearCheck()
{
    return AppearCheck() == 0;
}

// ---------------------------------------------------------------------------
// CShootMarker

msw::CShootMarker::CShootMarker(bcs::CTask *pParent, long nTeam)
    : bcs::CTask(pParent, 0x15)
{
    OnUnk08(1);
    m_nTeam = nTeam;
    m_bReset = 1;
    CreateObj_(nTeam);
    m_fRotY = -1.0f;
    m_Unk54 = 0.0f;
    m_nChr = -1;
}

msw::CShootMarker::~CShootMarker()
{
    if (m_pMdl != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
    }
    m_pMdl = 0;
    if (m_pMdlGround != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdlGround);
    }
    m_pMdlGround = 0;
}

// The target is coloured for the user's team or the other one.
void msw::CShootMarker::CreateObj_(long nTeam)
{
    nw4r::g3d::ResFile res = CChrMarker::GetRsrcPack();

    nw4r::g3d::ResMdl mdl = res.GetResMdl("mh_target_goal01_01");
    m_pMdl = new (0) bcs::CMdlObj(mdl, 0, 0);
    m_pMdl->OnFlag(0x100);
    m_pMdl->OnFlag(2);
    m_pMdl->SetExBlendMode(1);
    m_pMdl->OnFlag(0x2000);
    m_pMdl->m_vTrans.Set(0.0f, 0.2f, 0.0f);
    if ((unsigned long)nTeam <= 3) {
        long nColor = 0;
        if (nTeam != CGame::g_GameWork.m_nUnk00) {
            nColor = 1;
        }
        m_pMdl->SetMatTevColor("marker_color00", GX_TEVREG0, s_TeamColor[nColor]);
    }
    bcs::CRenderMan::PushBack(m_pMdl, 2);
    m_pMdl->CalcRootMtx();
    m_pMdl->CalcWorldMatrixAll(0);

    nw4r::g3d::ResMdl mdlGround = res.GetResMdl("mh_target_goal01_02");
    m_pMdlGround = new (0) bcs::CMdlObj(mdlGround, 0, 0);
    m_pMdlGround->OnFlag(0x100);
    m_pMdlGround->OnFlag(2);
    m_pMdlGround->SetExBlendMode(1);
    m_pMdlGround->OnFlag(0x2000);
    m_pMdlGround->MatSetZModeAll(1, GX_ALWAYS, 0);
    m_pMdlGround->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    bcs::CRenderMan::PushBack(m_pMdlGround, 1);
    m_pMdlGround->CalcRootMtx();
    m_pMdlGround->CalcWorldMatrixAll(0);
}

void msw::CShootMarker::MainFunc(float rate)
{
    if (AppearCheck()) {
        m_pMdl->m_vScale.Set(2.3f, 1.0f, 2.5f);
        m_pMdlGround->m_vScale.Set(2.3f, 1.0f, 2.5f);
        m_pMdl->SetAlphaRateF(1.0f);
        m_pMdlGround->SetAlphaRateF(1.0f);
        m_pMdl->SetRotY(m_fRotY);
        m_pMdlGround->SetRotY(m_fRotY);
        m_vPos.y = 0.2f;
        m_pMdl->m_vTrans = m_vPos;
        m_vPos.y = 0.0f;
        m_pMdlGround->m_vTrans = m_vPos;
    } else {
        m_pMdl->SetAlphaRateF(0.0f);
        m_pMdlGround->SetAlphaRateF(0.0f);
    }
    m_pMdl->CalcRootMtx();
    m_pMdl->CalcWorldMatrixAll(0);
    m_pMdlGround->CalcRootMtx();
    m_pMdlGround->CalcWorldMatrixAll(0);
}

void msw::CShootMarker::UpdateFunc(float rate)
{
}

void msw::CShootMarker::Main()
{
    MainFunc(1.0f);
}

// The marker follows the character of the team that holds the shooting
// flag (status bit 0), or else the first other candidate: a character of
// the team in play, not a keeper, that shows the shoot marker (status bit
// 1 clear). It is put where he aims and turned along the line from him to
// a point five units in front of the goal.
int msw::CShootMarker::AppearCheck()
{
    long nCand = -1;
    long candidate[4];
    long nShooter = -1;
    CChrCore *pShooter = 0;

    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && !pChr->IsKeeper() && m_nTeam == pChr->GetTeamNo()
            && pChr->IsHockeyDispShootMarker() && !(pChr->GetStatusFlag() & 2)) {
            pChr->SetUseShootMarker(0);
            nCand++;
            candidate[nCand] = i;
            if (pChr->GetStatusFlag() & 1) {
                nShooter = i;
                pShooter = pChr;
            }
        }
    }

    if (nCand < 0) {
        m_nChr = -1;
    } else {
        for (long i = 0; i <= nCand; i++) {
            if (m_nChr < 0 && nShooter != candidate[i]) {
                m_nChr = candidate[i];
            }
        }
        if (nShooter < 0) {
            CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
            if (pChr != 0 && !pChr->IsDisable()) {
                pShooter = pChr;
            }
        }
    }

    if (pShooter == 0) {
        m_bReset = 1;
        return 0;
    }

    pShooter->SetUseShootMarker(1);
    bcs::FVEC3 pos;
    pShooter->GetHockeyShootGoalTgtPos(pos, m_fRotY);
    m_vPos = pos;

    bcs::FVEC3 dir = CGame::GetGoalPos(pShooter->GetTargetGoalIdx());
    float fRot = NormalizeRad(3.1415927f + pShooter->GetTargetGoalYRot());
    dir.x += 5.0f * nw4r::math::SinRad(fRot);
    dir.z += 5.0f * nw4r::math::CosRad(fRot);
    const bcs::FVEC3 &rChrPos = pShooter->GetPos();
    dir.x -= rChrPos.x;
    dir.z -= rChrPos.z;
    m_fRotY = NormalizeRad(nw4r::math::Atan2Rad(dir.x, dir.z));
    return 1;
}

// ---------------------------------------------------------------------------
// CChr1on1MarkerHockey

msw::CChr1on1MarkerHockey::CChr1on1MarkerHockey(bcs::CTask *pParent, long nChr)
    : CChr1on1MarkerBase(pParent, nChr)
{
    Create();
}

int msw::CChr1on1MarkerHockey::CheckAppear()
{
    return 3;
}

float msw::CChr1on1MarkerHockey::GetAppearFrameNum() const
{
    return 16.0f;
}

float msw::CChr1on1MarkerHockey::GetDisappearFrameNum() const
{
    return 12.0f;
}

float msw::CChr1on1MarkerHockey::GetDispScale() const
{
    return 4.0f;
}

void msw::CChr1on1MarkerHockey::GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long nIdx)
{
    rColor0 = s_MarkerColor[0];
    rColor1 = s_MarkerColor[1];
}
