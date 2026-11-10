#include "game/msw/CCameraDodge.hpp"
#include "game/msw/CCameraDodgeGame.hpp"
#include "game/msw/CCameraDodgeBD.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/CWorldEnv.hpp"

void msw::CCameraDodge::Initialize()
{
    CGameCamera::Initialize();

    for (long i = 0; i < 4; i++) {
        g_pInstance[i] = 0;
    }

    g_nInstanceNum = bcs::CWorldEnv::GetViewSplitNum();

    for (long i = 0; i < g_nInstanceNum; i++) {
        CGameCamera *pCamera;

        if (CGame::g_GameWork.GetGameType() == 0) {
            pCamera = new CCameraDodgeGame(i);
        } else {
            pCamera = new CCameraDodgeBD(i);
        }
        g_pInstance[i] = pCamera;
        pCamera->SetActive(-1);
    }
}

msw::CCameraDodge::CCameraDodge(long view) : CGameCamera(view, 0x80) {}
