#ifndef CMENUPARTLIST_HPP_
#define CMENUPARTLIST_HPP_

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
