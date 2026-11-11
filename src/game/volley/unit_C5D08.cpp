// msw::CComDrvVolleyGame: running from and going round the stage gimmicks,
// items and obstacles. A file of its own, away from the rest of the class
// (game/volley/CComDrvVolleyGame.cpp), like CComDrvVolleyRR's three
// functions in game/volley/unit_CA990.cpp. The file name is not known.
#include "game/msw/CComDrvVolleyGame.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CItem.hpp"

// The obstacle kinds the driver runs from and goes round (bits; meanings
// not known).
static const unsigned long OBSTACLE_KIND = 0x1BFDE;

// The root actions during which the driver may break off to run from a
// gimmick: 0x0D, 0x0F, 0x12, 0x15-0x18, 0x1B and 0x1D.
int msw::CComDrvVolleyGame::ShoudEscGimmick() const
{
    if (!m_bGimmick) {
        return 0;
    }

    switch (GetRootActType()) {
        case 0x0D:
        case 0x0F:
        case 0x12:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x1B:
        case 0x1D:
            break;
        default:
            return 0;
    }

    unsigned long nItem = Item_GetItemToHitMe();
    if (nItem != 0) {
        CItem *pItem = static_cast<CItem *>(bcs::CTaskMan::GetTask(nItem));
        if (pItem != 0) {
            if (pItem->GetItemType() == CItem::KIND_BOMB) {
                if (m_bEscBomb) {
                    return 1;
                }
            } else if (m_bEscItem) {
                return 1;
            }
        }
    }

    if (m_bEscObstacle && Obstacle_GetObstacleToHitMe(OBSTACLE_KIND)) {
        return 1;
    }
    return 0;
}

int msw::CComDrvVolleyGame::GetEscPosFromGimmick(bcs::FVEC3 &rPos) const
{
    unsigned long nItem = Item_GetItemToHitMe();
    if (nItem != 0 && Item_GetEscPosFromItem(rPos, nItem)) {
        return 1;
    }

    unsigned long nObstacle = Obstacle_GetObstacleToHitMe(OBSTACLE_KIND);
    if (nObstacle != 0 && Obstacle_GetEscPosFromObstacle(rPos, nObstacle, m_bSpObstacleSide == 0)) {
        return 1;
    }
    return 0;
}

int msw::CComDrvVolleyGame::GetSafePosFromGimmick(bcs::FVEC3 &rPos) const
{
    long nPinball = Stage12_GetPinballToTarget(rPos, 15.0f);
    if (nPinball >= 0 && Stage12_GetSafePosFromPinball(rPos, nPinball)) {
        return 1;
    }

    long nCase = Stage15_GetCaseToTarget(rPos, 15.0f);
    if (nCase >= 0 && Stage15_GetSafePosFromCase(rPos, nCase)) {
        return 1;
    }
    return 0;
}

// Goes round what is in the way of rPos: a stage 15 case, a character, an
// item or an obstacle (kinds 0 to 3), and keeps going round the same one
// while it stays in the way.
int msw::CComDrvVolleyGame::GetPosTurnAroundGimmick(bcs::FVEC3 &rPos)
{
    long nKind = -1;
    long nDir = -1;
    long nTarget;
    int bItemSide = 0;
    if (!m_bEscBomb) {
        bItemSide = 1;
    }

    if (m_nTurnKind < 0) {
        nTarget = Stage15_GetCaseToTarget(rPos, 0.0f);
        if (nTarget >= 0) {
            nKind = 0;
            nDir = Stage15_GetPosTurnAroundCase(rPos, nTarget, -1);
        } else {
            nTarget = RunCorrect_GetChrToTarget(rPos);
            if (nTarget >= 0) {
                nKind = 1;
                nDir = RunCorrect_GetPosTurnAroundChr(rPos, nTarget, -1);
            } else {
                nTarget = Item_GetItemToTarget(rPos);
                if (nTarget != 0) {
                    nKind = 2;
                    nDir = Item_GetPosTurnAroundItem(rPos, nTarget, -1, bItemSide);
                } else {
                    nTarget = Obstacle_GetObstacleToTarget(rPos, OBSTACLE_KIND);
                    if (nTarget != 0) {
                        nKind = 3;
                        if (Obstacle_IsSpObstacleHandle(nTarget)) {
                            nDir = Obstacle_GetPosTurnAroundObstacle(rPos, nTarget, -1, m_bSpObstacleSide == 0);
                        } else {
                            nDir = Obstacle_GetPosTurnAroundObstacle(rPos, nTarget, -1, bItemSide);
                        }
                    }
                }
            }
        }

        if (nKind >= 0) {
            m_nTurnKind = nKind;
            m_nTurnTarget = nTarget;
            m_nTurnDir = nDir;
            m_fTurnFrame = 60.0f;
        }
    } else {
        nDir = m_nTurnDir;

        switch (m_nTurnKind) {
            case 0: {
                long nCase = Stage15_GetCaseToTarget(rPos, 0.0f);
                if (nCase == m_nTurnTarget) {
                    Stage15_GetPosTurnAroundCase(rPos, nCase, nDir);
                }
                break;
            }
            case 1: {
                long nChr = RunCorrect_GetChrToTarget(rPos);
                if (nChr == m_nTurnTarget) {
                    RunCorrect_GetPosTurnAroundChr(rPos, nChr, nDir);
                }
                break;
            }
            case 2: {
                long nItem = Item_GetItemToTarget(rPos);
                if (nItem == m_nTurnTarget) {
                    Item_GetPosTurnAroundItem(rPos, nItem, nDir, bItemSide);
                }
                break;
            }
            case 3: {
                long nObstacle = Obstacle_GetObstacleToTarget(rPos, OBSTACLE_KIND);
                if (nObstacle == m_nTurnTarget) {
                    if (!Obstacle_IsSpObstacleHandle(nObstacle)) {
                        Obstacle_GetPosTurnAroundObstacle(rPos, nObstacle, nDir, bItemSide);
                    } else {
                        Obstacle_GetPosTurnAroundObstacle(rPos, nObstacle, nDir, m_bSpObstacleSide == 0);
                    }
                }
                break;
            }
        }
    }
    return m_nTurnKind >= 0;
}

void msw::CComDrvVolleyGame::GetPosTurnAroundForRunEx(bcs::FVEC3 &rPos)
{
    GetPosTurnAroundGimmick(rPos);
}

// The first of the trolley, an item and a character that gives a point to
// look at.
void msw::CComDrvVolleyGame::GetViewPointPosForRunEx(bcs::FVEC3 &rPos)
{
    if (Stage17_GetViewPointPosFromTrolley(rPos)) {
        return;
    }
    if (Item_GetViewPointPosFromItem(rPos)) {
        return;
    }
    if (RunCorrect_GetViewPointPosFromChr(rPos)) {
        return;
    }
}
