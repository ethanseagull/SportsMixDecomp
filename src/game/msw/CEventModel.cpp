// msw::CEventModel and the classes defined with it in one DOL file
// (.text 0x80111CCC..0x801177F4): CEventModel, CEventMoveAction, CEventMove,
// CEvDemoMdlChr / CEventCameraNull and CEventChrCore. They share .sdata2
// constants, and CEventMove inlines CEventModel's SetLink* bodies.

#include "game/msw/CEventModel.hpp"
#include "game/msw/CEventMove.hpp"
#include "game/msw/CEventMoveAction.hpp"
#include "game/msw/CEventChrCore.hpp"
#include <stdlib.h>
#include "bcs/CMdlObj.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CEventNodeSimple.hpp"

namespace msw {

    // TODO(CEventModel): the rest of CEventModel (0x80111CCC..0x801143EC) goes
    // here, in address order. The bodies below are proven: with them in the
    // same file CEventMove's SetLink/ctor/SetUpdate inline them and match.

    void CEventModel::SetUpdateMove(int b)
    {
        _SetFlag(0x10, b);
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
        m_vUnk1C.Set(0.0f, 0.0f, 0.0f);
    }

    CEventMove::~CEventMove()
    {
    }

    void CEventMove::SetUpdate(int bUpdate)
    {
        m_pModel->SetUpdateMove(bUpdate);
    }
}

namespace msw {

    namespace {
        // The animation slot of the character the event motions play in.
        const unsigned long EVENT_ANIM_SLOT = 3;
    }

    CEventChrCore::CEventChrCore(long nType, long nPlayer, const short *pMotTable)
        : CEventModel(pMotTable)
    {
        m_pChr = CChrMan::GetChrCore(nPlayer);
        m_nType = nType;
        m_nIndex = nPlayer;
    }

    CEventChrCore::~CEventChrCore()
    {
        Destroy();
    }

    // Gives the character back its normal sound volumes.
    void CEventChrCore::Destroy()
    {
        if (CChrMan::IsActive() && m_nIndex != -1) {
            CChrCore *pChr = CChrMan::GetChrCore(m_nIndex);
            if (pChr != 0) {
                pChr->SetSeVolume(1.0f);
                pChr->SetVoiceVolume(1.0f, 0.0f);
            }
        }
        m_pChr = 0;
    }

    // Starts from where the character stands.
    void CEventChrCore::Create()
    {
        m_vPos = m_pChr->GetPos();
        m_vRot = m_pChr->GetRot();
        m_vScale.x = 1.0f;
        m_vScale.y = 1.0f;
        m_vScale.z = 1.0f;
    }

    // The character is drawn only while both the event shows it and bit
    // 0x40 of the flags is set.
    void CEventChrCore::SetDisp(int bDisp)
    {
        bool bShow = false;
        if (bDisp && (m_nFlag & 0x40)) {
            bShow = true;
        }
        CChrCore *pChr = m_pChr;
        if (!bShow) {
            pChr->OnDispFlag(4);
        } else {
            pChr->OffDispFlag(4);
        }
        if (bDisp) {
            m_nFlag |= 0x80;
        } else {
            m_nFlag &= ~0x80;
        }
    }

    void CEventChrCore::_UpdateFlags()
    {
        m_pChr->SetEnableFlagEC23((m_nFlag >> 18) & 1);
        m_pChr->SetEnableFlagEC24((m_nFlag >> 19) & 1);
    }

    // Steps the motion sequence on when the character's motion has ended.
    void CEventChrCore::UpdateFrame(float)
    {
        bool bSeq = false;
        if (m_pMotData != 0 && m_nMotIndex != -1) {
            bSeq = true;
        }
        if (bSeq && m_pChr->IsAnimEnd(EVENT_ANIM_SLOT)) {
            _NextMotion(1, 0.0f);
        }
    }

    void CEventChrCore::ChangeAnimFile(nw4r::g3d::ResFile) {}

    void CEventChrCore::SetAnimFrame(float, int) {}

