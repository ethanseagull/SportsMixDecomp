#ifndef CMENULYTOBJ_HPP_
#define CMENULYTOBJ_HPP_

#include "game/msw/CLytCtrl.hpp"

namespace msw {

    // Size 0xC + 4 * N. Owns up to N layout objects of type T registered
    // with CLytCtrl, by handle. Member and function names are guesses.
    // The constructor and destructor are inline (read from
    // CSelectMenuLayoutBase's; MenuF emits the destructor weak).
    template <class T, int N>
    class CMenuLytObj {
        public:
            CMenuLytObj()
                : m_nMax(N)
                , m_nNum(0)
            {
                for (int i = 0; i < N; i++) {
                    m_nHandle[i] = 0;
                }
            }

            unsigned long m_nHandle[N]; // 0x00, CLytCtrl object handles
            long m_nMax;                // N
            long m_nNum;                // handles in use (CSelectMenuLayoutBase::CreateLytObj appends)

            // vptr after the data. Defined after the class.
            virtual ~CMenuLytObj();

            // The object behind handle nIndex, or null. Inline in the game.
            T *GetLytObj(int nIndex = 0)
            {
                CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
                if (pLytCtrl == 0) {
                    return 0;
                }
                return static_cast<T *>(pLytCtrl->GetObject(m_nHandle[nIndex]));
            }

            // Deletes every object. Inline in the game.
            void DeleteLytObj()
            {
                CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
                if (pLytCtrl != 0) {
                    for (int i = 0; i < N; i++) {
                        if (m_nHandle[i] != 0) {
                            pLytCtrl->DeleteObject(m_nHandle[i]);
                            m_nHandle[i] = 0;
                        }
                    }
                    m_nNum = 0;
                }
            }
    };

    // Defined outside the class and not declared inline (it is still
    // inlined into derived destructors): with an in-class body, every
    // derived constructor and destructor makes the compiler emit the weak
    // copy right after itself, where the game has it among the template
    // instances at the end of the file (MenuF CSelectMenuLayoutBase.cpp,
    // DodgeF CCockpitDodgeTelopInOutChange.cpp).
    template <class T, int N>
    CMenuLytObj<T, N>::~CMenuLytObj()
    {
        DeleteLytObj();
    }
}

#endif /* !CMENULYTOBJ_HPP_ */
