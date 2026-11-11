// msw::CComDrvVolleyGame: answering a special shot of the driver's own team
// (moving its target marker, Bowser's fire shot). A file of its own after
// game/volley/CStageVolley12.cpp; the file name is not known.
#include "game/msw/CComDrvVolleyGame.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComMan.hpp"

namespace {
    inline float Clamp(float fValue, float fMin, float fMax)
    {
        if (fValue > fMax) {
            return fMax;
        }
        if (fValue < fMin) {
            return fMin;
        }
        return fValue;
    }

    // (names guessed) The work of action 0x20: moving the target marker of
    // the team's special shot along a curve to the place to aim at.
    struct SP_OFF_MOVE_MARKER_WORK : msw::CComDrv::ACT_WORK {
        bcs::FVEC3 m_Target;   // 0x1C, where the marker goes
        bcs::FVEC3 m_Bend;     // 0x28, added to the curve's middle point
        long m_nKind;          // 0x34, from GetDatSpTargetMarker
        float m_fMoveTime;     // 0x38 \ how long, and how far, to move it
        float m_fMoveDist;     // 0x3C /   before picking a new target
        float m_fMoved;        // 0x40
        float m_fStartFrame;   // 0x44, the shot's frame from which to aim far
        float m_fRate;         // 0x48, how far along the curve
        long m_nCount;         // 0x4C, targets picked so far
        long m_bEarly;         // 0x50
    };

    // (names guessed) The work of action 0x22: Bowser's fire shot, aimed at a
    // character of the other team (kind 0), anywhere in its court (kind 1)
    // or near the middle of the court (kind 2).
    struct SP_KOOPA_OFF_WORK : msw::CComDrv::ACT_WORK {
        long m_nKind;          // 0x1C
        long m_nTarget;        // 0x20, the character aimed at, -1 for none
        bcs::FVEC3 m_Pos;      // 0x24
    };
}

void msw::CComDrvVolleyGame::ActCallSpOffMoveMarker()
{
    SP_OFF_MOVE_MARKER_WORK *pWork = static_cast<SP_OFF_MOVE_MARKER_WORK *>(EntryAct(0x20));
    pWork->m_nKind = 0;
    pWork->m_nCount = 0;
    pWork->m_fStartFrame = 0.0f;
    pWork->m_bEarly = 1;
    float fMin = CComMan::GetDatF(0x2E3, GetChrTypeID());
    pWork->m_fStartFrame = RandRangeF(fMin, CComMan::GetDatF(0x2F8, GetChrTypeID()));
}

