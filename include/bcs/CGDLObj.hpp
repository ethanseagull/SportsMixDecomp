#ifndef BCS_CGDLOBJ_HPP_
#define BCS_CGDLOBJ_HPP_

#include "bcs/CScnObj.hpp"

namespace bcs {

    // A scene object drawn from prebuilt GX display lists, one for the
    // opaque pass and one for the translucent pass. A DOL class. It
    // overrides only ObjProc; its vtable (lbl_80466BB8) keeps
    // CScnObj::UpdateFrame null, so it is abstract. Members from 0x118 are
    // read from the constructor and the setters; the names are guesses.
    class CGDLObj : public CScnObj {
        public:
            CGDLObj();
            virtual ~CGDLObj();
            virtual void ObjProc(unsigned long, unsigned long, void *);

            void SetDLOpa(void *pDL, unsigned long nSize);
            // fn_80044BC8, unnamed in the binary (no module calls it).
            void SetDLXlu(void *pDL, unsigned long nSize);

            // (names guessed) Whether a list of non-zero size is set.
            bool HasDLOpa() const
            {
                return m_pDLOpa != 0 && m_nDLOpaSize != 0;
            }
            bool HasDLXlu() const
            {
                return m_pDLXlu != 0 && m_nDLXluSize != 0;
            }

        private:
            void *m_pDLOpa;          // 0x118
            void *m_pDLXlu;          // 0x11C
            unsigned long m_nDLOpaSize; // 0x120
            unsigned long m_nDLXluSize; // 0x124
    };
}

#endif /* !BCS_CGDLOBJ_HPP_ */
