// msw::CCockpitBasketBonusTime: the basket game's "bonus time" telop.
#include "game/msw/CCockpitBasketBonusTime.hpp"

#include "game/msw/CCockpit.hpp"

namespace {

    // Animations, by index into s_AnimName (names guessed from the order
    // they are played in; TE_06 is not played here).
    enum {
        ANIM_IN,      // TE_00
        ANIM_HOLD,    // TE_01, loops until the first go
        ANIM_CHANGE,  // TE_03
        ANIM_HOLD2,   // TE_04, loops until the second go
        ANIM_OUT,     // TE_05
        ANIM_NUM = 6
    };

    // Values of m_nAnimState.
    enum {
        ANIM_STATE_NONE = 100,
        ANIM_STATE_START,
        ANIM_STATE_IN,
        ANIM_STATE_HOLD,
        ANIM_STATE_CHANGE,
        ANIM_STATE_HOLD2,
        ANIM_STATE_OUT,
        ANIM_STATE_END
    };

    // Events of SendEvent.
    enum {
        EVENT_OUT = 10002,         // play the out animation now
        EVENT_WAIT_OUT = 10003,    // pParam != 0: go on by itself
        EVENT_IS_STARTING = 10004, // returns 1 while the in animation has not ended
        EVENT_SET_FLOAT = 10006    // pParam points to a float for CCockpitFrameOutBase
    };

    // The archive of the cockpit that holds the layout.
    const long ARCHIVE_BONUS_TIME = 2;

    const char *s_LayoutName[2] = {
        "TE_bonus_time.brlyt",
        0,
    };

    const char *s_AnimName[ANIM_NUM] = {
        "TE_bonus_time_TE_00.brlan",
        "TE_bonus_time_TE_01.brlan",
        "TE_bonus_time_TE_03.brlan",
        "TE_bonus_time_TE_04.brlan",
        "TE_bonus_time_TE_05.brlan",
        "TE_bonus_time_TE_06.brlan",
    };
}

msw::CCockpitBasketBonusTime::CCockpitBasketBonusTime(long nId, long, float fSortZ)
    : CCockpitLayoutBase(nId, fSortZ)
    , CCockpitFrameOutBase(1.0f)
{
    m_nStep = 0;
    m_nState = 0;
    m_nAnimState = ANIM_STATE_NONE;
}

msw::CCockpitBasketBonusTime::~CCockpitBasketBonusTime()
{
    Remove();
}

int msw::CCockpitBasketBonusTime::Set()
{
    if (GetLytObj() != 0) {
        return 0;
    }

    CCockpit *pCockpit = CCockpit::GetTask();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pArchiveName = pCockpit->GetArchiveFileName(ARCHIVE_BONUS_TIME);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

        if (pPaneCtrl != 0) {
            if (pPaneCtrl->BuildLayout(0, s_LayoutName[0]) < 0) {
                DeleteLytObj();
                return -2;
            }

            pPaneCtrl->BuildAnim(0, s_AnimName, ANIM_NUM);
            pPaneCtrl->SetSortZ(m_fSortZ);
            m_nStep = 1;
        }
    }
    return 0;
}

void msw::CCockpitBasketBonusTime::Remove()
{
    m_nStep = 0;
    m_nAnimState = ANIM_STATE_NONE;
}

void msw::CCockpitBasketBonusTime::Main()
{
    if (m_nStep == 1) {
        m_nAnimState = ANIM_STATE_START;
        m_nStep = 2;
    }
    StateControl();
}

void msw::CCockpitBasketBonusTime::StateControl()
{
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

    if (pPaneCtrl == 0) {
        return;
    }

    switch (m_nAnimState) {
        case ANIM_STATE_START:
            m_nWaitFrameOut = 0;
            pPaneCtrl->PlayAnim(ANIM_IN, 1.0f, 0.0f, 1, 1);
            m_nAnimState = ANIM_STATE_IN;
            break;

        case ANIM_STATE_IN:
            if (pPaneCtrl->IsAnimEnd(ANIM_IN)) {
                pPaneCtrl->PlayAnim(ANIM_HOLD, 1.0f, 0.0f, 1, 1);
                m_nAnimState = ANIM_STATE_HOLD;
            }
            break;

        case ANIM_STATE_HOLD:
            if (IsWaitFrameOut() == 1) {
                m_nWaitFrameOut = 0;
                pPaneCtrl->PlayAnim(ANIM_CHANGE, 1.0f, 0.0f, 1, 1);
                m_nAnimState = ANIM_STATE_CHANGE;
            }
            break;

        case ANIM_STATE_CHANGE:
            if (pPaneCtrl->IsAnimEnd(ANIM_CHANGE)) {
                pPaneCtrl->PlayAnim(ANIM_HOLD2, 1.0f, 0.0f, 1, 1);
                m_nAnimState = ANIM_STATE_HOLD2;
            }
            break;

        case ANIM_STATE_HOLD2:
            if (IsWaitFrameOut() == 1) {
                m_nWaitFrameOut = 0;
                pPaneCtrl->PlayAnim(ANIM_OUT, 1.0f, 0.0f, 1, 1);
                m_nAnimState = ANIM_STATE_OUT;
            }
            break;

        case ANIM_STATE_OUT:
            if (pPaneCtrl->IsAnimEnd(ANIM_OUT)) {
                m_Unk08 = 2;
                m_nAnimState = ANIM_STATE_END;
            }
            break;

        case ANIM_STATE_NONE:
            break;

        default:
            m_Unk08 = 2;
            break;
    }
}

long msw::CCockpitBasketBonusTime::SendEvent(long nEvent, void *pParam)
{
    long nResult = 0;

    switch (nEvent) {
        case EVENT_IS_STARTING:
            nResult = m_nAnimState <= ANIM_STATE_IN;
            break;

        case EVENT_OUT: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl != 0) {
                pPaneCtrl->PlayAnim(ANIM_OUT, 1.0f, 0.0f, 1, 1);
                m_nAnimState = ANIM_STATE_OUT;
                nResult = 1;
            }
            break;
        }

        case EVENT_WAIT_OUT:
            nResult = 1;
            SetWaitFrameOut(pParam != 0);
            break;

        case EVENT_SET_FLOAT:
            if (pParam != 0) {
                nResult = 1;
                m_fUnk04 = *static_cast<float *>(pParam);
            }
            break;
    }
    return nResult;
}
