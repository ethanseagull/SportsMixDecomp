#ifndef CMENUPARTLIST_HPP_
#define CMENUPARTLIST_HPP_

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
            // Returns 0 on the path read; the return type and the argument
            // names are guesses.
            long CheckButtonAnim(const unsigned short *pPushButtonId, unsigned long *pUnk);
            // -1 when no part has that kind id, -2 when it is disabled,
            // else the part's id.
            long SetPushButtonAnim(unsigned short nBtnKindId, long nPad);

        private:
            unsigned char m_Unk00[0xC]; // 0x00: a nw4r::ut::List of CMenuPartBase

        public:
            CMenuPaneAccessor *m_pPaneAccessor; // 0x0C, stored by the owner

        private:
            float m_Unk10; // 0x10
            float m_Unk14; // 0x14

        public:
            // vptr at 0x18
            virtual ~CMenuPartList();
    };
}

#endif /* !CMENUPARTLIST_HPP_ */
