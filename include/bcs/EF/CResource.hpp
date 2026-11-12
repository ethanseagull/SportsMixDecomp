#ifndef BCS_EF_CRESOURCE_HPP_
#define BCS_EF_CRESOURCE_HPP_

#include "bcs/CTask.hpp"
#include "nw4r/ut/List.hpp"

namespace nw4r {
    namespace g3d {
        class ResFile;
    }
}

namespace bcs {
    class CArcFileAccessor;

    namespace EF {

        class CEffect;

        // (name guessed) The base of the objects the effect manager keeps
        // in its six lists (the DOL's fn_8001BE44 builds a derived class on
        // it, likely bcs::EF::CEffect). Constructor
        // fn_80023DB0 and destructor fn_80023E74 in the DOL; the vtable is
        // lbl_80464E30. The vptr comes after the data (at 0x1C).
        class CEffectBase {
            private:
                // Declared before the virtuals: the vptr follows them.
                nw4r::ut::Link m_Link;      // 0x00
                unsigned long m_hHandle;    // 0x08 (name guessed)
                // (name guessed) The constructor stores the object itself;
                // CManager::GetEffect returns it.
                CEffectBase *m_pBody;       // 0x0C
                unsigned short m_nGroup;    // 0x10, the manager list it is in
                unsigned short m_Unk12;     // 0x12
                unsigned char m_Unk14;      // 0x14
                // (name guessed) Compared signed by CManager::CalcAll.
                long m_nFlag;               // 0x18

            public:
                virtual ~CEffectBase();
                // (names guessed) Empty in the base. Calc (slot 0x0C) runs
                // once a frame, CalcView (0x10) once per view, Disp (0x18)
                // from CManager::Disp; slot 0x14 is never called there.
                virtual void Calc();
                virtual void CalcView(long nView);
                virtual void Unk14();
                virtual void Disp();

                // (names guessed) Bits of m_nFlag: 0x4 asks the manager to
                // delete the object, 0x8 stops Calc, 0x10 stops CalcView.
                enum {
                    FLAG_DELETE = 0x4,
                    FLAG_STOP_CALC = 0x8,
                    FLAG_STOP_CALC_VIEW = 0x10
                };
                CEffectBase *GetBody() const { return m_pBody; }
                long GetFlag() const { return m_nFlag; }
        };

        // The effect system, a task (0x198 bytes, created by the DOL's
        // fn_800229A0). Only the members used so far.
        class CManager : public CTask {
            public:
                static CManager *GetInstance();
                // (the DOL's fn_80022A1C, name guessed) Asks the task
                // manager to delete the instance and forgets it.
                static void Destroy();

                virtual ~CManager();
                virtual void Main();
                virtual void Disp();

                // (the DOL's fn_80022B90, name guessed) Deletes the objects
                // marked for it and runs Calc on the others.
                void CalcAll();
                // (the DOL's fn_80022C60, name guessed) Sets the effect
                // system's camera to view nView and runs CalcView on the
                // objects of list nGroup.
                void CalcView(long nGroup, long nView);
                // Added for game/msw/cockpit/CMissionCount.cpp: the effect
                // with handle hEffect, null when it is gone.
                CEffect *GetEffect(unsigned long hEffect);
                // (name guessed; the DOL's fn_80023B2C, added for
                // msw::CEffectCluster::IsValid) Nonzero while the effect with
                // handle hEffect is in one of the manager's lists.
                int IsExist(unsigned long hEffect);

                // (name guessed) The byte at 0x95; msw::CTournamentMan
                // (TourF) clears it once the map is loaded.
                void SetUnk95(bool b) { m_bUnk95 = b; }

            private:
                // (name guessed) Set by Create; Destroy clears it.
                static CManager *s_pInstance;

                // The six lists of CEffectBase objects (one per group).
                nw4r::ut::List m_aList[6];  // 0x34
                unsigned char m_Unk7C[0x95 - 0x7C];
                bool m_bUnk95; // 0x95
                unsigned char m_Unk96[0x198 - 0x96];
        };

        // The effect archives known to the effect system. Lives in the DOL;
        // only the members used so far. Return types nobody has used yet are
        // guessed as void.
        class CResource {
            public:
                static CResource *GetInstance();

                CArcFileAccessor *GetArcAccesor(const char *pPath, const char *pRoot);
                void ReleaseArcAccesor(CArcFileAccessor *pAccessor);
                // Non-zero if the archive `pPath` is already registered.
                int GetResourceAfterLoad(const char *pPath, void *pData, unsigned long size); // return type guessed
                void AddArchive(const char *pPath);
                void RelocateCommand();
                void InvalidateCache();
                // The g3d file `pName` of the loaded archives (a null handle
                // when there is none).
                nw4r::g3d::ResFile GetG3DResFile(const char *pName);
                // The g3d file holding the model `pName` (a null handle when
                // there is none).
                nw4r::g3d::ResFile GetG3DResFileByMdlName(const char *pName);
        };
    }
}

#endif /* !BCS_EF_CRESOURCE_HPP_ */
