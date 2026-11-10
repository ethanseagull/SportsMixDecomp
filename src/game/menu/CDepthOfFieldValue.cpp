#include "game/msw/CDepthOfFieldValue.hpp"

#include <math.h>

#include "bcs/CMainApp.hpp"
#include "bcs/CRenderMan.hpp"

msw::CDepthOfFieldValue msw::CDepthOfFieldValue::s_Instance;

msw::CDepthOfFieldValue::~CDepthOfFieldValue()
{
}

void msw::CDepthOfFieldValue::Finalize()
{
    bcs::CRenderMan::EnableDepthOfFieldEffect(0);
}

void msw::CDepthOfFieldValue::Main()
{
    // The order of these declarations decides which saved float register
    // each one gets (f31, f30, f29).
    float elapsed;
    float speed;
    float duration;

    duration = m_Duration;
    elapsed = m_Elapsed;

    if (elapsed < duration) {
        speed = m_Speed;
        float frame = bcs::CMainApp::GetAppFrameRateF();
        float next = frame * speed + elapsed;

        if (next >= duration) {
            next = duration;
        }
        m_Elapsed = next;

        bcs::FVEC3 start(m_Start.m_Value0, m_Start.m_Value1, m_Start.m_Value2);
        bcs::FVEC3 end(m_End.m_Value0, m_End.m_Value1, m_End.m_Value2);
        bcs::FVEC3 cur(m_Cur.m_Value0, m_Cur.m_Value1, m_Cur.m_Value2);
        float t = next / duration;

        if (t < 0.0f) {
            t = 0.0f;
        } else if (t > 1.0f) {
            t = 1.0f;
        }

        // Slow at the start, fast at the end.
        float p = (float)pow(t, 2.5);
        float rest = 1.0f - p;
        bcs::FVEC3Blend(&cur, &start, &end, rest);
        m_Cur.m_Value0 = cur.x;
        m_Cur.m_Value1 = cur.y;
        m_Cur.m_Value2 = cur.z;

        if (next == duration) {
            m_Cur.m_Type = m_End.m_Type;
        }
        bcs::CRenderMan::SetDepthOfFieldParam(m_Cur.m_Type, m_Cur.m_Value0, m_Cur.m_Value1, m_Cur.m_Value2);
    }

    if (m_Cur.m_Value1 <= 0.0f) {
        bcs::CRenderMan::EnableDepthOfFieldEffect(0);
    } else {
        bcs::CRenderMan::EnableDepthOfFieldEffect(1);
    }
}
