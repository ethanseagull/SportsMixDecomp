// msw::CComDrvVolleyGame: the stage gimmick helpers (stage 10's panels,
// stage 12's pinballs, stage 15's cases, stage 17's trolley) and the data
// of the special shots' target marker. A file of its own; the file name is
// not known.
#include "game/msw/CComDrvVolleyGame.hpp"
#include "bcs/BCS_ShakerSort.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CSimpleSortBase.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComOSG.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageVolley10.hpp"
#include "game/msw/CStageVolley12.hpp"
#include "game/msw/CStageVolley15.hpp"
#include "game/msw/CStageVolley17.hpp"
#include "game/msw/CStgGmk_Case.hpp"
#include "game/msw/CStgGmk_Pinball.hpp"
#include "game/msw/CStgGmk_Trolley.hpp"

namespace {
    typedef bcs::CSimpleSortBase<32> SortBase;

    // (guessed helper) Whether a panel type of GetPanelType_CR is the
    // attacking team's own colour (plain or doubled).
    inline int IsMyPanelType(long nType)
    {
        return static_cast<unsigned long>(nType - 2) <= 1;
    }

    // (guessed helpers) The horizontal direction from rFrom to rTo, length 1;
    // and rIn's horizontal direction brought to length fLen in rOut, with
    // rIn's height.
    inline void DirXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        bcs::FVEC3 diff;
        diff.x = rTo.x - rFrom.x;
        diff.z = rTo.z - rFrom.z;
        bcs::BCS_Vec_Normalize(rOut, bcs::FVEC3(diff.x, 0.0f, diff.z));
    }

    inline void SetLenXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn, float fLen)
    {
        float y = rIn.y;
        bcs::BCS_Vec_Normalize(rOut, bcs::FVEC3(rIn.x, 0.0f, rIn.z));
        rOut.x *= fLen;
        rOut.z *= fLen;
        rOut.y = y;
    }

    // (guessed helper) The centre of panel (col, row) of court nCourt, on
    // the ground, in rDrv's court frame.
    inline bcs::FVEC3 GetPanelLocalPos(const msw::CComDrv &rDrv, long nCourt, long col, long row)
    {
        bcs::FVEC3 pos = msw::CStageVolley10::GetPanelPos_CR(nCourt, col, row);
        pos.y = 0.0f;
        return rDrv.World2Local(pos, -1);
    }

    // Stand-ins for unknown users of 0 and of 0.4, 0.7, +-pi/10 and 2 pi,
    // which the original pools in .rodata ahead of the constants of the
    // functions here: 0 first of all, the others between stage 10's and
    // stage 12's. DodgeF's copy of this file has two more stage 12 functions
    // there (Stage12_GetPinballToHitMe, Stage12_GetEscPosFromPinball) that
    // use them, so VolleyF most likely had them too and its linker stripped
    // them. See notes/decisions.md, "Unreferenced data kept by dead
    // references".
    inline void UnusedReferences0(float f)
    {
        if (f == 0.0f) {
        }
    }

    inline void UnusedReferences12(float f)
    {
        if (f == 0.4f) {
        }
        if (f == 0.7f) {
        }
        if (f == -0.31415927f) {
        }
        if (f == 0.31415927f) {
        }
        if (f == 6.2831855f) {
        }
    }

    // (guessed helper) fRot brought into 0..2 pi.
    inline float NormalizeRot(float fRot)
    {
        while (fRot > 6.2831855f) {
            fRot -= 6.2831855f;
        }
        while (fRot < 0.0f) {
            fRot += 6.2831855f;
        }
        return fRot;
    }

    // (names guessed) The level table entries of a special shot's target
    // marker, per character: how long and how far to move it (the minimum;
    // the maximum is the entry 8 further on) and the odds of the seven
    // places to move it to (every 8 entries); early in the shot and later.
    long s_aSpMarkerMoveDatEarly[25] = {
        0x30D, 0x3BD, 0x46D, 0x51D, 0x5CD, 0x67D, 0x72D, 0x30D, 0x7DD, 0x30D, 0x88D, 0x96D, 0xA1D,
        0x30D, 0xACD, 0x30D, 0x30D, 0x30D, 0x30D, 0x30D, 0x30D, 0x30D, 0x30D, 0x30D, 0x30D,
    };
    long s_aSpMarkerKindDatEarly[25] = {
        0x32D, 0x3DD, 0x48D, 0x53D, 0x5ED, 0x69D, 0x74D, 0x32D, 0x7FD, 0x32D, 0x8AD, 0x98D, 0xA3D,
        0x32D, 0xAED, 0x32D, 0x32D, 0x32D, 0x32D, 0x32D, 0x32D, 0x32D, 0x32D, 0x32D, 0x32D,
    };
    long s_aSpMarkerMoveDat[25] = {
        0x365, 0x415, 0x4C5, 0x575, 0x625, 0x6D5, 0x785, 0x365, 0x835, 0x365, 0x8E5, 0x9C5, 0xA75,
        0x365, 0xB25, 0x365, 0x365, 0x365, 0x365, 0x365, 0x365, 0x365, 0x365, 0x365, 0x365,
    };
    long s_aSpMarkerKindDat[25] = {
        0x385, 0x435, 0x4E5, 0x595, 0x645, 0x6F5, 0x7A5, 0x385, 0x855, 0x385, 0x905, 0x9E5, 0xA95,
        0x385, 0xB45, 0x385, 0x385, 0x385, 0x385, 0x385, 0x385, 0x385, 0x385, 0x385, 0x385,
    };
}

