// msw::CEventModel and the classes defined with it in one DOL file
// (.text 0x80111CCC..0x80116308): CEventModel, CEventMoveAction, CEventMove,
// CEventMdlObj and CEventCameraNull. CEventMove inlines CEventModel's
// SetLink* bodies. CEvDemoMdlChr/CEventChrCore follow in their own file.

#include "game/msw/CEventModel.hpp"
#include "game/msw/CEventMove.hpp"
#include "game/msw/CEventMoveAction.hpp"
#include "game/msw/CEventChrCore.hpp"
#include "game/msw/CEvDemoMdlChr.hpp"
#include "game/msw/CEventCamera.hpp"
#include "game/msw/CEventNodeKey.hpp"
#include <stdlib.h>
#include <string.h>
#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CAnmShp.hpp"
#include "bcs/CAnmTexPat.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CCamera.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "bcs/CWorldEnv.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrMdlObj.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CEventNodeSimple.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/g3d/Camera.hpp"
#include "nw4r/g3d/G3DState.hpp"
#include "revolution/mtx.h"

namespace msw {

    // The model CEventModel makes from its model file (0x1CC bytes, vtable
    // lbl_8046F0F0; nothing names it: class and member names guessed). It
    // can be shown through the camera of a view of its own.
    class CEventMdlObj : public bcs::CMdlObj {
        public:
            CEventMdlObj();
            virtual ~CEventMdlObj();
            virtual void ObjProc(unsigned long nTask, unsigned long nArg, void *pArg);

        private:
            short m_nView;    // 0x1C8: the view whose camera shows the model
            short m_bOwnView; // 0x1CA: m_nView is used
    }; // size 0x1CC
}

namespace msw {

    CEventModel::CEventModel(const short *pMotTable)
        : m_nFlag(0xC0040), m_nIndex(-1), m_nType(-1), m_pVoiceTable(0), m_pFootSeTable(0),
          m_pMotTable(pMotTable), m_nAnim(-1), m_pMotData(0), m_pMotNameData(0), m_nMotIndex(-1),
          m_nMotLoop(-1), m_fAnimSpeed(1.0f), m_fAlpha(1.0f), m_fAlphaTarget(0.0f), m_fAlphaSpeed(0.0f),
          m_nAttachHandle(0), m_nFloorHandle(0), m_pMove(0), m_pAction(0)
    {
        m_vPosOffset.x = 0.0f;
        m_vPosOffset.y = 0.0f;
        m_vPosOffset.z = 0.0f;
        m_mdlFile = nw4r::g3d::ResFile();
        m_resFile = nw4r::g3d::ResFile();
    }

    CEventModel::~CEventModel()
    {
        CEventMove *pMove = m_pMove;
        if (pMove != 0) {
            if (m_nFlag & 4) {
                if (pMove->m_pNode != 0) {
                    delete pMove->m_pNode;
                }
                pMove->m_pNode = 0;
            }
            pMove->m_pNode = 0;
        }
        if (m_nFlag & 2) {
            if (m_pAction != 0) {
                delete m_pAction;
            }
            m_pAction = 0;
        }
        if (m_nFlag & 1) {
            if (m_pMove != 0) {
                delete m_pMove;
            }
            m_pMove = 0;
        }
        m_mdlFile = nw4r::g3d::ResFile();
        m_resFile = nw4r::g3d::ResFile();
    }

    void CEventModel::SetValidMove(int bValid)
    {
        _SetFlag(0x20, bValid);
    }

    void CEventModel::SetUpdateMove(int b)
    {
        _SetFlag(0x10, b);
    }

    void CEventModel::SetFloorMove(int bMove)
    {
        _SetFlag(0x8, bMove);
    }

    void CEventModel::SetDirectAnim(int bDirect)
    {
        _SetFlag(0x800, bDirect);
    }

    void CEventModel::SetMotionReverse(int bReverse)
    {
        _SetFlag(0x2000, bReverse);
    }

    void CEventModel::SetFloorObjEnable(int bEnable)
    {
        _SetFlag(0x4000, bEnable);
        _UpdateFlags();
    }

    void CEventModel::SetStarEffect(int bStar)
    {
        _SetFlag(0x10000, bStar);
    }

    void CEventModel::SetGroupDisp(int bDisp)
    {
        _SetFlag(0x40, bDisp);
        SetDisp((m_nFlag >> 7) & 1);
    }

    void CEventModel::SetEnableFootSe(int bEnable)
    {
        _SetFlag(0x20000, bEnable);
        _UpdateFlags();
    }

    void CEventModel::SetEnableFlag18(int bEnable)
    {
        _SetFlag(0x40000, bEnable);
        _UpdateFlags();
    }

    void CEventModel::SetEnableFlag19(int bEnable)
    {
        _SetFlag(0x80000, bEnable);
        _UpdateFlags();
    }

