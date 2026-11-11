#include "game/msw/SelectMenu/CTitle.hpp"

#include <string.h>
#include <wchar.h>

#include "bcs/BCS_Rand.hpp"
#include "bcs/CHBMenu.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CMovie.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/G2D/CLytManager.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/menu/CMovieScnObj.hpp"
#include "game/msw/menu/CSndBGM.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "revolution/gx/GXAttr.h"
#include "revolution/gx/GXLighting.h"
#include "revolution/gx/GXPixel.h"
#include "revolution/gx/GXTev.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_UNK_00 = 0,
        OBJ_TITLE = 4,
        OBJ_BG_CHR = 0x3A,
        OBJ_UNK_3B = 0x3B,
        OBJ_UNK_40 = 0x40,
        OBJ_ALL = 0xFF
    };

    // The sub-steps of CSelectMenu::Title_Loop (m_nStep).
    enum {
        STEP_START = 0,       // set up the first screen and fade in
        STEP_FADE_OUT = 1,
        STEP_REMOVE = 2,      // remove it and fade to white
        STEP_TITLE_IN = 10,   // build the title screen, start a movie
        STEP_MOVIE = 11,      // the opening movie, until A or its end
        STEP_TITLE_MENU = 20, // the title itself, with its music
        STEP_TITLE_WAIT = 21, // until A, or time for the attract demo
        STEP_DEMO = 22        // going to the attract demo
    };

    // Modes passed to CSelectMenu::SetNextMode.
    enum {
        MODE_TOP_MENU = 2,
        MODE_DEMO = 0x31
    };

    // The A button in CMenuPad::GetInput.
    enum {
        PAD_A = 0x10
    };

    // System sound played when A is pressed on the title.
    enum {
        SE_TITLE_DECIDE = 0xBD1
    };
}

void msw::THPGXRestore()
{
    GXSetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_SET);
    GXSetNumTexGens(1);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
}

void msw::CSelectMenu::Title_In()
{
    SendObjEventTrigger(OBJ_UNK_00, 6, 0);
    SendObjEventTrigger(OBJ_UNK_00, 0xA, 0);
    SendObjEventTrigger(OBJ_UNK_00, 0x10, 0);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    SendObjEventTrigger(OBJ_UNK_00, 4, 0);
    SendObjEventTrigger(OBJ_UNK_00, 6, 0);
    SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
    SendObjEventTrigger(OBJ_UNK_3B, 0x35, 0);
    SendObjEventTrigger(OBJ_UNK_40, 0x47, 0);
    SendObjEventTrigger(OBJ_UNK_00, 1, 0);
    SendObjEventTrigger(OBJ_UNK_3B, 0x3A, (void *)-1);
    CGamePadCheck::GetInstance()->m_Unk38 = 0;

    for (long i = 0; i < 4; i++) {
        CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags06 &= ~0x08;
        m_aPadSelect[i].m_Unk03 = 0x80;
    }

    Build(OBJ_TITLE);
    SelectMenu::CTitle *pTitle = (SelectMenu::CTitle *)FindObj(OBJ_TITLE);
    if (pTitle != 0) {
        pTitle->ButtonOn();
        pTitle->BGOn();
        m_fInputWait = 1140.0f;
        m_fTitleDemoWait = 0.0f;
        PlayMenuBGM(0);
        m_nStep = STEP_TITLE_MENU;
    }
}

