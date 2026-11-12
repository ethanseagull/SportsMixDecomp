#include "bcs/BCS_Draw2D.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CMovie.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/G2D/CLytManager.hpp"
#include "bcs/G2D/CMenuScnGroup.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "bcs/FVEC2.hpp"
#include "bcs/New.hpp"
#include "FastCast.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CSelectMenu.hpp"
#include "game/msw/SelectMenu/CBgDisplay.hpp"
#include "game/msw/SelectMenu/CCourtChr.hpp"
#include "game/msw/SelectMenu/CNwLayout.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/CSndBGM.hpp"
#include "nw4r/g3d/G3DState.hpp"
#include "nw4r/lyt/animation.h"
#include "nw4r/lyt/drawInfo.h"
#include "nw4r/lyt/Group.hpp"
#include "nw4r/lyt/Material.hpp"
#include "nw4r/lyt/Picture.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"
#include "nw4r/lyt/TextBox.hpp"
#include "nw4r/lyt/layout.h"
#include "nw4r/ut/RuntimeTypeInfo.h"
#include "revolution/gx/GXFrameBuf.h"
#include "revolution/gx/GXGeometry.h"
#include "revolution/gx/GXPixel.h"
#include "revolution/gx/GXTev.h"
#include "revolution/gx/GXTexture.h"
#include "revolution/gx/GXTransform.h"
#include "revolution/mtx.h"
#include "string.h"
#include "wchar.h"

#include <new>

// The background display of the select menu (MenuF): SelectMenu::CBgDisplay
// plays the movie of the stage or cup being chosen in a picture pane of its
// own layout, or the menu music, and shows the texts of a BgDispParam;
// SelectMenu::CNwLayout draws that layout into a model texture of the menu's
// court (SelectMenu::CCourtChr). The original file is SelectMenuBgDisp.cpp
// (the name its pointer assertion passes). Names in this file are guesses.


enum {
    PANE_ALL_NULL = 0,
    PANE_MOVIE_PIC = 1,
    PANE_T_PLAYER = 61,
    PANE_T_HIGH_SCORE = 62,
    PANE_T_TITLE = 55,
    PANE_T_MES = 56,
    PANE_T_MES_00 = 59,
    PANE_MV_00 = 109,
    PANE_SCREEN2 = 110,
    PANE_SCREEN = 111,
    PANE_NONE = 111, // the end of a list in s_OptionPane
    PANE_NUM = 112
};

// The layouts (by m_nLayout), their animations and the panes this file
// looks up (by PANE_* number).
static const char *s_LayoutName[] = { "me_bg_mov.brlyt", 0 };

static const char *s_AnimName[msw::SelectMenu::CNwLayout::ANIM_NUM] = {
    "me_bg_mov_mov_00.brlan", "me_bg_mov_mov_01.brlan", "me_bg_mov_mov_02.brlan",
    "me_bg_mov_mov_03.brlan", "me_bg_mov_mov_04.brlan", "me_bg_mov_mov_05.brlan",
};

static const char *s_PaneName[PANE_NUM] = {
    "ALL_Null", "movie_pic", "movie_base", "BA_option", "t_BA_period", "shotclock_BA_tex", "time_BA_tex",
    "hatena_panel_BA", "VO_option", "t_VO_set", "t_VO_setpoint", "hatena_panel_VO", "DO_option", "t_DO_set",
    "t_DO_life", "shotclock_DO_tex", "time_DO_tex", "hatena_panel_DO", "HO_option", "t_HO_period",
    "time_HO_tex", "hatena_panel_HO", "cup", "MASHROOM_CUP", "FLOWER_CUP", "STAR_CUP", "EXTRA_CUP",
    "MASHROOM", "FLOWER", "STAR", "EXTRA", "cup_WiFi", "MASHROOM_WiFi", "FLOWER_WiFi", "STAR_WiFi",
    "EXTRA_WiFi", "MASHROOM_00", "FLOWER_00", "STAR_00", "EXTRA_00", "level_00", "LEVEL1_L", "LEVEL2_L",
    "LEVEL3_L", "level_01", "LEVEL1_S", "LEVEL2_S", "LEVEL3_S", "rank_00", "NORMAL_L", "HARD_L", "rank_01",
    "NORMAL_S", "HARD_S", "set_00", "t_title", "t_mes", "text_base", "set_01", "t_mes_00", "text_base_00",
    "t_player", "t_hight_score", "mov_co", "mov_vo_ocean", "mov_vo_chocobo", "mov_vo_mario",
    "mov_vo_medley", "ptg_BA", "Blue_BA", "Red_BA", "ptg_DO", "Blue_DO", "Red_DO", "ptg_HO", "Blue_HO",
    "Red_HO", "movie_pic_s", "movie_base", "movie_pic", "vo_base", "mov_border_00", "mov_title_sh_01",
    "mov_title_sh_00", "mov_title_01", "mov_title_00", "mov_line_01", "mov_line_00", "vo_base_oc",
    "cha_oc", "oc_awa_00", "oc_awa_01", "oc_awa_02", "vo_base_cho", "cha_cho", "cho_hane_00",
    "cho_hane_01", "cho_hane_02", "vo_base_mw", "cha_mw", "mw_hatena_00", "mw_hatena_01", "mw_hatena_02",
    "vo_base_med", "cha_mw_00", "base_cho_00", "cha_cho_00", "base_oc_00", "cha_oc_00", "_mv_00",
    "screen2", "screen",
};

// Groups of panes of which SetDispPanes shows one.
static const unsigned char s_CupPane[] = { 23, 24, 25, 26 };
static const unsigned char s_CupNamePane[] = { 27, 28, 29, 30 };
static const unsigned char s_CupWiFiPane[] = { 32, 33, 34, 35 };
static const unsigned char s_CupWiFiNamePane[] = { 36, 37, 38, 39 };
static const unsigned char s_LevelLPane[] = { 41, 42, 43 };
static const unsigned char s_LevelSPane[] = { 45, 46, 47 };
static const unsigned char s_RankLPane[] = { 49, 50 };
static const unsigned char s_RankSPane[] = { 52, 53 };
static const unsigned char s_SetPane[] = { 58, 54 };
static const unsigned char s_LevelPane[] = { 40, 44 };
static const unsigned char s_RankPane[] = { 48, 51 };
static const unsigned char s_OptionPane[] = { 3, 8, 12, 18 };
static const unsigned char s_CupBasePane[] = { 22 };
static const unsigned char s_CupWiFiBasePane[] = { 31 };
static const unsigned char s_MoviePane[] = { 63, 64, 65, 66, 67 };
static const unsigned char s_TeamBAPane[] = { 69, 70 };
static const unsigned char s_TeamDOPane[] = { 72, 73 };
static const unsigned char s_TeamHOPane[] = { 75, 76 };
static const unsigned char s_TeamPane[] = { 68, 71, 74 };
static const unsigned char s_TextBasePane[] = { 57, 60 };

struct PaneList {
    const unsigned char *pPanes;
    long nNum;
};

enum {
    LIST_CUP,
    LIST_CUP_NAME,
    LIST_CUP_WIFI,
    LIST_CUP_WIFI_NAME,
    LIST_LEVEL_L,
    LIST_LEVEL_S,
    LIST_RANK_L,
    LIST_RANK_S,
    LIST_SET,
    LIST_LEVEL,
    LIST_RANK,
    LIST_OPTION,
    LIST_CUP_BASE,
    LIST_CUP_WIFI_BASE,
    LIST_MOVIE,
    LIST_TEAM_BA,
    LIST_TEAM_DO,
    LIST_TEAM_HO,
    LIST_TEAM,
    LIST_TEXT_BASE
};

static const PaneList s_PaneList[] = {
    { s_CupPane, 4 },         { s_CupNamePane, 4 },     { s_CupWiFiPane, 4 }, { s_CupWiFiNamePane, 4 },
    { s_LevelLPane, 3 },      { s_LevelSPane, 3 },      { s_RankLPane, 2 },   { s_RankSPane, 2 },
    { s_SetPane, 2 },         { s_LevelPane, 2 },       { s_RankPane, 2 },    { s_OptionPane, 4 },
    { s_CupBasePane, 1 },     { s_CupWiFiBasePane, 1 }, { s_MoviePane, 5 },   { s_TeamBAPane, 2 },
    { s_TeamDOPane, 2 },      { s_TeamHOPane, 2 },      { s_TeamPane, 3 },    { s_TextBasePane, 2 },
};

