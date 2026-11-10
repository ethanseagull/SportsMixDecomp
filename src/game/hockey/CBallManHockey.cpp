// msw::CBallManHockey: the ball manager of the hockey game.

// Must come first: it stands in for bcs/CTask.hpp (see the file).
#include "bcs/CTask.hpp"

#include "game/msw/CBallManHockey.hpp"

#include "bcs/New.hpp"
#include "game/msw/CBallHockey.hpp"
#include "game/msw/CBallMarkerHockey.hpp"

msw::CBallMan *msw::CBallManHockey::CreateInstance(bcs::CTask *pParent)
{
    if (CBallMan::m_pInstance == 0) {
        CBallMan::m_pInstance = new (0) CBallManHockey(pParent, 4);
    }
    return CBallMan::m_pInstance;
}

msw::CBallManHockey::~CBallManHockey()
{
}

// One ball, and its marker.
void msw::CBallManHockey::Create_()
{
    DeleteAllBall_();

    CBall *pBall = new (0) CBallHockey(this, 0);
    pBall->Create(-1, 0x40);
    m_pBallHandle[0] = pBall->GetId();

    new (0x100) CBallMarkerHockey(this, pBall->GetId());
}
