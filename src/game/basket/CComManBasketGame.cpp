// msw::CComManBasketGame: the computer players' manager of the normal
// basketball game. It keeps per-character flags about the ball holder
// (who is behind it, whom it has broken past) and per-team draws of
// whether a defender goes to the ball, and answers the computer drivers'
// questions about team members, the court and the stage gimmicks.
#include "game/msw/CComManBasketGame.hpp"

#include <math.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CComDrvBasketGame.hpp"
#include "game/msw/CComManBasket.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CItem.hpp"
#include "game/msw/CQPanel.hpp"
#include "game/msw/CStageBasket10.hpp"
#include "game/msw/CStageBasket15.hpp"
#include "game/msw/CStageBasket17.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgGmk_Trolley.hpp"

// 70 degrees in radians, unused (see CComManBasketHB.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

namespace {

    // Stages with gimmicks the computer players look at (the game work's
    // stage number).
    enum {
        STAGE_BEACH = 2,  // falling green shells
        STAGE_GHOST = 6,  // the ghost
        STAGE_PACKUN = 7, // the boss Packun's mud bombs
        STAGE_SHIP = 15,  // the cases
        STAGE_TRAIN = 17  // the train (trolley)
    };

    // The driver type of a computer player.
    const long DRIVER_COM = 3;

    // The character state of a bad condition, and the one GetState shows
    // during a dash (names guessed).
    const long STATE_BAD_CONDITION = 6;
    const long STATE_DASH = 0x34;

    // The "?" panel kind of the arrow panels.
    const long QPANEL_KIND_ARROW = 2;

    // Computer parameters read here (the CComMan::GetDat* ids).
    enum {
        DAT_SHOOT_ALLCOM_MAX = 0xA8,
        DAT_SHOOT_ALLCOM_MIN = 0xA9,
        DAT_SHOOT_MAX = 0xAA,
        DAT_SHOOT_MIN = 0xAB,
        DAT_SHOOT_TIME = 0x90,
        DAT_SHOOT_GAME_LV = 0x98,
        DAT_GODEF_PERCENT = 0x43A,
        DAT_GODEF_INTERVAL = 0x442,
        DAT_LV_VS_PLAYER = 0x4EA,
        DAT_LV_FIXED = 0x4F2,
        DAT_LV_UP_SCORE_DIFF = 0x4F3,
        DAT_LV_UP = 0x4FB,
        DAT_BREAKTHROUGH_ON = 0x938,
        DAT_BREAKTHROUGH_OFF = 0x940
    };

    inline void Swap(float &a, float &b)
    {
        float t = a;
        a = b;
        b = t;
    }

    inline float Max0(float v)
    {
        if (v < 0.0f) {
            v = 0.0f;
        }
        return v;
    }

    // An angle brought into (-pi, pi].
    inline float RoundRadPI(float fRad)
    {
        while (fRad > 6.2831855f) {
            fRad -= 6.2831855f;
        }
        while (fRad < 0.0f) {
            fRad += 6.2831855f;
        }
        if (fRad > 3.1415927f) {
            fRad -= 6.2831855f;
        }
        return fRad;
    }

    // Stage 17's train, while it runs.
    inline int IsTrainActive()
    {
        if (msw::CGame::g_GameWork.GetStage() != STAGE_TRAIN) {
            return 0;
        }
        const msw::CStgGmk_Trolley *pTrolley = msw::CStageBasket17::GetTrolley(0);
        if (pTrolley != 0) {
            return pTrolley->IsActive();
        }
        return 0;
    }

    inline float GetTrainRotY()
    {
        if (!IsTrainActive()) {
            return 0.0f;
        }
        const msw::CStgGmk_Trolley *pTrolley = msw::CStageBasket17::GetTrolley(0);
        if (pTrolley != 0) {
            return pTrolley->GetDefaultRotY();
        }
        return 0.0f;
    }

