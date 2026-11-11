// msw::CComDrvDodgeGame, the computer driver of the dodgeball game: escaping
// what is about to hit the character (a bomb ball about to blow up, the
// stage gimmicks, items and obstacles) and going round what is in the way of
// a run. One of several files of the class (game/dodge/CComDrvDodgeGame.cpp
// is the main one); the file name is not known.
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComDrvDodge.hpp"
#include "game/msw/CItem.hpp"
#include "game/msw/CItemBallDodgeKinds.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CTaskMan.hpp"

namespace msw {

namespace {
    // The obstacle kinds the driver escapes and goes round (a mask of
    // obstacle kinds; the bits are not known).
    const unsigned long ESC_OBSTACLE_MASK = 0x1BFDE;

    // What GetPosTurnAroundGimmick is going round (m_nTurnKind).
    enum {
        TURN_PYLON = 0,     // stage 4
        TURN_MUD_BOMB = 1,  // stage 7
        TURN_CASE = 2,      // stage 15
        TURN_CHR = 10,
        TURN_ITEM = 20,
        TURN_OBSTACLE = 30
    };

    // Root actions during which the driver does not escape (meanings not
    // known).
    enum {
        ACT_NO_ESC_0D = 0x0D,
        ACT_NO_ESC_19 = 0x19,
        ACT_NO_ESC_1A = 0x1A,
        ACT_NO_ESC_25 = 0x25
    };

    // The unit direction from rFrom to rTo in the XZ plane.
    inline void DirXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        bcs::FVEC3 diff;
        diff.x = rTo.x - rFrom.x;
        diff.z = rTo.z - rFrom.z;
        bcs::BCS_Vec_Normalize(rOut, bcs::FVEC3(diff.x, 0.0f, diff.z));
    }

    // rVec brought to the length fLen in the XZ plane, its height kept.
    inline void SetLenXZ(bcs::FVEC3 &rVec, float fLen)
    {
        float y = rVec.y;
        bcs::BCS_Vec_Normalize(rVec, bcs::FVEC3(rVec.x, 0.0f, rVec.z));
        rVec.x *= fLen;
        rVec.z *= fLen;
        rVec.y = y;
    }
}

inline long CComDrvDodgeGame::ItemBall_GetBallToHitMe() const
{
    CBall *pBall = GetActiveBall();
    if (pBall == 0) {
        return -1;
    }
    if (pBall->GetItemBallType() != CItem::KIND_BOMB) {
        return -1;
    }

    bcs::FVEC3 pos = pBall->GetTargetPos();
    float fExplosion = CItemBallBombDodge::GetExplosionCollSize();
    float fRange = 0.5f + (fExplosion + m_pChr->GetCollSize_BodyDamage());
    if (GetDisXZ(m_MyWorldPos, pos) < fRange) {
        return pBall->GetBallNo();
    }
    return -1;
}

// Not zero when something is about to hit the character and it should get
// out of the way.
int CComDrvDodgeGame::ShoudEscGimmick() const
{
    if (m_nEscGimmick == 0) {
        return 0;
    }

    switch (GetRootActType()) {
        case ACT_NO_ESC_19:
        case ACT_NO_ESC_1A:
        case ACT_NO_ESC_0D:
        case ACT_NO_ESC_25:
            return 0;
    }

    if (ItemBall_GetBallToHitMe() >= 0) {
        return 1;
    }
    if (Stage12_GetPinballToHitMe(0.0f) >= 0) {
        return 1;
    }
    if (Stage17_GetTrolleyToHitMe() >= 0) {
        return 1;
    }

    unsigned long nItem = Item_GetItemToHitMe();
    if (nItem != 0) {
        CItem *pItem = static_cast<CItem *>(bcs::CTaskMan::GetTask(nItem));
        if (pItem != 0) {
            if (pItem->GetItemType() == CItem::KIND_BOMB) {
                if (m_bEscItemBomb) {
                    return 1;
                }
            } else if (m_bEscItem) {
                return 1;
            }
        }
    }

    if (m_bEscObstacle && Obstacle_GetObstacleToHitMe(ESC_OBSTACLE_MASK) != 0) {
        return 1;
    }
    return 0;
}

