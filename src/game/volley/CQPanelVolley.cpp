#include "game/msw/CQPanelManVolley.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/New.hpp"
#include <math.h>

// The "?" panels of the volley game: the manager's tables (item popup rates
// by score difference in the current set, panel places by stage) and the
// panel's access and timing. Same shape as BasketF
// game/basket/CQPanelBasket.cpp; the values are VolleyF's.

namespace {

    using msw::CQPanelMan;

    // Rate table 0 (SetItemPopRate_ index 0).
    const CQPanelMan::ITEM_POPUP_RATE_TBL_TBL s_ItemPopRateTbl0 = { {
        { { 30, 20, 15, 15, 10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 30, 20, 15, 15, 10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 50, 10, 10, 10, 10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 50, 7, 10, 10, 10, 3, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 50, 5, 10, 10, 10, 5, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    } };

    // Rate table 1 (SetItemPopRate_ index 1), the same as table 0.
    const CQPanelMan::ITEM_POPUP_RATE_TBL_TBL s_ItemPopRateTbl1 = { {
        { { 30, 20, 15, 15, 10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 30, 20, 15, 15, 10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 50, 10, 10, 10, 10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 50, 7, 10, 10, 10, 3, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 50, 5, 10, 10, 10, 5, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    } };

    // Every other stage: two rows of three places, one group per side of
    // the net (nUnk12 is 1 in the second group).
    const CQPanelMan::POPUP_GROUP s_aPopupGroupDefault[6] = {
        { 0, -1, { 0, 1, 1, 1, 4 }, {
            { 1, -3.5f, 9.0f },
            { 1, 0.0f, 9.0f },
            { 1, 3.5f, 9.0f },
            { 1, -3.5f, 5.7f },
            { 1, 0.0f, 5.7f },
            { 1, 3.5f, 5.7f },
        } },
        { 0, -1, { 0, 1, 1, 1, 4 }, {
            { 1, -3.5f, 9.0f, 0.0f, 0, 1, 0 },
            { 1, 0.0f, 9.0f, 0.0f, 0, 1, 0 },
            { 1, 3.5f, 9.0f, 0.0f, 0, 1, 0 },
            { 1, -3.5f, 5.7f, 0.0f, 0, 1, 0 },
            { 1, 0.0f, 5.7f, 0.0f, 0, 1, 0 },
            { 1, 3.5f, 5.7f, 0.0f, 0, 1, 0 },
        } },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
    };

    // Stage 9.
    const CQPanelMan::POPUP_GROUP s_aPopupGroup9[6] = {
        { 0, -1, { 0, 1, 1, 1, 4 }, {
            { 5, -3.5f, 9.0f },
            { 5, 0.0f, 9.0f },
            { 5, 3.5f, 9.0f },
            { 5, -3.5f, 5.7f },
            { 5, 0.0f, 5.7f },
            { 5, 3.5f, 5.7f },
        } },
        { 0, -1, { 0, 1, 1, 1, 4 }, {
            { 5, -3.5f, 9.0f, 0.0f, 0, 1, 0 },
            { 5, 0.0f, 9.0f, 0.0f, 0, 1, 0 },
            { 5, 3.5f, 9.0f, 0.0f, 0, 1, 0 },
            { 5, -3.5f, 5.7f, 0.0f, 0, 1, 0 },
            { 5, 0.0f, 5.7f, 0.0f, 0, 1, 0 },
            { 5, 3.5f, 5.7f, 0.0f, 0, 1, 0 },
        } },
        { 2, -1, { 1, 1, 1, 1, 2 }, {
            { 37, -3.5f, 9.0f, 0.0f },
            { 37, 0.0f, 9.0f, 180.0f },
            { 37, 3.5f, 9.0f, 0.0f },
            { 37, -3.5f, 5.7f, 180.0f },
            { 37, 0.0f, 5.7f, 0.0f },
            { 37, 3.5f, 5.7f, 180.0f },
        } },
        { 2, -1, { 1, 1, 1, 1, 2 }, {
            { 37, -3.5f, 9.0f, 0.0f, 0, 1, 0 },
            { 37, 0.0f, 9.0f, 180.0f, 0, 1, 0 },
            { 37, 3.5f, 9.0f, 0.0f, 0, 1, 0 },
            { 37, -3.5f, 5.7f, 180.0f, 0, 1, 0 },
            { 37, 0.0f, 5.7f, 0.0f, 0, 1, 0 },
            { 37, 3.5f, 5.7f, 180.0f, 0, 1, 0 },
        } },
        { 0 },
        { 0 },
    };

    // Stage 17.
    const CQPanelMan::POPUP_GROUP s_aPopupGroup17[6] = {
        { 0, -1, { 0, 1, 1, 1, 2 }, {
            { 1, -3.5f, 9.0f },
            { 1, 0.0f, 9.0f },
            { 1, 3.5f, 9.0f },
        } },
        { 0, -1, { 0, 1, 1, 1, 2 }, {
            { 1, -3.5f, 9.0f, 0.0f, 0, 1, 0 },
            { 1, 0.0f, 9.0f, 0.0f, 0, 1, 0 },
            { 1, 3.5f, 9.0f, 0.0f, 0, 1, 0 },
            // (disabled; only its side byte is set)
            { 0, 0.0f, 0.0f, 0.0f, 0, 1, 0 },
        } },
        { 0, -1, { 0, 1, 1, 1, 3 }, {
            { 49, 0.0f, -5.0f, 0.0f, 0, 2, 0 },
            { 49, 0.0f, 5.0f, 180.0f, 0, 2, 0 },
            { 49, -5.0f, 0.0f, 90.0f, 0, 2, 0 },
            { 49, 5.0f, 0.0f, 270.0f, 0, 2, 0 },
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

    CQPanelManVolley::CQPanelManVolley(bcs::CTask *pParent) : CQPanelMan(pParent, 1)
    {
    }

    CQPanelManVolley::~CQPanelManVolley()
    {
        AllReset();
    }

    const CQPanelMan::POPUP_GROUP *CQPanelManVolley::GetPopupGroupData_(long nStage) const
    {
        switch (nStage) {
            case 9:
                return s_aPopupGroup9;
            case 17:
                return s_aPopupGroup17;
            default:
                return s_aPopupGroupDefault;
        }
    }

    // No panels in the party game.
    void CQPanelManVolley::SetUp()
    {
        if (CGame::g_GameWork.GetGameType() == 2) {
            return;
        }
        CQPanelMan::SetUp();
    }

    CQPanel *CQPanelManVolley::CreatePanel_(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, PANEL_POP_INFO *pInfo,
                                            long nIdx)
    {
        return new (0x100) CQPanelVolley(this, m_pScnGroup, rPos, rRot, pInfo, nIdx);
    }

    void CQPanelManVolley::SetupItemPopRate_()
    {
        SetItemPopRate_(0, s_ItemPopRateTbl0);
        SetItemPopRate_(1, s_ItemPopRateTbl1);
    }

    // By the score difference in the current set: row 0/1 when nTeam leads
    // by 10/5 points or more, 4/3 when it trails by as much, 2 otherwise.
    long CQPanelManVolley::ChoiceItemPopRateTblIdx_(long nTeam) const
    {
        long nPeriod = CGame::g_GameWork.GetPeriod();
        long nDiff = CGame::GetTeamPeriodScore(nTeam, nPeriod) - CGame::GetTeamPeriodScore(1 - nTeam, nPeriod);
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

    CQPanelVolley::~CQPanelVolley()
    {
    }

    float CQPanelVolley::GetSize() const
    {
        return 1.09f;
    }

    long CQPanelVolley::DoAccess(const bcs::FVEC3 &rPos, long nChr, unsigned long nFlag, long &rItem, CItem **ppItem)
    {
        long ret = 0;

        DoCmnSendMsg_(nChr, -1);
        if (m_nUnk88 == 0) {
            bcs::FVEC3 pos = m_Pos;
            int bNoItem1 = nFlag & 2;
            int bArg = (nFlag >> 3) & 1;
            int bNoItem2 = (nFlag >> 2) & 1;

            if (bNoItem1 || bNoItem2) {
                rItem = -1;
            } else if (rItem < 0) {
                rItem = m_pMan->ChoicePopupItem(nChr, bArg);
            }
            ret = (rItem >= 0) ? 2 : 1;
            if (rItem >= 0 && m_pMan->IsItemCreate()) {
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

    // The times below are in seconds; m_pMan's level picks them.
    float CQPanelVolley::GetLifeTime() const
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

    float CQPanelVolley::GetFirstPopupWaitTime() const
    {
        float fBase = 20.0f;
        float fRand = 5.0f;

        switch (m_pMan->GetLevel()) {
            case 1:
                fBase = 20.0f;
                fRand = 5.0f;
                break;
            case 2:
                fBase = 10.0f;
                fRand = 5.0f;
                break;
            case 3:
                fBase = 2.0f;
                fRand = 2.0f;
                break;
            case 4:
                fBase = 2.0f;
                fRand = 4.0f;
                break;
        }
        return CalcPopupTime(fBase, fRand);
    }

    float CQPanelVolley::GetNextPopupWaitTime() const
    {
        float fBase = 60.0f;
        float fRand = 0.0f;

        switch (m_pMan->GetLevel()) {
            case 1:
                fBase = 60.0f;
                fRand = 0.0f;
                break;
            case 2:
                fBase = 30.0f;
                fRand = 0.0f;
                break;
            case 3:
                fBase = 6.0f;
                fRand = 0.0f;
                break;
            case 4:
                fBase = 1.0f;
                fRand = 4.0f;
                break;
        }
        return CalcPopupTime(fBase, fRand);
    }
}
