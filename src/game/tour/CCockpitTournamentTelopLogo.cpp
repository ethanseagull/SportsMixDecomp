#include "game/msw/CCockpitTournamentTelopLogo.hpp"

#include <wchar.h>

#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace {

    // A layout and its animations.
    struct TitleLayoutInfo {
        const char *pLayoutName;  // 0x00
        short nArchive;           // 0x04, file number for CCockpit::GetArchiveFileName
        short nPriority;          // 0x06, passed to CreateLytObj
        const char *pAnimName[6]; // 0x08
    };

    struct LogoLayoutInfo {
        const char *pLayoutName;  // 0x00
        short nArchive;           // 0x04
        short nPriority;          // 0x06
        const char *pAnimName[3]; // 0x08: in, out, loop
        unsigned char nTextPane;  // 0x14, index into s_PaneName
    };

    // Panes nChild[0 .. nNum - 1] are moved under pane nParent; the one whose
    // position is the telop's append table entry nSelect is shown, the
    // others are hidden. A list ends with an nParent of PANE_NONE.
    struct PaneLink {
        unsigned char nParent;   // 0x00
        unsigned char nNum;      // 0x01
        unsigned char nSelect;   // 0x02
        unsigned char nChild[6]; // 0x03
    };

    enum {
        PANE_SOZAI_KINOKO,
        PANE_SOZAI_FLOWER,
        PANE_SOZAI_STAR,
        PANE_CAP_TITLE_LOGO,
        PANE_TEXT_LOGO,
        PANE_TEXT_CAP,
        PANE_NUM,
        PANE_NONE = 0xFF
    };

    enum {
        ANIM_IN,
        ANIM_OUT,
        ANIM_LOOP,
        ANIM_NUM
    };

    // Events of SendEvent.
    enum {
        EVENT_SET_TEXT = 1,    // pParam points to the text (const wchar_t *)
        EVENT_OUT = 10002,     // play the out animation
        EVENT_WAIT_OUT = 10003 // pParam != 0: go out by itself after the in animation
    };

    // Steps of Main (m_nStep).
    enum {
        STEP_NONE,
        STEP_IN,
        STEP_LOOP,
        STEP_OUT
    };

    // The tournament's sport title telop (layout TO_TE_2D). Nothing in this
    // file reads these three tables; they are in the original's .rodata all
    // the same, in front of everything else.
    const TitleLayoutInfo s_TitleLayoutInfo[4] = {
        { "TO_TE_2D.brlyt", 0, 6,
          { "TO_TE_2D_TE_in_01.brlan", "TO_TE_2D_TE_out_01.brlan", "TO_TE_2D_Title_col_BA.brlan",
            "TO_TE_2D_Title_col_VA.brlan", "TO_TE_2D_Title_col_DO.brlan", "TO_TE_2D_Title_col_HO.brlan" } },
        { "TO_TE_2D.brlyt", 0, 6,
          { "TO_TE_2D_TE_in_02.brlan", "TO_TE_2D_TE_out_02.brlan", "TO_TE_2D_Title_col_BA.brlan",
            "TO_TE_2D_Title_col_VA.brlan", "TO_TE_2D_Title_col_DO.brlan", "TO_TE_2D_Title_col_HO.brlan" } },
        { "TO_TE_2D.brlyt", 0, 6,
          { "TO_TE_2D_TE_in_03.brlan", "TO_TE_2D_TE_out_03.brlan", "TO_TE_2D_Title_col_BA.brlan",
            "TO_TE_2D_Title_col_VA.brlan", "TO_TE_2D_Title_col_DO.brlan", "TO_TE_2D_Title_col_HO.brlan" } },
        { "TO_TE_2D.brlyt", 0, 6,
          { "TO_TE_2D_TE_in_04.brlan", "TO_TE_2D_TE_out_04.brlan", "TO_TE_2D_Title_col_BA.brlan",
            "TO_TE_2D_Title_col_VA.brlan", "TO_TE_2D_Title_col_DO.brlan", "TO_TE_2D_Title_col_HO.brlan" } },
    };

    const char s_TitleTextPaneName[4][16] = {
        "Title_txt_01",
        "Title_txt_02",
        "Title_txt_03",
        "Title_txt_04",
    };

    const unsigned char s_TitleOrder[3][4] = {
        { 1, 2, 3, 0 },
        { 2, 3, 0, 1 },
        { 3, 0, 1, 2 },
    };

    // (guessed) Stand-in for code of the original that was optimised away.
    // Behaviour-free; see notes/decisions.md, "Dead float comparison". The
    // original keeps the three title tables above although no code here
    // reads them (the compiler drops unreferenced tables), and it has 1.0f
    // and 0.0f in .rodata in front of s_EffectId, so something before
    // CCockpitTournamentModelTelopHold::Set referred to both. What that was
    // is not known.
    inline void UnusedReferences(float f)
    {
        (void)s_TitleLayoutInfo;
        (void)s_TitleTextPaneName;
        (void)s_TitleOrder;
        if (f == 1.0f) {
        }
        if (f == 0.0f) {
        }
    }
}

msw::CCockpitTournamentModelTelopHold::CCockpitTournamentModelTelopHold(long nId, long nType, float fSortZ, long nUnk)
    : CCockpitModelTelopHold(nId, nType, fSortZ, nUnk)
{
    UnusedReferences(fSortZ);
}

msw::CCockpitTournamentModelTelopHold::~CCockpitTournamentModelTelopHold()
{
}

namespace {

    // CCockpitTournamentModelTelopHold: the effect of each type.
    const long s_EffectId[1] = { 0x5A3 };
}

