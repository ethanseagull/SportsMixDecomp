#include "game/msw/CCameraHockey.hpp"
#include "game/msw/CCameraHockeyGame.hpp"
#include "game/msw/CCameraHockeyCS.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/CWorldEnv.hpp"

void msw::CCameraHockey::Initialize()
{
    CGameCamera::Initialize();

    for (long i = 0; i < 4; i++) {
        g_pInstance[i] = 0;
    }

    g_nInstanceNum = bcs::CWorldEnv::GetViewSplitNum();

    for (long i = 0; i < g_nInstanceNum; i++) {
        CGameCamera *pCamera;

        switch (CGame::g_GameWork.GetGameType()) {
            case 2:
            case 3:
                pCamera = new CCameraHockeyCS(i);
                break;
            case 1:
                pCamera = new CCameraHockeyGame(i);
                break;
            default:
                pCamera = new CCameraHockeyGame(i);
                break;
        }
        g_pInstance[i] = pCamera;
        pCamera->SetActive(-1);
    }
}

msw::CCameraHockey::CCameraHockey(long view) : CGameCamera(view, 0x80) {}
