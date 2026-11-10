// The court selection screen's models: the court (CCourtTableSetObj), the
// characters standing on it (CCourtCharaObj) and the ball (CCourtBallMdlObj).
//
// The three vtables are in .data in the reverse order of the classes here,
// so the headers are included in this order.
#include "game/msw/SelectMenu/CCourtTableSetObj.hpp"
#include "game/msw/SelectMenu/CCourtCharaObj.hpp"
#include "game/msw/SelectMenu/CCourtBallMdlObj.hpp"

#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CMainApp.hpp"
#include "game/msw/CChrMenuCtrl.hpp"
#include "game/msw/CChrMenuObj.hpp"
#include "math.h"

float msw::SelectMenu::CCourtCharaObj::s_FadeInOutTime = 4.0f;

msw::SelectMenu::CCourtTableSetObj::CCourtTableSetObj(bcs::CMdlObj *pMdl)
    : CTableSetMdlObj(pMdl)
{
}

msw::SelectMenu::CCourtTableSetObj::~CCourtTableSetObj()
{
}

msw::SelectMenu::CCourtCharaObj::CCourtCharaObj(unsigned long)
{
    m_nState = STATE_CHECK;
    m_fAlpha = 0.0f;
    m_hMdl = 0;
    SetNextMdlHandle(0);
}

msw::SelectMenu::CCourtCharaObj::~CCourtCharaObj()
{
    CChrMenuCtrl::s_Instance.DeleteChrMenuObj(m_hMdl);
    CChrMenuCtrl::s_Instance.DeleteChrMenuObj(m_hNextMdl);
}

bcs::CMdlObj *msw::SelectMenu::CCourtCharaObj::GetMdlObj(unsigned long hMdl)
{
    return static_cast<bcs::CMdlObj *>(bcs::CScnObj::GetScnObj(hMdl));
}

inline void msw::SelectMenu::CCourtCharaObj::ApplyNextMdl()
{
    if (m_hNextMdl != 0) {
        if (m_hMdl != 0) {
            CChrMenuCtrl::s_Instance.DeleteChrMenuObj(m_hMdl);
        }
        m_hMdl = m_hNextMdl;
        SetNextMdlHandle(0);
    }
}

inline void msw::SelectMenu::CCourtCharaObj::Hide()
{
    bcs::CMdlObj *pMdl = GetMdlObj(m_hMdl);
    if (pMdl != 0) {
        pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
    }
}

inline void msw::SelectMenu::CCourtCharaObj::Show()
{
    bcs::CMdlObj *pMdl = GetMdlObj(m_hMdl);
    if (pMdl != 0) {
        pMdl->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
    }
}

inline void msw::SelectMenu::CCourtCharaObj::SetAlpha(float alpha)
{
    m_fAlpha = alpha;
    CChrMenuObj *pObj = CChrMenuCtrl::s_Instance.GetChrMenuObj(m_hMdl);
    if (pObj != 0) {
        pObj->SetAlpha(alpha);
    }
}

namespace {

    // (names guessed) The fade value one frame further in or out, for a
    // fade lasting `time` frames.
    inline float FadeIn(float alpha, float time)
    {
        return alpha + 1.0f / time * bcs::CMainApp::GetAppFrameRateF();
    }

    inline float FadeOut(float alpha, float time)
    {
        return alpha - 1.0f / time * bcs::CMainApp::GetAppFrameRateF();
    }
}

void msw::SelectMenu::CCourtCharaObj::StateCheck()
{
    switch (m_nState) {
    case STATE_CHECK:
        if (m_nSubState == 0 && m_hNextMdl != 0) {
            ApplyNextMdl();
            Hide();
            m_nSubState = 1;
        }
        break;
    case STATE_IN:
        StateIn();
        break;
    case STATE_OUT:
        StateOut();
        break;
    case STATE_CHARA_CHANGE:
        StateCharaChange();
        break;
    }

    bcs::CMdlObj *pMdl = GetMdlObj(m_hMdl);
    if (pMdl != 0) {
        bcs::CAnmMotion *pAnm = static_cast<bcs::CAnmMotion *>(pMdl->GetAnmObjChr());
        if (pAnm != 0 && pAnm->VFunc64(3, 0) == 2 && pAnm->IsEnd()) {
            unsigned long flag = 2;
            if (pAnm->VFunc98(3, 0)) {
                flag |= 1;
            }
            pAnm->Play(3, flag, 1.0f, 0.0f, 0.0f);
        }
    }
}