void msw::CSelectMenu::Title_Loop()
{
    switch (m_nStep) {
        case STEP_START:
            if (!bcs::CMainApp::IsPlayMasterFade()) {
                CMenuLinkBase *pObj = FindObj(OBJ_ALL);
                if (pObj != 0) {
                    pObj->Set();
                }
                SetFadeIn(120.0f);
                m_nStep = STEP_FADE_OUT;
            }
            break;
        case STEP_FADE_OUT:
            if (!bcs::CMainApp::IsPlayMasterFade()) {
                SetBlockFadeOut(120.0f);
                m_nStep = STEP_REMOVE;
            }
            break;
        case STEP_REMOVE:
            if (!bcs::CMainApp::IsPlayMasterFade()) {
                if (FindObj(OBJ_ALL) != 0) {
                    RemoveObj(OBJ_ALL);
                }
                SetWhiteFadeOut(60.0f);
                m_nStep = STEP_TITLE_IN;
            }
            break;
        case STEP_TITLE_IN:
            if (!bcs::CMainApp::IsPlayMasterFade()) {
                bcs::CHBMenu *pHBMenu = bcs::CHBMenu::GetInstance();
                if (pHBMenu != 0) {
                    pHBMenu->SetBanFlag(1);
                }
                SetFadeIn(420.0f);
                Build(OBJ_TITLE);
                SelectMenu::CTitle *pTitle = (SelectMenu::CTitle *)FindObj(OBJ_TITLE);
                if (pTitle != 0) {
                    THPGXRestore();
                    bcs::CMainApp::ResetGP();
                    pTitle->m_pPaneAccessor->SetPaneDisp(1, 0);
                    pTitle->m_nMovie = m_nTitleMovie;
                    pTitle->m_nMovieState = SelectMenu::CTitle::MOVIE_OPEN;
                    m_nTitleMovie ^= 1;
                    pTitle->ButtonOff();
                    m_nStep = STEP_MOVIE;
                }
            }
            break;
        case STEP_MOVIE: {
            long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
            if (FindObj(OBJ_TITLE) != 0) {
                if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_A) || bcs::CMovie::GetInstance()->IsFrameEnd()) {
                    SetWhiteFadeOut(60.0f);
                    m_fInputWait = 1140.0f;
                    m_fTitleDemoWait = 0.0f;
                    m_nStep = STEP_TITLE_MENU;
                }
            }
            break;
        }
        case STEP_TITLE_MENU:
            if (!bcs::CMainApp::IsPlayMasterFade()) {
                bcs::CHBMenu *pHBMenu = bcs::CHBMenu::GetInstance();
                if (pHBMenu != 0) {
                    pHBMenu->SetBanFlag(0);
                }
                SelectMenu::CTitle *pTitle = (SelectMenu::CTitle *)FindObj(OBJ_TITLE);
                if (pTitle != 0) {
                    pTitle->ButtonOn();
                    pTitle->m_nMovieState = SelectMenu::CTitle::MOVIE_STOP;
                    pTitle->BGOn();
                    SetFadeIn(60.0f);
                    PlayMenuBGM(0);
                    m_nStep = STEP_TITLE_WAIT;
                }
            }
            break;
        case STEP_TITLE_WAIT: {
            long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
            if (FindObj(OBJ_TITLE) == 0) {
                break;
            }

            int bTimeUp = 0;
            m_fInputWait -= GetFrameRateF();
            if (m_fInputWait <= 0.0f) {
                bTimeUp = 1;
            }
            if (m_fTitleDemoWait == 0.0f) {
                // Once the title music has stopped, the demo starts a little
                // later even if m_fInputWait is not used up.
                menu::CSndBGM &rBGM = menu::CSndBGM::s_Instance;
                if (GetMenuBGMId(0) != 0 && !rBGM.IsPlayNow(rBGM.GetTaskId())) {
                    m_fTitleDemoWait = 180.0f;
                }
            } else {
                m_fTitleDemoWait -= GetFrameRateF();
                if (m_fTitleDemoWait <= 0.0f) {
                    bTimeUp = 1;
                }
            }

            if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
                if (CGamePadCheck::GetInstance() != 0) {
                    CGamePadCheck::GetInstance()->m_UnkAC = 0;
                }
                CEventDemo::m_bDemonstration = 0;
                menu::CSndEff::s_Instance.Play(SE_TITLE_DECIDE, 1);
                CGamePadCheck::GetInstance()->m_Unk38 = 1;
                m_aPadSelect[0].m_Unk03 = 0;
                SetNextMode(MODE_TOP_MENU, 0.0f);
            } else if (bTimeUp) {
                StopMenuBGM();
                // Every other time the opening movie is shown again instead
                // of a demo match.
                if (!(CEventDemo::m_nDemonstration & 1)) {
                    SetWhiteFadeOut(60.0f);
                    for (long i = 0; i < 4; i++) {
                        long nPlayerNo = CMenuPadInfo::s_Instance.GetDefaultPlayerNo(i);
                        m_aPadSelect[i].m_nPlayerNo = nPlayerNo & 0x3F;
                        CMenuPadInfo::s_Instance.m_aPad[i].m_nPlayerNo = nPlayerNo;
                        if (i == 0) {
                            CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags04 |= 0x04;
                        } else {
                            CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags04 &= ~0x04;
                        }
                        m_aPadSelect[i].m_Unk03 = 0x80;
                    }
                    m_nStep = STEP_TITLE_IN;
                } else {
                    SetBlockFadeOut(20.0f);
                    Title_SetUpRandGame();
                    m_nStep = STEP_DEMO;
                }
                CEventDemo::m_nDemonstration++;
            }
            break;
        }
        case STEP_DEMO:
            if (!bcs::CMainApp::IsPlayMasterFade()) {
                SetNextMode(MODE_DEMO, _GetModeChangeWaitTime());
                Scratch(OBJ_TITLE);
            }
            break;
    }
}

