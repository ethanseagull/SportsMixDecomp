#include "game/msw/CGameBGM.hpp"

namespace msw {

    CGameBGM *CGameBGM::s_pInstance;

    void CGameBGM::SetBgmState(long state)
    {
        if (s_pInstance == 0) {
            return;
        }

        long cur = GetBgmState();
        // States 4 and 5 neither restart themselves nor replace states 6 to
        // 8.
        if (state == 4) {
            if (cur == 4) {
                return;
            }
            if (cur == 6) {
                return;
            }
            if (cur == 7) {
                return;
            }
            if (cur == 8) {
                return;
            }
        }
        if (state == 5) {
            if (cur == 5) {
                return;
            }
            if (cur == 6) {
                return;
            }
            if (cur == 7) {
                return;
            }
            if (cur == 8) {
                return;
            }
        }
        s_pInstance->RequestState(state);
    }

    long CGameBGM::GetBgmState()
    {
        if (s_pInstance != 0) {
            return s_pInstance->m_nState;
        }
        return 0;
    }
}