// The panes of a game's rule settings: its option pane, the text pane
// of each of the five rule texts (PANE_NONE when it has none) and its
// "?" panel. The last row is for no game.
struct OptionPane {
    unsigned char nOption;
    unsigned char nText[5];
    unsigned char nHatena;
};

static const OptionPane s_OptionPaneTbl[] = {
    { 3, { 4, PANE_NONE, 5, 6, PANE_NONE }, 7 },
    { 8, { 9, PANE_NONE, PANE_NONE, PANE_NONE, 10 }, 11 },
    { 12, { 13, 14, 15, 16, PANE_NONE }, 17 },
    { 18, { 19, PANE_NONE, PANE_NONE, 20, PANE_NONE }, 21 },
    { 22, { PANE_NONE, PANE_NONE, PANE_NONE, PANE_NONE, PANE_NONE }, PANE_NONE },
};

// The display list CNwLayout gives its scene object (never filled: the
// layout is drawn by hand in ObjProc).
static unsigned char s_DummyDL[0x20] __attribute__((aligned(32)));

// The Y, U and V planes of the movie's current frame.
static void *GetMovieTextureY(bcs::CMovie *pMovie)
{
    if (pMovie == 0) {
        return 0;
    }
    return pMovie->m_pTextureY;
}

static void *GetMovieTextureU(bcs::CMovie *pMovie)
{
    if (pMovie == 0) {
        return 0;
    }
    return pMovie->m_pTextureU;
}

static void *GetMovieTextureV(bcs::CMovie *pMovie)
{
    if (pMovie == 0) {
        return 0;
    }
    return pMovie->m_pTextureV;
}

