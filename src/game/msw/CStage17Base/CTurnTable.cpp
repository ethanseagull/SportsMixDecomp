#include "game/msw/CStage17Base.hpp"

namespace msw {

    CStage17Base::CTurnTable::CTurnTable(CStage17Base *pOwner)
        : m_pOwner(pOwner),
          m_nIdx(0),
          m_Unk08(0.0f),
          m_Unk0C(0.0f),
          m_Unk10(0.0f),
          m_Unk14(0.0f),
          m_Unk18(0.0f),
          m_Unk1C(0.0f),
          m_Unk20(0.0f),
          m_Unk24(0.0f),
          m_Unk28(0.0f),
          m_Unk2C(0.0f),
          m_Unk30(0.0f),
          m_Unk38(0),
          m_Unk44(0.0f),
          m_Unk4C(0)
    {
    }
}
