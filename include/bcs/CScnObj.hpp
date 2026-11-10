#ifndef BCS_CSCNOBJ_HPP_
#define BCS_CSCNOBJ_HPP_

#include "bcs/CG3dObj.hpp"

namespace bcs {

    struct FMTX34;

    // An object in the 3D scene. Size 0x118 (the members of bcs::CMdlObj
    // start there). A DOL class. Only the virtual functions up to slot 0x3C
    // are declared, in the order of the vtable of bcs::CMdlObj. Return types
    // are read from the DOL bodies unless marked.
    class CScnObj : public CG3dObj {
        public:
            // (enumerator names guessed) ForEach returns whether the
            // callback returned 1.
            enum ForEachResult {
                FOREACHRESULT_0,
                FOREACHRESULT_1
            };

            typedef ForEachResult (*ForEachFunc)(CScnObj *, void *);

            // Bits of m_nFlag. (names guessed) msw::CTableSetMdlObj::SetVisible
            // sets both to hide a model and clears both to show it.
            enum {
                FLAG_HIDE_40 = 0x40,
                FLAG_HIDE_80 = 0x80
            };

            CScnObj();
            virtual ~CScnObj();
            virtual void ObjProc(unsigned long, unsigned long, void *);    // 0x1C
            virtual int ForEach(ForEachFunc pFunc, void *pArg, int);       // 0x20
            virtual float GetValueForSortOpa() const;                      // 0x24
            virtual float GetValueForSortXlu() const;                      // 0x28
            virtual void CalcBoundingVolume();                             // 0x2C
            virtual void CalcWorldMtx(const FMTX34 *);                     // 0x30 (return type guessed)
            virtual void GetNodeLocalMtx(unsigned long, FMTX34 &) const;   // 0x34 (return type guessed)
            virtual const FMTX34 *GetNodeLocalMtxPtr(unsigned long) const; // 0x38 (const result guessed)
            virtual void GetNodeWorldMtx(unsigned long, FMTX34 &) const;   // 0x3C (return type guessed)
            // More virtual functions follow; none is needed yet.

            // Queues the object to be deleted later.
            static void AppendDelayDeleteObject(CScnObj *pObj);

            // Rebuilds the matrix at 0x08 from the rotation and the
            // translation members.
            void CalcRootMtx();

            // (names guessed) Inline in the modules.
            void OnFlag(unsigned long flag) { m_nFlag |= flag; }
            void OffFlag(unsigned long flag) { m_nFlag &= ~flag; }

        private:
            unsigned char m_Unk08[0xA8];  // 0x08

        public:
            // (names guessed) The rotation about the three axes in radians,
            // as CalcRootMtx reads it. msw::CChrMarkerVolleyJump::MainFunc
            // turns the Y one directly.
            float m_fRotX;                // 0xB0
            float m_fRotY;                // 0xB4
            float m_fRotZ;                // 0xB8

        private:
            unsigned char m_UnkBC[0x40];  // 0xBC
            unsigned long m_nFlag;        // 0xFC (name guessed)
            unsigned char m_Unk100[0x18]; // 0x100
    };
}

#endif /* !BCS_CSCNOBJ_HPP_ */
