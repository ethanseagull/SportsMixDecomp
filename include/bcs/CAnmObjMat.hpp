#ifndef BCS_CANMOBJMAT_HPP_
#define BCS_CANMOBJMAT_HPP_

#include "bcs/CAnmObj.hpp"
#include "bcs/CFrameCtrl.hpp"

namespace bcs {

    // Per-material table of the material animations, the second base of
    // bcs::CAnmObjMat at 0x0C. (class and member names guessed) Create fills
    // m_pTable with one 0x8000 per material of the model. Its vtable
    // (0x80463EA8) has the destructor and one null slot; the four slots past
    // the destructor in the secondary vtables of the derived classes are not
    // known yet.
    class CAnmMatTable {
        public:
            CAnmMatTable() : m_nNum(0), m_pTable(0) {}
            virtual ~CAnmMatTable() { Release(); }

            void Release()
            {
                if (m_pTable != 0) {
                    delete[] m_pTable;
                }
                m_pTable = 0;
                m_nNum = 0;
            }

            unsigned long m_nNum;      // 0x04 (0x10 in the animation)
            unsigned short *m_pTable;  // 0x08 (0x14)
    };

    // (name guessed) Common part of bcs::CAnmMatClr, CAnmTexSrt and
    // CAnmTexPat, whose constructors and destructors are copies of each
    // other. Size 0x28. Constructor and destructor inline.
    class CAnmObjMat : public CAnmObj, public CAnmMatTable {
        public:
            CAnmObjMat() {}
            virtual ~CAnmObjMat() {}

        protected:
            // (name guessed) Rewinds the frame counter of a released
            // animation.
            void ResetFrame()
            {
                m_Frame.m_fEndFrame = 0.0f;
                m_Frame.SetLoop(false);
                m_Frame.SetFrame(0.0f);
            }

            // (name guessed) Frees the material table.
            void ReleaseMatTable()
            {
                OffAnmFlag(ANMFLAG_4);
                Release();
            }

            CFrameCtrl m_Frame; // 0x18
    };
}

#endif /* !BCS_CANMOBJMAT_HPP_ */