void msw::CComDrvVolleyGame::ActLoopSpOffMoveMarker()
{
    SP_OFF_MOVE_MARKER_WORK *pWork = static_cast<SP_OFF_MOVE_MARKER_WORK *>(GetCurrentActWork());
    bcs::FVEC3 pos;
    bcs::FVEC3 ctrl(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 marker(0.0f, 0.0f, 0.0f);

    // Early in the shot the marker is aimed near, later far.
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
            if (CanbeMoveTargetMarkerPos()) {
                pWork->m_nStep++;
            }
            break;
        case 2: {
            // Pick the next place to aim at.
            pWork->m_bEarly = bEarly;
            pWork->m_nKind = GetDatSpTargetMarker(&pWork->m_fMoveTime, &pWork->m_fMoveDist, bEarly);
            if (!GetTargetMarkerPos(marker)) {
                ReturnAct(0);
                break;
            }
            if (!GetMoveTargetMarkerPos(pos, pWork->m_nKind)) {
                ReturnAct(0);
                break;
            }

            // After the first target the curve bends on in the direction
            // the marker was going.
            if (pWork->m_nCount <= 0) {
                pWork->m_Bend.Set(0.0f, 0.0f, 0.0f);
            } else {
                bcs::BCS_Vec_Normalize(pWork->m_Bend,
                                       bcs::FVEC3(pWork->m_Target.x - marker.x, 0.0f, pWork->m_Target.z - marker.z));
                pWork->m_Bend.x *= 4.0f;
                pWork->m_Bend.z *= 4.0f;
            }
            pWork->m_Target = pos;
            if (GetDisXZ(marker, pWork->m_Target) < 3.0f) {
                pWork->m_fRate = 1.0f;
            } else {
                pWork->m_fRate = 0.0f;
            }
            pWork->m_nCount++;
            pWork->m_fTime = 0.0f;
            pWork->m_fMoved = 0.0f;
            pWork->m_nStep++;
        }
            // fall through
        case 3: {
            if (!GetTargetMarkerPos(marker)) {
                ReturnAct(1);
                break;
            }

            // Along a curve through a fifth of the way, bent.
            bcs::FVEC3Sub(&ctrl, &pWork->m_Target, &marker);
            bcs::FVEC3Scale(&ctrl, &ctrl, 0.2f);
            bcs::FVEC3Add(&ctrl, &ctrl, &marker);
            bcs::FVEC3Add(&ctrl, &ctrl, &pWork->m_Bend);
            bcs::BCS_Vec_BetaSplineCurveXZ(pos, marker, ctrl, pWork->m_Target, pWork->m_fRate);

            pWork->m_fRate += m_pChr->m_fFrameStep / 45.0f;
            pWork->m_fRate = Clamp(pWork->m_fRate, 0.0f, 1.0f);

            int bMoved;
            if (pWork->m_fRate >= 1.0f) {
                bMoved = MethdMoveTargetMarkerPos(pos, 0.2f);
            } else {
                bMoved = MethdMoveTargetMarkerPos(pos, -1.0f);
            }
            if (bMoved) {
                pWork->m_fMoved += m_pChr->m_fFrameStep;
            }

            if (pWork->m_fTime >= pWork->m_fMoveTime || pWork->m_fMoved > pWork->m_fMoveDist) {
                pWork->m_nStep = 2;
            }
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void msw::CComDrvVolleyGame::ActCallSpKoopaOff()
{
    SP_KOOPA_OFF_WORK *pWork = static_cast<SP_KOOPA_OFF_WORK *>(EntryAct(0x22));

    long aRate[3];
    aRate[0] = CComMan::GetDatI(0x93D, GetComLvF());
    aRate[1] = CComMan::GetDatI(0x945, GetComLvF());
    aRate[2] = CComMan::GetDatI(0x94D, GetComLvF());
    pWork->m_nKind = LotProbability(aRate, 3);
    pWork->m_nTarget = -1;
    pWork->m_fTime = 1000.0f;
    pWork->m_Pos.x = RandRangeF(GetEmCourtLeft(), GetEmCourtRight());
    pWork->m_Pos.y = 0.0f;
    pWork->m_Pos.z = RandRangeF(GetEmCourtFront(), GetEmCourtBack());

    switch (pWork->m_nKind) {
        case 0: {
            CChrCore *pChr = CComMan::GetChrCoreRand(m_nEmTeam);
            if (pChr != 0) {
                long nChr = pChr->GetPlayerNo();
                pWork->m_nTarget = nChr;
                pWork->m_Pos = CComMan::GetChrPos(nChr);
            }
            break;
        }
        case 1:
            pWork->m_Pos.x = CComMan::GetStageCourtCenterX(m_nCourtIdx) + RandRangeF(-2.0f, 2.0f);
            pWork->m_Pos.y = 0.0f;
            pWork->m_Pos.z = CComMan::GetStageCourtCenterZ(m_nCourtIdx) + RandRangeF(-2.0f, 2.0f);
            break;
    }
}

void msw::CComDrvVolleyGame::ActLoopSpKoopaOff()
{
    SP_KOOPA_OFF_WORK *pWork = static_cast<SP_KOOPA_OFF_WORK *>(GetCurrentActWork());

    // Once the fire is out, go after its marker (step 100 on).
    if (pWork->m_nStep < 100 && m_pChr->IsVolleyArtsKoopaShotFire()) {
        pWork->m_nStep = 100;
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~0x4;
            m_fUnk54 = 0.0f;
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 1:
            if (CanbeMoveTargetMarkerPos()) {
                pWork->m_nStep++;
            }
            break;
        case 2: {
            bcs::FVEC3 marker;
            if (GetTargetMarkerPos(marker)) {
                pWork->m_nStep++;
            }
            break;
        }
        case 3:
            // Follow the character aimed at, looking again now and then.
            if (pWork->m_nKind == 0 && pWork->m_nTarget >= 0 && pWork->m_fTime >= 0.0f) {
                float fMin = CComMan::GetDatF(0x29, GetComLvF());
                pWork->m_fTime = -RandRangeF(fMin, CComMan::GetDatF(0x31, GetComLvF()));
                pWork->m_Pos = CComMan::GetChrPos(pWork->m_nTarget);
            }
            MethdMoveTargetMarkerPos(pWork->m_Pos, 0.2f);
            break;
        case 100:
            pWork->m_nStep++;
            break;
        case 101:
            Reset();
            ActCallSpOffMoveMarker();
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// Bowser's shot has its own action; every other one moves the marker.
void msw::CComDrvVolleyGame::WraperActCallOffSpStart()
{
    CChrCore *pSp = GetLastSpChrCore();
    if (pSp == 0) {
        return;
    }
    if (pSp->GetChrTypeID() == 10) {
        ActCallSpKoopaOff();
    } else {
        ActCallSpOffMoveMarker();
    }
}

void msw::CComDrvVolleyGame::WraperActCallOffSpSub()
{
    ActCallGoHome();
}
