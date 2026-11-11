// msw::CComDrvVolleyGame: defending against a special shot of the other
// team (Daisy's, Bowser's and the Ninja's have actions of their own). A
// file of its own; the file name is not known.
#include "game/msw/CComDrvVolleyGame.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManVolley.hpp"
#include "game/msw/CComOSG.hpp"

namespace {
    // (names guessed) The work of action 0x21: against Daisy's shot, stand
    // at home or in an open space, or follow her target marker when it is
    // this character's to take (m_nMode 2).
    struct SP_DAISY_DEF_WORK : msw::CComDrv::ACT_WORK {
        bcs::FVEC3 m_Pos;      // 0x1C
        long m_nMode;          // 0x28: -1 none, 0 home, 1 open space, 2 follow the marker
    };

    // (names guessed) The work of action 0x23: run from where Bowser's fire
    // comes down, how far depending on m_nKind.
    struct SP_KOOPA_DEF_WORK : msw::CComDrv::ACT_WORK {
        long m_nKind;          // 0x1C
        long m_bFire;          // 0x20, set once the fire's marker is known
        bcs::FVEC3 m_EscPos;   // 0x24
        bcs::FVEC3 m_FirePos;  // 0x30
    };

    // (names guessed) The work of action 0x24: the Ninja's shot has three
    // markers; go to the one this character should take.
    struct SP_NINJA_DEF_WORK : msw::CComDrv::ACT_WORK {
        long m_bStart;         // 0x1C
        bcs::FVEC3 m_Pos;      // 0x20
        long m_nTarget;        // 0x2C, the marker followed, -1 for none
        float m_fCareTime;     // 0x30, how often to check the other markers
    };
}

