#ifndef MSW_CCOKCPITMENUDATALIST_HPP_
#define MSW_CCOKCPITMENUDATALIST_HPP_

#include "nw4r/ut/List.hpp"

namespace msw {

    // (purpose guessed) A stack of saved menu states: Push saves the four
    // pads' states and the cockpit's result words, Pop deletes the newest
    // entry, which puts them back. One static instance (GetInstance). Size
    // 0x10: the list, then the vptr at 0x0C. "Cokcpit" is the DOL's
    // spelling.
    class CCokcpitMenuDataList {
        public:
            // (name guessed; no symbol) One saved state. Size 0x1C, vptr at
            // 0x18 after the data.
            class CData {
                public:
                    CData();

                    nw4r::ut::Link m_Link;        // 0x00
                    unsigned short m_aPadState[4]; // 0x08, CMenuPadInfo's pad states
                    long m_nUnk70;                 // 0x10, CCockpit's word at 0x70
                    long m_nResult;                // 0x14, CCockpit's result

                    // Declared after the data: the vptr is at 0x18. Puts the
                    // saved state back.
                    virtual ~CData();
            };

            CCokcpitMenuDataList() { nw4r::ut::List_Init(&m_List, 0); }

            static CCokcpitMenuDataList *GetInstance();

            // (names guessed; no symbols) Initialize deletes every entry
            // and resets the list, Finalize only deletes them.
            void Initialize();
            void Finalize();

            // Saves the current state as the newest entry.
            void Push();
            // Deletes the newest entry, restoring its state.
            void Pop();

        private:
            CData *GetPrev(CData *pData)
            {
                return static_cast<CData *>(nw4r::ut::List_GetPrev(&m_List, pData));
            }

            // Deletes every entry, newest first.
            void DeleteAll()
            {
                CData *pData = GetPrev(0);
                while (pData != 0) {
                    CData *pPrev = GetPrev(pData);
                    nw4r::ut::List_Remove(&m_List, pData);
                    delete pData;
                    pData = pPrev;
                }
            }

            nw4r::ut::List m_List; // 0x00

        public:
            // Declared after the data: the vptr is at 0x0C.
            virtual ~CCokcpitMenuDataList();

        private:
            static CCokcpitMenuDataList s_Instance;
    };
}

#endif /* !MSW_CCOKCPITMENUDATALIST_HPP_ */
