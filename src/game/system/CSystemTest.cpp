// A test task. DebugF's module entry point creates it; it does nothing
// each frame.

#include "game/msw/CSystemTest.hpp"

namespace msw {

    CSystemTest::CSystemTest() : bcs::CTask(0, -1)
    {
        m_Unk34 = 1234;
    }

    void CSystemTest::Main()
    {
    }

    CSystemTest::~CSystemTest()
    {
    }
}