    // The corners of the train's body in the court's frame (the body
    // lengthened by 1.5 along X). Left alone when there is no train.
    inline void GetTrainRect(bcs::FVEC3 &rMin, bcs::FVEC3 &rMax)
    {
        if (!IsTrainActive()) {
            return;
        }
        const msw::CStgGmk_Trolley *pTrolley = msw::CStageBasket17::GetTrolley(0);
        if (pTrolley == 0) {
            return;
        }
        bcs::FVEC3 pos = pTrolley->GetPos();
        float fHalfZ = pTrolley->GetBodyDepth() / 2.0f;
        float fHalfX = (1.5f + pTrolley->GetBodyWidth()) / 2.0f;
        rMin = msw::CComMan::World2Local(bcs::FVEC3(pos.x - fHalfX, 0.0f, pos.z - fHalfZ), -1);
        rMax = msw::CComMan::World2Local(bcs::FVEC3(pos.x + fHalfX, 0.0f, pos.z + fHalfZ), -1);
    }
}

void msw::CComManBasketGame::CreateInstance()
{
    g_pInstance = new CComManBasketGame;
}

long msw::CComManBasketGame::GetMemBer(long nTeam, CChrCore **ppChr, long nMax, long nDrvType,
                                       const CChrCore *pIgnore)
{
    return GetMemBer(nTeam, const_cast<const CChrCore **>(ppChr), nMax, nDrvType, pIgnore);
}

long msw::CComManBasketGame::GetMemBer(long nTeam, const CChrCore **ppChr, long nMax, long nDrvType,
                                       const CChrCore *pIgnore)
{
    for (long i = 0; i < nMax; i++) {
        ppChr[i] = 0;
    }

    long nNum = 0;
    const CChrCore **ppOut = ppChr;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr == 0 || pChr->IsDisable()) {
            continue;
        }
        if (nTeam != pChr->GetTeamNo()) {
            continue;
        }
        if (pChr == pIgnore) {
            continue;
        }
        if (nDrvType > -1 && nDrvType != pChr->DriverGetType()) {
            continue;
        }
        nNum++;
        *ppOut++ = pChr;
        if (nNum == nMax) {
            break;
        }
    }
    return nNum;
}

long msw::CComManBasketGame::GetMemBer(long nTeam, MEMBER_INFO &rInfo, long nDrvType, const CChrCore *pIgnore)
{
    for (long i = 0; i < 6; i++) {
        rInfo.m_apChr[i] = 0;
        rInfo.m_aScore[i] = 0;
    }

    long nNum = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr == 0 || pChr->IsDisable()) {
            continue;
        }
        if (nTeam != pChr->GetTeamNo()) {
            continue;
        }
        if (pChr == pIgnore) {
            continue;
        }
        if (nDrvType > -1 && nDrvType != pChr->DriverGetType()) {
            continue;
        }
        rInfo.m_apChr[nNum] = pChr;
        nNum++;
        if (nNum == 6) {
            break;
        }
    }
    return nNum;
}

long msw::CComManBasketGame::GetMemberNoBadCondition(long nTeam, const CChrCore **ppChr, long nMax,
                                                     long nDrvType, const CChrCore *pIgnore)
{
    for (long i = 0; i < nMax; i++) {
        ppChr[i] = 0;
    }

    long nNum = 0;
    const CChrCore **ppOut = ppChr;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr == 0 || pChr->IsDisable()) {
            continue;
        }
        if (nTeam > -1 && nTeam != pChr->GetTeamNo()) {
            continue;
        }
        if (pChr == pIgnore) {
            continue;
        }
        if (nDrvType > -1 && nDrvType != pChr->DriverGetType()) {
            continue;
        }
        if (pChr->GetState() == STATE_BAD_CONDITION) {
            continue;
        }
        nNum++;
        *ppOut++ = pChr;
        if (nNum == nMax) {
            break;
        }
    }
    return nNum;
}

