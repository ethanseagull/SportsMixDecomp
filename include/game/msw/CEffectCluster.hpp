#ifndef CEFFECTCLUSTER_HPP_
#define CEFFECTCLUSTER_HPP_

#include "bcs/EF/CEffect.hpp"
#include "bcs/EF/CResource.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CMdlObj;
    namespace EF {
        class CEffect;
    }
}

namespace msw {

    // A group of effects started together (DOL class). Only the functions
    // the modules call so far; layout mapped only where a caller reads it.
    class CEffectCluster {
        public:
            void SetTranslate(const bcs::FVEC3 *pPos);
            // Rotation in radians.
            void SetRotateRad(const bcs::FVEC3 *pRot);
            // (parameter names and return types guessed) Fades the cluster
            // out over fFrame frames; the effects it holds.
            void SetFadeoutTime(float fFrame);
            long GetEffectNum();
            bcs::EF::CEffect *GetEffect(long nIndex);
            // (DOL; added for BossF game/boss/CChrCoreBossKinopio.cpp) The
            // models of the cluster; an entry may be null.
            long GetMdlObjNum();
            bcs::CMdlObj *GetMdlObj(long nIndex);

            // (name guessed; the DOL's fn_801233FC) Called with 1 on a
            // cluster just attached to a layout pane object. Calls a member
            // of each effect through a member pointer, like CallEffectFunc;
            // the member (fn_80123490) sets or clears bit 0x100000 of the
            // word at 0xA4.
            void Set2D(int b2D);

            // (name guessed) Calls pFunc with nArg on each effect of the
            // cluster that still exists. Inline in the DOL's
            // msw::cockpit::CEffectSet::SetDispFlag; the cluster's own
            // functions (ClearPauseFlag, fn_801233FC) have the same loop.
            void CallEffectFunc(void (bcs::EF::CEffect::*pFunc)(int), int nArg)
            {
                bcs::EF::CManager *pManager = bcs::EF::CManager::GetInstance();
                for (long i = 0; i < m_nEffectNum; i++) {
                    bcs::EF::CEffect *pEffect = pManager->GetEffect(m_Effect[i].m_hEffect);
                    if (pEffect != 0) {
                        (pEffect->*pFunc)(nArg);
                    }
                }
            }

        private:
            struct Entry {
                unsigned long m_hEffect; // 0x00, for bcs::EF::CManager::GetEffect
                unsigned long m_Unk04;
            };

            unsigned char m_Unk00[0x10];
            Entry m_Effect[16];          // 0x10 (the count is a guess)
            unsigned char m_Unk90[0x1C];
            long m_nEffectNum;           // 0xAC, entries in use
    };
}

#endif /* !CEFFECTCLUSTER_HPP_ */
