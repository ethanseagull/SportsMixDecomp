#include "game/msw/SelectMenu/CBgMdl.hpp"

#include "bcs/FVEC3.hpp"

// Most of this file is not in the module: the .rodata keeps the static locals
// of CBgMdl::Initialize, Set and ChangeRot, CSelectMenuBgMdlObj::Main and the
// CBgViewCamera constructor, whose functions the original link stripped
// while keeping their data. The unread tables, strings and constants are
// named below in the original's order (names and the members of the tables
// guessed) and kept by MenuF's force_active list in config.yml; nothing reads
// them. See notes/toolchain.md, "Unread constants of stripped code".

namespace {

    // A resource of the background files: its name and a number (the ball
    // of each game for the BG_ball_* models).
    struct ResEntry {
        const char *pName;
        unsigned char nId;
    };
}

namespace CBgMdl_cpp {
    extern const ResEntry UnusedMdlTable[6] = {
        { "bg_ball_rot", 0 },
        { "ball_bound", 0 },
        { "BG_ball_BK", 1 },
        { "BG_ball_VB", 2 },
        { "BG_ball_DB", 3 },
        { "BG_ball_HK", 4 },
    };
    extern const ResEntry UnusedAnmTable[3] = {
        { "bg_ball_rot", 0 },
        { "bg_ball_rot_4", 0 },
        { "ball_bound", 0 },
    };
}

namespace {

    // The node of the ball model whose motion each move plays. The names
    // of the objects in this file are guesses; the module has no symbols.
    const char *s_BallNodeName[] = {
        "ball_00",
        "ball_01",
        "ball_02",
        "ball_03",
        "ball_out",
        "ball_set"
    };

}

namespace CBgMdl_cpp {
    extern const unsigned char UnusedBallIdTable[5] = { 0, 2, 3, 4, 5 };
    extern const char UnusedCameraAnmName[] = "bg_ball_rot_cam00";
    extern const char UnusedCameraName[] = "Camera";
    extern const char UnusedLightName[] = "bg_ball_chr_lgt.lgt";
    // Initialize()'s static local resLen in the original.
    extern const long UnusedResLen = 5;
}

namespace {

    // (meaning guessed) One position of the ball models.
    struct BallPos {
        unsigned char m_nId[3];
        bcs::FVEC3 m_vPos;
        float m_fScale;
    };

    BallPos s_BallPos[4] = {
        { { 1, 2, 5 }, bcs::FVEC3(60.0f, 0.0f, -20.0f), 0.6f },
        { { 2, 3, 6 }, bcs::FVEC3(60.0f, 0.0f, -20.0f), 0.6f },
        { { 3, 4, 7 }, bcs::FVEC3(60.0f, 0.0f, -20.0f), 0.6f },
        { { 4, 5, 8 }, bcs::FVEC3(65.0f, 0.0f, -20.0f), 1.5f }
    };

    // (meaning guessed) A direction, probably of the light.
    bcs::FVEC3 s_LightDir(0.12f, -0.35000002f, -1.0f);
}

// The motion node is named by the state.
inline void msw::SelectMenu::CSelectMenuBgMdlObj::StartMove(float fFrames, long nState, unsigned char nFlag)
{
    m_fMoveTime = fFrames;
    m_fMoveTimer = fFrames;
    m_nState = nState;
    bcs::CMdlObj *pBall = static_cast<bcs::CMdlObj *>(bcs::CScnObj::GetScnObj(m_nBallHandle));
    if (pBall != 0) {
        bcs::CAnmMotion *pAnm = static_cast<bcs::CAnmMotion *>(pBall->GetAnmObjChr());
        if (pAnm != 0) {
            long nNode = pBall->GetNodeID(s_BallNodeName[nState]);
            pAnm->SetFrame(0.0f);
            float fFrame = pAnm->VFunc58(nNode);
            float fRate = 1.0f;
            if (m_fMoveTime >= 1.0f) {
                fRate = fFrame / m_fMoveTime;
            }
            pAnm->SetRate(fRate);
        }
    }
    m_vStartPos = m_vTrans;
    m_nFlag200 = nFlag;
}

// (guessed) Dead float comparisons, a stand-in for the stripped code that
// used these constants first: the original's literal pool has 60.0f and 1.0f
// ahead of the ones the functions below create (see notes/decisions.md, "Dead
// float comparison"). They generate no code.
inline void msw::SelectMenu::CBgMdl::UnusedReferences() const
{
    if (m_fMoveTime == 60.0f) {
    }
    if (m_fMoveTime == 1.0f) {
    }
}

void msw::SelectMenu::CBgMdl::ChangeBall(long nBall)
{
    UnusedReferences();
    switch (nBall) {
        case 0:
            SetSelectPosMdlObjId(1);
            break;
        case 1:
            SetSelectPosMdlObjId(2);
            break;
        case 2:
            SetSelectPosMdlObjId(3);
            break;
        case 3:
            SetSelectPosMdlObjId(4);
            break;
    }
}

namespace CBgMdl_cpp {
    extern const float UnusedFloat0 = 10000.0f;
}

void msw::SelectMenu::CBgMdl::SetSelectPosMdlObjId(long nId)
{
    CSelectMenuBgMdlObj *pOut;
    unsigned char nOldId = m_nPosMdlObjId[0];
    if (nOldId == nId) {
        return;
    }

    float fMoveTime = m_fMoveTime;
    for (int i = 1; i < 4; i++) {
        unsigned char nNewId = m_nPosMdlObjId[i];
        if (nNewId == nId) {
            m_nPosMdlObjId[i] = nOldId;
            pOut = m_pBallMdl[nOldId];
            if (pOut != 0) {
                pOut->StartMove(fMoveTime, 4, 0);
            }
            m_nPosMdlObjId[0] = nNewId;
            CSelectMenuBgMdlObj *pIn = m_pBallMdl[nNewId];
            if (pIn != 0) {
                pIn->StartMove(fMoveTime, 5, 2);
            }
            return;
        }
    }
}

// The rest of the stripped code's constants: they follow the functions'
// literals in the original. The ones named after static locals of the
// original are noted.
namespace CBgMdl_cpp {
    extern const float UnusedFloat1 = 16.0f / 9.0f;
    extern const float UnusedFloat2 = 4.0f / 3.0f;
    // Set()'s static locals viewNo and frame.
    extern const long UnusedViewNo = 0;
    extern const long UnusedFrame = 0;
    extern const float UnusedFloat3[3] = { 0.017453292f, 35.0f, 0.1f };
    // ChangeRot()'s static local speed.
    extern const float UnusedSpeed = 1.0f;
    // CSelectMenuBgMdlObj::Main()'s static local rotYSpeedDiv, then
    // constants of the same function.
    extern const float UnusedRotYSpeedDiv = 15.0f;
    extern const float UnusedFloat4[4] = { 2.0625f, 0.0f, 15.0f, 6.2831855f };
    // CBgViewCamera's constructor's static locals far, near, fovy and roll.
    extern const float UnusedFar = 10000.0f;
    extern const float UnusedNear = 0.1f;
    extern const float UnusedFovy = 35.0f;
    extern const float UnusedRoll = 0.0f;
}

// (guessed) CSelectMenuBgMdlObj's destructor stands in for its stripped
// functions: with it this file emits the class's vtable, and with that the
// weak copies of the inline bcs getters the original keeps here (the link
// strips the vtable and the destructor again).
msw::SelectMenu::CSelectMenuBgMdlObj::~CSelectMenuBgMdlObj()
{
}
