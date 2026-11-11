// msw::CComManHockeyGame: what the computer players of the hockey game
// look at: characters in the way, the ball's direction, whom to mark, the
// nearest "?" panel or green shell, and the score panels of stage 10.
#include "game/msw/CComManHockeyGame.hpp"

#include <math.h>

#include "bcs/BCS_Vec.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CQPanel.hpp"
#include "game/msw/CStageHockey.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"

namespace {

    // The stage with the score panels.
    const long STAGE_PANEL = 10;

    // Panel sides of stage 10: the panels of side 0 lie at z <= -8, those
    // of side 1 at z >= 8.
    const long PANEL_SIDE_FRONT = 0;
}

long msw::CComManHockeyGame::IsExistChrToTargetHockey(const CChrCore *pChr, float fAngle, const bcs::FVEC3 &rTarget,
                                                      float fWidth, float fLength, unsigned long nChrBits)
{
    return IsExistChrToTargetWorldHockey(pChr, fAngle, Local2World(rTarget, -1), fWidth, fLength, nChrBits);
}

long msw::CComManHockeyGame::IsExistChrToTargetWorldHockey(const CChrCore *pChr, float fAngle,
                                                           const bcs::FVEC3 &rTarget, float fWidth, float fLength,
                                                           unsigned long nChrBits)
{
    bcs::FVEC3 myPos = GetChrWorldPos(pChr);
    long myNo = pChr->GetPlayerNo();
    float targetRot = pChr->GetRotY_ToPosXZ(rTarget);

    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        if (i == myNo) {
            continue;
        }
        const CChrCore *pOther = CChrMan::GetChrCore(i);
        if (pOther == 0 || pOther->IsDisable()) {
            continue;
        }
        if (!IsEnableBit(nChrBits, i)) {
            continue;
        }
        int bit16 = (pOther->GetStatusFlag() >> 16) & 1;
        if (bit16 == 1) {
            continue;
        }

        bcs::FVEC3 pos = GetChrWorldPos(pOther);
        if (!IsPointInFaceXZ(pos, myPos, rTarget, fWidth, fLength)) {
            continue;
        }

        // The angle between the two directions, in [-pi, pi].
        float diff = pChr->GetRotY_ToPosXZ(pos) - targetRot;
        while (diff > 6.2831855f) {
            diff -= 6.2831855f;
        }
        while (diff < 0.0f) {
            diff += 6.2831855f;
        }
        if (diff > 3.1415927f) {
            diff -= 6.2831855f;
        }
        if (fabsf(diff) > fAngle) {
            continue;
        }
        return pOther->GetPlayerNo();
    }
    return -1;
}

int msw::CComManHockeyGame::IsQPanelDisable(unsigned long nTask)
{
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(nTask);
    if (pTask != 0) {
        if (pTask->IsDelete()) {
            return 1;
        }
        return !static_cast<CQPanel *>(pTask)->CanAccess();
    }
    return 1;
}

int msw::CComManHockeyGame::GetAngleFromBall(const CChrCore *pChr, float fTime, float &rAngle)
{
    long ballIdx = GetActiveBallIdx();
    if (ballIdx < 0) {
        return 0;
    }

    bcs::FVEC3 ballPos = GetBallWorldVelPos(CBallMan::GetBallPtr(ballIdx), fTime);
    bcs::FVEC3 chrPos = GetChrVelPos(pChr, fTime);
    ballPos.x -= chrPos.x;
    ballPos.z -= chrPos.z;

    float angle = nw4r::math::Atan2Rad(ballPos.x, ballPos.z);
    while (angle > 6.2831855f) {
        angle -= 6.2831855f;
    }
    while (angle < 0.0f) {
        angle += 6.2831855f;
    }
    rAngle = angle;
    return 1;
}

long msw::CComManHockeyGame::DefenseShouldGetCharaCoreIndex(const CChrCore *pChr)
{
    // Up to three field players a side.
    bcs::FVEC3 myPos[3];
    const CChrCore *apEnemy[3];
    bcs::FVEC3 enemyPos[3];

    long myTeam = pChr->GetTeamNo();
    bcs::FVEC3 *pMyPos = myPos;
    const CChrCore **ppEnemy = apEnemy;
    bcs::FVEC3 *pEnemyPos = enemyPos;
    long myNum = 0;
    long enemyNum = 0;
    long myIdx = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        const CChrCore *pOther = CChrMan::GetChrCore(i);
        if (pOther == 0 || pOther->IsDisable() || pOther->IsKeeper()) {
            continue;
        }
        if (myTeam == pOther->GetTeamNo()) {
            if (pChr == pOther) {
                myIdx = myNum;
            }
            *pMyPos++ = GetChrWorldPos(pOther);
            myNum++;
        } else {
            *ppEnemy++ = pOther;
            *pEnemyPos++ = GetChrWorldPos(pOther);
            enemyNum++;
        }
    }

    long best = GetNearBestPosBaseEx(myPos, enemyPos, myNum, enemyNum, myIdx);
    if (best < 0) {
        return -1;
    }
    return apEnemy[best]->GetPlayerNo();
}

