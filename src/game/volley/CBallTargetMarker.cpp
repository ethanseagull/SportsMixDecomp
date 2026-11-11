// The ball markers of the volley game: msw::CBallTargetMarker (the marker on
// the court where the ball will come down), its vs-boss and rhythm-receive
// variants, and msw::CBallMarkerVolley (the marker on the ball).
//
// INCOMPLETE: only CBallMarkerVolley and the colour table are written. The
// target markers' functions (constructors, Create_, Main, the states,
// SetHeightObjState_, UpdateViewFlag_, UpdateColor_ and the rhythm-receive
// functions) come first in the original file and are not written yet.
#include "game/msw/CBallMarkerVolley.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMan.hpp"

// Built by the file's static initialiser: four colours for each of nine
// kinds, the pair shown on the user team's side and the pair shown on the
// other side (as GetColor_ blends them). All nine kinds are the same.
namespace {
    bcs::BCS_Color s_MarkerColor[9][4] = {
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
    };
}

msw::CBallMarkerVolley::CBallMarkerVolley(bcs::CTask *pParent, unsigned long nBallHandle)
    : CBallMarker(pParent, nBallHandle, 1)
{
    m_fRate = 0.0f;
}

msw::CBallMarkerVolley::~CBallMarkerVolley()
{
}

// 1 keeps the marker down, 2 brings it up, 3 keeps it as it is (as read
// from CBallMarker's Sleep state; the meaning of 3 is a guess).
int msw::CBallMarkerVolley::AppearCheck()
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
    if (pBall->GetTouchNum() > 0 && !pBall->IsVolleyNetHit()) {
        return 3;
    }
    if (pBall->IsAttach()) {
        return 3;
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

// Moves m_fRate towards 1 while the ball is on the far side of the net from
// the user team's goal, towards 0 otherwise.
void msw::CBallMarkerVolley::MainFunc()
{
    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallHandle));

    if (pBall != 0) {
        // (guess) A change time of 0 frames: the rate moves by 1 a frame.
        float fStep = 1.0f;
        if (m_nState != 0) {
            float fFrames = 0.0f;
            if (fFrames > fStep) {
                fStep = fStep / fFrames;
            }
        }

        bcs::FVEC3 netPos(CStageMan::GetInstance()->GetNetPos());
        int bFar = 0;
        if (CGame::GetTargetGoalIdx(CGame::g_GameWork.GetUserTeam()) == 0) {
            if (pBall->GetPos().z > netPos.z) {
                bFar = 1;
            }
        } else {
            if (pBall->GetPos().z < netPos.z) {
                bFar = 1;
            }
        }
        if (bFar) {
            m_fRate += fStep * GetFrameRateF();
        } else {
            m_fRate -= fStep * GetFrameRateF();
        }
        m_fRate = m_fRate > 1.0f ? 1.0f : (m_fRate < 0.0f ? 0.0f : m_fRate);
    }
}

float msw::CBallMarkerVolley::GetAppearFrameNum() const
{
    return 16.0f;
}

float msw::CBallMarkerVolley::GetDisappearFrameNum() const
{
    return 12.0f;
}

// From fA on the user team's side to fB on the other.
float msw::CBallMarkerVolley::GetDispScale() const
{
    static const float fB = 1.7f;
    static const float fA = 1.4f;
    float rate = m_fRate;
    return fA + (fB - fA) * rate;
}

void msw::CBallMarkerVolley::GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long nIdx)
{
    bcs::BCS_Color near0, far0, near1, far1;
    GetColor_(near0, near1, far0, far1, nIdx);
    rColor0 = bcs::BCS_Color_Lerp(near0, far0, m_fRate);
    rColor1 = bcs::BCS_Color_Lerp(near1, far1, m_fRate);
}

void msw::CBallMarkerVolley::GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, bcs::BCS_Color &rColor2,
                                       bcs::BCS_Color &rColor3, long nIdx)
{
    long kind = 0;
    switch (nIdx) {
        case 3:
            kind = 1;
            break;
        case 9:
            kind = 2;
            break;
        case 2:
            kind = 3;
            break;
        case 6:
            kind = 4;
            break;
        case 7:
            kind = 5;
            break;
        case 10:
            kind = 6;
            break;
        case 11:
            kind = 7;
            break;
        case 17:
            kind = 8;
            break;
    }
    rColor0 = s_MarkerColor[kind][0];
    rColor1 = s_MarkerColor[kind][1];
    rColor2 = s_MarkerColor[kind][2];
    rColor3 = s_MarkerColor[kind][3];
}
