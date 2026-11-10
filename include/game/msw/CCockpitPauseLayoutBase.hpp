#ifndef CCOCKPITPAUSELAYOUTBASE_HPP_
#define CCOCKPITPAUSELAYOUTBASE_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Base of the pages of the pause menu. Lives in the DOL. Its constructor
    // calls CCockpitLayoutBase(long, float); no members of its own are seen
    // (the derived CCockpitPausePlayInfo writes 0x60).
    class CCockpitPauseLayoutBase : public CCockpitLayoutBase {
        public:
            CCockpitPauseLayoutBase(long nId, float fSortZ);
            virtual ~CCockpitPauseLayoutBase();

            // Not zero once the layout has been built (m_pPaneAccessor is
            // set). Return type guessed: the DOL body returns 0 or 1.
            int hasLayoutObject();
    };

    // A page of the pause menu that shows how to play. Size 0x68 (BossF
    // allocates it). Lives in the DOL. What nKind selects is not known: the
    // boss game passes 0 for its ball page and 1 for its normal page.
    class CCockpitPausePlayInfo : public CCockpitPauseLayoutBase {
        public:
            CCockpitPausePlayInfo(long nId, long nKind, float fSortZ);
            virtual ~CCockpitPausePlayInfo();
            virtual int Set();
            virtual void Remove();

        private:
            unsigned char m_Unk60[0x8]; // 0x60
    };

    // A page of the pause menu with a title and message texts. Size 0x64
    // (BossF allocates it). Lives in the DOL.
    class CCockpitPauseRuleInfo : public CCockpitPauseLayoutBase {
        public:
            CCockpitPauseRuleInfo(long nId, long nKind, float fSortZ);
            virtual ~CCockpitPauseRuleInfo();
            virtual int Set();
            virtual void Remove();

            void SetTitleText(const wchar_t *pText);
            void SetMessageText(long nLine, const wchar_t *pText);

        private:
            unsigned char m_Unk60[0x4]; // 0x60
    };
}

#endif /* !CCOCKPITPAUSELAYOUTBASE_HPP_ */
