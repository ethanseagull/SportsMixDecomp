#include "game/msw/CMenuPad.hpp"

#include "bcs/CLoadMan.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CPadWI.hpp"

namespace msw {

    // Mode values of PointerState::m_nMode.
    enum {
        INPUT_MODE_POINTER = 0,
        INPUT_MODE_STICK = 1,
    };

    int CMenuPad::IsInputPointerMode(long nPad)
    {
        return m_Pointer[nPad].m_nMode == INPUT_MODE_POINTER;
    }

    void CMenuPad::UpdateInputMode()
    {
        float fRate = bcs::CMainApp::GetAppFrameRateF();

        for (long i = 0; i < 4; i++) {
            bcs::FVEC2 pos(0.0f, 0.0f);
            bcs::CPadWI::GetScreenPos_F(&pos, m_Chan[i].m_nChan);

            PointerState &state = m_Pointer[i];
            state.PushHistory(pos);

            float fTime = state.m_fModeTime;
            if (fTime > 0.0f) {
                fTime -= fRate;
                if (fTime < 0.0f) {
                    fTime = 0.0f;
                }
                state.m_fModeTime = fTime;
            }

            if (IsInputPointerMode(i) == 1) {
                // Any direction button leaves pointer mode, once the hold
                // time is over.
                if (!(fTime > 0.0f) && IsInput(i, 0xF) == 1) {
                    ChangeInputStickModeForce(i);
                }
            } else if (IsInput(i, 0x10000000) == 1) {
                state.m_fModeTime = 30.0f;
            } else if (m_nPointerOnObj[i] != 0) {
                if (fTime <= 0.0f) {
                    ChangeInputPointerModeForce(i);
                }
            } else {
                state.m_fModeTime = 30.0f;
            }
        }
    }

    void CMenuPad::ChangeInputPointerModeForce(long nPad)
    {
        m_Pointer[nPad].m_nMode = INPUT_MODE_POINTER;
        m_Pointer[nPad].m_fModeTime = 5.0f;
    }

    void CMenuPad::ChangeInputStickModeForce(long nPad)
    {
        m_Pointer[nPad].m_nMode = INPUT_MODE_STICK;
        m_nPointerOnObj[nPad] = 0;
        m_Pointer[nPad].m_fModeTime = 30.0f;
    }

    int CMenuPad::IsConnect(long nPad)
    {
        return bcs::CPadWI::IsConnect(m_Chan[nPad].m_nChan);
    }

    int CMenuPad::IsNunchakuStyle(long nPad)
    {
        return bcs::CPadWI::IsNunchukStyle(m_Chan[nPad].m_nChan);
    }

    void CMenuPad::UpdateNoInputCount()
    {
        if (!bcs::CLoadMan::IsComplete()) {
            ResetNoInputCountTime();
            return;
        }

        float fRate = bcs::CMainApp::GetAppFrameRateF();
        for (long i = 0; i < 4; i++) {
            if (GetInput(i) != 0 || m_fUnk488[i] > 0.1f) {
                m_fNoInputTime[i] = m_fNoInputLimit;
            } else {
                m_fNoInputTime[i] -= fRate;
                if (m_fNoInputTime[i] < 0.0f) {
                    m_fNoInputTime[i] = 0.0f;
                }
            }
        }
    }

    void CMenuPad::ResetNoInputCountTime(float fTime)
    {
        m_fNoInputLimit = fTime;
        ResetNoInputCountTime();
    }

    void CMenuPad::ResetNoInputCountTime()
    {
        float fTime = m_fNoInputLimit;

        m_fNoInputTime[0] = fTime;
        m_fNoInputTime[1] = fTime;
        m_fNoInputTime[2] = fTime;
        m_fNoInputTime[3] = fTime;
    }
}