void msw::CSelectMenu::Title_Out()
{
}

// Sets up a random single player match of a random game for the attract
// demo.
void msw::CSelectMenu::Title_SetUpRandGame()
{
    m_Unk9B = 0;
    if (CGamePadCheck::GetInstance() != 0) {
        CGamePadCheck::GetInstance()->m_UnkAC = 1;
    }
    CEventDemo::m_bDemonstration = 1;

    long nGameKind = CEventDemo::GetRandomDemoGameMode(1);
    m_nGameKind = nGameKind;
    m_nPlayerNum = 0;
    m_nPlayMode = bcs::BCS_RandRange(12L, 13L);
    InitCharaSelectParam();

    // Pad 0 decides random characters until none is left to decide.
    unsigned char bDecided[0x12];
    memset(bDecided, 0, sizeof(bDecided));
    for (;;) {
        long nChara = bcs::BCS_RandRange(0L, 11L);
        if (!bDecided[nChara]) {
            Chara_SetDecideChara(0, nChara, 0, 0, 3);
            bDecided[nChara] = 1;
            if (Chara_FindNotDecideCharaIndex(0) < 0) {
                break;
            }
        }
    }
    SetCharaSelectSaveParam();

    long nStage;
    long nStageSub;
    for (;;) {
        nStage = bcs::BCS_RandRange(0L, 3L);
        nStageSub = bcs::BCS_RandRange(0L, 2L);
        const unsigned char *pStageNo = _GetStageNoInfo(nGameKind, nStage);
        if (pStageNo[nStageSub] != 0x10) {
            break;
        }
    }

    // The game keeps the old value only for 0xFF, which the draws above
    // never give (probably the shape of a shared setter).
    if (nStage == 0xFF) {
        m_nPrevStage = m_nStage;
    }
    m_nStage = nStage;
    if (nStageSub == 0xFF) {
        m_nPrevStageSub = m_nStageSub;
    }
    m_nStageSub = nStageSub;
}

// The file-level objects follow the CSelectMenu steps: their literals come
// after those steps' floats in .rodata.
namespace {

    enum {
        ANIM_LICENSE_00,
        ANIM_LICENSE_01,
        ANIM_LICENSE_02,
        ANIM_TITLE_OFF,
        ANIM_TI_01,
        ANIM_TI_02,
        ANIM_WII_ME_00,
        ANIM_WII_ME_01,
        ANIM_WII_ME_02,
        ANIM_NUM
    };

