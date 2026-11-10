#include <stdio.h>

#include "nw4r/g3d/ResFile.hpp"
#include "game/msw/CLoadData.hpp"

#include "game/msw/CTournamentChip.hpp"

#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnGroup.hpp"
#include "bcs/New.hpp"
#include "game/msw/CEvDemoMdlObj.hpp"
#include "game/msw/CEvDemoRes.hpp"
#include "game/msw/CEvMdlCreaterTmMdlChr.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/CTournamentMan.hpp"
#include "game/msw/CTournamentRouteMan.hpp"
#include "game/msw/CTrophyObj.hpp"
#include "game/msw/EvDemo.hpp"

namespace msw {

    namespace {

        // An angle brought into [0, 2 pi). (same helper as in
        // game/volley/CStageMan.cpp)
        inline float WrapRad(float rad)
        {
            while (rad > 6.2831855f) {
                rad -= 6.2831855f;
            }
            while (rad < 0.0f) {
                rad += 6.2831855f;
            }
            return rad;
        }

        // Sets the rotation of pObj, each angle brought into [0, 2 pi).
        inline void SetRot(bcs::CScnObj *pObj, float x, float y, float z)
        {
            pObj->m_fRotX = WrapRad(x);
            pObj->m_fRotY = WrapRad(y);
            pObj->m_fRotZ = WrapRad(z);
        }

        inline void SetRot(bcs::CScnObj *pObj, const bcs::FVEC3 &rRot)
        {
            SetRot(pObj, rRot.x, rRot.y, rRot.z);
        }

        // The draw layer models are put in when nothing else is asked for.
        // (name guessed)
        const long DRAW_LAYER_DEFAULT = 2;

        // msw::CLoadData entries of the event objects and NPCs. (names
        // guessed)
        const long LOADDATA_OBJ = 0x5C;
        const long LOADDATA_NPC_MDL = 0xB;
        const long LOADDATA_NPC_MOT = 0x1B;
        const long NPC_NUM = 8;
    }

    CTournamentChip::~CTournamentChip()
    {
        Destroy();
    }