    int CEventModel::IsValidMove() const
    {
        return (m_nFlag >> 5) & 1;
    }

    // Shown while both the event (0x80) and the group (0x40) show it.
    int CEventModel::IsDisp() const
    {
        int bDisp = 0;
        if ((m_nFlag & 0x80) && (m_nFlag & 0x40)) {
            bDisp = 1;
        }
        return bDisp;
    }

    // Lets a moving floor under the model carry it along.
    void CEventModel::_UpdateFloor()
    {
        if (m_nFlag & 8) {
            CStageMan *pStage = CStageMan::GetInstance();
            bcs::FVEC3 vPos;
            bcs::FVEC3Sum(vPos, m_vPos, m_vPosOffset);
            if (pStage->GetFloorAttribute(vPos, CStageMan::FLOW_TYPE_0) & 2) {
                bcs::FVEC3 vFlow;
                pStage->GetFloorFlow(vFlow, vPos, CStageMan::FLOW_TYPE_0);
                bcs::FVEC3Add(&m_vPosOffset, &m_vPosOffset, &vFlow);
                bcs::FVEC3 vWork;
                bcs::FVEC3Sum(vWork, m_vPos, m_vPosOffset);
                _SetPos(vWork);
            }
        }
    }

    void CEventModel::_UpdateSe()
    {
        for (long i = 0; i < GetSeMax(); i++) {
            if (GetSndTaskSe(i) == 0) {
                SetSndTaskSe(i, 0);
            }
        }
    }

    bcs::CSndTask *CEventModel::PlaySe(long nSe, long nSlot, float f1, float f2, float f3)
    {
        StopSe(0.0f, nSlot, nSlot + 1);
        SetSndTaskSe(nSlot, CGameSnd::Play(nSe, m_vPos, 1, -1, f1, f2, f3));
        return GetSndTaskSe(nSlot);
    }

    bcs::CSndTask *CEventModel::PlaySeNoPos(long nSe, long nSlot, float f1, float f2, float f3)
    {
        StopSe(0.0f, nSlot, nSlot + 1);
        SetSndTaskSe(nSlot, CGameSnd::Play(nSe, static_cast<const bcs::FVEC3 *>(0), 1, -1, f1, f2, f3));
        return GetSndTaskSe(nSlot);
    }

    void CEventModel::StopSe(float fFade, long nFirst, long nLast)
    {
        for (long i = nFirst; i < nLast; i++) {
            bcs::CSndTask *pSnd = GetSndTaskSe(i);
            if (pSnd != 0) {
                pSnd->Stop(fFade);
            }
            SetSndTaskSe(i, 0);
        }
    }

    inline CEffectCluster *CEventModel::_GetEffect(long n)
    {
        unsigned long hEffect = GetEffcetHandle(n);
        if (hEffect == 0) {
            return 0;
        }
        return CEffectCtrl::GetInstance()->FindList(hEffect);
    }

    void CEventModel::_UpdateEffect()
    {
        for (long i = 0; i < GetEffMax(); i++) {
            if (_GetEffect(i) == 0) {
                SetEffectHandle(0, i);
                SetEffectId(-1, i);
            }
        }
    }

    void CEventModel::StopEffect(long nFirst, long nLast)
    {
        if (CEffectCtrl::GetInstance() != 0) {
            for (long i = nFirst; i < nLast; i++) {
                if (_GetEffect(i) != 0) {
                    CEffectCtrl::StopAttachEffect(GetEffcetHandle(i));
                }
                SetEffectHandle(0, i);
                SetEffectId(-1, i);
            }
        }
    }

    void CEventModel::StopEffectForce(long nFirst, long nLast)
    {
        if (CEffectCtrl::GetInstance() != 0) {
            for (long i = nFirst; i < nLast; i++) {
                if (_GetEffect(i) != 0) {
                    CEffectCtrl::StopAttachEffectForce(GetEffcetHandle(i));
                }
                SetEffectHandle(0, i);
                SetEffectId(-1, i);
            }
        }
    }

    void CEventModel::EraseEffect(long nFirst, long nLast)
    {
        if (CEffectCtrl::GetInstance() != 0) {
            for (long i = nFirst; i < nLast; i++) {
                if (_GetEffect(i) != 0) {
                    CEffectCtrl::EraseAttachEffect(GetEffcetHandle(i));
                }
                SetEffectHandle(0, i);
                SetEffectId(-1, i);
            }
        }
    }

    long CEventModel::_GetResAnimNo(long nMot) const
    {
        if (m_nFlag & 0x800) {
            return abs(nMot);
        }
        short nAnim = m_pMotTable[abs(nMot)];
        if (nAnim < 0) {
            nAnim = -nAnim;
        }
        return nAnim;
    }

