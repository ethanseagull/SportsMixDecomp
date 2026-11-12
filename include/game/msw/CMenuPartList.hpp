#ifndef CMENUPARTLIST_HPP_
#define CMENUPARTLIST_HPP_

#include "nw4r/ut/List.hpp"

namespace bcs {
    struct FVEC3;
}

namespace msw {
    class CMenuPaneAccessor;
    class CMenuPartBase;

    // Lives in the DOL. Size 0x1C (the size its owners allocate), vptr at
    // 0x18; the destructor is the first virtual (seen in
    // CSelectMenuLayoutBase::DeleteMenuPartList). The buttons of one menu
    // screen. Only the members used so far; member names are guesses.
    //
    // The same class as in game/msw/pending/CMenuPartList.hpp (TourF
    // session), whose additions are merged below; that copy can go.
    class CMenuPartList {
        public:
            CMenuPartList();
            void Add(CMenuPartBase *pPart);
            void StateCheck();
            int IsPushButtonAnimEnd();

            // The part whose current kind id is nBtnKindId, or null.
            CMenuPartBase *FindObjectByCurrentBtnKindId(unsigned short nBtnKindId);
            // (added for game/menu/CEntry.cpp; return type guessed) The
            // part with id nId (its index in the list), or null.
            CMenuPartBase *FindObject(long nId);
            // The current kind id of the button under *pPos, or 0. The
            // return type is a guess (the function loads a 16-bit member).
            long CheckHitButton(bcs::FVEC3 *pPos);
            // The kind id the stick moves to from nBtnKindId (nBtnKindId
            // itself when there is none). The name of the last argument is
            // a guess: callers pass CMenuPad::GetInput(nPad).
            long CheckStckMoveButton(long nPad, unsigned short nBtnKindId, unsigned long nInput);
            // (added for game/menu/CTopMenu.cpp; return type and meaning
            // guessed, after CheckStckMoveButton) The kind id the stick
            // moves the cursor to from nBtnKindId; 0x8000 for none.
            unsigned short StickMovementJudgmentCursor(long nPad, unsigned short nBtnKindId, unsigned long nInput);
            // Returns 0 on the path read; the return type and the argument
            // names are guesses.
            long CheckButtonAnim(const unsigned short *pPushButtonId, unsigned long *pUnk);
            // -1 when no part has that kind id, -2 when it is disabled,
            // else the part's id.
            long SetPushButtonAnim(unsigned short nBtnKindId, long nPad);
            // (added for DOL game/msw/CMenuPartBase.cpp) The same for a
            // cancel: plays it on a pushed part. nPad is not used.
            long CancelPushButtonAnim(unsigned short nBtnKindId, long nPad);
            // The id of the part whose current kind id is nBtnKindId, or -1.
            long GetIndexByCurrentBtnKindId(unsigned short nBtnKindId);

        private:
            // (no symbols; names guessed) The stick search behind
            // CheckStckMoveButton (recursive, at most 16 steps) and
            // StickMovementJudgmentCursor, and whether the cursor of pad
            // nPad may stop on nBtnKindId.
            long SearchStickMove_(long nPad, unsigned short nBtnKindId, unsigned long nInput,
                                            unsigned short nStartBtnKindId, long nDepth);
            unsigned short JudgeStickMove_(long nPad, long nBtnKindId, unsigned long nInput,
                                           long nStartBtnKindId, long nDepth);
            long CanStopCursor_(long nPad, long nBtnKindId);

        public:

        private:
            nw4r::ut::List m_List; // 0x00: the parts, linked at their offset 0

        public:
            CMenuPaneAccessor *m_pPaneAccessor; // 0x0C, stored by the owner

        private:
            // (names guessed) Frames before CheckButtonAnim may play the
            // cursor sound / start a vibration again; counted down there.
            float m_fSeWait;  // 0x10
            float m_fVibWait; // 0x14

        public:
            // vptr at 0x18
            virtual ~CMenuPartList();
    };
}

#endif /* !CMENUPARTLIST_HPP_ */