// Whether taking panel (col, row) would complete a line of the driver's
// colour: the other two of its row or of its column are its own already.
int msw::CComDrvVolleyGame::Stage10_IsMyBingoIfGet(long col, long row) const
{
    if (IsMyPanelType(CStageVolley10::GetPanelType_CR(m_nCourtIdx, col, row))) {
        return 0;
    }

    int bBingo = 1;
    for (long i = 0; i < 3; i++) {
        if (i != col && IsMyPanelType(CStageVolley10::GetPanelType_CR(m_nCourtIdx, i, row)) == 0) {
            bBingo = 0;
            break;
        }
    }
    if (bBingo) {
        return 1;
    }

    bBingo = 1;
    for (long i = 0; i < 3; i++) {
        if (i != row && IsMyPanelType(CStageVolley10::GetPanelType_CR(m_nCourtIdx, col, i)) == 0) {
            bBingo = 0;
            break;
        }
    }
    if (bBingo) {
        return 1;
    }
    return 0;
}

// A place on a good panel: one that completes a line, else one not of the
// driver's colour yet; inside the panel, near its centre.
int msw::CComDrvVolleyGame::Stage10_GetGoodPos(bcs::FVEC3 &rPos, const CComOSG &rGrid, long nValue,
                                              const bcs::FVEC3 *pFrom) const
{
    UnusedReferences0(rPos.x);
    if (CGame::g_GameWork.GetStage() != 10) {
        return 0;
    }

    bcs::FVEC3 panel;
    bcs::FVEC3 pos;
    float fDepth = 0.5f * CStageVolley10::GetPanelDepth();
    float fWidth = 0.5f * CStageVolley10::GetPanelWidth();
    if (fDepth < fWidth) {
        fWidth = fDepth;
    }
    float fRange = fWidth - 0.1f;
    bcs::FVEC3 dir(0.0f, 0.0f, 0.0f);

    for (long i = 0; i < 2; i++) {
        if (Stage10_SearchGoodPos(panel, pos, rGrid, nValue, pFrom, i)) {
            float fDis = GetDisXZ(panel, pos);
            rPos = pos;
            if (fDis > fRange) {
                DirXZ(dir, panel, pos);
                SetLenXZ(dir, dir, fRange);
                rPos.x = panel.x + dir.x;
                rPos.z = panel.z + dir.z;
            }
            rPos.y = 0.0f;
            return 1;
        }
    }
    return 0;
}

