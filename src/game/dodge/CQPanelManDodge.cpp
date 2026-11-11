// CQPanelManDodge, CQPanelDodge and CQPanelManDodgeDodgeOrDie
// (written from BasketF game/basket/CQPanelBasket.cpp; tables and timings
// from DodgeF)
//
// The "?" panels of the dodgeball game: the manager's tables (item popup
// rates by the lives difference, panel places by stage), the touch check
// (coins and items, or a miss), the panel's access and timing, and the
// manager of the party game ("Dodge or Die"), whose panels only give coins.
#include "game/msw/CQPanelManDodge.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CDropCoinMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameDodge.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/New.hpp"
#include <math.h>

namespace {

    using msw::CQPanelMan;

    // Rate table 1 (SetItemPopRate_ index 1).
    const CQPanelMan::ITEM_POPUP_RATE_TBL_TBL s_ItemPopRateTbl1 = { {
        { { 0, 25, 25, 20, 10, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 25, 25, 20, 10, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 25, 25, 20, 10, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 25, 20, 20, 15, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 20, 20, 20, 20, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    } };

    // Rate table 0 (SetItemPopRate_ index 0).
    const CQPanelMan::ITEM_POPUP_RATE_TBL_TBL s_ItemPopRateTbl0 = { {
        { { 50, 15, 10, 10, 5, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 50, 15, 10, 10, 5, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 70, 10, 5, 5, 5, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 70, 7, 5, 5, 5, 3, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 70, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    } };

    // Panel places: s_aPopupGroupDefault for every other stage, the others
    // for the stage in their name.
    const CQPanelMan::POPUP_GROUP s_aPopupGroupDefault[6] = {
        { 0, -1, { 0, 1, 1, 1, 4 }, {
            { 65, -3.0f, 1.5f },
            { 65, 0.0f, 1.5f },
            { 65, 3.0f, 1.5f },
            { 65, -3.0f, 4.5f },
            { 65, 0.0f, 4.5f },
            { 65, 3.0f, 4.5f },
            { 65, -3.0f, 7.5f },
            { 65, 0.0f, 7.5f },
            { 65, 3.0f, 7.5f },
        } },
        { 0, -1, { 0, 1, 1, 1, 4 }, {
            { 65, -3.0f, 1.5f, 0.0f, 0, 1 },
            { 65, 0.0f, 1.5f, 0.0f, 0, 1 },
            { 65, 3.0f, 1.5f, 0.0f, 0, 1 },
            { 65, -3.0f, 4.5f, 0.0f, 0, 1 },
            { 65, 0.0f, 4.5f, 0.0f, 0, 1 },
            { 65, 3.0f, 4.5f, 0.0f, 0, 1 },
            { 65, -3.0f, 7.5f, 0.0f, 0, 1 },
            { 65, 0.0f, 7.5f, 0.0f, 0, 1 },
            { 65, 3.0f, 7.5f, 0.0f, 0, 1 },
        } },
        { 0, -1, { 0, 0, 1, 1, 3 }, {
            { 1, -3.2f, 10.3f },
            { 1, 0.0f, 10.3f },
            { 1, 3.2f, 10.3f },
            { 1, -5.55f, 7.9f },
            { 1, -5.55f, 4.8f },
            { 1, 5.55f, 7.9f },
            { 1, 5.55f, 4.8f },
        } },
        { 0, -1, { 0, 0, 1, 1, 3 }, {
            { 1, -3.2f, 10.3f, 0.0f, 0, 1 },
            { 1, 0.0f, 10.3f, 0.0f, 0, 1 },
            { 1, 3.2f, 10.3f, 0.0f, 0, 1 },
            { 1, -5.55f, 7.9f, 0.0f, 0, 1 },
            { 1, -5.55f, 4.8f, 0.0f, 0, 1 },
            { 1, 5.55f, 7.9f, 0.0f, 0, 1 },
            { 1, 5.55f, 4.8f, 0.0f, 0, 1 },
        } },
        { 0 },
        { 0 },
    };

    const CQPanelMan::POPUP_GROUP s_aPopupGroup5[6] = {
        { 0, -1, { 0, 1, 1, 1, 3 }, {
            { 65, -3.0f, 1.5f },
            { 65, 0.0f, 1.5f },
            { 65, 3.0f, 1.5f },
            { 65, -3.0f, 4.5f },
            { 65, 0.0f, 4.5f },
            { 65, 3.0f, 4.5f },
        } },
        { 0, -1, { 0, 1, 1, 1, 3 }, {
            { 65, -3.0f, 1.5f, 0.0f, 0, 1 },
            { 65, 0.0f, 1.5f, 0.0f, 0, 1 },
            { 65, 3.0f, 1.5f, 0.0f, 0, 1 },
            { 65, -3.0f, 4.5f, 0.0f, 0, 1 },
            { 65, 0.0f, 4.5f, 0.0f, 0, 1 },
            { 65, 3.0f, 4.5f, 0.0f, 0, 1 },
            { 0, 0.0f, 0.0f, 0.0f, 0, 1 },
        } },
        { 0, -1, { 0, 0, 1, 1, 3 }, {
            { 9, -3.2f, 10.3f, 0.0f, 0, 0, 10 },
            { 9, 0.0f, 10.3f, 0.0f, 0, 0, 10 },
            { 9, 3.2f, 10.3f, 0.0f, 0, 0, 10 },
            { 9, -5.55f, 7.9f, 0.0f, 0, 0, 10 },
            { 9, -5.55f, 4.8f, 0.0f, 0, 0, 10 },
            { 9, 5.55f, 7.9f, 0.0f, 0, 0, 10 },
            { 9, 5.55f, 4.8f, 0.0f, 0, 0, 10 },
        } },
        { 0, -1, { 0, 0, 1, 1, 3 }, {
            { 9, -3.2f, 10.3f, 0.0f, 0, 1, 10 },
            { 9, 0.0f, 10.3f, 0.0f, 0, 1, 10 },
            { 9, 3.2f, 10.3f, 0.0f, 0, 1, 10 },
            { 9, -5.55f, 7.9f, 0.0f, 0, 1, 10 },
            { 9, -5.55f, 4.8f, 0.0f, 0, 1, 10 },
            { 9, 5.55f, 7.9f, 0.0f, 0, 1, 10 },
            { 9, 5.55f, 4.8f, 0.0f, 0, 1, 10 },
        } },
        { 0 },
        { 0 },
    };

    const CQPanelMan::POPUP_GROUP s_aPopupGroup9[6] = {
        { 0, 0, { 0, 1, 1, 1, 4 }, {
            { 69, -3.0f, 2.0f },
            { 69, 0.0f, 2.0f },
            { 69, 3.0f, 2.0f },
            { 69, -3.0f, 7.0f },
            { 69, 0.0f, 7.0f },
            { 69, 3.0f, 7.0f },
        } },
        { 0, 0, { 0, 1, 1, 1, 4 }, {
            { 69, -3.0f, 2.0f, 0.0f, 0, 1 },
            { 69, 0.0f, 2.0f, 0.0f, 0, 1 },
            { 69, 3.0f, 2.0f, 0.0f, 0, 1 },
            { 69, -3.0f, 7.0f, 0.0f, 0, 1 },
            { 69, 0.0f, 7.0f, 0.0f, 0, 1 },
            { 69, 3.0f, 7.0f, 0.0f, 0, 1 },
        } },
        { 2, 0, { 1, 1, 1, 1, 2 }, {
            { 101, -3.0f, 2.0f, 90.0f },
            { 101, 0.0f, 2.0f, 270.0f },
            { 101, 3.0f, 2.0f, 90.0f },
            { 101, -3.0f, 7.0f, 270.0f },
            { 101, 0.0f, 7.0f, 90.0f },
            { 101, 3.0f, 7.0f, 270.0f },
        } },
        { 2, 0, { 1, 1, 1, 1, 2 }, {
            { 101, -3.0f, 2.0f, 90.0f, 0, 1 },
            { 101, 0.0f, 2.0f, 270.0f, 0, 1 },
            { 101, 3.0f, 2.0f, 90.0f, 0, 1 },
            { 101, -3.0f, 7.0f, 270.0f, 0, 1 },
            { 101, 0.0f, 7.0f, 90.0f, 0, 1 },
            { 101, 3.0f, 7.0f, 270.0f, 0, 1 },
        } },
        { 0, 1, { 0, 0, 1, 1, 3 }, {
            { 9, -3.2f, 10.7f, 0.0f, 0, 0, 10 },
            { 9, 0.0f, 10.7f, 0.0f, 0, 0, 10 },
            { 9, 3.2f, 10.7f, 0.0f, 0, 0, 10 },
            { 9, -5.75f, 7.9f, 0.0f, 0, 0, 10 },
            { 9, -5.75f, 4.8f, 0.0f, 0, 0, 10 },
            { 9, 5.75f, 7.9f, 0.0f, 0, 0, 10 },
            { 9, 5.75f, 4.8f, 0.0f, 0, 0, 10 },
        } },
        { 0, 1, { 0, 0, 1, 1, 3 }, {
            { 9, -3.2f, 10.7f, 0.0f, 0, 1, 10 },
            { 9, 0.0f, 10.7f, 0.0f, 0, 1, 10 },
            { 9, 3.2f, 10.7f, 0.0f, 0, 1, 10 },
            { 9, -5.75f, 7.9f, 0.0f, 0, 1, 10 },
            { 9, -5.75f, 4.8f, 0.0f, 0, 1, 10 },
            { 9, 5.75f, 7.9f, 0.0f, 0, 1, 10 },
            { 9, 5.75f, 4.8f, 0.0f, 0, 1, 10 },
        } },
    };

    const CQPanelMan::POPUP_GROUP s_aPopupGroup11[6] = {
        { 0, -1, { 0, 1, 1, 1, 4 }, {
            { 65, -3.0f, 1.5f },
            { 65, 0.0f, 1.5f },
            { 65, 3.0f, 1.5f },
            { 65, -3.0f, 4.5f },
            { 65, 0.0f, 4.5f },
            { 65, 3.0f, 4.5f },
            { 65, -3.0f, 7.5f },
            { 65, 0.0f, 7.5f },
            { 65, 3.0f, 7.5f },
        } },
        { 0, -1, { 0, 1, 1, 1, 4 }, {
            { 65, -3.0f, 1.5f, 0.0f, 0, 1 },
            { 65, 0.0f, 1.5f, 0.0f, 0, 1 },
            { 65, 3.0f, 1.5f, 0.0f, 0, 1 },
            { 65, -3.0f, 4.5f, 0.0f, 0, 1 },
            { 65, 0.0f, 4.5f, 0.0f, 0, 1 },
            { 65, 3.0f, 4.5f, 0.0f, 0, 1 },
            { 65, -3.0f, 7.5f, 0.0f, 0, 1 },
            { 65, 0.0f, 7.5f, 0.0f, 0, 1 },
            { 65, 3.0f, 7.5f, 0.0f, 0, 1 },
        } },
        { 0, -1, { 0, 0, 1, 1, 3 }, {
            { 9, -3.2f, 10.3f },
            { 9, 0.0f, 10.3f },
            { 9, 3.2f, 10.3f },
            { 9, -5.55f, 7.9f },
            { 9, -5.55f, 4.8f },
            { 9, 5.55f, 7.9f },
            { 9, 5.55f, 4.8f },
        } },
        { 0, -1, { 0, 0, 1, 1, 3 }, {
            { 9, -3.2f, 10.3f, 0.0f, 0, 1 },
            { 9, 0.0f, 10.3f, 0.0f, 0, 1 },
            { 9, 3.2f, 10.3f, 0.0f, 0, 1 },
            { 9, -5.55f, 7.9f, 0.0f, 0, 1 },
            { 9, -5.55f, 4.8f, 0.0f, 0, 1 },
            { 9, 5.55f, 7.9f, 0.0f, 0, 1 },
            { 9, 5.55f, 4.8f, 0.0f, 0, 1 },
        } },
        { 0 },
        { 0 },
    };

    const CQPanelMan::POPUP_GROUP s_aPopupGroup17[6] = {
        { 0, -1, { 0, 1, 1, 1, 2 }, {
            { 49, 0.0f, 10.25f, 180.0f },
            { 49, 0.0f, 2.25f },
            { 49, -4.0f, 6.25f, 90.0f },
            { 49, 4.0f, 6.25f, 270.0f },
        } },
        { 0, -1, { 0, 1, 1, 1, 2 }, {
            { 49, 0.0f, -10.25f, 0.0f, 0, 1 },
            { 49, 0.0f, -2.25f, 180.0f, 0, 1 },
            { 49, -4.0f, -6.25f, 90.0f, 0, 1 },
            { 49, 4.0f, -6.25f, 270.0f, 0, 1 },
        } },
        { 0 },
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

    CQPanelManDodge::CQPanelManDodge(bcs::CTask *pParent) : CQPanelMan(pParent, 1)
    {
    }

    CQPanelManDodge::~CQPanelManDodge()
    {
        AllReset();
    }

    const CQPanelMan::POPUP_GROUP *CQPanelManDodge::GetPopupGroupData_(long nStage) const
    {
        // The party game has no panels here (its manager is
        // CQPanelManDodgeDodgeOrDie).
        if (CGame::g_GameWork.m_nGameType == 2) {
            return 0;
        }
        switch (nStage) {
            case 5:
                return s_aPopupGroup5;
            case 9:
                return s_aPopupGroup9;
            case 11:
                return s_aPopupGroup11;
            case 17:
                return s_aPopupGroup17;
            default:
                return s_aPopupGroupDefault;
        }
    }

    void CQPanelManDodge::SetUp()
    {
        CQPanelMan::SetUp();
    }

    CQPanel *CQPanelManDodge::CreatePanel_(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, PANEL_POP_INFO *pInfo,
                                            long nIdx)
    {
        return new (0x100) CQPanelDodge(this, m_pScnGroup, rPos, rRot, pInfo, nIdx);
    }

    // A touch at rPos by team nTeam; nothing is touched from 0.1 above the
    // floor. An item panel gives its item when the team may take one, a coin
    // panel a coin while the game runs; otherwise the panel is a miss and 0
    // is returned.
    long CQPanelManDodge::QPanelCheck_(const bcs::FVEC3 &rPos, long nTeam, unsigned long nFlag, long *pItem,
                                       CItem **ppItem)
    {
        int bHitArg = !(nFlag & 2);

        if (rPos.y >= 0.1f) {
            return 0;
        }

        bcs::FVEC3 pos = rPos;
        CQPanel *pPanel = QPanelHitCheck_(pos, bHitArg != 0);
        if (pPanel != 0) {
            pos = pPanel->GetPos();
            long nItem = -1;
            long ret = pPanel->DoAccess(pos, nTeam, nFlag, nItem, ppItem);
            switch (ret) {
                case 2: {
                    int bOk = 1;
                    if (!CGame::IsMainLoopState(1)) {
                        bOk = 0;
                    }
                    if (!CGame::CanbeTeamItemPush(nTeam)) {
                        bOk = 0;
                    }
                    if (bOk) {
                        CGameSnd::Play(0xC3B, pos, 1, -1, 0.0f, 1.0f, 0.0f);
                    } else {
                        pPanel->SetSuka(1);
                        CGameSnd::Play(0xC40, pos, 1, -1, 0.0f, 1.0f, 0.0f);
                        ret = 0;
                    }
                    break;
                }
                case 1: {
                    int bOk = 1;
                    if (!CGame::IsMainLoopState(1)) {
                        bOk = 0;
                    }
                    if (CGame::g_GameWork.m_nUnk40 & 1) {
                        bOk = 0;
                    }
                    if (bOk) {
                        CGimmickMan::EffectCoinGetCreate(pos, DROP_COIN::COIN_TYPE_COIN);
                        CGame::AddTeamCoin(nTeam, 1);
                    } else {
                        pPanel->SetSuka(1);
                        CGameSnd::Play(0xC40, pos, 1, -1, 0.0f, 1.0f, 0.0f);
                        ret = 0;
                    }
                    break;
                }
            }
            if (pItem != 0) {
                *pItem = nItem;
            }
            return ret;
        }
        return 0;
    }

    void CQPanelManDodge::SetupItemPopRate_()
    {
        SetItemPopRate_(0, s_ItemPopRateTbl0);
        SetItemPopRate_(1, s_ItemPopRateTbl1);
    }

    // Row 0/1 when team nTeam's infielders have 200/100 lives more than the
    // other team's, 4/3 when they have as many fewer, 2 otherwise.
    long CQPanelManDodge::ChoiceItemPopRateTblIdx_(long nTeam) const
    {
        long nDiff = 0;

        if (CChrMan::IsActive() && nTeam >= 0 && nTeam < 2) {
            long anLife[2] = { 0, 0 };

            for (long nTm = 0; nTm < 2; nTm++) {
                long nNum = CChrMan::GetChrCoreNum();
                for (long i = 0; i < nNum; i++) {
                    CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr != 0 && !pChr->IsDisable() && nTm == pChr->GetTeamNo() &&
                        !CGame::IsCharEx_DodgeOutfielder(pChr->GetPlayerNo()) && pChr->IsDodgeInfield()) {
                        anLife[nTm] += CGameDodge::GetChrRestLife(pChr->GetPlayerNo());
                    }
                }
            }
            nDiff = anLife[nTeam] - anLife[1 - nTeam];
        }

        long nIdx = 2;
        if (nDiff >= 200) {
            nIdx = 0;
        } else if (nDiff >= 100) {
            nIdx = 1;
        } else if (nDiff <= -200) {
            nIdx = 4;
        } else if (nDiff <= -100) {
            nIdx = 3;
        }
        return nIdx;
    }

    CQPanelDodge::~CQPanelDodge()
    {
    }

    float CQPanelDodge::GetSize() const
    {
        return 0.8f;
    }

    // The times below are in seconds; m_pMan's level picks them.
    float CQPanelDodge::GetLifeTime() const
    {
        float fBase = 30.0f;
        float fRand = 0.0f;

        switch (m_pMan->GetLevel()) {
            case 1:
                fBase = 30.0f;
                fRand = 0.0f;
                break;
            case 2:
                fBase = 30.0f;
                fRand = 0.0f;
                break;
            case 3:
                fBase = 30.0f;
                fRand = 0.0f;
                break;
            case 4:
                fBase = 30.0f;
                fRand = 0.0f;
                break;
        }
        return CalcPopupTime(fBase, fRand);
    }

    float CQPanelDodge::GetFirstPopupWaitTime() const
    {
        float fBase = 35.0f;
        float fRand = 10.0f;

        switch (m_pMan->GetLevel()) {
            case 1:
                fBase = 35.0f;
                fRand = 10.0f;
                break;
            case 2:
                fBase = 15.0f;
                fRand = 10.0f;
                break;
            case 3:
                fBase = 2.0f;
                fRand = 10.0f;
                break;
            case 4:
                fBase = 1.0f;
                fRand = 4.0f;
                break;
        }
        return CalcPopupTime(fBase, fRand);
    }

    float CQPanelDodge::GetNextPopupWaitTime() const
    {
        float fBase = 58.0f;
        float fRand = 4.0f;

        switch (m_pMan->GetLevel()) {
            case 1:
                fBase = 58.0f;
                break;
            case 2:
                fBase = 28.0f;
                break;
            case 3:
                fBase = 8.0f;
                break;
            case 4:
                fBase = 8.0f;
                break;
        }
        return CalcPopupTime(fBase, fRand);
    }

    long CQPanelDodge::DoAccess(const bcs::FVEC3 &rPos, long nChr, unsigned long nFlag, long &rItem, CItem **ppItem)
    {
        // (guess) A switch the original keeps in .rodata and never sets:
        // with it the touched item would be created at the panel.
        static const int bIsBallBound = 0;
        long ret = 0;

        DoCmnSendMsg_(nChr, -1);
        if (m_nUnk88 == 0) {
            bcs::FVEC3 pos = m_Pos;
            int bArg = nFlag & 1;

            if (nFlag & 4) {
                rItem = -1;
            } else if (rItem < 0) {
                rItem = m_pMan->ChoicePopupItem(nChr, bArg);
            }
            ret = (rItem >= 0) ? 2 : 1;
            if (bIsBallBound && rItem >= 0 && m_pMan->IsItemCreate()) {
                CItem *pItem = CGimmickMan::CreateItem(rItem, pos);
                if (ppItem != 0) {
                    *ppItem = pItem;
                }
            }
        }
        m_nFlag &= ~(FLAG_REQ_POPUP | FLAG_CAN_ACCESS);
        ChangeState(3);
        return ret;
    }

    CQPanelManDodgeDodgeOrDie::CQPanelManDodgeDodgeOrDie(bcs::CTask *pParent) : CQPanelManDodge(pParent)
    {
    }

    CQPanelManDodgeDodgeOrDie::~CQPanelManDodgeDodgeOrDie()
    {
    }

    // The party game's panels only give coins.
    long CQPanelManDodgeDodgeOrDie::QPanelCheck_(const bcs::FVEC3 &rPos, long nTeam, unsigned long nFlag,
                                                 long *pItem, CItem **ppItem)
    {
        if (!(nFlag & 2) && rPos.y >= 0.1f) {
            return 0;
        }

        bcs::FVEC3 pos = rPos;
        CQPanel *pPanel = QPanelHitCheck_(pos, 0);
        if (pPanel != 0) {
            pos = pPanel->GetPos();
            long nItem = 0;
            long ret = pPanel->DoAccess(pos, nTeam, nFlag, nItem, ppItem);
            if (ret == 1 && nItem == -1) {
                CGimmickMan::EffectCoinGetCreate(pos, DROP_COIN::COIN_TYPE_COIN);
                CGame::AddTeamCoin(nTeam, 1);
            }
            if (pItem != 0) {
                *pItem = nItem;
            }
            return ret;
        }
        return 0;
    }
}
