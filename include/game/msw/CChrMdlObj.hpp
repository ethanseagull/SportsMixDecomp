#ifndef MSW_CCHRMDLOBJ_HPP_
#define MSW_CCHRMDLOBJ_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "bcs/CMdlObj.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "revolution/rfl.h"

namespace msw {

    // A character's model (player characters and Miis). A DOL class derived
    // from bcs::CMdlObj, size 0x240 (msw::CChrMenuCtrl::CreateMdlObj in MenuF
    // allocates 0x240). Only what the modules use so far is declared.
    class CChrMdlObj : public bcs::CMdlObj {
        public:
            CChrMdlObj();

            // Builds the model for the menus from file `file` for character
            // nChr. (meanings of the last two arguments not known; MenuF's
            // CChrMenuCtrl passes a word of its caller and 0; return type
            // guessed)
            void CreateMenuObj(nw4r::g3d::ResFile file, long nChr, RFLDataSource source, long, long);
            // (parameter name guessed) Sets the layer the model is drawn in.
            void SetDrawLayer(long nLayer);
            // Hides or shows a Mii's face. (return type guessed)
            void SetMiiNoFace(int bNoFace);

            // (DOL; added for MenuF's game/menu/CTopMenuChrObj.cpp; names of
            // the arguments and return types guessed)
            // Builds the model of the top menu's character of kind nKind.
            void CreateTopMenuObj(nw4r::g3d::ResFile file, long nKind, long);
            // Builds a Mii's model for the player info screens.
            void CreatePlayerInfoObj(nw4r::g3d::ResFile file, RFLStoreData &rData, unsigned long nFlag, long);
            // Shows or hides the model (meaning of the second argument not
            // known; 1 in both callers).
            void SetDispEnable(int bDisp, int);
            // The expression of the face (1 to 7).
            void SetFaceAnmType(long nType);
            // The gold version of the model.
            void EnableGoldModel(int bGold);
            // The size of a Mii's head.
            void SetMiiHeadScale(float fScale);

            // The trigger callback of the model's motion; pUser is the model.
            static void CallbackMotTrg(const bcs::CAnmObjMotBase::MOT_TRG *pTrg, void *pUser);

        private:
            unsigned char m_Unk1C8[0x78]; // 0x1C8, not mapped
    };
}

#endif /* !MSW_CCHRMDLOBJ_HPP_ */
