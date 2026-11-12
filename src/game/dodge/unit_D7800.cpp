// msw::CComDrvDodgeGame: answering a special shot of the driver's own team
// by moving its target marker over the other team's court. The dodgeball
// version of VolleyF's game/volley/unit_BEF84.cpp; the file name is not
// known.
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComDrvDodge.hpp"
#include "game/msw/CComManDodgeGame.hpp"

namespace msw {

namespace {
    // (names guessed) The work of action 0x2C: moving the target marker of
    // the team's special shot to a place picked by GetDatSpTargetMarker,
    // weaving round it along a curve.
    struct SP_OFF_MOVE_MARKER_WORK {
        unsigned long m_nUnk00;
        long m_nStep;           // 0x04
        float m_fTime;          // 0x08, frames on this target
        float m_fRetargetTime;  // 0x0C, frames since the target was renewed
        float m_fWeaveTime;     // 0x10, frames since the weave was renewed
        unsigned char m_Unk14[0x1C - 0x14];
        bcs::FVEC3 m_Target;    // 0x1C, where the marker goes
        bcs::FVEC3 m_Weave;     // 0x28, a random offset from it
        float m_fWeaveRot;      // 0x34, the turn of the curve's control point
        long m_nKind;           // 0x38, from GetDatSpTargetMarker
        long m_nChr;            // 0x3C, the character aimed at, -1 for none
        float m_fMoveTime;      // 0x40, how long to stay on this target
        float m_fRetarget;      // 0x44, how often to renew it
        float m_fStartFrame;    // 0x48, the shot's frame from which to aim late
        long m_bEarly;          // 0x4C
    };

    enum {
        ACT_SP_OFF_MOVE_MARKER = 0x2C
    };

    const float PI2 = 6.2831855f;

    // The parameter table entries, by character kind (CChrCore::
    // GetChrTypeID); each is the first of a pair, minimum and maximum, 8
    // entries apart. (names guessed)
    // How long to stay on a target of kinds 1 to 8,
    long s_aSpTargetTimeDatId[25] = {
        0x830, 0x8A8, 0x920, 0x830, 0x830, 0x998, 0xA10, 0xA88, 0xB00, 0x830, 0xB78, 0x830, 0xBF0,
        0x830, 0x830, 0x830, 0x830, 0x830, 0x830, 0x830, 0x830, 0x830, 0x830, 0x830, 0x830,
    };
    // the same for kind 0,
    long s_aSpTargetTime0DatId[25] = {
        0x840, 0x8B8, 0x930, 0x840, 0x840, 0x9A8, 0xA20, 0xA98, 0xB10, 0x840, 0xB88, 0x840, 0xC00,
        0x840, 0x840, 0x840, 0x840, 0x840, 0x840, 0x840, 0x840, 0x840, 0x840, 0x840, 0x840,
    };
    // the rates of the nine kinds (the first of nine, 8 entries apart).
    long s_aSpTargetRateDatId[25] = {
        0x850, 0x8C8, 0x940, 0x850, 0x850, 0x9B8, 0xA30, 0xAA8, 0xB20, 0x850, 0xB98, 0x850, 0xC10,
        0x850, 0x850, 0x850, 0x850, 0x850, 0x850, 0x850, 0x850, 0x850, 0x850, 0x850, 0x850,
    };

    // How often to renew the target, a pair as above.
    long s_aSpRetargetDatId[25] = {
        0x820, 0x898, 0x910, 0x820, 0x820, 0x988, 0xA00, 0xA78, 0xAF0, 0x820, 0xB68, 0x820, 0xBE0,
        0x820, 0x820, 0x820, 0x820, 0x820, 0x820, 0x820, 0x820, 0x820, 0x820, 0x820, 0x820,
    };

