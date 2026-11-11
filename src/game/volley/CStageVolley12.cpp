// msw::CStageVolley12: volley stage 12, the pinball stage. A launcher
// ("wl_meka") shoots balls across the court (CPinballManVolley); bumpers and
// exit holes play an animation when a ball hits them. HockeyF's
// CStageHockey12 and DodgeF's CStageDodge12 are the same stage in the
// other modes.
#include "game/msw/CStageVolley12.hpp"

#include "bcs/CRegex.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CPinballManVolley.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/CStgGmk_Pinball.hpp"
#include "game/msw/VolleyPrm.hpp"
#include <stdio.h>

namespace {

    // (names guessed) One entry of the parameter table GetPrmTbl gives: the
    // type of a tuning value, the value, and the offset of the word it
    // fills. The table ends with a zero entry.
    enum PRM_TYPE {
        PRM_TYPE_END,
        PRM_TYPE_LONG,
        PRM_TYPE_FLOAT
    };

    struct PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    const PRM_TBL s_PrmTbl[] = {
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_SIZE, 0x00 },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_COLLSIZE, 0x04 },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_SPEED, 0x08 },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_SPEED_LIMIT, 0x0C },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_KICK_POW, 0x10 },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_BALL_KICK_POW, 0x14 },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_DECEL, 0x18 },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_GUIDE_DECAY, 0x1C },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_FORCE_ACC, 0x20 },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_INIT_SLEEP, 0x24 },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_INTERVAL, 0x28 },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_ANGLE, 0x2C },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_SIDE_RATE, 0x30 },
        { PRM_TYPE_LONG, &msw::VOLLEY_PRM_S12_BALL_NUM, 0x34 },
        { PRM_TYPE_FLOAT, &msw::VOLLEY_PRM_S12_BALL_DOWN_TIME, 0x3C },
        { PRM_TYPE_END, 0, 0 },
    };

    // The ids of the model generation rules.
    enum {
        MDL_BUMPER = 1,
        MDL_EXIT = 2,
        MDL_LAUNCHER = 100
    };

    // Animations of a bumper or hole model.
    enum {
        ANM_HIT = 1,
        ANM_REST = 2
    };

    // SendMessage: a ball hit a bumper (arg2: its number) or a hole.
    enum {
        MSG_PINBALL = 0x25,
        MSG_PINBALL_BUMPER = 3,
        MSG_PINBALL_EXIT = 4
    };

    enum {
        SE_STAGE_ENV = 0xD6B
    };
}

const void *msw::CStageVolley12::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageVolley12::CStageVolley12(bcs::CTask *pParent) : CStageManVolley(pParent), m_pPinball(0)
{
    for (int i = 0; i < BUMPER_NUM; i++) {
        m_apBumper[i] = 0;
    }
    for (int i = 0; i < EXIT_NUM; i++) {
        m_apExit[i] = 0;
    }
    m_pPinball = new (1) CPinballManVolley();
}

msw::CStageVolley12::~CStageVolley12()
{
    if (m_pPinball != 0) {
        delete m_pPinball;
    }
    m_pPinball = 0;
}