    enum {
        PANE_TITLE,
        PANE_TITLE_00,
        PANE_TITLE_BG,
        PANE_MARIO_SPORTS,
        PANE_T_COPYRIGHT,
        PANE_MOVIE_PICTURE,
        PANE_MOVIE_PICTURE2,
        PANE_T_PRESS_A_BUTTON,
        PANE_PRESS,
        PANE_TO,
        PANE_START,
        PANE_A_BOTAN,
        PANE_NUM
    };

    const char *const s_LayoutName = "me_title.brlyt";

    // The opening movies: the first for a 4:3 and a 16:9 picture (chosen by
    // CLytCtrl::IsLocationAdjust), and the second one (the same file).
    // Three variables, not a table: MovieControl reaches them from the
    // start of .data.
    const char *s_MovieName = "movie/op_hipol.thp";
    const char *s_MovieNameWide = "movie/op_hipolB.thp";
    const char *s_MovieName2 = "movie/op_hipol.thp";

    // Null-terminated; BuildAnim is given the count.
    const char *s_AnimNames[ANIM_NUM + 1] = {
        "me_title_license_00.brlan",
        "me_title_license_01.brlan",
        "me_title_license_02.brlan",
        "me_title_title_off.brlan",
        "me_title_ti_01.brlan",
        "me_title_ti_02.brlan",
        "me_title_wii_me_00.brlan",
        "me_title_wii_me_01.brlan",
        "me_title_wii_me_02.brlan",
        0
    };

    const char *s_PaneNames[PANE_NUM] = {
        "title",
        "title_00",
        "title_bg",
        "Mario_sports",
        "t_copyright",
        "movie_picture",
        "movie_picture2",
        "t_press_a_button",
        "Press",
        "to",
        "start",
        "A_botan"
    };

    // The font of the title layout (CLytCtrl's font table).
    enum {
        FONT_TITLE = 3
    };
}

msw::SelectMenu::CTitle::CTitle(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
    m_nMovieScnObj = 0;
}

msw::SelectMenu::CTitle::~CTitle()
{
    Remove();
}

void msw::SelectMenu::CTitle::Main()
{
    MovieControl();
}

void msw::SelectMenu::CTitle::Disp()
{
}

int msw::SelectMenu::CTitle::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(pSelectMenu->GetLoadArchiveFileId(-1));
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(FONT_TITLE), pLytCtrl->GetFontResourceName(FONT_TITLE));
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            pPaneCtrl->BuildAnim(0, s_AnimNames, ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_PaneNames, PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(-1.0f);
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
        }
    }

    m_pPaneAccessor->SetPaneDisp(PANE_TITLE_00, 0);
    m_pPaneAccessor->PlayAnimation(PANE_T_PRESS_A_BUTTON, ANIM_TI_02, 1.0f, 0.0f, 1, 1);

    const wchar_t *pText = GetMessageLabelString("TopMenuMessage", "TITLE_02");
    m_pPaneAccessor->SetTextBoxString(PANE_T_COPYRIGHT, pText, wcslen(pText));

    SetMovieVisible(-1);

    menu::CMovieScnObj *pMovie = new menu::CMovieScnObj;
    if (pMovie != 0) {
        bcs::CRenderMan::PushBack(pMovie, 8);
        m_nMovieScnObj = pMovie->GetHandle();
        pMovie->SetDrawPositionXYWH(0.0f, -4.0f, 640.0f, 464.0f);
        pMovie->SetDrawTextureST(0.0f, 0.0f, 1.0f, 1.0f);
    }
    m_Unk64 = 0;
    return 0;
}

void msw::SelectMenu::CTitle::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();

    bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
    if (pMovie != 0 && pMovie->IsPlay()) {
        pMovie->Stop();
    }

    bcs::CScnObj *pScnObj = bcs::CScnObj::GetScnObj(m_nMovieScnObj);
    if (pScnObj != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(pScnObj);
        m_nMovieScnObj = 0;
    }

    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

