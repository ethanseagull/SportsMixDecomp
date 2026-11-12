// msw::CComDrvDodgeGame: escaping the other team's special shot by keeping
// away from its target marker. Follows game/dodge/unit_D7800.cpp (the
// attacking side); the file name is not known.
#include "game/msw/CComDrvDodge.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CChrCoreDodge.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManDodgeGame.hpp"
#include "game/msw/CComOSG.hpp"
#include "nw4r/math/triangular.h"

namespace msw {

namespace {
    inline float DegToRad(float fDeg)
    {
        return fDeg * (3.1415927f / 180.0f);
    }

    // Set by the file's static initialiser and used by nothing in the file
    // (as in game/dodge/unit_A40EC.cpp).
    float s_fAngle60 = DegToRad(60.0f);

    // (names guessed) The work of action 0x2D: wait a moment, pick a place
    // away from the marker, run there and start again when the marker
    // comes near.
    struct SP_DEF_ESC_MARKER_WORK : CComDrv::ACT_WORK {
        bcs::FVEC3 m_Pos;       // 0x1C, where to run
        long m_nKind;           // 0x28, from GetSpDefEscKind
        float m_fStartFrame;    // 0x2C, the shot's frame from which to escape late
    };

    enum {
        ACT_SP_DEF_ESC_MARKER = 0x2D
    };

    const float PI2 = 6.2831855f;

    // The parameter table entries of the four escape kinds' rates, by the
    // shooter's character kind (each the first of four, 8 entries apart):
    // early in the shot and later. (names guessed)
    long s_aSpDefEscEarlyDatId[25] = {
        0xCD2, 0xD12, 0xD52, 0xCD2, 0xCD2, 0xCD2, 0xCD2, 0xCD2, 0xD92, 0xCD2, 0xCD2, 0xDD2, 0xCD2,
        0xCD2, 0xCD2, 0xCD2, 0xCD2, 0xCD2, 0xCD2, 0xCD2, 0xCD2, 0xCD2, 0xCD2, 0xCD2, 0xCD2,
    };
    long s_aSpDefEscLateDatId[25] = {
        0xCF2, 0xD32, 0xD72, 0xCF2, 0xCF2, 0xCF2, 0xCF2, 0xCF2, 0xDB2, 0xCF2, 0xCF2, 0xDF2, 0xCF2,
        0xCF2, 0xCF2, 0xCF2, 0xCF2, 0xCF2, 0xCF2, 0xCF2, 0xCF2, 0xCF2, 0xCF2, 0xCF2, 0xCF2,
    };

    // v clamped to [lo, hi]; a macro, the original calls the court edges
    // again for the value it returns.
#define SP_CLAMP(v, lo, hi) ((v) > (hi) ? (hi) : ((v) < (lo) ? (lo) : (v)))

    // (guessed helpers) rOut = rIn in the XZ plane scaled to length 1;
    // rVec made fLen long in the XZ plane, its height kept.
    inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::FVEC3 xz(rIn.x, 0.0f, rIn.z);
        bcs::BCS_Vec_Normalize(rOut, xz);
    }

    inline void SetLenXZ(bcs::FVEC3 &rVec, float fLen)
    {
        float y = rVec.y;
        NormalizeXZ(rVec, rVec);
        rVec.x *= fLen;
        rVec.z *= fLen;
        rVec.y = y;
    }
}

// The blast's radius (1 when there is no shot) and the character's body.
inline float CComDrvDodgeGame::GetSpHitRadius()
{
    CChrCoreDodge *pSp = static_cast<CChrCoreDodge *>(GetLastSpChrCore());
    float fRadius = pSp == 0 ? 1.0f : pSp->GetArtsExplodeRadius();
    return fRadius + m_pChr->GetCollSize_BodyDamage();
}

inline int CComDrvDodgeGame::IsInSpMarker(const bcs::FVEC3 &rPos)
{
    CChrCore *pSp = GetLastSpChrCore();
    if (pSp == 0) {
        return 0;
    }
    bcs::FVEC3 marker;
    if (!CComManDodgeGame::GetSpTargetMarkerPos(pSp, marker, 0)) {
        return 0;
    }
    float fLen = 0.3f + GetSpHitRadius();
    return GetDisXZ(marker, rPos) < fLen;
}