// Turns pPane, a picture, into the movie's picture: three I8 textures
// (the Y, U and V planes) combined by four TEV stages into RGB, with the
// texture coordinates and size of pSrcPane when it is a picture.
static void SetupMoviePicture(nw4r::lyt::Pane *pPane, nw4r::lyt::Pane *pSrcPane)
{
    nw4r::lyt::Picture *pPicture = nw4r::ut::DynamicCast<nw4r::lyt::Picture *>(pPane);
    nw4r::lyt::Material *pMaterial = pPicture->GetMaterial();
    bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
    if (pMovie == 0) {
        return;
    }

    nw4r::lyt::Picture *pSrcPicture = nw4r::ut::DynamicCast<nw4r::lyt::Picture *>(pSrcPane);
    nw4r::lyt::Material *pSrcMaterial = pSrcPicture != 0 ? pSrcPicture->GetMaterial() : 0;

    pMaterial->SetName(pPicture->GetName());
    const bcs::THPVideoInfo *pInfo = pMovie->GetTHPVideoInfo();
    short nWidth = pInfo->xSize;
    short nHeight = pInfo->ySize;
    pMaterial->ReserveGXMem(3, 1, 2, 4, true, 0, 0, false, false, false, true);
    pMaterial->SetTextureNum(3);
    pMaterial->SetTexCoordGenNum(2);
    pMaterial->SetTevStageNum(4);
    pPicture->ReserveTexCoord(3);
    pPicture->SetTexCoordNum(3);

    nw4r::lyt::TexMap texMap[3];
    texMap[0].SetTexParam(GetMovieTextureY(pMovie), nWidth, nHeight, GX_TF_I8, GX_CLAMP, GX_CLAMP, false);
    nHeight >>= 1;
    nWidth >>= 1;
    texMap[1].SetTexParam(GetMovieTextureU(pMovie), nWidth, nHeight, GX_TF_I8, GX_CLAMP, GX_CLAMP, false);
    texMap[2].SetTexParam(GetMovieTextureV(pMovie), nWidth, nHeight, GX_TF_I8, GX_CLAMP, GX_CLAMP, false);
    for (int i = 0; i < 3; i++) {
        texMap[i].SetLODParam(GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, false, false, GX_ANISO_1);
        pMaterial->SetTexture(i, texMap[i]);
    }

    if (pSrcMaterial != 0) {
        for (int i = 0; i < 2; i++) {
            pMaterial->SetTexCoordGen(i, pSrcMaterial->GetTexCoordGen(0));
        }
        pMaterial->SetTexSRT(0, pSrcMaterial->GetTexSRT(0));
    } else {
        nw4r::lyt::TexCoordGen texCoordGen(GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
        for (int i = 0; i < 2; i++) {
            pMaterial->SetTexCoordGen(i, texCoordGen);
        }
        nw4r::lyt::TexSRT texSRT;
        texSRT.translate.x = 0.0f;
        texSRT.translate.y = 0.0f;
        texSRT.rotate = 0.0f;
        texSRT.scale.x = 1.0f;
        texSRT.scale.y = 1.0f;
        pMaterial->SetTexSRT(0, texSRT);
    }

    // YUV to RGB: stage 0 and 1 add the U and V planes times their
    // constant colours, stage 2 adds the Y plane, stage 3 scales.
    nw4r::lyt::TevStage tevStage[4];
    tevStage[0].SetOrder(GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL, GX_TEV_SWAP0, GX_TEV_SWAP0);
    tevStage[0].SetColorIn(GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_C0);
    tevStage[0].SetAlphaIn(GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, GX_CA_A0);
    tevStage[0].SetColorOp(GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, false, GX_TEVPREV, GX_TEV_KCSEL_K0);
    tevStage[0].SetAlphaOp(GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, false, GX_TEVPREV, GX_TEV_KASEL_K0_A);
    tevStage[0].SetIndirect(GX_INDTEXSTAGE0, GX_ITF_8, GX_ITB_NONE, GX_ITM_OFF, GX_ITW_OFF, GX_ITW_OFF, false,
                            false, GX_ITBA_OFF);
    tevStage[1].SetOrder(GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL, GX_TEV_SWAP0, GX_TEV_SWAP0);
    tevStage[1].SetColorIn(GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV);
    tevStage[1].SetAlphaIn(GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, GX_CA_APREV);
    tevStage[1].SetColorOp(GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, false, GX_TEVPREV, GX_TEV_KCSEL_K1);
    tevStage[1].SetAlphaOp(GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, false, GX_TEVPREV, GX_TEV_KASEL_K1_A);
    tevStage[1].SetIndirect(GX_INDTEXSTAGE0, GX_ITF_8, GX_ITB_NONE, GX_ITM_OFF, GX_ITW_OFF, GX_ITW_OFF, false,
                            false, GX_ITBA_OFF);
    tevStage[2].SetOrder(GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL, GX_TEV_SWAP0, GX_TEV_SWAP0);
    tevStage[2].SetColorIn(GX_CC_ZERO, GX_CC_TEXC, GX_CC_ONE, GX_CC_CPREV);
    tevStage[2].SetAlphaIn(GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    tevStage[2].SetColorOp(GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV, GX_TEV_KCSEL_1);
    tevStage[2].SetAlphaOp(GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV, GX_TEV_KASEL_1);
    tevStage[2].SetIndirect(GX_INDTEXSTAGE0, GX_ITF_8, GX_ITB_NONE, GX_ITM_OFF, GX_ITW_OFF, GX_ITW_OFF, false,
                            false, GX_ITBA_OFF);
    tevStage[3].SetOrder(GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL, GX_TEV_SWAP0, GX_TEV_SWAP0);
    tevStage[3].SetColorIn(GX_CC_APREV, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
    tevStage[3].SetAlphaIn(GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    tevStage[3].SetColorOp(GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV, GX_TEV_KCSEL_K2);
    tevStage[3].SetAlphaOp(GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV, GX_TEV_KASEL_1);
    tevStage[3].SetIndirect(GX_INDTEXSTAGE0, GX_ITF_8, GX_ITB_NONE, GX_ITM_OFF, GX_ITW_OFF, GX_ITW_OFF, false,
                            false, GX_ITBA_OFF);
    for (int i = 0; i < 4; i++) {
        pMaterial->SetTevStage(i, tevStage[i]);
    }

    const GXColorS10 tevColor = { -90, 0, -114, 135 };
    pMaterial->SetTevColor(0, tevColor);
    pMaterial->SetTevKColor(0, nw4r::ut::Color(0, 0, 226, 88));
    pMaterial->SetTevKColor(1, nw4r::ut::Color(179, 0, 0, 182));
    pMaterial->SetTevKColor(2, nw4r::ut::Color(255, 0, 255, 128));

    nw4r::lyt::TevSwapMode swap0(GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    nw4r::lyt::TevSwapMode swap1(GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    nw4r::lyt::TevSwapMode swap2(GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    nw4r::lyt::TevSwapMode swap3(GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
    pMaterial->SetTevSwapMode(0, swap0);
    pMaterial->SetTevSwapMode(1, swap1);
    pMaterial->SetTevSwapMode(2, swap2);
    pMaterial->SetTevSwapMode(3, swap3);

    nw4r::lyt::BlendMode blendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    pMaterial->SetBlendMode(blendMode);

    nw4r::lyt::Size size;
    bcs::FVEC2 texCoord[4];
    if (pSrcPicture != 0) {
        size = pSrcPicture->GetSize();
        pSrcPicture->GetTexCoord(0, reinterpret_cast<nw4r::math::VEC2 *>(texCoord));
    } else {
        size.width = 320.0f;
        size.height = 224.0f;
        texCoord[0].Set(0.0f, 0.0f);
        texCoord[1].Set(1.0f, 0.0f);
        texCoord[2].Set(0.0f, 1.0f);
        texCoord[3].Set(1.0f, 1.0f);
    }
    pPicture->SetSize(size);
    for (int i = 0; i < 3; i++) {
        pPicture->SetTexCoord(i, reinterpret_cast<nw4r::math::VEC2 *>(texCoord));
    }
}

// Points the textures of pPane, a movie picture set up by
// SetupMoviePicture, at the movie's current frame.
static void UpdateMovieTexture(nw4r::lyt::Pane *pPane)
{
    if (pPane == 0) {
        return;
    }

    nw4r::lyt::Picture *pPicture = nw4r::ut::DynamicCast<nw4r::lyt::Picture *>(pPane);
    nw4r::lyt::Material *pMaterial = pPicture->GetMaterial();
    bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
    if (pMovie == 0) {
        return;
    }

    unsigned long nTexNum = pMaterial->GetTextureNum();
    const bcs::THPVideoInfo *pInfo = pMovie->GetTHPVideoInfo();
    short nWidth = pInfo->xSize;
    short nHeight = pInfo->ySize;
    // As many textures as the material has, on the stack.
    unsigned long nSize = nTexNum * sizeof(nw4r::lyt::TexMap);
    nw4r::lyt::TexMap *texMap = static_cast<nw4r::lyt::TexMap *>(__alloca(nSize));
    nw4r::lyt::TexMap *pEnd = texMap + nSize / sizeof(nw4r::lyt::TexMap);
    nw4r::lyt::TexMap *p = texMap;
    do {
        new (p) nw4r::lyt::TexMap();
    } while (++p < pEnd);
    texMap[0].SetTexParam(GetMovieTextureY(pMovie), nWidth, nHeight, GX_TF_I8, GX_CLAMP, GX_CLAMP, false);
    texMap[1].SetTexParam(GetMovieTextureU(pMovie), nWidth >> 1, nHeight >> 1, GX_TF_I8, GX_CLAMP, GX_CLAMP,
                          false);
    texMap[2].SetTexParam(GetMovieTextureV(pMovie), nWidth >> 1, nHeight >> 1, GX_TF_I8, GX_CLAMP, GX_CLAMP,
                          false);
    for (unsigned long i = 0; i < nTexNum; i++) {
        texMap[i].SetLODParam(GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, false, false, GX_ANISO_1);
        pMaterial->SetTexture(i, texMap[i]);
    }
}

namespace msw {
    namespace SelectMenu {

        CBgDisplay::CBgDisplay(long nId) : CSelectMenuLayoutBase(nId)
        {
            Initialize();
        }

        CBgDisplay::~CBgDisplay()
        {
            Remove();
        }

        void CBgDisplay::Initialize()
        {
            m_pNwLayout = 0;
            m_nLayout = 0;
            m_nMode = MODE_MOVIE;
            m_Unk2C = 0;
            m_Unk30 = STATE_NONE;
            m_nMovieStep = 0;
            m_nBgmStep = 0;
            m_pMovieName = "movie/st01_BA.thp";
            m_StageName[0] = 0;
            m_IntroText[0] = 0;
            m_CupText[0] = 0;
            m_RuleText[0][0] = 0;
            m_RuleText[1][0] = 0;
            m_RuleText[2][0] = 0;
            m_RuleText[4][0] = 0;
            m_RuleText[3][0] = 0;
            m_ScoreText[0] = 0;
            m_ScoreLabel[0] = 0;
            m_nType = 0;
            m_bStage = 0;
            m_nGameKind = 0;
            m_nRuleFlag = 0;
            m_nCup = 0;
            m_nPartyCup = 0;
            m_nPartyCup2 = 0;
            m_nPartyStage = 0;
            m_nPartyLevel = 0;
            m_Unk94 = -1;
            m_Unk98 = -1;
        }

        void CBgDisplay::Main()
        {
            if (m_nMode == MODE_BGM) {
                BgmControl();
            } else {
                MovieControl();
            }

            if (m_pNwLayout != 0) {
                m_pNwLayout->UpdateAnimFrame();
            }

            switch (m_Unk30) {
                case STATE_SET:
                    m_Unk30 = STATE_IN;
                    break;
                case STATE_IN:
                    if (m_nMovieStep >= 5) {
                        m_Unk30 = STATE_ACTIVE;
                    }
                    break;
                case STATE_CLOSE:
                    m_Unk30 = STATE_OUT;
                    break;
                case STATE_OUT:
                    m_Unk30 = STATE_NONE;
                    break;
            }
        }

        CNwLayout *CBgDisplay::NwLayout()
        {
            return m_pNwLayout;
        }

        inline void CBgDisplay::SetPaneVisible(long nPane, bool bVisible)
        {
            if (m_pNwLayout != 0) {
                nw4r::lyt::Pane *pPane = m_pNwLayout->GetPane(s_PaneName[nPane]);
                if (pPane != 0) {
                    pPane->SetVisible(bVisible);
                }
            }
        }

        void CBgDisplay::MovieControl()
        {
            bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
            if (pMovie == 0) {
                return;
            }
            if (strlen(m_pMovieName) == 0) {
                return;
            }

            switch (m_nMovieStep) {
                case 1:
                    m_nMovieStep = 2;
                    pMovie->Stop();
                    break;
                case 2: {
                    nw4r::lyt::Pane *pMoviePane = m_pNwLayout->GetPane(s_PaneName[PANE_MV_00]);
                    if (pMovie->IsWait() == 1 && m_pMovieName != 0) {
                        m_pWaitMovieName = m_pMovieName;
                        m_pOpenMovieName = m_pMovieName;
                        if (pMoviePane != 0) {
                            pMoviePane->SetVisible(true);
                        }
                        if (pMovie->Open(m_pOpenMovieName, 1, 0) == 1) {
                            m_nMovieStep = 3;
                        }
                    }
                    break;
                }
                case 3:
                    if (pMovie->IsReady()) {
                        if (pMovie->Start(0)) {
                            m_nMovieStep = 4;
                        } else {
                            m_nMovieStep = 1;
                        }
                    }
                    break;
                case 4:
                    if (!pMovie->IsPlay()) {
                        break;
                    }
                    if (m_pNwLayout != 0) {
                        nw4r::lyt::Pane *pBasePane = m_pNwLayout->GetPane(s_PaneName[PANE_MOVIE_PIC]);
                        if (pBasePane != 0) {
                            nw4r::lyt::Pane *pMoviePane = m_pNwLayout->GetPane(s_PaneName[PANE_MV_00]);
                            if (pMoviePane == 0) {
                                pMoviePane = bcs::G2D::CreatePicturePane(s_PaneName[PANE_MV_00], 3);
                                if (pMoviePane != 0) {
                                    pBasePane->AppendChild(pMoviePane);
                                    bcs::G2D::SetPaneParent(pMoviePane, pBasePane, 0, 0);
                                    pMoviePane->SetSize(pBasePane->GetSize());
                                    pMoviePane->SetBasePositionH(pBasePane->GetBasePositionH());
                                    pMoviePane->SetBasePositionV(pBasePane->GetBasePositionV());
                                    pBasePane->SetVisible(true);
                                    pMoviePane->SetVisible(true);
                                }
                            }
                            SetupMoviePicture(pMoviePane, pBasePane);
                        } else {
                            UpdateMovieTexture(m_pNwLayout->GetPane(s_PaneName[PANE_MV_00]));
                        }
                        m_pNwLayout->UpdateDrawFlag();
                        SetDispParameter();
                        nw4r::lyt::Pane *pPane = m_pNwLayout->GetPane(s_PaneName[PANE_MOVIE_PIC]);
                        if (pPane != 0) {
                            pPane->SetVisible(true);
                        }
                    }
                    SetPaneVisible(PANE_MOVIE_PIC, true);
                    m_nMovieStep = 5;
                    break;
                case 5:
                    UpdateMovieTexture(m_pNwLayout->GetPane(s_PaneName[PANE_MV_00]));
                    if (strcmp(m_pMovieName, m_pOpenMovieName) != 0) {
                        m_fMovieWait = 10.0f;
                        m_pWaitMovieName = m_pMovieName;
                        m_nMovieStep = 6;
                    }
                    break;
                case 6: {
                    // A new movie was asked for: wait until the request has
                    // not changed for a while before switching.
                    UpdateMovieTexture(m_pNwLayout->GetPane(s_PaneName[PANE_MV_00]));
                    float fWait = m_fMovieWait;
                    fWait -= bcs::CMainApp::GetAppFrameRateF();
                    const char *pWaitMovieName = m_pWaitMovieName;
                    if (strcmp(m_pMovieName, pWaitMovieName) != 0) {
                        m_fMovieWait = 10.0f;
                        m_pWaitMovieName = m_pMovieName;
                    } else if (fWait <= 0.0f) {
                        if (strcmp(m_pOpenMovieName, pWaitMovieName) != 0) {
                            m_fMovieWait = 0.0f;
                            SetPaneVisible(PANE_MOVIE_PIC, false);
                            CNwLayout *pNwLayout = m_pNwLayout;
                            m_nMovieStep = 7;
                            m_pWaitMovieName = m_pMovieName;
                            if (pNwLayout != 0 && strlen(m_pMovieName) != 0) {
                                pNwLayout->OnFlag(0xC0);
                            }
                        } else {
                            m_nMovieStep = 5;
                        }
                    } else {
                        m_fMovieWait = fWait;
                    }
                    break;
                }
                case 7: {
                    UpdateMovieTexture(m_pNwLayout->GetPane(s_PaneName[PANE_MV_00]));
                    float fWait = m_fMovieWait;
                    fWait -= bcs::CMainApp::GetAppFrameRateF();
                    if (fWait <= 0.0f) {
                        fWait = 0.0f;
                        pMovie->Stop();
                        m_nMovieStep = 2;
                    }
                    m_fMovieWait = fWait;
                    break;
                }
            }
        }
    }
}

// A pane's name from its PANE_* number.
static const char *GetPaneName(long nPane)
{
    return s_PaneName[nPane];
}

namespace msw {
    namespace SelectMenu {

        void CBgDisplay::BgmControl()
        {
            switch (m_nBgmStep) {
                case 1: {
                    // A new piece was asked for: wait until the request has
                    // not changed for a while before switching.
                    float fWait = m_fBgmWait;
                    fWait -= bcs::CMainApp::GetAppFrameRateF();
                    if (m_nWaitBgm != m_nBgm) {
                        m_nWaitBgm = m_nBgm;
                        m_fBgmWait = 10.0f;
                    } else if (fWait <= 0.0f) {
                        float fFade = CSelectMenu::GetBgmFadeOutTime();
                        menu::CSndBGM::s_Instance.Stop(menu::CSndBGM::s_Instance.GetTaskId(), fFade);
                        menu::CSndBGM::s_Instance.Play(m_nBgm);
                        m_nPlayBgm = menu::CSndBGM::s_Instance.GetSeq();
                        m_nBgmStep = 2;
                    } else {
                        m_fBgmWait = fWait;
                    }
                    break;
                }
                case 2:
                    if (m_nBgm != m_nPlayBgm) {
                        float fFade = CSelectMenu::GetBgmFadeOutTime();
                        menu::CSndBGM::s_Instance.Stop(menu::CSndBGM::s_Instance.GetTaskId(), fFade);
                        m_nWaitBgm = m_nBgm;
                        m_nBgmStep = 1;
                    }
                    break;
            }
        }

        void CBgDisplay::Disp()
        {
        }

        int CBgDisplay::Set()
        {
            CSelectMenu *pMenu = GetSelectMenu();
            CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
            if (pLytCtrl == 0) {
                return -1;
            }

            unsigned char nLayout = m_nLayout;
            const char *pArcName = pMenu->GetArchiveFileName(1);
            const char *pRootName = pMenu->GetArchiveRootFolderName(1);
            if (pLytCtrl->NewArcResourceAccessor(pArcName, pRootName) != 0) {
                m_pNwLayout = new CNwLayout();
                if (m_pNwLayout == 0) {
                    return -2;
                }
                m_pNwLayout->OnFlag(0xC0);
                CNwLayout *pNwLayout = m_pNwLayout;
                pNwLayout->CreateLayout(s_LayoutName[nLayout], pMenu->GetArchiveFileName(1),
                                        pMenu->GetArchiveRootFolderName(1));
                bcs::CRenderMan::PushBack(m_pNwLayout, 6);

                CCourtChr *pCourtChr = static_cast<CCourtChr *>(GetSelectMenu()->FindObj(0x3B));
                if (pCourtChr != 0) {
                    nw4r::g3d::ResTexObj texObj = pCourtChr->GetResTexObj(s_PaneName[PANE_SCREEN2], 1);
                    if (texObj.IsValid()) {
                        m_pNwLayout->SetDrawTexture(0, texObj);
                    }
                    texObj = pCourtChr->GetResTexObj(s_PaneName[PANE_SCREEN], 0);
                    if (texObj.IsValid()) {
                        m_pNwLayout->SetDrawTexture(1, texObj);
                    }
                }

                m_Unk2C = 1;
                m_Unk30 = STATE_SET;
                ClearPushButtonId();
                m_nBgmStep = 2;
                m_nMovieStep = 1;

                BgDispParam param;
                param.m_nType = 0;
                param.m_pCupText = L"";
                SetParam(&param);
                m_pNwLayout->UpdateDrawFlag();
            }

            m_nBgm = m_nPlayBgm = menu::CSndBGM::s_Instance.GetSeq();
            return 0;
        }

        void CBgDisplay::Remove()
        {
            if (m_pNwLayout != 0) {
                m_pNwLayout->RemoveLayout();
                m_pNwLayout->DetachFromParent();
                if (m_pNwLayout != 0) {
                    bcs::CScnObj::AppendDelayDeleteObject(m_pNwLayout);
                }
                m_pNwLayout = 0;
            }
            DeleteMenuPartList();
            DeletePaneAccessor();
            DeleteLytObj();
            DeleteArcFont();

            bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
            if (pMovie != 0 && pMovie->IsPlay()) {
                pMovie->Stop();
            }
            m_nMovieStep = 0;
            m_nBgmStep = 0;
            m_Unk2C = 0;
            m_Unk30 = STATE_NONE;
        }

        long CBgDisplay::SendEvent(long nEvent, void *pParam)
        {
            long nResult = 0;
            switch (nEvent) {
                case EVENT_SET_PARAM:
                    nResult = SetParam(static_cast<const BgDispParam *>(pParam));
                    break;
                case EVENT_SET_MOVIE:
                    if (pParam != 0) {
                        m_pMovieName = static_cast<const char *>(pParam);
                        nResult = 0;
                    } else {
                        nResult = -1;
                    }
                    break;
                case EVENT_SET_MODE:
                    m_nMode = reinterpret_cast<long>(pParam);
                    break;
                case EVENT_SET_BGM:
                    m_nBgm = reinterpret_cast<long>(pParam);
                    if (m_nBgm < 0) {
                        m_nBgmStep = 0;
                    }
                    break;
            }
            return nResult;
        }
    }
}

// Copies pSrc into the nLen characters of pDst, or clears them all
// without a text.
static inline void SetText(wchar_t *pDst, const wchar_t *pSrc, long nLen)
{
    if (pSrc == 0) {
        for (long i = 0; i < nLen; i++) {
            pDst[i] = 0;
        }
    } else {
        wcsncpy(pDst, pSrc, nLen);
    }
}

namespace msw {
    namespace SelectMenu {

        long CBgDisplay::SetParam(const BgDispParam *pParam)
        {
            if (pParam == 0) {
                return -1;
            }

            if (bcs::CMainApp::GetRegionLanguage() == 0) {
                SetText(m_StageName, pParam->m_pStageName, 0x100);
                SetText(m_IntroText, pParam->m_pIntroText, 0x800);
                SetText(m_CupText, pParam->m_pCupText, 0x800);
            } else if (pParam->m_pCupText != 0) {
                SetText(m_CupText, pParam->m_pCupText, 0x800);
            } else if (pParam->m_pIntroText != 0) {
                SetText(m_CupText, pParam->m_pIntroText, 0x800);
            }
            SetText(m_RuleText[0], pParam->m_pRuleText0, 0x20);
            SetText(m_RuleText[1], pParam->m_pRuleText1, 0x20);
            SetText(m_RuleText[2], pParam->m_pRuleText2, 0x20);
            SetText(m_RuleText[4], pParam->m_pRuleText4, 0x20);
            SetText(m_RuleText[3], pParam->m_pRuleText3, 0x20);
            SetText(m_ScoreText, pParam->m_pScoreText, 0x40);
            SetText(m_ScoreLabel, pParam->m_pScoreLabel, 0x40);

            m_nType = pParam->m_nType;
            m_bStage = pParam->m_bStage;
            m_nGameKind = pParam->m_nGameKind;
            m_nRuleFlag = pParam->m_nRuleFlag;
            m_nCup = pParam->m_nCup;
            m_nPartyCup = pParam->m_nPartyCup;
            m_nPartyCup2 = pParam->m_nPartyCup2;
            m_nPartyStage = pParam->m_nPartyStage;
            m_nPartyLevel = pParam->m_nPartyLevel;

            if (m_nMode == MODE_MOVIE) {
                if (pParam->m_pMovieName != 0) {
                    SetDispParameter();
                    m_pMovieName = pParam->m_pMovieName;
                } else {
                    // No movie: hide the movie picture.
                    SetDispParameter();
                    m_pMovieName = "";
                    m_pWaitMovieName = "";
                    m_pOpenMovieName = "";
                    SetPaneVisible(PANE_MOVIE_PIC, false);
                    SetDispParameter();
                    if (m_pNwLayout != 0) {
                        m_pNwLayout->UpdateDrawFlag();
                    }
                }
            } else {
                SetDispParameter();
            }
            return 0;
        }

        void CBgDisplay::SetMessageTextNum(long nNum)
        {
        }
    }
}

// Whether nPane names a pane (PANE_NONE and above do not).
static int IsPaneName(unsigned char nPane)
{
    return nPane < PANE_SCREEN2;
}

namespace msw {
    namespace SelectMenu {

        void CBgDisplay::SetDispParameter()
        {
            if (NwLayout() == 0) {
                return;
            }

            int bSetRuleText = 0;
            int bWiFi = 0;
            // The original fetches the owner here and does not use it.
            CSelectMenu *pMenu = static_cast<CSelectMenu *>(GetParentPtr());
            (void)pMenu;
            if (dwc::CGameDwc::GetInstance() != 0) {
                bWiFi = 1;
            }

            const unsigned char *pTextBasePanes;
            if (m_bStage) {
                SetDispPanes(s_PaneList[LIST_SET].pPanes, s_PaneList[LIST_SET].nNum,
                             bcs::CMainApp::GetRegionLanguage() == 0);
                pTextBasePanes = s_PaneList[LIST_TEXT_BASE].pPanes;
                SetDispPanes(pTextBasePanes, s_PaneList[LIST_TEXT_BASE].nNum, 1);
            } else {
                SetDispPanes(s_PaneList[LIST_SET].pPanes, s_PaneList[LIST_SET].nNum, 0xFF);
                pTextBasePanes = s_PaneList[LIST_TEXT_BASE].pPanes;
                SetDispPanes(pTextBasePanes, s_PaneList[LIST_TEXT_BASE].nNum, 0xFF);
            }

            switch (m_nType) {
                case 0:
                    // A sport: its rules, or the cup.
                    SetDispPanes(s_PaneList[LIST_LEVEL].pPanes, s_PaneList[LIST_LEVEL].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_TEAM].pPanes, s_PaneList[LIST_TEAM].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_TEAM_BA].pPanes, s_PaneList[LIST_TEAM_BA].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_TEAM_DO].pPanes, s_PaneList[LIST_TEAM_DO].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_TEAM_HO].pPanes, s_PaneList[LIST_TEAM_HO].nNum, 0xFF);
                    if (m_bStage) {
                        SetDispPanes(s_PaneList[LIST_CUP_BASE].pPanes, s_PaneList[LIST_CUP_BASE].nNum, 0xFF);
                        SetDispPanes(s_PaneList[LIST_CUP_WIFI_BASE].pPanes, s_PaneList[LIST_CUP_WIFI_BASE].nNum,
                                     0xFF);
                        SetDispPanes(s_PaneList[LIST_OPTION].pPanes, s_PaneList[LIST_OPTION].nNum, m_nGameKind);
                        bSetRuleText = 1;
                        SetDispPanes(s_PaneList[LIST_MOVIE].pPanes, s_PaneList[LIST_MOVIE].nNum, 0xFF);
                        StopAllAnimBack();
                    } else {
                        if (!bWiFi) {
                            SetDispPanes(s_PaneList[LIST_CUP_BASE].pPanes, s_PaneList[LIST_CUP_BASE].nNum, 0);
                            SetDispPanes(s_PaneList[LIST_CUP].pPanes, s_PaneList[LIST_CUP].nNum, m_nCup);
                            SetDispPanes(s_PaneList[LIST_CUP_NAME].pPanes, s_PaneList[LIST_CUP_NAME].nNum, m_nCup);
                            SetDispPanes(s_PaneList[LIST_CUP_WIFI_BASE].pPanes,
                                         s_PaneList[LIST_CUP_WIFI_BASE].nNum, 0xFF);
                        } else {
                            SetDispPanes(s_PaneList[LIST_CUP_BASE].pPanes, s_PaneList[LIST_CUP_BASE].nNum, 0xFF);
                            SetDispPanes(s_PaneList[LIST_CUP_WIFI_BASE].pPanes,
                                         s_PaneList[LIST_CUP_WIFI_BASE].nNum, 0);
                            SetDispPanes(s_PaneList[LIST_CUP_WIFI].pPanes, s_PaneList[LIST_CUP_WIFI].nNum, m_nCup);
                            SetDispPanes(s_PaneList[LIST_CUP_WIFI_NAME].pPanes,
                                         s_PaneList[LIST_CUP_WIFI_NAME].nNum, m_nCup);
                        }
                        SetDispPanes(s_PaneList[LIST_OPTION].pPanes, s_PaneList[LIST_OPTION].nNum, 0xFF);
                        SetDispPanes(s_PaneList[LIST_MOVIE].pPanes, s_PaneList[LIST_MOVIE].nNum, 0);
                        const unsigned char cupAnim[4] = { 1, 3, 2, 5 };
                        SetAnimBack(cupAnim[m_nCup]);
                    }
                    break;
                case 1: {
                    // A party game with teams.
                    SetDispPanes(s_PaneList[LIST_LEVEL].pPanes, s_PaneList[LIST_LEVEL].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_CUP_BASE].pPanes, s_PaneList[LIST_CUP_BASE].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_CUP_WIFI_BASE].pPanes, s_PaneList[LIST_CUP_WIFI_BASE].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_OPTION].pPanes, s_PaneList[LIST_OPTION].nNum, 0xFF);
                    if (m_bStage) {
                        SetDispPanes(s_PaneList[LIST_TEAM].pPanes, s_PaneList[LIST_TEAM].nNum, m_nPartyLevel);
                        SetDispPanes(s_PaneList[LIST_TEAM_BA + m_nPartyLevel].pPanes,
                                     s_PaneList[LIST_TEAM_BA + m_nPartyLevel].nNum, m_nPartyCup2);
                        SetDispPanes(s_PaneList[LIST_MOVIE].pPanes, s_PaneList[LIST_MOVIE].nNum, 0xFF);
                        StopAllAnimBack();
                    } else {
                        SetDispPanes(s_PaneList[LIST_TEAM].pPanes, s_PaneList[LIST_TEAM].nNum, m_nPartyLevel);
                        SetDispPanes(s_PaneList[LIST_TEAM_BA + m_nPartyLevel].pPanes,
                                     s_PaneList[LIST_TEAM_BA + m_nPartyLevel].nNum, m_nPartyCup2);
                        SetDispPanes(s_PaneList[LIST_MOVIE].pPanes, s_PaneList[LIST_MOVIE].nNum, 0);
                    }
                    const unsigned char teamAnim[2] = { 4, 1 };
                    SetAnimBack(teamAnim[m_nPartyCup2]);
                    break;
                }
                case 2:
                    // A party game with levels.
                    SetDispPanes(s_PaneList[LIST_CUP_BASE].pPanes, s_PaneList[LIST_CUP_BASE].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_CUP_WIFI_BASE].pPanes, s_PaneList[LIST_CUP_WIFI_BASE].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_OPTION].pPanes, s_PaneList[LIST_OPTION].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_TEAM].pPanes, s_PaneList[LIST_TEAM].nNum, 0xFF);
                    if (m_bStage) {
                        SetDispPanes(s_PaneList[LIST_LEVEL].pPanes, s_PaneList[LIST_LEVEL].nNum, 1);
                        SetDispPanes(s_PaneList[LIST_LEVEL_L].pPanes, s_PaneList[LIST_LEVEL_L].nNum, 0xFF);
                        SetDispPanes(s_PaneList[LIST_LEVEL_S].pPanes, s_PaneList[LIST_LEVEL_S].nNum, m_nPartyCup);
                        SetDispPanes(s_PaneList[LIST_MOVIE].pPanes, s_PaneList[LIST_MOVIE].nNum, m_nPartyStage);
                        const unsigned char levelAnim[3] = { 1, 2, 3 };
                        SetAnimBack(levelAnim[m_nPartyCup]);
                        SetDispPanes(pTextBasePanes, s_PaneList[LIST_TEXT_BASE].nNum, 0xFF);
                    } else {
                        SetDispPanes(s_PaneList[LIST_LEVEL].pPanes, s_PaneList[LIST_LEVEL].nNum, 0);
                        SetDispPanes(s_PaneList[LIST_LEVEL_L].pPanes, s_PaneList[LIST_LEVEL_L].nNum, m_nPartyCup);
                        SetDispPanes(s_PaneList[LIST_LEVEL_S].pPanes, s_PaneList[LIST_LEVEL_S].nNum, 0xFF);
                        SetDispPanes(s_PaneList[LIST_MOVIE].pPanes, s_PaneList[LIST_MOVIE].nNum, m_nPartyStage);
                        const unsigned char levelAnim[3] = { 4, 2, 1 };
                        SetAnimBack(levelAnim[m_nPartyCup]);
                    }
                    break;
                default:
                    SetDispPanes(s_PaneList[LIST_LEVEL].pPanes, s_PaneList[LIST_LEVEL].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_CUP_BASE].pPanes, s_PaneList[LIST_CUP_BASE].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_CUP_WIFI_BASE].pPanes, s_PaneList[LIST_CUP_WIFI_BASE].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_OPTION].pPanes, s_PaneList[LIST_OPTION].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_MOVIE].pPanes, s_PaneList[LIST_MOVIE].nNum, 0xFF);
                    SetDispPanes(s_PaneList[LIST_TEAM].pPanes, s_PaneList[LIST_TEAM].nNum, 0xFF);
                    StopAllAnimBack();
                    break;
            }

            static const long optTblLen = 5;
            if (bSetRuleText) {
                const OptionPane &rOption = s_OptionPaneTbl[m_bStage ? m_nGameKind : optTblLen - 1];
                if (IsPaneName(rOption.nText[0])) {
                    NwLayout()->SetTextString(GetPaneName(rOption.nText[0]), m_RuleText[0]);
                }
                if (IsPaneName(rOption.nText[1])) {
                    NwLayout()->SetTextString(GetPaneName(rOption.nText[1]), m_RuleText[1]);
                }
                if (IsPaneName(rOption.nText[2])) {
                    NwLayout()->SetTextString(GetPaneName(rOption.nText[2]), m_RuleText[2]);
                }
                if (IsPaneName(rOption.nText[3])) {
                    NwLayout()->SetTextString(GetPaneName(rOption.nText[3]), m_RuleText[3]);
                }
                if (IsPaneName(rOption.nText[4])) {
                    NwLayout()->SetTextString(GetPaneName(rOption.nText[4]), m_RuleText[4]);
                }
            }
            NwLayout()->SetTextString(GetPaneName(PANE_T_PLAYER), m_ScoreLabel);
            NwLayout()->SetTextString(GetPaneName(PANE_T_HIGH_SCORE), m_ScoreText);
            NwLayout()->SetTextString(GetPaneName(PANE_T_TITLE), m_StageName);
            NwLayout()->SetTextString(GetPaneName(PANE_T_MES), m_IntroText);
            NwLayout()->SetTextString(GetPaneName(PANE_T_MES_00), m_CupText);
        }

        void CBgDisplay::SetDispPanes(const unsigned char *pPanes, unsigned char nNum, unsigned char nSel)
        {
            for (unsigned char i = 0; i < nNum; i++) {
                nw4r::lyt::Pane *pPane = m_pNwLayout->GetPane(s_PaneName[pPanes[i]]);
                if (pPane != 0) {
                    pPane->SetVisible(i == nSel);
                }
            }
        }

        void CBgDisplay::SetParameter(const BgDispParam *pParam)
        {
            SetParam(pParam);
        }
    }
}

