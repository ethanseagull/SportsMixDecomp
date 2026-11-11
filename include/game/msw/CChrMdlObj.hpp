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

            // The trigger callback of the model's motion; pUser is the model.
            static void CallbackMotTrg(const bcs::CAnmObjMotBase::MOT_TRG *pTrg, void *pUser);

        private:
            unsigned char m_Unk1C8[0x78]; // 0x1C8, not mapped
    };
}

#endif /* !MSW_CCHRMDLOBJ_HPP_ */