msw::CChrCore *msw::CComManBasketGame::GetUrgentGuardCore(long nTeam, long nDrvType, const CChrCore *pIgnore)
{
    const float DIST_MAX = 9999.9f;
    float fMinDist = DIST_MAX;
    CChrCore *pNear = 0;
    bcs::FVEC3 goalPos = World2Local(CGame::GetGoalPos(CGame::GetGuardGoalIdx(nTeam)), -1);

    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr == 0) {
            continue;
        }
        // The original tests pChr for null twice (a second beq on the same compare).
        if (pChr == 0 || pChr->IsDisable()) {
            continue;
        }
        if (nTeam != pChr->GetTeamNo()) {
            continue;
        }
        if (pChr == pIgnore) {
            continue;
        }
        if (nDrvType > -1 && nDrvType != pChr->DriverGetType()) {
            continue;
        }
        if (!pChr->CanbeJumpBlock()) {
            continue;
        }
        bcs::FVEC3 pos = GetChrPos(pChr);
        float fDist = CComManBasket::GetDistXZ(goalPos, pos);
        if (fDist < fMinDist) {
            fMinDist = fDist;
            pNear = pChr;
        }
    }
    return pNear;
}

msw::CComDrvBasketGame *msw::CComManBasketGame::GetComDrvBK(const CChrCore *pChr)
{
    if (pChr->DriverGetType() != DRIVER_COM) {
        return 0;
    }
    return static_cast<CComDrvBasketGame *>(pChr->DriverGetCurrent());
}

int msw::CComManBasketGame::InMoveEnableArea(const bcs::FVEC3 &rPos)
{
    float fLeft, fRight, fFront, fBack;
    CStageMan::GetInstance()->GetCourtRect(&fLeft, &fRight, &fFront, &fBack);

    if (IsEnableViewColl()) {
        float fWallZ = GetViewCollPosZ();
        switch (GetViewCollTargetDir()) {
            case 0:
                if (fWallZ < fBack) {
                    fBack = fWallZ;
                }
                break;
            case 1:
                if (fWallZ > fFront) {
                    fFront = fWallZ;
                }
                break;
        }
    }

    bcs::FVEC3 pos = Local2World(rPos, -1);
    return pos.x >= fLeft && pos.x <= fRight && pos.z >= fFront && pos.z <= fBack;
}

void msw::CComManBasketGame::NormalizeCanMovePosToInvisibleWall(bcs::FVEC3 &rPos)
{
    if (!IsEnableViewColl()) {
        return;
    }

    float fLeft, fRight, fFront, fBack;
    CStageMan::GetInstance()->GetCourtRect(&fLeft, &fRight, &fFront, &fBack);
    float fWallZ = GetViewCollPosZ();
    switch (GetViewCollTargetDir()) {
        case 0:
            if (fWallZ < fBack) {
                fBack = fWallZ;
            }
            break;
        case 1:
            if (fWallZ > fFront) {
                fFront = fWallZ;
            }
            break;
    }

    bcs::FVEC3 pos = Local2World(rPos, -1);
    pos.x = pos.x > fRight ? fRight : (pos.x < fLeft ? fLeft : pos.x);
    pos.z = pos.z > fBack ? fBack : (pos.z < fFront ? fFront : pos.z);
    rPos = World2Local(pos, -1);
}

long msw::CComManBasketGame::GetBaseShootPoint()
{
    // The time left in seconds (all but endless without a time limit).
    float fTime;
    if (CGame::g_GameWork.m_fUnk20 <= 0.0f) {
        fTime = 999.9f;
    } else {
        fTime = CGame::g_GameWork.m_fGameTime / 60.0f;
    }
    long nTime = fTime;
    float fShotTime = CGame::GetMaxShotTime() < 1.0f ? 30.0f : CGame::GetMaxShotTime() / 60.0f;

    if (!CChrManBasket::GetInstance()->IsNoShotClock()) {
        long nClock = CChrManBasket::GetInstance()->GetClockTimeSeconds_();
        if (nTime > nClock) {
            nTime = nClock;
        }
    }

    long nMax, nMin;
    if (CComManBasket::IsAllComTeam(CGame::g_GameWork.GetUserTeam())) {
        nMax = GetDatI(DAT_SHOOT_ALLCOM_MAX, 0L);
        nMin = GetDatI(DAT_SHOOT_ALLCOM_MIN, 0L);
    } else {
        nMax = GetDatI(DAT_SHOOT_MAX, 0L);
        nMin = GetDatI(DAT_SHOOT_MIN, 0L);
    }

    float fRate = nTime / fShotTime;
    fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
    return fRate * (nMax - nMin) + nMin;
}

