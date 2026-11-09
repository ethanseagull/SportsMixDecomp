#ifndef CEFFECTCTRLMENU_HPP_
#define CEFFECTCTRLMENU_HPP_

#include "game/msw/CEffectCtrl.hpp"

namespace msw {

    // The effect controller of the menus. Adds no data.
    class CEffectCtrlMenu : public CEffectCtrl {
        public:
            CEffectCtrlMenu(bcs::CTask *pParent) : CEffectCtrl(pParent)
            {
                AttachResource();
                InitializeDataTable();
            }

            virtual ~CEffectCtrlMenu();
            virtual void Main();
            virtual void Disp();
            virtual void InitializeDataTable();
            virtual int InitPatch(long, bcs::EF::CEffect *);

            // Returns the instance, creating it if there is none yet.
            static CEffectCtrl *CreateInstance(bcs::CTask *pParent);
            // Registers the menu's effect archives that have finished loading.
            static void AttachResource();
    };
}

#endif /* !CEFFECTCTRLMENU_HPP_ */
