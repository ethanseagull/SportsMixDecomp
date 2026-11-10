#include "game/msw/CStageFactoryBasket.hpp"
#include "game/msw/CStageBasket.hpp"

namespace msw {

// Stage numbers without a class of their own (0, 4, 8, 12, 14, 18 to 24,
// 27 to 30, 32 to 69 and anything past 72) get the plain basket stage.
CStageManBasket *CStageFactoryBasket::Create(long stageNo, bcs::CTask *pParent) {
    CStageManBasket *pStage;

    switch (stageNo) {
        case 1:
            pStage = new CStageBasket01(pParent);
            break;
        case 2:
            pStage = new CStageBasket02(pParent);
            break;
        case 3:
            pStage = new CStageBasket03(pParent);
            break;
        case 5:
        case 25:
            pStage = new CStageBasket05(pParent);
            break;
        case 6:
        case 26:
            pStage = new CStageBasket06(pParent);
            break;
        case 7:
            pStage = new CStageBasket07(pParent);
            break;
        case 9:
            pStage = new CStageBasket09(pParent);
            break;
        case 10:
            pStage = new CStageBasket10(pParent, -1);
            break;
        case 11:
            pStage = new CStageBasket11(pParent);
            break;
        case 13:
            pStage = new CStageBasket13(pParent);
            break;
        case 15:
            pStage = new CStageBasket15(pParent);
            break;
        case 16:
            pStage = new CStageBasket16(pParent);
            break;
        case 17:
            pStage = new CStageBasket17(pParent);
            break;
        case 31:
            pStage = new CStageManTournament(pParent);
            break;
        case 70:
            pStage = new CStageBasketHungryBossPackun70(pParent);
            break;
        case 71:
            pStage = new CStageBasketHungryBossPackun71(pParent);
            break;
        case 72:
            pStage = new CStageBasketHungryBossPackun72(pParent);
            break;
        default:
            pStage = new CStageManBasket(pParent);
            break;
    }

    if (pStage != 0) {
        pStage->SetStageNo(stageNo);
        pStage->Initialize_();
    }
    return pStage;
}

} // namespace msw
