#ifndef BCS_CANMOBJMAT_HPP_
#define BCS_CANMOBJMAT_HPP_

#include "bcs/CAnmObj.hpp"
#include "bcs/CFrameCtrl.hpp"

namespace bcs {

    // Per-material table of the material animations, the second base of
    // bcs::CAnmObjMat at 0x0C. (class and member names guessed) Create fills
    // m_pTable with one 0x8000 per material of the model. Its vtable
    // (0x80463EA8) has only the destructor. In the derived classes' vtables
    // its part sits at 0x44, followed by the derived classes' own virtuals
    // (see bcs::CAnmObjMat).
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
    //
    // The vtable of a derived class is the CAnmObj slots (to 0x40), then
    // CAnmMatTable's (0x44: two zero words and the destructor thunk), then
    // the four virtuals each derived class adds (0x50 GetResult, 0x54 Bind,
    // 0x58 ChangeRes, 0x5C Release; names guessed after nw4r's AnmObj).
    // Callers (bcs::CMdlObj) call 0x50 on the CAnmObjMat pointer.
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

            // (name guessed) Marks every material unbound (0x8000) and
            // clears the bound flag. Inline in Create, Release and ChangeRes
            // of bcs::CAnmTexSrt and CAnmTexPat.
            void UnbindAll()
            {
                for (unsigned long i = 0; i < m_nNum; i++) {
                    m_pTable[i] = 0x8000;
                }
                OffAnmFlag(ANMFLAG_4);
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