float msw::CComManBasketGame::GetDunkFrameRate(const CChrCore *pChr)
{
    if (pChr == 0) {
        return 1.0f;
    }
    CComDrvBasketGame *pDrv = GetComDrvBK(pChr);
    if (pDrv != 0) {
        return pDrv->GetDunkFrameRate();
    }
    return 1.0f;
}

unsigned long msw::CComManBasketGame::GetNBeachNearFallGShell(const bcs::FVEC3 &rPos)
{
    if (CGame::g_GameWork.GetStage() != STAGE_BEACH) {
        return 0;
    }

    bcs::FVEC3 pos = Local2World(rPos, -1);
    CGimmickMan::GetInstance();
    float fMinDist = 9999.9f;
    CItem *pNear = 0;
    for (CItem *pItem = CGimmickMan::ItemGetFirst(); pItem != 0; pItem = CGimmickMan::ItemGetNext(pItem)) {
        if (pItem->IsTouchDown()) {
            continue;
        }
        if (!pItem->CanbePickup()) {
            continue;
        }
        float fDist = CComManBasket::GetDistXZ(pItem->GetPos(), pos);
        if (fDist > fMinDist) {
            continue;
        }
        fMinDist = fDist;
        pNear = pItem;
    }

    if (pNear == 0) {
        return 0;
    }
    return pNear->GetId();
}

int msw::CComManBasketGame::IsNBeachFallGSellDisable(unsigned long nId)
{
    if (CGame::g_GameWork.GetStage() != STAGE_BEACH) {
        return 1;
    }
    if (nId == 0) {
        return 1;
    }
    CItem *pItem = static_cast<CItem *>(bcs::CTaskMan::GetTask(nId));
    if (pItem != 0) {
        if (pItem->IsTouchDown()) {
            return 1;
        }
        if (!pItem->CanbePickup()) {
            return 1;
        }
        return 0;
    }
    return 1;
}

bcs::FVEC3 msw::CComManBasketGame::GetNBeachFallGShellPos(unsigned long nId)
{
    if (!IsNBeachFallGSellDisable(nId)) {
        CItem *pItem = static_cast<CItem *>(bcs::CTaskMan::GetTask(nId));
        if (pItem != 0) {
            return World2Local(pItem->GetPos(), -1);
        }
    }
    return bcs::FVEC3(0.0f, 0.0f, 0.0f);
}

msw::CNpcBossPackun::CMudBomb *msw::CComManBasketGame::GetMudPtr(unsigned long nId)
{
    return static_cast<CNpcBossPackun::CMudBomb *>(bcs::CTaskMan::GetTask(nId));
}

const msw::CNpcBossPackun::CMudBomb *msw::CComManBasketGame::GetMudFirst()
{
    if (CGame::g_GameWork.GetStage() != STAGE_PACKUN) {
        return 0;
    }
    CNpcBossPackun::CMudBombMan *pMan = CNpcBossPackun::CMudBombMan::GetInstance();
    if (pMan == 0) {
        return 0;
    }
    CNpcBossPackun::CMudBomb *pBomb = pMan->GetList().GetFirst();
    if (pBomb != 0) {
        return pBomb;
    }
    return 0;
}

const msw::CNpcBossPackun::CMudBomb *msw::CComManBasketGame::GetMudNext(const CNpcBossPackun::CMudBomb *pBomb)
{
    if (CGame::g_GameWork.GetStage() != STAGE_PACKUN) {
        return 0;
    }
    CNpcBossPackun::CMudBombMan *pMan = CNpcBossPackun::CMudBombMan::GetInstance();
    if (pMan == 0) {
        return 0;
    }
    if (pBomb == 0) {
        return 0;
    }
    return pMan->GetList().GetNext(const_cast<CNpcBossPackun::CMudBomb *>(pBomb));
}

namespace {

    // Not zero for an arrow panel.
    inline int IsArrowKind(const msw::CQPanel *pPanel)
    {
        return ((1 << pPanel->GetUnk88()) & (1 << QPANEL_KIND_ARROW)) != 0;
    }

    // Not zero for an arrow panel that can be touched.
    inline int IsArrowPanel(const msw::CQPanel *pPanel)
    {
        if (pPanel == 0) {
            return 0;
        }
        return !pPanel->CanAccess() ? 0 : IsArrowKind(pPanel);
    }
}

