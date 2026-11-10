// The signs shown over the characters on the character select screen. The
// game's file is SelectMenuChrSign.cpp: its local statics are named
// `...@unnamed@SelectMenuChrSign_cpp@9CSignTask...`.
#include "game/msw/SelectMenu/CChrSign.hpp"

#include "bcs/FVEC2.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "nw4r/lyt/Pane.hpp"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    const char *s_LayoutName = "me_cha_p.brlyt";

    const char *s_AnimNames[3] = {
        "me_cha_p_Vis_00.brlan",
        "me_cha_p_Vis_01.brlan",
        "me_cha_p_Vis_02.brlan",
    };

    // The panes, by the index the sign table below uses.
    const char *s_PaneNames[11] = {
        "cha_s",
        "cha_com01",
        "cha_com02",
        "cha_com03",
        "cha_com04",
        "cha_com05",
        "cha_com06",
        "cha_1P",
        "cha_2P",
        "cha_3P",
        "cha_4P",
    };

    // Kept in the game's .rodata although nothing in the module reads it
    // (see UnusedReferences below). Probably the player numbers.
    const unsigned char s_PlayerNo[4] = { 1, 2, 3, 4 };

    // The pane of each kind of sign, by player. A kind with one pane shows
    // the same pane for every player.
    struct SignInfo {
        unsigned char nNum;
        unsigned char nPane[6];
    };

    const SignInfo s_SignInfo[5] = {
        { 1, { 7, 8, 9, 10 } },
        { 1, { 8 } },
        { 1, { 9 } },
        { 1, { 10 } },
        { 6, { 1, 2, 3, 4, 5, 6 } },
    };

    // A task's kind is this plus the player number; ClearSign sets the top
    // bit of the tasks it closes, so they are not found again.
    enum {
        SIGN_KIND = 10000,
        KIND_CLOSING = 0x80000000
    };

    // (guessed) Stand-in for code of the original that was optimised away
    // or dropped by the linker. Behaviour-free; see notes/decisions.md,
    // "Unreferenced data kept by dead references".
    inline void UnusedReferences()
    {
        (void)s_PlayerNo;
    }
}

namespace msw {
    namespace SelectMenu {
        namespace {

            // One sign: plays its pane's animations and keeps the pane over
            // the world point m_Pos. The pane index is in m_Unk18, the step
            // in m_Unk1C.
            class CSignTask : public menu::CDispTaskLink {
                public:
                    // Values of m_Unk1C. The animations played in a step
                    // have the same number. (names guessed)
                    enum {
                        STEP_LOOP = 0,     // anim 0, played again whenever it ends
                        STEP_OUT = 1,      // anim 1 closes the sign
                        STEP_START = 2,    // waits for the first frame drawn
                        STEP_CANCEL = -1,  // closed before it was shown
                        STEP_REWIND = -2   // anim 0 played backwards, then anim 2
                    };

                    CSignTask(long nType)
                        : m_nType(nType)
                    {
                        m_bEnd = 0;
                        m_vOfs.x = 0.0f;
                        m_vOfs.y = 0.0f;
                    }

                    virtual ~CSignTask();
                    virtual void StatusCheck(CMenuPaneAccessor *pAccessor);
                    virtual void Disp(CMenuPaneAccessor *pAccessor, long nCamera);

                    void Main(CMenuPaneAccessor *pAccessor);
                    void SetOutAnim(CMenuPaneAccessor *pAccessor);

                    long m_nType;       // 0x30
                    bcs::FVEC2 m_vOfs;  // 0x34, added to the screen position
            };

            // The original stores the vtable before calling the base
            // destructor, which the compiler does only when the body calls
            // something; most likely a call that is empty in this build (same
            // stand-in as in game/menu/CCourtCharaObj.cpp).
            static inline void Nothing() {}

            CSignTask::~CSignTask()
            {
                Nothing();
            }

            inline void CSignTask::Main(CMenuPaneAccessor *pAccessor)
            {
                // The game declares these in the cases below (numbered
                // recursive, reset@0, recursive@1, reset@2, animId@3,
                // recursive@4, reset@5) and stores them last first; declared
                // here in the stored order: static-order swap (see
                // notes/decisions.md).
                static const int resetRewind = 1;
                static const int recursiveRewind = 1;
                static const long animIdRewind = 2;
                static const int resetLoop = 1;
                static const int recursiveLoop = 1;
                static const int resetStart = 1;
                static const int recursiveStart = 1;

                long nStep = m_Unk1C;
                switch (nStep) {
                    case STEP_START:
                        if (m_nFrame > 0) {
                            pAccessor->PlayAnimation(m_Unk18, STEP_LOOP, 1.0f, 0.0f, recursiveStart, resetStart);
                            m_Unk1C = STEP_LOOP;
                        }
                        break;
                    case STEP_LOOP: {
                        bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                        if (pObj != 0 && pObj->IsAnimEnd(nStep)) {
                            pAccessor->PlayAnimation(m_Unk18, STEP_LOOP, 1.0f, 0.0f, recursiveLoop, resetLoop);
                            m_Unk1C = STEP_LOOP;
                        }
                        break;
                    }
                    case STEP_OUT: {
                        bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                        if (pObj != 0 && pObj->IsAnimEnd(nStep)) {
                            m_bEnd = 1;
                        }
                        break;
                    }
                    case STEP_REWIND: {
                        bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                        if (pObj != 0 && pObj->IsAnimEnd(STEP_LOOP)) {
                            pAccessor->PlayAnimation(m_Unk18, animIdRewind, 1.0f, 0.0f, recursiveRewind, resetRewind);
                            m_bEnd = 1;
                        }
                        break;
                    }
                    default:
                        m_bEnd = 1;
                        break;
                }
            }

