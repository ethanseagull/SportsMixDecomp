#ifndef MSW_CCHRMDLOBJ_HPP_
#define MSW_CCHRMDLOBJ_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "bcs/CMdlObj.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "revolution/rfl.h"

namespace bcs {
    class CMdlShadow;
    class CScnGroup;
}

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
            // (DOL; used by msw::CEvDemoMdlChr) The face animation; whether
            // the model and its shadow are drawn.
            void SetFaceAnmType(long nType);
            void SetDispEnable(int bDisp, int bShadow);
            // (DOL fn_800F2EFC and fn_800F3010, unnamed; names and meaning
            // guessed) Build the model of an event demo for character nChr
            // from file `file`; msw::CEvDemoMdlChr::CreateModel uses the
            // second for type 2 (a Mii, it seems). pInit is the setup's
            // first word.
            void CreateEventObj(nw4r::g3d::ResFile file, long nChr, void *pInit, long);
            void CreateEventMiiObj(nw4r::g3d::ResFile file, long nChr, void *pInit, long);
            // (DOL fn_800F4A40, unnamed; name guessed) Adds the model to
            // group pGroup.
            void AppendToGroup(bcs::CScnGroup *pGroup);
            // (name guessed) The model's shadow, or null.
            bcs::CMdlShadow *GetShadow() const { return m_pShadow; }

            // (DOL; added for MenuF's game/menu/CTopMenuChrObj.cpp; names of
            // the arguments and return types guessed)
            // Builds the model of the top menu's character of kind nKind.
            void CreateTopMenuObj(nw4r::g3d::ResFile file, long nKind, long);
            // Builds a Mii's model for the player info screens.
            void CreatePlayerInfoObj(nw4r::g3d::ResFile file, RFLStoreData &rData, unsigned long nFlag, long);
            // (SetDispEnable, declared above: MenuF's callers pass 1 for
            // bShadow. SetFaceAnmType, above: the face expression, 1 to 7.)
            // The gold version of the model.
            void EnableGoldModel(int bGold);
            // The size of a Mii's head.
            void SetMiiHeadScale(float fScale);

            // The trigger callback of the model's motion; pUser is the model.
            static void CallbackMotTrg(const bcs::CAnmObjMotBase::MOT_TRG *pTrg, void *pUser);

        private:
            unsigned char m_Unk1C8[0x2C]; // 0x1C8, not mapped
            bcs::CMdlShadow *m_pShadow;   // 0x1F4 (name guessed)
            unsigned char m_Unk1F8[0x48]; // 0x1F8, not mapped
    };
}

#endif /* !MSW_CCHRMDLOBJ_HPP_ */