msw::CQPanel *msw::CComManBasketGame::ArrowPanel_GetFirst()
{
    for (CQPanel *pPanel = QPanel_GetFirst(); pPanel != 0; pPanel = QPanel_GetNext(pPanel)) {
        if (pPanel->CanAccess() && IsArrowKind(pPanel)) {
            return pPanel;
        }
    }
    return 0;
}

msw::CQPanel *msw::CComManBasketGame::ArrowPanel_GetNext(CQPanel *pPanel)
{
    if (pPanel == 0) {
        return 0;
    }
    for (CQPanel *pNext = QPanel_GetNext(pPanel); pNext != 0; pNext = QPanel_GetNext(pNext)) {
        if (pNext->CanAccess() && IsArrowKind(pNext)) {
            return pNext;
        }
    }
    return 0;
}

int msw::CComManBasketGame::IsTeamTargetDirArrowPanel(long nTeam, CQPanel *pPanel)
{
    if (!IsArrowPanel(pPanel)) {
        return 0;
    }
    // The arrow should point down the court towards the goal (0: +Z, 2: -Z).
    long nDir;
    if (CGame::GetGoalPos(CGame::GetTargetGoalIdx(nTeam)).z > 0.0f) {
        nDir = 0;
    } else {
        nDir = 2;
    }
    return pPanel->GetDir4() == nDir;
}

msw::CStage10Base::CPanelInfoBase *msw::CComManBasketGame::GetJStreetPanelRide()
{
    for (CStage10Base::CPanelInfoBase *pPanel = CStageBasket10::GetFirstPanel(); pPanel != 0;
         pPanel = CStageBasket10::GetNextPanel(pPanel)) {
        if (pPanel->GetState() == 1) {
            return pPanel;
        }
    }
    return 0;
}

msw::CStage10Base::CPanelInfoBase *msw::CComManBasketGame::GetJStreetPanelFirst()
{
    return CStageBasket10::GetFirstPanel();
}

msw::CStage10Base::CPanelInfoBase *msw::CComManBasketGame::GetJStreetPanelNext(
    const CStage10Base::CPanelInfoBase *pPanel)
{
    return CStageBasket10::GetNextPanel(pPanel);
}

int msw::CComManBasketGame::IsJStreetBonus()
{
    return CStageBasket10::IsNowFeverTime();
}

void msw::CComManBasketGame::MsgShootCatchToGhost()
{
    if (CGame::g_GameWork.GetStage() != STAGE_GHOST) {
        return;
    }
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        CComDrvBasketGame *pDrv = GetComDrvBK(pChr);
        if (pDrv != 0) {
            pDrv->MsgShootCatchToGhost();
        }
    }
}

long msw::CComManBasketGame::GetGmkCaseNum()
{
    if (CGame::g_GameWork.GetStage() != STAGE_SHIP) {
        return 0;
    }
    return CStageBasket15::GetCaseMaxNum();
}

const msw::CStgGmk_Case *msw::CComManBasketGame::GetGmkCase(long nIdx)
{
    if (CGame::g_GameWork.GetStage() != STAGE_SHIP) {
        return 0;
    }
    if (nIdx < 0 || nIdx >= GetGmkCaseNum()) {
        return 0;
    }
    return CStageBasket15::GetCase(nIdx);
}

const msw::CStgGmk_Case *msw::CComManBasketGame::GetGmkCasePtr(unsigned long nId)
{
    if (CGame::g_GameWork.GetStage() != STAGE_SHIP) {
        return 0;
    }
    return reinterpret_cast<const CStgGmk_Case *>(bcs::CTaskMan::GetTask(nId));
}