// rPos set to where to escape what is about to hit the character; zero when
// nothing is.
int CComDrvDodgeGame::GetEscPosFromGimmick(bcs::FVEC3 &rPos) const
{
    long nBall = ItemBall_GetBallToHitMe();
    if (nBall >= 0 && ItemBall_GetEscPosFromBall(rPos, nBall)) {
        return 1;
    }

    long nPinball = Stage12_GetPinballToHitMe(0.0f);
    if (nPinball >= 0 && Stage12_GetEscPosFromPinball(rPos, nPinball)) {
        return 1;
    }

    long nTrolley = Stage17_GetTrolleyToHitMe();
    if (nTrolley >= 0 && Stage17_GetEscPosFromTrolley(rPos, nTrolley)) {
        return 1;
    }

    unsigned long nItem = Item_GetItemToHitMe();
    if (nItem != 0 && Item_GetEscPosFromItem(rPos, nItem)) {
        return 1;
    }

    unsigned long nObstacle = Obstacle_GetObstacleToHitMe(ESC_OBSTACLE_MASK);
    if (nObstacle != 0 && Obstacle_GetEscPosFromObstacle(rPos, nObstacle, !m_bUnk22F)) {
        return 1;
    }
    return 0;
}

// rPos moved to a safe place from a stage 12 pinball heading for it; zero
// when none is.
int CComDrvDodgeGame::GetSafePosFromGimmick(bcs::FVEC3 &rPos) const
{
    long nPinball = Stage12_GetPinballToTarget(rPos, 15.0f);
    if (nPinball >= 0 && Stage12_GetSafePosFromPinball(rPos, nPinball)) {
        return 1;
    }
    return 0;
}

