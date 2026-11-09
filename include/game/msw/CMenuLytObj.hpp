#ifndef CMENULYTOBJ_HPP_
#define CMENULYTOBJ_HPP_

namespace msw {

    // Size 0x10. Owns N layout objects of type T registered with CLytCtrl.
    // Member names are guesses; the destructor body is not written yet (it
    // releases m_nHandle through CLytCtrl).
    template <class T, int N>
    class CMenuLytObj {
        public:
            unsigned long m_nHandle; // 0x00, CLytCtrl object handle
            long m_nCount;           // 0x04, set to N
            T *m_pObj;               // 0x08

            // vptr at 0x0C
            virtual ~CMenuLytObj();
    };
}

#endif /* !CMENULYTOBJ_HPP_ */
