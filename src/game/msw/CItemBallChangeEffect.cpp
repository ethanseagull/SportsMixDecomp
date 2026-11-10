#include "game/msw/CItemBallChangeEffect.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CItemBall.hpp"
#include "bcs/CTaskMan.hpp"

// The original file is wider than this unit: it starts with msw::CItemBall
// (game/msw/CItemBall.cpp and the functions after it share its float
// constants) and goes on with CItemBallChangeEffect::Main and weak inlines
// up to 0x800CFCF8.

namespace msw {

    CItemBallChangeEffect::CItemBallChangeEffect(bcs::CTask *pParent, unsigned long nBallId)
        : bcs::CTask(pParent, 0x19), m_nBallId(nBallId), m_nStep(0), m_fTimer(0.0f), m_fBlinkTimer(0.0f)
    {
        ClearUnk08(4);
        m_fTime = 60.0f;
        CGame::StartStopEffect(m_fTime, 16.0f, 0);
    }

    CItemBallChangeEffect::~CItemBallChangeEffect() {
    }

    void CItemBallChangeEffect::UpdateItemDisp(int bEnd) {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall == 0) {
            return;
        }
        CItemBall *pItem = pBall->GetItemEffect_();
        if (pItem == 0) {
            return;
        }
        if (bEnd) {
            pItem->ChgDemoDisp(0);
            return;
        }
        if (m_fBlinkTimer >= 0.5f * m_fTime) {
            pItem->ChgDemoDisp(1);
        } else {
            pItem->ChgDemoDisp(0);
        }
        if (m_fBlinkTimer >= m_fTime) {
            m_fBlinkTimer -= m_fTime;
        }
    }

    void CItemBallChangeEffect::Main() {
        float rate = GetFrameRateF();
        m_fTimer += rate;
        m_fBlinkTimer += 4.0f * rate;
        switch (m_nStep) {
            case 0:
                m_nStep++;
                break;
            case 1:
                SetStartEffect();
                m_nStep++;
                break;
            case 2:
                if (m_fTimer >= m_fTime) {
                    UpdateItemDisp(1);
                    SetEndEffect();
                    Delete();
                    m_nStep++;
                } else {
                    UpdateItemDisp(0);
                }
                break;
            default:
                if (!IsDelete()) {
                    Delete();
                }
                break;
        }
    }
}
