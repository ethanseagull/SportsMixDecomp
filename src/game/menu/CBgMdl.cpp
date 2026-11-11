#include "game/msw/SelectMenu/CBgMdl.hpp"

#include "bcs/FVEC3.hpp"

// Most of this file is not in the module: the .rodata keeps the static locals
// of CBgMdl::Initialize, Set and ChangeRot, CSelectMenuBgMdlObj::Main and the
// CBgViewCamera constructor, whose functions the original link stripped.
// Only the two functions below, the weak getters of the bcs model classes
// (emitted with a vtable that was stripped too) and the static initialiser
// remain.

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
    bcs::FVEC3 s_LightDir(0.12f, -0.35f, -1.0f);
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

void msw::SelectMenu::CBgMdl::ChangeBall(long nBall)
{
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
