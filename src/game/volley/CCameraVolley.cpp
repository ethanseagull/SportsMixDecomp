#include "game/msw/CCameraVolley.hpp"
#include "game/msw/CCameraVolleyBoss.hpp"
#include "game/msw/CCameraVolleyGame.hpp"
#include "game/msw/CCameraVolleyRR.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/CWorldEnv.hpp"

void msw::CCameraVolley::Initialize()
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
                pCamera = new CCameraVolleyRR(i);
                break;
            case 1:
                pCamera = new CCameraVolleyBoss(i);
                break;
            default:
                pCamera = new CCameraVolleyGame(i);
                break;
        }
        g_pInstance[i] = pCamera;
        pCamera->SetActive(-1);
    }
}

msw::CCameraVolley::CCameraVolley(long view) : CGameCamera(view, 0x80) {}