unsigned long msw::CComManHockeyGame::GetExisQPanelDist(long nKind, bcs::FVEC3 pos, float &rDist,
                                                        bcs::FVEC3 &rPanelPos)
{
    unsigned long kindBits;
    switch (nKind) {
        case 0:
            kindBits = 1;
            break;
        case 1:
            kindBits = 2;
            break;
        case 2:
            kindBits = 4;
            break;
        default:
            return 0;
    }

    unsigned long panel = QPanel_GetNear(pos, kindBits);
    rPanelPos.x = 0.0f;
    rPanelPos.y = 0.0f;
    rPanelPos.z = 0.0f;
    rDist = -1.0f;
    if (panel == 0) {
        return 0;
    }

    rPanelPos = QPanel_GetPos(panel);
    bcs::FVEC3 posXZ(pos.x, 0.0f, pos.z);
    bcs::FVEC3 panelXZ(rPanelPos.x, 0.0f, rPanelPos.z);
    rDist = bcs::FVEC3Dist(&posXZ, &panelXZ);
    return panel;
}

unsigned long msw::CComManHockeyGame::GetExisGreenShellDist(bcs::FVEC3 pos, float &rDist, bcs::FVEC3 &rShellPos)
{
    unsigned long shell = FallGShell_GetNear(pos);
    rShellPos.x = 0.0f;
    rShellPos.y = 0.0f;
    rShellPos.z = 0.0f;
    rDist = -1.0f;
    if (shell == 0) {
        return 0;
    }
    if (!FallGShell_IsEnable(shell)) {
        return 0;
    }

    rShellPos = FallGShell_GetPos(shell);
    bcs::FVEC3 posXZ(pos.x, 0.0f, pos.z);
    bcs::FVEC3 shellXZ(rShellPos.x, 0.0f, rShellPos.z);
    rDist = bcs::FVEC3Dist(&posXZ, &shellXZ);
    return shell;
}

int msw::CComManHockeyGame::IsNowFeverTime()
{
    int fever = 0;
    if (CGame::g_GameWork.GetStage() == STAGE_PANEL) {
        fever = CStageHockey10::IsNowFeverTime();
    }
    return fever;
}

int msw::CComManHockeyGame::isExistScorePanel(long nMode, long nSide, bcs::FVEC3 pos, float fMaxDist,
                                              bcs::FVEC3 &rPanelPos, float &rRadius)
{
    int found = 0;
    if (CGame::g_GameWork.GetStage() == STAGE_PANEL) {
        found = GetPanelMaxNum_Stage10(nMode, nSide, pos, fMaxDist, rPanelPos, rRadius);
    }
    return found;
}

int msw::CComManHockeyGame::GetPanelMaxNum_Stage10(long nMode, long nSide, bcs::FVEC3 pos, float fMaxDist,
                                                   bcs::FVEC3 &rPanelPos, float &rRadius)
{
    int found = 0;
    long num = CStageHockey10::GetPanelMaxNum();
    float best = fMaxDist;
    CStage10Base::CPanelInfoBase *pPanel = 0;
    for (long i = 0; i < num; i++) {
        pPanel = CStageHockey10::GetNextPanel(pPanel);
        if (pPanel == 0 || !pPanel->IsEnable()) {
            continue;
        }
        if (pPanel->GetPanelScore(1) < 1 && nMode == 1) {
            continue;
        }

        bcs::FVEC3 panelPos = pPanel->GetPos();
        if (nSide == PANEL_SIDE_FRONT) {
            if (panelPos.z > -8.0f) {
                continue;
            }
        } else {
            if (panelPos.z < 8.0f) {
                continue;
            }
        }

        const bcs::FVEC3 &rPos = pPanel->GetPos();
        bcs::FVEC3 posXZ(pos.x, 0.0f, pos.z);
        bcs::FVEC3 panelXZ(rPos.x, 0.0f, rPos.z);
        float dist = bcs::FVEC3Dist(&posXZ, &panelXZ);
        if (dist >= fMaxDist) {
            continue;
        }
        if (dist < best) {
            rPanelPos = pPanel->GetPos();
            rRadius = pPanel->GetRadius();
            best = dist;
            found = 1;
        }
    }
    return found;
}

long msw::CComManHockeyGame::isExistScorePanelNowPos(bcs::FVEC3 pos)
{
    if (CGame::g_GameWork.GetStage() != STAGE_PANEL) {
        return -1;
    }

    long result = -1;
    long num = CStageHockey10::GetPanelMaxNum();
    CStage10Base::CPanelInfoBase *pPanel = 0;
    for (long i = 0; i < num; i++) {
        pPanel = CStageHockey10::GetNextPanel(pPanel);
        if (pPanel == 0 || !pPanel->IsEnable()) {
            continue;
        }

        const bcs::FVEC3 &rPos = pPanel->GetPos();
        bcs::FVEC3 posXZ(pos.x, 0.0f, pos.z);
        bcs::FVEC3 panelXZ(rPos.x, 0.0f, rPos.z);
        float dist = bcs::FVEC3Dist(&posXZ, &panelXZ);
        if (dist <= pPanel->GetRadius()) {
            if (pPanel->GetPanelScore(1) < 1) {
                result = 0;
            } else {
                result = 1;
            }
            break;
        }
    }
    return result;
}
