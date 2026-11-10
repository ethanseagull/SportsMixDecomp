// The captions shown over the characters on the team and play mode screens.
// The game's file is SelectMenuChrCap.cpp: its local statics are named
// `...@unnamed@SelectMenuChrCap_cpp@9CSignTask...`. It is a copy of
// SelectMenuChrSign.cpp (game/menu/CChrSign.cpp) with a caption task added.
#include "game/msw/SelectMenu/CChrCaption.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/FVEC2.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/menu/CMessage.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The second entry is null in the game (a terminator, or an unused
    // second layout).
    const char *s_LayoutName[2] = { "me_p.brlyt", 0 };

    const char *s_AnimNames[4] = {
        "me_p_T_00.brlan",
        "me_p_T_01.brlan",
        "me_p_T_02.brlan",
        "me_p_T_03.brlan",
    };

    // The panes, by the index the caption table below uses.
    const char *s_PaneNames[49] = {
        "P",
        "com_n",
        "com_n_1",
        "com_n_2",
        "com_n_3",
        "com_n_4",
        "com_n_5",
        "com_n_6",
        "com_n_7",
        "com_n_8",
        "com_n_9",
        "1p_n",
        "1p_n_1",
        "1p_n_2",
        "1p_n_3",
        "1p_n_4",
        "1p_n_5",
        "2p_n",
        "2p_n_1",
        "2p_n_2",
        "2p_n_3",
        "3p_n",
        "3p_n_1",
        "4p_n",
        "4p_n_1",
        "com",
        "com_1",
        "com_2",
        "com_3",
        "com_4",
        "com_5",
        "com_6",
        "com_7",
        "com_8",
        "com_9",
        "1p",
        "1p_1",
        "1p_2",
        "1p_3",
        "1p_4",
        "1p_5",
        "2p",
        "2p_1",
        "2p_2",
        "2p_3",
        "3p",
        "3p_1",
        "4p",
        "4p_1",
    };

    // Kept in the game's .rodata although nothing in the module reads it
    // (as in CChrSign.cpp; see UnusedReferences below).
    const unsigned char s_PlayerNo[4] = { 1, 2, 3, 4 };

    // Each kind of caption has nNum panes, used in turn by SetCaption, and as
    // many text boxes, filled by Set with the text of message pLabel.
    struct CaptionInfo {
        unsigned char nNum;
        unsigned char nPane[10];
        unsigned char nTextPane[10];
        const char *pLabel;
    };

    enum {
        CAPTION_NUM = 5
    };

    const CaptionInfo s_CaptionInfo[CAPTION_NUM] = {
        { 6, { 11, 12, 13, 14, 15, 16 }, { 35, 36, 37, 38, 39, 40 }, "VsTemPlayerAssign_00" },
        { 4, { 17, 18, 19, 20 }, { 41, 42, 43, 44 }, "VsTemPlayerAssign_01" },
        { 2, { 21, 22 }, { 45, 46 }, "VsTemPlayerAssign_02" },
        { 2, { 23, 24 }, { 47, 48 }, "VsTemPlayerAssign_03" },
        { 10, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }, { 25, 26, 27, 28, 29, 30, 31, 32, 33, 34 }, "VsTemPlayerAssign_04" },
    };

    // The message file of the caption texts.
    const char *s_MessageFileName = "VsTemPlayerAssign";

    // The pane SetCaption uses next, per kind of caption.
    unsigned char s_nNextPane[CAPTION_NUM];

    // The screen offset of the caption of each character.
    bcs::FVEC2 s_ChrOffset[6] = {
        bcs::FVEC2(0.0f, 0.0f),
        bcs::FVEC2(-28.0f, 23.0f),
        bcs::FVEC2(0.0f, 0.0f),
        bcs::FVEC2(-5.0f, -4.0f),
        bcs::FVEC2(0.0f, 0.0f),
        bcs::FVEC2(-25.0f, 6.0f),
    };

    // A task's kind: the captions all have this one. The signs' kind is
    // this plus the player number; ClearSign sets the top bit of the tasks
    // it closes, so they are not found again.
    enum {
        CAPTION_KIND = 10000,
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

            // The original stores the vtable before calling the base
            // destructor, which the compiler does only when the body calls
            // something; most likely a call that is empty in this build (same
            // stand-in as in game/menu/CCourtCharaObj.cpp).
            static inline void Nothing() {}

            // One caption: keeps its pane over a scene object (shown as the
            // object is) until the object is gone. The pane index is in
            // m_Unk18. (class name guessed)
            class CCaptionTask : public menu::CDispTaskLink {
                public:
                    CCaptionTask(long nCaption)
                        : m_nCaption(nCaption)
                    {
                        m_bEnd = 0;
                        m_vOfs.x = 0.0f;
                        m_vOfs.y = 0.0f;
                    }

                    virtual ~CCaptionTask();
                    virtual void StatusCheck(CMenuPaneAccessor *pAccessor);
                    virtual void Disp(CMenuPaneAccessor *pAccessor, long nCamera);

                    long m_nCaption;          // 0x30
                    unsigned long m_hScnObj;  // 0x34
                    bcs::FVEC2 m_vOfs;        // 0x38, added to the screen position
            };

            // The same as CChrSign's sign task, with other animations.
            class CSignTask : public menu::CDispTaskLink {
                public:
                    // Values of m_Unk1C. The animations waited for in a step
                    // have the same number. (names guessed)
                    enum {
                        STEP_START = 0,
                        STEP_IN = 1,       // anim 1 opens the sign
                        STEP_SHOWN = 2,    // anim 2 loops
                        STEP_OUT = 3,      // anim 3 closes it
                        STEP_CANCEL = -1,
                        STEP_REWIND = -2   // anim 1 played backwards, then anim 0
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

            CCaptionTask::~CCaptionTask()
            {
                Nothing();
            }

            void CCaptionTask::StatusCheck(CMenuPaneAccessor *pAccessor)
            {
                if (bcs::CScnObj::GetScnObj(m_hScnObj) == 0) {
                    m_bEnd = 1;
                    pAccessor->SetPaneDisp(m_Unk18, 0);
                }
            }

            void CCaptionTask::Disp(CMenuPaneAccessor *pAccessor, long nCamera)
            {
                bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                if (pObj == 0) {
                    m_bEnd = 1;
                    return;
                }

                bcs::CScnObj *pScnObj = bcs::CScnObj::GetScnObj(m_hScnObj);
                if (pScnObj == 0) {
                    pObj->GetPane()->SetVisible(false);
                    m_bEnd = 1;
                    return;
                }

                const bcs::FMTX34 *pMtx = pScnObj->GetNodeWorldMtxPtr(0);
                bcs::FVEC3 pos;
                pos.x = pMtx->m[0][3];
                pos.y = pMtx->m[1][3];
                pos.z = pMtx->m[2][3];
                bcs::FVEC3Add(&pos, &pos, &m_Pos);
                bcs::FVEC3 screen;
                bcs::G2D::GetWorldScreenPosition(pos, nCamera, screen);
                screen.x += m_vOfs.x;
                screen.y += m_vOfs.y;

                nw4r::lyt::Pane *pPane = pObj->GetPane();
                // The original passes the FVEC3 straight on (FVEC3 is
                // probably derived from nw4r::math::VEC3 there).
                pPane->SetTranslate(reinterpret_cast<const nw4r::math::VEC3 &>(screen));
                pPane->SetAlpha(pScnObj->GetAlphaRate());
                pPane->SetVisible((pScnObj->GetFlag() & bcs::CScnObj::FLAG_HIDE_40) == 0);
            }

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
                static const long animIdRewind = STEP_START;
                static const int resetIn = 1;
                static const int recursiveIn = 1;
                static const int resetStart = 1;
                static const int recursiveStart = 1;

                long nStep = m_Unk1C;
                switch (nStep) {
                    case STEP_START:
                        pAccessor->PlayAnimation(m_Unk18, STEP_IN, 1.0f, 0.0f, recursiveStart, resetStart);
                        m_Unk1C = STEP_IN;
                        break;
                    case STEP_IN: {
                        bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                        if (pObj != 0 && pObj->IsAnimEnd(nStep)) {
                            pAccessor->PlayAnimation(m_Unk18, STEP_SHOWN, 1.0f, 0.0f, recursiveIn, resetIn);
                            m_Unk1C = STEP_SHOWN;
                        }
                        break;
                    }
                    case STEP_SHOWN:
                        break;
                    case STEP_OUT: {
                        bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                        if (pObj != 0 && pObj->IsAnimEnd(nStep)) {
                            m_bEnd = 1;
                        }
                        break;
                    }
                    case STEP_REWIND: {
                        bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                        if (pObj != 0 && pObj->IsAnimEnd(STEP_IN)) {
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
                // See CCaptionTask::Disp.
                pObj->GetPane()->SetTranslate(reinterpret_cast<const nw4r::math::VEC3 &>(screen));
            }

            // Starts closing the sign. Only ClearSign used it; the game keeps
            // its local statics (animId 1 and animId@0 3, then recursive@1
            // and reset@2, stored last first): static-order swap. What the
            // second test was is not known; 2 as in CChrSign.
            inline void CSignTask::SetOutAnim(CMenuPaneAccessor *pAccessor)
            {
                static const int reset = 1;
                static const int recursive = 1;
                static const long animIdOut = STEP_OUT;
                static const long animIdIn = STEP_IN;

                bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                if (pObj == 0) {
                    return;
                }

                if (pObj->IsAnimPlay(animIdIn)) {
                    pObj->SetAnimSpeed(animIdIn, -1.0f);
                    m_Unk1C = STEP_REWIND;
                } else if (pObj->IsAnimPlay(STEP_SHOWN)) {
                    m_Unk1C = STEP_CANCEL;
                } else {
                    pAccessor->PlayAnimation(m_Unk18, animIdOut, 1.0f, 0.0f, recursive, reset);
                    m_Unk1C = STEP_OUT;
                }
            }
        }
    }
}

msw::SelectMenu::CChrCaption::CChrCaption(long nId)
    : CSelectMenuLayoutBase(nId)
{
    for (long i = 0; i < CAPTION_NUM; i++) {
        s_nNextPane[i] = 0;
    }
}

msw::SelectMenu::CChrCaption::~CChrCaption()
{
    Remove();
}

void msw::SelectMenu::CChrCaption::Main()
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

void msw::SelectMenu::CChrCaption::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

int msw::SelectMenu::CChrCaption::Set()
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
            pPaneCtrl->BuildLayout(camNo, s_LayoutName[0]);
            pPaneCtrl->BuildAnim(camNo, s_AnimNames, 4);
            SetPaneAccessor(pPaneCtrl, s_PaneNames, 49);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(0.0f);
            pPaneCtrl->PlayAnim(0, 1.0f, 0.0f, 1, 1);

            nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(0);
            if (pPane != 0) {
                pPane->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
            }

            for (long i = 0; i < CAPTION_NUM; i++) {
                const CaptionInfo *pInfo = &s_CaptionInfo[i];
                const wchar_t *pText = menu::CMessage::s_Instance.GetTextByLabel(
                    menu::CMessage::s_Instance.GetFileIndex(s_MessageFileName), pInfo->pLabel);
                if (pText != 0) {
                    long nNum = pInfo->nNum;
                    for (long j = 0; j < nNum; j++) {
                        m_pPaneAccessor->SetTextBoxString(pInfo->nTextPane[j], pText, wcslen(pText));
                    }
                }
            }

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
        }
    }
    UnusedReferences();
    return 0;
}