    inline void CEventModel::_PlayMotion(float fFrame)
    {
        if (m_pMotData[m_nMotIndex].nMot == 0) {
            m_nMotIndex = m_nMotLoop;
            if (m_nMotIndex < 0) {
                return;
            }
        }
        const MOT_DATA *pData = &m_pMotData[m_nMotIndex];
        long nMot = pData->nMot;
        unsigned long nFlag = pData->nFlag;
        if (m_pMotData[m_nMotIndex + 1].nMot != 0 && IsAnimResLoopType(nMot)) {
            nFlag |= 2;
        }
        if (m_nFlag & 0x2000) {
            nMot = -nMot;
        }
        AnimPlay(nMot, nFlag, pData->fBlend, pData->fFrame + fFrame, 0);
    }

    void CEventModel::AnimPlay(const MOT_DATA *pData, long nIndex, float fFrame, long nLoop)
    {
        if (m_pMotData == pData) {
            return;
        }
        m_pMotData = pData;
        m_nMotIndex = nIndex;
        m_nMotLoop = nLoop;
        bool bSeq = false;
        if (m_pMotData != 0 && m_nMotIndex != -1) {
            bSeq = true;
        }
        if (bSeq) {
            _PlayMotion(fFrame);
        }
    }

    void CEventModel::_NextMotion(short nStep, float fFrame)
    {
        m_nMotIndex += nStep;
        _PlayMotion(fFrame);
    }

    inline void CEventModel::_PlayMotionName(float fFrame)
    {
        if (m_pMotNameData[m_nMotIndex].pName == 0) {
            m_nMotIndex = m_nMotLoop;
            if (m_nMotIndex < 0) {
                return;
            }
        }
        const MOT_NAME_DATA *pData = &m_pMotNameData[m_nMotIndex];
        AnimPlay(pData->pName, pData->nFlag, pData->fBlend, pData->fFrame + fFrame, 0);
    }

    void CEventModel::AnimPlay(const MOT_NAME_DATA *pData, long nIndex, float fFrame, long nLoop)
    {
        if (m_pMotNameData == pData) {
            return;
        }
        m_pMotNameData = pData;
        m_nMotIndex = nIndex;
        m_nMotLoop = nLoop;
        bool bSeq = false;
        if (m_pMotNameData != 0 && m_nMotIndex != -1) {
            bSeq = true;
        }
        if (bSeq) {
            _PlayMotionName(fFrame);
        }
    }

    void CEventModel::_NextMotionName(short nStep, float fFrame)
    {
        m_nMotIndex += nStep;
        _PlayMotionName(fFrame);
    }

    void CEventModel::SetMotionSeq(MOT_DATA *pData)
    {
        for (long i = 0;; i++) {
            if (pData[i + 1].nMot == 0) {
                pData[i].nFlag = IsAnimResLoopType(pData[i].nMot) ? 2 : 0;
                break;
            }
            pData[i].nFlag = 0;
        }
        m_pMotData = pData;
        m_nMotIndex = 0;
        m_nMotLoop = -1;
        _PlayMotion(0.0f);
    }

    void CEventModel::AttachToObject(const CEventModel *pParent, long nNode, bcs::CScnObj::AttachMode mode)
    {
        bcs::CScnObj *pObj = GetScnObj();
        if (pObj != 0) {
            pObj->AttachToObject(pParent != 0 ? pParent->GetScnObj() : 0, nNode, mode);
        }
    }

    void CEventModel::AttachToObject(const CEventModel *pParent, const char *pNode, bcs::CScnObj::AttachMode mode)
    {
        long nNode = pParent != 0 ? pParent->GetNodeId(pNode) : 0;
        if (nNode != -1) {
            AttachToObject(pParent, nNode, mode);
        }
    }

    void CEventModel::AttachToModel(CEventModel *pParent)
    {
        m_nAttachHandle = 0;
        if (pParent != 0) {
            m_nAttachHandle = pParent->GetScnObj()->GetHandle();
        }
    }

