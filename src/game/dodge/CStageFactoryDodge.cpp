#include "game/msw/CStageFactoryDodge.hpp"
#include "game/msw/CStageDodge.hpp"

namespace msw {

// Stage numbers without a class of their own (0, 6, 8, 10, 14, 18 to 54
// and anything past 57) get the plain dodge stage.
CStageManDodge *CStageFactoryDodge::Create(long stageNo, bcs::CTask *pParent) {
    CStageManDodge *pStage;

    switch (stageNo) {
        case 1:
            pStage = new CStageDodge01(pParent);
            break;
        case 2:
            pStage = new CStageDodge02(pParent);
            break;
        case 3:
            pStage = new CStageDodge03(pParent);
            break;
        case 4:
            pStage = new CStageDodge04(pParent);
            break;
        case 5:
            pStage = new CStageDodge05(pParent);
            break;
        case 7:
            pStage = new CStageDodge07(pParent);
            break;
        case 9:
            pStage = new CStageDodge09(pParent);
            break;
        case 11:
            pStage = new CStageDodge11(pParent);
            break;
        case 12:
            pStage = new CStageDodge12(pParent);
            break;
        case 13:
            pStage = new CStageDodge13(pParent);
            break;
        case 15:
            pStage = new CStageDodge15(pParent);
            break;
        case 16:
            pStage = new CStageDodge16(pParent);
            break;
        case 17:
            pStage = new CStageDodge17(pParent);
            break;
        case 55:
            pStage = new CStageDodgeDOD55(pParent);
            break;
        case 56:
            pStage = new CStageDodgeDOD56(pParent);
            break;
        case 57:
            pStage = new CStageDodgeDOD57(pParent);
            break;
        default:
            pStage = new CStageManDodge(pParent);
            break;
    }

    if (pStage != 0) {
        pStage->SetStageNo(stageNo);
        pStage->Initialize_();
    }
    return pStage;
}

} // namespace msw
