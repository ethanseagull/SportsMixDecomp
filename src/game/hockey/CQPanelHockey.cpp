// msw::CQPanelManHockey and msw::CQPanelHockey: the "?" panels of the hockey
// game. The manager's tables (item popup rates by score difference, panel
// places by stage) and the panel's access and timing. The party stages
// (0x55 to 0x57) have their own rate table and panel places.
#include "game/msw/CQPanelManHockey.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/New.hpp"
#include <math.h>

namespace {

    using msw::CQPanelMan;

    // Rate table 1 (SetupItemPopRate_ index 1).
    const CQPanelMan::ITEM_POPUP_RATE_TBL_TBL s_ItemPopRateTbl1 = { {
        { { 60, 10, 8, 8, 6, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 60, 10, 8, 8, 6, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 80, 5, 5, 4, 4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 80, 4, 4, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 80, 3, 3, 3, 3, 5, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    } };

    // Rate table 0 (SetupItemPopRate_ index 0).
    const CQPanelMan::ITEM_POPUP_RATE_TBL_TBL s_ItemPopRateTbl0 = { {
        { { 40, 20, 10, 10, 10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 40, 20, 10, 10, 10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 40, 15, 13, 11, 10, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 40, 12, 13, 10, 12, 3, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 40, 10, 13, 10, 12, 5, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    } };

    // Both rate tables of the party stages 0x55 to 0x57.
    const CQPanelMan::ITEM_POPUP_RATE_TBL_TBL s_ItemPopRateTblParty = { {
        { { 0, 0, 0, 100, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 0, 0, 100, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 0, 0, 100, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 0, 0, 100, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        { { 0, 0, 0, 100, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    } };

    // Every other stage.
    const CQPanelMan::POPUP_GROUP s_aPopupGroupDefault[6] = {
        { 0, -1, { 0, 1, 1, 2, 8 }, {
            { 1, 8.0f, -12.0f },
            { 1, 5.0f, -9.0f },
            { 1, 2.0f, -6.0f },
            { 1, -2.0f, -6.0f },
            { 1, -5.0f, -9.0f },
            { 1, -8.0f, -12.0f },
            { 1, 8.0f, 12.0f },
            { 1, 5.0f, 9.0f },
            { 1, 2.0f, 6.0f },
            { 1, -2.0f, 6.0f },
            { 1, -5.0f, 9.0f },
            { 1, -8.0f, 12.0f },
            { 1, 7.0f, -3.0f },
            { 1, 5.0f, 0.0f },
            { 1, 7.0f, 3.0f },
            { 1, -7.0f, -3.0f },
            { 1, -5.0f, 0.0f },
            { 1, -7.0f, 3.0f },
        } },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
    };

    // Stage 3.
    const CQPanelMan::POPUP_GROUP s_aPopupGroup3[6] = {
        { 0, -1, { 0, 1, 1, 2, 8 }, {
            { 1, 8.0f, -12.0f },
            { 1, 5.0f, -9.0f },
            { 1, 2.0f, -6.0f },
            { 1, -2.0f, -6.0f },
            { 1, -5.0f, -9.0f },
            { 1, -8.0f, -12.0f },
            { 1, 8.0f, 12.0f },
            { 1, 5.0f, 9.0f },
            { 1, 2.0f, 6.0f },
            { 1, -2.0f, 6.0f },
            { 1, -5.0f, 9.0f },
            { 1, -8.0f, 12.0f },
            { 1, 7.0f, -3.0f },
            { 1, 5.0f, 0.0f },
            { 1, 7.0f, 3.0f },
            { 1, -7.0f, -3.0f },
            { 1, -5.0f, 0.0f },
            { 1, -7.0f, 3.0f },
        } },
        { 1, -1, { 1, 1, 1, 1, 1 }, {
            { 1, -5.0f, -12.0f },
            { 1, -8.0f, -9.0f },
            { 1, -5.0f, -6.0f },
            { 1, 5.0f, -12.0f },
            { 1, 8.0f, -9.0f },
            { 1, 5.0f, -6.0f },
            { 1, -5.0f, 12.0f },
            { 1, -8.0f, 9.0f },
            { 1, -5.0f, 6.0f },
            { 1, 5.0f, 12.0f },
            { 1, 8.0f, 9.0f },
            { 1, 5.0f, 6.0f },
        } },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
    };

    // Stage 10.
    const CQPanelMan::POPUP_GROUP s_aPopupGroup10[6] = {
        { 0, -1, { 0, 1, 1, 2, 6 }, {
            { 1, 4.0f, -9.0f },
            { 1, -4.0f, -9.0f },
            { 1, 8.0f, -4.0f },
            { 1, -8.0f, -4.0f },
            { 1, 5.0f, -2.0f },
            { 1, -5.0f, -2.0f },
            { 1, 5.0f, 2.0f },
            { 1, -5.0f, 2.0f },
            { 1, 8.0f, 4.0f },
            { 1, -8.0f, 4.0f },
            { 1, 4.0f, 9.0f },
            { 1, -4.0f, 9.0f },
        } },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
    };

    // Stage 9.
    const CQPanelMan::POPUP_GROUP s_aPopupGroup9[6] = {
        { 0, -1, { 0, 1, 1, 2, 6 }, {
            { 1, -8.0f, -9.0f },
            { 1, -3.0f, -9.0f },
            { 1, 3.0f, -9.0f },
            { 1, 8.0f, -9.0f },
            { 1, -6.0f, -3.0f },
            { 1, 6.0f, -3.0f },
            { 1, -6.0f, 3.0f },
            { 1, 6.0f, 3.0f },
            { 1, -8.0f, 9.0f },
            { 1, -3.0f, 9.0f },
            { 1, 3.0f, 9.0f },
            { 1, 8.0f, 9.0f },
        } },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
    };

    // Stage 17.
    const CQPanelMan::POPUP_GROUP s_aPopupGroup17[6] = {
        { 0, -1, { 0, 1, 1, 1, 2 }, {
            { 1, -8.0f, -13.0f },
            { 1, -3.0f, -13.0f },
            { 1, 3.0f, -13.0f },
            { 1, 8.0f, -13.0f },
        } },
        { 0, -1, { 0, 1, 1, 1, 2 }, {
            { 1, -8.0f, 13.0f },
            { 1, -3.0f, 13.0f },
            { 1, 3.0f, 13.0f },
            { 1, 8.0f, 13.0f },
        } },
        { 0, -1, { 0, 1, 1, 1, 2 }, {
            { 1, 0.0f, -7.0f, 0.0f, 0, 2, 0 },
            { 1, -7.0f, 0.0f, 90.0f, 0, 2, 0 },
            { 1, 7.0f, 0.0f, 270.0f, 0, 2, 0 },
            { 1, 0.0f, 7.0f, 180.0f, 0, 2, 0 },
        } },
        { 0 },
        { 0 },
        { 0 },
    };

    // Stage 0x55 (party).
    const CQPanelMan::POPUP_GROUP s_aPopupGroup85[6] = {
        { 0, -1, { 0, 1, 1, 1, 4 }, {
            { 1, -6.0f, -2.0f },
            { 1, -2.0f, -6.0f },
            { 1, 2.0f, -6.0f },
            { 1, 6.0f, -2.0f },
            { 1, -6.0f, 2.0f },
            { 1, -2.0f, 6.0f },
            { 1, 2.0f, 6.0f },
            { 1, 6.0f, 2.0f },
        } },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
    };

    // Stage 0x56 (party).
    const CQPanelMan::POPUP_GROUP s_aPopupGroup86[6] = {
        { 0, -1, { 0, 1, 1, 1, 4 }, {
            { 1, -5.0f, -2.0f },
            { 1, -2.0f, -5.0f },
            { 1, 2.0f, -5.0f },
            { 1, 5.0f, -2.0f },
            { 1, -5.0f, 2.0f },
            { 1, -2.0f, 5.0f },
            { 1, 2.0f, 5.0f },
            { 1, 5.0f, 2.0f },
        } },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
    };

    // Stage 0x57 (party).
    const CQPanelMan::POPUP_GROUP s_aPopupGroup87[6] = {
        { 0, -1, { 0, 1, 1, 1, 4 }, {
            { 1, -6.0f, -2.0f },
            { 1, -2.0f, -6.0f },
            { 1, 2.0f, -6.0f },
            { 1, 6.0f, -2.0f },
            { 1, -6.0f, 2.0f },
            { 1, -2.0f, 6.0f },
            { 1, 2.0f, 6.0f },
            { 1, 6.0f, 2.0f },
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

    CQPanelManHockey::CQPanelManHockey(bcs::CTask *pParent) : CQPanelMan(pParent, 1)
    {
    }

    CQPanelManHockey::~CQPanelManHockey()
    {
        AllReset();
    }

    void CQPanelManHockey::SetUp()
    {
        CQPanelMan::SetUp();
    }

    CQPanel *CQPanelManHockey::CreatePanel_(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, PANEL_POP_INFO *pInfo,
                                            long nIdx)
    {
        return new (0x100) CQPanelHockey(this, m_pScnGroup, rPos, rRot, pInfo, nIdx);
    }

    const CQPanelMan::POPUP_GROUP *CQPanelManHockey::GetPopupGroupData_(long nStage) const
    {
        switch (nStage) {
            case 3:
                return s_aPopupGroup3;
            case 10:
                return s_aPopupGroup10;
            case 17:
                return s_aPopupGroup17;
            case 9:
                return s_aPopupGroup9;
            case 0x55:
                return s_aPopupGroup85;
            case 0x56:
                return s_aPopupGroup86;
            case 0x57:
                return s_aPopupGroup87;
            default:
                return s_aPopupGroupDefault;
        }
    }

    // Flag bit 1 keeps the panels shut.
    long CQPanelManHockey::QPanelCheck_(const bcs::FVEC3 &rPos, long nChr, unsigned long nFlag, long *pItem,
                                        CItem **ppItem)
    {
        if (nFlag & 2) {
            return 0;
        }
        return CQPanelMan::QPanelCheck_(rPos, nChr, nFlag, pItem, ppItem);
    }

    void CQPanelManHockey::SetupItemPopRate_()
    {
        switch (CGame::g_GameWork.m_nStage) {
            case 0x55:
            case 0x56:
            case 0x57:
                SetItemPopRate_(0, s_ItemPopRateTblParty);
                SetItemPopRate_(1, s_ItemPopRateTblParty);
                break;
            default:
                SetItemPopRate_(0, s_ItemPopRateTbl0);
                SetItemPopRate_(1, s_ItemPopRateTbl1);
                break;
        }
    }

    // Row 0/1 when nTeam leads by 20/10 points or more, 4/3 when it trails
    // by as much, 2 otherwise.
    long CQPanelManHockey::ChoiceItemPopRateTblIdx_(long nTeam) const
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

    CQPanelHockey::~CQPanelHockey()
    {
    }

    float CQPanelHockey::GetSize() const
    {
        return 1.09f * m_fSizeScale;
    }

    // Unlike the other sports, the panel only chooses the item; it does not
    // create it.
    long CQPanelHockey::DoAccess(const bcs::FVEC3 &rPos, long nChr, unsigned long nFlag, long &rItem, CItem **ppItem)
    {
        long ret = 0;

        DoCmnSendMsg_(nChr, -1);
        if (m_nUnk88 == 0) {
            // (copied and not used)
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
        }
        m_nFlag &= ~(FLAG_REQ_POPUP | FLAG_CAN_ACCESS);
        ChangeState(3);
        return ret;
    }

    // The times below are in seconds; m_pMan's level picks them, and the
    // party game has its own.
    float CQPanelHockey::GetLifeTime() const
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

    float CQPanelHockey::GetFirstPopupWaitTime() const
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
                fRand = 4.0f;
                break;
            case 3:
                fBase = 1.0f;
                fRand = 3.0f;
                break;
            case 4:
                fBase = 1.0f;
                fRand = 4.0f;
                break;
        }
        if (CGame::g_GameWork.GetGameType() == 2) {
            fBase = 15.0f;
            fRand = 5.0f;
        }
        return CalcPopupTime(fBase, fRand);
    }

    float CQPanelHockey::GetNextPopupWaitTime() const
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
                fRand = 2.0f;
                break;
            case 4:
                fBase = 1.0f;
                break;
        }
        if (CGame::g_GameWork.GetGameType() == 2) {
            fBase = 20.0f;
            fRand = 10.0f;
        }
        return CalcPopupTime(fBase, fRand);
    }
}
