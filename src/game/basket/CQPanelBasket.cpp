#include "game/msw/CQPanelManBasket.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/New.hpp"
#include <math.h>

// The "?" panels of the basketball game: the manager's tables (item popup
// rates by score difference, panel places by stage) and the panel's access
// and timing.

namespace {

    using msw::CQPanelMan;

    // Rate table 1 (SetItemPopRate_ index 1).
    const CQPanelMan::ITEM_POPUP_RATE_TBL_TBL s_ItemPopRateTbl1 = { {
        { { 60, 10, 8, 8, 6, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 60, 10, 8, 8, 6, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 80, 5, 5, 4, 4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 80, 4, 4, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 80, 3, 3, 3, 3, 5, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    } };

    // Rate table 0 (SetItemPopRate_ index 0).
    const CQPanelMan::ITEM_POPUP_RATE_TBL_TBL s_ItemPopRateTbl0 = { {
        { { 40, 20, 10, 10, 10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 40, 20, 10, 10, 10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 40, 15, 13, 11, 10, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 40, 12, 13, 10, 12, 3, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 40, 10, 13, 10, 12, 5, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    } };

    // Every other stage.
    const CQPanelMan::POPUP_GROUP s_aPopupGroupDefault[6] = {
        { 0, -1, { 0, 1, 1, 2, 5 }, {
            { 1, 6.0f, -6.0f },
            { 1, 2.0f, -3.0f },
            { 1, -2.0f, -3.0f },
            { 1, -6.0f, -6.0f },
            { 1, 6.0f, 6.0f },
            { 1, 2.0f, 3.0f },
            { 1, -2.0f, 3.0f },
            { 1, -6.0f, 6.0f },
            { 1, 5.0f, 0.0f },
            { 1, -5.0f, 0.0f },
        } },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
    };

    // Stage 3.
    const CQPanelMan::POPUP_GROUP s_aPopupGroup3[6] = {
        { 0, -1, { 0, 1, 1, 2, 5 }, {
            { 1, 6.0f, -6.0f },
            { 1, 2.0f, -3.0f },
            { 1, -2.0f, -3.0f },
            { 1, -6.0f, -6.0f },
            { 1, 6.0f, 6.0f },
            { 1, 2.0f, 3.0f },
            { 1, -2.0f, 3.0f },
            { 1, -6.0f, 6.0f },
            { 1, 5.0f, 0.0f },
            { 1, -5.0f, 0.0f },
        } },
        { 1, -1, { 1, 1, 1, 1, 1 }, {
            { 1, 5.0f, -3.0f },
            { 1, -5.0f, -3.0f },
            { 1, 5.0f, 3.0f },
            { 1, -5.0f, 3.0f },
        } },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
    };

    // Stage 9.
    const CQPanelMan::POPUP_GROUP s_aPopupGroup9[6] = {
        { 0, -1, { 0, 1, 1, 2, 5 }, {
            { 5, 6.0f, -6.0f },
            { 5, 2.0f, -3.0f },
            { 5, -2.0f, -3.0f },
            { 5, -6.0f, -6.0f },
            { 5, 6.0f, 6.0f },
            { 5, 2.0f, 3.0f },
            { 5, -2.0f, 3.0f },
            { 5, -6.0f, 6.0f },
            { 5, 5.0f, 0.0f },
            { 5, -5.0f, 0.0f },
        } },
        { 2, -1, { 1, 1, 1, 2, 4 }, {
            { 37, 6.0f, -3.0f, 90.0f },
            { 37, 2.0f, -6.0f, 270.0f },
            { 37, -2.0f, -6.0f, 90.0f },
            { 37, -6.0f, -3.0f, 270.0f },
            { 37, 6.0f, 3.0f, 90.0f },
            { 37, 2.0f, 6.0f, 270.0f },
            { 37, -2.0f, 6.0f, 90.0f },
            { 37, -6.0f, 3.0f, 270.0f },
        } },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
    };

    // Stage 10.
    const CQPanelMan::POPUP_GROUP s_aPopupGroup10[6] = {
        { 0, -1, { 0, 1, 1, 1, 2 }, {
            { 1, 6.0f, -6.0f },
            { 1, -6.0f, -6.0f },
            { 1, 6.0f, 6.0f },
            { 1, -6.0f, 6.0f },
        } },
        { 0 },
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

    CQPanelManBasket::CQPanelManBasket(bcs::CTask *pParent) : CQPanelMan(pParent, 1)
    {
    }

    CQPanelManBasket::~CQPanelManBasket()
    {
        AllReset();
    }

    void CQPanelManBasket::SetUp()
    {
        CQPanelMan::SetUp();
    }

    CQPanel *CQPanelManBasket::CreatePanel_(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, PANEL_POP_INFO *pInfo,
                                            long nIdx)
    {
        return new (0x100) CQPanelBasket(this, m_pScnGroup, rPos, rRot, pInfo, nIdx);
    }

    const CQPanelMan::POPUP_GROUP *CQPanelManBasket::GetPopupGroupData_(long nStage) const
    {
        switch (nStage) {
            case 3:
                return s_aPopupGroup3;
            case 9:
                return s_aPopupGroup9;
            case 10:
                return s_aPopupGroup10;
            default:
                return s_aPopupGroupDefault;
        }
    }

    void CQPanelManBasket::SetupItemPopRate_()
    {
        SetItemPopRate_(0, s_ItemPopRateTbl0);
        SetItemPopRate_(1, s_ItemPopRateTbl1);
    }

    // Row 0/1 when nTeam leads by 20/10 points or more, 4/3 when it trails
    // by as much, 2 otherwise.
    long CQPanelManBasket::ChoiceItemPopRateTblIdx_(long nTeam) const
    {
        long nDiff = CGame::GetTeamScore(nTeam) - CGame::GetTeamScore(1 - nTeam);
        long nIdx = 2;

        if (nDiff >= 20) {
            nIdx = 0;
        } else if (nDiff >= 10) {
            nIdx = 1;
        } else if (nDiff <= -20) {
            nIdx = 4;
        } else if (nDiff <= -10) {
            nIdx = 3;
        }
        return nIdx;
    }

    CQPanelBasket::~CQPanelBasket()
    {
    }

    float CQPanelBasket::GetSize() const
    {
        return 1.09f;
    }

    long CQPanelBasket::DoAccess(const bcs::FVEC3 &rPos, long nChr, unsigned long nFlag, long &rItem, CItem **ppItem)
    {
        long ret = 0;

        DoCmnSendMsg_(nChr, -1);
        if (m_nUnk88 == 0) {
            bcs::FVEC3 pos = m_Pos;
            int bNoItem1 = nFlag & 2;
            int bArg = nFlag & 1;
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
    float CQPanelBasket::GetLifeTime() const
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

    float CQPanelBasket::GetFirstPopupWaitTime() const
    {
        float fBase = 10.0f;
        float fRand = 10.0f;

        switch (m_pMan->GetLevel()) {
            case 1:
                fBase = 10.0f;
                fRand = 10.0f;
                break;
            case 2:
                fBase = 4.0f;
                fRand = 6.0f;
                break;
            case 3:
                fBase = 1.0f;
                fRand = 4.0f;
                break;
            case 4:
                fBase = 1.0f;
                fRand = 4.0f;
                break;
        }
        return CalcPopupTime(fBase, fRand);
    }

    float CQPanelBasket::GetNextPopupWaitTime() const
    {
        float fBase = 48.0f;
        float fRand = 4.0f;

        switch (m_pMan->GetLevel()) {
            case 1:
                fBase = 48.0f;
                break;
            case 2:
                fBase = 10.0f;
                break;
            case 3:
                fBase = 8.0f;
                break;
            case 4:
                fBase = 1.0f;
                break;
        }
        return CalcPopupTime(fBase, fRand);
    }
}