// The animation link of rAnimRes on pPane itself or on one of its
// materials, or null.
static inline nw4r::lyt::AnimationLink *FindAnimationLink(nw4r::lyt::Pane *pPane, const nw4r::lyt::AnimResource &rAnimRes)
{
    nw4r::lyt::AnimationLink *pLink = pPane->FindAnimationLinkSelf(rAnimRes);
    if (pLink != 0) {
        return pLink;
    }
    unsigned char nMaterialNum = pPane->GetMaterialNum();
    for (unsigned char i = 0; i < nMaterialNum; i++) {
        pLink = pPane->GetMaterial(i)->FindAnimationLink(rAnimRes);
        if (pLink != 0) {
            return pLink;
        }
    }
    return 0;
}

namespace msw {
    namespace SelectMenu {

        long CBgDisplay::PlayPaneAnim(nw4r::lyt::Pane *pPane, long nAnim, int bEnable, int bRecursive, int bReset)
        {
            if (pPane == 0) {
                return -1;
            }

            if (bReset == 1) {
                long nAnimNum = m_pNwLayout->GetAnimNum();
                bool bRecursiveSet = bRecursive != 0;
                for (long i = 0; i < nAnimNum; i++) {
                    pPane->SetAnimationEnable(m_pNwLayout->GetAnimResource(i), false, bRecursiveSet);
                }
            }
            if (bEnable == 1) {
                nw4r::lyt::AnimResource &rAnimRes = m_pNwLayout->GetAnimResource(nAnim);
                pPane->SetAnimationEnable(rAnimRes, true, bRecursive != 0);
                nw4r::lyt::AnimationLink *pLink = FindAnimationLink(pPane, rAnimRes);
                if (pLink != 0) {
                    pLink->GetAnimTransform()->SetFrame(0.0f);
                }
            }
            return 0;
        }

