#include "game/msw/LayoutTask.hpp"

#include "bcs/G2D/CLytManager.hpp"

namespace msw {

    void LayoutTask::Initialize()
    {
        bcs::G2D::CLytManager::CreateInstance();
    }
}
