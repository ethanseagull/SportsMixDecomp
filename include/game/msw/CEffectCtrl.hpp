#ifndef CEFFECTCTRL_HPP_
#define CEFFECTCTRL_HPP_

#include "bcs/CUnivTask.hpp"

namespace bcs {
    class CScnObj;
    struct FMTX34;
    struct FVEC3;

    namespace EF {
        class CEffect;
    }

    namespace G2D {
        class CPaneObj;
    }
}

namespace msw {
    class CEffectCluster;
    class CEffectTableList;

    // Size 0x4C. The class lives in the DOL; each module derives its own
    // effect controller from it. The virtual functions are declared in the
    // order of the vtables the modules emit. Return types nobody has used yet
    // are guessed as void.
    class CEffectCtrl : public bcs::CUnivTask {
        public:
            struct ResourceInfo;

            CEffectCtrl(bcs::CTask *pParent);
            virtual ~CEffectCtrl();
            virtual void Main();
            virtual void InitializeDataTable();
            virtual void InitializeDataTable(ResourceInfo *, long);
            virtual void SetSimpleEffect(long, const bcs::FMTX34 *, const bcs::FVEC3 *, unsigned long,
                                         unsigned long, unsigned long, unsigned long);
            virtual void SetSimpleEffect(const char *, const char *, const bcs::FMTX34 *, const bcs::FVEC3 *,
                                         unsigned long, unsigned long, unsigned long);
            virtual void CreateCluster(const char *, const char *, unsigned long, unsigned long, unsigned long);
            virtual void CreateAttachCluster(long, const bcs::CScnObj *, unsigned long, const bcs::FVEC3 &, int, int,
                                             int, unsigned long, unsigned long, unsigned long, unsigned long);
            virtual void CreateAttachCluster(long, bcs::G2D::CPaneObj *, const bcs::FVEC3 &, long, int, int, int,
                                             unsigned long, unsigned long, unsigned long);
            virtual void CreateAttachClusterAuto(long, const bcs::CScnObj *, unsigned long);
            virtual void CreateAttachClusterAuto(const char *, const char *, const bcs::CScnObj *, const char *,
                                                 unsigned long);
            virtual int InitPatch(long, bcs::EF::CEffect *); // returns 0; type guessed
            // DOL CreateAttachEffect__Q23msw11CEffectCtrlFlPCQ23bcs7CScnObjPCQ23bcs5FVEC3UliiiUlUlUlUl.
            // Static: the body takes the id in r3 and finds the instance through
            // s_InstanceHandle itself, and msw::CGimmickManBoss::EffectCoinGetCreate_
            // (BossF) passes the id in r3. Returns what the instance's virtual at
            // 0x2C returns, 0 without an instance; the caller keeps it as a handle
            // for StopAttachEffect (type guessed).
            static unsigned long CreateAttachEffect(long, const bcs::CScnObj *, const bcs::FVEC3 *, unsigned long, int,
                                                    int, int, unsigned long, unsigned long, unsigned long,
                                                    unsigned long);

            // (DOL) Plays effect nId once at rPos. (meanings of the three
            // words not known; msw::CDropCoin passes 3, 0, 0; return type
            // guessed)
            static void SimpleEffect(long nId, const bcs::FVEC3 &rPos, unsigned long, unsigned long, unsigned long);

            // Added for game/msw/cockpit/CMissionCount.cpp: the same for an
            // effect attached to a layout pane object (static as well; the
            // cockpit passes no position, -1, three zeros, then 4, 0, 0).
            static unsigned long CreateAttachEffect(long, bcs::G2D::CPaneObj *, const bcs::FVEC3 *, long, int, int,
                                                    int, unsigned long, unsigned long, unsigned long);
            // Static (the handle is in r3). Return types guessed. Erase
            // removes the effect at once; Stop lets it end.
            static void EraseAttachEffect(unsigned long hEffect);
            static void StopAttachEffect(unsigned long hEffect);

            // The cluster with handle hEffect, null when there is none.
            CEffectCluster *FindList(unsigned long hEffect);
            // The same through the instance. Static and inline, yet emitted
            // out of line (BasketF exports GetObject__Q23msw11CEffectCtrlFUl,
            // and the DOL's copy at 0x801BB088 is called, not inlined, by the
            // cockpit), and its callers go through GetInstance() first.
            static CEffectCluster *GetObject(unsigned long hEffect) { return GetInstance()->FindList(hEffect); }

            // Looks the instance up in the task manager by s_InstanceHandle.
            static CEffectCtrl *GetInstance();
            static void AttachResource();
            // Registers the effects of archive `pPath`, file `fileId` of CLoadData.
            static void AttachArcResource(long fileId, const char *pPath); // return type guessed

        protected:
            // (name guessed) Remembers which task is the instance. Inferred
            // from the null check that the modules' CreateInstance and
            // destructors repeat on a pointer already known to be non-null.
            static void SetInstance(CEffectCtrl *pInstance)
            {
                s_InstanceHandle = pInstance != 0 ? pInstance->GetId() : 0;
            }

            static unsigned long s_InstanceHandle;

        private:
            unsigned char m_Unk34[0xC]; // 0x34: a nw4r::ut::List

        protected:
            // Filled in by InitializeDataTable.
            CEffectTableList *m_pTableList;       // 0x40
            CEffectTableList *m_pAttachTableList; // 0x44

        private:
            // Not set by the constructor. The class is 0x4C bytes: every
            // module's CreateInstance allocates that much.
            unsigned long m_Unk48; // 0x48
    };
}

#endif /* !CEFFECTCTRL_HPP_ */
