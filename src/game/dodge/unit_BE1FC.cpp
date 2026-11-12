// msw::CComDrvDodgeGame: the stage gimmick helpers (stage 4's pylons, stage
// 7's mud bombs, stage 12's pinballs, stage 15's cases, stage 17's
// trolley). The dodgeball version of VolleyF's game/volley/unit_C0918.cpp;
// the file name is not known.
#include "bcs/BCS_ShakerSort.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CSimpleSortBase.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComDrvDodge.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CNpcBossPackun.hpp"
#include "game/msw/CStageDodge04.hpp"
#include "game/msw/CStageDodge12.hpp"
#include "game/msw/CStageDodge15.hpp"
#include "game/msw/CStageDodge17.hpp"
#include "game/msw/CStgGmk_Case.hpp"
#include "game/msw/CStgGmk_Pinball.hpp"
#include "game/msw/CStgGmk_Trolley.hpp"

namespace {
    typedef bcs::CSimpleSortBase<32> SortBase;
    typedef msw::CNpcBossPackun::CMudBomb MudBomb;

    // (guessed helper) rIn's horizontal direction brought to length fLen in
    // rOut, with rIn's height.
    inline void SetLenXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn, float fLen)
    {
        float y = rIn.y;
        bcs::BCS_Vec_Normalize(rOut, bcs::FVEC3(rIn.x, 0.0f, rIn.z));
        rOut.x *= fLen;
        rOut.z *= fLen;
        rOut.y = y;
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
}