    // Places the model where the model it is attached to puts it.
    void CEventModel::_UpdateAttach()
    {
        if (m_nAttachHandle == 0) {
            return;
        }
        bcs::CScnObj *pObj = bcs::CScnObj::GetScnObj(m_nAttachHandle);
        if (pObj == 0) {
            m_nAttachHandle = 0;
            return;
        }
        pObj->CalcRootMtx();
        bcs::FMTX34 mtx;
        pObj->GetMtx(bcs::CScnObj::SCNOBJMTXTYPE_ROOT, mtx);
        PSMTXConcat(mtx.m, m_AttachMtx.m, mtx.m);
        m_vPosOffset.x = 0.0f;
        m_vPosOffset.y = 0.0f;
        m_vPosOffset.z = 0.0f;
        m_vScale.x = PSVECMag(reinterpret_cast<const Vec *>(mtx.m[0]));
        m_vScale.y = PSVECMag(reinterpret_cast<const Vec *>(mtx.m[1]));
        m_vScale.z = PSVECMag(reinterpret_cast<const Vec *>(mtx.m[2]));
        m_vPos.Set(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
        CEventNodeBase::_GetMtxRot(m_vRot, m_vScale, mtx);
        _SetPos(m_vPos);
        _SetRot(m_vRot);
        _SetScale(m_vScale);
    }

    bcs::CMdlObj *CEventModel::_CreateMdlObj(const char *pName, unsigned long nFlag)
    {
        CEventMdlObj *pMdl = new CEventMdlObj;
        nw4r::g3d::ResMdl mdl;
        if (pName != 0) {
            mdl = m_mdlFile.GetResMdl(pName);
        } else {
            mdl = m_mdlFile.GetResMdl(0);
        }
        unsigned long nAnm = 0;
        // A skeletal animation needs nothing of the model; the original
        // still asks the file.
        if ((nFlag & 1) && m_resFile.HasResAnmChr()) {
            nAnm |= 0;
        }
        if ((nFlag & 2) && m_resFile.HasResAnmClr()) {
            nAnm |= 0x108;
        }
        if ((nFlag & 4) && m_resFile.HasResAnmTexPat()) {
            nAnm |= 0x3;
        }
        if ((nFlag & 8) && m_resFile.HasResAnmTexSrt()) {
            nAnm |= 0x204;
        }
        if ((nFlag & 0x10) && m_resFile.HasResAnmShp()) {
            nAnm |= 0x7000;
        }
        pMdl->Create(mdl, nAnm, 0);
        pMdl->OnFlag(0x2000);
        pMdl->SetExBlendMode(1);
        pMdl->OnFlag(0x4000);
        return pMdl;
    }

    bcs::CAnmMotion *CEventModel::_CreateAnmMotion(bcs::CMdlObj *pMdl, unsigned long nFlag)
    {
        bcs::CAnmMotion *pAnm = 0;
        if ((nFlag & 1) && m_resFile.HasResAnmChr()) {
            pAnm = new bcs::CAnmMotion;
            pAnm->Create(pMdl, m_resFile, 0, 3, 0);
            pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_CHR);
        }
        return pAnm;
    }

    namespace {
        // Plays animation res in slot `type` of pMdl: the animation already
        // there switches to it, else a new one is made. (helper name
        // guessed; the original repeats this for the four kinds)
        template <class TAnm, class TRes>
        inline void PlayAnmObj(bcs::CMdlObj *pMdl, TAnm *pAnm, bcs::CMdlObj::AnmObjType type, const TRes &res,
                               const float &fSpeed, float fFrame)
        {
            if (pAnm != 0) {
                pAnm->ChangeRes(pMdl->GetResMdl(), res);
                pAnm->SetRate(fSpeed);
                if (fFrame > 0.0f) {
                    pAnm->SetFrame(fFrame);
                }
            } else {
                pAnm = new TAnm;
                if (pAnm->Create(pMdl, res, 0, 0)) {
                    pMdl->AttachAnmObj(pAnm, type);
                    pAnm->SetRate(fSpeed);
                    if (fFrame > 0.0f) {
                        pAnm->SetFrame(fFrame);
                    }
                } else if (pAnm != 0) {
                    delete pAnm;
                }
            }
        }
    }