        long CBgDisplay::SetAnimBack(long nAnim)
        {
            static const int enable = 1;

            const unsigned char *pPanes = s_PaneList[LIST_MOVIE].pPanes;
            for (int i = 0; i < s_PaneList[LIST_MOVIE].nNum; i++) {
                static const int recursive = 1;
                static const int reset = 1;
                nw4r::lyt::Pane *pPane = m_pNwLayout->GetPane(s_PaneName[pPanes[i]]);
                if (pPane != 0) {
                    if (pPane->IsVisible()) {
                        PlayPaneAnim(pPane, nAnim, enable, recursive, reset);
                    } else {
                        PlayPaneAnim(pPane, nAnim, !enable, recursive, reset);
                    }
                }
            }
            return 0;
        }

        long CBgDisplay::StopAllAnimBack()
        {
            static const int enable = 0;

            const unsigned char *pPanes = s_PaneList[LIST_MOVIE].pPanes;
            for (int i = 0; i < s_PaneList[LIST_MOVIE].nNum; i++) {
                static const int recursive = 1;
                static const int reset = 1;
                nw4r::lyt::Pane *pPane = m_pNwLayout->GetPane(s_PaneName[pPanes[i]]);
                if (pPane != 0) {
                    for (long nAnim = 0; nAnim < CNwLayout::ANIM_NUM; nAnim++) {
                        PlayPaneAnim(pPane, nAnim, enable, recursive, reset);
                    }
                }
            }
            return 0;
        }