// The nearest pylon in the way to rPos, or -1.
long msw::CComDrvDodgeGame::Stage04_GetPylonToTarget(const bcs::FVEC3 &rPos) const
{
    SortBase::SORT *pSort; // declared first: matches the original register choice
    if (CGame::g_GameWork.GetStage() != 4) {
        return -1;
    }

    SortBase::SORT aSort[32];
    long nNum = 0;
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 1.0f) {
        return -1;
    }

    pSort = aSort;
    for (long i = 0; i < CStageDodge04::GetPylonMaxNum(); i++) {
        const CStgGmk_Pylon *pPylon = CStageDodge04::GetPylon(i);
        if (pPylon == 0) {
            continue;
        }
        bcs::FVEC3 pos = pPylon->GetPos();
        float fRadius = pPylon->GetRadius() + m_pChr->GetCollSize_BodyPush();
        if (CComMan::IsPointInFaceXZ(pos, m_MyWorldPos, target, 0.7f * fRadius, 0.5f + fRadius)) {
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

// The position to go round pylon nPylon on the way to rPos (nDir: the side,
// as returned before; -1 for a new one).
long msw::CComDrvDodgeGame::Stage04_GetPosTurnAroundPylon(bcs::FVEC3 &rPos, long nPylon, long nDir) const
{
    if (CGame::g_GameWork.GetStage() != 4) {
        return -1;
    }
    const CStgGmk_Pylon *pPylon = CStageDodge04::GetPylon(nPylon);
    if (pPylon == 0) {
        return -1;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }

    bcs::FVEC3 pos = pPylon->GetPos();
    float fRange = 0.5f + (pPylon->GetRadius() + m_pChr->GetCollSize_BodyPush());
    if (nDir < 0) {
        bcs::FVEC3 local = World2Local(pos, -1);
        nDir = RunCorrect_GetFirstRotDir(rPos, local.x - pPylon->GetRadius(), local.x + pPylon->GetRadius());
    }
    return CComMan::GetPosTurnAroundGimmick(rPos, m_MyWorldPos, target, pos, fRange, nDir);
}

// When a pylon is in the way to rPos, a point beside it to make for first.
int msw::CComDrvDodgeGame::Stage04_GetViewPointPosFromPylon(bcs::FVEC3 &rPos) const
{
    if (CGame::g_GameWork.GetStage() != 4) {
        return 0;
    }
    long nPylon = Stage04_GetPylonToTarget(rPos);
    if (nPylon < 0) {
        return 0;
    }
    const CStgGmk_Pylon *pPylon = CStageDodge04::GetPylon(nPylon);
    if (pPylon == 0) {
        return 0;
    }

    bcs::FVEC3 target = Local2World(rPos, -1);
    bcs::FVEC3 pos = pPylon->GetPos();
    float fRange = 0.2f + (pPylon->GetRadius() + m_pChr->GetCollSize_BodyPush());
    bcs::FVEC3 view;
    if (CComMan::GetViewPointPosFromOthers(view, m_MyWorldPos, target, pos, fRange, 0.3f)) {
        rPos = view;
        return 1;
    }
    return 0;
}

// The nearest mud bomb (its task id) in the way to rPos, or 0.
unsigned long msw::CComDrvDodgeGame::Stage07_GetMudBombToTarget(const bcs::FVEC3 &rPos) const
{
    if (CGame::g_GameWork.GetStage() != 7) {
        return 0;
    }

    SortBase::SORT aSort[32];
    long nNum = 0;
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return 0;
    }
    CNpcBossPackun::CMudBombMan *pMan = CNpcBossPackun::CMudBombMan::GetInstance();
    if (pMan == 0) {
        return 0;
    }

    bcs::CList<MudBomb> &rList = pMan->GetList();
    MudBomb *pBomb = rList.GetFirst();
    SortBase::SORT *pSort = aSort;
    for (; pBomb != 0; pBomb = rList.GetNext(pBomb)) {
        if (!pBomb->IsNowExplode()) {
            continue;
        }
        bcs::FVEC3 pos = pBomb->GetPos();
        float fRadius = pBomb->GetAtkCollRadius() + m_pChr->GetCollSize_BodyDamage();
        if (CComMan::IsPointInFaceXZ(pos, m_MyWorldPos, target, 1.2f * fRadius, 0.7f + fRadius)) {
            float fDis = GetDisXZ(m_MyWorldPos, pos);
            pSort->m_nId = pBomb->GetId();
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
    return 0;
}

// The position to go round mud bomb nBomb (a task id) on the way to rPos
// (nDir: the side, as returned before; -1 for a new one).
long msw::CComDrvDodgeGame::Stage07_GetPosTurnAroundMudBomb(bcs::FVEC3 &rPos, unsigned long nBomb, long nDir) const
{
    if (CGame::g_GameWork.GetStage() != 7) {
        return -1;
    }
    const MudBomb *pBomb = static_cast<const MudBomb *>(bcs::CTaskMan::GetTask(nBomb));
    if (pBomb == 0) {
        return -1;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }

    bcs::FVEC3 pos = pBomb->GetPos();
    float fRange = 0.7f + (pBomb->GetAtkCollRadius() + m_pChr->GetCollSize_BodyDamage());
    if (nDir < 0) {
        bcs::FVEC3 local = World2Local(pos, -1);
        nDir = RunCorrect_GetFirstRotDir(rPos, local.x - pBomb->GetAtkCollRadius(),
                                         local.x + pBomb->GetAtkCollRadius());
    }
    return CComMan::GetPosTurnAroundGimmick(rPos, m_MyWorldPos, target, pos, fRange, nDir);
}

// When a mud bomb is in the way to rPos, a point beside it to make for
// first.
int msw::CComDrvDodgeGame::Stage07_GetViewPointPosFromMudBomb(bcs::FVEC3 &rPos) const
{
    if (CGame::g_GameWork.GetStage() != 7) {
        return 0;
    }
    const MudBomb *pBomb = static_cast<const MudBomb *>(bcs::CTaskMan::GetTask(Stage07_GetMudBombToTarget(rPos)));
    if (pBomb == 0) {
        return 0;
    }

    bcs::FVEC3 target = Local2World(rPos, -1);
    bcs::FVEC3 pos = pBomb->GetPos();
    float fRange = 0.2f + (pBomb->GetAtkCollRadius() + m_pChr->GetCollSize_BodyDamage());
    bcs::FVEC3 view;
    if (CComMan::GetViewPointPosFromOthers(view, m_MyWorldPos, target, pos, fRange, 0.3f)) {
        rPos = view;
        return 1;
    }
    return 0;
}

// The nearest pinball about to hit the character (fMargin more in front of
// it), or -1.
long msw::CComDrvDodgeGame::Stage12_GetPinballToHitMe(float fMargin) const
{
    if (CGame::g_GameWork.GetStage() != 12) {
        return -1;
    }

    SortBase::SORT aSort[32];
    SortBase::SORT *pSort = aSort;
    long nNum = 0;
    for (long i = 0; i < CStageDodge12::GetPinballMaxNum(); i++) {
        CStgGmk_Pinball *pPinball = CStageDodge12::GetPinball(i);
        if (pPinball == 0) {
            continue;
        }
        bcs::FVEC3 pos = pPinball->GetPos();
        bcs::FVEC3 vel = pPinball->GetVel();
        float fRange = 0.4f + (pPinball->GetRadius() + m_pChr->GetCollSize_BodyDamage());
        bcs::FVEC3 velXZ(vel.x, 0.0f, vel.z);
        float fSpeed = PSVECMag((const Vec *)&velXZ);
        if (CComMan::IsPointInDirFaceXZ(m_MyWorldPos, pos, vel, fRange, fMargin + (0.5f * fSpeed + fRange))) {
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

// A place away from pinball nPinball.
int msw::CComDrvDodgeGame::Stage12_GetEscPosFromPinball(bcs::FVEC3 &rPos, long nPinball) const
{
    if (CGame::g_GameWork.GetStage() != 12) {
        return 0;
    }
    CStgGmk_Pinball *pPinball = CStageDodge12::GetPinball(nPinball);
    if (pPinball == 0) {
        return 0;
    }

    bcs::FVEC3 pos = pPinball->GetPos();
    bcs::FVEC3 vel = pPinball->GetVel();
    float fRange = 0.4f + (pPinball->GetRadius() + m_pChr->GetCollSize_BodyDamage());
    fRange = 0.7f + (fRange + m_pChr->GetCollSize_BodyDamage());
    float fRot = NormalizeRot(RandRange(-0.31415927f, 0.31415927f));
    rPos = CComMan::GetEscPosFromGimmick(m_MyWorldPos, pos, vel, fRange, fRot);
    return 1;
}

// The nearest moving pinball in the way to rPos (fMargin more in front of
// it), or -1.
long msw::CComDrvDodgeGame::Stage12_GetPinballToTarget(const bcs::FVEC3 &rPos, float fMargin) const
{
    SortBase::SORT *pSort; // declared first: matches the original register choice
    if (CGame::g_GameWork.GetStage() != 12) {
        return -1;
    }

    SortBase::SORT aSort[32];
    long nNum = 0;
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 5.0f) < 0.5f) {
        return -1;
    }

    pSort = aSort;
    for (long i = 0; i < CStageDodge12::GetPinballMaxNum(); i++) {
        CStgGmk_Pinball *pPinball = CStageDodge12::GetPinball(i);
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
int msw::CComDrvDodgeGame::Stage12_GetSafePosFromPinball(bcs::FVEC3 &rPos, long nPinball) const
{
    if (CGame::g_GameWork.GetStage() != 12) {
        return 0;
    }
    CStgGmk_Pinball *pPinball = CStageDodge12::GetPinball(nPinball);
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
long msw::CComDrvDodgeGame::Stage15_GetCaseToTarget(const bcs::FVEC3 &rPos, float fMargin) const
{
    SortBase::SORT *pSort; // declared first: matches the original register choice
    if (CGame::g_GameWork.GetStage() != 15) {
        return -1;
    }

    SortBase::SORT aSort[32];
    long nNum = 0;
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }

    pSort = aSort;
    for (long i = 0; i < CStageDodge15::GetCaseMaxNum(); i++) {
        const CStgGmk_Case *pCase = CStageDodge15::GetCase(i);
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

// The position to go round case nCase on the way to rPos (nDir: the side, as
// returned before; -1 for a new one).
long msw::CComDrvDodgeGame::Stage15_GetPosTurnAroundCase(bcs::FVEC3 &rPos, long nCase, long nDir) const
{
    if (CGame::g_GameWork.GetStage() != 15) {
        return -1;
    }
    const CStgGmk_Case *pCase = CStageDodge15::GetCase(nCase);
    if (pCase == 0) {
        return -1;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }

    float fWidth = pCase->GetWidth();
    float fDepth = pCase->GetDepth();
    bcs::FVEC3 pos = pCase->GetPos();
    bcs::FVEC3 vel = pCase->GetVel();
    float fSize = fWidth;
    if (fDepth > fSize) {
        fSize = fDepth;
    }
    float fRange = 0.5f + (0.5f * fSize + m_pChr->GetCollSize_BodyPush());
    if (nDir < 0) {
        bcs::FVEC3 local = World2Local(pos, -1);
        nDir = RunCorrect_GetFirstRotDir(rPos, local.x - 0.5f * fWidth, local.x + 0.5f * fWidth);
    }
    return CComMan::GetPosTurnAroundGimmick(rPos, m_MyWorldPos, target, pos, fRange, nDir);
}

// The trolley about to hit the character, or -1.
long msw::CComDrvDodgeGame::Stage17_GetTrolleyToHitMe() const
{
    if (CGame::g_GameWork.GetStage() != 17) {
        return -1;
    }

    // How far to the side of the track the trolley hits.
    float fWidth = 2.0f;
    for (long i = 0; i < CStageDodge17::GetTrolleyMaxNum(); i++) {
        const CStgGmk_Trolley *pTrolley = CStageDodge17::GetTrolley(i);
        if (pTrolley == 0) {
            continue;
        }
        bcs::FVEC3 pos = pTrolley->GetPos();
        float fDepth = pTrolley->GetBodyDepth();
        bcs::FVEC3 vel = pTrolley->GetVel();
        if (CComMan::IsPointInDirFaceXZ(m_MyWorldPos, pos, vel, fWidth, 0.5f * fDepth + 20.0f)) {
            return i;
        }
    }
    return -1;
}

// A place away from trolley nTrolley: beside the track on the character's
// side.
int msw::CComDrvDodgeGame::Stage17_GetEscPosFromTrolley(bcs::FVEC3 &rPos, long nTrolley) const
{
    if (CGame::g_GameWork.GetStage() != 17) {
        return 0;
    }
    if (CStageDodge17::GetTrolley(nTrolley) == 0) {
        return 0;
    }

    rPos = m_MyPos;
    if (m_MyWorldPos.x >= 0.0f) {
        rPos.x = 3.0f;
    } else {
        rPos.x = -3.0f;
    }
    return 1;
}

// When the way to rPos crosses a trolley's track (the trolley's length and
// more in front of it), a point to make for first: beside the track on the
// character's side.
int msw::CComDrvDodgeGame::Stage17_GetViewPointPosFromTrolley(bcs::FVEC3 &rPos) const
{
    if (CGame::g_GameWork.GetStage() != 17) {
        return 0;
    }

    // How far from the middle of the court the track is taken to lie.
    float fSide = 2.0f;
    for (long i = 0; i < CStageDodge17::GetTrolleyMaxNum(); i++) {
        const CStgGmk_Trolley *pTrolley = CStageDodge17::GetTrolley(i);
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
        front.z += 0.5f * fDepth;
        back.z -= 0.5f * fDepth;
        if (vel.z > 0.0f) {
            front.z += 24.0f;
            back.z -= 2.0f;
        } else {
            front.z += 2.0f;
            back.z -= 24.0f;
        }

        if (m_MyWorldPos.x > fSide) {
            front.x += fSide;
            back.x += fSide;
        } else if (m_MyWorldPos.x < -fSide) {
            front.x -= fSide;
            back.x -= fSide;
        } else {
            return 0;
        }

        float x;
        float z;
        if (bcs::BCS_GetSegLineToSegLineCross(&x, &z, my.x, my.z, target.x, target.z, front.x, front.z, back.x,
                                              back.z)) {
            if (m_MyWorldPos.x >= 0.0f) {
                rPos.x = 3.0f;
            } else {
                rPos.x = -3.0f;
            }
            return 1;
        }
    }
    return 0;
}