    float CEventModel::_AnimPlayRes(bcs::CMdlObj *pMdl, const nw4r::g3d::ResFile &file, const char *pName,
                                    unsigned long nFlag, float fBlend, float fFrame)
    {
        long nLen;
        if (pName != 0) {
            nLen = strlen(pName);
        } else {
            nLen = 0;
        }
        if (nFlag & 1) {
            if (nLen > 0) {
                nw4r::g3d::ResAnmChr res = file.GetResAnmChr(pName);
                if (res.IsValid()) {
                    bcs::CAnmMotion *pAnm = static_cast<bcs::CAnmMotion *>(pMdl->GetAnmObjChr());
                    unsigned long nPlay = 0;
                    if (res.GetAnmPolicy() == 1) {
                        nPlay |= 2;
                    }
                    pAnm->Play(res, nPlay, m_fAnimSpeed, fBlend, fFrame);
                }
            } else if (pMdl->GetAnmObjChr() != 0) {
                static_cast<bcs::CAnmMotion *>(pMdl->GetAnmObjChr())->Stop();
            }
        }
        if (nFlag & 2) {
            if (nLen > 0) {
                nw4r::g3d::ResAnmClr res = file.GetResAnmClr(pName);
                if (res.IsValid()) {
                    PlayAnmObj(pMdl, static_cast<bcs::CAnmMatClr *>(pMdl->GetAnmObjMatClr()),
                               bcs::CMdlObj::ANMOBJTYPE_MATCLR, res, m_fAnimSpeed, fFrame);
                }
            } else if (pMdl->GetAnmObjMatClr() != 0) {
                pMdl->DetachAnmObj(pMdl->GetAnmObjMatClr());
            }
        }
        if (nFlag & 4) {
            if (nLen > 0) {
                nw4r::g3d::ResAnmTexPat res = file.GetResAnmTexPat(pName);
                if (res.IsValid()) {
                    PlayAnmObj(pMdl, static_cast<bcs::CAnmTexPat *>(pMdl->GetAnmObjTexPat()),
                               bcs::CMdlObj::ANMOBJTYPE_3, res, m_fAnimSpeed, fFrame);
                }
            } else if (pMdl->GetAnmObjTexPat() != 0) {
                pMdl->DetachAnmObj(pMdl->GetAnmObjTexPat());
            }
        }
        if (nFlag & 8) {
            if (nLen > 0) {
                nw4r::g3d::ResAnmTexSrt res = file.GetResAnmTexSrt(pName);
                if (res.IsValid()) {
                    PlayAnmObj(pMdl, static_cast<bcs::CAnmTexSrt *>(pMdl->GetAnmObjTexSrt()),
                               bcs::CMdlObj::ANMOBJTYPE_TEXSRT, res, m_fAnimSpeed, fFrame);
                }
            } else if (pMdl->GetAnmObjTexSrt() != 0) {
                pMdl->DetachAnmObj(pMdl->GetAnmObjTexSrt());
            }
        }
        if (nFlag & 0x10) {
            if (nLen > 0) {
                nw4r::g3d::ResAnmShp res = file.GetResAnmShp(pName);
                if (res.IsValid()) {
                    PlayAnmObj(pMdl, static_cast<bcs::CAnmShp *>(pMdl->GetAnmObjShp()),
                               bcs::CMdlObj::ANMOBJTYPE_5, res, m_fAnimSpeed, fFrame);
                }
            } else if (pMdl->GetAnmObjShp() != 0) {
                pMdl->DetachAnmObj(pMdl->GetAnmObjShp());
            }
        }
        return AnimResFrame(pName, nFlag);
    }

    int CEventModel::_IsAnimEnd(const bcs::CMdlObj *pMdl, unsigned long nFlag) const
    {
        bcs::CAnmObj *pAnm;
        if ((nFlag & 1) && (pAnm = pMdl->GetAnmObjChr()) != 0 && !pAnm->IsEnd()) {
            return 0;
        }
        if ((nFlag & 2) && (pAnm = pMdl->GetAnmObjMatClr()) != 0 && !pAnm->IsEnd()) {
            return 0;
        }
        if ((nFlag & 4) && (pAnm = pMdl->GetAnmObjTexPat()) != 0 && !pAnm->IsEnd()) {
            return 0;
        }
        if ((nFlag & 8) && (pAnm = pMdl->GetAnmObjTexSrt()) != 0 && !pAnm->IsEnd()) {
            return 0;
        }
        if ((nFlag & 0x10) && (pAnm = pMdl->GetAnmObjShp()) != 0 && !pAnm->IsEnd()) {
            return 0;
        }
        return 1;
    }

    float CEventModel::AnimResFrame(const char *pName, unsigned long nFlag) const
    {
        float fMax = 0.0f;
        if (pName != 0) {
            if (nFlag & 1) {
                nw4r::g3d::ResAnmChr res = m_resFile.GetResAnmChr(pName);
                if (res.IsValid()) {
                    float fFrame = res.GetNumFrame();
                    if (fMax < fFrame) {
                        fMax = fFrame;
                    }
                }
            }
            if (nFlag & 2) {
                nw4r::g3d::ResAnmClr res = m_resFile.GetResAnmClr(pName);
                if (res.IsValid()) {
                    float fFrame = res.GetNumFrame();
                    if (fMax < fFrame) {
                        fMax = fFrame;
                    }
                }
            }
            if (nFlag & 4) {
                nw4r::g3d::ResAnmTexPat res = m_resFile.GetResAnmTexPat(pName);
                if (res.IsValid()) {
                    float fFrame = res.GetNumFrame();
                    if (fMax < fFrame) {
                        fMax = fFrame;
                    }
                }
            }
            if (nFlag & 8) {
                nw4r::g3d::ResAnmTexSrt res = m_resFile.GetResAnmTexSrt(pName);
                if (res.IsValid()) {
                    float fFrame = res.GetNumFrame();
                    if (fMax < fFrame) {
                        fMax = fFrame;
                    }
                }
            }
            if (nFlag & 0x10) {
                nw4r::g3d::ResAnmShp res = m_resFile.GetResAnmShp(pName);
                if (res.IsValid()) {
                    float fFrame = res.GetNumFrame();
                    if (fMax < fFrame) {
                        fMax = fFrame;
                    }
                }
            }
        }
        return fMax;
    }

