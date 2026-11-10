#ifndef CINFOTABCHANGE_HPP_
#define CINFOTABCHANGE_HPP_

namespace msw {

    // Size 0xC, vptr at 0x08 (after the data). Steps through the tabs of an
    // info window, wrapping at both ends. A second base of
    // SelectMenu::CPlayerInfo (at 0x5C there: its constructor stores the tab
    // count and 0 before the vtables, and its vtable part has a destructor,
    // ChangeTab and SetVisibleArrow, each reached through an `@92@` thunk).
    // No vtable of this class exists in the game, so all of its virtuals
    // except the destructor are pure.
    class CInfoTabChange {
        public:
            // The constructor is inline (CPlayerInfo's has it inlined).
            CInfoTabChange(long nTabNum)
                : m_nTabNum(nTabNum)
                , m_nTabNo(0)
            {
            }

            // Both store the new tab number and return it.
            long IncTabNo(long nTabNo);
            long DecTabNo(long nTabNo);

            long m_nTabNum; // 0x00
            long m_nTabNo;  // 0x04

            // vptr at 0x08
            virtual ~CInfoTabChange() {}
            // Shows tab nTabNo; 0, or -1 when there is no layout (the
            // return types are CPlayerInfo's).
            virtual long ChangeTab(long nTabNo) = 0;
            // Shows or hides the arrow button nBtnKindId; 0, or -1 when
            // there is no such button.
            virtual long SetVisibleArrow(unsigned short nBtnKindId, int bVisible) = 0;
    };
}

#endif /* !CINFOTABCHANGE_HPP_ */