// rPos (where the character runs to) moved to go round what is in the way:
// a gimmick, a character, an item or an obstacle. Once a turn has started,
// keeps going round the same one while it is still in the way. Not zero
// while turning.
int CComDrvDodgeGame::GetPosTurnAroundGimmick(bcs::FVEC3 &rPos)
{
    long nKind = -1;
    long nDir = -1;
    long nTarget;
    int bNoBomb = 0;
    if (!m_bEscItemBomb) {
        bNoBomb = 1;
    }

    if (m_nTurnKind < 0) {
        if ((nTarget = Stage04_GetPylonToTarget(rPos)) >= 0) {
            nKind = TURN_PYLON;
            nDir = Stage04_GetPosTurnAroundPylon(rPos, nTarget, -1);
        } else if ((nTarget = Stage07_GetMudBombToTarget(rPos)) != 0) {
            nKind = TURN_MUD_BOMB;
            nDir = Stage07_GetPosTurnAroundMudBomb(rPos, nTarget, -1);
        } else if ((nTarget = Stage15_GetCaseToTarget(rPos, 0.0f)) >= 0) {
            nKind = TURN_CASE;
            nDir = Stage15_GetPosTurnAroundCase(rPos, nTarget, -1);
        } else if ((nTarget = RunCorrect_GetChrToTarget(rPos)) >= 0) {
            nKind = TURN_CHR;
            nDir = RunCorrect_GetPosTurnAroundChr(rPos, nTarget, -1);
        } else if ((nTarget = Item_GetItemToTarget(rPos)) != 0) {
            nKind = TURN_ITEM;
            nDir = Item_GetPosTurnAroundItem(rPos, nTarget, -1, bNoBomb);
        } else if ((nTarget = Obstacle_GetObstacleToTarget(rPos, ESC_OBSTACLE_MASK)) != 0) {
            nKind = TURN_OBSTACLE;
            if (!Obstacle_IsSpObstacleHandle(nTarget)) {
                nDir = Obstacle_GetPosTurnAroundObstacle(rPos, nTarget, -1, bNoBomb);
            } else {
                nDir = Obstacle_GetPosTurnAroundObstacle(rPos, nTarget, -1, !m_bUnk22F);
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
            case TURN_PYLON: {
                long nPylon = Stage04_GetPylonToTarget(rPos);
                if (nPylon == m_nTurnTarget) {
                    Stage04_GetPosTurnAroundPylon(rPos, nPylon, nDir);
                }
                break;
            }
            case TURN_MUD_BOMB: {
                long nBomb = Stage07_GetMudBombToTarget(rPos);
                if (nBomb == m_nTurnTarget) {
                    Stage07_GetPosTurnAroundMudBomb(rPos, nBomb, nDir);
                }
                break;
            }
            case TURN_CASE: {
                long nCase = Stage15_GetCaseToTarget(rPos, 0.0f);
                if (nCase == m_nTurnTarget) {
                    Stage15_GetPosTurnAroundCase(rPos, nCase, nDir);
                }
                break;
            }
            case TURN_CHR: {
                long nChr = RunCorrect_GetChrToTarget(rPos);
                if (nChr == m_nTurnTarget) {
                    RunCorrect_GetPosTurnAroundChr(rPos, nChr, nDir);
                }
                break;
            }
            case TURN_ITEM: {
                long nItem = Item_GetItemToTarget(rPos);
                if (nItem == m_nTurnTarget) {
                    Item_GetPosTurnAroundItem(rPos, nItem, nDir, bNoBomb);
                }
                break;
            }
            case TURN_OBSTACLE: {
                long nObstacle = Obstacle_GetObstacleToTarget(rPos, ESC_OBSTACLE_MASK);
                if (nObstacle == m_nTurnTarget) {
                    if (!Obstacle_IsSpObstacleHandle(nObstacle)) {
                        Obstacle_GetPosTurnAroundObstacle(rPos, nObstacle, nDir, bNoBomb);
                    } else {
                        Obstacle_GetPosTurnAroundObstacle(rPos, nObstacle, nDir, !m_bUnk22F);
                    }
                }
                break;
            }
        }
    }
    return m_nTurnKind >= 0;
}

void CComDrvDodgeGame::GetPosTurnAroundForRunEx(bcs::FVEC3 &rPos)
{
    GetPosTurnAroundGimmick(rPos);
}

// rPos set to where to look while running round something; the first of the
// gimmicks and items that gives one.
void CComDrvDodgeGame::GetViewPointPosForRunEx(bcs::FVEC3 &rPos)
{
    if (Stage04_GetViewPointPosFromPylon(rPos)) {
        return;
    }
    if (Stage07_GetViewPointPosFromMudBomb(rPos)) {
        return;
    }
    if (Stage17_GetViewPointPosFromTrolley(rPos)) {
        return;
    }
    if (Item_GetViewPointPosFromItem(rPos)) {
        return;
    }
}

// rPos set to where to escape the bomb ball nBall: straight away from it,
// just out of its blast. Zero when nBall is not a bomb ball.
int CComDrvDodgeGame::ItemBall_GetEscPosFromBall(bcs::FVEC3 &rPos, long nBall) const
{
    CBall *pBall = CBallMan::GetBallPtr(nBall);
    if (pBall == 0) {
        return 0;
    }
    if (pBall->GetItemBallType() != CItem::KIND_BOMB) {
        return 0;
    }

    bcs::FVEC3 ballPos = pBall->GetTargetPos();
    float fExplosion = CItemBallBombDodge::GetExplosionCollSize();
    float fRange = 0.5f + (fExplosion + m_pChr->GetCollSize_BodyDamage()) + 0.4f;

    bcs::FVEC3 dir;
    DirXZ(dir, ballPos, m_MyWorldPos);
    SetLenXZ(dir, fRange);

    bcs::FVEC3 escPos(ballPos.x + dir.x, 0.0f, ballPos.z + dir.z);
    RunCorrect_RenewEscPos(escPos, ballPos, dir, fRange);
    bcs::FVEC3 localPos = World2Local(escPos, -1);
    rPos = localPos;
    return 1;
}

// The way to turn first to go round something at pos lying between fMinX and
// fMaxX across the court: 0 or 1 when one side is blocked by the court's
// edge, -1 when either will do (or the character is not in the infield).
long CComDrvDodgeGame::RunCorrect_GetFirstRotDir(const bcs::FVEC3 &pos, float fMinX, float fMaxX) const
{
    if (!m_pChr->IsDodgeInfield()) {
        return -1;
    }

    float fLeft = GetMyCourtLeft() + (0.5f + m_pChr->GetCollSize_BodyPush());
    float fRight = GetMyCourtRight() - (0.5f + m_pChr->GetCollSize_BodyPush());

    if (pos.z < m_MyPos.z) {
        if (fMaxX >= fRight) {
            return 1;
        }
        if (fMinX <= fLeft) {
            return 0;
        }
    } else {
        if (fMaxX >= fRight) {
            return 0;
        }
        if (fMinX <= fLeft) {
            return 1;
        }
    }
    return -1;
}

// The same for going round a character.
long CComDrvDodgeGame::RunCorrect_GetFirstRotDirAroundChr(const bcs::FVEC3 &pos, float fMinX, float fMaxX) const
{
    if (!m_pChr->IsDodgeInfield()) {
        return -1;
    }

    float fLeft = GetMyCourtLeft() + (0.5f + m_pChr->GetCollSize_BodyPush());
    float fRight = GetMyCourtRight() - (0.5f + m_pChr->GetCollSize_BodyPush());

    if (pos.z < m_MyPos.z) {
        if (fMaxX >= fRight) {
            return 1;
        }
        if (fMinX <= fLeft) {
            return 0;
        }
    } else {
        if (fMaxX >= fRight) {
            return 0;
        }
        if (fMinX <= fLeft) {
            return 1;
        }
    }
    return -1;
}

}