            void CSignTask::StatusCheck(CMenuPaneAccessor *pAccessor)
            {
                Main(pAccessor);
            }

            void CSignTask::Disp(CMenuPaneAccessor *pAccessor, long nCamera)
            {
                bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                if (pObj == 0) {
                    m_bEnd = 1;
                    return;
                }

                bcs::FVEC3 pos = m_Pos;
                bcs::FVEC3 screen;
                bcs::G2D::GetWorldScreenPosition(pos, nCamera, screen);
                screen.x += m_vOfs.x;
                screen.y += m_vOfs.y;
                // The original passes the FVEC3 straight on (FVEC3 is
                // probably derived from nw4r::math::VEC3 there).
                pObj->GetPane()->SetTranslate(reinterpret_cast<const nw4r::math::VEC3 &>(screen));
            }

            // Starts closing the sign: plays anim 0 backwards if it is still
            // opening, drops it if it has not started, else plays anim 1.
            inline void CSignTask::SetOutAnim(CMenuPaneAccessor *pAccessor)
            {
                // The game numbers these animId, animId@0, recursive@1,
                // reset@2; declared in the stored order: static-order swap.
                static const int reset = 1;
                static const int recursive = 1;
                static const long animIdOut = STEP_OUT;
                static const long animIdLoop = STEP_LOOP;

                bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                if (pObj == 0) {
                    return;
                }

                if (pObj->IsAnimPlay(animIdLoop)) {
                    pObj->SetAnimSpeed(animIdLoop, -1.0f);
                    m_Unk1C = STEP_REWIND;
                } else if (pObj->IsAnimPlay(STEP_START)) {
                    m_Unk1C = STEP_CANCEL;
                } else {
                    pAccessor->PlayAnimation(m_Unk18, animIdOut, 1.0f, 0.0f, recursive, reset);
                    m_Unk1C = STEP_OUT;
                }
            }
        }
    }
}

msw::SelectMenu::CChrSign::CChrSign(long nId)
    : CSelectMenuLayoutBase(nId)
{
}

msw::SelectMenu::CChrSign::~CChrSign()
{
    Remove();
}

void msw::SelectMenu::CChrSign::Main()
{
    if (m_Unk2C != 0) {
        m_DispTaskList.StatusCheck(m_pPaneAccessor);
    }

    switch (m_Unk30) {
        case STATE_SET:
            m_Unk30 = STATE_IN;
            break;
        case STATE_IN:
            m_Unk30 = STATE_ACTIVE;
            break;
        case STATE_CLOSE:
            m_Unk30 = STATE_OUT;
            break;
        case STATE_OUT:
            m_Unk08 = 2;
            break;
    }
}

void msw::SelectMenu::CChrSign::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

int msw::SelectMenu::CChrSign::Set()
{
    // Where the game uses it is not known: every candidate is 0.
    static const long camNo = 0;

    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(1);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 1, 1);
        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(camNo, s_LayoutName);
            pPaneCtrl->BuildAnim(camNo, s_AnimNames, 3);
            SetPaneAccessor(pPaneCtrl, s_PaneNames, 11);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(0.0f);
            pPaneCtrl->PlayAnim(2, 1.0f, 0.0f, 1, 1);

            nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(0);
            if (pPane != 0) {
                pPane->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
            }

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
        }
    }
    UnusedReferences();
    return 0;
}

void msw::SelectMenu::CChrSign::Remove()
{
    m_DispTaskList.RemoveAll();
    DeletePaneAccessor();
    DeleteLytObj();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CChrSign::SendEvent(long nEvent, void *pParam)
{
    return -1;
}

long msw::SelectMenu::CChrSign::SetSign(long nPlayer, long nType, const bcs::FVEC3 *pPos, long)
{
    // The game numbers these animId, recursive@0, reset@1; declared in the
    // stored order: static-order swap (see notes/decisions.md).
    static const int reset = 1;
    static const int recursive = 1;
    static const long animId = CSignTask::STEP_START;

    if (m_pPaneAccessor == 0) {
        return -1;
    }

    const SignInfo &rInfo = s_SignInfo[nType];
    long nPane = rInfo.nPane[rInfo.nNum == 1 ? 0 : nPlayer];

    CSignTask *pTask = new CSignTask(nType);
    pTask->m_Unk18 = nPane;
    pTask->m_Unk1C = CSignTask::STEP_START;
    pTask->m_Pos = *pPos;
    pTask->m_nKind = nPlayer + SIGN_KIND;
    m_DispTaskList.PushBack(pTask);

    nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(nPane);
    nw4r::lyt::Pane *pRoot = m_pPaneAccessor->GetPane("RootPane", 0, 0);
    if (pPane != 0) {
        m_pPaneAccessor->PlayAnimation(nPane, animId, 1.0f, 0.0f, recursive, reset);
        pPane->SetTranslate(zero);
        if (pRoot != 0) {
            bcs::G2D::SetPaneParent(pPane, pRoot, 1, 0);
        }
    }
    return 0;
}

long msw::SelectMenu::CChrSign::ClearSign(long nPlayer)
{
    CSignTask *pTask;
    unsigned long nKind = nPlayer + SIGN_KIND;

    pTask = static_cast<CSignTask *>(m_DispTaskList.FindNextKindObj(nKind, 0));
    if (pTask != 0) {
        menu::CDispTaskList *pList = &m_DispTaskList;
        while (pTask != 0) {
            pTask->SetOutAnim(m_pPaneAccessor);
            pTask->m_nKind = nKind | KIND_CLOSING;
            pTask = static_cast<CSignTask *>(pList->FindNextKindObj(nKind, 0));
        }
    }
    return 0;
}