void msw::SelectMenu::CTitle::ButtonOff()
{
    if (m_Unk2C == 1) {
        m_pPaneAccessor->PlayAnimation(PANE_TITLE_00, ANIM_LICENSE_00, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_T_PRESS_A_BUTTON, 0);
        m_pPaneAccessor->PlayAnimation(PANE_T_PRESS_A_BUTTON, ANIM_TI_01, 1.0f, 0.0f, 1, 0);
        m_Unk2C = 3;
    }
}

void msw::SelectMenu::CTitle::ButtonOn()
{
    if (m_Unk2C == 3) {
        m_pPaneAccessor->PlayAnimation(PANE_TITLE_00, ANIM_LICENSE_02, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_T_PRESS_A_BUTTON, 1);
        m_pPaneAccessor->PlayAnimation(PANE_T_PRESS_A_BUTTON, ANIM_TI_02, 1.0f, 0.0f, 1, 0);
        m_Unk2C = 1;
    }
}

void msw::SelectMenu::CTitle::BGOn()
{
    m_pPaneAccessor->SetPaneDisp(PANE_TITLE_00, 1);
}

void msw::SelectMenu::CTitle::MovieControl()
{
    bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
    if (pMovie == 0) {
        return;
    }

    switch (m_nMovieState) {
        case MOVIE_STOP:
            pMovie->Stop();
            m_nMovieState = MOVIE_NONE;
            break;
        case MOVIE_OPEN:
            if (pMovie->IsWait() == 1) {
                int bOpened;
                if (m_nMovie == 0) {
                    bOpened = pMovie->Open(CLytCtrl::IsLocationAdjust() ? s_MovieName : s_MovieNameWide, 0, 0);
                } else {
                    bOpened = pMovie->Open(s_MovieName2, 0, 0);
                }
                if (bOpened == 1) {
                    m_nMovieState = MOVIE_START;
                }
            }
            break;
        case MOVIE_START:
            if (pMovie->IsReady() && pMovie->Start(0x60)) {
                pMovie->SetDrawEnv(0, -4, 0.0f, 0.0f);
                m_nMovieState = MOVIE_PLAY;
            }
            break;
    }
}

bcs::G2D::CPaneObj *msw::SelectMenu::CTitle::SetMovieVisible(long nVisible)
{
    bcs::G2D::CPaneObj *pPaneObj;

    if (nVisible == -1) {
        pPaneObj = m_pPaneAccessor->GetPaneObj(PANE_MOVIE_PICTURE);
        pPaneObj->GetPane()->SetVisible(false);
        pPaneObj = m_pPaneAccessor->GetPaneObj(PANE_MOVIE_PICTURE2);
        pPaneObj->GetPane()->SetVisible(false);
    } else if (bcs::G2D::CLytManager::GetInstance()->GetAdjustMode()) {
        pPaneObj = m_pPaneAccessor->GetPaneObj(PANE_MOVIE_PICTURE);
        pPaneObj->GetPane()->SetVisible(nVisible == 1);
    } else {
        pPaneObj = m_pPaneAccessor->GetPaneObj(PANE_MOVIE_PICTURE2);
        pPaneObj->GetPane()->SetVisible(nVisible == 1);
    }

    menu::CMovieScnObj *pMovie = (menu::CMovieScnObj *)bcs::CScnObj::GetScnObj(m_nMovieScnObj);
    if (pMovie != 0) {
        pMovie->SetDrawFlag(nVisible > 0);
    }
    return pPaneObj;
}

msw::SelectMenu::CTitleCo::~CTitleCo()
{
    Remove();
}

int msw::SelectMenu::CTitleCo::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(pSelectMenu->GetLoadArchiveFileId(-1));
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->BuildLayout(0, "me_co_logo.brlyt");
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);
        }
    }
    return 0;
}

void msw::SelectMenu::CTitleCo::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
}

void msw::SelectMenu::CTitleCo::Disp()
{
}

void msw::SelectMenu::CTitleCo::Main()
{
}
