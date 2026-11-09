#include "game/msw/SelectMenu/CFade.hpp"

#include "game/msw/CCockpit.hpp"

namespace {
    // Cockpit object driven by this class.
    const long COCKPIT_OBJ_FADE = 0x1042;

    enum {
        EVENT_FADE_OPEN = 0x31,
        EVENT_FADE_CLOSE = 0x32
    };
}

msw::SelectMenu::CFade::CFade(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
}

msw::SelectMenu::CFade::~CFade()
{
    Remove();
}

void msw::SelectMenu::CFade::Main()
{
}

void msw::SelectMenu::CFade::Disp()
{
}

int msw::SelectMenu::CFade::Set()
{
    return 0;
}

void msw::SelectMenu::CFade::Remove()
{
    CCockpit::RemoveObj(COCKPIT_OBJ_FADE);
    m_Unk2C = 0;
    m_Unk30 = 0;
}

long msw::SelectMenu::CFade::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_FADE_OPEN:
            if (CCockpit::IsEndObj(COCKPIT_OBJ_FADE)) {
                CCockpit::CreateObj(COCKPIT_OBJ_FADE, (long)pParam);
                CCockpit::SetAnimEndClose(COCKPIT_OBJ_FADE, 0);
            }
            break;
        case EVENT_FADE_CLOSE:
            if (!CCockpit::IsEndObj(COCKPIT_OBJ_FADE)) {
                CCockpit::SetAnimEndClose(COCKPIT_OBJ_FADE, 1);
            }
            break;
    }
    return 0;
}