    // v clamped to [lo, hi]; a macro, the original calls the court edges
    // again for the value it returns.
#define SP_CLAMP(v, lo, hi) ((v) > (hi) ? (hi) : ((v) < (lo) ? (lo) : (v)))
}

void CComDrvDodgeGame::ActLoopSpOffMoveMarker()
{
    SP_OFF_MOVE_MARKER_WORK *pWork = static_cast<SP_OFF_MOVE_MARKER_WORK *>(GetCurrentActWork());
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 to(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 marker(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 ctrl(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 half(0.0f, 0.0f, 0.0f);
    int bRenew = 0;

    // Early in the shot the marker is aimed one way, later another.
    int bEarly = 1;
    CChrCore *pSp = GetLastSpChrCore();
    float fFrame = pSp == 0 ? 0.0f : pSp->GetAirTrickStatePassFrame();
    if (fFrame > pWork->m_fStartFrame) {
        bEarly = 0;
    }

    switch (pWork->m_nStep) {
        case 0:
            pWork->m_nStep++;
            break;
        case 1:
            if (CanbeMoveSpTargetMarkerPos()) {
                pWork->m_nStep++;
            }
            break;
        case 2:
            // Pick the next place to aim at.
            pWork->m_bEarly = bEarly;
            pWork->m_nKind = GetDatSpTargetMarker(&pWork->m_fMoveTime, bEarly);
            pWork->m_nChr = -1;
            if (!GetSpTargetMarkerPos(marker)) {
                ReturnAct(0);
                break;
            }
            if (!GetMoveSpTargetMarkerPos(pWork->m_Target, pWork->m_nKind, pWork->m_nChr)) {
                ReturnAct(0);
                break;
            }
            bRenew = 1;
            pWork->m_fTime = 0.0f;
            pWork->m_fRetargetTime = 0.0f;
            pWork->m_fWeaveTime = 1000.0f;
            pWork->m_nStep++;
            // fall through
        case 3:
            // Follow a target that moves (all but the random places).
            if (pWork->m_nKind != 7 && pWork->m_nKind != 8 && pWork->m_fRetargetTime >= pWork->m_fRetarget) {
                long nId = s_aSpRetargetDatId[m_pChr->GetChrTypeID()];
                float fMin = CComMan::GetDatF(nId, GetComLvF());
                pWork->m_fRetarget = RandRangeF(fMin, CComMan::GetDatF(nId + 8, GetComLvF()));
                pWork->m_fRetargetTime = 0.0f;
                if (!GetMoveSpTargetMarkerPos(pWork->m_Target, pWork->m_nKind, pWork->m_nChr)) {
                    ReturnAct(0);
                    break;
                }
            }
            if (!GetSpTargetMarkerPos(marker)) {
                ReturnAct(1);
                break;
            }

            if (pWork->m_nKind != 0) {
                // Weave round the target: a new offset now and then, or
                // once the marker is there.
                if (pWork->m_fWeaveTime >= 120.0f) {
                    bRenew = 1;
                }
                bcs::FVEC3Add(&to, &pWork->m_Target, &pWork->m_Weave);
                if (GetDisXZ(marker, to) <= 0.8f) {
                    bRenew = 1;
                }
                if (bRenew) {
                    float fMin;
                    float fMax;

                    pWork->m_fWeaveTime = 0.0f;
                    if (bEarly) {
                        fMin = CComMan::GetDatF(0x7D6, GetComLvF());
                        fMax = CComMan::GetDatF(0x7DE, GetComLvF());
                    } else {
                        fMin = CComMan::GetDatF(0x7E6, GetComLvF());
                        fMax = CComMan::GetDatF(0x7EE, GetComLvF());
                    }
                    pWork->m_Weave.x = 0.0f;
                    pWork->m_Weave.y = 0.0f;
                    pWork->m_Weave.z = 0.0f;
                    pWork->m_Weave.x = RandRangeF(fMin, fMax);
                    pWork->m_Weave.z = RandRangeF(fMin, fMax);
                    if (RandI() & 1) {
                        pWork->m_Weave.x = -pWork->m_Weave.x;
                    }
                    if (RandI() & 1) {
                        pWork->m_Weave.z = -pWork->m_Weave.z;
                    }

                    // A quarter turn either way, give or take a sixteenth.
                    pWork->m_fWeaveRot = 1.5707964f + RandRangeF(-0.19634955f, 0.19634955f);
                    if (RandI() & 1) {
                        pWork->m_fWeaveRot = -pWork->m_fWeaveRot;
                    }
                    float fRot = pWork->m_fWeaveRot;
                    while (fRot > PI2) {
                        fRot -= PI2;
                    }
                    while (fRot < 0.0f) {
                        fRot += PI2;
                    }
                    pWork->m_fWeaveRot = fRot;
                }

                // Along a curve whose control point is the halfway point
                // pushed aside by half the way turned.
                bcs::FVEC3Add(&to, &pWork->m_Target, &pWork->m_Weave);
                bcs::FVEC3Sub(&half, &to, &marker);
                bcs::FVEC3Scale(&half, &half, 0.5f);
                bcs::FVEC3Add(&ctrl, &half, &marker);
                bcs::BCS_Vec_RotY(half, half, pWork->m_fWeaveRot);
                bcs::FVEC3Add(&ctrl, &ctrl, &half);
                bcs::BCS_Vec_BetaSplineCurveXZ(pos, marker, ctrl, to, 0.5f);
                MethdMoveSpTargetMarkerPos(pos, 0.2f);
            }

            if (pWork->m_fTime >= pWork->m_fMoveTime) {
                pWork->m_nStep = 2;
            }
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// Draws what to aim the marker at (0 to 8, see GetMoveSpTargetMarkerPos) and
// how long to stay on it.
long CComDrvDodgeGame::GetDatSpTargetMarker(float *pTime, int bEarly)
{
    long nChrType = m_pChr->GetChrTypeID();
    long nTimeMinId = s_aSpTargetTimeDatId[nChrType];
    long nTime0MinId = s_aSpTargetTime0DatId[nChrType];
    long nRateId = s_aSpTargetRateDatId[nChrType];
    long nTimeMaxId = nTimeMinId + 8;
    long nTime0MaxId = nTime0MinId + 8;
    long aRate[9];

    aRate[0] = CComMan::GetDatI(nRateId, GetComLvF());
    aRate[1] = CComMan::GetDatI(nRateId + 0x08, GetComLvF());
    aRate[2] = CComMan::GetDatI(nRateId + 0x10, GetComLvF());
    aRate[3] = CComMan::GetDatI(nRateId + 0x18, GetComLvF());
    if (GetBadConditionEmInnerChr() < 0) {
        aRate[3] = 0;
    }
    aRate[4] = CComMan::GetDatI(nRateId + 0x20, GetComLvF());
    aRate[5] = CComMan::GetDatI(nRateId + 0x28, GetComLvF());
    aRate[6] = CComMan::GetDatI(nRateId + 0x30, GetComLvF());
    aRate[7] = CComMan::GetDatI(nRateId + 0x38, GetComLvF());
    if (!bEarly) {
        aRate[7] = 0;
    }
    aRate[8] = CComMan::GetDatI(nRateId + 0x40, GetComLvF());
    if (bEarly) {
        aRate[8] = 0;
    }

    long nKind = LotProbability(aRate, 9);
    if (nKind < 0) {
        nKind = 0;
    }

    float fMin;
    float fMax;
    if (nKind == 0) {
        fMin = CComMan::GetDatF(nTime0MinId, GetComLvF());
        fMax = CComMan::GetDatF(nTime0MaxId, GetComLvF());
    } else {
        fMin = CComMan::GetDatF(nTimeMinId, GetComLvF());
        fMax = CComMan::GetDatF(nTimeMaxId, GetComLvF());
    }
    if (pTime != 0) {
        *pTime = RandRangeF(fMin, fMax);
    }
    return nKind;
}

// Where to aim the marker for kind nKind: 0 where it is, 1 the character
// nearest to it, 2 the one nearest to the driver, 3 one in bad condition,
// 4 the one with the least life, 5 the most, 6 anyone, 7 and 8 anywhere.
// rChr keeps the character between calls (-1 to pick again).
int CComDrvDodgeGame::GetMoveSpTargetMarkerPos(bcs::FVEC3 &rPos, long nKind, long &rChr)
{
    rPos.x = 0.0f;
    rPos.y = 0.0f;
    rPos.z = 0.0f;

    bcs::FVEC3 marker;
    if (!GetSpTargetMarkerPos(marker)) {
        return 0;
    }

    switch (nKind) {
        case 0:
            rPos = marker;
            rPos.y = 0.0f;
            return 1;
        case 1:
            rChr = GetNearEmInnerChr(marker, 1, 0);
            if (rChr < 0) {
                return 0;
            }
            break;
        case 2:
            rChr = GetNearEmInnerChr(m_MyPos, 1, 0);
            if (rChr < 0) {
                return 0;
            }
            break;
        case 3:
            if (rChr < 0) {
                rChr = GetBadConditionEmInnerChr();
            }
            if (rChr < 0) {
                return 0;
            }
            break;
        case 4:
            if (rChr < 0) {
                rChr = GetLifeFewEmInnerChr();
            }
            if (rChr < 0) {
                return 0;
            }
            break;
        case 5:
            if (rChr < 0) {
                rChr = GetLifeLotEmInnerChr();
            }
            if (rChr < 0) {
                return 0;
            }
            break;
        case 6:
            if (rChr < 0) {
                CChrCore *pChr = CComManDodgeGame::GetInnerChrCoreRand(m_nEmTeam, 0, 0);
                if (pChr != 0) {
                    rChr = pChr->GetPlayerNo();
                }
            }
            if (rChr < 0) {
                return 0;
            }
            break;
        case 7:
        case 8: {
            float fMax = 0.7f * GetEmCourtRight();
            rPos.x = RandRangeF(0.7f * GetEmCourtLeft(), fMax);
            rPos.y = 0.0f;
            fMax = 0.7f * GetEmCourtBack();
            rPos.z = RandRangeF(0.7f * GetEmCourtFront(), fMax);
            return 1;
        }
        default:
            return 0;
    }

    if (rChr >= 0) {
        rPos = CComMan::GetChrPosXZ(rChr);
    }
    rPos.x = SP_CLAMP(rPos.x, 0.7f * GetEmCourtLeft(), 0.7f * GetEmCourtRight());
    rPos.z = SP_CLAMP(rPos.z, 0.7f * GetEmCourtFront(), 0.7f * GetEmCourtBack());
#undef SP_CLAMP
    return 1;
}

// Starts the marker action once a special shot of the team is out.
void CComDrvDodgeGame::WraperActCallOffSpStart()
{
    if (GetLastSpChrCore() != 0) {
        SP_OFF_MOVE_MARKER_WORK *pWork = static_cast<SP_OFF_MOVE_MARKER_WORK *>(EntryAct(ACT_SP_OFF_MOVE_MARKER));
        pWork->m_Target.x = 0.0f;
        pWork->m_Target.y = 0.0f;
        pWork->m_Target.z = 0.0f;
        pWork->m_Weave.x = 0.0f;
        pWork->m_Weave.y = 0.0f;
        pWork->m_Weave.z = 0.0f;
        pWork->m_fWeaveRot = 0.0f;
        pWork->m_nKind = 0;
        pWork->m_nChr = -1;
        pWork->m_fMoveTime = 0.0f;
        pWork->m_fRetarget = 0.0f;
        pWork->m_fStartFrame = 0.0f;
        pWork->m_bEarly = 1;
        float fMin = CComMan::GetDatF(0x7F6, static_cast<long>(m_pChr->GetChrTypeID()));
        pWork->m_fStartFrame = RandRangeF(fMin, CComMan::GetDatF(0x80B, static_cast<long>(m_pChr->GetChrTypeID())));
    }
}

void CComDrvDodgeGame::WraperActCallOffSpSub()
{
    ActCallGoHome();
}

}