void msw::SelectMenu::CChrCaption::Remove()
{
    m_DispTaskList.RemoveAll();
    DeletePaneAccessor();
    DeleteLytObj();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CChrCaption::SendEvent(long nEvent, void *pParam)
{
    return -1;
}

long msw::SelectMenu::CChrCaption::SetCaption(long nChr, long nCaption, unsigned long hScnObj, long nCamera,
                                              const bcs::FVEC3 *pPos)
{
    // The game numbers these recursive, reset@0; both are 1.
    static const int recursive = 1;
    static const int reset = 1;

    const CaptionInfo *pInfo = &s_CaptionInfo[nCaption];
    unsigned char nNext = s_nNextPane[nCaption];
    long nPane = pInfo->nPane[nNext];

    CCaptionTask *pTask = new CCaptionTask(nCaption);
    pTask->m_Unk18 = nPane;
    if (pPos != 0) {
        pTask->m_Pos = *pPos;
    }
    pTask->m_vOfs = s_ChrOffset[nChr];
    pTask->m_hScnObj = hScnObj;
    pTask->m_nKind = CAPTION_KIND;
    m_DispTaskList.PushBack(pTask);

    if (m_pPaneAccessor->GetPane(nPane) != 0) {
        m_pPaneAccessor->PlayAnimation(nPane, 0, 1.0f, 0.0f, recursive, reset);
    }

    nNext++;
    if (nNext >= pInfo->nNum) {
        nNext = 0;
    }
    s_nNextPane[nCaption] = nNext;
    return 0;
}

// SetSign and ClearSign are not in the game's module (the link dropped them),
// but their local statics (SetSign's animId 0, recursive@0, reset@1) and the
// -1.0f of the inlined SetOutAnim are, in this order after SetCaption's. The
// bodies are CChrSign's (see game/menu/CChrSign.cpp) with the caption table
// in place of the sign table and without the "RootPane" parenting (the
// module has no such string here): a guess.
long msw::SelectMenu::CChrCaption::SetSign(long nPlayer, long nType, const bcs::FVEC3 *pPos, long)
{
    // static-order swap, as in CChrSign::SetSign.
    static const int reset = 1;
    static const int recursive = 1;
    static const long animId = CSignTask::STEP_START;

    if (m_pPaneAccessor == 0) {
        return -1;
    }

    const CaptionInfo &rInfo = s_CaptionInfo[nType];
    long nPane = rInfo.nPane[rInfo.nNum == 1 ? 0 : nPlayer];

    CSignTask *pTask = new CSignTask(nType);
    pTask->m_Unk18 = nPane;
    pTask->m_Unk1C = CSignTask::STEP_START;
    pTask->m_Pos = *pPos;
    pTask->m_nKind = nPlayer + SIGN_KIND;
    m_DispTaskList.PushBack(pTask);

    nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(nPane);
    if (pPane != 0) {
        m_pPaneAccessor->PlayAnimation(nPane, animId, 1.0f, 0.0f, recursive, reset);
        pPane->SetTranslate(zero);
    }
    return 0;
}

long msw::SelectMenu::CChrCaption::ClearSign(long nPlayer)
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