void msw::CStageVolley12::Main_(float frame)
{
    if (m_pPinball != 0 && IsGimmickRunning()) {
        m_pPinball->Main(frame);
    }
    if (m_pPinball != 0) {
        m_pPinball->MechaMain(frame);
    }
    CStageMan::Main_(frame);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageVolley12::GetMdlGenRule_() const
{
    // The end entry chains to CStageMan's table (not CStageManVolley's).
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_banper_0([0-9]+)", MDL_BUMPER, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_ana_0([0-9]+)", MDL_EXIT, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_wl_meka$", MDL_LAUNCHER, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageMan::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageVolley12::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                           CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                           long n)
{
    switch (pRule->m_nId) {
        case MDL_BUMPER: {
            long no;
            if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0) {
                // The names count from 1.
                no--;
                if (no >= 0 && no < BUMPER_NUM) {
                    m_apBumper[no] = pNode;
                }
            }
            break;
        }
        case MDL_EXIT: {
            long no;
            if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0) {
                no--;
                if (no >= 0 && no < EXIT_NUM) {
                    m_apExit[no] = pNode;
                }
            }
            break;
        }
        case MDL_LAUNCHER:
            m_pPinball->SetMechaMdl(pNode);
            break;
    }

    CStageManVolley::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageVolley12::OnSceneEntry()
{
    CPinballManVolley *pPinball = m_pPinball;
    pPinball->Init();
    pPinball->SetDefaultPos(bcs::FVEC3(9.0f, 0.0f, 0.0f));
    pPinball->CreateGmk(this, 0);
}

void msw::CStageVolley12::OnSceneReentry()
{
    BumperReset();
    ExitReset();
}

void msw::CStageVolley12::OnEntranceEventGmkStart()
{
    if (m_pPinball != 0) {
        m_pPinball->MechaEvent00();
    }
}

void msw::CStageVolley12::OnEntranceEventGmkEnd()
{
    if (m_pPinball != 0) {
        m_pPinball->Reset();
    }
}

void msw::CStageVolley12::OnSoundEvent(long event)
{
    if (event == 0) {
        if (!SE_IsAlive(1)) {
            SE_RepeatPlay(1, SE_STAGE_ENV, 180.0f, 420.0f, -1.0f, 0.0f, 12);
        }
    }
}

// (name guessed) The animations of a model node, or null.
static inline msw::CStageNodeAnm *GetNodeAnm(msw::CStageMdlListNode *pNode)
{
    return pNode != 0 ? pNode->GetAnm() : 0;
}

void msw::CStageVolley12::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    // The bumpers are numbered in another order in the messages.
    const long bumperOrder[BUMPER_NUM] = { 1, 0, 3, 2 };

    if (msg == MSG_PINBALL) {
        switch (arg1) {
            case MSG_PINBALL_BUMPER:
                if (arg2 < BUMPER_NUM) {
                    CStageNodeAnm *pAnm = GetNodeAnm(m_apBumper[bumperOrder[arg2]]);
                    if (pAnm != 0) {
                        pAnm->Play(ANM_HIT, 0, 1.0f, 0.0f, 0.0f, -1);
                    }
                }
                break;
            case MSG_PINBALL_EXIT: {
                CStageNodeAnm *pAnm = GetNodeAnm(m_apExit[arg2]);
                if (pAnm != 0) {
                    pAnm->Play(ANM_HIT, 0, 1.0f, 0.0f, 0.0f, -1);
                }
                break;
            }
        }
    }
    CStageManVolley::SendMessage(msg, arg1, arg2, arg3);
}

void msw::CStageVolley12::BumperReset()
{
    for (int i = 0; i < BUMPER_NUM; i++) {
        CStageNodeAnm *pAnm = GetNodeAnm(m_apBumper[i]);
        if (pAnm != 0) {
            pAnm->Play(ANM_REST, 0, 1.0f, 0.0f, 0.0f, -1);
        }
    }
}

void msw::CStageVolley12::ExitReset()
{
    for (int i = 0; i < EXIT_NUM; i++) {
        CStageNodeAnm *pAnm = GetNodeAnm(m_apExit[i]);
        if (pAnm != 0) {
            pAnm->Play(ANM_REST, 0, 1.0f, 0.0f, 0.0f, -1);
        }
    }
}

void msw::CStageVolley12::OnGimmickReset()
{
    if (m_pPinball != 0) {
        m_pPinball->InactivateAll();
        m_pPinball->Reset();
    }
    BumperReset();
    ExitReset();
}

void msw::CStageVolley12::OnGimmickStop()
{
    if (m_pPinball != 0) {
        m_pPinball->InactivateAll();
        m_pPinball->Reset();
    }
}

void msw::CStageVolley12::OnGimmickStart()
{
}

void msw::CStageVolley12::OnGimmickPause()
{
}

void msw::CStageVolley12::OnGimmickResume()
{
}

void msw::CStageVolley12::OnGimmickCancel()
{
    if (m_pPinball != 0) {
        m_pPinball->InactivateAll();
        m_pPinball->Reset();
    }
}

long msw::CStageVolley12::GetPinballMaxNum()
{
    CStageVolley12 *pStage = static_cast<CStageVolley12 *>(CStageMan::GetInstance());
    if (pStage->m_pPinball == 0) {
        return 0;
    }
    return pStage->m_pPinball->GetMaxNum();
}

msw::CStgGmk_Pinball *msw::CStageVolley12::GetPinball(long idx)
{
    CStageVolley12 *pStage = static_cast<CStageVolley12 *>(CStageMan::GetInstance());
    if (pStage->m_pPinball == 0) {
        return 0;
    }
    if (idx < 0 || idx >= pStage->m_pPinball->GetMaxNum()) {
        return 0;
    }

    CStgGmk_Pinball *pBall = pStage->m_pPinball->GetGmk(idx);
    if (pBall == 0 || !pBall->IsActive()) {
        return 0;
    }
    return pBall;
}