        CNwLayout::~CNwLayout()
        {
            if (m_pAnimRes != 0) {
                delete[] m_pAnimRes;
            }
            m_pAnimRes = 0;
            RemoveLayout();
        }

        void CNwLayout::Initialize()
        {
            static const long arcFontNum = FONT_LINK_NUM;

            OnFlag(0xC0);
            m_pLayout = 0;
            m_pResAccessor = 0;
            for (long i = 0; i < arcFontNum; i++) {
                m_pFontRefLink[i] = 0;
            }
            m_pRestoreBuf = 0;
            for (long i = 0; i < DRAW_TEX_NUM; i++) {
                m_ResTexObj[i] = nw4r::g3d::ResTexObj();
            }
            m_pAnimRes = new (1) nw4r::lyt::AnimResource[ANIM_NUM];
            m_nAnimNum = ANIM_NUM;
        }

        void CNwLayout::MainLoop()
        {
        }

        void CNwLayout::DispLoop()
        {
        }

        long CNwLayout::CreateLayout(const char *pLayoutName, const char *pArcName, const char *pRootName)
        {
            static const long arcFontNum = 3;

            CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
            if (pLytCtrl == 0) {
                return -1;
            }
            nw4r::lyt::ArcResourceAccessor *pResAccessor = pLytCtrl->NewArcResourceAccessor(pArcName, pRootName);
            if (pResAccessor == 0) {
                return -2;
            }
            m_pResAccessor = pResAccessor;

            const long fontId[arcFontNum] = { 0, 1, 3 };
            for (long i = 0; i < arcFontNum; i++) {
                long nFont = fontId[i];
                nw4r::lyt::FontRefLink *pFontRefLink = new nw4r::lyt::FontRefLink();
                if (pFontRefLink != 0) {
                    const char *pFontName = pLytCtrl->GetFontResourceName(nFont);
                    nw4r::ut::Font *pFont = pLytCtrl->GetArchiveFont(nFont);
                    pFontRefLink->Set(pFontName, pFont);
                    pResAccessor->RegistFont(pFontRefLink);
                }
                m_pFontRefLink[i] = pFontRefLink;
            }

            nw4r::lyt::Layout *pLayout = new nw4r::lyt::Layout();
            if (pLayout != 0) {
                void *pLytRes = pResAccessor->GetResource('blyt', pLayoutName, 0);
                if (pLytRes != 0) {
                    pLayout->Build(pLytRes, pResAccessor);
                }

                // The layout's origin in the top left corner of the frame.
                GXRenderModeObj *pRenderMode = bcs::CMainApp::GetRenderModeObj();
                unsigned short nHalfWidth = pRenderMode->fbWidth / 2;
                unsigned short nHalfHeight = pRenderMode->efbHeight / 2;
                nw4r::math::VEC3 translate(-nHalfWidth, nHalfHeight, 0.0f);
                pLayout->GetRootPane()->SetTranslate(translate);

                for (long i = 0; i < ANIM_NUM; i++) {
                    void *lpaRes = pResAccessor->GetResource('anim', s_AnimName[i], 0);
                    if (lpaRes == 0) {
                        nw4r::db::Panic("SelectMenuBgDisp.cpp", 2521, "NW4R:Pointer must not be NULL (lpaRes)");
                    }
                    m_pAnimRes[i].Set(lpaRes);
                    pLayout->BindAnimationAuto(m_pAnimRes[i], pResAccessor);
                }
                m_pLayout = pLayout;
                SetRestoreBuffer();
            }
            return 0;
        }

