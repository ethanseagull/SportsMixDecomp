#include "game/msw/CCameraBasket.hpp"
#include "game/msw/CCameraBasketGame.hpp"
#include "game/msw/CCameraBasketHB.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/CWorldEnv.hpp"

void msw::CCameraBasket::Initialize()
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
                pCamera = new CCameraBasketHB(i);
                break;
            case 1:
                pCamera = new CCameraBasketGame(i);
                break;
            default:
                pCamera = new CCameraBasketGame(i);
                break;
        }
        g_pInstance[i] = pCamera;
        pCamera->SetActive(-1);
    }
}

msw::CCameraBasket::CCameraBasket(long view) : CGameCamera(view, 0x80) {}