// The panel (rPanel, its centre) and the free grid place on it (rPos)
// nearest to pFrom (or to the panel's centre), among the panels that
// complete a line (nMode 0) or are not of the driver's colour (nMode 1).
int msw::CComDrvVolleyGame::Stage10_SearchGoodPos(bcs::FVEC3 &rPanel, bcs::FVEC3 &rPos, const CComOSG &rGrid,
                                                 long nValue, const bcs::FVEC3 *pFrom, long nMode) const
{
    float fBest = -1.0f;
    long nNum = 0;
    for (long row = 0; row < 3; row++) {
        for (long col = 0; col < 3; col++) {
            int bGood = 0;
            switch (nMode) {
                case 0:
                    bGood = Stage10_IsMyBingoIfGet(col, row);
                    break;
                case 1:
                    bGood = !IsMyPanelType(CStageVolley10::GetPanelType_CR(m_nCourtIdx, col, row));
                    break;
            }

            long x = 0;
            long y = 0;
            if (bGood) {
                bcs::FVEC3 pos;
                bcs::FVEC3 panel = GetPanelLocalPos(*this, m_nCourtIdx, col, row);
                if (rGrid.SearchValueNearPos(x, y, nValue, panel)) {
                    rGrid.GetPos(pos, x, y, 0);
                    float fDis = GetDisXZ(pos, panel);
                    float fCmp = fDis;
                    if (pFrom != 0) {
                        fCmp = GetDisXZ(pos, *pFrom);
                    }
                    if (fDis < 3.0f && fCmp < 5.0f && (fBest < 0.0f || fCmp < fBest)) {
                        fBest = fCmp;
                        rPanel = panel;
                        nNum++;
                        rPos = pos;
                    }
                }
            }
        }
    }
    return nNum > 0;
}

// The nearest moving pinball in the way to rPos (fMargin more in front of
// it), or -1.
long msw::CComDrvVolleyGame::Stage12_GetPinballToTarget(const bcs::FVEC3 &rPos, float fMargin) const
{
    UnusedReferences12(fMargin);
    if (CGame::g_GameWork.GetStage() != 12) {
        return -1;
    }

    SortBase::SORT aSort[32];
    long nNum = 0;
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 5.0f) < 0.5f) {
        return -1;
    }

    SortBase::SORT *pSort = aSort;
    for (long i = 0; i < CStageVolley12::GetPinballMaxNum(); i++) {
        CStgGmk_Pinball *pPinball = CStageVolley12::GetPinball(i);
        if (pPinball == 0) {
            continue;
        }
        bcs::FVEC3 dir;
        bcs::FVEC3 pos = pPinball->GetPos();
        bcs::FVEC3 vel = pPinball->GetVel();
        if (PSVECMag((const Vec *)&vel) <= 0.001f) {
            continue;
        }

        float fRadius = pPinball->GetRadius() + 0.5f * m_pChr->GetCollSize_BodyDamage();
        float fRange = fRadius + 0.5f * m_pChr->GetCollSize_BodyDamage();
        bcs::FVEC3 from;
        bcs::FVEC3 to;
        SetLenXZ(dir, vel, fRadius);
        from.x = pos.x - dir.x;
        from.z = pos.z - dir.z;
        SetLenXZ(dir, vel, fRadius + fMargin);
        to.x = pos.x + dir.x;
        to.z = pos.z + dir.z;
        if (CComMan::IsHitGimmickToTarget(m_MyWorldPos, target, from, to, fRange)) {
            float fDis = GetDisXZ(m_MyWorldPos, pos);
            pSort->m_nId = i;
            nNum++;
            pSort->m_fParam = fDis;
            pSort++;
            if (nNum >= 32) {
                break;
            }
        }
    }

    if (nNum > 0) {
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        return aSort[0].m_nId;
    }
    return -1;
}

// A place on the way to rPos away from pinball nPinball.
int msw::CComDrvVolleyGame::Stage12_GetSafePosFromPinball(bcs::FVEC3 &rPos, long nPinball) const
{
    if (CGame::g_GameWork.GetStage() != 12) {
        return 0;
    }
    CStgGmk_Pinball *pPinball = CStageVolley12::GetPinball(nPinball);
    if (pPinball == 0) {
        return 0;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 5.0f) < 0.5f) {
        return 0;
    }

    bcs::FVEC3 pos = pPinball->GetPos();
    bcs::FVEC3 vel = pPinball->GetVel();
    float fRange = pPinball->GetRadius() + 0.5f * m_pChr->GetCollSize_BodyDamage();
    fRange = 0.5f + (fRange + 0.5f * m_pChr->GetCollSize_BodyDamage());
    float fRot = NormalizeRot(RandRange(-0.31415927f, 0.31415927f));
    rPos = CComMan::GetSafePosFromGimmick(m_MyWorldPos, target, pos, vel, fRange, fRot);
    return 1;
}