void msw::CComDrvVolleyGame::ActLoopSpDaisyDef()
{
    SP_DAISY_DEF_WORK *pWork = static_cast<SP_DAISY_DEF_WORK *>(GetCurrentActWork());

    // The marker counts only when it is not near the middle of the court.
    bcs::FVEC3 marker;
    int bMarker = GetEmTargetMarkerPos(marker);
    if (bMarker) {
        bcs::FVEC3 center;
        center.x = GetMyCourtCenterX();
        center.y = 0.0f;
        center.z = GetMyCourtCenterZ();
        if (GetDisXZ(center, marker) < 4.5f) {
            bMarker = 0;
        }
    }

    if (pWork->m_nMode == 2) {
        if (!bMarker) {
            pWork->m_nMode = -1;
            pWork->m_nStep = 0;
        } else {
            pWork->m_Pos = marker;
        }
    } else if (bMarker && m_nMyChrIdx == GetShoudFollowTargetChrIdx(marker, 0)) {
        pWork->m_Pos = marker;
        pWork->m_nMode = 2;
        pWork->m_nStep = 0;
    }

    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~0x4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            if (pWork->m_nMode != 2) {
                pWork->m_nMode = 0;
                if (LotRate() < 20 && GetOpenSpaceInMyCourt(pWork->m_Pos, &m_MyPos, 0)) {
                    pWork->m_nMode = 1;
                }
                if (pWork->m_nMode == 0) {
                    GetHomePos(pWork->m_Pos, 1);
                }
            }
            pWork->m_nStep++;
            break;
        case 3: {
            long ret = MethdRunEx(pWork->m_Pos, 0.5f, 2.0f, RandRangeF(60.0f, 120.0f), 7, 10.0f);
            if (ret == 2 || ret == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        case 4:
            ActCallWait(RandRangeF(15.0f, 30.0f));
            pWork->m_nStep = 0;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void msw::CComDrvVolleyGame::ActLoopSpKoopaDef()
{
    SP_KOOPA_DEF_WORK *pWork = static_cast<SP_KOOPA_DEF_WORK *>(GetCurrentActWork());
    CChrCore *pSp = GetLastSpChrCore();
    if (pSp == 0) {
        ReturnAct(0);
        return;
    }

    if (!pSp->IsVolleyArtsKoopaShotFire() && CComManVolley::GetTargetMarkerPos(pSp, pWork->m_FirePos, 0)) {
        pWork->m_bFire = 1;
    }
    if (pSp->IsVolleyArtsKoopaFireEnd()) {
        if (ShoudTargetMarkerGo()) {
            Reset();
            ActCallTargetMarkerGo();
        } else {
            Reset();
            ActCallDefHome();
        }
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
            ActCallWait(RandRangeF(0.0f, 60.0f));
            pWork->m_nStep++;
            break;
        case 2:
            if (pWork->m_bFire && ActSubfSpKoopaDef_GetEscPos(pWork->m_EscPos, pWork->m_FirePos)) {
                pWork->m_fTime = -RandRangeF(30.0f, 60.0f);
                pWork->m_nStep++;
            }
            break;
        case 3:
            // Look for a new place now and then.
            if (pWork->m_fTime >= 0.0f) {
                if (!ActSubfSpKoopaDef_GetEscPos(pWork->m_EscPos, pWork->m_FirePos)) {
                    ReturnAct(1);
                    break;
                }
                pWork->m_fTime = -RandRangeF(30.0f, 60.0f);
            }
            MethdRunEx(pWork->m_EscPos, 0.4f, 1.0f, 600.0f, 7, 10.0f);
            break;
    }
}

// The open cell of a 4 by 4 grid nearest the character, with the cells
// around rFire filled, then a point that far from rFire in its direction.
int msw::CComDrvVolleyGame::ActSubfSpKoopaDef_GetEscPos(bcs::FVEC3 &rPos, const bcs::FVEC3 &rFire)
{
    SP_KOOPA_DEF_WORK *pWork = static_cast<SP_KOOPA_DEF_WORK *>(GetCurrentActWork());
    CComOSG grid;
    grid.Init(4, 4, m_nUnk98);
    grid.FillCircum(rFire, 1, -1);

    long x;
    long y;
    if (!grid.SearchValueNearPos(x, y, 0, m_MyPos)) {
        return 0;
    }
    bcs::FVEC3 pos;
    grid.GetPos(pos, x, y, 1);

    float fLen = 3.5f;
    if (pWork->m_nKind == 0) {
        fLen += RandRangeF(2.7f, 3.5f);
    } else if (pWork->m_nKind == 1) {
        fLen += RandRangeF(-0.5f, 0.5f);
    } else if (pWork->m_nKind == 2) {
        fLen += RandRangeF(8.5f, 9.0f);
    } else {
        fLen += RandRangeF(-0.5f, 8.0f);
    }

    bcs::FVEC3 dir;
    bcs::FVEC3Sub(&dir, &pos, &rFire);
    bcs::BCS_Vec_Normalize(rPos, dir);
    bcs::FVEC3Scale(&rPos, &rPos, fLen);
    bcs::FVEC3Add(&rPos, &rPos, &rFire);
    return 1;
}

void msw::CComDrvVolleyGame::ActLoopSpNinjaDef()
{
    SP_NINJA_DEF_WORK *pWork = static_cast<SP_NINJA_DEF_WORK *>(GetCurrentActWork());
    CChrCore *pSp = GetLastSpChrCore();
    if (pSp == 0) {
        ReturnAct(0);
        return;
    }

    if (pWork->m_bStart) {
        // Pick the marker to take now and then.
        if (pWork->m_fTime >= CComMan::GetDatF(0xB95, GetComLvF())) {
            int bChange = 0;
            if (pWork->m_nTarget < 0) {
                bChange = 1;
            }
            long nTarget = ActSubfSpNinjaDef_GetTargetIdx(bChange);
            if (nTarget < 0) {
                ReturnAct(0);
                return;
            }
            pWork->m_fTime = 0.0f;
            if (pWork->m_nTarget != nTarget) {
                pWork->m_nTarget = nTarget;
                pWork->m_fTime2 = 1000.0f;
            }
        }
        if (pWork->m_nTarget >= 0 && pWork->m_fTime2 >= pWork->m_fCareTime) {
            if (!CComManVolley::GetTargetMarkerPos(pSp, pWork->m_Pos, pWork->m_nTarget)) {
                ReturnAct(0);
                return;
            }
            ActSubfSpNinjaDef_CareTarget(pWork->m_Pos, pWork->m_nTarget);
            pWork->m_fTime2 = 0.0f;
        }
    }

    switch (pWork->m_nStep) {
        case 0: {
            bcs::FVEC3 marker;
            if (CComManVolley::GetTargetMarkerPos(pSp, marker, 0)) {
                pWork->m_nStep++;
            }
            break;
        }
        case 1:
            m_nFlag &= ~0x4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 2:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 3:
            pWork->m_bStart = 1;
            pWork->m_nStep++;
            break;
        case 4:
            if (pWork->m_nTarget >= 0) {
                MethdRunEx(pWork->m_Pos, 0.4f, 1.0f, 600.0f, 7, 10.0f);
            }
            break;
    }
}

// The marker the team's best placing gives this character; with bChange,
// sometimes the nearest of the other two instead.
long msw::CComDrvVolleyGame::ActSubfSpNinjaDef_GetTargetIdx(int bChange)
{
    if (m_nMateNum <= 0) {
        return -1;
    }
    CChrCore *pSp = GetLastSpChrCore();
    if (pSp == 0) {
        return -1;
    }
    if (pSp->GetVolleyTargetMarkerNum() != 3) {
        return -1;
    }

    bcs::FVEC3 aMy[3];
    aMy[0] = m_MyPos;
    aMy[1] = m_aMatePos[0];
    bcs::FVEC3 aTarget[3];
    for (long i = 0; i < 3; i++) {
        aTarget[i].Set(0.0f, 0.0f, 0.0f);
        if (!CComManVolley::GetTargetMarkerPos(pSp, aTarget[i], i)) {
            return -1;
        }
    }

    long nBest;
    if (m_nMateNum == 1) {
        nBest = CComMan::GetNearBestPos2on3(aMy, aTarget, 0);
    } else {
        aMy[2] = m_aMatePos[1];
        nBest = CComMan::GetNearBestPosBaseEx(aMy, aTarget, 3, 3, 0);
    }
    if (nBest < 0) {
        return -1;
    }

    if (bChange) {
        long nRate = CComMan::GetDatI(0xB7D, GetComLvF());
        float fNear = CComMan::GetDatF(0xB85, GetComLvF());
        long nOther;
        float fDist;
        if (nBest == 0) {
            fDist = GetDisXZ(m_MyPos, aTarget[1]);
            float fDist2 = GetDisXZ(m_MyPos, aTarget[2]);
            if (fDist < fDist2) {
                nOther = 1;
            } else {
                fDist = fDist2;
                nOther = 2;
            }
        } else if (nBest == 1) {
            fDist = GetDisXZ(m_MyPos, aTarget[0]);
            float fDist2 = GetDisXZ(m_MyPos, aTarget[2]);
            if (fDist < fDist2) {
                nOther = 0;
            } else {
                fDist = fDist2;
                nOther = 2;
            }
        } else {
            fDist = GetDisXZ(m_MyPos, aTarget[0]);
            float fDist2 = GetDisXZ(m_MyPos, aTarget[1]);
            if (fDist < fDist2) {
                nOther = 0;
            } else {
                nOther = 1;
                fDist = fDist2;
            }
        }
        if (fDist < fNear && LotRate() < nRate) {
            nBest = nOther;
        }
    }
    return nBest;
}

// With one team mate, when this character takes marker nTarget, stand
// towards the marker neither of the two takes.
int msw::CComDrvVolleyGame::ActSubfSpNinjaDef_CareTarget(bcs::FVEC3 &rPos, long nTarget)
{
    if (m_nMateNum != 1) {
        return 0;
    }
    CChrCore *pSp = GetLastSpChrCore();
    if (pSp == 0) {
        return 0;
    }
    if (pSp->GetVolleyTargetMarkerNum() != 3) {
        return 0;
    }

    bcs::FVEC3 aMy[3];
    aMy[0] = m_MyPos;
    aMy[1] = m_aMatePos[0];
    bcs::FVEC3 aTarget[3];
    for (long i = 0; i < 3; i++) {
        aTarget[i].Set(0.0f, 0.0f, 0.0f);
        if (!CComManVolley::GetTargetMarkerPos(pSp, aTarget[i], i)) {
            return 0;
        }
    }

    long nMine = CComMan::GetNearBestPos2on3(aMy, aTarget, 0);
    if (nMine != nTarget) {
        return 0;
    }
    long nMate = CComMan::GetNearBestPos2on3(aMy, aTarget, 1);
    long nFree = -1;
    for (long i = 0; i < 3; i++) {
        if (i != nMine && i != nMate) {
            nFree = i;
            break;
        }
    }

    float fLen = CComMan::GetDatF(0xB8D, GetComLvF());
    bcs::FVEC3 &rFree = aTarget[nFree];
    float fHalf = 0.35f * GetDisXZ(rPos, rFree);
    if (fHalf < fLen) {
        fLen = fHalf;
    }

    bcs::FVEC3 dir;
    bcs::FVEC3 diff;
    bcs::FVEC3Sub(&diff, &rFree, &rPos);
    bcs::BCS_Vec_Normalize(dir, diff);
    dir.x *= fLen;
    dir.z *= fLen;
    rPos.x += dir.x;
    rPos.z += dir.z;
    return 1;
}

// Starts the defence against the special shot just played.
void msw::CComDrvVolleyGame::WraperActCallDefSpStart()
{
    CChrCore *pSp = GetLastSpChrCore();
    if (pSp == 0) {
        return;
    }

    long nChr = pSp->GetChrTypeID();
    if (nChr == 3 || nChr == 5) {
        SP_DAISY_DEF_WORK *pWork = static_cast<SP_DAISY_DEF_WORK *>(EntryAct(0x21));
        pWork->m_nMode = -1;
    } else if (nChr == 10) {
        SP_KOOPA_DEF_WORK *pWork = static_cast<SP_KOOPA_DEF_WORK *>(EntryAct(0x23));
        long aRate[3];
        aRate[0] = CComMan::GetDatI(0x955, GetComLvF());
        aRate[1] = CComMan::GetDatI(0x95D, GetComLvF());
        aRate[2] = CComMan::GetDatI(0x965, GetComLvF());
        pWork->m_nKind = LotProbability(aRate, 3);
        pWork->m_bFire = 0;
    } else if (nChr == 14) {
        SP_NINJA_DEF_WORK *pWork = static_cast<SP_NINJA_DEF_WORK *>(EntryAct(0x24));
        pWork->m_fTime = 1000.0f;
        pWork->m_fTime2 = 1000.0f;
        pWork->m_bStart = 0;
        pWork->m_nTarget = -1;
        float fMin = CComMan::GetDatF(0xB9D, GetComLvF());
        pWork->m_fCareTime = RandRangeF(fMin, CComMan::GetDatF(0xBA5, GetComLvF()));
    } else {
        ActCallDefHome();
    }
}
