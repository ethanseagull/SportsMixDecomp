// msw::CChrCoreHockeyDiddy: Diddy Kong in the hockey game. His shot trick
// shows his arts model (a boomerang in hand) and throws a boomerang
// (CC08_HOCKEY::CcBoomerangHockey) from it.
//
// msw::CC08_HOCKEY::CcBoomerangHockey: the boomerang. The base class
// (msw::CC_CMN::DIDDY::CcBoomerang, in main.dol) does the work; this class
// gives the tuning values and the trailing effect.
#include "game/msw/CChrCoreHockey.hpp"

#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcBoomerang.hpp"

namespace msw {
    namespace CC08_HOCKEY {

        class CcBoomerangHockey : public CC_CMN::DIDDY::CcBoomerang {
            public:
                CcBoomerangHockey(bcs::CTask *pParent, long nPlayer, long nTeam)
                    : CcBoomerang(pParent, nPlayer, nTeam)
                {
                }
                virtual ~CcBoomerangHockey() {}

                virtual void Create(nw4r::g3d::ResFile res, const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo,
                                    const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, float fRotY);

            protected:
                virtual void SetParam_();
        };
    }
}

void msw::CC08_HOCKEY::CcBoomerangHockey::Create(nw4r::g3d::ResFile res, const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo,
                                                 const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, float fRotY)
{
    CcBoomerang::Create(res, pInfo, rPos, rRot, fRotY);
    SetTailEffect_(0x5D1);
}

// (values' meanings not known)
void msw::CC08_HOCKEY::CcBoomerangHockey::SetParam_()
{
    m_afParam[0] = 0.16666667f;
    m_afParam[1] = 5.0f;
    m_afParam[2] = 2.8f;
    m_afParam[3] = 0.0f;
    m_afParam[4] = 0.029088821f;
    m_afParam[5] = 0.07853982f;
    m_afParam[6] = 0.029088821f;
    m_afParam[7] = 16.0f;
    m_afParam[8] = 1.2f;
    m_afParam[9] = 0.1f;
    m_afParam[10] = 0.42f;
    m_afParam[11] = 180.0f;
    m_fUnkCC = 180.0f;
}

msw::CChrCoreHockeyDiddy::CChrCoreHockeyDiddy()
{
    m_hBullet = 0;
}

msw::CChrCoreHockeyDiddy::~CChrCoreHockeyDiddy()
{
}

void msw::CChrCoreHockeyDiddy::Reset_()
{
    if (m_hBullet != 0) {
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hBullet);
        if (pTask) {
            pTask->Delete();
        }
        m_hBullet = 0;
    }
    CChrCoreHockey::Reset_();
}

// Step 0 waits for the trick motion, plays the throw (0x9D) and brings out
// the boomerang model; in step 1 the boomerang leaves at the end of the
// throw or at its frame 70; step 2 waits for the throw to end.
int msw::CChrCoreHockeyDiddy::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xA3, 3);
                PlaySePos(0x436, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                ArtsOptCreate(1, 0, 1);
                ArtsOptFadeIn(5.0f, 32.0f, 1);
                pWork->m_fTrickMotFrameNum = AnimGetFrameNum(3);
                SubStateArtsShotSetTrickJumpParam_(pWork, 1);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 70.0f) {
                SubStateArtsShotFireBullet_(pWork, 0);
                pWork->m_nTrickStep++;
            }
            break;
        case 2:
            if (IsAnimEnd(3)) {
                pWork->m_nTrickStep++;
                bReady = 1;
            }
            break;
        default:
            bReady = 1;
            break;
    }
    return bReady;
}

void msw::CChrCoreHockeyDiddy::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);
    SE_PlayArtsJumpSE(0);
}

void msw::CChrCoreHockeyDiddy::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(pWork);
}

void msw::CChrCoreHockeyDiddy::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    Eff_Stop(0x15);
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x16, 0xA4, 3);
}

void msw::CChrCoreHockeyDiddy::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    ThrowBanana_();
    m_hBullet = 0;
    ArtsOptDestroy();
}

// The boomerang leaves the model in his hand (or his feet, without one),
// flying out 80 degrees to his right, and the model goes.
void msw::CChrCoreHockeyDiddy::ThrowBanana_()
{
    // (member names guessed) Only the node, the angles and the full turn
    // are read; the rest is unused here.
    struct SP_BULLET_NODE {
        long nNode;         // the model node the boomerang leaves from
        float fRotYOfs;     // its heading from his own
        float fRound;       // a full turn
        float fRotOfs;      // its model's turn about Y
        float afUnk10[5];
        long nUnk24;
    };
    static const SP_BULLET_NODE _SP_BULLET_NODE = {
        1, -1.3962634f, 6.2831855f, 3.1415927f, { 0.5f, -1.0f, 90.0f, 100.0f, 6.0f }, 0
    };

    if (m_hBullet != 0) {
        return;
    }
    const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo =
        CC_DATA::GetArtsOptionMdl_CreateInfo(GetChrIdS(), CGame::g_GameWork.m_nGameMode);
    if (!pInfo) {
        return;
    }
    nw4r::g3d::ResFile res = GetChrOptResFile();
    if (!res.IsValid()) {
        return;
    }

    bcs::FVEC3 pos(GetPos());
    bcs::FVEC3 rot(GetRot());
    float fRotY = GetRotY() + _SP_BULLET_NODE.fRotYOfs;
    while (fRotY > _SP_BULLET_NODE.fRound) {
        fRotY -= _SP_BULLET_NODE.fRound;
    }
    while (fRotY < 0.0f) {
        fRotY += _SP_BULLET_NODE.fRound;
    }
    float fRot = _SP_BULLET_NODE.fRotOfs + rot.y;
    while (fRot > _SP_BULLET_NODE.fRound) {
        fRot -= _SP_BULLET_NODE.fRound;
    }
    while (fRot < 0.0f) {
        fRot += _SP_BULLET_NODE.fRound;
    }
    rot.y = fRot;

    bcs::CMdlObj *pMdl = GetArtsOptMdl();
    if (pMdl) {
        bcs::FMTX34 mtx;
        pMdl->GetNodeWorldMtx(_SP_BULLET_NODE.nNode, mtx);
        bcs::FVEC3 hand(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
        pos = hand;
    }

    CC08_HOCKEY::CcBoomerangHockey *pBoomerang =
        new (0x100) CC08_HOCKEY::CcBoomerangHockey(CGame::GetTempParent(), GetPlayerNo(), GetTeamNo());
    if (pBoomerang) {
        pBoomerang->Create(res, pInfo, pos, rot, fRotY);
    }
    ArtsOptDestroy();
    PlaySePos(0x438, 0, 0, -1, 0.0f, 1.0f, 0.0f);
}