    void CEventModel::_UpdateFloorHeight(bcs::CMdlShadow *pShadow)
    {
        if (pShadow == 0) {
            return;
        }
        if (m_nFlag & 0x4000) {
            if (m_nFloorHandle != 0) {
                bcs::CScnObj *pObj = bcs::CScnObj::GetScnObj(m_nFloorHandle);
                if (pObj != 0) {
                    pShadow->SetFloorY(pObj->m_vTrans.y);
                }
            }
        } else {
            pShadow->SetFloorY(0.0f);
        }
    }

    void CEventModel::SetFloorObj(CEventModel *pObj)
    {
        if (pObj != 0) {
            bcs::CScnObj *pScnObj = pObj->GetScnObj();
            if (pScnObj != 0) {
                m_nFloorHandle = pScnObj->GetHandle();
            }
        }
    }

    void CEventModel::_UpdateAlpha(float fFrame)
    {
        if (m_fAlphaSpeed <= 0.0f) {
            return;
        }
        if (m_fAlpha < m_fAlphaTarget) {
            m_fAlpha += m_fAlphaSpeed * fFrame;
            if (m_fAlpha > m_fAlphaTarget) {
                m_fAlpha = m_fAlphaTarget;
                m_fAlphaSpeed = 0.0f;
            }
        } else if (m_fAlpha > m_fAlphaTarget) {
            m_fAlpha -= m_fAlphaSpeed * fFrame;
            if (m_fAlpha < m_fAlphaTarget) {
                m_fAlpha = m_fAlphaTarget;
                m_fAlphaSpeed = 0.0f;
            }
        } else {
            m_fAlpha = m_fAlphaTarget;
            m_fAlphaSpeed = 0.0f;
        }
    }

    // Starts node nNode of the move's path (-1 keeps the node) at frame
    // fFrame.
    int CEventModel::SetNode(long nNode, float fFrame)
    {
        CEventMove *pMove = m_pMove;
        int bValid = 0;
        pMove->m_nNodeId = nNode != -1 ? nNode : pMove->m_nNodeId;
        if (pMove->m_pNode != 0) {
            pMove->m_fEndFrame = pMove->m_pNode->GetNumFrame();
            bValid = pMove->m_pNode->IsEnable(pMove->m_nNodeId);
        }
        pMove->m_pModel->SetValidMove(bValid);
        if (bValid) {
            SetUpdateMove(1);
            m_pMove->SetFrame(fFrame);
        }
        return bValid;
    }

    void CEventModel::UpdateMove(float fFrame)
    {
        if ((m_nFlag & 0x30) == 0x30) {
            m_pMove->UpdateFrame(fFrame);
        }
    }

    void CEventModel::SetLinkNode(CEventNodeBase *pNode, int b)
    {
        CEventMove *pMove = m_pMove;
        if (pMove == 0) {
            if (b && pNode != 0) {
                delete pNode;
            }
            return;
        }
        if (m_nFlag & 4) {
            if (pMove->m_pNode != 0) {
                delete pMove->m_pNode;
            }
            pMove->m_pNode = 0;
        }
        pMove->m_pNode = pNode;
        _SetFlag(4, b);
    }

    void CEventModel::SetLinkMove(CEventMove *pMove, int b)
    {
        if (m_nFlag & 1) {
            SetLinkNode(0, 0);
            if (m_pMove != 0) {
                delete m_pMove;
            }
            m_pMove = 0;
        }
        m_pMove = pMove;
        _SetFlag(1, b);
    }

    void CEventModel::SetLinkAction(CEventMoveAction *pAction, int b)
    {
        if (m_nFlag & 2) {
            if (m_pAction != 0) {
                delete m_pAction;
            }
            m_pAction = 0;
        }
        m_pAction = pAction;
        _SetFlag(2, b);
    }

    float CEventModel::GetMoveFrame() const
    {
        if (m_pMove != 0) {
            return m_pMove->m_fEndFrame;
        }
        return 0.0f;
    }

    void CEventModel::SetMoveOffset(const bcs::FVEC3 &vOffset)
    {
        if (m_pMove != 0) {
            m_pMove->m_vOffset = vOffset;
        }
    }

    void CEventModel::SetMoveOffsetRot(const bcs::FVEC3 &vRot)
    {
        if (m_pMove != 0) {
            m_pMove->m_vOffsetRot = vRot;
        }
    }

    void CEventModel::SetMovePathRot(const bcs::FVEC3 &vRot)
    {
        if (m_pMove != 0) {
            m_pMove->m_vPathRot = vRot;
        }
    }

    void CEventModel::SetMoveFlip(long nFlip)
    {
        if (m_pMove != 0) {
            m_pMove->m_nFlip = nFlip;
        }
    }