// The nearest case in the way to rPos (fMargin more in front of a moving
// one), or -1.
long msw::CComDrvVolleyGame::Stage15_GetCaseToTarget(const bcs::FVEC3 &rPos, float fMargin) const
{
    if (CGame::g_GameWork.GetStage() != 15) {
        return -1;
    }

    SortBase::SORT aSort[32];
    long nNum = 0;
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }

    SortBase::SORT *pSort = aSort;
    for (long i = 0; i < CStageVolley15::GetCaseMaxNum(); i++) {
        const CStgGmk_Case *pCase = CStageVolley15::GetCase(i);
        if (pCase == 0) {
            continue;
        }
        float fWidth = pCase->GetWidth();
        float fDepth = pCase->GetDepth();
        bcs::FVEC3 dir;
        bcs::FVEC3 pos = pCase->GetPos();
        bcs::FVEC3 vel = pCase->GetVel();
        bcs::FVEC3 from = pos;
        bcs::FVEC3 to = pos;
        if (PSVECMag((const Vec *)&vel) <= 0.001f) {
            fWidth = 0.5f * fWidth + 0.5f * m_pChr->GetCollSize_BodyPush();
            fDepth = 0.5f * fDepth + 0.5f * m_pChr->GetCollSize_BodyPush();
            from.x -= fWidth;
            to.x += fWidth;
        } else {
            fWidth = 0.5f * fWidth + 0.5f * m_pChr->GetCollSize_BodyPush();
            fDepth = 0.5f * fDepth + 0.5f * m_pChr->GetCollSize_BodyPush();
            SetLenXZ(dir, vel, fWidth);
            from.x = pos.x - dir.x;
            from.z = pos.z - dir.z;
            SetLenXZ(dir, vel, fWidth + fMargin);
            to.x = pos.x + dir.x;
            to.z = pos.z + dir.z;
        }
        if (CComMan::IsHitGimmickToTarget(m_MyWorldPos, target, from, to, fDepth)) {
            float fDis = GetDisXZ(m_MyWorldPos, pos);
            pSort->m_nId = i;
            nNum++;
            pSort->m_fParam = fDis;
            pSort++;
            if (nNum >= 32) {
                break;
            }
        }
    }

    if (nNum > 0) {
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        return aSort[0].m_nId;
    }
    return -1;
}

// A place on the way to rPos away from case nCase.
int msw::CComDrvVolleyGame::Stage15_GetSafePosFromCase(bcs::FVEC3 &rPos, long nCase) const
{
    if (CGame::g_GameWork.GetStage() != 15) {
        return 0;
    }
    const CStgGmk_Case *pCase = CStageVolley15::GetCase(nCase);
    if (pCase == 0) {
        return 0;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return 0;
    }

    float fSize = pCase->GetWidth();
    float fDepth = pCase->GetDepth();
    bcs::FVEC3 pos = pCase->GetPos();
    bcs::FVEC3 vel = pCase->GetVel();
    if (fDepth > fSize) {
        fSize = fDepth;
    }
    float fRange = 0.5f * fSize + 0.5f * m_pChr->GetCollSize_BodyPush();
    fRange = 0.3f + (fRange + 0.5f * m_pChr->GetCollSize_BodyPush());
    float fRot = NormalizeRot(RandRange(-0.31415927f, 0.31415927f));
    rPos = CComMan::GetSafePosFromGimmick(m_MyWorldPos, target, pos, vel, fRange, fRot);
    return 1;
}

// The position to go round case nCase on the way to rPos (nDir: the side, as
// returned before; -1 for a new one).
long msw::CComDrvVolleyGame::Stage15_GetPosTurnAroundCase(bcs::FVEC3 &rPos, long nCase, long nDir) const
{
    if (CGame::g_GameWork.GetStage() != 15) {
        return -1;
    }
    const CStgGmk_Case *pCase = CStageVolley15::GetCase(nCase);
    if (pCase == 0) {
        return -1;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }

    float fSize = pCase->GetWidth();
    float fDepth = pCase->GetDepth();
    bcs::FVEC3 pos = pCase->GetPos();
    bcs::FVEC3 vel = pCase->GetVel();
    if (fDepth > fSize) {
        fSize = fDepth;
    }
    float fRange = 0.5f + (0.5f * fSize + m_pChr->GetCollSize_BodyPush());
    return CComMan::GetPosTurnAroundGimmick(rPos, m_MyWorldPos, target, pos, fRange, nDir);
}

