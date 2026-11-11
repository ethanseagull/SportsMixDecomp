// msw::CCockpitHockeyTelopOut and msw::CCockpitHockeyTelopReady: two
// telops of the hockey party game that show a character's face and pad
// label, the "out" telop (HO_out_te) and the "ready" telop
// (TE_ready_ho_mini, with one face or the two of a match).
#include "game/msw/CCockpitHockeyTelopOut.hpp"
#include "game/msw/CCockpitHockeyTelopReady.hpp"

#include "bcs/G2D/CLayoutDL.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/MenuUtil.hpp"

namespace {

    // ---- CCockpitHockeyTelopOut ----------------------------------------

    // The layout of each type (only one) and its animation.
    struct OutRes {
        const char *pLayout;
        const char *pAnim[1];
    };
    const OutRes s_OutRes[1] = {
        { "HO_out_te.brlyt", { "HO_out_te_Uo_00.brlan" } },
    };

    enum {
        OUT_PANE_NULL,
        OUT_PANE_1P,
        OUT_PANE_2P,
        OUT_PANE_3P,
        OUT_PANE_4P,
        OUT_PANE_FACE,
        OUT_PANE_FC00, // the face picture made at run time
        OUT_PANE_NUM
    };

    const char *s_OutPaneName[OUT_PANE_NUM] = {
        "out_NULL",
        "1P_null",
        "2P_null",
        "3P_null",
        "4P_null",
        "p_face_NULL",
        "fc00",
    };

    // The pad labels, by pad number.
    const unsigned char s_OutPadPane[4] = { OUT_PANE_1P, OUT_PANE_2P, OUT_PANE_3P, OUT_PANE_4P };

    // Events of SendEvent.
    const long EVENT_OUT = 10002;
    const long EVENT_SET_OUT = 10003; // pParam: nonzero to leave after the loop

    // The resource file of the cockpit that holds the "out" layout.
    const long OUT_RES_FILE = 2;
}

// ---- CCockpitHockeyTelopOut --------------------------------------------

msw::CCockpitHockeyTelopOut::CCockpitHockeyTelopOut(long nId, long nType, float fSortZ)
    : CCockpitLayoutBase(nId, fSortZ)
{
    m_nType = nType;
    m_nStep = 1;
    m_nChr = 0;
}

msw::CCockpitHockeyTelopOut::~CCockpitHockeyTelopOut()
{
    Remove();
}

int msw::CCockpitHockeyTelopOut::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = CCockpit::GetTask();
    if (pCockpit == 0) {
        return -1;
    }
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pArchiveName = pCockpit->GetArchiveFileName(OUT_RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

        if (pPaneCtrl != 0) {
            const OutRes *pRes = &s_OutRes[m_nType];
            pPaneCtrl->BuildLayout(0, pRes->pLayout);
            pPaneCtrl->BuildAnim(0, const_cast<const char **>(pRes->pAnim), 1);
            SetPaneAccessor(pPaneCtrl, s_OutPaneName, OUT_PANE_NUM);
            pPaneCtrl->SetSortZ(m_fSortZ);
            pPaneCtrl->PlayAnim(0, 1.0f, 0.0f, 1, 1);
            long nChr = m_nChr;
            SetChrPadNo(nChr);
            SetChrFaceIcon(nChr);
        }
    }
    return 0;
}

void msw::CCockpitHockeyTelopOut::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
}

void msw::CCockpitHockeyTelopOut::Main()
{
    switch (m_nStep) {
        case 1:
            Set();
            if (GetLytObj() == 0) {
                m_Unk08 = 2;
                return;
            }
            m_nStep = 2;
            break;
        case 2:
            if (m_pPaneAccessor->GetPaneObj(OUT_PANE_NULL)->IsAnimEnd(0)) {
                m_Unk08 = 2;
            }
            break;
    }
}

void msw::CCockpitHockeyTelopOut::Disp()
{
}

long msw::CCockpitHockeyTelopOut::SendEvent(long nEvent, void *)
{
    switch (nEvent) {
        case EVENT_OUT:
            m_Unk08 = 2;
            break;
        case EVENT_SET_OUT:
            m_Unk08 = 2;
            break;
    }
    return 0;
}

