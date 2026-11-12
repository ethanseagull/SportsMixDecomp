#ifndef CCOCKPITMENUBASE_HPP_
#define CCOCKPITMENUBASE_HPP_

#include "game/msw/CCockpitButtonInfo.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"
#include "game/msw/CCockpitPartsInfo.hpp"
#include "game/msw/CCockpitWindowInfo.hpp"

namespace msw {

    // Size 0x98. Base of the in-game menus (windows with buttons). Lives in
    // the DOL. CCockpitLayoutBase is at 0, the three info arrays at 0x60,
    // 0x6C and 0x78 (vptrs at 0x68, 0x74 and 0x80), own members from 0x84.
    // The layout is read from the constructor and the destructor. The virtual
    // functions are in the order of the vtable (lbl_80485F00); return types
    // are guesses.
    class CCockpitMenuBase : public CCockpitLayoutBase,
                             public CCockpitButtonInfo,
                             public CCockpitWindowInfo,
                             public CCockpitPartsInfo {
        public:
            // In the DOL at 0x801EDAC8, without a name: no module calls it.
            // The three longs are the sizes of the info arrays.
            CCockpitMenuBase(long nId, float fSortZ, long nButtonNum, long nWindowNum, long nPartsNum);
            virtual ~CCockpitMenuBase();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Remove();
            virtual long StickMoveCheck(long, unsigned short);
            virtual long CancelMenu(); // returns 0 (the original sets r3)
            virtual long PushButton(long);

            // Inline: the modules emit it.
            virtual void SetMessageText(long, const wchar_t *) {}
            virtual void WriteText();
            virtual void Setup();
            virtual void MenuOut();
            // Calls StepIn, StepLoop or StepOut for m_nStep 1, 2 or 3.
            virtual void StepControl();
            virtual void StepIn();
            virtual void StepLoop();
            virtual void StepOut();
            // Called by Main after StepControl; empty here.
            virtual void StateControl();
            virtual void InitMenuPadParameter(unsigned char);
            virtual void EndMenuPadParameter(long);

            // (added for BasketF game/basket/CCockpitBasket.cpp; return type
            // guessed) Whether the menu has gone out.
            int IsMenuOut();

        protected:
            unsigned long m_Unk84; // 0x84, not set by the constructor
            unsigned long m_Unk88; // 0x88
            float m_Unk8C;         // 0x8C, counted down by Main
            long m_Unk90;          // 0x90
            long m_Unk94;          // 0x94
    };
}

#endif /* !CCOCKPITMENUBASE_HPP_ */