// When the way to rPos crosses a trolley's track (the trolley's length and
// more in front of it), a point to make for first: beside the track on the
// driver's side.
int msw::CComDrvVolleyGame::Stage17_GetViewPointPosFromTrolley(bcs::FVEC3 &rPos) const
{
    if (CGame::g_GameWork.GetStage() != 17) {
        return 0;
    }

    // How far from the middle of the court the track is taken to lie.
    float fSide = 2.5f;
    for (long i = 0; i < CStageVolley17::GetTrolleyMaxNum(); i++) {
        const CStgGmk_Trolley *pTrolley = CStageVolley17::GetTrolley(i);
        if (pTrolley == 0) {
            continue;
        }
        float fDepth = pTrolley->GetBodyDepth();
        bcs::FVEC3 vel = pTrolley->GetVel();
        bcs::FVEC3 pos = pTrolley->GetPos();
        bcs::FVEC3 my = m_MyWorldPos;
        bcs::FVEC3 target = Local2World(rPos, -1);
        bcs::FVEC3 front = pos;
        bcs::FVEC3 back = pos;
        front.x += 0.5f * fDepth;
        back.x -= 0.5f * fDepth;
        if (vel.x > 0.0f) {
            front.x += 24.0f;
            back.x -= 2.0f;
        } else {
            front.x += 2.0f;
            back.x -= 24.0f;
        }

        if (m_MyWorldPos.z > fSide) {
            front.z += fSide;
            back.z += fSide;
        } else if (m_MyWorldPos.z < -fSide) {
            front.z -= fSide;
            back.z -= fSide;
        } else {
            return 0;
        }

        float x;
        float z;
        if (bcs::BCS_GetSegLineToSegLineCross(&x, &z, my.x, my.z, target.x, target.z, front.x, front.z, back.x,
                                              back.z)) {
            if (m_MyWorldPos.z >= 0.0f) {
                rPos.z = 3.0f;
            } else {
                rPos.z = -3.0f;
            }
            return 1;
        }
    }
    return 0;
}

// How long (*pTime) and how far (*pDist) to move a special shot's target
// marker, from the level tables of the driver's character (bEarly: early in
// the shot), and where to (GetMoveTargetMarkerPos's kind; 2 by default).
long msw::CComDrvVolleyGame::GetDatSpTargetMarker(float *pTime, float *pDist, int bEarly)
{
    long nChr = m_pChr->GetChrTypeID();
    long nMoveId;
    long nKindId;
    if (bEarly) {
        nMoveId = s_aSpMarkerMoveDatEarly[nChr];
        nKindId = s_aSpMarkerKindDatEarly[nChr];
    } else {
        nMoveId = s_aSpMarkerMoveDat[nChr];
        nKindId = s_aSpMarkerKindDat[nChr];
    }

    float fMin = CComMan::GetDatF(nMoveId, GetComLvF());
    float fMax = CComMan::GetDatF(nMoveId + 8, GetComLvF());
    if (pTime != 0) {
        *pTime = RandRangeF(fMin, fMax);
    }
    fMin = CComMan::GetDatF(nMoveId + 0x10, GetComLvF());
    fMax = CComMan::GetDatF(nMoveId + 0x18, GetComLvF());
    if (pDist != 0) {
        *pDist = RandRangeF(fMin, fMax);
    }

    long aRate[7];
    aRate[0] = CComMan::GetDatI(nKindId, GetComLvF());
    aRate[1] = CComMan::GetDatI(nKindId + 0x08, GetComLvF());
    aRate[2] = CComMan::GetDatI(nKindId + 0x10, GetComLvF());
    aRate[3] = CComMan::GetDatI(nKindId + 0x18, GetComLvF());
    aRate[4] = CComMan::GetDatI(nKindId + 0x20, GetComLvF());
    aRate[5] = CComMan::GetDatI(nKindId + 0x28, GetComLvF());
    aRate[6] = CComMan::GetDatI(nKindId + 0x30, GetComLvF());
    if (IsExistPadDrv()) {
        aRate[0] = 0;
    }
    long nKind = LotProbability(aRate, 7);
    if (nKind < 0) {
        nKind = 2;
    }
    return nKind;
}
