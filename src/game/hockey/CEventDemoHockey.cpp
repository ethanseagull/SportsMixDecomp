// The hockey event demo: which demos hockey plays and the telops (cockpit
// objects) they show. One file with msw::EvDemoHockey::CreateInstance and the
// two weak CEventDemoBase inlines the vtable needs.
#include "game/msw/CEventDemoHockey.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitHockey.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageHockey17.hpp"
#include "game/msw/EvDemo.hpp"

// The C library is not in include/ yet.
extern "C" int strcmp(const char *, const char *);

namespace {
    // What each demo gets from CreateDemo. What the halfwords mean is not
    // known; the trailing zeros may be padding.
    const short sVictory0[8] = { 1, 0xC1, 0xC2, 0xC3, 0xC4, 0xC3, 0xC4, 0 };
    const short sVictory1[3] = { 0x106E, 0x106B, 0x1071 };
    const short sVictory2[3] = { 0x88, 0x89, 0x89 };
    const short sVictory3[3] = { -1, -1, -1 };
    const msw::VICTORY_DATA sVictory = { { sVictory0, sVictory1, sVictory2, sVictory3 } };
    const short sEntrance[8] = { 1, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0 };
    const short sReadyStart[2] = { 1, 1 };
    const short sChangeCourt[3] = { 1, 3, 3 };
}

msw::CEventDemoHockey::~CEventDemoHockey()
{
    _Destroy();
}

void msw::CEventDemoHockey::CreateDemo(long nDemo, bcs::CTask *pParent)
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

int msw::CEventDemoHockey::IsDemoPlay(long nDemo) const
{
    switch (nDemo) {
        case 0:
        case 1:
        case 4:
            return 1;
    }
    return 0;
}

void msw::CEventDemoHockey::CreateTelop(long nObjId, long nCloseAnim)
{
    if (nObjId < 0) {
        return;
    }
    CCockpit::RemoveObj(nObjId);
    CCockpitHockey::CreateObj(nObjId, 0);
    if (nCloseAnim >= 0) {
        CCockpit::SetAnimEndClose(nObjId, nCloseAnim);
    }
    if (!IsTelop(nObjId)) {
        RegistTelop(nObjId);
    }
}

void msw::CEventDemoHockey::CreateTelop(long nObjId, long nParam, long nCloseAnim)
{
    if (nObjId < 0) {
        return;
    }
    CCockpit::RemoveObj(nObjId);
    CCockpitHockey::CreateObj(nObjId, nParam);
    if (nCloseAnim >= 0) {
        CCockpit::SetAnimEndClose(nObjId, nCloseAnim);
    }
    if (!IsTelop(nObjId)) {
        RegistTelop(nObjId);
    }
}

void msw::CEventDemoHockey::PlayEndTelop(long nObjId)
{
    if (nObjId < 0) {
        return;
    }
    if (IsTelop(nObjId)) {
        CCockpit::PlayEndAnim(nObjId);
        ClearTelop(nObjId);
    }
}

int msw::CEventDemoHockey::IsEndTelop(long nObjId)
{
    if (nObjId >= 0 && IsTelop(nObjId)) {
        return CCockpit::IsEndObj(nObjId);
    }
    return 1;
}

void msw::CEventDemoHockey::RemoveTelop(long nObjId)
{
    if (nObjId < 0) {
        return;
    }
    if (IsTelop(nObjId)) {
        CCockpit::RemoveObj(nObjId);
        ClearTelop(nObjId);
    }
}

void msw::CEventDemoHockey::PlayEndTelopAll()
{
    for (int i = 0; i < TELOP_MAX; i++) {
        if (m_nTelop[i] != -1) {
            CCockpit::PlayEndAnim(m_nTelop[i]);
            m_nTelop[i] = -1;
        }
    }
}

void msw::CEventDemoHockey::RemoveTelopAll()
{
    for (int i = 0; i < TELOP_MAX; i++) {
        if (m_nTelop[i] != -1) {
            CCockpit::RemoveObj(m_nTelop[i]);
            m_nTelop[i] = -1;
        }
    }
}

long msw::CEventDemoHockey::GetTelopId(long nTelop)
{
    static const long sTelopId[5] = { 0x1063, 0x1064, 0x11, 0xD, 0xE };

    return sTelopId[nTelop];
}

void msw::CEventDemoHockey::SetCockpitState(long nPart, long nState)
{
    switch (nPart) {
        case 0:
            if (nState == 2) {
                CCockpitHockey::SetPartMove(0, 3);
                CCockpitHockey::SetRadarIconDisp(0);
            } else if (nState == 1) {
                CCockpitHockey::SetPartMove(0, 1);
                CCockpitHockey::SetRadarIconDisp(1);
            }
            break;
        case 1:
            if (nState == 2) {
                CCockpitHockey::SetMissionCountDisp(0, 0);
            } else if (nState == 1) {
                CCockpitHockey::SetMissionCountDisp(0, 1);
            }
            break;
    }
}

void msw::CEventDemoHockey::TimerEvent(short, short, unsigned long)
{
}

void msw::CEventDemoHockey::CameraEvent(const char *pName, long nArg, long, float)
{
    if (pName == 0 || strcmp(pName, "MakeScore") != 0) {
        return;
    }
    if (nArg == 1 && CGame::g_GameWork.GetStage() == 0x11) {
        CStageHockey17::TrolleyForceReset();
    }
}

void msw::EvDemoHockey::CreateInstance(bcs::CTask *pParent)
{
    new CEventDemoHockey(pParent);
}
