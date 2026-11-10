// The volley event demo: which demos volley plays and the telops (cockpit
// objects) they show. One file with msw::EvDemoVolley::CreateInstance and the
// two weak CEventDemoBase inlines the vtable needs.
#include "game/msw/CEventDemoVolley.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitVolley.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageVolley17.hpp"
#include "game/msw/EvDemo.hpp"

// The C library is not in include/ yet.
extern "C" int strcmp(const char *, const char *);

namespace {
    // What each demo gets from CreateDemo. What the halfwords mean is not
    // known; the trailing zeros may be padding.
    const short sVictory0[8] = { 1, 0x88, 0x89, 0x8A, 0x8B, 0x8A, 0x8B, 0 };
    const short sVictory1[3] = { 0x106E, 0x106B, 0x1071 };
    const short sVictory2[3] = { 0x48, 0x49, 0x49 };
    const short sVictory3[3] = { -1, -1, -1 };
    const msw::VICTORY_DATA sVictory = { { sVictory0, sVictory1, sVictory2, sVictory3 } };
    // Also passed to the perfect demo.
    const short sEntrance[8] = { 1, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0 };
    const short sReadyStart[2] = { 0x40, 0x42 };
    const short sChangeCourt[3] = { 1, 2, 3 };
}

msw::CEventDemoVolley::~CEventDemoVolley()
{
    _Destroy();
}

void msw::CEventDemoVolley::CreateDemo(long nDemo, bcs::CTask *pParent)
{
    switch (nDemo) {
        case 5:
            EvDemo::CreateChangeCourtDemo(pParent, sChangeCourt);
            break;
        case 0:
            EvDemo::CreateEntranceDemo(pParent, sEntrance);
            break;
        case 1:
            EvDemo::CreateVictoryDemo(pParent, &sVictory);
            break;
        case 4:
            EvDemoVolley::CreateReadyStartDemo(pParent, sReadyStart);
            break;
        case 7:
            EvDemoVolley::CreatePerfectDemo(pParent, sEntrance);
            break;
    }
}

int msw::CEventDemoVolley::IsDemoPlay(long nDemo) const
{
    switch (nDemo) {
        case 0:
        case 1:
        case 4:
        case 7:
            return 1;
    }
    return 0;
}

void msw::CEventDemoVolley::CreateTelop(long nObjId, long nCloseAnim)
{
    if (nObjId < 0) {
        return;
    }
    CCockpit::RemoveObj(nObjId);
    CCockpitVolley::CreateObj(nObjId, 0);
    if (nCloseAnim >= 0) {
        CCockpit::SetAnimEndClose(nObjId, nCloseAnim);
    }
    if (!IsTelop(nObjId)) {
        RegistTelop(nObjId);
    }
}

void msw::CEventDemoVolley::CreateTelop(long nObjId, long nParam, long nCloseAnim)
{
    if (nObjId < 0) {
        return;
    }
    CCockpit::RemoveObj(nObjId);
    CCockpitVolley::CreateObj(nObjId, nParam);
    if (nCloseAnim >= 0) {
        CCockpit::SetAnimEndClose(nObjId, nCloseAnim);
    }
    if (!IsTelop(nObjId)) {
        RegistTelop(nObjId);
    }
}

void msw::CEventDemoVolley::PlayEndTelop(long nObjId)
{
    if (nObjId < 0) {
        return;
    }
    if (IsTelop(nObjId)) {
        CCockpit::PlayEndAnim(nObjId);
        ClearTelop(nObjId);
    }
}

int msw::CEventDemoVolley::IsEndTelop(long nObjId)
{
    if (nObjId >= 0 && IsTelop(nObjId)) {
        return CCockpit::IsEndObj(nObjId);
    }
    return 1;
}

void msw::CEventDemoVolley::RemoveTelop(long nObjId)
{
    if (nObjId < 0) {
        return;
    }
    if (IsTelop(nObjId)) {
        CCockpit::RemoveObj(nObjId);
        ClearTelop(nObjId);
    }
}

void msw::CEventDemoVolley::PlayEndTelopAll()
{
    for (int i = 0; i < TELOP_MAX; i++) {
        if (m_nTelop[i] != -1) {
            CCockpit::PlayEndAnim(m_nTelop[i]);
            m_nTelop[i] = -1;
        }
    }
}

void msw::CEventDemoVolley::RemoveTelopAll()
{
    for (int i = 0; i < TELOP_MAX; i++) {
        if (m_nTelop[i] != -1) {
            CCockpit::RemoveObj(m_nTelop[i]);
            m_nTelop[i] = -1;
        }
    }
}

long msw::CEventDemoVolley::GetTelopId(long nTelop)
{
    static const long sTelopId[7] = { 0x1063, 0x1064, 0x11, 0x1E, 0xE, 0x1D, 0x18 };

    return sTelopId[nTelop];
}

void msw::CEventDemoVolley::SetCockpitState(long nPart, long nState)
{
    switch (nPart) {
        case 0:
            if (nState == 2) {
                CCockpitVolley::SetPartMove(0, 3, 0);
                CCockpitVolley::SetRadarIconDisp(0);
            } else if (nState == 1) {
                CCockpitVolley::SetPartMove(0, 1, 0);
                CCockpitVolley::SetRadarIconDisp(1);
            }
            break;
        case 1:
            if (nState == 2) {
                CCockpitVolley::SetMissionCountDisp(0, 0);
            } else if (nState == 1) {
                CCockpitVolley::SetMissionCountDisp(0, 1);
            }
            break;
        case 2:
            if (nState == 2) {
                CCockpitVolley::SetPartMove(1, 3, 0);
            } else if (nState == 1) {
                CCockpitVolley::SetPartMove(1, 1, 0);
            }
            break;
    }
}

void msw::CEventDemoVolley::TimerEvent(short, short, unsigned long)
{
}

void msw::CEventDemoVolley::CameraEvent(const char *pName, long nArg, long, float)
{
    if (pName == 0 || strcmp(pName, "MakeScore") != 0) {
        return;
    }
    if (nArg == 1 && CGame::g_GameWork.GetStage() == 0x11) {
        CStageVolley17::TrolleyForceReset();
    }
}

void msw::EvDemoVolley::CreateInstance(bcs::CTask *pParent)
{
    new CEventDemoVolley(pParent);
}