void msw::SelectMenu::CCourtCharaObj::StateIn()
{
    float alpha;

    switch (m_nSubState) {
    case 0:
        ApplyNextMdl();
        if (m_hMdl == 0) {
            m_nSubState = -1;
            break;
        }
        Show();
        alpha = 0.0f;
        SetAlpha(alpha);
        m_nSubState = 1;
        break;
    case 1:
        alpha = FadeIn(m_fAlpha, s_FadeInOutTime);
        if (alpha >= 1.0f) {
            alpha = 1.0f;
            m_nSubState = 2;
        }
        SetAlpha(alpha);
        break;
    }
}

void msw::SelectMenu::CCourtCharaObj::StateOut()
{
    float alpha;

    switch (m_nSubState) {
    case 0:
        if (m_hMdl == 0) {
            ApplyNextMdl();
            Hide();
            m_nSubState = -1;
            break;
        }
        Show();
        m_nSubState = 1;
        // fall through
    case 1:
        alpha = FadeOut(m_fAlpha, s_FadeInOutTime);
        if (alpha <= 0.0f) {
            alpha = 0.0f;
            m_nSubState = 2;
            ApplyNextMdl();
            Hide();
        }
        SetAlpha(alpha);
        break;
    }
}

void msw::SelectMenu::CCourtCharaObj::StateCharaChange()
{
    float alpha;

    switch (m_nSubState) {
    case 0:
        if (m_hMdl == 0) {
            ApplyNextMdl();
            // Dead store kept for the match: the original stores 3 here and 1 just after it.
            m_nSubState = 3;
        }
        Show();
        m_nSubState = 1;
        break;
    case 1:
        alpha = FadeOut(m_fAlpha, s_FadeInOutTime);
        if (alpha <= 0.0f) {
            alpha = 0.0f;
            m_nSubState = 2;
        }
        SetAlpha(alpha);
        break;
    case 2:
        if (m_hNextMdl != 0) {
            ApplyNextMdl();
            m_nSubState = 3;
        } else {
            m_nSubState = -1;
        }
        break;
    case 3:
        if (m_hMdl == 0) {
            m_nSubState = -1;
            break;
        }
        Show();
        alpha = 0.0f;
        SetAlpha(alpha);
        m_nSubState = 4;
        break;
    case 4:
        alpha = FadeIn(m_fAlpha, s_FadeInOutTime);
        if (alpha >= 1.0f) {
            alpha = 1.0f;
            m_nSubState = 5;
        }
        SetAlpha(alpha);
        break;
    }
}

void msw::SelectMenu::CCourtCharaObj::SetNextMdlHandle(unsigned long hMdl)
{
    if (m_hNextMdl != 0 && m_hNextMdl != m_hMdl) {
        CChrMenuCtrl::s_Instance.DeleteChrMenuObj(m_hNextMdl);
    }
    m_hNextMdl = hMdl;
}

void msw::SelectMenu::CCourtCharaObj::ChangeState(long state, int bForce)
{
    if (state != m_nState || bForce == 1) {
        m_nState = state;
        m_nSubState = 0;
    }
}

namespace {

    // An angle brought into [0, 2 pi).
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
}

msw::SelectMenu::CCourtBallMdlObj::CCourtBallMdlObj()
    : m_fMoveTime(0.0f)
    , m_fMoveTimer(0.0f)
    , m_vStartPos(0.0f, 0.0f, 0.0f)
    , m_vEndPos(0.0f, 0.0f, 0.0f)
    , m_vRot(0.0f, 0.0f, 0.0f)
    , m_fRotSpeedMax(0.0f)
    , m_nRotMode(0)
    , m_fRotSpeed(0.0f)
{
}

// The original stores the vtable before calling the base destructor, which
// the compiler does only when the body calls something; it was most likely a
// call that is empty in this build (same stand-in as in
// game/boss/CCockpitBossLayoutBase.cpp).
static inline void Nothing() {}

msw::SelectMenu::CCourtBallMdlObj::~CCourtBallMdlObj()
{
    Nothing();
}