    void CTournamentChip::Destroy()
    {
        if (m_pMdlObj != 0) {
            m_pMdlObj->AttachToObject(0, 0, bcs::CScnObj::ATTACHMODE_0);
            m_pMdlObj->DetachFromParent();
        }
        if (m_pMdlObj != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdlObj);
        }
        m_pMdlObj = 0;
        m_nUnkB8 = 0;
    }

    inline void CTournamentChip::_CreateMdlObj()
    {
        if (m_Mdl.IsValid()) {
            m_pMdlObj = new (0) bcs::CMdlObj;
            m_pMdlObj->Create(m_Mdl, 0, 1);
            m_pMdlObj->OnFlag(0x2000);
            m_pMdlObj->SetExBlendMode(1);
            m_pMdlObj->OnFlag(0x4000);
            m_fFrame = 0.0f;
            m_pMdlObj->MatSetZModeAll(1, GX_ALWAYS, 0);
            m_pMdlObj->SetAlphaMode(bcs::CScnObj::ALPHAMODE_1);
            SetDrawPriority(-1);
        }
    }

    CTournamentChip *CTournamentChip::CreateModel(nw4r::g3d::ResMdl mdl, int bDisp, long nLayer, long nSelector)
    {
        CTournamentChip *pChip = new CTournamentChip;

        pChip->m_Mdl = mdl;
        pChip->_CreateMdlObj();
        pChip->SetDisp(bDisp);
        if (pChip->m_pMdlObj != 0) {
            bcs::CRenderMan::PushBack(pChip->m_pMdlObj, nLayer);
        }
        if (nSelector != -1) {
            pChip->SetLightSelector(nSelector);
        }
        return pChip;
    }

    void CTournamentChip::SetDrawLayer(long nLayer)
    {
        if (m_pMdlObj != 0) {
            m_pMdlObj->DetachFromParent();
            bcs::CRenderMan::PushBack(m_pMdlObj, nLayer);
        }
    }

    void CTournamentChip::SetDrawPriority(long nPriority)
    {
        if (m_pMdlObj != 0) {
            m_pMdlObj->SetDrawPriority(nPriority + 0x80, nPriority + 0x80);
        }
    }

    void CTournamentChip::AppendChild(bcs::CScnGroup *pGroup)
    {
        if (m_pMdlObj != 0) {
            m_pMdlObj->DetachFromParent();
            if (pGroup != 0) {
                pGroup->AppendChild(m_pMdlObj);
            } else {
                bcs::CRenderMan::PushBack(m_pMdlObj, DRAW_LAYER_DEFAULT);
            }
        }
    }

    void CTournamentChip::Create()
    {
        _CreateMdlObj();
        if (m_pMdlObj != 0) {
            bcs::CRenderMan::PushBack(m_pMdlObj, DRAW_LAYER_DEFAULT);
        }
    }

    void CTournamentChip::_SetPos(const bcs::FVEC3 &rPos)
    {
        if (m_pMdlObj != 0) {
            m_pMdlObj->m_vTrans = rPos;
        }
    }

    void CTournamentChip::_SetRot(const bcs::FVEC3 &rRot)
    {
        if (m_pMdlObj != 0) {
            SetRot(m_pMdlObj, rRot);
        }
    }

    void CTournamentChip::_SetScale(const bcs::FVEC3 &rScale)
    {
        if (m_pMdlObj != 0) {
            m_pMdlObj->m_vScale = rScale;
        }
    }

    void CTournamentChip::SetDisp(int bDisp)
    {
        if (m_pMdlObj != 0) {
            if (bDisp == 0) {
                m_pMdlObj->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            } else {
                m_pMdlObj->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
        }
    }

    void CTournamentChip::UpdateFrame(float fFrame)
    {
        if (m_pMdlObj == 0) {
            return;
        }

        m_fFrame += fFrame;
        if (m_fFrame >= 2.0f) {
            CTournamentRouteMan *pRouteMan = CTournamentRouteMan::GetInstance();
            if (m_nStagePoint != -1) {
                pRouteMan->SetupStagePoint(m_nStagePoint, 1);
                m_nStagePoint = -1;
            } else if (m_nStageChip != -1) {
                pRouteMan->SetupStageChip(m_nStageChip, m_nStageChipArg1, m_nStageChipArg2);
                m_nStageChip = -1;
            }

            _UpdateAlpha(fFrame);
            if (m_pMdlObj != 0) {
                float fAlpha = GetAlpha();
                if (fAlpha > 1.0f) {
                    fAlpha = 1.0f;
                }
                if (fAlpha < 0.0f) {
                    fAlpha = 0.0f;
                }
                if (fAlpha != m_pMdlObj->GetAlphaRateF()) {
                    m_pMdlObj->SetAlphaRateF(fAlpha);
                }
            }
        }
    }

    const bcs::FMTX34 *CTournamentChip::GetNodeWorldMtxPtr(unsigned long nNode) const
    {
        if (m_pMdlObj != 0 && nNode < m_pMdlObj->GetResNodeNum()) {
            return m_pMdlObj->GetNodeWorldMtxPtr(nNode);
        }
        return 0;
    }

    long CTournamentChip::GetNodeId(const char *pName) const
    {
        if (m_pMdlObj != 0) {
            return m_pMdlObj->GetNodeID(pName);
        }
        return -1;
    }

    void CTournamentChip::SetLightSelector(long nSelector)
    {
        if (m_pMdlObj != 0) {
            m_pMdlObj->SetLightSelector(nSelector);
        }
    }

    void CTournamentChip::ResetLightSelector()
    {
        if (m_pMdlObj != 0) {
            m_pMdlObj->ResetLightSelector();
        }
    }

    CTournamentTrophy::~CTournamentTrophy()
    {
        Destroy();
    }

    void CTournamentTrophy::Destroy()
    {
        if (m_pTrophy != 0) {
            m_pTrophy->Destroy();
        }
        if (m_pTrophy != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pTrophy);
        }
        m_pTrophy = 0;
        m_nUnkB4 = 0;
    }

    CTournamentTrophy *CTournamentTrophy::CreateModel(long nArc, int nTrophy, int, long nLayer, long nSelector)
    {
        CTournamentTrophy *pTrophy = new CTournamentTrophy;
        long nIndex;
        nw4r::g3d::ResFile res(
            static_cast<nw4r::g3d::ResFileData *>(EvDemo::GetArcResDataMdl(nIndex, nArc, "trophy", 0)));

        pTrophy->m_pTrophy = CTrophyObj::CreateTrophy(res, CTournament::GetCup(), CTournament::GetGameMode(), nTrophy);
        CTrophyObj::CreateTrophyIn(pTrophy->m_pTrophy, res, CTournament::GetCup(), CTournament::GetGameMode());
        CTrophyObj::SetCrystalColor(pTrophy->m_pTrophy, res, CTournament::GetGameMode(), CTournament::GetCup());
        CTrophyObj::SetCrystalEnvTex(pTrophy->m_pTrophy, "crystal_env", nArc, CTournament::GetStageGameMode(),
                                     CTournament::GetCup());
        if (nSelector != -1) {
            pTrophy->SetLightSelector(nSelector);
        }
        pTrophy->m_pTrophy->_PushBack(nLayer);
        return pTrophy;
    }

    void CTournamentTrophy::SetDrawLayer(long nLayer)
    {
        m_pTrophy->_PushBack(nLayer);
    }

    void CTournamentTrophy::SetDrawPriority(long nPriority)
    {
        m_pTrophy->_SetDrawPriority(nPriority);
    }

    void CTournamentTrophy::AppendChild(bcs::CScnGroup *pGroup)
    {
        if (pGroup != 0) {
            m_pTrophy->_PushBack(pGroup);
        } else {
            m_pTrophy->_PushBack(DRAW_LAYER_DEFAULT);
        }
    }

    void CTournamentTrophy::_SetPos(const bcs::FVEC3 &rPos)
    {
        m_pTrophy->m_vTrans = rPos;
    }

    void CTournamentTrophy::_SetRot(const bcs::FVEC3 &rRot)
    {
        SetRot(m_pTrophy, rRot);
    }

    void CTournamentTrophy::_SetScale(const bcs::FVEC3 &rScale)
    {
        m_pTrophy->m_vScale = rScale;
    }

    void CTournamentTrophy::SetBaseColor(bcs::BCS_Color color)
    {
        m_pTrophy->_SetBaseColor(color);
    }

    void CTournamentTrophy::SetColorScale(bcs::BCS_Color color)
    {
        m_pTrophy->_SetColorScale(color);
    }

    void CTournamentTrophy::SetDisp(int bDisp)
    {
        m_pTrophy->SetDisp(bDisp);
    }

    void CTournamentTrophy::UpdateFrame(float)
    {
    }

    bcs::CScnObj *CTournamentTrophy::GetScnObj()
    {
        return m_pTrophy;
    }

    const bcs::CScnObj *CTournamentTrophy::GetScnObj() const
    {
        return m_pTrophy;
    }

    const bcs::FMTX34 *CTournamentTrophy::GetNodeWorldMtxPtr(unsigned long nNode) const
    {
        if (nNode < m_pTrophy->GetResNodeNum()) {
            return m_pTrophy->GetNodeWorldMtxPtr(nNode);
        }
        return 0;
    }

    long CTournamentTrophy::GetNodeId(const char *pName) const
    {
        return m_pTrophy->GetNodeID(pName);
    }

    void CTournamentTrophy::SetLightSelector(long nSelector)
    {
        m_pTrophy->_SetLightSelector(nSelector);
    }

    void CTournamentTrophy::ResetLightSelector()
    {
        m_pTrophy->_ResetLightSelector();
    }

    long CEvMdlCreaterTmMdlObj::LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile)
    {
        // Selection 0 is the object's model in the common archive; the
        // others are files of the demo's own data, which take an entry.
        if (rInfo.nMotSel - 1 < 0) {
            rInfo.nMdlFile = rInfo.nMotFile[rInfo.nMotSel] = LOADDATA_OBJ;
            return nFile;
        }

        if (rInfo.nMotFile[rInfo.nMotSel] >= 0) {
            return nFile;
        }

        long nLoaded = CLoadData::LoadEventData(nFile, "event/demos", CEvDemoRes::GetInstance()->GetDemoName(),
                                                rInfo.nEvDataNo, 1, rInfo.nEvDataKind);
        if (nLoaded != nFile) {
            // Another entry already has the file.
            rInfo.nMdlFile = rInfo.nMotFile[rInfo.nMotSel] = nLoaded;
            return nFile;
        }

        rInfo.SetBind(0);
        rInfo.nMdlFile = rInfo.nMotFile[rInfo.nMotSel] = nFile;
        return nFile + 1;
    }

    void CEvMdlCreaterTmMdlObj::UnLoadRes(CEvDemoRes::_CHR_INFO &)
    {
    }

    int CEvMdlCreaterTmMdlObj::BindRes(CEvDemoRes::_CHR_INFO &rInfo)
    {
        if (CLoadData::IsComplete(rInfo.nMdlFile)) {
            EvDemo::InitArcResData(CLoadData::GetPtr(rInfo.nMdlFile), 0);
        }
        return 0;
    }

    CEventModel *CEvMdlCreaterTmMdlObj::CreateModel(CEvDemoRes::_CHR_INFO &rInfo)
    {
        if (rInfo.pModel != 0) {
            if (rInfo.IsCreate()) {
                CTournamentMan::GetInstance()->CleanObjModel(rInfo.nIndex, 0);
            } else {
                return rInfo.pModel;
            }
        }

        CEvDemoMdlObj::_SETUP setup(rInfo.IsDisp());
        setup.nIndex = rInfo.nIndex;
        setup.nDrawLayer = rInfo.nDrawLayer;
        setup.nShadowSize = rInfo.GetShadowSize();
        setup.nLightSelector = rInfo.nLightSelector;

        // For an object pInit names its model; the model's own init value
        // is its number as text.
        char name[32];
        sprintf(name, "m%02d", rInfo.nId);
        return CEvDemoMdlObj::CreateModel(setup, rInfo.nMdlFile, static_cast<const char *>(rInfo.pInit), name);
    }

    long CEvMdlCreaterTmMdlNpc::LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile)
    {
        // The NPC's files follow from its slot among the game's NPCs.
        if (rInfo.nMotSel - 1 < 0) {
            long nSlot = -1;
            for (long i = 0; i < NPC_NUM; i++) {
                if (rInfo.nId == CGame::GetNpcType(i)) {
                    nSlot = i;
                    break;
                }
            }
            rInfo.nMotFile[rInfo.nMotSel] = nSlot + LOADDATA_NPC_MOT;
            rInfo.nMdlFile = nSlot + LOADDATA_NPC_MDL;
            return nFile;
        }
        return nFile;
    }

    void CEvMdlCreaterTmMdlNpc::UnLoadRes(CEvDemoRes::_CHR_INFO &)
    {
    }

    int CEvMdlCreaterTmMdlNpc::BindRes(CEvDemoRes::_CHR_INFO &)
    {
        return 0;
    }

    CEventModel *CEvMdlCreaterTmMdlNpc::CreateModel(CEvDemoRes::_CHR_INFO &rInfo)
    {
        nw4r::g3d::ResFile mdlRes;
        nw4r::g3d::ResFile motRes;

        mdlRes = CLoadData::GetRes(rInfo.nMdlFile);
        motRes = CLoadData::GetRes(rInfo.nMotFile[rInfo.nMotSel]);

        if (rInfo.pModel != 0) {
            if (rInfo.IsCreate()) {
                CTournamentMan::GetInstance()->CleanObjModel(rInfo.nIndex, 0);
            } else {
                // The result is not used. (a stripped check, probably)
                rInfo.IsBind();
                return rInfo.pModel;
            }
        }

        CEvDemoMdlObj::_SETUP setup(rInfo.IsDisp());
        setup.nIndex = rInfo.nIndex;
        setup.nType = rInfo.nId;
        setup.pMdlRes = &mdlRes;
        setup.pMotRes = &motRes;
        setup.pInit = rInfo.pInit;
        setup.nDrawLayer = rInfo.nDrawLayer;
        setup.nShadowSize = rInfo.GetShadowSize();
        setup.nLightSelector = rInfo.nLightSelector;
        return CEvDemoMdlObj::CreateModel(setup, 0);
    }
}
