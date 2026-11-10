#ifndef MSW_CSTGOBJ_HPP_
#define MSW_CSTGOBJ_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    class CStgAct;
    class CStgDrv;
    class CStgMdl;
    class CStgSrt;

    // An object of a stage (a gimmick, a stage part): a task that owns up to
    // GetMdlNum() models, one actor (the logic) and one driver. A DOL class,
    // size 0x48: bcs::CUnivTask at 0x00 (vptr at 0x00), own members from
    // 0x34. Offsets come from the inline functions the modules emit (DodgeF
    // has them in game/dodge/CStgGmk_BossPackun_Dodge_Act.cpp) and from
    // msw::CStgGmk's constructor in the DOL.
    //
    // The order of the virtual functions is the order of the slots in
    // __vt__Q23msw18CStgGmk_Wave_Dodge (DodgeF). Several of them are inline
    // in the original: the modules emit weak copies of GetIdx, SetIdx,
    // GetSrt, SetPos, SetFront, LookAt, GetMdlNum, GetMdl, GetAct and GetDrv.
    // Bodies are given for the plain getters; the others need the layout of
    // msw::CStgAct and msw::CStgSrt and are declared only, for now. Return
    // types not visible in a body are guesses.
    class CStgObj : public bcs::CUnivTask {
        public:
            virtual ~CStgObj();

            virtual void SetIdx(long idx) { m_nIdx = idx; }
            virtual long GetIdx() const { return m_nIdx; }
            virtual void Create();
            virtual void SceneEntry();
            virtual void SceneLeave();
            // Inline in the original: the actor's GetSrt(), or 0 without an
            // actor.
            virtual CStgSrt *GetSrt();
            virtual const CStgSrt *GetSrt() const;
            // Inline in the original: forward to the actor's CStgSrt.
            virtual void SetPos(const bcs::FVEC3 &rPos);
            virtual void SetFront(const bcs::FVEC3 &rFront);
            virtual void LookAt(const bcs::FVEC3 &rTarget);
            virtual void EventNotify(CStgAct *pAct, long event, void *pArg);
            virtual unsigned long CalcSum() const;
            // Factories for the parts; each stage object class supplies them.
            virtual CStgMdl *NewMdl(long heap, unsigned long idx) = 0;
            virtual CStgAct *NewAct(long heap) = 0;
            virtual CStgDrv *NewDrv(long heap) = 0;
            virtual unsigned long GetMdlNum() const { return m_nMdlNum; }
            // Inline in the original: model idx, or 0 when idx is not below
            // GetMdlNum() (called through the vtable).
            virtual CStgMdl *GetMdl(unsigned long idx);
            virtual const CStgMdl *GetMdl(unsigned long idx) const;
            virtual CStgAct *GetAct() { return m_pAct; }
            virtual const CStgAct *GetAct() const { return m_pAct; }
            virtual CStgDrv *GetDrv() { return m_pDrv; }
            virtual const CStgDrv *GetDrv() const { return m_pDrv; }

        private:
            unsigned long m_nMdlNum; // 0x34
            CStgMdl **m_ppMdl;       // 0x38, GetMdlNum() models
            CStgAct *m_pAct;         // 0x3C
            CStgDrv *m_pDrv;         // 0x40
            long m_nIdx;             // 0x44
    };

    // The driver of a stage object. A DOL class, size 0xC, vptr at 0x08
    // (constructor __ct__Q23msw7CStgDrvFPQ23msw7CStgObj). Only what the
    // modules need to create one is declared.
    class CStgDrv {
        public:
            CStgDrv(CStgObj *pParent);
            virtual ~CStgDrv();

        private:
            CStgObj *m_pParent;    // 0x00
            unsigned long m_Unk04; // 0x04
    };

    // A model of a stage object. A DOL class, size 0x12C, vptr at 0x128
    // (constructor __ct__Q23msw7CStgMdlFPQ23msw7CStgObj). Only what the
    // modules need to create one is declared.
    class CStgMdl {
        public:
            CStgMdl(CStgObj *pParent);

        private:
            CStgObj *m_pParent;           // 0x00
            unsigned char m_Unk04[0x124]; // 0x04

        public:
            virtual ~CStgMdl();
    };
}

#endif /* !MSW_CSTGOBJ_HPP_ */
