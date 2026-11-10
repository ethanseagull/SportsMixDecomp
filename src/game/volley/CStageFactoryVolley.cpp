#include "game/msw/CStageFactoryVolley.hpp"
#include "game/msw/CStageVolley.hpp"

namespace msw {

// Stage numbers without a class of their own (0, 4, 7, 8, 14, 18 to 24, 26
// to 36, 39 to 41 and anything past 42) get the plain volley stage.
CStageManVolley *CStageFactoryVolley::Create(long stageNo, bcs::CTask *pParent) {
    CStageManVolley *pStage;

    switch (stageNo) {
        case 1:
        case 25:
            pStage = new CStageVolley01(pParent);
            break;
        case 2:
            pStage = new CStageVolley02(pParent);
            break;
        case 3:
            pStage = new CStageVolley03(pParent);
            break;
        case 5:
            pStage = new CStageVolley05(pParent);
            break;
        case 6:
            pStage = new CStageVolley06(pParent);
            break;
        case 9:
            pStage = new CStageVolley09(pParent);
            break;
        case 10:
        case 37:
        case 38:
            pStage = new CStageVolley10(pParent, 4);
            break;
        case 11:
            pStage = new CStageVolley11(pParent);
            break;
        case 12:
            pStage = new CStageVolley12(pParent);
            break;
        case 13:
            pStage = new CStageVolley13(pParent);
            break;
        case 15:
            pStage = new CStageVolley15(pParent);
            break;
        case 16:
            pStage = new CStageVolley16(pParent);
            break;
        case 17:
            pStage = new CStageVolley17(pParent);
            break;
        case 42:
            pStage = new CStageVolley42(pParent);
            break;
        default:
            pStage = new CStageManVolley(pParent);
            break;
    }

    if (pStage != 0) {
        pStage->SetStageNo(stageNo);
        pStage->Initialize_();
    }
    return pStage;
}

} // namespace msw