int msw::CComManBasketGame::HitCheckTrain(bcs::FVEC3 &rOut, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
{
    if (!IsTrainActive()) {
        return 0;
    }

    bcs::FVEC3 to = rTo;
    bcs::FVEC3 lo = rFrom;
    bcs::FVEC3 hi = rTo;
    if (lo.x > hi.x) {
        Swap(lo.x, hi.x);
    }
    if (lo.z > hi.z) {
        Swap(hi.z, lo.z);
    }

    bcs::FVEC3 trainMin, trainMax;
    GetTrainRect(trainMin, trainMax);
    if (trainMin.x <= hi.x && lo.x <= trainMax.x && trainMin.z <= hi.z && lo.z <= trainMax.z) {
        // Stop at the side of the train facing rFrom, short of the line it
        // runs on unless rFrom is already past that line.
        rOut.x = rFrom.x < 0.0f ? trainMin.x : trainMax.x;
        rOut.y = 0.0f;
        rOut.z = to.z;
        if (fabsf(RoundRadPI(GetTrainRotY())) > 1.5707964f) {
            if (trainMax.z < 5.0f) {
                if (trainMax.z < rFrom.z) {
                    rOut.z = rFrom.z;
                    rOut.x = to.x;
                } else {
                    rOut.z = trainMax.z;
                }
            }
        } else {
            if (trainMin.z > -5.0f) {
                if (trainMin.z > rFrom.z) {
                    rOut.z = rFrom.z;
                    rOut.x = to.x;
                } else {
                    rOut.z = trainMin.z;
                }
            }
        }
        return 1;
    }

    rOut = to;
    return 0;
}

int msw::CComManBasketGame::IsTrainCrossLine(const bcs::FVEC3 &rPos0, const bcs::FVEC3 &rPos1)
{
    if (!IsTrainActive()) {
        return 0;
    }

    bcs::FVEC3 trainMin, trainMax;
    GetTrainRect(trainMin, trainMax);
    float fCrossX, fCrossZ;
    if (bcs::BCS_GetLineToSegLineCross(&fCrossX, &fCrossZ, 0.0f, trainMin.z, 0.0f, trainMax.z, rPos0.x, rPos0.z,
                                       rPos1.x, rPos1.z)) {
        float fMinX = rPos0.x;
        float fMaxX = rPos1.x;
        if (fMinX > fMaxX) {
            float f = fMinX;
            fMinX = fMaxX;
            fMaxX = f;
        }
        return fCrossZ >= trainMin.z && fCrossZ <= trainMax.z && fCrossX >= fMinX && fCrossX <= fMaxX;
    }
    return 0;
}

msw::CComManBasketGame::~CComManBasketGame()
{
}

void msw::CComManBasketGame::Clear()
{
    m_nDefBreakthroughBits = 0;
    m_nOnBallerBackBits = 0;
    m_nDashToTrickBits = 0;
    for (long i = 0; i < 2; i++) {
        m_fGoDefTimer[i] = 0.0f;
        m_bGoDefOnBaller[i] = 0;
    }
}

void msw::CComManBasketGame::Main(float fFrame)
{
    if (CGame::g_GameWork.GetGameType() == CGame::GAME_TYPE_PARTY) {
        return;
    }

    m_bGoDefOnBaller[0] = 0;
    m_bGoDefOnBaller[1] = 0;

    const CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
    if (!(CGame::g_GameWork.m_nFlag18 & 1) && pHolder != 0) {
        // Who is behind the ball holder, and whom it has broken past, by
        // the distances to the goal it attacks (with some hysteresis).
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            const CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable()) {
                continue;
            }
            unsigned long nBit = 1 << pChr->GetPlayerNo();
            if (pHolder->GetTeamNo() == pChr->GetTeamNo()) {
                m_nDefBreakthroughBits &= ~nBit;
                m_nOnBallerBackBits &= ~nBit;
                continue;
            }

            const bcs::FVEC3 &rGoal = pHolder->GetTargetGoalPos();
            float fHolderDist = GetDisXZ(pHolder->GetPos(), rGoal);
            float fDist = GetDisXZ(pChr->GetPos(), rGoal);

            if (m_nOnBallerBackBits & nBit) {
                if (Max0(fHolderDist - 2.5f) >= fDist) {
                    m_nOnBallerBackBits &= ~nBit;
                }
            } else {
                if (Max0(fHolderDist) < fDist) {
                    m_nOnBallerBackBits |= nBit;
                }
            }

            if (m_nDefBreakthroughBits & nBit) {
                float fOff = GetDatF(DAT_BREAKTHROUGH_OFF, CGame::GetTeamComLevel(pChr->GetTeamNo()));
                if (Max0(fHolderDist - fOff) >= fDist) {
                    m_nDefBreakthroughBits &= ~nBit;
                }
            } else {
                float fOn = GetDatF(DAT_BREAKTHROUGH_ON, CGame::GetTeamComLevel(pChr->GetTeamNo()));
                if (Max0(fHolderDist + fOn) < fDist) {
                    m_nDefBreakthroughBits |= nBit;
                }
            }
        }

        // Every so often each team draws whether a defender goes to the
        // ball holder.
        for (long nTeam = 0; nTeam < 2; nTeam++) {
            m_fGoDefTimer[nTeam] -= fFrame;
            if (m_fGoDefTimer[nTeam] <= 0.0f) {
                float fLv = static_cast<CComManBasketGame *>(GetInstance())->GetComLvF_(nTeam);
                m_fGoDefTimer[nTeam] = GetDatI(DAT_GODEF_INTERVAL, fLv);
                long nPercent = GetDatI(DAT_GODEF_PERCENT, fLv);
                m_bGoDefOnBaller[nTeam] = bcs::BCS_RandMax(99L) < nPercent;
            }
        }
    } else {
        m_nDefBreakthroughBits = 0;
    }

    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr == 0 || pChr->IsDisable()) {
            continue;
        }
        if (pChr->GetState() != STATE_DASH) {
            m_nDashToTrickBits &= ~(1 << i);
        }
    }
}

