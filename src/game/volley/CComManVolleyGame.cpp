#include "game/msw/CComManVolleyGame.hpp"

#include "game/msw/CChrCore.hpp"

namespace msw {

    // The character state that adds to the distance (meaning not known).
    enum {
        HANDICAP_STATE = 6
    };

    // The computer parameter read for the distance.
    enum {
        DAT_HANDICAP_DIS = 0x49
    };

    void CComManVolleyGame::CreateInstance()
    {
        if (g_pInstance == 0) {
            g_pInstance = new CComManVolleyGame;
        }
    }

    void CComManVolleyGame::Clear()
    {
    }

    void CComManVolleyGame::Main(float fFrame)
    {
    }

    float CComManVolleyGame::GetConditionHandicapDis(const CChrCore *pChr)
    {
        float fDis = 0.0f;
        if (pChr == 0) {
            return fDis;
        }

        if (pChr->GetState() == HANDICAP_STATE) {
            fDis += 8.0f;
        }

        float fLevelDis = GetDatF(DAT_HANDICAP_DIS, pChr->GetComLevelWork());
        if (GetExistPadDrvNum(pChr->GetTeamNo()) >= 1 && fLevelDis < 0.0f) {
            fLevelDis = 0.0f;
        }
        fDis += fLevelDis;
        return fDis;
    }

    CComManVolleyGame::~CComManVolleyGame()
    {
    }
}
