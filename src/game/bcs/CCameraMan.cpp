#include "bcs/CCameraMan.hpp"
#include "bcs/CCamera.hpp"

namespace bcs {

    CCamera *CCameraMan::m_pCamera[VIEW_MAX][ENTRY_MAX];

    // Asks every entered camera to delete itself and empties the table.
    void CCameraMan::RemoveEntryAll()
    {
        for (long nView = 0; nView < VIEW_MAX; nView++) {
            for (long i = 0; i < ENTRY_MAX; i++) {
                CCamera *pCamera = m_pCamera[nView][i];
                if (pCamera != 0) {
                    pCamera->Delete();
                    m_pCamera[nView][i] = 0;
                }
            }
        }
    }

    CCamera *CCameraMan::GetActiveCamera(long nView)
    {
        return m_pCamera[nView][0];
    }

    int CCameraMan::IsDbgCamera(long nView)
    {
        CCamera *pCamera = m_pCamera[nView][0];
        if (pCamera != 0 && pCamera->GetCamPriority() <= 0x10) {
            return 1;
        }
        return 0;
    }
}
