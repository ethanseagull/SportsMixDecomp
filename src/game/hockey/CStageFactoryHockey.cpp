#include "game/msw/CStageFactoryHockey.hpp"
#include "game/msw/CStageHockey.hpp"

namespace msw {

// Stage numbers without a class of their own (0, 6, 8, 14, 18 to 84 and
// anything past 87) get the plain hockey stage.
CStageManHockey *CStageFactoryHockey::Create(long stageNo, bcs::CTask *pParent) {
    CStageManHockey *pStage;

    switch (stageNo) {
        case 1:
            pStage = new CStageHockey01(pParent);
            break;
        case 2:
            pStage = new CStageHockey02(pParent);
            break;
        case 3:
            pStage = new CStageHockey03(pParent);
            break;
        case 4:
            pStage = new CStageHockey04(pParent);
            break;
        case 5:
            pStage = new CStageHockey05(pParent);
            break;
        case 7:
            pStage = new CStageHockey07(pParent);
            break;
        case 9:
            pStage = new CStageHockey09(pParent);
            break;
        case 10:
            pStage = new CStageHockey10(pParent, -1);
            break;
        case 11:
            pStage = new CStageHockey11(pParent);
            break;
        case 12:
            pStage = new CStageHockey12(pParent);
            break;
        case 13:
            pStage = new CStageHockey13(pParent);
            break;
        case 15:
            pStage = new CStageHockey15(pParent);
            break;
        case 16:
            pStage = new CStageHockey16(pParent);
            break;
        case 17:
            pStage = new CStageHockey17(pParent);
            break;
        case 85:
            pStage = new CStageHockeyCrushSkate85(pParent);
            break;
        case 86:
            pStage = new CStageHockeyCrushSkate86(pParent);
            break;
        case 87:
            pStage = new CStageHockeyCrushSkate87(pParent);
            break;
        default:
            pStage = new CStageManHockey(pParent);
            break;
    }

    if (pStage != 0) {
        pStage->SetStageNo(stageNo);
        pStage->Initialize_();
    }
    return pStage;
}

} // namespace msw