int msw::CCockpitTournamentModelTelopHold::Set()
{
    m_nEffectId = s_EffectId[m_nType];
    SetEffect(0, m_nEffectId, m_nUnk7C, 1, 1.0f);
    m_nWaitFrameOut = 1;
    m_nStep = 1;
    m_nState = 1;
    return 0;
}

namespace {

    // CCockpitTournamentTelopLogo, by type.
    const LogoLayoutInfo s_LogoLayoutInfo[2] = {
        { "TO_logo_gn.brlyt", 1, 0,
          { "TO_logo_gn_TE_01.brlan", "TO_logo_gn_TE_03.brlan", "TO_logo_gn_TE_02.brlan" }, PANE_TEXT_LOGO },
        { "TO_cap_logo.brlyt", 1, 6,
          { "TO_cap_logo_lo_01.brlan", "TO_cap_logo_lo_03.brlan", "TO_cap_logo_lo_02.brlan" }, PANE_TEXT_CAP },
    };

    const char *s_PaneName[PANE_NUM] = {
        "sozai_kinoko",
        "sozai_flower",
        "sozai_star",
        "cap_title_logo",
        "GN_t",
        "GN_t",
    };

    const PaneLink s_LogoPaneLink[] = {
        { PANE_NONE },
    };

    const PaneLink s_CapPaneLink[] = {
        { PANE_CAP_TITLE_LOGO, 3, 0, { PANE_SOZAI_KINOKO, PANE_SOZAI_FLOWER, PANE_SOZAI_STAR } },
        { PANE_NONE },
    };

    const PaneLink *s_PaneLink[2] = {
        s_LogoPaneLink,
        s_CapPaneLink,
    };
}

msw::CCockpitTournamentTelopLogo::CCockpitTournamentTelopLogo(long nId, long nType, float fSortZ)
    : CCockpitTelopSwitchColor(nId, nType, fSortZ)
{
    m_nStep = STEP_IN;
}

msw::CCockpitTournamentTelopLogo::~CCockpitTournamentTelopLogo()
{
    Remove();
}

int msw::CCockpitTournamentTelopLogo::Set()
{
    long nType;
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = CCockpit::GetTask();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    nType = m_nType;
    const char *pArchiveName = pCockpit->GetArchiveFileName(s_LogoLayoutInfo[nType].nArchive);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, s_LogoLayoutInfo[nType].nPriority, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));

            if (pPaneCtrl->BuildLayout(0, s_LogoLayoutInfo[nType].pLayoutName) < 0) {
                DeleteLytObj();
                return -2;
            }

            pPaneCtrl->BuildAnim(0, const_cast<const char **>(s_LogoLayoutInfo[nType].pAnimName), ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_PaneName, PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(m_fSortZ);
            pPaneCtrl->PlayAnim(ANIM_IN, 1.0f, 0.0f, 1, 1);

            const PaneLink *pLink = s_PaneLink[nType];

            for (long nParent = pLink->nParent; nParent < PANE_NUM; nParent = (++pLink)->nParent) {
                nw4r::lyt::Pane *pParent = pPaneCtrl->FindPaneByName(s_PaneName[nParent], 1, 0);

                if (pParent == 0) {
                    continue;
                }

                long nNum = pLink->nNum;

                for (long i = 0; i < nNum; i++) {
                    long nChild = pLink->nChild[i];

                    if (nChild >= PANE_NUM) {
                        continue;
                    }

                    nw4r::lyt::Pane *pChild = pPaneCtrl->FindPaneByName(s_PaneName[nChild], 1, 0);

                    if (pChild == 0) {
                        continue;
                    }

                    if (i == m_nAppendTable[pLink->nSelect]) {
                        bcs::G2D::SetPaneParent(pChild, pParent, 1, 0);
                        pChild->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
                        pChild->SetVisible(true);
                    } else {
                        pChild->SetVisible(false);
                    }
                }
            }

            if (m_nWaitFrameOut < 0) {
                SetWaitFrameOut(IsWaitAnim(pPaneCtrl, ANIM_LOOP));
            }
        }
    }
    return 0;
}

void msw::CCockpitTournamentTelopLogo::Remove()
{
    m_nStep = STEP_NONE;
}

void msw::CCockpitTournamentTelopLogo::Main()
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
            if (IsWaitFrameOut() == 1) {
                pPaneCtrl->PlayAnim(ANIM_OUT, 1.0f, 0.0f, 1, 1);
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

void msw::CCockpitTournamentTelopLogo::Disp()
{
}

long msw::CCockpitTournamentTelopLogo::SendEvent(long nEvent, void *pParam)
{
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

        case EVENT_SET_TEXT: {
            CMenuPaneAccessor *pPaneAccessor = m_pPaneAccessor;

            if (pPaneAccessor != 0 && pParam != 0) {
                unsigned char nPane = s_LogoLayoutInfo[m_nType].nTextPane;

                if (nPane <= PANE_TEXT_CAP) {
                    const wchar_t *pText = *static_cast<const wchar_t **>(pParam);

                    pPaneAccessor->SetTextBoxString(nPane, pText, wcslen(pText));
                }
            }
            break;
        }
    }
    return 0;
}

long msw::CCockpitTournamentTelopLogo::SetParameter(long nObjId, const wchar_t *pText)
{
    if (CCockpit::GetTask() == 0) {
        return -1;
    }

    const wchar_t *pParam = pText;

    return CCockpit::SendObjEventTrigger(nObjId, EVENT_SET_TEXT, &pParam);
}