    CEventNodeBase *CEventModel::SetMoveLinear(const bcs::FVEC3 &vMove, float fFrame, int bUpdate)
    {
        bcs::FVEC3 vZero(0.0f, 0.0f, 0.0f);
        float buf[64];

        CEventNodeKey *pNode = new CEventNodeKey;
        pNode->Clear();
        pNode->Begin(0, buf);
        pNode->AddKey(fFrame, vZero.x, 0.0f, 0.0f, 1);
        pNode->AddKey(0.0f, vMove.x, 0.0f, 0.0f, 0);
        pNode->End();
        pNode->Begin(1, buf);
        pNode->AddKey(fFrame, vZero.y, 0.0f, 0.0f, 1);
        pNode->AddKey(0.0f, vMove.y, 0.0f, 0.0f, 0);
        pNode->End();
        pNode->Begin(2, buf);
        pNode->AddKey(fFrame, vZero.z, 0.0f, 0.0f, 1);
        pNode->AddKey(0.0f, vMove.z, 0.0f, 0.0f, 0);
        pNode->End();
        pNode->Begin(3, buf);
        pNode->AddRot(vZero);
        pNode->AddKey(0.0f, 0.0f, 0.0f, 0.0f, 0);
        pNode->End();
        pNode->Finish();

        SetLinkNode(pNode, 1);
        SetMoveOffset(m_vPos);
        SetMoveOffsetRot(m_vRot);
        SetNode(0, 0.0f);
        SetUpdateMove(bUpdate);
        return pNode;
    }
}



namespace msw {

    void CEventMoveAction::SetLink(int b)
    {
        m_pModel->SetLinkAction(this, b);
    }

    // The virtual call to the inline CEventModel::SetAnimSpeed emits its
    // weak body right after this function, where the original has it.
    void CEventMoveAction::AnimPlay(long nMot, unsigned long nFlag, float fBlend, float fFrame, float fSpeed)
    {
        m_pModel->SetAnimSpeed(fSpeed);
        m_pModel->AnimPlay(nMot, nFlag, fBlend, fFrame, 1);
    }

    CEventMove::CEventMove(CEventModel *pModel, CEventNodeBase *pNode, int b)
        : m_fFrame(0.0f), m_fEndFrame(0.0f), m_pModel(pModel), m_pNode(pNode), m_nNodeId(-1), m_nFlip(0)
    {
        if (pModel != 0) {
            pModel->SetLinkMove(this, b);
        }
        m_vOffset.Set(0.0f, 0.0f, 0.0f);
        m_vOffsetRot.Set(0.0f, 0.0f, 0.0f);
        m_vPathRot.Set(0.0f, 0.0f, 0.0f);
    }

    CEventMove::~CEventMove()
    {
    }

    void CEventMove::SetUpdate(int bUpdate)
    {
        m_pModel->SetUpdateMove(bUpdate);
    }

    namespace {
        // An angle brought into [0, 2 pi]. (helper name guessed; the
        // original repeats the two loops for each angle)
        inline float WrapRad(float fRad)
        {
            while (fRad > 6.2831855f) {
                fRad -= 6.2831855f;
            }
            while (fRad < 0.0f) {
                fRad += 6.2831855f;
            }
            return fRad;
        }
    }

    // Places the model where the node path puts it at frame fFrame.
    void CEventMove::SetFrame(float fFrame)
    {
        if (m_pModel->m_nFlag & 0x20) {
            m_pNode->GetResultEx(m_vPos, m_vRot, m_vScale, fFrame, m_nNodeId, m_nFlip);
            bcs::BCS_Vec_RotY(m_vPos, m_vPos, m_vPathRot.y);
            bcs::FVEC3Add(&m_vPos, &m_vPos, &m_vOffset);
            m_pModel->SetPosition(m_vPos);
            m_vRot.x = WrapRad(m_vRot.x + m_vOffsetRot.x);
            m_vRot.y = WrapRad(m_vRot.y + m_vOffsetRot.y);
            m_vRot.z = WrapRad(m_vRot.z + m_vOffsetRot.z);
            m_pModel->SetRotation(m_vRot);
            m_pModel->SetScale(m_vScale);
            CEventMoveAction *pAction = m_pModel->GetMoveAction();
            if (pAction != 0) {
                pAction->SetFrame(fFrame);
            }
            m_fFrame = fFrame;
        }
    }

