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
