// msw::CCockpitChrIcon2D: the base of the per-sport character icons. Each
// icon is a display task (CChrIconTask below) that keeps one pane of the
// layout over a character.
#include "game/msw/CCockpitChrIcon2D.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/cockpit/CockpitUtil.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace msw {
    namespace {

        // (names guessed; the class has no symbols) One icon. m_Unk18 is the
        // icon's pane, m_nSubPane a second pane whose animations it waits
        // for, m_Unk1C its step (see below). It keeps the icon over the
        // character m_nChr, raised by the offset m_Pos.
        class CChrIconTask : public menu::CDispTaskLink {
            public:
                // Values of m_Unk1C; the animation played on entering a step
                // has its number. (names guessed)
                enum {
                    STEP_WAIT = 0,    // waits for the first frame drawn
                    STEP_IN = 1,      // anim 1 plays; then anim 2
                    STEP_SHOW = 2,    // anim 2 loops
                    STEP_OUT = 3,     // anim 3 closes the icon (ClearChrIcon)
                    STEP_CLOSED = -1  // hidden at once by ClearChrIcon
                };

                CChrIconTask()
                {
                    m_bEnd = 0;
                    m_nChr = 0;
                    m_fUnk34 = 0.0f;
                    m_nSubPane = 0;
                }

                virtual ~CChrIconTask();
                virtual void StatusCheck(CMenuPaneAccessor *pAccessor);
                virtual void Disp(CMenuPaneAccessor *pAccessor, long nCamera);

                long m_nChr;      // 0x30: for CChrMan::GetChrCore
                float m_fUnk34;   // 0x34
                long m_nSubPane;  // 0x38
        };

        // A task's kind: this plus the icon times 100 plus the character.
        // ClearChrIcon sets the top bit of the tasks it closes, so they are
        // not found again.
        const unsigned long ICON_KIND = 10000;
        const unsigned long KIND_CLOSING = 0x80000000;

        // The second pane of icon n is n + SUB_PANE.
        const long SUB_PANE = 4;

        // Out of line and empty: the original emits it first, without
        // storing the vtable.
        CChrIconTask::~CChrIconTask()
        {
        }

        void CChrIconTask::StatusCheck(CMenuPaneAccessor *pAccessor)
        {
            long nStep = m_Unk1C;
            switch (nStep) {
                case STEP_WAIT:
                    if (m_nFrame > 0) {
                        bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                        if (pObj != 0) {
                            pObj->PlayAnimation(STEP_IN, 1.0f, 0.0f, 1, 1, 1);
                            pObj->GetPane()->SetVisible(true);
                        }
                        m_Unk1C = STEP_IN;
                    }
                    break;
                case STEP_IN: {
                    bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                    bcs::G2D::CPaneObj *pSubObj = pAccessor->GetPaneObj(m_nSubPane);
                    if (pSubObj->IsAnimPlay(nStep) && pSubObj->IsAnimEnd(nStep)) {
                        if (pObj != 0) {
                            pObj->PlayAnimation(STEP_SHOW, 1.0f, 0.0f, 1, 1, 1);
                        }
                        m_Unk1C = STEP_SHOW;
                    }
                    break;
                }
                case STEP_SHOW:
                    break;
                case STEP_OUT: {
                    bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
                    bcs::G2D::CPaneObj *pSubObj = pAccessor->GetPaneObj(m_nSubPane);
                    if (pSubObj->IsAnimPlay(nStep) && pSubObj->IsAnimEnd(nStep)) {
                        if (pObj != 0) {
                            pObj->StopAnimation(nStep, 1);
                            pObj->GetPane()->SetVisible(false);
                        }
                        m_bEnd = 1;
                    }
                    break;
                }
                default:
                    m_bEnd = 1;
                    break;
            }
        }

        void CChrIconTask::Disp(CMenuPaneAccessor *pAccessor, long nCamera)
        {
            bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);
            if (pObj == 0) {
                m_bEnd = 1;
                return;
            }

            CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
            if (pChr == 0 || pChr->IsDisable()) {
                m_bEnd = 1;
                return;
            }

            bcs::FMTX34 mtx;
            pChr->GetMdl()->GetScnMtxPos(&mtx, 0, bcs::CScnObj::SCNOBJMTXTYPE_1);
            bcs::FVEC3 ofs;
            cockpit::GetBillboardOffset(&m_Pos, &mtx, nCamera, &ofs);

            bcs::FVEC3 pos = pChr->GetPos();
            pos.y = pChr->GetTransNodeHeight();
            bcs::FVEC3Add(&pos, &ofs, &pos);

            bcs::FVEC3 screen;
            bcs::G2D::GetWorldScreenPosition(pos, nCamera, screen);
            // The original passes the FVEC3 straight on (as in
            // game/menu/CChrSign.cpp).
            pObj->GetPane()->SetTranslate(reinterpret_cast<const nw4r::math::VEC3 &>(screen));
        }
    }
}