long msw::CCockpitHockeyTelopOut::SetChrPadNo(long nChr)
{
    static const long tblLen = 4;

    for (long i = 0; i < tblLen; i++) {
        m_pPaneAccessor->SetPaneDisp(s_OutPadPane[i], 0);
    }

    CChrCore *pChr = CChrMan::GetChrCore(nChr);
    if (pChr == 0) {
        return -1;
    }
    if (pChr->IsDisable()) {
        return -2;
    }
    if (!pChr->IsPadCtrl()) {
        return -3;
    }
    long nPad = pChr->GetPadNo();
    if ((unsigned long)nPad <= tblLen - 1) {
        m_pPaneAccessor->SetPaneDisp(s_OutPadPane[nPad], 1);
    }
    return 0;
}

void msw::CCockpitHockeyTelopOut::SetChrFaceIcon(long nChr)
{
    static const long facePicPaneId = OUT_PANE_FC00;

    CChrCore *pChr = CChrMan::GetChrCore(nChr);
    if (pChr != 0 && !pChr->IsDisable()) {
        nw4r::lyt::Pane *pFace = m_pPaneAccessor->GetPane(OUT_PANE_FACE);
        nw4r::lyt::Pane *pPicture = m_pPaneAccessor->GetPane(facePicPaneId);
        if (pPicture == 0 && pFace != 0) {
            bcs::G2D::SetPaneSizeFromParent(menu::CreateFaceIconPicturePane(
                pChr->GetPlayerNo(), GetLytObj(), pFace, s_OutPaneName[facePicPaneId]));
        }
    }
}

// ---- CCockpitHockeyTelopReady ------------------------------------------

namespace {

    enum {
        READY_PANE_LINE,
        READY_PANE_1P,
        READY_PANE_2P,
        READY_PANE_3P,
        READY_PANE_4P,
        READY_PANE_FACE,   // type 0's face
        READY_PANE_FACE_L, // type 1's faces
        READY_PANE_FACE_R,
        READY_PANE_FC00,   // the face pictures made at run time
        READY_PANE_FC01,
        READY_PANE_FC02,
        READY_PANE_NUM
    };

    const char *s_ReadyPaneName[READY_PANE_NUM] = {
        "L_line",
        "1P_null",
        "2P_null",
        "3P_null",
        "4P_null",
        "p_face_NULL",
        "p_face_Null",
        "p_c_face_Null",
        "fc00",
        "fc01",
        "fc02",
    };

    // Animations of both layouts.
    enum {
        READY_ANIM_IN,
        READY_ANIM_LOOP,
        READY_ANIM_OUT,
        READY_ANIM_NUM
    };

    // The layout of each type: its resource file, its animations, the
    // pane each face goes in and how many faces it has.
    struct ReadyRes {
        const char *pLayout;
        long nResFile;
        const char *pAnim[READY_ANIM_NUM];
        unsigned char nFacePane[2];
        unsigned char nChrNum;
    };
    const ReadyRes s_ReadyRes[2] = {
        {
            "TE_ready_ho_mini.brlyt",
            2,
            { "TE_ready_ho_mini_Te_00.brlan", "TE_ready_ho_mini_Te_02.brlan", "TE_ready_ho_mini_Te_01.brlan" },
            { READY_PANE_FACE, READY_PANE_FACE },
            1,
        },
        {
            "TE_ready_ho_mini_to_vs.brlyt",
            2,
            { "TE_ready_ho_mini_to_vs_Te_00.brlan", "TE_ready_ho_mini_to_vs_Te_02.brlan",
              "TE_ready_ho_mini_to_vs_Te_01.brlan" },
            { READY_PANE_FACE_L, READY_PANE_FACE_R },
            2,
        },
    };

    const unsigned char s_ReadyPadPane[4] = { READY_PANE_1P, READY_PANE_2P, READY_PANE_3P, READY_PANE_4P };
    const unsigned char s_ReadyFacePicPane[3] = { READY_PANE_FC00, READY_PANE_FC01, READY_PANE_FC02 };

}

msw::CCockpitHockeyTelopReady::CCockpitHockeyTelopReady(long nId, long nType, float fSortZ)
    : CCockpitLayoutBase(nId, fSortZ)
    , CCockpitFrameOutBase(1.0f)
{
    m_nType = nType;
    m_nStep = 1;
    m_nChr[0] = -1;
    m_nChr[1] = -1;
}