    // Moves the model fFrame frames on along the node path; the move action
    // gets what the model moved by.
    void CEventMove::UpdateFrame(float fFrame)
    {
        float fNext = m_fFrame + fFrame;
        bcs::FVEC3 vPos;
        bcs::FVEC3 vRot;
        bcs::FVEC3 vScale;
        m_pNode->GetResultEx(vPos, vRot, vScale, fNext, m_nNodeId, m_nFlip);
        bcs::BCS_Vec_RotY(vPos, vPos, m_vPathRot.y);
        m_vPos = m_pModel->GetPos();
        m_vRot = m_pModel->GetRot();
        m_vScale = m_pModel->GetScale();
        bcs::FVEC3Add(&vPos, &vPos, &m_vOffset);
        m_pModel->SetPosition(vPos);
        vRot.x = WrapRad(vRot.x + m_vOffsetRot.x);
        vRot.y = WrapRad(vRot.y + m_vOffsetRot.y);
        vRot.z = WrapRad(vRot.z + m_vOffsetRot.z);
        m_pModel->SetRotation(vRot);
        m_pModel->SetScale(vScale);
        CEventMoveAction *pAction = m_pModel->GetMoveAction();
        if (pAction != 0) {
            bcs::FVEC3 vMove;
            bcs::FVEC3Sub(&vMove, &vPos, &m_vPos);
            bcs::FVEC3 vTurn;
            bcs::FVEC3Sub(&vTurn, &vRot, &m_vRot);
            // (the original passes copies of the two differences)
            pAction->UpdateFrame(bcs::FVEC3(vMove), bcs::FVEC3(vTurn), fFrame);
        }
        m_fFrame = fNext;
    }

    CEventMdlObj::CEventMdlObj() : m_nView(0), m_bOwnView(0) {}

    CEventMdlObj::~CEventMdlObj()
    {
        AttachToObject(0, 0, static_cast<AttachMode>(0));
        DetachFromParent();
    }

    namespace {
        // Loads the camera of view nView into GX and g3d.
        inline void SetViewCamera(long nView)
        {
            nw4r::g3d::CameraData data = *reinterpret_cast<nw4r::g3d::CameraData *>(bcs::CWorldEnv::GetViewCameraData(nView));
            nw4r::g3d::Camera camera(&data);
            camera.GXSetViewport();
            camera.GXSetProjection();
            nw4r::g3d::G3DState::SetCameraProjMtx(camera, 0, true);
        }
    }

    // A model shown in another view's camera: with m_bOwnView it is placed
    // and drawn with the camera of view m_nView whatever view is drawn.
    void CEventMdlObj::ObjProc(unsigned long nTask, unsigned long nArg, void *pArg)
    {
        if (IsProcDisabled(nTask)) {
            return;
        }
        switch (nTask) {
            case 4:
                if (m_bOwnView && m_nView != (long)nArg) {
                    nw4r::g3d::CameraData data = *reinterpret_cast<nw4r::g3d::CameraData *>(bcs::CWorldEnv::GetViewCameraData(m_nView));
                    nw4r::g3d::Camera camera(&data);
                    nw4r::math::MTX34 mtx;
                    camera.GetCameraMtx(&mtx);
                    ProcCalcView(m_nView, &mtx);
                } else {
                    ProcCalcView(nArg, pArg);
                }
                break;
            case 6:
                if (m_bOwnView && m_nView != (long)nArg) {
                    SetViewCamera(m_nView);
                    ProcDrawOpa(m_nView, pArg);
                    SetViewCamera(nArg);
                    nw4r::g3d::G3DState::Invalidate(0x7FF);
                } else {
                    ProcDrawOpa(nArg, pArg);
                }
                break;
            case 7:
                if (m_bOwnView && m_nView != (long)nArg) {
                    SetViewCamera(m_nView);
                    ProcDrawXlu(m_nView, pArg);
                    SetViewCamera(nArg);
                    nw4r::g3d::G3DState::Invalidate(0x7FF);
                } else {
                    ProcDrawXlu(nArg, pArg);
                }
                break;
            default:
                bcs::CMdlObj::ObjProc(nTask, nArg, pArg);
                break;
        }
    }

    CEventCameraNull::CEventCameraNull() : m_pCamera(0) {}

    CEventCameraNull::~CEventCameraNull()
    {
        AttachToObject(0, 0, static_cast<AttachMode>(0));
        DetachFromParent();
    }

    void CEventCameraNull::ObjProc(unsigned long nTask, unsigned long nArg, void *pArg)
    {
        if (IsProcDisabled(nTask)) {
            return;
        }
        switch (nTask) {
            case 1:
                if (m_pCamera != 0) {
                    PSMTXIdentity(m_Mtx[SCNOBJMTXTYPE_ROOT].m);
                    const bcs::CCamera::VIEW_DATA &rView = m_pCamera->GetViewData();
                    bcs::FMTX34LookDir(&m_Mtx[SCNOBJMTXTYPE_ROOT], &rView.axisZ, &rView.m_Unk1C, 0);
                }
                PSMTXCopy(m_Mtx[SCNOBJMTXTYPE_ROOT].m, m_Mtx[SCNOBJMTXTYPE_1].m);
                break;
            case 4:
                CalcViewMtx(static_cast<const bcs::FMTX34 *>(pArg));
                break;
            case 0x10003:
                SetParent(0);
                break;
            case 0x10002:
                SetParent(static_cast<bcs::CG3dObj *>(pArg));
                break;
        }
    }
}