    void CEventChrCore::SetAnimSpeed(float fSpeed)
    {
        m_fAnimSpeed = fSpeed;
        m_pChr->AnimSetSpeed(EVENT_ANIM_SLOT, m_fAnimSpeed);
    }

    int CEventChrCore::IsAnimResLoopType(long nMot) const
    {
        return m_pChr->IsAnimResLoopType(_GetResAnimNo(nMot));
    }

    float CEventChrCore::AnimResFrame(long nMot) const
    {
        return m_pChr->AnimGetResFrameNum(_GetResAnimNo(nMot));
    }

    float CEventChrCore::AnimResTrgFrame(long, unsigned long, float) const
    {
        return 0.0f;
    }

    // Bit 0 of nFlag plays the mirrored motion; bit 1 goes on to
    // CChrCore::AnimPlay. A motion already playing is not restarted.
    float CEventChrCore::AnimPlay(long nMot, unsigned long nFlag, float fBlend, float fFrame, int bStopSeq)
    {
        int bFlag2 = (nFlag >> 1) & 1;
        long nAnim = nMot;
        if (!(m_nFlag & 0x800)) {
            nAnim = m_pMotTable[abs(nMot)];
        }
        if (nFlag & 1) {
            nAnim = -nAnim;
        }
        if (m_nAnim != nAnim) {
            m_pChr->AnimPlay(EVENT_ANIM_SLOT, nAnim, bFlag2, fBlend, fFrame);
            m_pChr->AnimSetSpeed(EVENT_ANIM_SLOT, m_fAnimSpeed);
            m_nAnim = nAnim;
        }
        if (bStopSeq) {
            m_pMotData = 0;
            m_nMotIndex = -1;
        }
        return m_pChr->AnimGetResFrameNum(_GetResAnimNo(nMot));
    }

    void CEventChrCore::_SetPos(const bcs::FVEC3 &rPos)
    {
        m_pChr->SetPos(rPos);
    }

    void CEventChrCore::_SetRot(const bcs::FVEC3 &rRot)
    {
        m_pChr->SetRot(rRot);
    }

    void CEventChrCore::_SetScale(const bcs::FVEC3 &) {}

    bcs::CScnObj *CEventChrCore::GetScnObj()
    {
        return m_pChr->GetMdl();
    }

    const bcs::CScnObj *CEventChrCore::GetScnObj() const
    {
        return m_pChr->GetMdl();
    }

    long CEventChrCore::GetNodeId(const char *pName) const
    {
        bcs::CMdlObj *pMdl = m_pChr->GetMdl();
        if (pMdl != 0) {
            return pMdl->GetNodeID(pName);
        }
        return -1;
    }

    const bcs::FMTX34 *CEventChrCore::GetNodeWorldMtxPtr(unsigned long nNode) const
    {
        bcs::CMdlObj *pMdl = m_pChr->GetMdl();
        if (pMdl != 0 && nNode < pMdl->GetResNodeNum()) {
            return pMdl->GetNodeWorldMtxPtr(nNode);
        }
        return 0;
    }

    void CEventChrCore::SetLightSelector(long) {}

    void CEventChrCore::ResetLightSelector() {}

    void CEventChrCore::PlayVoice(long nVoice)
    {
        if (nVoice == -1) {
            return;
        }
        m_pChr->PlayVoice(nVoice, 0);
    }

    void CEventChrCore::PlaySe(long nSe)
    {
        if (nSe == -1) {
            return;
        }
        m_pChr->PlaySe(nSe, 0, -1, 0.0f, 1.0f, 0.0f);
    }

    void CEventChrCore::SetSeVolume(float fRate)
    {
        m_pChr->SetSeVolume(fRate);
    }

    void CEventChrCore::SetVoiceVolume(float fRate, float f)
    {
        m_pChr->SetVoiceVolume(fRate, f);
    }

    long CEventChrCore::GetTeamNo() const
    {
        return m_pChr->GetTeamNo();
    }

    int CEventChrCore::IsObj() const
    {
        if (m_pChr != 0) {
            return m_pChr->IsEnable();
        }
        return 0;
    }
}