void msw::SelectMenu::CCourtBallMdlObj::Main()
{
    static const float rotYSpeedDiv = 15.0f;

    float rate = bcs::CMainApp::GetAppFrameRateF();
    float timer = m_fMoveTimer;
    float time = m_fMoveTime;

    if (timer >= 1.0f && time >= 1.0f) {
        float t = timer / time;
        if (t < 0.0f) {
            t = 0.0f;
        } else if (t > 1.0f) {
            t = 1.0f;
        }
        bcs::FVEC3 pos;
        bcs::FVEC3Lerp(&pos, &m_vStartPos, &m_vEndPos, 1.0f - static_cast<float>(pow(t, 2.5)));
        m_vTrans = pos;
        CalcRootMtx();

        float left = timer - rate;
        if (left < 1.0f) {
            left = 0.0f;
            m_fRotSpeed = 0.0f;
        }
        m_fMoveTimer = left;
        return;
    }

    m_vTrans = m_vEndPos;
    if (m_nRotMode != 1) {
        float speed = rate * m_fRotSpeed;
        if (m_nRotMode == 2) {
            speed -= m_fRotSpeedMax / rotYSpeedDiv;
            if (speed < -m_fRotSpeedMax) {
                speed = -m_fRotSpeedMax;
            }
        } else {
            speed += m_fRotSpeedMax / rotYSpeedDiv;
            if (speed > m_fRotSpeedMax) {
                speed = m_fRotSpeedMax;
            }
        }
        m_fRotSpeed = speed;

        float rotY = m_fRotY + speed;
        // (guessed) The original wraps the angle once more here and drops
        // the result.
        WrapRad(rotY);
        m_fRotX = WrapRad(m_vRot.x);
        m_fRotY = WrapRad(rotY);
        m_fRotZ = WrapRad(m_vRot.z);
    }
    CalcRootMtx();
}

void msw::SelectMenu::CCourtBallMdlObj::Play(nw4r::g3d::ResAnmChr res, float rate, unsigned long flag)
{
    if (res.IsValid()) {
        bcs::CAnmMotion *pAnm = static_cast<bcs::CAnmMotion *>(GetAnmObjChr());
        if (pAnm != 0) {
            float frame = pAnm->GetFrame();
            pAnm->Play(res, flag | 2, 1.0f, 0.0f, 0.0f);
            pAnm->SetRate(rate);
            pAnm->SetFrame(frame);
        }
    }
}

void msw::SelectMenu::CCourtBallMdlObj::Play(nw4r::g3d::ResAnmClr res, float)
{
    if (res.IsValid()) {
        bcs::CAnmObj *pOld = GetAnmObjMatClr();
        if (pOld != 0) {
            DetachAnmObj(ANMOBJTYPE_MATCLR);
            if (pOld != 0) {
                delete pOld;
            }
        }

        bcs::CAnmMatClr *pAnm = new bcs::CAnmMatClr;
        if (pAnm->Create(this, res, 0, 0) == 1) {
            AttachAnmObj(pAnm, ANMOBJTYPE_MATCLR);
        } else if (pAnm != 0) {
            delete pAnm;
        }
    }
}

void msw::SelectMenu::CCourtBallMdlObj::Play(nw4r::g3d::ResAnmTexSrt res, float)
{
    if (res.IsValid()) {
        bcs::CAnmObj *pOld = GetAnmObjTexSrt();
        if (pOld != 0) {
            DetachAnmObj(ANMOBJTYPE_TEXSRT);
            if (pOld != 0) {
                delete pOld;
            }
        }

        bcs::CAnmTexSrt *pAnm = new bcs::CAnmTexSrt;
        if (pAnm->Create(this, res, 0, 0) == 1) {
            AttachAnmObj(pAnm, ANMOBJTYPE_TEXSRT);
        } else if (pAnm != 0) {
            delete pAnm;
        }
    }
}

void msw::SelectMenu::CCourtBallMdlObj::SetGoldColor(int bGold)
{
    if (bGold) {
        SetKColors(bcs::BCS_Color(0xFFDCA000), bcs::BCS_Color(0x000000FF));
    } else {
        RestoreMatTevKColor(GX_KCOLOR0);
        RestoreMatTevKColor(GX_KCOLOR1);
    }
}
