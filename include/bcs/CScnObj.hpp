#ifndef BCS_CSCNOBJ_HPP_
#define BCS_CSCNOBJ_HPP_

#include "bcs/CG3dObj.hpp"
#include "bcs/FVEC3.hpp"

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
            // 0x40-0x50 from the vtable of bcs::CMdlObj (return types guessed).
            virtual const FMTX34 *GetNodeWorldMtxPtr(unsigned long) const;  // 0x40
            virtual void SetNodeLocalMtx(unsigned long, const FMTX34 &);    // 0x44
            virtual void SetNodeWorldMtx(unsigned long, const FMTX34 &);    // 0x48
            virtual void SetAlphaRate(unsigned char);                       // 0x4C
            virtual void SetAlphaRateF(float);                              // 0x50
            // More virtual functions follow; none is needed yet.

            // Queues the object to be deleted later.
            static void AppendDelayDeleteObject(CScnObj *pObj);

            // (names guessed) Inline in the modules.
            void OnFlag(unsigned long flag) { m_nFlag |= flag; }
            void OffFlag(unsigned long flag) { m_nFlag &= ~flag; }

        private:
            unsigned char m_Unk08[0x90];  // 0x08

        public:
            // (names guessed) Read by bcs::CScnObj::CalcRootMtx in the DOL;
            // the modules write them directly (msw::GIMMICK_BOSS::
            // CEffectCoinGet::Main in BossF).
            FVEC3 m_Pos;                  // 0x98

        private:
            unsigned char m_UnkA4[0xC];   // 0xA4

        public:
            FVEC3 m_Rot;                  // 0xB0, radians
            FVEC3 m_Scale;                // 0xBC

        private:
            unsigned char m_UnkC8[0x34];  // 0xC8
            unsigned long m_nFlag;        // 0xFC (name guessed)
            unsigned char m_Unk100[0x18]; // 0x100
    };
}

#endif /* !BCS_CSCNOBJ_HPP_ */
