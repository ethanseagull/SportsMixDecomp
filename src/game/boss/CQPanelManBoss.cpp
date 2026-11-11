// CQPanelManBoss and CQPanelBoss
// (written from DodgeF game/dodge/CQPanelManDodge.cpp; tables and timings
// from BossF)
//
// The "?" panels of the boss game: the manager's tables (item popup rates by
// the score difference, one set of panel places), the touch check (coins and
// items; with bit 1 of the flags the touch can also throw a ball in) and the
// panel's access and timing.
#include "game/msw/CQPanelManBoss.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/New.hpp"
#include <math.h>

namespace {

    using msw::CQPanelMan;

    // Rate table 0 (SetItemPopRate_ index 0).
    const CQPanelMan::ITEM_POPUP_RATE_TBL_TBL s_ItemPopRateTbl0 = { {
        { { 0, 50, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 50, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 50, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 50, 0, 0, 50, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 50, 0, 0, 50, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    } };

    // Rate table 1 (SetItemPopRate_ index 1).
    const CQPanelMan::ITEM_POPUP_RATE_TBL_TBL s_ItemPopRateTbl1 = { {
        { { 0, 50, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 50, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 50, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 50, 0, 0, 50, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 50, 0, 0, 50, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    } };

    // Panel places, for every stage: a ring of eight, a second ring turned
    // by half a step, and a cross of four.
    const CQPanelMan::POPUP_GROUP s_aPopupGroup[6] = {
        { 0, -1, { 4, 4, 4, 4, 4 }, {
            { 33, 0.0f, -20.0f, 0.0f },
            { 33, 14.142f, -14.142f, -45.0f },
            { 33, 20.0f, 0.0f, -90.0f },
            { 33, 14.142f, 14.142f, -135.0f },
            { 33, 0.0f, 20.0f, -180.0f },
            { 33, -14.142f, 14.142f, -225.0f },
            { 33, -20.0f, 0.0f, -270.0f },
            { 33, -14.142f, -14.142f, -315.0f },
        } },
        { 0, -1, { 4, 4, 4, 4, 4 }, {
            { 33, 7.65f, -18.48f, -22.5f },
            { 33, 18.48f, -7.65f, -67.5f },
            { 33, 18.48f, 7.65f, -112.5f },
            { 33, 7.65f, 18.48f, -157.5f },
            { 33, -7.65f, 18.48f, -202.5f },
            { 33, -18.48f, 7.65f, -247.5f },
            { 33, -18.48f, -7.65f, -292.5f },
            { 33, -7.65f, -18.48f, -337.5f },
        } },
        { 0, -1, { 2, 2, 2, 2, 2 }, {
            { 33, 0.0f, -7.0f, 0.0f },
            { 33, -7.0f, 0.0f, 90.0f },
            { 33, 0.0f, 7.0f, 180.0f },
            { 33, 7.0f, 0.0f, 270.0f },
        } },
        { 0 },
        { 0 },
        { 0 },
    };

    // (name guessed) fBase seconds plus a random part up to fRand seconds,
    // in frames, rounded up.
    inline float CalcPopupTime(float fBase, float fRand)
    {
        float fAdd = 0.0f;
        if (fRand > fAdd) {
            fAdd = bcs::BCS_RandMax(fRand);
        }
        fAdd += fBase;
        return ceil(60.0f * fAdd);
    }
}

namespace msw {

    CQPanelManBoss::CQPanelManBoss(bcs::CTask *pParent) : CQPanelMan(pParent, 1)
    {
    }

    CQPanelManBoss::~CQPanelManBoss()
    {
        AllReset();
    }

    const CQPanelMan::POPUP_GROUP *CQPanelManBoss::GetPopupGroupData_(long nStage) const
    {
        return s_aPopupGroup;
    }

    void CQPanelManBoss::SetUp()
    {
        CQPanelMan::SetUp();
    }

    CQPanel *CQPanelManBoss::CreatePanel_(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, PANEL_POP_INFO *pInfo,
                                          long nIdx)
    {
        return new (0x100) CQPanelBoss(this, m_pScnGroup, rPos, rRot, pInfo, nIdx);
    }

    void CQPanelManBoss::SetupItemPopRate_()
    {
        SetItemPopRate_(0, s_ItemPopRateTbl0);
        SetItemPopRate_(1, s_ItemPopRateTbl1);
    }

    // Row 0/1 when nTeam leads by 10/5 points or more, 4/3 when it trails
    // by as much, 2 otherwise.
    long CQPanelManBoss::ChoiceItemPopRateTblIdx_(long nTeam) const
    {
        long nDiff = CGame::GetTeamScore(nTeam) - CGame::GetTeamScore(1 - nTeam);
        long nIdx = 2;

        if (nDiff >= 10) {
            nIdx = 0;
        } else if (nDiff >= 5) {
            nIdx = 1;
        } else if (nDiff <= -10) {
            nIdx = 4;
        } else if (nDiff <= -5) {
            nIdx = 3;
        }
        return nIdx;
    }

    // A touch at rPos by team nTeam; with bit 1 of nFlag set the height is
    // not checked, and the touch may throw a ball in (mostly when no ball
    // is active) instead of giving its reward. An item panel gives its item
    // when the team may take one, a coin panel a coin (unless bit 0 of the
    // game work's word 0x40 is set); otherwise the panel is a miss.
    long CQPanelManBoss::QPanelCheck_(const bcs::FVEC3 &rPos, long nTeam, unsigned long nFlag, long *pItem,
                                      CItem **ppItem)
    {
        int bAnyHeight = (nFlag >> 1) & 1;

        if (!bAnyHeight && rPos.y >= 0.1f) {
            return 0;
        }

        bcs::FVEC3 pos = rPos;
        CQPanel *pPanel = QPanelHitCheck_(pos, 0);
        if (pPanel != 0) {
            pos = pPanel->GetPos();
            long nItem = -1;
            long ret = pPanel->DoAccess(pos, nTeam, nFlag, nItem, ppItem);

            int bThrowBall = bAnyHeight;
            if (CBallMan::GetActiveBallNum() > 0) {
                bThrowBall = 0;
            }
            if (bcs::BCS_RandMax(99L) > 65) {
                bThrowBall = 0;
            }
            if (bThrowBall) {
                CBall *pBall = CBallMan::GetBallPtr(0);
                if (pBall != 0) {
                    pBall->SetPos(pos);
                    pBall->SetJumpBallMode();
                }
                return 0;
            }

            switch (ret) {
                case 2:
                    if (CGame::CanbeTeamItemPush(nTeam)) {
                        CGameSnd::Play(0xC3B, pos, 1, -1, 0.0f, 1.0f, 0.0f);
                    } else if (bAnyHeight && CBallMan::GetActiveBallNum() <= 0) {
                        CBall *pBall = CBallMan::GetBallPtr(0);
                        if (pBall != 0) {
                            pBall->SetPos(pos);
                            pBall->SetJumpBallMode();
                            return 0;
                        }
                        CGameSnd::Play(0xC40, pos, 1, -1, 0.0f, 1.0f, 0.0f);
                    } else {
                        pPanel->SetSuka(1);
                        CGameSnd::Play(0xC40, pos, 1, -1, 0.0f, 1.0f, 0.0f);
                        ret = 0;
                    }
                    break;
                case 1:
                    if (!(CGame::g_GameWork.m_nUnk40 & 1)) {
                        CGimmickMan::EffectCoinGetCreate(pos, DROP_COIN::COIN_TYPE_COIN);
                        CGame::AddTeamCoin(nTeam, 1);
                    }
                    break;
            }
            if (pItem != 0) {
                *pItem = nItem;
            }
            return ret;
        }
        return 0;
    }

    CQPanelBoss::~CQPanelBoss()
    {
    }

    float CQPanelBoss::GetSize() const
    {
        return 1.09f;
    }

    long CQPanelBoss::DoAccess(const bcs::FVEC3 &rPos, long nChr, unsigned long nFlag, long &rItem, CItem **ppItem)
    {
        bcs::FVEC3 pos = m_Pos;
        int bArg = nFlag & 1;
        int bNoItem = nFlag & 4;

        if (bNoItem || bArg) {
            rItem = -1;
        } else if (rItem < 0) {
            rItem = m_pMan->ChoicePopupItem(nChr, bArg);
        }
        long ret = (rItem >= 0) ? 2 : 1;
        if (rItem >= 0 && m_pMan->IsItemCreate()) {
            CItem *pItem = CGimmickMan::CreateItem(rItem, pos);
            if (ppItem != 0) {
                *ppItem = pItem;
            }
        }
        m_nFlag &= ~(FLAG_REQ_POPUP | FLAG_CAN_ACCESS);
        ChangeState(3);
        return ret;
    }

    // The times below are in seconds.
    float CQPanelBoss::GetLifeTime() const
    {
        float fBase = 30.0f;
        float fRand = 0.0f;

        return CalcPopupTime(fBase, fRand);
    }

    // Bit 0 of the game work's word 0x1C8 makes the panels pop up again
    // without waiting.
    float CQPanelBoss::GetFirstPopupWaitTime() const
    {
        float fTime = 2.0f;

        if (CGame::g_GameWork.m_nUnk1C8 & 1) {
            fTime = 0.0f;
        }
        return CalcPopupTime(fTime, fTime);
    }

    float CQPanelBoss::GetNextPopupWaitTime() const
    {
        float fBase = 6.0f;
        float fRand = 0.0f;

        if (CGame::g_GameWork.m_nUnk1C8 & 1) {
            fBase = 0.0f;
        }
        return CalcPopupTime(fBase, fRand);
    }
}