msw::CCockpitHockeyTelopReady::~CCockpitHockeyTelopReady()
{
    Remove();
}

int msw::CCockpitHockeyTelopReady::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = CCockpit::GetTask();
    if (pCockpit == 0) {
        return -1;
    }
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const ReadyRes *pRes = &s_ReadyRes[m_nType];
    const char *pArchiveName = pCockpit->GetArchiveFileName(pRes->nResFile);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(2), pLytCtrl->GetFontResourceName(2));
            pPaneCtrl->BuildLayout(0, pRes->pLayout);
            pPaneCtrl->BuildAnim(0, const_cast<const char **>(pRes->pAnim), READY_ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_ReadyPaneName, READY_PANE_NUM);
            pPaneCtrl->SetSortZ(m_fSortZ);
            pPaneCtrl->PlayAnim(READY_ANIM_IN, 1.0f, 0.0f, 1, 1);

            if (m_nWaitFrameOut < 0) {
                SetWaitFrameOut(IsWaitAnim(pPaneCtrl, READY_ANIM_LOOP));
            }

            for (long i = 0; i < pRes->nChrNum; i++) {
                long nChr = m_nChr[i];
                if (nChr >= 0) {
                    if (i == 0) {
                        SetChrPadNo(nChr);
                    }
                    long nPicture = s_ReadyFacePicPane[i];
                    long nFace = pRes->nFacePane[i];
                    CChrCore *pChr = CChrMan::GetChrCore(nChr);
                    if (pChr != 0 && !pChr->IsDisable()) {
                        nw4r::lyt::Pane *pFace = m_pPaneAccessor->GetPane(nFace);
                        nw4r::lyt::Pane *pPicture = m_pPaneAccessor->GetPane(nPicture);
                        if (pPicture == 0 && pFace != 0) {
                            bcs::G2D::SetPaneSizeFromParent(menu::CreateFaceIconPicturePane(
                                pChr->GetPlayerNo(), GetLytObj(), pFace, s_ReadyPaneName[nPicture]));
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void msw::CCockpitHockeyTelopReady::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
}

void msw::CCockpitHockeyTelopReady::Main()
{
    switch (m_nStep) {
        case 1:
            Set();
            if (GetLytObj() == 0) {
                m_Unk08 = 2;
                return;
            }
            m_nStep = 2;
            break;
        case 2:
            if (m_pPaneAccessor->GetPaneObj(READY_PANE_LINE)->IsAnimEnd(READY_ANIM_IN) && IsWaitFrameOut() == 1) {
                GetLytObj()->PlayAnim(READY_ANIM_OUT, 1.0f, 0.0f, 1, 1);
                m_nStep = 3;
            }
            break;
        case 3:
            if (m_pPaneAccessor->GetPaneObj(READY_PANE_LINE)->IsAnimEnd(READY_ANIM_OUT)) {
                m_Unk08 = 2;
            }
            break;
    }
}

void msw::CCockpitHockeyTelopReady::Disp()
{
}

long msw::CCockpitHockeyTelopReady::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_OUT: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
            if (pPaneCtrl != 0) {
                pPaneCtrl->PlayAnim(READY_ANIM_OUT, 1.0f, 0.0f, 1, 1);
                m_nStep = 3;
            }
            break;
        }
        case EVENT_SET_OUT:
            SetWaitFrameOut(pParam != 0);
            break;
    }
    return 0;
}

long msw::CCockpitHockeyTelopReady::SetChrPadNo(long nChr)
{
    static const long tblLen = 4;

    for (long i = 0; i < tblLen; i++) {
        m_pPaneAccessor->SetPaneDisp(s_ReadyPadPane[i], 0);
    }

    CChrCore *pChr = CChrMan::GetChrCore(nChr);
    if (pChr == 0) {
        return -1;
    }
    if (pChr->IsDisable()) {
        return -2;
    }
    if (!pChr->IsPadCtrl()) {
        return -3;
    }
    long nPad = pChr->GetPadNo();
    if ((unsigned long)nPad <= tblLen - 1) {
        m_pPaneAccessor->SetPaneDisp(s_ReadyPadPane[nPad], 1);
    }
    return 0;
}