msw::CCockpitChrIcon2D::CCockpitChrIcon2D(long nId, float fSortZ, long nLayer, long nType)
    : CCockpitLayoutBase(nId, fSortZ)
{
    m_nType = nType;
    m_nLayer = nLayer;
}

// (guess) The original destructor stores both vtable pointers, which the
// compiler only keeps when the body calls something; see
// game/boss/CCockpitBossLayoutBase.cpp and notes/decisions.md.
static inline void Nothing() {}

msw::CCockpitChrIcon2D::~CCockpitChrIcon2D()
{
    Nothing();
}

void msw::CCockpitChrIcon2D::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);
}

void msw::CCockpitChrIcon2D::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, m_nLayer);
}

int msw::CCockpitChrIcon2D::Set()
{
    return 0;
}

void msw::CCockpitChrIcon2D::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

long msw::CCockpitChrIcon2D::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == 10001) {
        int bVisible = (int)pParam;
        bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
        if (pPaneCtrl != 0) {
            pPaneCtrl->SetVisible(bVisible);
        }
    }
    return 0;
}

long msw::CCockpitChrIcon2D::SetChrIcon(long nIcon, const bcs::FVEC3 &rPos, long nChr)
{
    return SetChrIconImpl(nIcon, rPos, nChr);
}

long msw::CCockpitChrIcon2D::SetChrIconImpl(long nIcon, const bcs::FVEC3 &rPos, long nChr)
{
    unsigned long nKind = ICON_KIND + nIcon * 100 + nChr;
    long nSubPane = nIcon + SUB_PANE;

    CChrIconTask *pTask = new CChrIconTask;
    pTask->SetPane(nIcon);
    pTask->m_Unk1C = 0;
    pTask->m_nSubPane = nSubPane;
    pTask->SetPos(rPos);
    pTask->m_nChr = nChr;
    pTask->SetKind(nKind);
    m_DispTaskList.PushBack(pTask);

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nIcon);
    if (pObj != 0) {
        pObj->GetPane()->SetVisible(false);
        pObj->StopAllAnimation(1);
        nw4r::lyt::Pane *pRoot = m_pPaneAccessor->GetPane("RootPane", 0, 0);
        bcs::G2D::SetPaneParent(pObj->GetPane(), pRoot, 1, 0);
    }
    return 0;
}

inline long msw::CCockpitChrIcon2D::ClearIconTasks(long nIcon, int bFade, long nChr)
{
    unsigned long nKind = ICON_KIND + nIcon * 100 + nChr;

    // Same shape as CChrSign::ClearSign in MenuF.
    menu::CDispTaskLink *pTask = m_DispTaskList.FindNextKindObj(nKind, 0);
    if (pTask != 0) {
        menu::CDispTaskList *pList = &m_DispTaskList;
        while (pTask != 0) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(pTask->m_Unk18);
            if (pObj != 0) {
                long nStep = pTask->m_Unk1C;
                if (bFade && (nStep == CChrIconTask::STEP_IN || nStep == CChrIconTask::STEP_SHOW)) {
                    pObj->PlayAnimation(CChrIconTask::STEP_OUT, 1.0f, 0.0f, 1, 1, 1);
                    pTask->m_Unk1C = CChrIconTask::STEP_OUT;
                } else {
                    pObj->StopAllAnimation(1);
                    pObj->GetPane()->SetVisible(false);
                    pTask->m_Unk1C = CChrIconTask::STEP_CLOSED;
                }
            }
            pTask->SetKind(nKind | KIND_CLOSING);
            pTask = pList->FindNextKindObj(nKind, 0);
        }
    }
    return 0;
}

long msw::CCockpitChrIcon2D::ClearChrIcon(long nIcon, int bFade, long nChr)
{
    return ClearIconTasks(nIcon, bFade, nChr);
}

long msw::CCockpitChrIcon2D::ClearChrIconImpl(long nIcon, int bFade, long nChr)
{
    return ClearIconTasks(nIcon, bFade, nChr);
}
