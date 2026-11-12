#ifndef CMENUPARTBUTTON_HPP_
#define CMENUPARTBUTTON_HPP_

#include "game/msw/CMenuPartBase.hpp"

namespace msw {
    class CSelectMenuLayoutBase;

    // Size 0x90 (the size the SelectMenu screens allocate). Lives in MenuF.
    // A button that reports its animations to the screen that owns it.
    // Only what the screens need so far.
    class CMenuPartButton : public CMenuPartBase {
        public:
            CMenuPartButton(long nId);
            virtual ~CMenuPartButton();
            virtual long SetButtonIn();
            virtual long SetButtonOut();
            virtual long SetButtonPush();
            virtual long SetButtonPushCancel();
            virtual long SetButtonMultiIn(long);
            virtual long SetButtonMultiOut(long);

            void SetCallBackAnimIn(CSelectMenuLayoutBase *pLayout);
            void SetCallBackAnimOut(CSelectMenuLayoutBase *pLayout);
            void SetCallBackAnimPush(CSelectMenuLayoutBase *pLayout);
            void SetCallBackAnimCancel(CSelectMenuLayoutBase *pLayout);
            void SetCallBackAnimMultiIn(CSelectMenuLayoutBase *pLayout);
            void SetCallBackAnimMultiOut(CSelectMenuLayoutBase *pLayout);

        private:
            // 0x48: six 0xC-byte callback objects, one per SetCallBack
            // function above (std::tr1::function in the game; the
            // constructor clears the first word of each).
            unsigned long m_CallBack[6][3];
    };
}

#endif /* !CMENUPARTBUTTON_HPP_ */
