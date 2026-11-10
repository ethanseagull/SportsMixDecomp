// The basket event demo: which demos basket plays and the telops (cockpit
// objects) they show. One file with msw::EvDemoBasket::CreateInstance and the
// two weak CEventDemoBase inlines the vtable needs.
#include "game/msw/CEventDemoBasket.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitBasket.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageBasket17.hpp"
#include "game/msw/EvDemo.hpp"

// The C library is not in include/ yet.
extern "C" int strcmp(const char *, const char *);

namespace {
    // What each demo gets from CreateDemo. What the halfwords mean is not
    // known; the trailing zeros may be padding.
    const short sVictory0[8] = { 1, 0x9B, 0x9C, 0x9D, 0x9E, 0x9D, 0x9E, 0 };
    const short sVictory1[3] = { 0x106E, 0x106B, 0x1071 };
    const short sVictory2[3] = { 0x28, 0x29, 0x29 };
    const short sVictory3[3] = { -1, -1, -1 };
    const msw::VICTORY_DATA sVictory = { { sVictory0, sVictory1, sVictory2, sVictory3 } };
    const short sEntrance[8] = { 1, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0 };
    const short sReadyStart[2] = { 1, 1 };
    const short sChangeCourt[3] = { 1, 0x99, 2 };
}

msw::CEventDemoBasket::~CEventDemoBasket()
{
    _Destroy();
}

void msw::CEventDemoBasket::CreateDemo(long nDemo, bcs::CTask *pParent)
{
    switch (nDemo) {
        case 5:
            EvDemo::CreateChangeCourtDemo(pParent, sChangeCourt);
            break;
        case 4:
            EvDemo::CreateReadyStartDemo(pParent, sReadyStart);
            break;
        case 0:
            EvDemo::CreateEntranceDemo(pParent, sEntrance);
            break;
        case 1:
            EvDemo::CreateVictoryDemo(pParent, &sVictory);
            break;
    }
}

int msw::CEventDemoBasket::IsDemoPlay(long nDemo) const
{
    switch (nDemo) {
        case 0:
        case 1:
        case 4:
            return 1;
    }
    return 0;
}

void msw::CEventDemoBasket::CreateTelop(long nObjId, long nCloseAnim)
{
    if (nObjId < 0) {
        return;
    }
    CCockpit::RemoveObj(nObjId);
    CCockpitBasket::CreateObj(nObjId, 0);
    if (nCloseAnim >= 0) {
        CCockpit::SetAnimEndClose(nObjId, nCloseAnim);
    }
    if (!IsTelop(nObjId)) {
        RegistTelop(nObjId);
    }
}

void msw::CEventDemoBasket::CreateTelop(long nObjId, long nParam, long nCloseAnim)
{
    if (nObjId < 0) {
        return;
    }
    CCockpit::RemoveObj(nObjId);
    CCockpitBasket::CreateObj(nObjId, nParam);
    if (nCloseAnim >= 0) {
        CCockpit::SetAnimEndClose(nObjId, nCloseAnim);
    }
    if (!IsTelop(nObjId)) {
        RegistTelop(nObjId);
    }
}

void msw::CEventDemoBasket::PlayEndTelop(long nObjId)
{
    if (nObjId < 0) {
        return;
    }
    if (IsTelop(nObjId)) {
        CCockpit::PlayEndAnim(nObjId);
        ClearTelop(nObjId);
    }
}

int msw::CEventDemoBasket::IsEndTelop(long nObjId)
{
    if (nObjId >= 0 && IsTelop(nObjId)) {
        return CCockpit::IsEndObj(nObjId);
    }
    return 1;
}

void msw::CEventDemoBasket::RemoveTelop(long nObjId)
{
    if (nObjId < 0) {
        return;
    }
    if (IsTelop(nObjId)) {
        CCockpit::RemoveObj(nObjId);
        ClearTelop(nObjId);
    }
}

void msw::CEventDemoBasket::PlayEndTelopAll()
{
    for (int i = 0; i < TELOP_MAX; i++) {
        if (m_nTelop[i] != -1) {
            CCockpit::PlayEndAnim(m_nTelop[i]);
            m_nTelop[i] = -1;
        }
    }
}

void msw::CEventDemoBasket::RemoveTelopAll()
{
    for (int i = 0; i < TELOP_MAX; i++) {
        if (m_nTelop[i] != -1) {
            CCockpit::RemoveObj(m_nTelop[i]);
            m_nTelop[i] = -1;
        }
    }
}

long msw::CEventDemoBasket::GetTelopId(long nTelop)
{
    static const long sTelopId[5] = { 0x1063, 0x1064, 0xF, 0xA, 0xB };

    return sTelopId[nTelop];
}

void msw::CEventDemoBasket::SetCockpitState(long nPart, long nState)
{
    switch (nPart) {
        case 0:
            if (nState == 2) {
                CCockpitBasket::SetPartMove(0, 3);
                CCockpitBasket::SetRadarIconDisp(0);
            } else if (nState == 1) {
                CCockpitBasket::SetPartMove(0, 1);
                CCockpitBasket::SetRadarIconDisp(1);
            }
            break;
        case 1:
            if (nState == 2) {
                CCockpitBasket::SetMissionCountDisp(0, 0);
            } else if (nState == 1) {
                CCockpitBasket::SetMissionCountDisp(0, 1);
            }
            break;
    }
}

void msw::CEventDemoBasket::TimerEvent(short, short, unsigned long)
{
}

void msw::CEventDemoBasket::CameraEvent(const char *pName, long nArg, long, float)
{
    if (pName == 0 || strcmp(pName, "MakeScore") != 0) {
        return;
    }
    if (nArg == 1 && CGame::g_GameWork.GetStage() == 0x11) {
        CStageBasket17::TrolleyForceReset();
    }
}

void msw::EvDemoBasket::CreateInstance(bcs::CTask *pParent)
{
    new CEventDemoBasket(pParent);
}
