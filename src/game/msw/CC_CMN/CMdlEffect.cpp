#include "game/msw/CMdlEffect.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CStageMan.hpp"
#include "bcs/CAnmObj.hpp"
#include "bcs/CMdlObj.hpp"

namespace msw {
    namespace CC_CMN {

        // (name guessed) Clears bEnd while animation slot type of pMdl
        // still plays: attached, not looping and not at its end.
        static inline void CheckAnmEnd(bcs::CMdlObj *pMdl, bcs::CMdlObj::AnmObjType type, int &bEnd)
        {
            bcs::CAnmObj *pAnm = pMdl->GetAnmObj(type);
            if (pAnm != 0 && !pAnm->IsLooped() && !pAnm->IsEnd()) {
                bEnd = 0;
            }
        }

        CMdlEffect::CMdlEffect(bcs::CTask *pParent, long nEff, const bcs::FVEC3 &pos, const bcs::FVEC3 &rot,
                               unsigned long nFlag, long nArg)
            : bcs::CUnivTaskEx(pParent, 0x16), m_nEffFlag(nFlag)
        {
            m_Pos = pos;
            m_Rot = rot;
            m_hEffect = 0;
            unsigned long hEffect = CEffectCtrl::CreateAttachEffect(nEff, 0, 0, 0, 1, 1, 0, nArg, 0, 0, 0);
            m_hEffect = hEffect;
            CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
            if (pCluster != 0) {
                pCluster->SetTranslate(&m_Pos);
                pCluster->SetRotateRad(&m_Rot);
            }
        }

        CMdlEffect::~CMdlEffect()
        {
            CEffectCtrl::EraseAttachEffect(m_hEffect);
            m_hEffect = 0;
        }

        void CMdlEffect::Main()
        {
            GetFrameRateF();

            if (m_nEffFlag & FLAG_FOLLOW_FLOOR) {
                bcs::FVEC3 pos(m_Pos.x, 0.0f, m_Pos.z);
                CStageMan *pStage = CStageMan::GetInstance();
                if (CCollMan::IsGrdAttrFlow(pStage->GetFloorAttribute(pos, CStageMan::FLOW_TYPE_0))) {
                    bcs::FVEC3 flow;
                    pStage->GetFloorFlow(flow, pos, CStageMan::FLOW_TYPE_2);
                    pos.z += flow.z;
                    pos.x += flow.x;
                    m_Pos.x = pos.x;
                    m_Pos.z = pos.z;
                }
            }

            CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
            if (pCluster != 0) {
                pCluster->SetTranslate(&m_Pos);
            }

            int bEnd = 1;
            unsigned long hEffect = m_hEffect;
            if (hEffect != 0) {
                CEffectCluster *pEffect = CEffectCtrl::GetObject(hEffect);
                if (pEffect != 0) {
                    long nNum = pEffect->GetMdlObjNum();
                    for (long i = 0; i < nNum; i++) {
                        bcs::CMdlObj *pMdl = pEffect->GetMdlObj(i);
                        if (pMdl != 0) {
                            CheckAnmEnd(pMdl, bcs::CMdlObj::ANMOBJTYPE_CHR, bEnd);
                            CheckAnmEnd(pMdl, bcs::CMdlObj::ANMOBJTYPE_MATCLR, bEnd);
                            CheckAnmEnd(pMdl, bcs::CMdlObj::ANMOBJTYPE_3, bEnd);
                            CheckAnmEnd(pMdl, bcs::CMdlObj::ANMOBJTYPE_TEXSRT, bEnd);
                            CheckAnmEnd(pMdl, bcs::CMdlObj::ANMOBJTYPE_5, bEnd);
                        }
                    }
                }
            }
            if (bEnd) {
                Delete();
            }
        }

        void CMdlEffect::SetMatTevKColorRGB(_GXTevKColorID id, bcs::BCS_Color color)
        {
            unsigned long hEffect = m_hEffect;
            if (hEffect == 0) {
                return;
            }
            CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
            if (pCluster == 0) {
                return;
            }
            long nNum = pCluster->GetMdlObjNum();
            for (long i = 0; i < nNum; i++) {
                bcs::CMdlObj *pMdl = pCluster->GetMdlObj(i);
                if (pMdl != 0) {
                    pMdl->SetMatTevKColorRGB(id, color);
                }
            }
        }
    }
}
