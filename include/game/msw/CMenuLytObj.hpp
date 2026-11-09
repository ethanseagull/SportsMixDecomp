#ifndef CMENULYTOBJ_HPP_
#define CMENULYTOBJ_HPP_

#include "game/msw/CLytCtrl.hpp"

namespace msw {

    // Size 0xC + 4 * N. Owns up to N layout objects of type T registered
    // with CLytCtrl, by handle. Member and function names are guesses; the
    // destructor body is not written yet (it does what DeleteLytObj does).
    template <class T, int N>
    class CMenuLytObj {
        public:
            unsigned long m_nHandle[N]; // 0x00, CLytCtrl object handles
            long m_nMax;                // set to N by the owner's constructor
            long m_nNum;                // handles in use (CSelectMenuLayoutBase::CreateLytObj appends)

            // vptr after the data
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
}

#endif /* !CMENULYTOBJ_HPP_ */
