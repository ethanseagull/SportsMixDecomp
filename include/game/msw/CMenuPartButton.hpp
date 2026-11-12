#ifndef CMENUPARTBUTTON_HPP_
#define CMENUPARTBUTTON_HPP_

#include <functional>

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
            // 0x48: what the Set* functions above report to, each bound by
            // the matching SetCallBackAnim* to the owner screen's
            // CallbackButtonAnim* virtual. Called with
            // m_nNextBtnKindId[0] (the Multi ones with their argument
            // first). 0xC bytes each.
            std::tr1::function<void (long)> m_CallBackAnimIn;            // 0x48
            std::tr1::function<void (long)> m_CallBackAnimOut;           // 0x54
            std::tr1::function<void (long)> m_CallBackAnimPush;          // 0x60
            std::tr1::function<void (long)> m_CallBackAnimCancel;        // 0x6C
            std::tr1::function<void (long, long)> m_CallBackAnimMultiIn;  // 0x78
            std::tr1::function<void (long, long)> m_CallBackAnimMultiOut; // 0x84
    };
}

#endif /* !CMENUPARTBUTTON_HPP_ */