void CComDrvDodgeGame::ActLoopSpDefEscMarker()
{
    SP_DEF_ESC_MARKER_WORK *pWork = static_cast<SP_DEF_ESC_MARKER_WORK *>(GetCurrentActWork());
    if (GetLastSpChrCore() == 0) {
        ReturnAct(1);
        return;
    }

    // Early in the shot the driver escapes one way, later another.
    int bEarly = 1;
    CChrCore *pSp = GetLastSpChrCore();
    float fFrame = pSp == 0 ? 0.0f : pSp->GetAirTrickStatePassFrame();
    if (fFrame > pWork->m_fStartFrame) {
        bEarly = 0;
    }

    switch (pWork->m_nStep) {
        case 0: {
            pWork->m_nKind = GetSpDefEscKind(bEarly);
            if (bEarly && LotRate() < 20) {
                SetDefStanceMode(1);
            } else {
                SetDefStanceMode(2);
            }
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            float fMin = CComMan::GetDatF(0xC58, GetComLvF());
            pWork->m_fTime = -RandRangeF(fMin, CComMan::GetDatF(0xC60, GetComLvF()));
            pWork->m_nStep++;
            break;
        }
        case 1:
            if (pWork->m_fTime >= 0.0f) {
                pWork->m_nStep++;
            }
            break;
        case 2:
            if (pWork->m_nKind == 0) {
                pWork->m_fTime = -RandRangeF(60.0f, 120.0f);
                pWork->m_nStep = 30;
            } else if (GetSpDefEscMarkerPos(pWork->m_Pos, pWork->m_nKind)) {
                pWork->m_nStep++;
            }
            break;
        case 3:
            // Pick again when the marker has come to the place.
            if ((pWork->m_nKind == 1 || pWork->m_nKind == 2) && IsInSpMarker(pWork->m_Pos)) {
                pWork->m_nStep = 0;
                break;
            }
            if (MethdRunEx(pWork->m_Pos, 0.3f, 1.0f, 150.0f, 7, 10.0f)) {
                pWork->m_fTime = -RandRange(30.0f, 90.0f);
                pWork->m_nStep++;
            }
            break;
        case 4:
            if (IsInSpMarker(m_MyPos) || pWork->m_fTime >= 0.0f) {
                pWork->m_nStep = 0;
            }
            break;
        case 30:
            if (pWork->m_fTime >= 0.0f) {
                pWork->m_nStep = 0;
            }
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// Draws how to escape the shot (see GetSpDefEscMarkerPos).
long CComDrvDodgeGame::GetSpDefEscKind(int bEarly)
{
    CChrCore *pSp = GetLastSpChrCore();
    if (pSp == 0) {
        return 0;
    }

    // bEarly is reused for the table entry (the original keeps both in
    // one register; a fresh local is allocated the other way round).
    long nChrType = pSp->GetChrTypeID();
    if (bEarly) {
        bEarly = s_aSpDefEscEarlyDatId[nChrType];
    } else {
        bEarly = s_aSpDefEscLateDatId[nChrType];
    }
    long nId = bEarly;
    long aRate[16];
    aRate[0] = CComMan::GetDatI(nId, GetComLvF());
    aRate[1] = CComMan::GetDatI(nId + 0x08, GetComLvF());
    aRate[2] = CComMan::GetDatI(nId + 0x10, GetComLvF());
    aRate[3] = CComMan::GetDatI(nId + 0x18, GetComLvF());
    if (nChrType == 3) {
        aRate[1] = 0;
        aRate[2] = 0;
    }

    long nKind = LotProbability(aRate, 4);
    if (nKind == 0) {
        return 0;
    }
    if (nKind == 1) {
        return 1;
    }
    return nKind == 2 ? 2 : 3;
}

// Where to escape to for kind nKind: 0 where the driver is, 1 an open place
// away from the marker, 2 the same off the front or back line, 3 anywhere in
// the court (round the middle for character kind 3). Zero when there is no
// marker.
int CComDrvDodgeGame::GetSpDefEscMarkerPos(bcs::FVEC3 &rPos, long nKind)
{
    rPos = m_MyPos;
    rPos.y = 0.0f;

    CChrCore *pSp = GetLastSpChrCore();
    if (pSp == 0) {
        return 0;
    }
    long nChrType = pSp->GetChrTypeID();
    bcs::FVEC3 marker;
    if (!CComManDodgeGame::GetSpTargetMarkerNearPos(marker, pSp, m_pChr)) {
        return 0;
    }
    if (nKind == 0) {
        return 1;
    }

    if (nKind == 3) {
        if (nChrType == 3) {
            float fLen = 2.8f;
            float fRot = RandRangeF(0.0f, PI2);
            while (fRot > PI2) {
                fRot -= PI2;
            }
            while (fRot < 0.0f) {
                fRot += PI2;
            }
            rPos.x = CComMan::GetStageCourtCenterX(m_nUnk98);
            rPos.z = CComMan::GetStageCourtCenterZ(m_nUnk98);
            rPos.x += fLen * nw4r::math::SinRad(fRot);
            rPos.z += fLen * nw4r::math::CosRad(fRot);
        } else {
            rPos.x = RandRangeF(GetMyCourtLeft(), GetMyCourtRight());
            rPos.z = RandRangeF(GetMyCourtFront(), GetMyCourtBack());
        }
        return 1;
    }

    // An open cell of the court away from the marker.
    CComOSG grid;
    long x = 0;
    long y = 0;
    grid.Init(5, 5, m_nUnk98);
    grid.FillOne(marker, 1, -1);
    grid.FillCircum(marker, 1, -1);
    if (nKind == 2) {
        if (m_nUnk98 == 0) {
            grid.FillFront(1, 2, -1);
        } else {
            grid.FillBack(3, 2, -1);
        }
    }

    // The fewer left in the infield, the more often the nearest cell.
    long nNum = CChrMan::GetInnerChrNum(m_nMyTeam);
    long nRate;
    if (nNum == 1) {
        nRate = 70;
    } else if (nNum == 2) {
        nRate = 50;
    } else {
        nRate = 30;
    }
    if (LotRate() < nRate) {
        if (grid.SearchValueNearPos(x, y, 0, m_MyPos)) {
            grid.GetPos(rPos, x, y, 1);
        }
    } else {
        if (grid.SearchValue(x, y, 0, 1)) {
            grid.GetPos(rPos, x, y, 1);
        }
    }

    // Still within the blast: push it out along the line from the marker.
    float fLen = 1.0f + GetSpHitRadius();
    if (GetDisXZ(marker, rPos) < fLen) {
        bcs::FVEC3 dir;
        bcs::FVEC3 diff;
        diff.x = rPos.x - marker.x;
        diff.z = rPos.z - marker.z;
        NormalizeXZ(dir, diff);
        SetLenXZ(dir, fLen);
        rPos.x = dir.x + marker.x;
        rPos.z = dir.z + marker.z;
    }

    rPos.y = 0.0f;
    rPos.x = SP_CLAMP(rPos.x, GetMyCourtLeft(), GetMyCourtRight());
    rPos.z = SP_CLAMP(rPos.z, GetMyCourtFront(), GetMyCourtBack());
#undef SP_CLAMP
    return 1;
}

// Starts escaping the special shot just played by the other team.
void CComDrvDodgeGame::WraperActCallDefSpStart()
{
    if (GetLastSpChrCore() != 0) {
        SP_DEF_ESC_MARKER_WORK *pWork = static_cast<SP_DEF_ESC_MARKER_WORK *>(EntryAct(ACT_SP_DEF_ESC_MARKER));
        pWork->m_nKind = 0;
        pWork->m_fStartFrame = 0.0f;
        CChrCore *pSp = GetLastSpChrCore();
        if (pSp != 0) {
            float fMin = CComMan::GetDatF(0xC78, static_cast<long>(pSp->GetChrTypeID()));
            pWork->m_fStartFrame = RandRangeF(fMin, CComMan::GetDatF(0xC8D, static_cast<long>(pSp->GetChrTypeID())));
        }
    }
}

}
