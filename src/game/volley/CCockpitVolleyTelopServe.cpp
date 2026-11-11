// msw::CSignBoardVolley: the volley game's boards, and
// msw::CCockpitVolleyTelopServe: the telop shown before a serve (the
// server's name and the set point).
#include "game/msw/CSignBoardVolley.hpp"
#include "game/msw/CCockpitVolleyTelopServe.hpp"

#include <wchar.h>

#include "bcs/G2D/CPaneCtrl.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/CMessage.hpp"

namespace {

    // A "good" / "perfect" telop (layouts TE_good and TE_perfect). Nothing
    // in this file reads these tables; they are in the original's .data and
    // .rodata all the same, in front of everything else.
    const char *s_GoodAnimName[] = { "TE_good_GOOD.brlan" };
    const char *s_PerfectAnimName[] = { "TE_perfect_perfect.brlan" };
    const char *s_GoodPaneName[] = { "G_Null", "good_Null" };
    const char *s_PerfectPaneName[] = { "P_Null", "perfect_Null" };

    struct JudgeLayoutInfo {
        long nPaneNum;
        const char *pLayoutName;
        const char **ppAnimName;
        const char **ppPaneName;
    };

    const JudgeLayoutInfo s_JudgeLayoutInfo[] = {
        { 2, "TE_good.brlyt", s_GoodAnimName, s_GoodPaneName },
        { 2, "TE_perfect.brlyt", s_PerfectAnimName, s_PerfectPaneName },
    };

    const unsigned char s_JudgeOrder[] = { 1, 2, 3, 4 };

    // (guessed) Stand-in for code of the original that was optimised away;
    // see notes/decisions.md, "Unreferenced data kept by dead references"
    // and "Dead float comparison". The original keeps the tables above and
    // has 1.0f and 0.0f in .rodata right after them, in front of the serve
    // telop's layout table.
    // Stand-in for an unknown debug-only call (notes/decisions.md).
    static inline void Nothing() {}

    inline void UnusedReferences(float f)
    {
        (void)s_JudgeLayoutInfo;
        (void)s_JudgeOrder;
        if (f == 1.0f) {
        }
        if (f == 0.0f) {
        }
    }
}

msw::CSignBoardVolley::CSignBoardVolley(long nId, long nType, float fSortZ)
    : CSignBoard(nId, nType, fSortZ)
{
    UnusedReferences(fSortZ);
}

msw::CSignBoardVolley::~CSignBoardVolley()
{
    // The original stores the vtables here; see notes/decisions.md, "Empty
    // inline call in a destructor".
    Nothing();
}

long msw::CSignBoardVolley::SendEvent(long nEvent, void *pParam)
{
    CSignBoard::SendEvent(nEvent, pParam);
    return 0;
}

namespace {

    struct ServeLayoutInfo {
        const char *pLayoutName;
        long nArchive;
        const char *pAnimName[3];
    };

    enum {
        ANIM_IN,
        ANIM_OUT,
        ANIM_LOOP,
        ANIM_NUM
    };

    enum {
        PANE_NAME,
        PANE_SET_POINT,
        PANE_NUM
    };

    // Events of SendEvent.
    enum {
        EVENT_SET_PARAM = 1,     // pParam: a SERVE_PARAM
        EVENT_OUT = 10002,       // play the out animation
        EVENT_WAIT_OUT = 10003,  // pParam != 0: go out by itself after the loop
        EVENT_GET_FRAME = 10005, // returns the length of in, loop and out
        EVENT_SET_SPEED = 10006  // pParam: a float, the animation speed
    };

    // Steps of Main (m_nStep).
    enum {
        STEP_NONE,
        STEP_IN,
        STEP_LOOP,
        STEP_OUT
    };

    struct SERVE_PARAM {
        const wchar_t *pName;
        long nPointA;
        long nPointB;
    };

    const ServeLayoutInfo s_ServeLayoutInfo = {
        "TE_ready_vo.brlyt", 1, { "TE_ready_vo_Te_00.brlan", "TE_ready_vo_Te_01.brlan", "TE_ready_vo_Te_02.brlan" }
    };

    const char *s_PaneName[PANE_NUM] = { "t_cha_name", "t_set_point" };

    // The animation that belongs to a step, or -1.
    inline long GetStepAnim(long nStep)
    {
        switch (nStep) {
            case STEP_LOOP:
                return ANIM_LOOP;
            case STEP_OUT:
                return ANIM_OUT;
            case STEP_IN:
                return ANIM_IN;
            default:
                return -1;
        }
    }
}

msw::CCockpitVolleyTelopServe::CCockpitVolleyTelopServe(long nId, float fSortZ)
    : CCockpitLayoutBase(nId)
    , CCockpitFrameOutBase(1.0f)
{
    m_fSortZ = fSortZ;
    m_nStep = STEP_IN;
    m_Name[0] = 0;
    m_nPointA = 0;
    m_nPointB = 0;
    Set();
}

msw::CCockpitVolleyTelopServe::~CCockpitVolleyTelopServe()
{
    Remove();
}

// Writes "set point" with the two numbers into its text box. Inlined into
// its callers; the module link dropped the unused out-of-line copy.
void msw::CCockpitVolleyTelopServe::SetSetPointStr()
{
    static const long strBufLen = 128;

    if (m_pPaneAccessor == 0) {
        return;
    }

    wchar_t buf[strBufLen];
    const char *pFile = "TE_ready_vo";
    const char *pLabel = "t_set_point";
    const wchar_t *pFormat =
        menu::CMessage::s_Instance.GetTextByLabel(menu::CMessage::s_Instance.GetFileIndex(pFile), pLabel);

    swprintf(buf, strBufLen, pFormat, m_nPointA, m_nPointB);
    m_pPaneAccessor->SetTextBoxString(PANE_SET_POINT, buf, wcslen(buf));
}

