// msw::CBallMarkerHockey: the marker above the puck in the hockey game.
// The same file as BasketF's game/basket/CBallMarkerBasket.cpp, with another
// display scale, another first pair of colours and no fog setting.
#include "game/msw/CBallMarkerHockey.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CGame.hpp"

// Built by the file's static initialiser. GetColor_ reads only the first
// pair; BasketF's ball marker builds the same table but for that pair, so
// it most likely comes from a shared header (which one, and the names, are
// not known).
namespace {
    // Nine pairs of colours (what each pair is for is not known).
    bcs::BCS_Color s_MarkerColor[18] = {
        bcs::BCS_Color(0x3C, 0x14, 0x00, 0xFF), bcs::BCS_Color(0xFF, 0xE6, 0x00, 0xB4),
        bcs::BCS_Color(0x8C, 0xBE, 0x6E, 0x50), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0xB4, 0xC8, 0xBE, 0x32), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0x82, 0x5A, 0x5A, 0x5A), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x32), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0xB4, 0xAA, 0x8C, 0x4B), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0x28, 0xFF, 0xFF, 0x32), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0xFF, 0x82, 0x6E, 0x32), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
        bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x3C), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
    };
}

msw::CBallMarkerHockey::CBallMarkerHockey(bcs::CTask *pParent, unsigned long nBallHandle)
    : CBallMarker(pParent, nBallHandle, 1)
{
    m_nAttachHandle = 0;
}

msw::CBallMarkerHockey::~CBallMarkerHockey()
{
}

// 1 keeps the marker down, 2 brings it up, 3 keeps it as it is (as read
// from CBallMarker's Sleep state; the meaning of 3 is a guess).
int msw::CBallMarkerHockey::AppearCheck()
{
    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallHandle));

    if (pBall == 0) {
        return 1;
    }
    if (pBall->IsSleep()) {
        return 1;
    }
    if (pBall->IsFlag4()) {
        return 1;
    }
    if (CGame::g_GameWork.m_nFlag18 & 8) {
        return 1;
    }
    if ((CGame::g_GameWork.m_nFlag18 & 0x4306) == 0) {
        if (CGame::IsArtsEventMode()) {
            return 1;
        }
        if (pBall->IsArtsShootMode()) {
            return 1;
        }
        if (pBall->IsScored()) {
            return 3;
        }
        return pBall->IsFlag100000() ? 3 : 2;
    }
    return pBall->IsFlag100000() ? 1 : 3;
}

// Keeps the marker's model on the ball, or on the model that holds it.
void msw::CBallMarkerHockey::MainFunc()
{
    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallHandle));

    if (pBall != 0 && !pBall->IsSleep()) {
        if (pBall->IsAttach()) {
            bcs::CScnObj *pHolder = pBall->GetAttachMdlObj();
            if (pHolder != 0 && m_nAttachHandle != pHolder->GetHandle()) {
                m_pMdlObj->AttachToObject(pHolder, 0, static_cast<bcs::CScnObj::AttachMode>(3));
                m_nAttachHandle = pHolder->GetHandle();
            }
        } else if (m_nAttachHandle != 0) {
            m_pMdlObj->AttachToObject(pBall->GetMdlObj(), 0, static_cast<bcs::CScnObj::AttachMode>(3));
            m_nAttachHandle = 0;
        }
    }
}

float msw::CBallMarkerHockey::GetAppearFrameNum() const
{
    return 16.0f;
}

float msw::CBallMarkerHockey::GetDisappearFrameNum() const
{
    return 12.0f;
}

float msw::CBallMarkerHockey::GetDispScale() const
{
    return 4.0f;
}

void msw::CBallMarkerHockey::GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long)
{
    rColor0 = s_MarkerColor[0];
    rColor1 = s_MarkerColor[1];
}