        void CNwLayout::RemoveLayout()
        {
            nw4r::lyt::ArcResourceAccessor *pResAccessor;
            if (m_pLayout != 0) {
                delete m_pLayout;
                CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
                if (pLytCtrl != 0) {
                    pResAccessor = m_pResAccessor;
                    if (pResAccessor != 0) {
                        static const long arcFontNum = FONT_LINK_NUM;
                        for (long i = 0; i < arcFontNum; i++) {
                            if (m_pFontRefLink[i] != 0) {
                                pLytCtrl->UnregistFont(pResAccessor, m_pFontRefLink[i]);
                            }
                        }
                        pLytCtrl->DeleteArcResourceAccessor(pResAccessor);
                    }
                }
            }
            m_pLayout = 0;
            OnFlag(0xC0);
            for (long i = 0; i < FONT_LINK_NUM; i++) {
                if (m_pFontRefLink[i] != 0) {
                    delete m_pFontRefLink[i];
                }
                m_pFontRefLink[i] = 0;
            }
            m_pResAccessor = 0;
            if (m_pRestoreBuf != 0) {
                delete[] static_cast<unsigned char *>(m_pRestoreBuf);
                m_pRestoreBuf = 0;
            }
        }

        inline nw4r::lyt::Pane *CNwLayout::GetPane(const char *pName)
        {
            static const bool recursive = true;
            nw4r::lyt::Pane *pRootPane = m_pLayout->GetRootPane();
            if (pRootPane == 0) {
                return 0;
            }
            return pRootPane->FindPaneByName(pName, recursive);
        }

        long CNwLayout::SetTextString(const char *pPaneName, const wchar_t *pText)
        {
            if (pText == 0) {
                return -1;
            }
            nw4r::lyt::TextBox *pTextBox = nw4r::ut::DynamicCast<nw4r::lyt::TextBox *>(GetPane(pPaneName));
            if (pTextBox == 0) {
                return -2;
            }
            unsigned short nLen = wcslen(pText);
            pTextBox->AllocStringBuffer(nLen);
            pTextBox->SetString(pText, 0);
            return nLen;
        }

        void CNwLayout::EFBClear(void *pBuffer, unsigned short nX, unsigned short nY)
        {
            if (pBuffer == 0) {
                return;
            }
            static const GXColor EFB_CLEAR_COLOR = nw4r::ut::Color(0xFFFFFFFF);

            unsigned short nWidth = m_nRestoreWidth;
            unsigned short nHeight = m_nRestoreHeight;
            GXTexFmt format = m_RestoreFormat;
            unsigned char bMipmap = m_bRestoreMipmap;
            GXSetAlphaUpdate(GX_TRUE);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
            GXSetCopyClear(EFB_CLEAR_COLOR, 0xFFFFFF);
            GXSetTexCopySrc(nX, nY, nWidth, nHeight);
            GXSetTexCopyDst(nWidth, nHeight, format, bMipmap);
            GXSetCopyFilter(GX_FALSE, 0, GX_FALSE, 0);
            GXCopyTex(pBuffer, GX_TRUE);
            GXRenderModeObj *pRenderMode = bcs::CMainApp::GetRenderModeObj();
            GXSetCopyFilter(pRenderMode->aa, pRenderMode->sample_pattern, GX_TRUE, pRenderMode->vfilter);
            GXInvalidateTexAll();
        }

        void CNwLayout::EFBRestore(void *pBuffer, unsigned short nX, unsigned short nY)
        {
            if (pBuffer == 0) {
                return;
            }

            unsigned short nWidth = m_nRestoreWidth;
            unsigned short nHeight = m_nRestoreHeight;
            GXTexFmt format = m_RestoreFormat;
            unsigned char bMipmap = m_bRestoreMipmap;
            GXPixModeSync();
            GXSetAlphaUpdate(GX_TRUE);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
            GXTexObj texObj;
            GXInitTexObj(&texObj, pBuffer, nWidth, nHeight, format, GX_CLAMP, GX_CLAMP, bMipmap);
            GXLoadTexObj(&texObj, GX_TEXMAP0);
            bcs::BCS_GX_DrawSpriteEx(nX, nY, nWidth, nHeight, bcs::BCS_Color(0x80808080), 0.0f, 0.0f, 1.0f, 1.0f, 0,
                                     1);
        }