int msw::CCockpitVolleyTelopServe::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = CCockpit::GetTask();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pArchiveName = pCockpit->GetArchiveFileName(s_ServeLayoutInfo.nArchive);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(s_ServeLayoutInfo.nArchive));

    if (pAccessor == 0) {
        return -2;
    }

    bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

    if (pPaneCtrl == 0) {
        return -3;
    }

    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(2), pLytCtrl->GetFontResourceName(2));
    pPaneCtrl->BuildLayout(0, s_ServeLayoutInfo.pLayoutName);
    // (the table is const; BuildAnim takes plain pointers)
    pPaneCtrl->BuildAnim(0, const_cast<const char **>(s_ServeLayoutInfo.pAnimName), ANIM_NUM);
    SetPaneAccessor(pPaneCtrl, s_PaneName, PANE_NUM);
    SetTextBoxFromMessage(pPaneCtrl);
    pPaneCtrl->SetSortZ(m_fSortZ);
    pPaneCtrl->PlayAnim(ANIM_IN, m_fUnk04, 0.0f, 1, 1);

    if (m_nWaitFrameOut < 0) {
        SetWaitFrameOut(IsWaitAnim(pPaneCtrl, ANIM_LOOP));
    }

    const wchar_t *pName = m_Name;

    m_pPaneAccessor->SetTextBoxString(PANE_NAME, pName, wcslen(pName));
    SetSetPointStr();
    return 0;
}

void msw::CCockpitVolleyTelopServe::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = STEP_NONE;
    m_nState = 0;
}

void msw::CCockpitVolleyTelopServe::Main()
{
    StepControl();
}

void msw::CCockpitVolleyTelopServe::StepControl()
{
    switch (m_nStep) {
        case STEP_IN: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl == 0) {
                m_Unk08 = 2;
                break;
            }
            if (pPaneCtrl->IsAnimEnd(ANIM_IN)) {
                pPaneCtrl->PlayAnim(ANIM_LOOP, 1.0f, 0.0f, 1, 1);
                m_nStep = STEP_LOOP;
            }
            break;
        }

        case STEP_LOOP: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl == 0) {
                m_Unk08 = 2;
                break;
            }
            if (IsWaitFrameOut() == 1 && pPaneCtrl->IsAnimEnd(ANIM_LOOP)) {
                pPaneCtrl->PlayAnim(ANIM_OUT, m_fUnk04, 0.0f, 1, 1);
                m_nStep = STEP_OUT;
            }
            break;
        }

        case STEP_OUT: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl == 0 || pPaneCtrl->IsAnimEnd(ANIM_OUT)) {
                m_Unk08 = 2;
            }
            break;
        }
    }
}

void msw::CCockpitVolleyTelopServe::Disp()
{
}

long msw::CCockpitVolleyTelopServe::SendEvent(long nEvent, void *pParam)
{
    CMenuPaneAccessor *pPaneAccessor;
    long nResult = 0;

    switch (nEvent) {
        case EVENT_OUT: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl != 0) {
                pPaneCtrl->PlayAnim(ANIM_OUT, 1.0f, 0.0f, 1, 1);
                m_nStep = STEP_OUT;
            }
            break;
        }

        case EVENT_WAIT_OUT:
            SetWaitFrameOut(pParam != 0);
            break;

        case EVENT_SET_PARAM: {
            if (pParam == 0) {
                break;
            }

            SERVE_PARAM *pServe = static_cast<SERVE_PARAM *>(pParam);
            const wchar_t *pName = pServe->pName;

            if (pName != 0) {
                long nLen = wcslen(pName) + 1;

                if (nLen < 0x40) {
                    wcsncpy(m_Name, pName, nLen);

                    pPaneAccessor = m_pPaneAccessor;

                    if (pPaneAccessor != 0) {
                        pPaneAccessor->SetTextBoxString(PANE_NAME, pName, wcslen(pName));
                    }
                }
            }
            m_nPointA = pServe->nPointA;
            m_nPointB = pServe->nPointB;
            SetSetPointStr();
            break;
        }

        case EVENT_GET_FRAME: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl != 0) {
                nResult = (long)pPaneCtrl->GetAnimFrameSize(ANIM_IN);
                nResult += (long)pPaneCtrl->GetAnimFrameSize(ANIM_LOOP);
                nResult += (long)pPaneCtrl->GetAnimFrameSize(ANIM_OUT);
            }
            break;
        }

        case EVENT_SET_SPEED: {
            if (pParam == 0) {
                break;
            }

            m_fUnk04 = *static_cast<float *>(pParam);

            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl != 0) {
                long nAnim = GetStepAnim(m_nStep);

                if (nAnim >= 0) {
                    pPaneCtrl->PlayAnim(nAnim, *static_cast<float *>(pParam), 0.0f, 1, 1);
                }
            }
            nResult = 1;
            break;
        }
    }
    return nResult;
}

long msw::CCockpitVolleyTelopServe::SetParameter(CCockpit *pCockpit, long nObjId, const wchar_t *pName, long nPointA,
                                               long nPointB)
{
    SERVE_PARAM param;

    param.pName = pName;
    param.nPointA = nPointA;
    param.nPointB = nPointB;
    return CCockpit::SendObjEventTrigger(nObjId, EVENT_SET_PARAM, &param);
}
