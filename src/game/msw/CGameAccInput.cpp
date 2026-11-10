#include "game/msw/CGameAccInput.hpp"

#include "bcs/CAccInputMan.hpp"

namespace msw {

    void CGameAccInput::Initialize()
    {
        bcs::CAccInputMan::CreateInstances();
    }
}