float msw::CComManBasketGame::GetBaseComLvF_(long nTeam) const
{
    if (!CComManBasket::IsAllComTeam(nTeam)) {
        if (CGame::g_GameWork.m_Unk151 != 0) {
            return GetDatF(DAT_LV_FIXED, 0L);
        }
        return GetDatF(DAT_LV_VS_PLAYER, CGame::GetTeamComLevel(1 - nTeam));
    }
    return CGame::GetTeamComLevel(nTeam);
}

float msw::CComManBasketGame::GetComLvF_(long nTeam) const
{
    float fLv = GetBaseComLvF_(nTeam);
    // A computer team that trails by enough plays better.
    if (CComManBasket::IsAllComTeam(nTeam) && fLv >= 3.0f && fLv < 7.0f) {
        long nDiff = CGame::GetTeamScore(1 - nTeam) - CGame::GetTeamScore(nTeam);
        if (nDiff >= (long)GetDatF(DAT_LV_UP_SCORE_DIFF, fLv)) {
            fLv += GetDatF(DAT_LV_UP, fLv);
            fLv = fLv > 7.0f ? 7.0f : (fLv < 0.0f ? 0.0f : fLv);
        }
    }
    return fLv;
}

int msw::CComManBasketGame::IsThanOnBallerBack_(const CChrCore *pChr) const
{
    return (m_nOnBallerBackBits & (1 << pChr->GetPlayerNo())) != 0;
}

int msw::CComManBasketGame::IsDefBreakthrough_(const CChrCore *pChr) const
{
    return (m_nDefBreakthroughBits & (1 << pChr->GetPlayerNo())) != 0;
}

long msw::CComManBasketGame::CanbeActCallGoDefOnBaller_(long nTeam) const
{
    return m_bGoDefOnBaller[nTeam];
}

int msw::CComManBasketGame::CheckDashToTrickMoveActInter_(const CChrCore *pChr)
{
    unsigned long nBit = 1 << pChr->GetPlayerNo();
    if (!(m_nDashToTrickBits & nBit)) {
        m_nDashToTrickBits |= nBit;
        return 1;
    }
    return 0;
}

int msw::CComManBasketGame::CanbeShootState_(long nTeam) const
{
    float fLv = GetComLvF_(nTeam);
    float fTime = CGame::g_GameWork.m_fGameTime;
    float fShootTime = GetDatF(DAT_SHOOT_TIME, fLv);
    if (fTime > 0.0f && fTime < fShootTime) {
        return 1;
    }
    return CGameBasket::GetInstance()->m_fUnk1C0 > GetDatF(DAT_SHOOT_GAME_LV, fLv);
}