        void CNwLayout::EFBCopyToTex(const GXTexObj *pTexObj, unsigned short nX, unsigned short nY,
                                     unsigned short nWidth, unsigned short nHeight, unsigned char bClear)
        {
            if (pTexObj == 0) {
                return;
            }

            void *pData = GXGetTexObjData(pTexObj);
            unsigned short nTexWidth = GXGetTexObjWidth(pTexObj);
            unsigned short nTexHeight = GXGetTexObjHeight(pTexObj);
            unsigned char bMipmap = GXGetTexObjMipMap(pTexObj);
            GXTexFmt format = GXGetTexObjFmt(pTexObj);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
            GXSetTexCopySrc(nX, nY, nWidth, nHeight);
            GXSetTexCopyDst(nTexWidth, nTexHeight, format, bMipmap);
            GXCopyTex(pData, bClear);
            GXPixModeSync();
        }

        void CNwLayout::SetRestoreBuffer()
        {
            nw4r::lyt::Pane *pPane = GetPane(s_PaneName[PANE_ALL_NULL]);
            if (pPane == 0) {
                return;
            }

            nw4r::lyt::Size size = pPane->GetSize();
            unsigned short nWidth;
            unsigned short nHeight;
            F32ToU16(size.width, &nWidth);
            F32ToU16(size.height, &nHeight);
            GXTexFmt format = GX_TF_RGB565;
            unsigned char bMipmap = 0;
            unsigned long nSize = GXGetTexBufferSize(nWidth, nHeight, format, bMipmap, 0);
            unsigned char *pBuffer = new (0x101, 0x20) unsigned char[nSize];
            if (pBuffer != 0) {
                if (m_pRestoreBuf != 0) {
                    delete[] static_cast<unsigned char *>(m_pRestoreBuf);
                }
                m_pRestoreBuf = pBuffer;
                m_nRestoreWidth = nWidth;
                m_nRestoreHeight = nHeight;
                m_RestoreFormat = format;
                m_bRestoreMipmap = bMipmap;
            }
        }

        void CNwLayout::ObjProc(unsigned long nTask, unsigned long nParam, void *pInfo)
        {
            if (IsProcDisabled(nTask)) {
                return;
            }

            switch (static_cast<long>(nTask)) {
                case 4:
                    SetDLOpa(s_DummyDL, sizeof(s_DummyDL));
                    bcs::CGDLObj::ObjProc(nTask, nParam, pInfo);
                    break;
                case 6:
                    GetLayoutDrawTexture();
                    nw4r::g3d::G3DState::Invalidate(0x7FF);
                    break;
                default:
                    bcs::CGDLObj::ObjProc(nTask, nParam, pInfo);
                    break;
            }
        }

        void CNwLayout::GetLayoutDrawTexture()
        {
            static const unsigned short efbx = 0;

            if (m_pRestoreBuf == 0) {
                return;
            }
            static const unsigned short efby = 0;

            unsigned char bClear = m_bRestoreMipmap;
            EFBClear(m_pRestoreBuf, efbx, efby);
            nw4r::lyt::Layout *pLayout = m_pLayout;
            nw4r::lyt::DrawInfo drawInfo;
            bcs::G2D::CLytManager *pLytManager = bcs::G2D::CLytManager::GetInstance();
            if (pLytManager != 0 && pLayout != 0) {
                static const int isLocationAdjust = 1;

                const bcs::G2D::CMenuScnGroup::OrthoProjInfo &rInfo = bcs::G2D::CMenuScnGroup::s_OrthoProjInfo;
                float fLeft = rInfo.fLeft;
                float fRight = rInfo.fRight;
                float fBottom = rInfo.fBottom;
                float fTop = rInfo.fTop;
                nw4r::math::MTX34 viewMtx;
                Mtx44 projMtx;
                C_MTXOrtho(projMtx, fTop, fBottom, fLeft, fRight, rInfo.fNear, rInfo.fFar);
                PSMTXIdentity(reinterpret_cast<float(*)[4]>(&viewMtx));
                drawInfo.SetViewMtx(viewMtx);
                nw4r::ut::Rect viewRect;
                viewRect.left = fLeft;
                viewRect.top = fTop;
                viewRect.right = fRight;
                viewRect.bottom = fBottom;
                drawInfo.SetViewRect(viewRect);
                float fScaleH = pLytManager->GetLocationAdjustScaleH(isLocationAdjust);
                float fScaleV = pLytManager->GetLocationAdjustScaleV(isLocationAdjust);
                nw4r::math::VEC2 scale(fScaleH, fScaleV);
                drawInfo.SetLocationAdjustScale(scale);
                drawInfo.SetLocationAdjust(true);

                pLayout->Animate(0);
                pLayout->CalculateMtx(drawInfo);
                GXSetProjection(projMtx, GX_ORTHOGRAPHIC);
                GXSetZTexture(GX_ZT_DISABLE, (GXTexFmt)0x11, 0);
                GXColor fogColor = { 0, 0, 0, 0 };
                GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
                GXSetZMode(GX_FALSE, GX_NEVER, GX_FALSE);
                GXSetCoPlanar(GX_FALSE);
                GXSetCullMode(GX_CULL_NONE);
                GXSetAlphaUpdate(GX_TRUE);
                GXSetColorUpdate(GX_TRUE);
                pLayout->Draw(drawInfo);
            }

            // Copy the drawn layout into the model textures, one under the
            // other, then put the frame back.
            unsigned short nY = 0;
            for (long i = 0; i < DRAW_TEX_NUM; i++) {
                nw4r::g3d::ResTexObj texObj = m_ResTexObj[i];
                if (texObj.IsValid()) {
                    GXTexObj *pTexObj = texObj.GetTexObj(m_TexMapId[i]);
                    if (pTexObj == 0) {
                        return;
                    }
                    unsigned short nWidth = GXGetTexObjWidth(pTexObj);
                    unsigned short nHeight = GXGetTexObjHeight(pTexObj);
                    EFBCopyToTex(pTexObj, 0, nY, nWidth, nHeight, bClear);
                    nY += nHeight;
                }
            }
            EFBRestore(m_pRestoreBuf, efbx, efby);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        }

        void CNwLayout::UpdateAnimFrame()
        {
            nw4r::lyt::Layout *pLayout = m_pLayout;
            if (pLayout == 0) {
                return;
            }

            float fRate = bcs::CMainApp::GetAppFrameRateF();
            long nAnimNum = m_nAnimNum;
            for (long i = 0; i < nAnimNum; i++) {
                nw4r::lyt::AnimResource &rAnimRes = m_pAnimRes[i];
                unsigned short nGroupNum = rAnimRes.GetGroupNum();
                const nw4r::lyt::AnimationGroupRef *pGroups = rAnimRes.GetGroupArray();
                for (long j = 0; j < nGroupNum; j++) {
                    nw4r::lyt::Group *pGroup = pLayout->GetGroupContainer()->FindGroupByName(pGroups[j].GetName());
                    if (pGroup == 0) {
                        continue;
                    }
                    nw4r::lyt::PaneLinkList &rList = pGroup->GetPaneList();
                    for (nw4r::lyt::PaneLinkList::Iterator it = rList.GetBeginIter(); it != rList.GetEndIter(); ++it) {
                        nw4r::lyt::AnimationLink *pLink = FindAnimationLink(it->mTarget, rAnimRes);
                        if (pLink == 0 || !pLink->IsEnable()) {
                            continue;
                        }

                        // Advance the animation of the first pane of the
                        // group that plays it, looping or stopping at the end.
                        nw4r::lyt::AnimTransform *pAnimTrans = pLink->GetAnimTransform();
                        float fFrame = pAnimTrans->GetFrame();
                        if (pAnimTrans->IsLoopData()) {
                            float fFrameSize = pAnimTrans->GetFrameSize();
                            fFrame += fRate;
                            if (fFrame >= fFrameSize) {
                                fFrame -= fFrameSize;
                            }
                        } else {
                            float fLast = pAnimTrans->GetFrameSize() - 1;
                            fFrame += fRate;
                            fFrame = fLast > fFrame ? fFrame : fLast;
                        }
                        pAnimTrans->SetFrame(fFrame);
                        j = nGroupNum;
                        break;
                    }
                }
            }
        }
    }
}
