#ifndef CCOCKPITRESULT_HPP_
#define CCOCKPITRESULT_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"
#include "game/msw/CCockpitResultTime.hpp"

namespace msw {

    // The result screen's team page. Lives in the DOL (layout
    // "result_a.brlyt"; its file runs from before 0x8019A004 to the
    // destructor thunks at 0x8019B244, which adjust by 0x1C and 0x60: the
    // CMenuLytObj part and CCockpitResultTime). Only what has been needed so
    // far; the size is not known.
    class CCockpitResult : public CCockpitLayoutBase, public CCockpitResultTime {
        public:
            // (DOL fn_8019A344, not written yet)
            virtual ~CCockpitResult();
            virtual int Set();
            virtual void Remove();
            // (fn_8019AF20) As the base: sets m_nState to 4, returns 0.
            virtual long CancelMenuImpl();

            // Returns 0.
            long SetTitleText(const wchar_t *pText);
            // (fn_8019AF30, name guessed) Switches the four score text boxes
            // to the "_01_B" font when bColor is set, else to "_02_B",
            // keeping their size.
            void ChangeTextFont(int bColor);
            // (fn_8019B0CC, name guessed) Queues the result buttons, kind
            // 0x20 when team 0 won (nScore0 > nScore1), 0x21 when it lost,
            // 0x22 for a draw.
            void SetResultButton(long nScore0, long nScore1);
            void SetDelayResultButton();
    };

    namespace cockpit {

        // (names guessed) The display tasks CCockpitResult queues, size 0x38
        // each. Defined in the DOL (constructors fn_8023D858 and
        // fn_8023D74C, vtables lbl_8048BCF8 and lbl_8048BD0C).
        class CResultButtonTask : public menu::CDispTaskLink {
            public:
                CResultButtonTask();

                float m_fDelay;     // 0x30
                long m_nButton;     // 0x34, 0x20 to 0x22
        };

        class CDelayResultButtonTask : public menu::CDispTaskLink {
            public:
                CDelayResultButtonTask();

                float m_fDelay;     // 0x30
                unsigned char m_Unk34[0x4];
        };
    }
}

#endif /* !CCOCKPITRESULT_HPP_ */
