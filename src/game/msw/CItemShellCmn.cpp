#include "game/msw/CItemKinds.hpp"
#include "bcs/BCS_Vec.hpp"

// NOTE: only the EU slice of this file. Its float constants are shared with
// Create_ before it and with LoopStateBreak, CallbackBodyPush_ and the
// CItemGShell / CItemRShell code after it, so the real file is wider.

namespace msw {

    float CItemShellCmn::GetBodyCollRadius() const
    {
        return 0.2f * GetParamScale();
    }

    float CItemShellCmn::GetParamHeight() const
    {
        return 0.35f * GetParamScale();
    }

    float CItemShellCmn::GetParamAttachHeight() const
    {
        return GetParamHeight();
    }

    float CItemShellCmn::GetAtkCollRadius() const
    {
        return m_AtkColl.GetRadius(0);
    }

    // The shell breaks: collisions off, it turns round and bounces away.
    void CItemShellCmn::CtorStateBreak()
    {
        m_nFlag &= ~0x2000;
        CollBodySetEnableAll(0);
        CollAtkSetEnable(0, 0);
        SetRotYWrapped(3.1415927f);
        bcs::BCS_VecZ_RotY(m_vVelocity, -0.05f, CollAtkGetUnkA0(0));
        m_vVelocity.y = 0.125f;
        m_fGravity = -0.00625f;
        m_fUnk190 = 30.0f;
        PlaySePos(0xC4C);
    }
}
