// msw::CStageHockey10: hockey stage 10, the panel stage. Panels of several
// kinds appear on the court (CStage10Base, a DOL class, runs the board);
// shooting from a panel and scoring the goal scores it. Written from
// BasketF's CStageBasket10, the same stage.
#include "game/msw/CStageHockey10.hpp"

#include <stdio.h>

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/HockeyPrm.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStgMdl.hpp"
#include "game/msw/CCockpitHockey.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/g3d/Camera.hpp"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, the value, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum S10_PRM_TYPE {
        S10_PRM_TYPE_END,
        S10_PRM_TYPE_LONG,
        S10_PRM_TYPE_FLOAT
    };

    struct S10_PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    static const S10_PRM_TBL s_PrmTbl[] = {
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_RESPAWN_TIME, 0x00 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_RESPAWN_RND, 0x04 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_SIZE_A, 0x08 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_CYCLE_A, 0x0C },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_LIFE_A, 0x10 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_LIFE_RND_A, 0x14 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_SIZE_B, 0x18 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_SIZE_END_B, 0x1C },
        { S10_PRM_TYPE_LONG, &HOCKEY_PRM_S10_PANEL_RATE_BEG_B, 0x20 },
        { S10_PRM_TYPE_LONG, &HOCKEY_PRM_S10_PANEL_RATE_END_B, 0x24 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_LIFE_B, 0x28 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_LIFE_RND_B, 0x2C },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_SIZE_C, 0x30 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_CYCLE_C, 0x34 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_LIFE_C, 0x38 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_LIFE_RND_C, 0x3C },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_SIZE_Q, 0x40 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_LIFE_Q, 0x44 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_LIFE_RND_Q, 0x48 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_BONUS_START_TIME_0, 0x4C },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_BONUS_START_TIME_1, 0x50 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_BONUS_START_TIME_2, 0x54 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_BONUS_START_TIME_3, 0x58 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_BONUS_START_TIME_4, 0x5C },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_INTERVAL_D, 0x60 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_LIFE_D, 0x64 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_LIFE_RND_D, 0x68 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_SIZE_E, 0x6C },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_SPEED0_E, 0x70 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_SPEED1_E, 0x74 },
        { S10_PRM_TYPE_FLOAT, &HOCKEY_PRM_S10_PANEL_INTERVAL_E, 0x78 },
        { S10_PRM_TYPE_LONG, &HOCKEY_PRM_S10_PANEL_SET, 0x7C },
        { S10_PRM_TYPE_END, 0, 0 },
    };

    // The arrangements: three sets (HOCKEY_PRM_S10_PANEL_SET picks one) of
    // five groups, all three the same here. Positions are on the court, the
    // third value the width the panel moves over.
    const CStage10Base::PANEL_ARRANGE_GROUP CStageHockey10::s_PanelArrangeSet[3][5] = {
        {
            { 1, { { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
            { 1, { { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
            { 1, { { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
            { 2, { { 1, 5.0f, -17.0f, 0.0f }, { 1, -5.0f, -17.0f, 0.0f }, { 2, 0.0f, -10.0f, 14.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
            { 2, { { 2, 0.0f, 10.0f, 14.0f }, { 1, 5.0f, 17.0f, 0.0f }, { 1, -5.0f, 17.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
        },
        {
            { 1, { { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
            { 1, { { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
            { 1, { { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
            { 2, { { 1, 5.0f, -17.0f, 0.0f }, { 1, -5.0f, -17.0f, 0.0f }, { 2, 0.0f, -10.0f, 14.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
            { 2, { { 2, 0.0f, 10.0f, 14.0f }, { 1, 5.0f, 17.0f, 0.0f }, { 1, -5.0f, 17.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
        },
        {
            { 1, { { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
            { 1, { { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
            { 1, { { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
            { 2, { { 1, 5.0f, -17.0f, 0.0f }, { 1, -5.0f, -17.0f, 0.0f }, { 2, 0.0f, -10.0f, 14.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
            { 2, { { 2, 0.0f, 10.0f, 14.0f }, { 1, 5.0f, 17.0f, 0.0f }, { 1, -5.0f, 17.0f, 0.0f }, { -1, 0.0f, 0.0f, 0.0f } } },
        },
    };

    // The odds of the kinds while the team leads by at least 10, is level
    // or behind by up to 10, and behind by more (the same three).
    const CStage10Base::ARRANGE_ODDS CStageHockey10::s_ArrangeOddsTbl[3] = {
        { 10, { { 2, 1, 30 }, { 2, 2, 20 }, { 2, 3, 2 }, { 5, 0, 25 }, { 6, 0, 25 }, { 7, 0, 2 }, { 0, 0, -1 } } },
        { 0, { { 2, 1, 30 }, { 2, 2, 20 }, { 2, 3, 2 }, { 5, 0, 25 }, { 6, 0, 25 }, { 7, 0, 2 }, { 0, 0, -1 } } },
        { -10, { { 2, 1, 30 }, { 2, 2, 20 }, { 2, 3, 2 }, { 5, 0, 25 }, { 6, 0, 25 }, { 7, 0, 2 }, { 0, 0, -1 } } },
    };

    // The tuning of each panel kind (the lower 16 bits of a panel's type).
    // Kind 1 is the panel model "h", 2 to 4 "a", 5 "b", 6 "c", 7 "d", 8
    // "e" and 9 "f"; the kinds left out have no value.
    static const float s_PanelSize[10] = {
        0.0f,
        HOCKEY_PRM_S10_PANEL_SIZE_Q,
        HOCKEY_PRM_S10_PANEL_SIZE_A,
        HOCKEY_PRM_S10_PANEL_SIZE_A,
        HOCKEY_PRM_S10_PANEL_SIZE_A,
        HOCKEY_PRM_S10_PANEL_SIZE_B,
        HOCKEY_PRM_S10_PANEL_SIZE_C,
        HOCKEY_PRM_S10_PANEL_SIZE_D,
        HOCKEY_PRM_S10_PANEL_SIZE_E,
        0.0f,
    };
    static const float s_PanelCycle[10] = {
        0.0f,
        0.0f,
        HOCKEY_PRM_S10_PANEL_CYCLE_A,
        HOCKEY_PRM_S10_PANEL_CYCLE_A,
        HOCKEY_PRM_S10_PANEL_CYCLE_A,
        0.0f,
        HOCKEY_PRM_S10_PANEL_CYCLE_C,
        0.0f,
        0.0f,
        0.0f,
    };
    static const float s_PanelLife[10] = {
        0.0f,
        HOCKEY_PRM_S10_PANEL_LIFE_Q,
        HOCKEY_PRM_S10_PANEL_LIFE_A,
        HOCKEY_PRM_S10_PANEL_LIFE_A,
        HOCKEY_PRM_S10_PANEL_LIFE_A,
        HOCKEY_PRM_S10_PANEL_LIFE_B,
        HOCKEY_PRM_S10_PANEL_LIFE_C,
        HOCKEY_PRM_S10_PANEL_LIFE_D,
        0.0f,
        0.0f,
    };
    static const float s_PanelLifeRand[10] = {
        0.0f,
        HOCKEY_PRM_S10_PANEL_LIFE_RND_Q,
        HOCKEY_PRM_S10_PANEL_LIFE_RND_A,
        HOCKEY_PRM_S10_PANEL_LIFE_RND_A,
        HOCKEY_PRM_S10_PANEL_LIFE_RND_A,
        HOCKEY_PRM_S10_PANEL_LIFE_RND_B,
        HOCKEY_PRM_S10_PANEL_LIFE_RND_C,
        HOCKEY_PRM_S10_PANEL_LIFE_RND_D,
        0.0f,
        0.0f,
    };
    static const float s_PanelInterval[10] = {
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        HOCKEY_PRM_S10_PANEL_INTERVAL_D,
        HOCKEY_PRM_S10_PANEL_INTERVAL_E,
        0.0f,
    };
}

// Model kinds of the stage's MDL_GEN_RULE table.
enum {
    MDL_PANEL = 1,       // "s<n>_panel_<kind><no>": a panel model
    MDL_PANEL_ALT = 10,  // (not in this table) taken as a panel model too
    MDL_GROUND_02 = 0x64, // "s<n>_ground_02": drawn first
    MDL_FEVER = 0x65,    // "s<n>_ground_03": the fever model, for CStage10Base
    MDL_GROUND_04 = 0x66 // "s<n>_ground_04": drawn after the court
};

// Sound slots and sounds of the stage; SE_GetSlotNum gives the five.
enum {
    SE_SLOT_AMBIENT0 = 0,
    SE_SLOT_AMBIENT2 = 2,
    SE_SLOT_EVENT = 3,
    SE_SLOT_BONUS = 4,
    SE_AMBIENT0 = 0xD54,
    SE_AMBIENT2 = 0xD60,
    SE_EVENT = 0xD61,
    SE_BONUS = 0xD53
};

// Stage messages handled here (sent through CStageMan::SendMessage).
enum {
    MSG_AMBIENT0 = 3,    // (meaning not known) starts the ambient sound 0
    MSG_AMBIENT0_2 = 0x13,
    MSG_RULE = 0x27,     // arg1 7: the extra rule
    MSG_RESET_ROT = 0x30 // the panels turn back
};

// Panel kinds named in the code.
enum {
    PANEL_GOAL = 7, // ridden within a circle round the goal
    PANEL_BONUS = 9
};

const void *msw::CStageHockey10::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageHockey10::CStageHockey10(bcs::CTask *pParent, long) : CStageManHockey(pParent), CStage10Base()
{
}

msw::CStageHockey10::~CStageHockey10()
{
    DeletePanelAll();
}

void msw::CStageHockey10::Main_(float frame)
{
    Stage10_Main_(frame);
    CStageManHockey::Main_(frame);
}

void msw::CStageHockey10::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManHockey::SendMessage(msg, arg1, arg2, arg3);

    switch (msg) {
        case MSG_RESET_ROT:
            ResetPanelRotAll();
            break;
        case MSG_AMBIENT0:
        case MSG_AMBIENT0_2:
            if (!SE_IsAlive(SE_SLOT_AMBIENT0)) {
                SE_Play(SE_SLOT_AMBIENT0, SE_AMBIENT0, 0.0f, 3);
            }
            break;
        case MSG_RULE:
            if (arg1 == 7) {
                SetExRule(1);
            }
            break;
    }
}

// The model of the panel kind and number of nType (number in the upper 16
// bits); number 1 of the kind when that one was not in the archive.
msw::CStage10Base::CPanelInfoBase *msw::CStageHockey10::NewPanel_(long nType) const
{
    const PANEL_MDL_TMPL *pTmpl = &m_PanelMdl[nType & 0xFFFF][(nType >> 16) & 0xFFFF];
    if (pTmpl->m_nIdx < 0) {
        pTmpl = &m_PanelMdl[nType & 0xFFFF][1];
    }

    CPanelInfoBase *pPanel = new (1) CPanelInfo(this, nType);
    if (pPanel != 0) {
        pPanel->Create(*pTmpl);
    }
    return pPanel;
}

void msw::CStageHockey10::AddBonusPanel_()
{
    CStage10Base::AddBonusPanel_();
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageHockey10::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageManHockey.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_panel_(a)([0-9]+)", MDL_PANEL, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_panel_(b)([0-9]+)", MDL_PANEL, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_panel_(c)([0-9]+)", MDL_PANEL, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_panel_(d)([0-9]+)", MDL_PANEL, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_panel_(e)([0-9]+)", MDL_PANEL, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_panel_(f)([0-9]+)", MDL_PANEL, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_panel_(h)([0-9]+)", MDL_PANEL, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_ground_02", MDL_GROUND_02, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_ground_03", MDL_FEVER, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_ground_04", MDL_GROUND_04, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_borde_01", MDL_GROUND_04, 0, 0, CStageMan::NewMdl }, // "borde" as in the original
        { 0, reinterpret_cast<long>(CStageManHockey::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

// The panel models are kept for NewPanel_ instead of being made here: the
// kind letter (sub-match 1) and the number (sub-match 2) give the slot. The
// models of kind "a" serve kinds 2 to 4. The fever model goes to
// CStage10Base.
bool msw::CStageHockey10::OnPreCreateObj(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex, nw4r::g3d::ResFile file,
                                         CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_PANEL:
        case MDL_PANEL_ALT: {
            const char *pName = rArn.GetName(n);
            long no;
            sscanf(pName + rRegex.GetSubMatchPos(2), "%d", &no);

            // A letter not in the list leaves the kind unset, as in the
            // original.
            long kind;
            switch (pName[rRegex.GetSubMatchPos(1)]) {
                case 'a':
                    kind = 2;
                    break;
                case 'b':
                    kind = 5;
                    break;
                case 'c':
                    kind = 6;
                    break;
                case 'd':
                    kind = 7;
                    break;
                case 'e':
                    kind = 8;
                    break;
                case 'f':
                    kind = 9;
                    break;
                case 'h':
                    kind = 1;
                    break;
            }

            PANEL_MDL_TMPL &rTmpl = m_PanelMdl[kind][no];
            rTmpl.m_pArn = &rArn;
            rTmpl.m_nIdx = n;
            if (kind == 2) {
                for (long i = 0; i < 3; i++) {
                    PANEL_MDL_TMPL &rCopy = m_PanelMdl[kind + i][no];
                    rCopy.m_pArn = &rArn;
                    rCopy.m_nIdx = n;
                }
            }
            return false;
        }
        case MDL_FEVER:
            CreateFeverMdl_(rArn, n);
            return false;
        default:
            return true;
    }
}

void msw::CStageHockey10::OnPostCreateObj(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex, bcs::CScnObj *pObj,
                                          nw4r::g3d::ResFile file, CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_GROUND_02:
            if (pObj != 0) {
                pObj->SetDrawPriorityOpa(1);
                pObj->SetDrawPriorityXlu(1);
            }
            break;
        case MDL_GROUND_04:
            if (pObj != 0) {
                pObj->SetDrawPriorityOpa(6);
                pObj->SetDrawPriorityXlu(6);
            }
            break;
    }
}

void msw::CStageHockey10::OnSceneEntry()
{
}

void msw::CStageHockey10::OnPostInitialize()
{
    CStageManHockey::OnPostInitialize();

    // Projection camera of view 0: orthographic, 4 units high, looking down
    // from straight above with +X as up (as in VolleyF's CStageVolley42).
    nw4r::g3d::CameraData data;
    nw4r::g3d::Camera camera(&data);
    camera.Init();
    float aspect = 1.0f;
    float height = 4.0f;
    float width = height * aspect;
    camera.SetOrtho(0.5f * height, 0.5f * -height, 0.5f * -width, 0.5f * width, 0.5f, 10000.0f);
    camera.SetPosition(nw4r::math::VEC3(0.0f, 1.0f, 0.0f));

    nw4r::g3d::Camera::PostureInfo posture;
    posture.tp = nw4r::g3d::Camera::POSTURE_LOOKAT;
    posture.cameraUp = nw4r::math::VEC3(1.0f, 0.0f, 0.0f);
    posture.cameraTarget = nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
    camera.SetPosture(posture);

    bcs::CWorldEnv::SetProjCameraData(0, data);
}

void msw::CStageHockey10::OnGoalIn(long, GOAL_IN_TYPE)
{
}

void msw::CStageHockey10::OnPeriodInit()
{
    Stage10_OnPeriodInit();
}

void msw::CStageHockey10::OnTurnInit()
{
    Stage10_OnTurnInit();
}

void msw::CStageHockey10::OnGimmickReset()
{
    Stage10_OnGimmickReset();
}

void msw::CStageHockey10::OnGimmickStart()
{
    Stage10_OnGimmickStart();
}

void msw::CStageHockey10::OnGimmickStop()
{
    Stage10_OnGimmickStop();
}

void msw::CStageHockey10::OnGimmickCancel()
{
    Stage10_OnGimmickCancel();
}

void msw::CStageHockey10::OnGimmickPause()
{
    Stage10_OnGimmickPause();
}

void msw::CStageHockey10::OnGimmickResume()
{
    Stage10_OnGimmickResume();
}

// A goal scores the panel being played for (or fails it, with a miss);
// the goal side panel is left as it is. The bonus panel is not scored that
// way, it plays its sound on a goal.
void msw::CStageHockey10::OnHockeyGoalResult(int bScored)
{
    CPanelInfoBase *pPanel = GetActivePanel_();

    if (pPanel != 0 && (pPanel->GetType() & 0xFFFF) != PANEL_BONUS) {
        if (bScored) {
            if ((pPanel->GetType() & 0xFFFF) != PANEL_GOAL) {
                pPanel->SetScored();
            }
        } else {
            if ((pPanel->GetType() & 0xFFFF) != PANEL_GOAL) {
                pPanel->SetFailed();
            }
        }
    }

    if (pPanel != 0 && (pPanel->GetType() & 0xFFFF) == PANEL_BONUS && bScored) {
        SE_Play(SE_SLOT_BONUS, SE_BONUS, 0.0f, 0);
    }
}

// A shot starts the panel the shooter stands on and shows its rate over
// the shooter.
void msw::CStageHockey10::OnHockeyShoot(long nChr)
{
    CChrCore *pChr = CChrMan::GetChrCore(nChr);
    if (pChr != 0 && pChr->IsEnable()) {
        TriggerPanel_(pChr->GetPos());

        CPanelInfoBase *pPanel = GetActivePanel_();
        if (pPanel != 0 && nChr >= 0) {
            pPanel->RateDisp(nChr);
        }
    }
}

void msw::CStageHockey10::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(SE_SLOT_AMBIENT2)) {
                SE_Play(SE_SLOT_AMBIENT2, SE_AMBIENT2, 0.0f, 3);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

float msw::CStageHockey10::GetPanelParam_Size(long nType) const
{
    float size = s_PanelSize[nType & 0xFFFF];
    if (size) {
        return size;
    }
    return 0.0f;
}

float msw::CStageHockey10::GetPanelParam_Cycle(long nType) const
{
    float cycle = s_PanelCycle[nType & 0xFFFF];
    if (cycle) {
        return cycle;
    }
    return 0.0f;
}

float msw::CStageHockey10::GetPanelParam_Life(long nType) const
{
    float life = s_PanelLife[nType & 0xFFFF];
    if (life) {
        return life;
    }
    return 0.0f;
}

float msw::CStageHockey10::GetPanelParam_LifeRand(long nType) const
{
    float rand = s_PanelLifeRand[nType & 0xFFFF];
    if (rand) {
        return rand;
    }
    return 0.0f;
}

long msw::CStageHockey10::GetPanelParam_RateBeg(long nType) const
{
    return HOCKEY_PRM_S10_PANEL_RATE_BEG_B;
}

long msw::CStageHockey10::GetPanelParam_RateEnd(long nType) const
{
    return HOCKEY_PRM_S10_PANEL_RATE_END_B;
}

float msw::CStageHockey10::GetPanelParam_Speed0(long nType) const
{
    return HOCKEY_PRM_S10_PANEL_SPEED0_E;
}

float msw::CStageHockey10::GetPanelParam_Speed1(long nType) const
{
    return HOCKEY_PRM_S10_PANEL_SPEED1_E;
}

float msw::CStageHockey10::GetPanelParam_StartTime(long nType) const
{
    return 100.0f;
}

float msw::CStageHockey10::GetPanelParam_Interval(long nType) const
{
    float interval = s_PanelInterval[nType & 0xFFFF];
    if (interval) {
        return interval;
    }
    return 0.0f;
}

long msw::CStageHockey10::GetParam_ArrangeSetNum() const
{
    return 3;
}

long msw::CStageHockey10::GetParam_ArrangeSet() const
{
    return HOCKEY_PRM_S10_PANEL_SET;
}

float msw::CStageHockey10::GetParam_PanelRespawnTime() const
{
    return HOCKEY_PRM_S10_PANEL_RESPAWN_TIME + bcs::BCS_RandMax(HOCKEY_PRM_S10_PANEL_RESPAWN_RND);
}

long msw::CStageHockey10::GetParam_BonusPanelNum() const
{
    return 4;
}

// The start times of the bonus panels, in frames.
float msw::CStageHockey10::GetParam_BonusStartTime(long n) const
{
    switch (n) {
        case 0:
            return 60.0f * HOCKEY_PRM_S10_BONUS_START_TIME_0;
        case 1:
            return 60.0f * HOCKEY_PRM_S10_BONUS_START_TIME_1;
        case 2:
            return 60.0f * HOCKEY_PRM_S10_BONUS_START_TIME_2;
        case 3:
            return 60.0f * HOCKEY_PRM_S10_BONUS_START_TIME_3;
        case 4:
            return 60.0f * HOCKEY_PRM_S10_BONUS_START_TIME_4;
        default:
            return 0.0f;
    }
}

long msw::CStageHockey10::GetPanelMaxNum()
{
    return static_cast<CStageHockey10 *>(CStageMan::GetInstance())->GetPanelMaxNum_();
}

msw::CStage10Base::CPanelInfoBase *msw::CStageHockey10::GetNextPanel(const CPanelInfoBase *pPanel)
{
    return static_cast<CStageHockey10 *>(CStageMan::GetInstance())->GetNextPanel_(pPanel);
}

int msw::CStageHockey10::IsNowFeverTime()
{
    return static_cast<CStageHockey10 *>(CStageMan::GetInstance())->IsNowFeverTime_();
}

msw::CStageHockey10::CPanelInfo::~CPanelInfo()
{
}

// Plays the sound of the panel (another one for a panel with no bonus
// rate) unless it is scored already; a panel that is ready or running
// (states 2 and 4) is scored, except the goal side panel, which only
// reports success.
bool msw::CStageHockey10::CPanelInfo::SetScored()
{
    if (GetState() != 3) {
        if (GetPanelBonusRate() < 0) {
            CGameSnd::Play(0xD5E, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        } else {
            CGameSnd::Play(0xD53, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    switch (GetState()) {
        case 2:
        case 4:
            if ((GetType() & 0xFFFF) != PANEL_GOAL) {
                SetState(3);
                CheckState();
            }
            break;
        default:
            return false;
    }
    return true;
}

// The model is scaled to the panel: twice the radius (kind 1 by its size
// tuning), a third of it for the goal side panel; the bonus panel keeps
// its scale.
void msw::CStageHockey10::CPanelInfo::CalcRootMtx()
{
    CStgSrt &rSrt = GetSrt();
    CStgMdl *pMdl = GetMdl();

    long kind = GetType() & 0xFFFF;
    if (kind == 1) {
        rSrt.SetScale(2.0f * GetParam_Radius());
    } else if (kind == PANEL_GOAL) {
        rSrt.SetScale(GetRadius() / 3.0f);
    } else if (kind != PANEL_BONUS) {
        rSrt.SetScale(2.0f * GetRadius());
    }

    if (pMdl != 0) {
        pMdl->SetMtx(rSrt.GetMtx());
        pMdl->Commit();
    }
}

// Half the size of the kind; the panels of kind 5 shrink from
// HOCKEY_PRM_S10_PANEL_SIZE_B towards HOCKEY_PRM_S10_PANEL_SIZE_END_B as
// their life runs out, kind 8 has the full size.
float msw::CStageHockey10::CPanelInfo::GetParam_Radius() const
{
    long type = GetType();
    float radius = GetParent()->GetPanelParam_Size(type);

    switch (GetType() & 0xFFFF) {
        case 5: {
            float rate = GetLife() / GetLifeMax();
            radius = 0.5f * (rate * (HOCKEY_PRM_S10_PANEL_SIZE_END_B - HOCKEY_PRM_S10_PANEL_SIZE_B) +
                             HOCKEY_PRM_S10_PANEL_SIZE_B);
            break;
        }
        case 8:
            break;
        default:
            radius *= 0.5f;
            break;
    }
    return radius;
}

bcs::FVEC3 msw::CStageHockey10::CTelopDispHockey::GetOffsetPos() const
{
    return bcs::FVEC3(0.0f, 2.0f, 0.0f);
}

float msw::CStageHockey10::CTelopDispHockey::GetDispFrame() const
{
    return 90.0f;
}

void msw::CStageHockey10::CTelopDispHockey::DoDisp(long nTeam, const bcs::FVEC3 *pPos, long nKind)
{
    CCockpitHockey::CreateObj(0x100D, 0);
    CCockpit::SetShootPtsIcon(nTeam, pPos, nKind);
}

int msw::CStageHockey10::Stage10_IsGimmickRunning() const
{
    return IsGimmickRunning();
}

long msw::CStageHockey10::GetParam_ArrangeOddsTblNum() const
{
    return 3;
}

const msw::CStage10Base::PANEL_ARRANGE_GROUP *msw::CStageHockey10::GetArrangeGroup(long nSet, long nGroup) const
{
    return &s_PanelArrangeSet[nSet][nGroup];
}

const msw::CStage10Base::ARRANGE_ODDS *msw::CStageHockey10::GetArrangeOddsTbl(long n) const
{
    return &s_ArrangeOddsTbl[n];
}

long msw::CStageHockey10::SE_GetSlotNum() const
{
    return 5;
}
