#include "game/msw/CStageScroll.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/CGame.hpp"

namespace msw {

    CStageScroll::CStageScroll(CStageMan *pStage)
        : m_pStage(pStage),
          m_Unk04(0),
          m_Unk08(0),
          m_Unk230(0.0f),
          m_Unk234(0),
          m_Unk238(0),
          m_Unk23C(0.0f),
          m_Unk240(0.0f),
          m_Unk244(0.0f),
          m_Unk248(0.0f),
          m_Unk25C(0.4f),
          m_Unk260(0.4f),
          m_Unk264(0.0f),
          m_Unk268(0.0f),
          m_Unk26C(0.0f),
          m_pScnObj870(0),
          m_pScnObj874(0)
    {
        for (long i = 0; i < SCROLL_NODE_NUM; i++) {
            m_aNode[i].Link(m_FreeList.GetEnd());
        }
        // Stage 0x19 (meaning not known).
        if (CGame::g_GameWork.GetStage() == 0x19) {
            m_Unk08 = 1;
        }
    }

    CStageScroll::~CStageScroll()
    {
        if (m_pScnObj874 != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pScnObj874);
        }
        m_pScnObj874 = 0;
        if (m_pScnObj870 != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pScnObj870);
        }
        m_pScnObj870 = 0;
    }
}
