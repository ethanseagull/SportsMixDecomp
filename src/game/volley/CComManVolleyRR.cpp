#include "game/msw/CComManVolleyRR.hpp"

#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CItem.hpp"

namespace msw {

    CBall *CComManVolleyRR::SearchPersonalBall(const CChrCore *pChr, int bColor)
    {
        float fMinFrame = -1.0f;
        CBall *pFound = 0;

        for (long i = 0; i < CBallMan::GetBallNum(); i++) {
            CBall *pBall = CBallMan::GetBallPtr(i);
            if (pBall == 0 || !pBall->IsActive()) {
                continue;
            }
            if (GetValidPtr(i) == 0) {
                continue;
            }

            bcs::FVEC3 pos;
            if (!GetBallTargetPos(pBall, pos)) {
                continue;
            }

            // The balls are numbered per pair of players, as the teams are.
            if (bColor) {
                long nTeam = pChr->GetPlayerNo() / 2;
                if (pBall->GetVolleyBallColorTypeRR() != nTeam) {
                    continue;
                }
            } else {
                long nTeam = pChr->GetPlayerNo() / 2;
                if (pBall->GetVolleyBallTypeRR() != nTeam) {
                    continue;
                }
            }

            float fFrame = pBall->GetTouchDownFrame(0.3f);
            if (fFrame < 0.0f) {
                continue;
            }
            if (fMinFrame < 0.0f || fMinFrame > fFrame) {
                fMinFrame = fFrame;
                pFound = pBall;
            }
        }
        return pFound;
    }

    int CComManVolleyRR::IsExistOthersBall(const CChrCore *pChr, const bcs::FVEC3 &rPos, float fRange)
    {
        for (long i = 0; i < CBallMan::GetBallNum(); i++) {
            CBall *pBall = CBallMan::GetBallPtr(i);
            if (pBall == 0 || !pBall->IsActive()) {
                continue;
            }

            bcs::FVEC3 pos;
            if (!GetBallTargetPos(pBall, pos)) {
                continue;
            }

            long nType = pBall->GetVolleyBallTypeRR();
            if (nType == 4 || nType == pChr->GetPlayerNo() / 2) {
                continue;
            }
            if (GetDisXZ(pos, rPos) > fRange) {
                continue;
            }
            return 1;
        }
        return 0;
    }

    int CComManVolleyRR::IsExistItemBall(const CChrCore *pChr, const bcs::FVEC3 &rPos)
    {
        bcs::FVEC3 chrPos = Local2World(rPos, -1);

        for (CItem *pItem = ThrowItem_GetFirst(); pItem != 0; pItem = ThrowItem_GetNext(pItem)) {
            long nKind = pItem->GetItemType();
            bcs::FVEC3 pos;
            pos = pItem->GetPos();

            if (pItem->CanbePickup()) {
                continue;
            }
            if (nKind != CItem::KIND_TOADSTOOL && nKind != CItem::KIND_BANANA) {
                continue;
            }
            if (!pItem->IsTouchDown()) {
                if (pItem->GetTouchDownFrame() > 200.0f) {
                    continue;
                }
                if (!pItem->GetThrowTargetPos(pos)) {
                    continue;
                }
            }

            float fDist = GetDisXZ(chrPos, pos);
            if (fDist < 0.5f + (pChr->GetCollSize_BodyDamage() + pItem->GetAtkCollRadius())) {
                return 1;
            }
        }
        return 0;
    }

    CBall *CComManVolleyRR::GetValidPtr(long nBall)
    {
        if (nBall < 0) {
            return 0;
        }
        CBall *pBall = CBallMan::GetBallPtr(nBall);
        if (pBall == 0) {
            return 0;
        }
        if (!pBall->IsActive()) {
            return 0;
        }
        bcs::FVEC3 pos;
        // (written so: an early `return 0` here gives the branch the other
        // way round, in this function and where SearchPersonalBall inlines it)
        if (!GetBallTargetPos(pBall, pos)) {
            pBall = 0;
        }
        return pBall;
    }
}
