#include "game/msw/SelectMenu/CTrofy.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/dwc/CFriend.hpp"
#include "nw4r/lyt/Pane.hpp"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    const char *const s_LayoutName = "me_WiFi_trofy.brlyt";

    // The trophy panes: four grades for each sport.
    enum {
        GRADE_BRONZE,
        GRADE_SILVER,
        GRADE_GOLD,
        GRADE_PLATINUM,
        GRADE_NUM
    };

    enum {
        SPORT_NUM = 4
    };

    // The first pane of each sport's trophies in s_PaneNames, in the
    // order of the sport index (basketball, volleyball, dodgeball, hockey).
    long s_TrofyPaneTop[SPORT_NUM] = { 0, 8, 4, 12 };

    const char *s_PaneNames[SPORT_NUM * GRADE_NUM] = {
        "BA_bronze", "BA_silver", "BA_gold", "BA_platinum",
        "DO_bronze", "DO_silver", "DO_gold", "DO_platinum",
        "VO_bronze", "VO_silver", "VO_gold", "VO_platinam",
        "HO_bronze", "HO_silver", "HO_gold", "HO_platinum"
    };

    // The rating each grade needs.
    enum {
        RATE_BRONZE = 1000,
        RATE_SILVER = 2000,
        RATE_GOLD = 4000,
        RATE_PLATINUM = 8000
    };

    // The sparkle effect of each grade.
    enum {
        EFFECT_BRONZE = 0x463,
        EFFECT_SILVER = 0x464,
        EFFECT_GOLD = 0x465,
        EFFECT_PLATINUM = 0x466
    };

    // The sparkles started by SetWiFiTrofyEffect, one per sport.
    unsigned long s_hEffect[SPORT_NUM];
}

msw::SelectMenu::CTrofy::CTrofy(long nId)
    : CSelectMenuLayoutBase(nId)
{
}

msw::SelectMenu::CTrofy::~CTrofy()
{
    Remove();
}

void msw::SelectMenu::CTrofy::Main()
{
}

void msw::SelectMenu::CTrofy::Disp()
{
}

int msw::SelectMenu::CTrofy::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(pSelectMenu->GetLoadArchiveFileId(-1));
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            SetPaneAccessor(pPaneCtrl, s_PaneNames, SPORT_NUM * GRADE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(110.0f);
        }

        // Every trophy starts hidden; GetWiFiTrofyPane shows them.
        for (long i = 0; i < SPORT_NUM * GRADE_NUM; i++) {
            m_pPaneAccessor->SetPaneDisp(i, 0);
        }
        for (long i = 0; i < SPORT_NUM; i++) {
            s_hEffect[i] = 0;
        }
        m_pFriend = 0;
    }
    return 0;
}

void msw::SelectMenu::CTrofy::Remove()
{
    ClearTrofyEffect();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
}

bcs::G2D::CPaneObj *msw::SelectMenu::CTrofy::GetWiFiTrofyPane(long nIndex, long &rEffect)
{
    long nRate = 0;

    if (m_pFriend != 0) {
        dwc::CFriend::StorageData *pData = m_pFriend->GetOwnStorageData();
        switch (nIndex) {
            case 0:
                nRate = pData->m_Skill[0].m_nMaxRate;
                break;
            case 1:
                nRate = pData->m_Skill[1].m_nMaxRate;
                break;
            case 2:
                nRate = pData->m_Skill[2].m_nMaxRate;
                break;
            case 3:
                nRate = pData->m_Skill[3].m_nMaxRate;
                break;
        }
    } else {
        nRate = CSave::GetPlayerData(-1)->GetMaxSkill(nIndex);
    }

    bcs::G2D::CPaneObj *pPaneObj = 0;
    if (nRate >= RATE_PLATINUM) {
        long nPane = s_TrofyPaneTop[nIndex] + GRADE_PLATINUM;
        pPaneObj = m_pPaneAccessor->GetPaneObj(nPane);
        m_pPaneAccessor->SetPaneDisp(nPane, 1);
        rEffect = EFFECT_PLATINUM;
    } else if (nRate >= RATE_GOLD) {
        long nPane = s_TrofyPaneTop[nIndex] + GRADE_GOLD;
        pPaneObj = m_pPaneAccessor->GetPaneObj(nPane);
        m_pPaneAccessor->SetPaneDisp(nPane, 1);
        rEffect = EFFECT_GOLD;
    } else if (nRate >= RATE_SILVER) {
        long nPane = s_TrofyPaneTop[nIndex] + GRADE_SILVER;
        pPaneObj = m_pPaneAccessor->GetPaneObj(nPane);
        m_pPaneAccessor->SetPaneDisp(nPane, 1);
        rEffect = EFFECT_SILVER;
    } else if (nRate >= RATE_BRONZE) {
        long nPane = s_TrofyPaneTop[nIndex] + GRADE_BRONZE;
        pPaneObj = m_pPaneAccessor->GetPaneObj(nPane);
        m_pPaneAccessor->SetPaneDisp(nPane, 1);
        rEffect = EFFECT_BRONZE;
    }
    return pPaneObj;
}

void msw::SelectMenu::CTrofy::SetWiFiTrofy(long nIndex, bcs::G2D::CPaneObj *pPaneObj)
{
    nw4r::math::VEC3 pos(0.0f, 0.0f, 0.0f);
    long nEffect = -1;

    bcs::G2D::CPaneObj *pTrofy = GetWiFiTrofyPane(nIndex, nEffect);
    if (pTrofy != 0) {
        // Hang the trophy under pPaneObj, at its origin.
        bcs::G2D::SetPaneParent(pTrofy->GetPane(), pPaneObj->GetPane(), 1, 0);
        pTrofy->GetPane()->SetTranslate(pos);
    }
}

void msw::SelectMenu::CTrofy::SetWiFiTrofyEffect(long nIndex, bcs::G2D::CPaneObj *pPaneObj)
{
    long nEffect = -1;
    bcs::G2D::CPaneObj *pTrofy = GetWiFiTrofyPane(nIndex, nEffect);
    if (nEffect >= 0) {
        s_hEffect[nIndex] = CEffectCtrl::CreateAttachEffect(nEffect, pTrofy, 0, -1, 0, 0, 0, 4, 0, 0);
    }
}

void msw::SelectMenu::CTrofy::ClearTrofyEffect()
{
    for (long i = 0; i < SPORT_NUM; i++) {
        if (s_hEffect[i] != 0) {
            CEffectCtrl::EraseAttachEffect(s_hEffect[i]);
        }
    }
}
