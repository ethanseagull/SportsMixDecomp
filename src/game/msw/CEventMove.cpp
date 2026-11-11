#include "game/msw/CEventMove.hpp"

#include "game/msw/CEventModel.hpp"
#include "game/msw/CEventMoveAction.hpp"

// A slice of the DOL file that holds msw::CEventModel (0x80111CCC to
// 0x801177F4, see notes/toolchain.md). In the original, CEventModel's
// SetLinkAction, SetLinkMove and SetUpdateMove are defined earlier in the
// same file and inlined into the functions below; compiled on their own,
// they stay calls.

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
