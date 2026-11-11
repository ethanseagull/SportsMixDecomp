#ifndef MSW_CSTGOBJ_HPP_
#define MSW_CSTGOBJ_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CStgAct.hpp"
#include "game/msw/CStgSrt.hpp"

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
    // BasketF has all of them in game/basket/CStgGmk_WaterWorks_Basket.cpp.
    // Return types not visible in a body are guesses.
    class CStgObj : public bcs::CUnivTask {
        public:
            virtual ~CStgObj();

            virtual void SetIdx(long idx) { m_nIdx = idx; }
            virtual long GetIdx() const { return m_nIdx; }
            virtual void Create();
            virtual void SceneEntry();
            virtual void SceneLeave();
            // The actor's CStgSrt, or 0 without an actor.
            virtual CStgSrt *GetSrt() { return m_pAct ? m_pAct->GetSrt() : 0; }
            virtual const CStgSrt *GetSrt() const { return m_pAct ? m_pAct->GetSrt() : 0; }
            // Forward to the actor's CStgSrt; nothing without an actor.
            virtual void SetPos(const bcs::FVEC3 &rPos)
            {
                if (m_pAct) {
                    m_pAct->GetSrt()->SetPos(rPos);
                }
            }
            virtual void SetFront(const bcs::FVEC3 &rFront)
            {
                if (m_pAct) {
                    m_pAct->GetSrt()->SetFront(rFront);
                }
            }
            virtual void LookAt(const bcs::FVEC3 &rTarget)
            {
                if (m_pAct) {
                    m_pAct->GetSrt()->SetLookAtPos(rTarget);
                }
            }
            virtual void EventNotify(CStgAct *pAct, long event, void *pArg);
            virtual unsigned long CalcSum() const;
            // Factories for the parts; each stage object class supplies them.
            virtual CStgMdl *NewMdl(long heap, unsigned long idx) = 0;
            virtual CStgAct *NewAct(long heap) = 0;
            virtual CStgDrv *NewDrv(long heap) = 0;
            virtual unsigned long GetMdlNum() const { return m_nMdlNum; }
            // Model idx, or 0 when idx is not below GetMdlNum().
            virtual CStgMdl *GetMdl(unsigned long idx) { return idx < GetMdlNum() ? m_ppMdl[idx] : 0; }
            virtual const CStgMdl *GetMdl(unsigned long idx) const
            {
                return idx < GetMdlNum() ? m_ppMdl[idx] : 0;
            }
            virtual CStgAct *GetAct() { return m_pAct; }
            virtual const CStgAct *GetAct() const { return m_pAct; }
            virtual CStgDrv *GetDrv() { return m_pDrv; }
            virtual const CStgDrv *GetDrv() const { return m_pDrv; }
            // Hooks around Create, SceneEntry and SceneLeave. They belong to
            // this class, not to msw::CStgGmk: the vtables of the gimmicks
            // that keep the default name OnCalcSum__Q23msw7CStgObjCFv in
            // that slot (HockeyF __vt__Q23msw25CStgGmk_WaterWorks_Hockey,
            // CGoalObjHockey), and CStgGmk's versions of the other four
            // are overrides.
            virtual void OnPreCreate();
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();
            virtual unsigned long OnCalcSum() const { return 0; }

        private:
            unsigned long m_nMdlNum; // 0x34
            CStgMdl **m_ppMdl;       // 0x38, GetMdlNum() models
            CStgAct *m_pAct;         // 0x3C
            CStgDrv *m_pDrv;         // 0x40
            long m_nIdx;             // 0x44
    };

    // The driver of a stage object. A DOL class, size 0xC, vptr at 0x08
    // (constructor __ct__Q23msw7CStgDrvFPQ23msw7CStgObj): the two data
    // members come before the first virtual function. The order of the
    // virtual functions is that of __vt__Q23msw29CStgGmk_WaterWorks_Hockey_Drv
    // (HockeyF); the modules emit weak copies of everything after Main, so
    // those are inline (HockeyF has them in
    // game/hockey/CStgGmk_WaterWorks_Hockey.cpp).
    class CStgDrv {
        private:
            CStgObj *m_pParent; // 0x00
            CStgAct *m_pAct;    // 0x04

        public:
            CStgDrv(CStgObj *pParent);
            virtual ~CStgDrv();
            virtual void Main(float rate);
            virtual CStgObj *GetParent() { return m_pParent; }
            virtual const CStgObj *GetParent() const { return m_pParent; }
            virtual void SetAct(CStgAct *pAct) { m_pAct = pAct; }
            virtual CStgAct *GetAct() { return m_pAct; }
            virtual const CStgAct *GetAct() const { return m_pAct; }
            virtual void OnUserEvent(CStgAct *pAct, long event, void *pArg) {}
    };
}

#include "game/msw/CStgMdl.hpp"

#endif /* !MSW_CSTGOBJ_HPP_ */
